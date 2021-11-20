#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "sim_proc.h"
#include "OO_Structures.h"
/*
   typedef struct rob_point{
      size_t index;
      void operator=(uint_fast16_t inc){
         index=inc;
         if(index == rob_)
      }
} rob_pointer;*/

   uint_fast32_t uid, clk, instr_count;

   size_t head_rob, tail_rob, head_iq, tail_iq;

   const size_t ARF_SIZE = 67;

   std::vector<rob_line> rob;
   std::vector<arf_line> arf;
   std::vector<rmt_line> rmt;
   std::vector<iq_line> iq;

   void run_simulation(std::vector<instruction> &instrs, uint_fast16_t width, uint_fast16_t iq_max);


int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    proc_params params;       // look at sim_bp.h header file for the the definition of struct proc_params
    int op_type, dest, src1, src2;  // Variables are read from trace file
    unsigned long int pc; // Variable holds the pc read from input file
    
    if (argc != 5)
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.rob_size     = strtoul(argv[1], NULL, 10);
    params.iq_size      = strtoul(argv[2], NULL, 10);
    params.width        = strtoul(argv[3], NULL, 10);
    trace_file          = argv[4];
    /*printf("rob_size:%lu "
            "iq_size:%lu "
            "width:%lu "
            "tracefile:%s\n", params.rob_size, params.iq_size, params.width, trace_file);*/
   size_t i;

    for(i=0;i<params.rob_size;++i){
       rob.emplace_back(rob_line(i));
    }
    head_rob=tail_rob=0;
/*
   for(i=0;i<params.iq_size;++i){
      iq.emplace_back(iq_line());
   }
*/
   for(i=0;i<ARF_SIZE;++i){
      rmt.emplace_back(rmt_line());
   }

   uid= clk= instr_count=0;
   head_rob=tail_rob=0;


   /////// DEBUG POINTERS
   /*std::vector<rob_line> *dbg_z_rb = &rob;
   std::vector<rmt_line> *dbg_z_rt = &rmt;
   std::vector<iq_line> *dbg_z_isq = &iq;
   uint_fast32_t *dbg_uid=&uid, *dbg_clk=&clk, *dbg_instr_count=&instr_count;
   size_t *dbg_head_rob=&head_rob, *dbg_tail_rob=&tail_rob;*/

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    std::vector<instruction> instr;

    while(fscanf(FP, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) != EOF)
    {
         //    printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); //Print to check if inputs have been read correctly
        instr.emplace_back(instruction(uid, pc, op_type, dest, src1, src2));
        ++uid;
    }
    fclose(FP);

   run_simulation(instr, params.width, params.iq_size);

    for(instruction &i : instr) {
       std::cout << i.to_s();
    }


    return EXIT_SUCCESS;
}

void run_simulation(std::vector<instruction> &instrs, uint_fast16_t width, uint_fast16_t iq_max){
   bool done = false;
   /*bool rob_full = false;
   bool ra1, ra2;
   int_fast32_t ra1t, ra2t;*/
	std::vector<instruction *> exwb, isex, diis, rrdi, rnrr, dern, fede;

   uint_fast16_t j;
   uint_fast16_t rob_avail = rob.size();
   uint_fast16_t iss_avail = iq_max;

   uint_fast32_t last_fetched = 0;

   while(!done){

      //RETIRE
      for(j=0; j < width; ++j){
         if(rob_avail==rob.size()) break;
         if(rob[head_rob].ready){
            instrs[rob[head_rob].index].rt_beg = clk;
            ++head_rob;
            ++rob_avail;
            if(head_rob == rob.size()) head_rob = 0;
         }
      }

      // WRITEBACK
      for(j=0; j < width; ++j){
         for(instruction * i : exwb){
            i->wb_beg=clk;
            rob[i->rob_dest].ready;

         }
         exwb.clear();
      }


      //EXECUTE
      for(instruction *i: isex){
         if(i->ex_beg + i->ex_dur == clk) {
            i->complete=true;
            exwb.emplace_back(i);
         }
      }
      // Wakeup other instrs
      for(instruction *i: isex){
         if(i->complete) {
            for(instruction *isq : diis) {
               if(isq->r1_renamed && isq->r1b == i->rob_dest) isq->r1_ready=true;
               if(isq->r2_renamed && isq->r1b == i->rob_dest) isq->r2_ready=true;
            }
            for(instruction *isq : rrdi) {
               if(isq->r1_renamed && isq->r1b == i->rob_dest) isq->r1_ready=true;
               if(isq->r2_renamed && isq->r1b == i->rob_dest) isq->r2_ready=true;
            }
            for(instruction *isq : rnrr) {
               if(isq->r1_renamed && isq->r1b == i->rob_dest) isq->r1_ready=true;
               if(isq->r2_renamed && isq->r1b == i->rob_dest) isq->r2_ready=true;
            }
         }
      }
      isex.erase(std::remove_if(isex.begin(), isex.end(), [](instruction *in){return in->complete;}), isex.end());



      //ISSUE
      for(j=0; j < width; ++j){
         for(instruction *i : diis) {
            if(i->loaded && i->r1_ready && i->r2_ready) {
               i->is_beg=clk;
               i->ex_beg=clk+1;
               i->loaded=false;
               isex.emplace_back(i);
            }
         }
      }
      diis.erase(std::remove_if(diis.begin(), diis.end(), [](instruction *in){return !in->loaded;}), diis.end());



      //DISPATCH
      if(iss_avail >= rrdi.size() && rrdi.size()>0) {
         for (j = 0; j < width; ++j) {
            rrdi[j]->di_beg=clk;
            diis.emplace_back(rrdi[j]);
            iss_avail--;

            /*
            if(rrdi[j]->r1_renamed && rob[rrdi[j]->r1b].ready) {
               ra1 = true;
            } else if (rrdi[j]->r1_renamed && !rob[rrdi[j]->r1b].ready) {
               ra1 = false;
            } else {
               ra1=true;
            }
            ra1t = rrdi[j]->r1b;

            if(rrdi[j]->r2_renamed && rob[rrdi[j]->r1b].ready) {
               ra2 = true;
            } else if (rrdi[j]->r2_renamed && !rob[rrdi[j]->r1b].ready) {
               ra2 = false;
            } else {
               ra2=true;
            }
            ra2t = rrdi[j]->r2b;
            iq.emplace_back(iq_line( ra1, ra1t, ra2, ra2t, rrdi[j]->dest, rrdi[j]->pc));*/

         }
         rrdi.clear();
      }


      //REG READ
      if(rrdi.empty()) {
         for (j = 0; j < rnrr.size(); ++j) {
            rnrr[j]->rr_beg=clk;

            // If both source operands ready
            //if(((rnrr[j]->r1_renamed && rob[rnrr[j]->r1].ready)||!rnrr[j]->r1_renamed) &&
            //((rnrr[j]->r2_renamed && rob[rnrr[j]->r2].ready)||!rnrr[j]->r2_renamed)) {
               rrdi.emplace_back(rnrr[j]);
               rnrr[j]->loaded = true;

            //}

         }
         rnrr.erase(std::remove_if(rnrr.begin(), rnrr.end(), [](instruction *in){return in->loaded;}), rnrr.end());
      }


      /////////////////////////////// TODO WHEN PICKING BACK UP
      //////////// THE RMT IS NOT BEING UPDATED WHICH IS CAUSING A DEADLOCK.
      /////////// IN BELOW FUNCITONALITY WE ARE PULLING RMT ON ALWAYS_VALID MOST LIKELY

      //RENAME
      if(dern.size() <= rob_avail && !dern.empty() && rnrr.empty()) {

            for (j = 0; j < dern.size(); ++j) {
               dern[j]->rn_beg=clk;

               // move rob pointer
               tail_rob++;
               rob_avail--;
               if(tail_rob==rob.size()) tail_rob=0;

               // emplace instr into rob
               rob[tail_rob].ready=false;
               rob[tail_rob].pc=dern[j]->pc;
               rob[tail_rob].arf_dest = dern[j]->dest;


               // update the rmt
               if(rob[tail_rob].arf_dest != -1) {
                  rmt[rob[tail_rob].arf_dest].tag = tail_rob;
                  rmt[rob[tail_rob].arf_dest].valid = true;
               }

               // rename the registers
               if(dern[j]->r1 != -1 && rmt[dern[j]->r1].valid) {
                  dern[j]->r1_renamed=true;
                  dern[j]->r1b=rmt[dern[j]->r1].tag;
                  dern[j]->r1_ready = rob[dern[j]->r1b].ready;
               } else
               {
                  dern[j]->r1_renamed=false;
                  dern[j]->r1b=dern[j]->r1;
                  dern[j]->r1_ready = true;
               }
               if(dern[j]->r2 != -1 && rmt[dern[j]->r2].valid) {
                  dern[j]->r2_renamed=true;
                  dern[j]->r2b=rmt[dern[j]->r2].tag;
                  dern[j]->r2_ready = rob[dern[j]->r2b].ready;
               } else
               {
                  dern[j]->r2_renamed=false;
                  dern[j]->r2b=dern[j]->r2;
                  dern[j]->r2_ready = true;
               }
               dern[j]->rob_dest = tail_rob;
               rnrr.emplace_back(dern[j]);
            }
            dern.clear();

      }


      //DECODE
      if(!fede.empty() && dern.empty()){
      for(j=0; j < fede.size(); ++j){
         fede[j]->de_beg=clk;
         dern.emplace_back(fede[j]);

      }
      fede.clear();
      }


      //FETCH
      if(fede.empty()) {
         for (j = 0; j < width && last_fetched < instrs.size(); ++j) {
            fede.emplace_back(&instrs[++last_fetched]);
            instrs[last_fetched].fe_beg=clk;
         }
      }


      // ADVANCE CLOCK CYCLE AND EXIT CONDITION
      ++clk;
      if(clk > 3 && rob_avail == rob.size() && fede.empty()) break;
   }

}


