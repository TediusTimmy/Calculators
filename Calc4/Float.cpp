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

#include <sstream>

namespace DecFloat
 {

   mpfr_rnd_t roundModes[ROUND_MAX_VALUE_NOT_A_MODE] =
    { MPFR_RNDN, MPFR_RNDU, MPFR_RNDD, MPFR_RNDZ, MPFR_RNDA };

   // Controlling the exponent range can't be done "cleanly" in MPFR.
   // By clean, I mean decimal limits like [-99,99] can't be done when the exponent is binary.
#ifdef MODE_1
   unsigned long Float::minPrecision = 50;
   unsigned long Float::maxPrecision = 50;
#else
 #ifdef MODE_2
   unsigned long Float::minPrecision = 70;
   unsigned long Float::maxPrecision = 70;
 #else
   unsigned long Float::minPrecision = 8;
   unsigned long Float::maxPrecision = 256;
 #endif
#endif

   SupportedRoundModes Float::roundMode = ROUND_TO_NEAREST_TIES_TO_EVEN;

   Float::Float () : Data(DataHolder::ZERO().ref()) { }

   Float::Float (const Float & src) : Data(src.Data->ref()) { }

   Float::Float (const std::string & src) : Data (DataHolder::build(src)) { }

   Float::Float (const DataHolder * src) : Data (src->ref()) { }

   Float::~Float ()
    {
      Data->deref();
      Data = NULL;
    }


   unsigned long Float::getPrecision (void) const
    {
      return Data->getPrecision();
    }

   unsigned long Float::setPrecision (unsigned long newPrecision)
    {
      if (Data->getPrecision() != newPrecision)
       {
         DataHolder * newData = DataHolder::build (newPrecision);
         mpfr_set(newData->getInternal(), Data->get(), roundModes[roundMode]);
         Data->deref();
         Data = newData;
       }

      return Data->getPrecision();
    }

   unsigned long Float::changePrecision (unsigned long newPrecision)
    {
      if (newPrecision < minPrecision)
       {
         newPrecision = minPrecision;
       }
      else if (newPrecision > maxPrecision)
       {
         newPrecision = maxPrecision;
       }

      return setPrecision(newPrecision);
    }

   unsigned long Float::enforcePrecision (void)
    {
      if (Data->getPrecision() < minPrecision)
       {
         setPrecision(minPrecision);
       }
      else if (Data->getPrecision() > maxPrecision)
       {
         setPrecision(maxPrecision);
       }

      return Data->getPrecision();
    }


   std::string Float::toString (void) const
    {
      std::string res;
      mp_exp_t exp;

      unsigned long prec = Data->getPrecision();
      if (prec < 2) // MPFR aborts if we pass < 2
         prec = 2;

      char * loc = mpfr_get_str(NULL, &exp, 10, prec, Data->get(), roundModes[roundMode]);
      res = loc;
      mpfr_free_str(loc);
      if (mpfr_number_p(Data->get()))
       {
         // Special zero handling....
         if (mpfr_zero_p(Data->get()))
          {
            // Pad a Zero with more zeros.
            while (res.size() < Data->getPrecision())
               res += '0';
          }
         else
          {
            // Exponent is .XYZeW and we want X.YZeW
            --exp; // So decrement it to normalize it.
          }

         // Convert the exponent to a string
         std::stringstream expo;
         expo << exp;

         // Add in the decimal place
         if (res[0] == '-')
            res = res.substr(0, 2) + '.' + res.substr(2);
         else
            res = res.substr(0, 1) + '.' + res.substr(1);

         // Add in the exponent
         res = res + 'E' + expo.str();
       }
      return res;
    }

   void Float::fromString (const std::string & src)
    {
      unsigned long usePrec = Data->getPrecision();
      Data->deref();
      Data = DataHolder::build(src, usePrec);
    }


   bool Float::isZero (void) const
    {
      return (0 != mpfr_zero_p(Data->get()));
    }

   bool Float::isInfinity (void) const
    {
      return (0 != mpfr_inf_p(Data->get()));
    }

   bool Float::isNaN (void) const
    {
      return (0 != mpfr_nan_p(Data->get()));
    }


   Float & Float::negate (void)
    {
      Data = Data->own();
      mpfr_neg(Data->getInternal(), Data->get(), roundModes[roundMode]);
      return *this;
    }

   Float & Float::abs (void)
    {
      Data = Data->own();
      mpfr_abs(Data->getInternal(), Data->get(), roundModes[roundMode]);
      return *this;
    }

   Float & Float::copySign (const Float & src)
    {
      Data = Data->own();
      mpfr_copysign(Data->getInternal(), Data->get(), src.Data->get(), roundModes[roundMode]);
      return *this;
    }

   Float & Float::setSign (bool sign)
    {
      Data = Data->own();
      mpfr_setsign(Data->getInternal(), Data->get(), sign, roundModes[roundMode]);
      return *this;
    }


   Float & Float::operator = (const Float & rhs)
    {
      put(rhs.Data);
      return *this;
    }

   Float & Float::operator |= (const Float & rhs)
    {
      if (Data != rhs.Data)
       {
         Data = Data->own();
         mpfr_set(Data->getInternal(), rhs.Data->get(), roundModes[roundMode]);
       }
      return *this;
    }


   Float & Float::operator += (const Float & rhs)
    {
      return (*this = *this + rhs);
    }

   Float & Float::operator -= (const Float & rhs)
    {
      return (*this = *this - rhs);
    }

   Float & Float::operator *= (const Float & rhs)
    {
      return (*this = *this * rhs);
    }

   Float & Float::operator /= (const Float & rhs)
    {
      return (*this = *this / rhs);
    }

   Float & Float::operator %= (const Float & rhs)
    {
      return (*this = *this % rhs);
    }


   Float Float::operator - (void) const
    {
      Float result (*this);
      return result.negate();
    }


   const DataHolder * Float::get (void) const
    {
      return Data;
    }

   void Float::put (const DataHolder * data)
    {
      if (Data != data)
       {
         Data->deref();
         Data = data->ref();
       }
    }


   Float operator + (const Float & lhs, const Float & rhs)
    {
      unsigned long prec = lhs.getPrecision() < rhs.getPrecision() ?
         rhs.getPrecision() : lhs.getPrecision();

      DataHolder * newData = DataHolder::build(prec);

      mpfr_add(newData->getInternal(), lhs.get()->get(), rhs.get()->get(), roundModes[Float::getRoundMode()]);

      Float result (newData);

      newData->deref(); // Release function's reference

      return result;
    }

   Float operator - (const Float & lhs, const Float & rhs)
    {
      unsigned long prec = lhs.getPrecision() < rhs.getPrecision() ?
         rhs.getPrecision() : lhs.getPrecision();

      DataHolder * newData = DataHolder::build(prec);

      mpfr_sub(newData->getInternal(), lhs.get()->get(), rhs.get()->get(), roundModes[Float::getRoundMode()]);

      Float result (newData);

      newData->deref(); // Release function's reference

      return result;
    }

   Float operator * (const Float & lhs, const Float & rhs)
    {
      unsigned long prec = lhs.getPrecision() < rhs.getPrecision() ?
         rhs.getPrecision() : lhs.getPrecision();

      DataHolder * newData = DataHolder::build(prec);

      mpfr_mul(newData->getInternal(), lhs.get()->get(), rhs.get()->get(), roundModes[Float::getRoundMode()]);

      Float result (newData);

      newData->deref(); // Release function's reference

      return result;
    }

   Float operator / (const Float & lhs, const Float & rhs)
    {
      unsigned long prec = lhs.getPrecision() < rhs.getPrecision() ?
         rhs.getPrecision() : lhs.getPrecision();

      DataHolder * newData = DataHolder::build(prec);

      mpfr_div(newData->getInternal(), lhs.get()->get(), rhs.get()->get(), roundModes[Float::getRoundMode()]);

      Float result (newData);

      newData->deref(); // Release function's reference

      return result;
    }

   Float operator % (const Float & lhs, const Float & rhs)
    {
      unsigned long prec = lhs.getPrecision() < rhs.getPrecision() ?
         rhs.getPrecision() : lhs.getPrecision();

      DataHolder * newData = DataHolder::build(prec);

      mpfr_remainder(newData->getInternal(), lhs.get()->get(), rhs.get()->get(), roundModes[Float::getRoundMode()]);

      Float result (newData);

      newData->deref(); // Release function's reference

      return result;
    }


   bool operator == (const Float & lhs, const Float & rhs)
    {
      return (0 != mpfr_equal_p(lhs.get()->get(), rhs.get()->get()));
    }

   bool operator != (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() && rhs.isNaN())
         return true;
      return (0 != mpfr_lessgreater_p(lhs.get()->get(), rhs.get()->get()));
    }

   bool operator > (const Float & lhs, const Float & rhs)
    {
      return (0 != mpfr_greater_p(lhs.get()->get(), rhs.get()->get()));
    }

   bool operator >= (const Float & lhs, const Float & rhs)
    {
      return (0 != mpfr_greaterequal_p(lhs.get()->get(), rhs.get()->get()));
    }

   bool operator < (const Float & lhs, const Float & rhs)
    {
      return (0 != mpfr_less_p(lhs.get()->get(), rhs.get()->get()));
    }

   bool operator <= (const Float & lhs, const Float & rhs)
    {
      return (0 != mpfr_lessequal_p(lhs.get()->get(), rhs.get()->get()));
    }

 } //  namespace DecFloat
