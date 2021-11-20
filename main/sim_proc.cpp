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

   size_t head_rob, tail_rob;

   const size_t ARF_SIZE = 67;

   std::vector<rob_line> rob;
   std::vector<arf_line> arf;
   std::vector<rmt_line> rmt;
   std::vector<iq_line> iq;

   void run_simulation(std::vector<instruction> &instrs, uint_fast16_t width);


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

   for(i=0;i<params.iq_size;++i){
      iq.emplace_back(iq_line());
   }

   for(i=0;i<ARF_SIZE;++i){
      rmt.emplace_back(rmt_line());
   }

   uid= clk= instr_count=0;
   head_rob=tail_rob=0;


   /////// DEBUG POINTERS
   std::vector<rob_line> *dbg_z_rb = &rob;
   std::vector<rmt_line> *dbg_z_rt = &rmt;
   std::vector<iq_line> *dbg_z_isq = &iq;
   uint_fast32_t *dbg_uid=&uid, *dbg_clk=&clk, *dbg_instr_count=&instr_count;
   size_t *dbg_head_rob=&head_rob, *dbg_tail_rob=&tail_rob;

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

   run_simulation(instr, params.width);

    for(instruction &i : instr) {
       std::cout << i.to_s();
    }


    return EXIT_SUCCESS;
}

void run_simulation(std::vector<instruction> &instrs, uint_fast16_t width){
   bool done = false;
   bool rob_full = false;
	std::vector<instruction *> exwb, isex, diis, rrdi, rnrr, dern, fede;

   uint_fast16_t j;

   uint_fast32_t last_fetched = 0;

   while(!done){

      //RETIRE
      for(j=0; j < width; ++j){
         if(rob[head_rob].ready){
            instrs[rob[head_rob].index].rt_beg = clk;
            ++head_rob;
            if(head_rob == rob.size()) head_rob = 0;
         }
      }

      // WRITEBACK
      for(j=0; j < width; ++j){
         for(instruction * i : exwb){
            std::find_if(rob.begin(), rob.end(), [&](instruction inst) {return i->uid == inst.uid;})->ready=true;
         }
         exwb.clear();
      }


      //EXECUTE
      for(j=0; j < width; ++j){

      }


      //ISSUE
      for(j=0; j < width; ++j){

      }


      //DISPATCH
      for(j=0; j < width; ++j){

      }


      //REG READ
      if(rrdi.empty() && !rnrr.empty()) {
         for (j = 0; j < rnrr.size(); ++j) {
            rnrr[j]->rr_beg=clk;
            /////////// STOPPED HERE
         }
      }


      //RENAME
      if(!rob_full && !dern.empty() && rnrr.empty()) {
         if((head_rob <= tail_rob && rob.size()-(tail_rob-head_rob) >= dern.size()) ||
                  (head_rob > tail_rob && head_rob-tail_rob >= dern.size())) {
            for (j = 0; j < dern.size(); ++j) {
               dern[j]->rn_beg=clk;
               tail_rob++;
               if(tail_rob==rob.size()) tail_rob=0;

               rob[tail_rob].ready=false;
               rob[tail_rob].pc=dern[j]->pc;
               rob[tail_rob].arf_dest = dern[j]->dest;

               dern[j]->r1; ///// RENAME REG 1
               dern[j]->r2; ///// RENAME REG 2
               dern[j]->dest; ///// RENAME DEST
               rnrr.emplace_back(dern[j]);
            }
            dern.clear();
         }
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
      if(clk > 10263) break;
   }

}


