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
#include <stack>
#include <vector>

#include "StackOp.hpp"

void DB_panic (const std::string & msg, const StackFrame & stack, size_t lineNo) __attribute__ ((__noreturn__));

extern const DecFloat::Float DBTrue ("1.0000000"), DBFalse ("0.0000000");

static bool convertToBoolean(ValueType::ValueHolder arg, const StackFrame & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::ARRAY == arg.data->type) )
      DB_panic("Cannot convert null or array to boolean value.", context, lineNo);

   bool truth = false;
   switch (arg.data->type)
    {
      case ValueType::STRING:
         truth = 0 == static_cast<StringValue*>(arg.data)->val.size();
         break;
      case ValueType::NUMBER:
         truth = false == static_cast<NumericValue*>(arg.data)->val.isZero();
         break;
      default:
         break;
    }

   return truth;
 }

#define CHECK_UNARY(x) \
   if (0U == currentStack.size()) \
    { \
      DB_panic("INTERPRETER ERROR!!! : " x " empty stack.", context, \
         currentOp->lineNumber); \
    }

#define CHECK_BINARY(x) \
   if (2U > currentStack.size()) \
    { \
      DB_panic("INTERPRETER ERROR!!! : Not enough arguments for " x ".", context, \
         currentOp->lineNumber); \
    }

#define CHECK_TERNARY(x) \
   if (3U > currentStack.size()) \
    { \
      DB_panic("INTERPRETER ERROR!!! : Not enough arguments for " x ".", context, \
         currentOp->lineNumber); \
    }

#define MATH_OP_BOILERPLATE(x) \
   CHECK_BINARY(x) \
   first = currentStack.top(); currentStack.pop(); \
   second = currentStack.top(); \
   if ( (NULL == first.data) || (NULL == second.data) || \
        (ValueType::NUMBER != first.data->type) || \
        (ValueType::NUMBER != second.data->type) ) \
      DB_panic ("Type mismatch in " x ".", context, currentOp->lineNumber);

#define COMPARISON_OP_HEAD_BOILERPLATE(x) \
   CHECK_BINARY(x) \
   first = currentStack.top(); currentStack.pop(); \
   second = currentStack.top(); \
   if ( (NULL == first.data) || (NULL == second.data) || \
        (ValueType::ARRAY == first.data->type) || \
        (ValueType::ARRAY == second.data->type) ) \
      DB_panic ("Bad data type in " x ".", context, currentOp->lineNumber); \
   if (first.data->type != second.data->type) \
      DB_panic ("Type mismatch in " x ".", context, currentOp->lineNumber); \
   { \
      bool truth = false;

#define COMPARISON_OP_TAIL_BOILERPLATE \
      if (true == truth) \
         currentStack.top() = ValueType::ValueHolder(new NumericValue(DBTrue)); \
      else \
         currentStack.top() = ValueType::ValueHolder(new NumericValue(DBFalse)); \
   } \
   break;

ValueType::ValueHolder RecursiveInterpreter
   (const InstructionStream & instructions, StackFrame & context, bool dieIfNotReturning)
 {
   std::stack<ValueType::ValueHolder, std::vector<ValueType::ValueHolder> > currentStack;
   ValueType::ValueHolder first, second, third;

   for (size_t ip = 0; ip < instructions.size();)
    {
      StackOperation * currentOp = instructions[ip].first;

      switch (instructions[ip++].second)
       {
         case StackOperation::AND_OP:
            CHECK_BINARY("And Operation")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top();
            if (convertToBoolean(second, context, currentOp->lineNumber) &&
                convertToBoolean(first, context, currentOp->lineNumber))
               currentStack.top() = ValueType::ValueHolder(new NumericValue(DBTrue));
            else
               currentStack.top() = ValueType::ValueHolder(new NumericValue(DBFalse));
            break;
         case StackOperation::OR_OP:
            CHECK_BINARY("Or Operation")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top();
            if (convertToBoolean(second, context, currentOp->lineNumber) ||
                convertToBoolean(first, context, currentOp->lineNumber))
               currentStack.top() = ValueType::ValueHolder(new NumericValue(DBTrue));
            else
               currentStack.top() = ValueType::ValueHolder(new NumericValue(DBFalse));
            break;
         case StackOperation::EQUALITY:
            COMPARISON_OP_HEAD_BOILERPLATE("Equality Operation")
               switch (first.data->type)
                {
                  case ValueType::STRING:
                     truth = static_cast<StringValue*>(second.data)->val == static_cast<StringValue*>(first.data)->val;
                     break;
                  case ValueType::NUMBER:
                     truth = static_cast<NumericValue*>(second.data)->val == static_cast<NumericValue*>(first.data)->val;
                     break;
                  default:
                     break;
                }
            COMPARISON_OP_TAIL_BOILERPLATE
         case StackOperation::INEQUALITY:
            COMPARISON_OP_HEAD_BOILERPLATE("Inequality Operation")
               switch (first.data->type)
                {
                  case ValueType::STRING:
                     truth = static_cast<StringValue*>(second.data)->val != static_cast<StringValue*>(first.data)->val;
                     break;
                  case ValueType::NUMBER:
                     truth = static_cast<NumericValue*>(second.data)->val != static_cast<NumericValue*>(first.data)->val;
                     break;
                  default:
                     break;
                }
            COMPARISON_OP_TAIL_BOILERPLATE
         case StackOperation::GREATER_THAN:
            COMPARISON_OP_HEAD_BOILERPLATE("Greater Than Operation")
               switch (first.data->type)
                {
                  case ValueType::STRING:
                     truth = static_cast<StringValue*>(second.data)->val > static_cast<StringValue*>(first.data)->val;
                     break;
                  case ValueType::NUMBER:
                     truth = static_cast<NumericValue*>(second.data)->val > static_cast<NumericValue*>(first.data)->val;
                     break;
                  default:
                     break;
                }
            COMPARISON_OP_TAIL_BOILERPLATE
         case StackOperation::LESS_THAN:
            COMPARISON_OP_HEAD_BOILERPLATE("Less Than Operation")
               switch (first.data->type)
                {
                  case ValueType::STRING:
                     truth = static_cast<StringValue*>(second.data)->val < static_cast<StringValue*>(first.data)->val;
                     break;
                  case ValueType::NUMBER:
                     truth = static_cast<NumericValue*>(second.data)->val < static_cast<NumericValue*>(first.data)->val;
                     break;
                  default:
                     break;
                }
            COMPARISON_OP_TAIL_BOILERPLATE
         case StackOperation::GREATER_THAN_OR_EQUAL_TO:
            COMPARISON_OP_HEAD_BOILERPLATE("GEQ Operation")
               switch (first.data->type)
                {
                  case ValueType::STRING:
                     truth = static_cast<StringValue*>(second.data)->val >= static_cast<StringValue*>(first.data)->val;
                     break;
                  case ValueType::NUMBER:
                     truth = static_cast<NumericValue*>(second.data)->val >= static_cast<NumericValue*>(first.data)->val;
                     break;
                  default:
                     break;
                }
            COMPARISON_OP_TAIL_BOILERPLATE
         case StackOperation::LESS_THAN_OR_EQUAL_TO:
            COMPARISON_OP_HEAD_BOILERPLATE("LEQ Operation")
               switch (first.data->type)
                {
                  case ValueType::STRING:
                     truth = static_cast<StringValue*>(second.data)->val <= static_cast<StringValue*>(first.data)->val;
                     break;
                  case ValueType::NUMBER:
                     truth = static_cast<NumericValue*>(second.data)->val <= static_cast<NumericValue*>(first.data)->val;
                     break;
                  default:
                     break;
                }
            COMPARISON_OP_TAIL_BOILERPLATE
         case StackOperation::PLUS:
            MATH_OP_BOILERPLATE("Addition")
            currentStack.top() = ValueType::ValueHolder(new NumericValue(
               static_cast<NumericValue*>(second.data)->val + static_cast<NumericValue*>(first.data)->val ));
            break;
         case StackOperation::MINUS:
            MATH_OP_BOILERPLATE("Subtraction")
            currentStack.top() = ValueType::ValueHolder(new NumericValue(
               static_cast<NumericValue*>(second.data)->val - static_cast<NumericValue*>(first.data)->val ));
            break;
         case StackOperation::STRING_CAT:
            CHECK_BINARY("String Catenation")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top();
            if ( (NULL == first.data) || (NULL == second.data) ||
                 (ValueType::STRING != first.data->type) ||
                 (ValueType::STRING != second.data->type) )
               DB_panic ("Type mismatch in String Catenation.", context, currentOp->lineNumber);
            currentStack.top() = ValueType::ValueHolder(new StringValue(
               static_cast<StringValue*>(second.data)->val + static_cast<StringValue*>(first.data)->val ));
            break;
         case StackOperation::MULTIPLY:
            MATH_OP_BOILERPLATE("Multiply")
            currentStack.top() = ValueType::ValueHolder(new NumericValue(
               static_cast<NumericValue*>(second.data)->val * static_cast<NumericValue*>(first.data)->val ));
            break;
         case StackOperation::DIVIDE:
            MATH_OP_BOILERPLATE("Divide")
            currentStack.top() = ValueType::ValueHolder(new NumericValue(
               static_cast<NumericValue*>(second.data)->val / static_cast<NumericValue*>(first.data)->val ));
            break;
         case StackOperation::REMAINDER:
            MATH_OP_BOILERPLATE("Remainder")
            currentStack.top() = ValueType::ValueHolder(new NumericValue(
               static_cast<NumericValue*>(second.data)->val % static_cast<NumericValue*>(first.data)->val ));
            break;
         case StackOperation::POWER:
            MATH_OP_BOILERPLATE("Exponentiation")
            currentStack.top() = ValueType::ValueHolder(new NumericValue(DecFloat::pow(
               static_cast<NumericValue*>(second.data)->val, static_cast<NumericValue*>(first.data)->val )));
            break;
         case StackOperation::NOT:
            CHECK_UNARY("Not Operation with")
            if (true == convertToBoolean(currentStack.top(), context, currentOp->lineNumber))
               currentStack.top() = ValueType::ValueHolder(new NumericValue(DBFalse));
            else
               currentStack.top() = ValueType::ValueHolder(new NumericValue(DBTrue));
            break;
         case StackOperation::ABS:
            CHECK_UNARY("Absolute Value with")
            first = currentStack.top();
            if ( (NULL == first.data) || (ValueType::NUMBER != first.data->type) )
               DB_panic ("Bad data type in Absolute Value.", context, currentOp->lineNumber);
            currentStack.top() =
               ValueType::ValueHolder(new NumericValue( static_cast<NumericValue*>(first.data)->val.abs() ));
            break;
         case StackOperation::NEGATE:
            CHECK_UNARY("Negate with")
            first = currentStack.top();
            if ( (NULL == first.data) || (ValueType::NUMBER != first.data->type) )
               DB_panic ("Bad data type in Negate.", context, currentOp->lineNumber);
            currentStack.top() =
               ValueType::ValueHolder(new NumericValue( static_cast<NumericValue*>(first.data)->val.negate() ));
            break;
         case StackOperation::FORCE_LOGICAL:
            CHECK_UNARY("Force Logical with")
            if (true == convertToBoolean(currentStack.top(), context, currentOp->lineNumber))
               currentStack.top() = ValueType::ValueHolder(new NumericValue(DBTrue));
            else
               currentStack.top() = ValueType::ValueHolder(new NumericValue(DBFalse));
            break;
         case StackOperation::CONSTANT:
          {
            Constant * con = static_cast<Constant*>(currentOp);
            currentStack.push(con->value);
          }
            break;
         case StackOperation::LOAD_GLOBAL_VARIABLE:
          {
            LdGlobalVar * var = static_cast<LdGlobalVar*>(currentOp);
            currentStack.push(context.Globals[var->index]);
          }
            break;
         case StackOperation::LOAD_STATIC_VARIABLE:
          {
            LdStaticVar * var = static_cast<LdStaticVar*>(currentOp);
            currentStack.push(context.Statics[var->index]);
          }
            break;
         case StackOperation::LOAD_LOCAL_VARIABLE:
          {
            LdLocalVar * var = static_cast<LdLocalVar*>(currentOp);
            currentStack.push(context.Locals[var->index]);
          }
            break;
         case StackOperation::STORE_GLOBAL_VARIABLE:
            CHECK_UNARY("Store Global Variable with")
          {
            StGlobalVar * var = static_cast<StGlobalVar*>(currentOp);
            context.Globals[var->index] = currentStack.top(); currentStack.pop();
          }
            break;
         case StackOperation::STORE_STATIC_VARIABLE:
            CHECK_UNARY("Store Static Variable with")
          {
            StStaticVar * var = static_cast<StStaticVar*>(currentOp);
            context.Statics[var->index] = currentStack.top(); currentStack.pop();
          }
            break;
         case StackOperation::STORE_LOCAL_VARIABLE:
            CHECK_UNARY("Store Local Variable with")
          {
            StLocalVar * var = static_cast<StLocalVar*>(currentOp);
            context.Locals[var->index] = currentStack.top(); currentStack.pop();
          }
            break;
         case StackOperation::LOAD_INDIRECT:
            CHECK_BINARY("Load Indirect")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top();
            if ( (NULL == second.data) || (NULL == first.data) ||
                 (ValueType::ARRAY != second.data->type) ||
                 (ValueType::NUMBER != first.data->type) )
               DB_panic ("Bad data type in Load Indirect.", context, currentOp->lineNumber);
            currentStack.top() =
               static_cast<ArrayValue*>(second.data)->getIndex(fromFloat(static_cast<NumericValue*>(first.data)->val));
            break;
         case StackOperation::STORE_INDIRECT:
            CHECK_TERNARY("Store Indirect")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top(); currentStack.pop();
            third = currentStack.top();
            if ( (NULL == third.data) || (NULL == second.data) ||
                 (ValueType::ARRAY != third.data->type) ||
                 (ValueType::NUMBER != second.data->type) )
               DB_panic ("Bad data type in Store Indirect : this should not happen.", context, currentOp->lineNumber);
            static_cast<ArrayValue*>(third.data)->setIndex(fromFloat(static_cast<NumericValue*>(second.data)->val), first);
            currentStack.top() = third;
            break;
         case StackOperation::FUNCTION_CALL:
          {
            FunCall * fun = static_cast<FunCall*>(currentOp);
            if (currentStack.size() < fun->nargs)
             {
               DB_panic("INTERPRETER ERROR!!! : Not enough arguments for call to \"" +
                  context.FunctionNames[fun->index] + "\".", context, currentOp->lineNumber);
             }

            StackFrame newFrame (context, fun->index, fun->lineNumber);

            newFrame.Locals.resize(fun->nargs + newFrame.FunLocals[fun->index].size());

            for (size_t i = 0; fun->nargs > i; ++i)
             {
               if (0 != i) currentStack.pop();
               newFrame.Locals[fun->nargs - 1 - i] = currentStack.top();
             }

            for (size_t i = 0; newFrame.FunLocals[fun->index].size() > i; ++i)
             {
               newFrame.Locals[fun->nargs + i] = newFrame.FunLocals[fun->index][i];
             }

            if (0 == fun->nargs)
               currentStack.push(ValueType::ValueHolder());

            currentStack.top() = RecursiveInterpreter(newFrame.Functions[fun->index], newFrame, true);
          }
            break;
         case StackOperation::STANDARD_CONSTANT_FUNCTION:
          {
            StdConstFun * con = static_cast<StdConstFun*>(currentOp);
            currentStack.push(con->function());
          }
             break;
         case StackOperation::STANDARD_UNARY_FUNCTION:
            CHECK_UNARY("Standard Unary Function with")
            first = currentStack.top();
          {
            StdUnaryFun * con = static_cast<StdUnaryFun*>(currentOp);
            currentStack.top() = con->function(first, context, con->lineNumber);
          }
             break;
         case StackOperation::STANDARD_BINARY_FUNCTION:
            CHECK_BINARY("Standard Binary Function")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top();
          {
            StdBinaryFun * con = static_cast<StdBinaryFun*>(currentOp);
            currentStack.top() = con->function(second, first, context, con->lineNumber);
          }
             break;
         case StackOperation::STANDARD_TERNARY_FUNCTION:
            CHECK_TERNARY("Standard Ternary Function")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top(); currentStack.pop();
            third = currentStack.top();
          {
            StdTernaryFun * con = static_cast<StdTernaryFun*>(currentOp);
            currentStack.top() = con->function(third, second, first, context, con->lineNumber);
          }
             break;
         case StackOperation::COPY:
            CHECK_UNARY("Copying")
            currentStack.push(currentStack.top());
            break;
         case StackOperation::ROTATE:
            CHECK_TERNARY("Rotate")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top(); currentStack.pop();
            third = currentStack.top();
            currentStack.top() = first;
            currentStack.push(third);
            currentStack.push(second);
            break;
         case StackOperation::SWAP:
            CHECK_BINARY("Swap")
            first = currentStack.top(); currentStack.pop();
            second = currentStack.top();
            currentStack.top() = first;
            currentStack.push(second);
            break;
         case StackOperation::POP:
            CHECK_UNARY("Popping")
            currentStack.pop();
            break;
         case StackOperation::JUMP:
          {
            IndexedStackOperation * temp = static_cast<IndexedStackOperation*>(currentOp);
            ip = temp->index;
          }
            break;
         case StackOperation::BRANCH:
            CHECK_UNARY("Branching")
            first = currentStack.top(); currentStack.pop();
            if (false == convertToBoolean(first, context, currentOp->lineNumber))
             {
               IndexedStackOperation * temp = static_cast<IndexedStackOperation*>(currentOp);
               ip = temp->index;
             }
            break;
         case StackOperation::RETURN:
            if (1U != currentStack.size())
             {
               DB_panic("INTERPRETER ERROR!!! : Return with stack size not 1.", context,
                  currentOp->lineNumber);
             }
            return currentStack.top();
            break;
         case StackOperation::TAILCALL:
          {
            TailCall * fun = static_cast<TailCall*>(currentOp);
            if (currentStack.size() != fun->nargs)
             {
               DB_panic("INTERPRETER ERROR!!! : Incorrect arguments for tailcall to \"" +
                  context.FunctionNames[context.FunctionIndex] + "\".", context, currentOp->lineNumber);
             }

            for (size_t i = 0; fun->nargs > i; ++i)
             {
               context.Locals[fun->nargs - 1 - i] = currentStack.top(); currentStack.pop();
             }

            for (size_t i = 0; context.FunLocals[context.FunctionIndex].size() > i; ++i)
             {
               context.Locals[fun->nargs + i] = context.FunLocals[context.FunctionIndex][i];
             }

            ip = 0U;
          }
            break;
       }
    }

   if (true == dieIfNotReturning)
    {
      DB_panic("Function \"" + context.FunctionNames[context.FunctionIndex] + " never returned a value.",
         context, context.ParentLineNumber);
    }
   if (1U != currentStack.size())
    {
      DB_panic("INTERPRETER ERROR!!! : Stack incorrect size for return.", context,
         context.ParentLineNumber);
    }
   return currentStack.top();
 }
