/*
Copyright (c) 2013 Thomas DiModica.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of Thomas DiModica nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THOMAS DIMODICA AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DIMODICA OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#include "Float.hpp"
#include "DataHolder.hpp"
#include <cmath>

namespace DecFloat
 {

   static const double digits2bits = 3.321928094887362347870319429489;
   static const unsigned long guardDigits = 2U;
   static const unsigned long guardBits = 0U;

#define PRECISION_CALC(x) \
   (static_cast<unsigned long>(std::ceil((x + guardDigits) * digits2bits)) + guardBits)

   DataHolder::DataHolder (unsigned long prec) : Refs(1), precision(prec),
      bits(PRECISION_CALC(precision))
    {
//Initialize Mutex
      mpfr_init2(Data, bits);
      mpfr_set_d(Data, 0.0, GMP_RNDN);
    }

   DataHolder::DataHolder (const DataHolder & src) :
      Refs(1), precision(src.precision), bits(src.bits)
    {
//Initialize Mutex
      mpfr_init2(Data, bits);
      mpfr_set(Data, src.Data, GMP_RNDN);
    }

   DataHolder::DataHolder (const std::string & src, unsigned long usePrec) : Refs(1), precision(0)
    {
//Initialize Mutex
      std::string toUse = src.substr(0, 5);
      size_t index = 0;

      if ((toUse == "@NaN@") || (toUse == "@Inf@"))
       {
         precision = Float::minPrecision;
         bits = PRECISION_CALC(precision);
         mpfr_init2(Data, bits);
         mpfr_set_str(Data, toUse.c_str(), 10, GMP_RNDN);
         return;
       }
      else
       {
         toUse = src.substr(0, 6);

         if (toUse == "-@Inf@")
          {
            precision = Float::minPrecision;
            bits = PRECISION_CALC(precision);
            mpfr_init2(Data, bits);
            mpfr_set_str(Data, toUse.c_str(), 10, GMP_RNDN);
            return;
          }
         else
          {
            toUse = src;
          }
       }

      // Trim leading whitespace
      while ( (index < toUse.size()) &&
             ((toUse[index] == ' ' ) || (toUse[index] == '\t')) )
         ++index;
      toUse = toUse.substr(index);

      // Find end of number
      // ["+"|"-"](<digit>+("."|",")<digit>*|("."|",")<digit>+)[("e"|"E")["+"|"-"]<digit>+]
      if ((index < toUse.size()) && ((toUse[index] == '-') || (toUse[index] == '+')))
         ++index;
      while ((index < toUse.size()) && (toUse[index] >= '0') && (toUse[index] <= '9'))
       {
         ++index;
         ++precision;
       }
      if ((index < toUse.size()) && ((toUse[index] == '.') || (toUse[index] == ',')))
       {
          // Normalize ',' to '.'
         if (',' == toUse[index]) toUse[index] = '.';
         ++index;
       }
      while ((index < toUse.size()) && (toUse[index] >= '0') && (toUse[index] <= '9'))
       {
         ++index;
         ++precision;
       }
      if ((index < toUse.size()) && (precision != 0) && ((toUse[index] == 'e') || (toUse[index] == 'E')))
       {
         ++index;
         if ((index < toUse.size()) && ((toUse[index] == '-') || (toUse[index] == '+')))
            ++index;
         while ((index < toUse.size()) && (toUse[index] >= '0') && (toUse[index] <= '9'))
            ++index;
         if ((toUse[index - 1] == '-') || (toUse[index - 1] == '+'))
            --index;
         if ((toUse[index - 1] == 'e') || (toUse[index - 1] == 'E'))
            --index;
       }

      if (usePrec != 0)
       {
         toUse = toUse.substr(0, index);
         precision = usePrec;
         bits = PRECISION_CALC(usePrec);
         mpfr_init2(Data, bits);
         mpfr_set_str(Data, toUse.c_str(), 10, GMP_RNDN);
       }
      else if (precision != 0)
       {
         toUse = toUse.substr(0, index);
         bits = PRECISION_CALC(precision);
         mpfr_init2(Data, bits);
         mpfr_set_str(Data, toUse.c_str(), 10, GMP_RNDN);
       }
      else
       {
         precision = Float::minPrecision;
         bits = PRECISION_CALC(precision);
         mpfr_init2(Data, bits);
         mpfr_set_d(Data, 0.0, GMP_RNDN);
       }
    }

   DataHolder::~DataHolder()
    {
//Finalize Mutex
      mpfr_clear(Data);
    }

   DataHolder * DataHolder::ref (void) const
    {
//Lock Mutex
      ++Refs;
//Unlock Mutex
      return const_cast<DataHolder *>(this);
    }

   void DataHolder::deref (void)
    {
//Lock Mutex
      long res = --Refs;
//Unlock Mutex
      if (res == 0)
       {
         delete this;
       }
    }

   DataHolder * DataHolder::own (void)
    {
      DataHolder * newThis = this;
//Lock Mutex
      if (Refs != 1)
       {
         --Refs;
         newThis = new DataHolder(*this);
       }
//Unlock Mutex
      return newThis;
    }

   mpfr_ptr DataHolder::getInternal (void)
    {
      mpfr_ptr ret = NULL;
//Lock Mutex
      if (Refs == 1)
       {
         ret = Data;
       }
//Unlock Mutex
      return ret;
    }

   mpfr_srcptr DataHolder::get (void) const
    {
      return Data;
    }

   unsigned long DataHolder::getPrecision (void) const
    {
      return precision;
    }

   DataHolder & DataHolder::ZERO (void)
    {
      static DataHolder instance (0);
      return instance;
    }

 } /* namespace DecFloat */
