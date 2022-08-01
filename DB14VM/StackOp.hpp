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

#include "BaseStackOp.hpp"
#include "ValueType.hpp"
#include "SymbolTable.hpp"

#define STANDARD_OPCODE_DEFINE(x,y) \
   class x : public StackOperation \
    { \
         x (const x & src) : StackOperation(src) { } \
      public: \
         x (size_t lineNo = 0U) : StackOperation (y, lineNo) { } \
         x * Clone (void) { return new x(*this); } \
    };

STANDARD_OPCODE_DEFINE(AndOp, AND_OP)
STANDARD_OPCODE_DEFINE(OrOp, OR_OP)
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
STANDARD_OPCODE_DEFINE(ForceLogical, FORCE_LOGICAL)
STANDARD_OPCODE_DEFINE(LDI, LOAD_INDIRECT)
STANDARD_OPCODE_DEFINE(STI, STORE_INDIRECT)
STANDARD_OPCODE_DEFINE(Copy, COPY)
STANDARD_OPCODE_DEFINE(Rot, ROTATE)
STANDARD_OPCODE_DEFINE(Swap, SWAP)
STANDARD_OPCODE_DEFINE(Pop, POP)
STANDARD_OPCODE_DEFINE(Return, RETURN)

#undef STANDARD_OPCODE_DEFINE

class IndexedStackOperation : public StackOperation
 {
   public:
      size_t index;
   protected:
      IndexedStackOperation(TYPE type, size_t lineNo) : StackOperation (type, lineNo) { }
      IndexedStackOperation(const IndexedStackOperation & src) : StackOperation(src), index(src.index) { }
 };

#define STANDARD_OPCODE_DEFINE(x,y) \
   class x : public IndexedStackOperation \
    { \
         x (const x & src) : IndexedStackOperation(src) { } \
      public: \
         x (size_t lineNo = 0U) : IndexedStackOperation (y, lineNo) { } \
         x * Clone (void) { return new x(*this); } \
    };

STANDARD_OPCODE_DEFINE(LdGlobalVar, LOAD_GLOBAL_VARIABLE)
STANDARD_OPCODE_DEFINE(LdStaticVar, LOAD_STATIC_VARIABLE)
STANDARD_OPCODE_DEFINE(LdLocalVar, LOAD_LOCAL_VARIABLE)
STANDARD_OPCODE_DEFINE(StGlobalVar, STORE_GLOBAL_VARIABLE)
STANDARD_OPCODE_DEFINE(StStaticVar, STORE_STATIC_VARIABLE)
STANDARD_OPCODE_DEFINE(StLocalVar, STORE_LOCAL_VARIABLE)
STANDARD_OPCODE_DEFINE(Jump, JUMP)
STANDARD_OPCODE_DEFINE(Branch, BRANCH)

#undef STANDARD_OPCODE_DEFINE

class FunCall : public StackOperation
 {
      FunCall (const FunCall & src) : StackOperation(src), index(src.index), nargs(src.nargs) { }
   public:
      size_t index;
      size_t nargs;
      FunCall (size_t lineNo = 0U) : StackOperation (FUNCTION_CALL, lineNo) { }
      FunCall * Clone (void) { return new FunCall(*this); }
 };

class TailCall : public StackOperation
 {
      TailCall (const TailCall & src) : StackOperation(src), nargs(src.nargs) { }
   public:
      size_t nargs;
      TailCall (size_t lineNo = 0U) : StackOperation (TAILCALL, lineNo) { }
      TailCall * Clone (void) { return new TailCall(*this); }
 };

class Constant : public StackOperation
 {
      Constant (const Constant & src) : StackOperation(src), value(src.value) { }
   public:
      ValueType::ValueHolder value;

      Constant (size_t lineNo = 0U) : StackOperation (CONSTANT, lineNo) { }
      Constant * Clone (void) { return new Constant(*this); }
 };

class StdConstFun : public StackOperation
 {
      StdConstFun (const StdConstFun & src) : StackOperation(src), function(src.function) { }
   public:
      ConstantFunctionPointer function;

      StdConstFun (size_t lineNo = 0U) : StackOperation (STANDARD_CONSTANT_FUNCTION, lineNo) { }
      StdConstFun * Clone (void) { return new StdConstFun(*this); }
 };

class StdUnaryFun : public StackOperation
 {
      StdUnaryFun (const StdUnaryFun & src) : StackOperation(src), function(src.function) { }
   public:
      UnaryFunctionPointer function;

      StdUnaryFun (size_t lineNo = 0U) : StackOperation (STANDARD_UNARY_FUNCTION, lineNo) { }
      StdUnaryFun * Clone (void) { return new StdUnaryFun(*this); }
 };

class StdBinaryFun : public StackOperation
 {
      StdBinaryFun (const StdBinaryFun & src) : StackOperation(src), function(src.function) { }
   public:
      BinaryFunctionPointer function;

      StdBinaryFun (size_t lineNo = 0U) : StackOperation (STANDARD_BINARY_FUNCTION, lineNo) { }
      StdBinaryFun * Clone (void) { return new StdBinaryFun(*this); }
 };

class StdTernaryFun : public StackOperation
 {
      StdTernaryFun (const StdTernaryFun & src) : StackOperation(src), function(src.function) { }
   public:
      TernaryFunctionPointer function;

      StdTernaryFun (size_t lineNo = 0U) : StackOperation (STANDARD_TERNARY_FUNCTION, lineNo) { }
      StdTernaryFun * Clone (void) { return new StdTernaryFun(*this); }
 };

#endif /* STACKOP_HPP */
