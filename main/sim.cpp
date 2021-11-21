#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "sim_proc.h"
#include "OO_Structures.h"

uint_fast32_t uid, clk;

const size_t ARF_SIZE = 67;

std::vector<rob_line> rob;
std::vector<arf_line> arf;
std::vector<rmt_line> rmt;
std::vector<iq_line> iq;

void run_simulation(std::vector<instruction> &instrs, uint_fast16_t width, uint_fast16_t iq_max);


void print_report(const char *trace_file, const proc_params &params, size_t printchars,
                  std::vector<instruction> &instr);

int main(int argc, char *argv[]) {
   FILE *FP;               // File handler
   char *trace_file;       // Variable that holds trace file name;
   proc_params params;       // look at sim_bp.h header file for the the definition of struct proc_params
   int op_type, dest, src1, src2;  // Variables are read from trace file
   unsigned long int pc; // Variable holds the pc read from input file

   if (argc < 5) {
      printf("Error: Wrong number of inputs:%d\n", argc - 1);
      exit(EXIT_FAILURE);
   }


   params.rob_size = strtoul(argv[1], NULL, 10);
   params.iq_size = strtoul(argv[2], NULL, 10);
   params.width = strtoul(argv[3], NULL, 10);
   size_t printchars = 0;
   if (argc > 5) printchars = strtoul(argv[5], NULL, 10);
   trace_file = argv[4];
   size_t i;

   for (i = 0; i < params.rob_size; ++i) {
      rob.emplace_back(rob_line(i));
   }

   for (i = 0; i < ARF_SIZE; ++i) {
      rmt.emplace_back(rmt_line());
   }

   uid = clk = 0;

   // Open trace_file in read mode
   FP = fopen(trace_file, "r");
   if (FP == NULL) {
      // Throw error and exit if fopen() failed
      printf("Error: Unable to open file %s\n", trace_file);
      exit(EXIT_FAILURE);
   }

   std::vector<instruction> instr;

   while (fscanf(FP, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) != EOF) {
      instr.emplace_back(instruction(uid, pc, op_type, dest, src1, src2));
      ++uid;
   }
   fclose(FP);

   run_simulation(instr, params.width, params.iq_size);

   print_report(trace_file, params, printchars, instr);

   return EXIT_SUCCESS;
}


void run_simulation(std::vector<instruction> &instrs, uint_fast16_t width, uint_fast16_t iq_max) {
   size_t head_rob, tail_rob, count;
   head_rob = tail_rob = 0;
   std::vector<instruction *> ex_wb, is_ex, di_is, rr_di, rn_rr, de_rn, fe_de;

   uint_fast16_t j;
   uint_fast16_t rob_avail = rob.size();
   uint_fast16_t iss_avail = iq_max;

   uint_fast32_t last_fetched = 0;

   while (true) {
      //RETIRE
      for (j = 0; j < width; ++j) {
         if (rob_avail == rob.size()) break;
         if (rob[head_rob].ready && instrs[rob[head_rob].index].wb_beg > 0) {
            instrs[rob[head_rob].index].rt_dur = clk - instrs[rob[head_rob].index].rt_beg + 1;
            for (rmt_line &l: rmt) {
               if (l.tag == (int_fast32_t) head_rob) l.valid = false;
            }

            ++head_rob;
            ++rob_avail;
            if (head_rob == rob.size()) head_rob = 0;
         }
      }

      // WRITEBACK
      for (j = 0; j < width; ++j) {
         for (instruction *i: ex_wb) {
            i->wb_beg = clk;
            i->wb_dur = 1;
            i->rt_beg = clk + 1;
            if (i->rob_dest >= 0) {
               rob[i->rob_dest].ready = true;
            } else {
               i->rt_dur = 1;
            }
         }
         ex_wb.clear();
      }


      //EXECUTE
      for (instruction *i: is_ex) {
         if (i->ex_beg + i->ex_dur == clk + 1) {
            i->complete = true;
            ex_wb.emplace_back(i);
         }
      }
      // Wakeup other instrs
      for (instruction *i: is_ex) {
         if (i->complete) {
            for (instruction *isq: di_is) {
               if (isq->r1_renamed && isq->r1b == i->rob_dest) isq->r1_ready = true;
               if (isq->r2_renamed && isq->r2b == i->rob_dest) isq->r2_ready = true;
            }
            for (instruction *isq: rr_di) {
               if (isq->r1_renamed && isq->r1b == i->rob_dest) isq->r1_ready = true;
               if (isq->r2_renamed && isq->r2b == i->rob_dest) isq->r2_ready = true;
            }
            for (instruction *isq: rn_rr) {
               if (isq->r1_renamed && isq->r1b == i->rob_dest) isq->r1_ready = true;
               if (isq->r2_renamed && isq->r2b == i->rob_dest) isq->r2_ready = true;
            }
            if (i->rob_dest >= 0) {
               rob[i->rob_dest].ready = true;
            }
         }
      }
      is_ex.erase(std::remove_if(is_ex.begin(), is_ex.end(), [](instruction *in) { return in->complete; }),
                  is_ex.end());

      //ISSUE
      count = 0;
      for (j = 0; j < width && count < width; ++j) {
         for (instruction *i: di_is) {
            if (i->is_beg == 0) {
               i->is_beg = clk;
               i->di_dur = clk - i->di_beg;
            }
            if (i->loaded && i->r1_ready && i->r2_ready && count < width) {
               i->ex_beg = clk + 1;
               i->is_dur = clk - i->is_beg + 1;
               i->loaded = false;
               is_ex.emplace_back(i);
               count++;
               iss_avail++;
            }
         }
      }
      di_is.erase(std::remove_if(di_is.begin(), di_is.end(), [](instruction *in) { return !in->loaded; }), di_is.end());



      //DISPATCH
      if (iss_avail >= rr_di.size() && rr_di.size() > 0) {
         for (j = 0; j < width; ++j) {
            di_is.emplace_back(rr_di[j]);
            iss_avail--;
         }
         rr_di.clear();
      }


      //REG READ
      if (rr_di.empty() && !rn_rr.empty()) {
         for (j = 0; j < rn_rr.size(); ++j) {
            // If both source operands ready
            rr_di.emplace_back(rn_rr[j]);
            rn_rr[j]->loaded = true;
            rr_di[j]->di_beg = clk + 1;
            rr_di[j]->rr_dur = clk + 1 - rr_di[j]->rr_beg;
         }
         rn_rr.erase(std::remove_if(rn_rr.begin(), rn_rr.end(), [](instruction *in) { return in->loaded; }),
                     rn_rr.end());
      }


      //RENAME
      if (de_rn.size() <= rob_avail && !de_rn.empty() && rn_rr.empty()) {
         for (j = 0; j < de_rn.size(); ++j) {
            // emplace instr into rob
            rob[tail_rob].ready = false;
            rob[tail_rob].pc = de_rn[j]->pc;
            rob[tail_rob].arf_dest = de_rn[j]->dest;
            rob[tail_rob].index = de_rn[j]->uid;

            // rename the registers
            if (de_rn[j]->r1 != -1 && rmt[de_rn[j]->r1].valid) {
               de_rn[j]->r1_renamed = true;
               de_rn[j]->r1b = rmt[de_rn[j]->r1].tag;
               de_rn[j]->r1_ready = rob[de_rn[j]->r1b].ready;
            } else {
               de_rn[j]->r1_renamed = false;
               de_rn[j]->r1b = de_rn[j]->r1;
               de_rn[j]->r1_ready = true;
            }
            if (de_rn[j]->r2 != -1 && rmt[de_rn[j]->r2].valid) {
               de_rn[j]->r2_renamed = true;
               de_rn[j]->r2b = rmt[de_rn[j]->r2].tag;
               de_rn[j]->r2_ready = rob[de_rn[j]->r2b].ready;
            } else {
               de_rn[j]->r2_renamed = false;
               de_rn[j]->r2b = de_rn[j]->r2;
               de_rn[j]->r2_ready = true;
            }
            // update the rmt
            if (de_rn[j]->dest != -1) {
               rmt[rob[tail_rob].arf_dest].tag = tail_rob;
               rmt[rob[tail_rob].arf_dest].valid = true;

            }

            de_rn[j]->rob_dest = tail_rob;
            // move rob pointer
            tail_rob++;
            rob_avail--;
            if (tail_rob == rob.size()) tail_rob = 0;
            // Advance instruction into next stage
            rn_rr.emplace_back(de_rn[j]);
            rn_rr[j]->rr_beg = clk + 1;
            rn_rr[j]->rn_dur = clk + 1 - rn_rr[j]->rn_beg;
         }
         de_rn.clear();
      }

      //DECODE
      if (!fe_de.empty() && de_rn.empty()) {
         for (j = 0; j < fe_de.size(); ++j) {
            de_rn.emplace_back(fe_de[j]);
            de_rn[j]->rn_beg = clk + 1;
            de_rn[j]->de_dur = clk + 1 - de_rn[j]->de_beg;
         }
         fe_de.clear();
      }

      //FETCH
      if (fe_de.empty()) {
         for (j = 0; j < width && last_fetched < instrs.size(); ++j) {
            fe_de.emplace_back(&instrs[last_fetched]);
            instrs[last_fetched].fe_beg = clk;
            fe_de[j]->de_beg = clk + 1;
            fe_de[j]->fe_dur = clk + 1 - fe_de[j]->fe_beg;
            ++last_fetched;
         }
      }

      // ADVANCE CLOCK CYCLE AND EXIT CONDITION : More than 3 clock cycles elapsed, Rob is empty
      // all instructions retired, and fetch is empty, so no new instructions to handle
      ++clk;
      if (clk > 3 && rob_avail == rob.size() && fe_de.empty()) break;
   }
}

void print_report(const char *trace_file, const proc_params &params, size_t printchars,
                  std::vector<instruction> &instr) {
   bool trunc;
   printchars == 0 ? trunc = false : trunc = true;

   for (instruction &i: instr) {
      if (trunc && --printchars == 0) break;
      std::cout << i.to_s();
   }

   uint_fast32_t num_cycle = instr.back().rt_beg + instr.back().rt_dur;

   printf("# === Simulator Command =========\n"
          "# ./sim %lu %lu %lu %s\n"
          "# === Processor Configuration ===\n"
          "# ROB_SIZE = %lu\n"
          "# IQ_SIZE  = %lu\n"
          "# WIDTH    = %lu\n"
          "# === Simulation Results ========\n"
          "# Dynamic Instruction Count    = %lu\n"
          "# Cycles                       = %lu\n"
          "# Instructions Per Cycle (IPC) = %.2f\n", params.rob_size, params.iq_size, params.width, trace_file,
          params.rob_size, params.iq_size, params.width, instr.size(), num_cycle,
          (double) ((double) instr.size() / (double) num_cycle));
}
