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
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "ValueType.hpp"
#include "SymbolTable.hpp"
#include <string>
#include <vector>

class Expression
 {
   public:
      size_t lineNo;

       /* CallingContext can't be const, because if we propogate it
          to a function call, the function call is allowed to modify it. */
      virtual ValueType::ValueHolder evaluate (CallingContext &) const = 0;
      virtual ~Expression() { }

      static bool convertToBoolean(ValueType::ValueHolder, const CallingContext &, size_t);
 };

class Constant : public Expression
 {
   public:
      ValueType::ValueHolder value;

      ValueType::ValueHolder evaluate (CallingContext &) const { return value; }
 };

class Variable : public Expression
 {
   public:
      std::string referent;

      ValueType::ValueHolder evaluate (CallingContext & context) const { return context.getValue(referent, lineNo); }
 };

class StandardConstantFunction : public Expression
 {
   public:
      ConstantFunctionPointer function;

      ValueType::ValueHolder evaluate (CallingContext &) const { return function(); }
 };

/*
 ** AND_OP
 ** OR_OP
 ** SHORT_OR
 ** SHORT_AND
 ** EQUALITY
 ** INEQUALITY
 ** GREATER_THAN
 ** LESS_THAN
 ** GREATER_THAN_OR_EQUAL_TO
 ** LESS_THAN_OR_EQUAL_TO
 ** PLUS
 ** MINUS
 ** STRING_CAT
 ** MULTIPLY
 ** DIVIDE
 ** POWER
 ** NOT
 ** ABS
 ** NEGATE

 ** CONSTANT
 ** VARIABLE (symbolic constant)
 ** DEREFERENCED_VARIABLE
 ** FUNCTION_CALL
 */

class BinaryOperation : public Expression
 {
   public:
      Expression * lhs, * rhs;

      ValueType::ValueHolder evaluate (CallingContext &) const = 0;
      BinaryOperation() : lhs(NULL), rhs(NULL) { }
      ~BinaryOperation()
       {
         if (NULL != lhs) { delete lhs; lhs = NULL; }
         if (NULL != rhs) { delete rhs; rhs = NULL; }
       }
 };

class Plus : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Minus : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class StringCat : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Multiply : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Divide : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Power : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class AndOp : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class OrOp : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class ShortAnd : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class ShortOr : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Equals : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class NotEquals : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Greater : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Less : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class GEQ : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class LEQ : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class DerefVar : public BinaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class StandardBinaryFunction : public BinaryOperation
 {
   public:
      BinaryFunctionPointer function;
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };



class UnaryOperation : public Expression
 {
   public:
      Expression * arg;
      ValueType::ValueHolder evaluate (CallingContext &) const = 0;
      UnaryOperation() : arg(NULL) { }
      ~UnaryOperation()
       {
         if (NULL != arg) { delete arg; arg = NULL; }
       }
 };

class Not : public UnaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Abs : public UnaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class Negate : public UnaryOperation
 {
   public:
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };

class StandardUnaryFunction : public UnaryOperation
 {
   public:
      UnaryFunctionPointer function;
      ValueType::ValueHolder evaluate (CallingContext &) const;
 };



class FunctionCall : public Expression
 {
   public:
      std::vector<Expression*> args;
      std::string name;

      ValueType::ValueHolder evaluate (CallingContext &) const;
      ~FunctionCall()
       {
         for (std::vector<Expression*>::iterator iter = args.begin();
            iter != args.end(); ++iter)
          {
            delete *iter;
          }
       }
 };

class StandardTernaryFunction : public Expression
 {
   public:
      Expression * first, * second, * third;

      TernaryFunctionPointer function;
      ValueType::ValueHolder evaluate (CallingContext &) const;
      StandardTernaryFunction() : first(NULL), second(NULL), third(NULL) { }
      ~StandardTernaryFunction()
       {
         if (NULL != first) { delete first; first = NULL; }
         if (NULL != second) { delete second; second = NULL; }
         if (NULL != third) { delete third; third = NULL; }
       }
 };

#endif /* EXPRESSION_HPP */
