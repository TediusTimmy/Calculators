/*
Copyright (c) 2014 Thomas DiModica.
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
#ifndef STACKOP_HPP
#define STACKOP_HPP

#include "ValueType.hpp"
#include "SymbolTable.hpp"

class StackOperation
 {
   public:
      enum TYPE
       {
         AND_OP,
         OR_OP,
         SHORT_OR,
         SHORT_AND,
         EQUALITY,
         INEQUALITY,
         GREATER_THAN,
         LESS_THAN,
         GREATER_THAN_OR_EQUAL_TO,
         LESS_THAN_OR_EQUAL_TO,
         PLUS,
         MINUS,
         STRING_CAT,
         MULTIPLY,
         DIVIDE,
         REMAINDER,
         POWER,
         NOT,
         ABS,
         NEGATE,
         CONSTANT,
         GLOBAL_VARIABLE,
         STATIC_VARIABLE,
         LOCAL_VARIABLE,
         LOAD_INDIRECT,
         FUNCTION_CALL,
         STANDARD_CONSTANT_FUNCTION,
         STANDARD_UNARY_FUNCTION,
         STANDARD_BINARY_FUNCTION,
         STANDARD_TERNARY_FUNCTION,
         STORE_INDIRECT,
         COPY,
         ROTATE,
         SWAP,
         POP,
         JUMP,
         BRANCH,
         RETURN,
         TAILCALL
       };

      const TYPE type;
      size_t lineNumber;

      virtual ~StackOperation() { }

      static bool convertToBoolean(ValueType::ValueHolder, const CallingContext &, size_t);

   protected:
      StackOperation(TYPE type) : type(type), lineNumber(0U) { }
 };

#define STANDARD_OPCODE_DEFINE (x, y) \
   class x : public StackOperation \
    { \
      public: \
         x () : StackOperation (y) { } \
    };

STANDARD_OPCODE_DEFINE(AndOp, AND_OP)
STANDARD_OPCODE_DEFINE(OrOp, OR_OP)
STANDARD_OPCODE_DEFINE(ShortAnd, SHORT_AND)
STANDARD_OPCODE_DEFINE(ShortOr, SHORT_OR)
STANDARD_OPCODE_DEFINE(Equals, EQUALITY)
STANDARD_OPCODE_DEFINE(NotEquals, INEQUALITY)
STANDARD_OPCODE_DEFINE(Greater, GREATER_THAN)
STANDARD_OPCODE_DEFINE(Less, LESS_THAN)
STANDARD_OPCODE_DEFINE(GEQ, GREATER_THAN_OR_EQUAL_TO)
STANDARD_OPCODE_DEFINE(LEQ, LESS_THAN_OR_EQUAL_TO)
STANDARD_OPCODE_DEFINE(Plus, PLUS)
STANDARD_OPCODE_DEFINE(Minus, MINUS)
STANDARD_OPCODE_DEFINE(StringCat, STRING_CAT)
STANDARD_OPCODE_DEFINE(Multiply, MULTIPLY)
STANDARD_OPCODE_DEFINE(Divide, DIVIDE)
STANDARD_OPCODE_DEFINE(Remainder, REMAINDER)
STANDARD_OPCODE_DEFINE(Power, POWER)
STANDARD_OPCODE_DEFINE(Not, NOT)
STANDARD_OPCODE_DEFINE(Abs, ABS)
STANDARD_OPCODE_DEFINE(Negate, NEGATE)
STANDARD_OPCODE_DEFINE(LDI, LOAD_INDIRECT)
STANDARD_OPCODE_DEFINE(STI, STORE_INDIRECT)
STANDARD_OPCODE_DEFINE(Copy, COPY)
STANDARD_OPCODE_DEFINE(Rot, ROTATE)
STANDARD_OPCODE_DEFINE(Swap, SWAP)
STANDARD_OPCODE_DEFINE(Pop, POP)
STANDARD_OPCODE_DEFINE(Return, RETURN)
STANDARD_OPCODE_DEFINE(TailCall, TAILCALL)

#undef STANDARD_OPCODE_DEFINE

#define STANDARD_OPCODE_DEFINE (x, y) \
   class x : public StackOperation \
    { \
      public: \
         size_t index; \
         x () : StackOperation (y) { } \
    };

STANDARD_OPCODE_DEFINE(GlobalVar, GLOBAL_VARIABLE)
STANDARD_OPCODE_DEFINE(StaticVar, STATIC_VARIABLE)
STANDARD_OPCODE_DEFINE(LocalVar, LOCAL_VARIABLE)
STANDARD_OPCODE_DEFINE(Jump, JUMP)
STANDARD_OPCODE_DEFINE(Branch, BRANCH)

#undef STANDARD_OPCODE_DEFINE

class FunCall : public StackOperation
 {
   public:
      size_t index;
      size_t nargs;
      FunCall () : StackOperation (FUNCTION_CALL) { }
 };

class Constant : public StackOperation
 {
   public:
      ValueType::ValueHolder value;

      Constant () : StackOperation (CONSTANT) { }
 };

class StdConstantFun : public StackOperation
 {
   public:
      ConstantFunctionPointer function;

      StdConstantFun () : StackOperation (STANDARD_CONSTANT_FUNCTION) { }
 };

class StdUnaryFun : public StackOperation
 {
   public:
      UnaryFunctionPointer function;

      StdUnaryFun () : StackOperation (STANDARD_UNARY_FUNCTION) { }
 };

class StdBinaryFun : public StackOperation
 {
   public:
      BinaryFunctionPointer function;

      StdBinaryFun () : StackOperation (STANDARD_BINARY_FUNCTION) { }
 };

class StdTernaryFun : public StackOperation
 {
   public:
      TernaryFunctionPointer function;

      StdTernaryFun () : StackOperation (STANDARD_TERNARY_FUNCTION) { }
 };

#endif /* STACKOP_HPP */
