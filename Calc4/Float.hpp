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

/*
   An arbitrary precision float class, a wrapper for MPFR.
*/

#ifndef FLOAT_HPP
#define FLOAT_HPP

#include <string>

namespace DecFloat
 {
   class DataHolder;

   enum SupportedRoundModes
    {
      ROUND_MIN_VALUE_NOT_A_MODE = -1,
      ROUND_TO_NEAREST_TIES_TO_EVEN,
      ROUND_TO_POSITIVE_INFINITY,
      ROUND_TO_NEGATIVE_INFINITY,
      ROUND_TO_ZERO,
      ROUND_AWAY_FROM_ZERO,
      ROUND_MAX_VALUE_NOT_A_MODE
    };

   class Float
    {

      private:
         static unsigned long minPrecision;
         static unsigned long maxPrecision;

         static SupportedRoundModes roundMode;

      public:
         static unsigned long getMinPrecision (void) { return minPrecision; }
         static unsigned long setMinPrecision (unsigned long newPrecision)
          { return (minPrecision = newPrecision); }

         static unsigned long getMaxPrecision (void) { return maxPrecision; }
         static unsigned long setMaxPrecision (unsigned long newPrecision)
          { return (maxPrecision = newPrecision); }

         static SupportedRoundModes getRoundMode (void) { return roundMode; }
         static SupportedRoundModes setRoundMode (SupportedRoundModes newRoundMode)
          { return roundMode = newRoundMode; }

      private:
         DataHolder * Data; // Stores everything.
         // If we peeled off the sign,
         // we would just need to copy the mpfr_t
         // and re-add the sign every operation.

      public:
         Float ();
         Float (const Float &);
         Float (const std::string &);
         Float (const DataHolder *);
         ~Float ();

         unsigned long getPrecision (void) const;
         // Does not honor min/max precision
         unsigned long setPrecision (unsigned long);
         // Honors min/max precision
         unsigned long changePrecision (unsigned long);

         unsigned long enforcePrecision (void);

         std::string toString (void) const;
         void fromString (const std::string &); // Uses current precision, not string precision.

         bool isZero (void) const;
         bool isInfinity (void) const;
         bool isNaN (void) const;

         Float & negate (void);
         Float & abs (void);
         Float & copySign (const Float &);
         Float & setSign (bool);

         Float & operator = (const Float &); // Takes precision of lhs
         Float & operator |= (const Float &); // Preserves precision

         Float & operator += (const Float &);
         Float & operator -= (const Float &);
         Float & operator *= (const Float &);
         Float & operator /= (const Float &);
         Float & operator %= (const Float &);

         Float operator - (void) const;

         const DataHolder * get (void) const;
         void put (const DataHolder *);

         friend class DataHolder;
    }; /* class Float */

   Float operator + (const Float &, const Float &);
   Float operator - (const Float &, const Float &);
   Float operator * (const Float &, const Float &);
   Float operator / (const Float &, const Float &);
   Float operator % (const Float &, const Float &);

   bool operator > (const Float &, const Float &);
   bool operator < (const Float &, const Float &);
   bool operator >= (const Float &, const Float &);
   bool operator <= (const Float &, const Float &);
   bool operator == (const Float &, const Float &);
   bool operator != (const Float &, const Float &);

   Float pi (unsigned long precision);

#define UNARY_FUNCTION_DEC(x) \
   Float x (const Float &);

UNARY_FUNCTION_DEC(sqrt)
UNARY_FUNCTION_DEC(cbrt)
UNARY_FUNCTION_DEC(sqr)
UNARY_FUNCTION_DEC(exp)
UNARY_FUNCTION_DEC(log)
UNARY_FUNCTION_DEC(sin)
UNARY_FUNCTION_DEC(cos)
UNARY_FUNCTION_DEC(tan)
UNARY_FUNCTION_DEC(asin)
UNARY_FUNCTION_DEC(acos)
UNARY_FUNCTION_DEC(atan)
UNARY_FUNCTION_DEC(sinh)
UNARY_FUNCTION_DEC(cosh)
UNARY_FUNCTION_DEC(tanh)
UNARY_FUNCTION_DEC(asinh)
UNARY_FUNCTION_DEC(acosh)
UNARY_FUNCTION_DEC(atanh)
UNARY_FUNCTION_DEC(erf)
UNARY_FUNCTION_DEC(erfc)
UNARY_FUNCTION_DEC(gamma)
UNARY_FUNCTION_DEC(lngamma)
UNARY_FUNCTION_DEC(log10)
UNARY_FUNCTION_DEC(exp10)
UNARY_FUNCTION_DEC(expm1)
UNARY_FUNCTION_DEC(log1p)
UNARY_FUNCTION_DEC(frac)
UNARY_FUNCTION_DEC(floor)
UNARY_FUNCTION_DEC(ceil)
UNARY_FUNCTION_DEC(trunc)
UNARY_FUNCTION_DEC(round)

Float toFloat (long);
long fromFloat (const Float &);

#define BINARY_FUNCTION_DEC(x) \
   Float x (const Float &, const Float &);

BINARY_FUNCTION_DEC(pow)
BINARY_FUNCTION_DEC(atan2)
BINARY_FUNCTION_DEC(hypot)

   Float fma (const Float &, const Float &, const Float &);
   void sincos (const Float & arg, Float & sinVal, Float & cosVal);

 } /* namespace DecFloat */

#endif /* FLOAT_HPP */
