/*
Copyright (c) 2015 Thomas DiModica.
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
#include "Missing.hpp"
#include "../AltCalc5Slimmed/Constants.hpp"

#include <sstream>


namespace BigInt
 {

   Float pi (unsigned long precision)
    {
      Float pi(M_PI);
      pi.setPrecision(precision);
      return pi;
    }

   long fromFloat (const Float & src)
    {
      std::istringstream str(src.toString());
      double res;
      str >> res;
      return static_cast<long>(res);
    }

   Float toFloat (long src)
    {
      std::ostringstream str;
      str << src;
      Float temp (str.str());
      temp.normalize();
      return temp;
    }

   Float floor (const Float & src)
    {
         /* I'm not sure if I like that floor(-0) -> -0, but it the C/C++ standard says so. */
      if ((true == src.isInfinity()) || (true == src.isNaN()) || (true == src.isZero()))
       {
         return src;
       }
      if (src.exponent() < 0)
       {
         if (true == src.isSigned())
          {
            Float negOne (M_1);
            negOne.negate();
            negOne.setPrecision(src.getPrecision());
            return negOne;
          }
         else
          {
            Float zero;
            zero.setPrecision(src.getPrecision());
            return zero;
          }
       }
      if (static_cast<unsigned long>(src.exponent()) >= src.getPrecision())
       {
         return src;
       }
         /* I don't like what I'm doing here, but this is the C Standard's
            implementation with respect to a return integer function. */
      Fixed_Round_Mode replace = Fixed::getRoundMode();
      Fixed::setRoundMode(ROUND_NEGATIVE_INFINITY);
      Float result (src);
      unsigned long prec = result.getPrecision();
      result.setPrecision(result.exponent());
      result.setPrecision(prec);
      Fixed::setRoundMode(replace);
      return result;
    }

 } /* namespace BigInt */
