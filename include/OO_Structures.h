//
// Created by smdupor on 11/10/21.
//

#ifndef SUPERSCALAR_OO_STRUCTURES_H
#define SUPERSCALAR_OO_STRUCTURES_H

#include <cstdint>
#include <string>

struct instruction{
   uint_fast64_t uid;
   uint_fast32_t pc;
   uint_fast8_t type;
   int_fast16_t dest, r1, r2, fu;
   bool loaded, complete;

   uint_fast32_t fe_beg, fe_dur, de_beg, de_dur, rn_beg, rn_dur, rr_beg, rr_dur, di_beg, di_dur, is_beg, is_dur,
                  ex_beg, ex_dur, wb_beg, wb_dur, rt_beg, rt_dur;

   explicit instruction(uint_fast64_t Uid, uint_fast32_t Pc, uint_fast8_t Type, int_fast16_t Dest, int_fast16_t R1,
                        int_fast16_t R2){
      this->uid = Uid;
      this->pc = Pc;
      this->type = Type;
      this->dest = Dest;
      this->r1 = R1;
      this->r2 = R2;
      loaded = complete = false;
      fu= -1;
              fe_beg= fe_dur= de_beg= de_dur= rn_beg= rn_dur= rr_beg= rr_dur= di_beg= di_dur= is_beg= is_dur=
              ex_beg= ex_dur= wb_beg= wb_dur= rt_beg= rt_dur = 0;
   }

   std::string to_s() const {
      std::string out;
      out += std::to_string(uid) + " fu{" + std::to_string(fu) + "} src{" + std::to_string(r1) + "," +
              std::to_string(r2) +"} dst{" + std::to_string(dest) +
              "} FE{"+std::to_string(fe_beg) + "," + std::to_string(fe_dur) +
              "} DE{"+std::to_string(de_beg) + "," + std::to_string(de_dur) +
              "} RN{"+std::to_string(rn_beg) + "," + std::to_string(rn_dur) +
              "} RR{"+std::to_string(rr_beg) + "," + std::to_string(rr_dur) +
              "} DI{"+std::to_string(di_beg) + "," + std::to_string(di_dur) +
              "} IS{"+std::to_string(is_beg) + "," + std::to_string(is_dur) +
              "} EX{"+std::to_string(ex_beg) + "," + std::to_string(ex_dur) +
              "} WB{"+std::to_string(wb_beg) + "," + std::to_string(wb_dur) +
              "} RT{"+std::to_string(rt_beg) + "," + std::to_string(rt_dur) + "}\n";
      return out;
   }

};

struct rob_line {
   int_fast16_t arf_dest;
   uint_fast32_t pc, index;
   bool ready, exception, mispredict, head, tail;

   rob_line *next, *prev;

   explicit rob_line(uint_fast16_t DST, uint_fast16_t IND, uint_fast32_t PC, rob_line *NXT) {
      this->arf_dest = DST;
      this->index = IND;
      this->pc = PC;
      ready=exception=mispredict=head=tail=false;
      this->next = NXT;
      this->prev = nullptr;
   }
   explicit rob_line(uint_fast32_t ind) {
      this->arf_dest = INT16_MIN;
      this->index = ind;
      this->pc = 0;
      ready=exception=mispredict=head=tail=false;
      this->next = nullptr;
      this->prev = nullptr;
   }
};

struct rmt_line {
   int_fast32_t tag;
   bool valid;

   explicit rmt_line(){
      tag = INT32_MIN;
      valid= false;
   }
};

struct arf_line {

};

struct iq_line {
   bool valid, ready_r1, ready_r2;
   int_fast32_t tag_dest, tag_r1, tag_r2;

   explicit iq_line(){
      valid=ready_r1=ready_r2 = false;
      tag_dest=tag_r1=tag_r2 = INT32_MIN;
   }
};

#endif //SUPERSCALAR_OO_STRUCTURES_H
