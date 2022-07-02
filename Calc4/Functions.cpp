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

namespace DecFloat
 {

   extern mpfr_rnd_t roundModes [ROUND_MAX_VALUE_NOT_A_MODE];

/////////////////////////
// NO INPUT ONE OUTPUT //
/////////////////////////

   Float pi (unsigned long precision)
    {
      DataHolder * result = DataHolder::build(precision);
      mpfr_const_pi(result->getInternal(), roundModes[Float::getRoundMode()]);
      Float res (result);
      result->deref();
      return res;
    }

//////////////////////////
// ONE INPUT ONE OUTPUT //
//////////////////////////

#define UNARY_FUNCTION(x) \
   Float x (const Float & arg) \
    { \
      DataHolder * result = DataHolder::build(arg.getPrecision()); \
      mpfr_##x(result->getInternal(), arg.get()->get(), roundModes[Float::getRoundMode()]); \
      Float res (result); \
      result->deref(); \
      return res; \
    }

UNARY_FUNCTION(sqrt)
UNARY_FUNCTION(cbrt)
UNARY_FUNCTION(sqr)
UNARY_FUNCTION(exp)
UNARY_FUNCTION(log)
UNARY_FUNCTION(sin)
UNARY_FUNCTION(cos)
UNARY_FUNCTION(tan)
UNARY_FUNCTION(asin)
UNARY_FUNCTION(acos)
UNARY_FUNCTION(atan)
UNARY_FUNCTION(sinh)
UNARY_FUNCTION(cosh)
UNARY_FUNCTION(tanh)
UNARY_FUNCTION(asinh)
UNARY_FUNCTION(acosh)
UNARY_FUNCTION(atanh)
UNARY_FUNCTION(erf)
UNARY_FUNCTION(erfc)
UNARY_FUNCTION(gamma)
UNARY_FUNCTION(lngamma)
UNARY_FUNCTION(log10)
UNARY_FUNCTION(exp10)
UNARY_FUNCTION(expm1)
UNARY_FUNCTION(log1p)
UNARY_FUNCTION(frac)

#define ROUNDING_FUNCTION(x) \
   Float x (const Float & arg) \
    { \
      DataHolder * result = DataHolder::build(arg.getPrecision()); \
      mpfr_##x(result->getInternal(), arg.get()->get()); \
      Float res (result); \
      result->deref(); \
      return res; \
    }

ROUNDING_FUNCTION(floor)
ROUNDING_FUNCTION(ceil)
ROUNDING_FUNCTION(trunc)
ROUNDING_FUNCTION(round)

   Float toFloat (long value)
    {   //   Accommodate a 64 bit long
      DataHolder * result = DataHolder::build(20);
      mpfr_set_si(result->getInternal(), value, roundModes[Float::getRoundMode()]);
      Float res (result);
      result->deref();
      return res;
    }

   long fromFloat (const Float & value)
    {
      return mpfr_get_si(value.get()->get(), roundModes[Float::getRoundMode()]);
    }

///////////////////////////
// TWO INPUTS ONE OUTPUT //
///////////////////////////

#define BINARY_FUNCTION(x) \
   Float x (const Float & arg1, const Float & arg2) \
    { \
      unsigned long prec = arg1.getPrecision() < arg2.getPrecision() ? \
         arg2.getPrecision() : arg1.getPrecision(); \
      DataHolder * result = DataHolder::build(prec); \
      mpfr_##x(result->getInternal(), arg1.get()->get(), arg2.get()->get(), roundModes[Float::getRoundMode()]); \
      Float res (result); \
      result->deref(); \
      return res; \
    }

BINARY_FUNCTION(pow)
BINARY_FUNCTION(atan2)
BINARY_FUNCTION(hypot)

/////////////////////////////
// THREE INPUTS ONE OUTPUT //
/////////////////////////////

   Float fma (const Float & arg1, const Float & arg2, const Float & arg3)
    {
      unsigned long prec = arg1.getPrecision() < arg2.getPrecision() ?
         arg2.getPrecision() : arg1.getPrecision();
      prec = prec < arg3.getPrecision() ? arg3.getPrecision() : prec;
      DataHolder * result = DataHolder::build(prec);
      mpfr_fma(result->getInternal(), arg1.get()->get(), arg2.get()->get(),
         arg3.get()->get(), roundModes[Float::getRoundMode()]);
      Float res (result);
      result->deref();
      return res;
    }

///////////////////////////
// ONE INPUT TWO OUTPUTS //
///////////////////////////

   void sincos (const Float & arg, Float & sinVal, Float & cosVal)
    {
      DataHolder * sinRes = DataHolder::build(arg.getPrecision());
      DataHolder * cosRes = DataHolder::build(arg.getPrecision());
      mpfr_sin_cos(sinRes->getInternal(), cosRes->getInternal(),
         arg.get()->get(), roundModes[Float::getRoundMode()]);
      // Could also be cosVal = Float(cosRes); Would be clearer but less efficient.
      cosVal.put(cosRes); // if &sinVal == &cosVal, result is sin
      sinVal.put(sinRes);
      sinRes->deref();
      cosRes->deref();
      return;
    }

 } // namespace DecFloat
