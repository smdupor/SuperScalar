#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include "sim_proc.h"
#include "OO_Structures.h"

   uint_fast32_t uid, clk, instr_count;

   size_t head_rob, tail_rob;

   const size_t ARF_SIZE = 67;

   std::vector<rob_line> rob;
   std::vector<arf_line> arf;
   std::vector<rmt_line> rmt;
   std::vector<iq_line> iq;

   void run_simulation(std::vector<instruction> *instrs);


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

    for(instruction &i : instr) {
       std::cout << i.to_s();
    }


    return EXIT_SUCCESS;
}

void run_simulation(std::vector<instruction> *instrs){

	std::vector<*instruction> exwb, isex, diis, rrdi, rnrr, dern, fede; 


}


