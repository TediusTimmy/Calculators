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
#include "ValueType.hpp"
#include "../AltCalc5Slimmed/Fixed.hpp"
#include <iostream>
#include <ctime>
#include <cstdio>
#include <cctype>

class CallingContext;

void DB_panic (const std::string &, const CallingContext &, size_t) __attribute__ ((__noreturn__));

#ifndef DONT_USE_RAND850
namespace BigInt
 {
   Float rand (void);
 }
#endif

extern const BigInt::Float DBTrue , DBFalse;



#define STD_MATH_UNARY_FUN(x,y) \
   ValueType::ValueHolder DB_##x (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo) \
    { \
      if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) ) \
         DB_panic("Bad data type in " y ".", context, lineNo); \
      return ValueType::ValueHolder(new NumericValue( BigInt::x(static_cast<NumericValue*>(arg.data)->val) )); \
    }

STD_MATH_UNARY_FUN(sin, "sin")
STD_MATH_UNARY_FUN(cos, "cos")
STD_MATH_UNARY_FUN(tan, "tan")
STD_MATH_UNARY_FUN(asin, "asin")
STD_MATH_UNARY_FUN(acos, "acos")
STD_MATH_UNARY_FUN(atan, "atan")
STD_MATH_UNARY_FUN(log, "log")
STD_MATH_UNARY_FUN(exp, "exp")
STD_MATH_UNARY_FUN(sqrt, "sqrt")
STD_MATH_UNARY_FUN(floor, "floor")


ValueType::ValueHolder DB_rand (void)
 {
   return ValueType::ValueHolder(new NumericValue(BigInt::rand()));
 }

ValueType::ValueHolder DB_getround (void)
 {
   return ValueType::ValueHolder(new NumericValue(BigInt::toFloat(static_cast<long>(BigInt::Fixed::getRoundMode()))));
 }

ValueType::ValueHolder DB_date (void)
 {
   time_t nowTime;
   struct tm * timeStruct;
   char timeString [80];

   (void) std::time(&nowTime);
   timeStruct = std::localtime(&nowTime);

   std::sprintf(timeString, "%04d-%02d-%02d",
      timeStruct->tm_year + 1900,
      timeStruct->tm_mon + 1,
      timeStruct->tm_mday);

   return ValueType::ValueHolder(new StringValue(timeString));
 }

ValueType::ValueHolder DB_time (void)
 {
   time_t nowTime;
   struct tm * timeStruct;
   char timeString [80];

   (void) std::time(&nowTime);
   timeStruct = std::localtime(&nowTime);

   std::sprintf(timeString, "%02d:%02d:%02d",
      timeStruct->tm_hour,
      timeStruct->tm_min,
      timeStruct->tm_sec);

   return ValueType::ValueHolder(new StringValue(timeString));
 }

ValueType::ValueHolder DB_instr (void)
 {
   if (true == std::cin.eof())
      return ValueType::ValueHolder();
   else
    {
      std::string temp ("");
      getline(std::cin, temp);
      if ((true == std::cin.eof()) && (true == temp.empty()))
         return ValueType::ValueHolder();
      return ValueType::ValueHolder(new StringValue(temp));
    }
 }

ValueType::ValueHolder DB_inchr (void)
 {
   if (true == std::cin.eof())
      return ValueType::ValueHolder();
   else
    {
      std::string temp ("");
      temp = temp + static_cast<char>(static_cast<unsigned char>(std::cin.get()));
      if ((true == std::cin.eof()) && (EOF == temp[0]))
         return ValueType::ValueHolder();
      return ValueType::ValueHolder(new StringValue(temp));
    }
 }

ValueType::ValueHolder DB_eolstr (void)
 {
   return ValueType::ValueHolder(new StringValue("\n"));
 }

ValueType::ValueHolder DB_pi (void)
 {
   return ValueType::ValueHolder(new NumericValue(BigInt::pi(BigInt::Float::getMaxPrecision())));
 }


ValueType::ValueHolder DB_val (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in val.", context, lineNo);

   BigInt::Float conv (static_cast<StringValue*>(arg.data)->val);

   if (conv.getPrecision() < BigInt::Float::getMinPrecision())
    {
      conv = BigInt::pi(BigInt::Float::getMinPrecision());
      conv.fromString(static_cast<StringValue*>(arg.data)->val);
    }
   else if (conv.getPrecision() > BigInt::Float::getMaxPrecision())
    {
      conv = BigInt::pi(BigInt::Float::getMaxPrecision());
      conv.fromString(static_cast<StringValue*>(arg.data)->val);
    }

   return ValueType::ValueHolder(new NumericValue(conv));
 }

ValueType::ValueHolder DB_str (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in str.", context, lineNo);

   return ValueType::ValueHolder(new StringValue(static_cast<NumericValue*>(arg.data)->val.toString()));
 }

ValueType::ValueHolder DB_setround (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in setround.", context, lineNo);

   long mode = BigInt::fromFloat(static_cast<NumericValue*>(arg.data)->val);

    /* I don't like these knowing the first and last items in the list,
       but I also don't want to change the other code. */
   if ( (mode < static_cast<long>(BigInt::ROUND_TIES_EVEN)) ||
        (mode > static_cast<long>(BigInt::ROUND_05_AWAY)) )
      DB_panic("Bad round mode in setround.", context, lineNo);

   BigInt::Fixed::setRoundMode(static_cast<BigInt::Fixed_Round_Mode>(mode));

   return arg;
 }

ValueType::ValueHolder DB_print (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in print.", context, lineNo);

   std::cout << static_cast<StringValue*>(arg.data)->val;

   return arg;
 }

ValueType::ValueHolder DB_ucase (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in ucase.", context, lineNo);

   std::string result(static_cast<StringValue*>(arg.data)->val);

   for (std::string::iterator iter = result.begin();
      result.end() != iter; ++iter)
    {
      *iter = static_cast<char>(std::toupper(*iter));
    }

   return ValueType::ValueHolder(new StringValue(result));
 }

ValueType::ValueHolder DB_lcase (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in lcase.", context, lineNo);

   std::string result(static_cast<StringValue*>(arg.data)->val);

   for (std::string::iterator iter = result.begin();
      result.end() != iter; ++iter)
    {
      *iter = static_cast<char>(std::tolower(*iter));
    }

   return ValueType::ValueHolder(new StringValue(result));
 }

ValueType::ValueHolder DB_chr (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in chr.", context, lineNo);

   std::string temp ("");
   long chr = BigInt::fromFloat(static_cast<NumericValue*>(arg.data)->val);

    // No 0s in strings.
   if ((chr < 1) || (chr > 255))
      DB_panic("Value out of range in chr.", context, lineNo);

   temp = temp + static_cast<char>(chr);

   return ValueType::ValueHolder(new StringValue(temp));
 }

ValueType::ValueHolder DB_asc (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in asc.", context, lineNo);

   if (0 == static_cast<StringValue*>(arg.data)->val.length())
      DB_panic("Empty string in asc.", context, lineNo);

   long chr = static_cast<long>(static_cast<unsigned char>(static_cast<StringValue*>(arg.data)->val[0]));

   return ValueType::ValueHolder(new NumericValue(BigInt::toFloat(chr)));
 }

ValueType::ValueHolder DB_neg (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in neg.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue( static_cast<NumericValue*>(arg.data)->val.abs().negate() ));
 }

ValueType::ValueHolder DB_sign (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in sign.", context, lineNo);

   if (true == static_cast<NumericValue*>(arg.data)->val.isZero())
      return ValueType::ValueHolder(new NumericValue(DBFalse));

   if (static_cast<NumericValue*>(arg.data)->val < DBFalse) // False is 0
      return ValueType::ValueHolder(new NumericValue(-DBTrue)); // True is 1

   return ValueType::ValueHolder(new NumericValue(DBTrue));
 }

ValueType::ValueHolder DB_ltrim (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in ltrim.", context, lineNo);

   std::string result(static_cast<StringValue*>(arg.data)->val);

   std::string::iterator iter = result.begin();
   while ((result.end() != iter) && std::isspace(*iter)) ++iter;

   result.erase(result.begin(), iter);

   return ValueType::ValueHolder(new StringValue(result));
 }

ValueType::ValueHolder DB_rtrim (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in rtrim.", context, lineNo);

   std::string result(static_cast<StringValue*>(arg.data)->val);

   size_t iter = result.length() - 1;
   while ((result.length() > iter) && std::isspace(result[iter])) --iter;

   if (iter != std::string::npos) // assumes npos == ((size_t)-1)
      result.erase(iter + 1, std::string::npos);
   else
      result.erase();

   return ValueType::ValueHolder(new StringValue(result));
 }

ValueType::ValueHolder DB_spacestr (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in spacestr.", context, lineNo);

   long size = BigInt::fromFloat(static_cast<NumericValue*>(arg.data)->val);

   std::string result;
   for (int i = 0; i < size; i++) result.push_back(' ');

   return ValueType::ValueHolder(new StringValue(result));
 }

ValueType::ValueHolder DB_isstr (const ValueType::ValueHolder & arg, const CallingContext &, size_t)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      return ValueType::ValueHolder(new NumericValue(DBFalse));
   return ValueType::ValueHolder(new NumericValue(DBTrue));
 }

ValueType::ValueHolder DB_isval (const ValueType::ValueHolder & arg, const CallingContext &, size_t)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      return ValueType::ValueHolder(new NumericValue(DBFalse));
   return ValueType::ValueHolder(new NumericValue(DBTrue));
 }

ValueType::ValueHolder DB_isarray (const ValueType::ValueHolder & arg, const CallingContext &, size_t)
 {
   if ( (NULL == arg.data) || (ValueType::ARRAY != arg.data->type) )
      return ValueType::ValueHolder(new NumericValue(DBFalse));
   return ValueType::ValueHolder(new NumericValue(DBTrue));
 }

ValueType::ValueHolder DB_isnull (const ValueType::ValueHolder & arg, const CallingContext &, size_t)
 {
   if (NULL == arg.data)
      return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder DB_alloc (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in alloc.", context, lineNo);

   long elements = BigInt::fromFloat(static_cast<NumericValue*>(arg.data)->val);

   if (0 > elements)
      DB_panic("Cannot alloc array.", context, lineNo);

   return ValueType::ValueHolder(new ArrayValue(elements));
 }

ValueType::ValueHolder DB_len (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in len.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(BigInt::toFloat(
      static_cast<long>(static_cast<StringValue*>(arg.data)->val.length()))));
 }

ValueType::ValueHolder DB_prec (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in prec.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(BigInt::toFloat(
      static_cast<NumericValue*>(arg.data)->val.getPrecision())));
 }

ValueType::ValueHolder DB_size (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::ARRAY != arg.data->type) )
      DB_panic("Bad data type in size.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(BigInt::toFloat(
      static_cast<long>(static_cast<ArrayValue*>(arg.data)->size()))));
 }

ValueType::ValueHolder DB_isinf (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in isinf.", context, lineNo);

   if (true == static_cast<NumericValue*>(arg.data)->val.isInfinity())
      return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }


ValueType::ValueHolder DB_isnan (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::NUMBER != arg.data->type) )
      DB_panic("Bad data type in isnan.", context, lineNo);

   if (true == static_cast<NumericValue*>(arg.data)->val.isNaN())
      return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder DB_die (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in die.", context, lineNo);

   DB_panic(static_cast<StringValue*>(arg.data)->val, context, lineNo);
 }



ValueType::ValueHolder DB_midstr
   (const ValueType::ValueHolder & first, const ValueType::ValueHolder & second, const ValueType::ValueHolder & third,
    const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == first.data) || (ValueType::STRING != first.data->type) ||
        (NULL == second.data) || (ValueType::NUMBER != second.data->type) ||
        (NULL == third.data) || (ValueType::NUMBER != third.data->type) )
      DB_panic("Bad data type in midstr.", context, lineNo);

   std::string result (static_cast<StringValue*>(first.data)->val);
   long index = BigInt::fromFloat(static_cast<NumericValue*>(second.data)->val);
   long count = BigInt::fromFloat(static_cast<NumericValue*>(third.data)->val);

   if ((1U == result.size()) && (1 == index) && (count > -1))
      return ValueType::ValueHolder(new StringValue(""));

   if ((index < 0) || (count < 0) || (static_cast<size_t>(index) >= result.size()))
      DB_panic("Bad value in midstr.", context, lineNo);

   return ValueType::ValueHolder(new StringValue( result.substr(index, count) ));
 }


#define STD_MATH_BINARY_FUN(x,y) \
   ValueType::ValueHolder DB_##x (const ValueType::ValueHolder & first, const ValueType::ValueHolder & second, \
                                  const CallingContext & context, size_t lineNo) \
    { \
      if ( (NULL == first.data) || (ValueType::NUMBER != first.data->type) || \
           (NULL == second.data) || (ValueType::NUMBER != second.data->type)) \
         DB_panic("Bad data type in " y ".", context, lineNo); \
      return ValueType::ValueHolder(new NumericValue( BigInt::x( \
         static_cast<NumericValue*>(first.data)->val, \
         static_cast<NumericValue*>(second.data)->val ))); \
    }

STD_MATH_BINARY_FUN(atan2, "atan2")
STD_MATH_BINARY_FUN(pow, "pow")


ValueType::ValueHolder DB_reprec (const ValueType::ValueHolder & num, const ValueType::ValueHolder & prec,
   const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == num.data) || (ValueType::NUMBER != num.data->type) ||
        (NULL == prec.data) || (ValueType::NUMBER != prec.data->type))
      DB_panic("Bad data type in reprec.", context, lineNo);

   long precise = BigInt::fromFloat(static_cast<NumericValue*>(prec.data)->val);
   BigInt::Float temp (static_cast<NumericValue*>(num.data)->val);

   temp.changePrecision(precise); // Takes into account min and max precisions.

   return ValueType::ValueHolder(new NumericValue(temp));
 }

ValueType::ValueHolder DB_resize (const ValueType::ValueHolder & array, const ValueType::ValueHolder & size,
   const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == array.data) || (ValueType::ARRAY != array.data->type) ||
        (NULL == size.data) || (ValueType::NUMBER != size.data->type))
      DB_panic("Bad data type in resize.", context, lineNo);

   long newSize = BigInt::fromFloat(static_cast<NumericValue*>(size.data)->val);

   if (0 > newSize)
      DB_panic("Cannot resize array.", context, lineNo);

   size_t end = static_cast<ArrayValue*>(array.data)->size();

   if (static_cast<size_t>(newSize) == end) return array;

   if (static_cast<size_t>(newSize) < end) end = static_cast<size_t>(newSize);

   ArrayValue * newArray  = new ArrayValue (newSize);

   for (size_t i = 0; i < end; ++i)
      newArray->setIndex(i, static_cast<ArrayValue*>(array.data)->getIndex(i));

   return ValueType::ValueHolder(newArray);
 }

ValueType::ValueHolder DB_leftstr (const ValueType::ValueHolder & str, const ValueType::ValueHolder & num,
   const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == str.data) || (ValueType::STRING != str.data->type) ||
        (NULL == num.data) || (ValueType::NUMBER != num.data->type))
      DB_panic("Bad data type in leftstr.", context, lineNo);

   long count = BigInt::fromFloat(static_cast<NumericValue*>(num.data)->val);
   std::string result (static_cast<StringValue*>(str.data)->val);

   if (count < 0)
      DB_panic("Bad value in leftstr.", context, lineNo);

   if (static_cast<size_t>(count) > result.length()) return str;

   result.erase(count, std::string::npos);

   return ValueType::ValueHolder(new StringValue(result));
 }

ValueType::ValueHolder DB_rightstr (const ValueType::ValueHolder & str, const ValueType::ValueHolder & num,
   const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == str.data) || (ValueType::STRING != str.data->type) ||
        (NULL == num.data) || (ValueType::NUMBER != num.data->type))
      DB_panic("Bad data type in leftstr.", context, lineNo);

   long count = BigInt::fromFloat(static_cast<NumericValue*>(num.data)->val);
   std::string result (static_cast<StringValue*>(str.data)->val);

   if (count < 0)
      DB_panic("Bad value in rightstr.", context, lineNo);

   if (static_cast<size_t>(count) > result.length()) return str;

   result.erase(0, result.length() - count);

   return ValueType::ValueHolder(new StringValue(result));
 }

ValueType::ValueHolder DB_copysign (const ValueType::ValueHolder & from, const ValueType::ValueHolder & to,
   const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == from.data) || (ValueType::NUMBER != from.data->type) ||
        (NULL == to.data) || (ValueType::NUMBER != to.data->type))
      DB_panic("Bad data type in copysign.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(
      static_cast<NumericValue*>(to.data)->val.copySign(
         static_cast<NumericValue*>(from.data)->val ) ));
 }

ValueType::ValueHolder DB_stringstr (const ValueType::ValueHolder & str, const ValueType::ValueHolder & num,
   const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == str.data) || (ValueType::STRING != str.data->type) ||
        (NULL == num.data) || (ValueType::NUMBER != num.data->type))
      DB_panic("Bad data type in stringstr.", context, lineNo);

   long size = BigInt::fromFloat(static_cast<NumericValue*>(num.data)->val);

   std::string result;
   for (int i = 0; i < size; i++) result = result + static_cast<StringValue*>(str.data)->val;

   return ValueType::ValueHolder(new StringValue(result));
 }
