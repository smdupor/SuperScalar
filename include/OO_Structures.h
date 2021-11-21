/**
 * OO_Structures handles all structures for the OOO superscalar simulator.
 * Notably, this file only contains structure definitions: all funcitonality is
 * purely procedural and handled in the sim.cpp file.
 *
 * Created on: November 10th, 2021
 * Author: Stevan Dupor
 * Copyright (C) 2021 Stevan Dupor - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 */

#ifndef SUPERSCALAR_OO_STRUCTURES_H
#define SUPERSCALAR_OO_STRUCTURES_H

#include <cstdint>
#include <string>

enum {
   FE = 1, DE = 2, RN = 3, RR = 4, DI = 5, IS = 6, EX = 7, WB = 8, RT = 9
};

struct instruction {

   uint_fast32_t int_counter;
   uint_fast64_t uid;
   uint_fast32_t pc;
   uint_fast8_t type;
   int_fast16_t dest, r1, r2, r1b, r2b, fu, rob_dest;
   bool loaded, complete, r1_renamed, r2_renamed, r1_ready, r2_ready, robbed;

   uint_fast32_t fe_beg, fe_dur, de_beg, de_dur, rn_beg, rn_dur, rr_beg, rr_dur, di_beg, di_dur, is_beg, is_dur,
           ex_beg, ex_dur, wb_beg, wb_dur, rt_beg, rt_dur;

   explicit instruction(uint_fast64_t Uid, uint_fast32_t Pc, uint_fast8_t Type, int_fast16_t Dest, int_fast16_t R1,
                        int_fast16_t R2) {
      int_counter = 0;
      this->uid = Uid;
      this->pc = Pc;
      this->type = Type;
      this->dest = Dest;
      this->r1 = R1;
      this->r2 = R2;
      r1_renamed = r2_renamed = loaded = complete = false;
      fu = type;
      fe_beg = fe_dur = de_beg = de_dur = rn_beg = rn_dur = rr_beg = rr_dur = di_beg = di_dur = is_beg = is_dur =
      ex_beg = ex_dur = wb_beg = wb_dur = rt_beg = rt_dur = 0;
      switch (this->type) {
         case 2:
            this->ex_dur = 2;
            break;
         case 3:
            this->ex_dur = 3;
            break;
         case 4:
            this->ex_dur = 4;
            break;
         case 5:
            this->ex_dur = 5;
            break;
         case 6:
            this->ex_dur = 6;
            break;
         case 7:
            this->ex_dur = 7;
            break;
         default:
            this->ex_dur = 1;
      }

   }


   std::string to_s() const {
      std::string out;
      out += std::to_string(uid) + " fu{" + std::to_string(fu) + "} src{" + std::to_string(r1) + "," +
             std::to_string(r2) + "} dst{" + std::to_string(dest) +
             "} FE{" + std::to_string(fe_beg) + "," + std::to_string(fe_dur) +
             "} DE{" + std::to_string(de_beg) + "," + std::to_string(de_dur) +
             "} RN{" + std::to_string(rn_beg) + "," + std::to_string(rn_dur) +
             "} RR{" + std::to_string(rr_beg) + "," + std::to_string(rr_dur) +
             "} DI{" + std::to_string(di_beg) + "," + std::to_string(di_dur) +
             "} IS{" + std::to_string(is_beg) + "," + std::to_string(is_dur) +
             "} EX{" + std::to_string(ex_beg) + "," + std::to_string(ex_dur) +
             "} WB{" + std::to_string(wb_beg) + "," + std::to_string(wb_dur) +
             "} RT{" + std::to_string(rt_beg) + "," + std::to_string(rt_dur) + "}\n";
      return out;
   }
   std::string to_string() const {
      std::string out;
      out += "i"+std::to_string(uid+1);
      int t=fe_beg;
      while(t>0) {
         out += " [--] ";
         --t;
      }
      t=fe_dur;
      while(t>0) {
         out += " [FE] ";
         --t;
      }

      t=de_dur;
      while(t>0) {
         out += " [DE] ";
         --t;
      }


      t=rn_dur;
      while(t>0) {
         out += " [RN] ";
         --t;
      }



      t=rr_dur;
      while(t>0) {
         out += " [RR] ";
         --t;
      }


      t=di_dur;
      while(t>0) {
         out += " [DI] ";
         --t;
      }


      t=is_dur;
      while(t>0) {
         out += " [IS] ";
         --t;
      }


      t=ex_dur;
      while(t>0) {
         out += " [EX] ";
         --t;
      }


      t=wb_dur;
      while(t>0) {
         out += " [WB] ";
         --t;
      }


      t=rt_dur;
      while(t>0) {
         out += " [RT] ";
         --t;
      }
      //out+="\n";
   return out;
   }

   bool operator==(uint_fast32_t input) {
      bool result;
      input == this->int_counter ? result = true : result = false;
      return result;
   }

   bool operator<=(uint_fast32_t input) {
      bool result;
      this->int_counter <= input ? result = true : result = false;
      return result;
   }

   bool operator<(uint_fast32_t input) {
      bool result;
      this->int_counter < input ? result = true : result = false;
      return result;
   }

   bool ready(uint_fast8_t state) {
      switch (state) {
         case EX:
            if (type == 0 && this->int_counter == this->ex_dur) return true;
            break;
         default:
            return false;
      }
      return false;
   }

};

struct rob_line {
   int_fast16_t arf_dest;
   uint_fast32_t pc, index;
   bool ready, exception, mispredict;

   explicit rob_line(uint_fast32_t ind) {
      this->arf_dest = INT16_MIN;
      this->index = ind;
      this->pc = 0;
      ready = exception = mispredict = false;
   }
};

struct rmt_line {
   int_fast32_t tag;
   bool valid;

   explicit rmt_line() {
      tag = INT32_MIN;
      valid = false;
   }
};

struct arf_line {

};

struct iq_line {
   bool valid, ready_r1, ready_r2;
   int_fast32_t tag_dest, tag_r1, tag_r2;

   uint_fast32_t pc;

   explicit iq_line() {
      valid = ready_r1 = ready_r2 = false;
      tag_dest = tag_r1 = tag_r2 = INT32_MIN;
   }

   explicit iq_line(bool rr1, int_fast32_t tr1, bool rr2, int_fast32_t tr2, int_fast32_t dst, uint_fast32_t procg) {
      valid = true;
      ready_r1 = rr1;
      ready_r2 = rr2;

      tag_dest = dst;
      tag_r1 = tr1;
      tag_r2 = tr2;

      pc = procg;
   }
};

#endif //SUPERSCALAR_OO_STRUCTURES_H
