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
#ifndef DB_STDLIB_HPP
#define DB_STDLIB_HPP

#include "ValueType.hpp"

class CallingContext;

#define CONSTANT_FUNCTION(x) \
   ValueType::ValueHolder DB_##x (void)

CONSTANT_FUNCTION(date);
CONSTANT_FUNCTION(time);
CONSTANT_FUNCTION(getround);
CONSTANT_FUNCTION(instr);
CONSTANT_FUNCTION(inchr);
CONSTANT_FUNCTION(pi);
CONSTANT_FUNCTION(rand);
CONSTANT_FUNCTION(eolstr);

#undef CONSTANT_FUNCTION

#define UNARY_FUNCTION(x) \
   ValueType::ValueHolder DB_##x (const ValueType::ValueHolder &, const CallingContext &, size_t)

UNARY_FUNCTION(sin);
UNARY_FUNCTION(cos);
UNARY_FUNCTION(tan);
UNARY_FUNCTION(asin);
UNARY_FUNCTION(acos);
UNARY_FUNCTION(atan);
UNARY_FUNCTION(str);
UNARY_FUNCTION(val);
UNARY_FUNCTION(log);
UNARY_FUNCTION(exp);
UNARY_FUNCTION(sqrt);
UNARY_FUNCTION(floor);
UNARY_FUNCTION(setround);
UNARY_FUNCTION(print);
UNARY_FUNCTION(ucase);
UNARY_FUNCTION(lcase);
UNARY_FUNCTION(chr);
UNARY_FUNCTION(asc);
UNARY_FUNCTION(neg);
UNARY_FUNCTION(sign);
UNARY_FUNCTION(ltrim);
UNARY_FUNCTION(rtrim);
UNARY_FUNCTION(spacestr);
UNARY_FUNCTION(isstr);
UNARY_FUNCTION(isval);
UNARY_FUNCTION(isarray);
UNARY_FUNCTION(isnull);
UNARY_FUNCTION(alloc);
UNARY_FUNCTION(len);
UNARY_FUNCTION(prec);
UNARY_FUNCTION(size);
UNARY_FUNCTION(isinf);
UNARY_FUNCTION(isnan);
UNARY_FUNCTION(die);
UNARY_FUNCTION(eval);
UNARY_FUNCTION(define);

#undef UNARY_FUNCTION

#define BINARY_FUNCTION(x) \
   ValueType::ValueHolder DB_##x (const ValueType::ValueHolder &, const ValueType::ValueHolder &, \
                                  const CallingContext &, size_t)

BINARY_FUNCTION(atan2);
BINARY_FUNCTION(pow);
BINARY_FUNCTION(reprec);
BINARY_FUNCTION(resize);
BINARY_FUNCTION(leftstr);
BINARY_FUNCTION(rightstr);
BINARY_FUNCTION(copysign);
BINARY_FUNCTION(stringstr);

#undef BINARY_FUNCTION

#define TERNARY_FUNCTION(x) \
   ValueType::ValueHolder DB_##x \
      (const ValueType::ValueHolder &, const ValueType::ValueHolder &, const ValueType::ValueHolder &, \
       const CallingContext &, size_t)

TERNARY_FUNCTION(midstr);

#undef TERNARY_FUNCTION

#endif /* DB_STDLIB_HPP */
