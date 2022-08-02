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
#include "SymbolTable.hpp"
#include "StdLib.hpp"

void DB_panic (const std::string &) __attribute__ ((__noreturn__));
void DB_panic (const std::string &, const CallingContext &, size_t) __attribute__ ((__noreturn__));

ValueType::ValueHolder CallingContext::getValue (const std::string & name, size_t lineNo) const
 {
   std::map<std::string, ValueType::ValueHolder>::const_iterator test;

   test = m_constants.find(name);
   if (m_constants.end() != test) return test->second;

   test = m_locals.find(name);
   if (m_locals.end() != test) return test->second;

   test = m_statics.find(name);
   if (m_statics.end() != test) return test->second;

   test = m_globals.find(name);
   if (m_globals.end() != test) return test->second;

   DB_panic("INTERPRETER ERROR!!! : getValue to non-existent variable", *this, lineNo);
 }

void CallingContext::setValue (const std::string & name, const ValueType::ValueHolder & value, size_t lineNo)
 {
   std::map<std::string, ValueType::ValueHolder>::iterator test;

   test = m_locals.find(name);
   if (m_locals.end() != test)
    {
      test->second = value;
      return;
    }

   test = m_statics.find(name);
   if (m_statics.end() != test)
    {
      test->second = value;
      return;
    }

   test = m_globals.find(name);
   if (m_globals.end() != test)
    {
      test->second = value;
      return;
    }

   DB_panic("INTERPRETER ERROR!!! : setValue to non-existent variable", *this, lineNo);
 }


      /*
         Pass in the vector.
         If the vector is not empty:
            Create a new vector whose contents are the current vector without its first item.
            Create an ArrayValue of size first item. (vector[0])
            If the new vector is not empty:
               For each index in the array:
                  Assign it with a call to createVariable passing in the new vector.
            Return the ArrayValue.
         Else return a NULL variable.
      */
ValueType::ValueHolder CallingContext::createVariable(const std::vector<long> & indexes) const
 {
   if (0 != indexes.size())
    {
      std::vector<long> newVec;
      for (size_t i = 1; i < indexes.size(); ++i) newVec.push_back(indexes[i]);

      ArrayValue * newArray = new ArrayValue(indexes[0]);

      if (0 != newVec.size())
       {
         for (size_t i = 0; i < static_cast<size_t>(indexes[0]); ++i)
          {
            newArray->setIndex(i, createVariable(newVec));
          }
       }

      return newArray;
    }
   return ValueType::ValueHolder();
 }

ConstantFunctionPointer CallingContext::getConstantFunction (const std::string & name)
 {
   std::map<std::string, ConstantFunctionPointer>::iterator test (s_constantFunctions.find(name));
   if (s_constantFunctions.end() != test) return test->second;

   DB_panic("INTERPRETER ERROR!!! : request for non existent standard function \"" + name + "\".");
 }

UnaryFunctionPointer CallingContext::getUnaryFunction (const std::string & name)
 {
   std::map<std::string, UnaryFunctionPointer>::iterator test (s_unaryFunctions.find(name));
   if (s_unaryFunctions.end() != test) return test->second;

   DB_panic("INTERPRETER ERROR!!! : request for non existent standard function \"" + name + "\".");
 }

BinaryFunctionPointer CallingContext::getBinaryFunction (const std::string & name)
 {
   std::map<std::string, BinaryFunctionPointer>::iterator test (s_binaryFunctions.find(name));
   if (s_binaryFunctions.end() != test) return test->second;

   DB_panic("INTERPRETER ERROR!!! : request for non existent standard function \"" + name + "\".");
 }

TernaryFunctionPointer CallingContext::getTernaryFunction (const std::string & name)
 {
   std::map<std::string, TernaryFunctionPointer>::iterator test (s_ternaryFunctions.find(name));
   if (s_ternaryFunctions.end() != test) return test->second;

   DB_panic("INTERPRETER ERROR!!! : request for non existent standard function \"" + name + "\".");
 }

static std::map<std::string, ConstantFunctionPointer> createConstantFunctionsMap (void)
 {
   std::map<std::string, ConstantFunctionPointer> result;

   result.insert(std::make_pair("date", DB_date));
   result.insert(std::make_pair("time", DB_time));
   result.insert(std::make_pair("getround", DB_getround));
   result.insert(std::make_pair("instr", DB_instr));
   result.insert(std::make_pair("inchr", DB_inchr));
   result.insert(std::make_pair("pi", DB_pi));
   result.insert(std::make_pair("rand", DB_rand));
   result.insert(std::make_pair("eolstr", DB_eolstr));

   return result;
 }

static std::map<std::string, UnaryFunctionPointer> createUnaryFunctionsMap (void)
 {
   std::map<std::string, UnaryFunctionPointer> result;

   result.insert(std::make_pair("sin", DB_sin));
   result.insert(std::make_pair("cos", DB_cos));
   result.insert(std::make_pair("tan", DB_tan));
   result.insert(std::make_pair("asin", DB_asin));
   result.insert(std::make_pair("acos", DB_acos));
   result.insert(std::make_pair("atan", DB_atan));
   result.insert(std::make_pair("str", DB_str));
   result.insert(std::make_pair("val", DB_val));
   result.insert(std::make_pair("log", DB_log));
   result.insert(std::make_pair("exp", DB_exp));
   result.insert(std::make_pair("sqrt", DB_sqrt));
   result.insert(std::make_pair("floor", DB_floor));
   result.insert(std::make_pair("setround", DB_setround));
   result.insert(std::make_pair("print", DB_print));
   result.insert(std::make_pair("ucase", DB_ucase));
   result.insert(std::make_pair("lcase", DB_lcase));
   result.insert(std::make_pair("chr", DB_chr));
   result.insert(std::make_pair("asc", DB_asc));
   result.insert(std::make_pair("neg", DB_neg));
   result.insert(std::make_pair("sign", DB_sign));
   result.insert(std::make_pair("ltrim", DB_ltrim));
   result.insert(std::make_pair("rtrim", DB_rtrim));
   result.insert(std::make_pair("spacestr", DB_spacestr));
   result.insert(std::make_pair("isstr", DB_isstr));
   result.insert(std::make_pair("isval", DB_isval));
   result.insert(std::make_pair("isarray", DB_isarray));
   result.insert(std::make_pair("isnull", DB_isnull));
   result.insert(std::make_pair("alloc", DB_alloc));
   result.insert(std::make_pair("len", DB_len));
   result.insert(std::make_pair("prec", DB_prec));
   result.insert(std::make_pair("size", DB_size));
   result.insert(std::make_pair("isinf", DB_isinf));
   result.insert(std::make_pair("isnan", DB_isnan));
   result.insert(std::make_pair("die", DB_die));
   result.insert(std::make_pair("eval", DB_eval));
   result.insert(std::make_pair("define", DB_define));

   return result;
 }

static std::map<std::string, BinaryFunctionPointer> createBinaryFunctionsMap (void)
 {
   std::map<std::string, BinaryFunctionPointer> result;

   result.insert(std::make_pair("atan2", DB_atan2));
   result.insert(std::make_pair("pow", DB_pow));
   result.insert(std::make_pair("reprec", DB_reprec));
   result.insert(std::make_pair("resize", DB_resize));
   result.insert(std::make_pair("leftstr", DB_leftstr));
   result.insert(std::make_pair("rightstr", DB_rightstr));
   result.insert(std::make_pair("copysign", DB_copysign));
   result.insert(std::make_pair("stringstr", DB_stringstr));

   return result;
 }

static std::map<std::string, TernaryFunctionPointer> createTernaryFunctionsMap (void)
 {
   std::map<std::string, TernaryFunctionPointer> result;

   result.insert(std::make_pair("midstr", DB_midstr));

   return result;
 }

std::map<std::string, ConstantFunctionPointer> CallingContext::s_constantFunctions = createConstantFunctionsMap();
std::map<std::string, UnaryFunctionPointer> CallingContext::s_unaryFunctions = createUnaryFunctionsMap();
std::map<std::string, BinaryFunctionPointer> CallingContext::s_binaryFunctions = createBinaryFunctionsMap();
std::map<std::string, TernaryFunctionPointer> CallingContext::s_ternaryFunctions = createTernaryFunctionsMap();
