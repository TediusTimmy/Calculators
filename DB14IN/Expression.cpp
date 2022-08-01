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
#include "Expression.hpp"
#include "Statement.hpp"
#include "RunTimeFlowControl.hpp"

void DB_panic (const std::string &, const CallingContext &, size_t) __attribute__ ((__noreturn__));

extern const DecFloat::Float DBTrue ("1.0000000"), DBFalse ("0.0000000");

bool Expression::convertToBoolean(ValueType::ValueHolder arg, const CallingContext & context, size_t lineNo)
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

ValueType::ValueHolder Plus::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::NUMBER != Lhs.data->type) ||
        (ValueType::NUMBER != Rhs.data->type) )
      DB_panic ("Type mismatch in addition.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(
      static_cast<NumericValue*>(Lhs.data)->val + static_cast<NumericValue*>(Rhs.data)->val ));
 }

ValueType::ValueHolder Minus::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::NUMBER != Lhs.data->type) ||
        (ValueType::NUMBER != Rhs.data->type) )
      DB_panic ("Type mismatch in subtraction.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(
      static_cast<NumericValue*>(Lhs.data)->val - static_cast<NumericValue*>(Rhs.data)->val ));
 }

ValueType::ValueHolder StringCat::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::STRING != Lhs.data->type) ||
        (ValueType::STRING != Rhs.data->type) )
      DB_panic ("Type mismatch in string concatenation.", context, lineNo);

   return ValueType::ValueHolder(new StringValue(
      static_cast<StringValue*>(Lhs.data)->val + static_cast<StringValue*>(Rhs.data)->val ));
 }

ValueType::ValueHolder Multiply::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::NUMBER != Lhs.data->type) ||
        (ValueType::NUMBER != Rhs.data->type) )
      DB_panic ("Type mismatch in multiplication.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(
      static_cast<NumericValue*>(Lhs.data)->val * static_cast<NumericValue*>(Rhs.data)->val ));
 }

ValueType::ValueHolder Divide::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::NUMBER != Lhs.data->type) ||
        (ValueType::NUMBER != Rhs.data->type) )
      DB_panic ("Type mismatch in division.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(
      static_cast<NumericValue*>(Lhs.data)->val / static_cast<NumericValue*>(Rhs.data)->val ));
 }

ValueType::ValueHolder Remainder::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::NUMBER != Lhs.data->type) ||
        (ValueType::NUMBER != Rhs.data->type) )
      DB_panic ("Type mismatch in remainder.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(
      static_cast<NumericValue*>(Lhs.data)->val % static_cast<NumericValue*>(Rhs.data)->val ));
 }

ValueType::ValueHolder Power::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::NUMBER != Lhs.data->type) ||
        (ValueType::NUMBER != Rhs.data->type) )
      DB_panic ("Type mismatch in exponentiation.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue(DecFloat::pow(
      static_cast<NumericValue*>(Lhs.data)->val, static_cast<NumericValue*>(Rhs.data)->val )));
 }

ValueType::ValueHolder AndOp::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if (convertToBoolean(Lhs, context, lineNo) && convertToBoolean(Rhs, context, lineNo))
      return ValueType::ValueHolder(new NumericValue(DBTrue));

   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder OrOp::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if (convertToBoolean(Lhs, context, lineNo) || convertToBoolean(Rhs, context, lineNo))
      return ValueType::ValueHolder(new NumericValue(DBTrue));

   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder ShortAnd::evaluate (CallingContext & context) const
 {
   if (convertToBoolean(lhs->evaluate(context), context, lineNo) &&
       convertToBoolean(rhs->evaluate(context), context, lineNo))
      return ValueType::ValueHolder(new NumericValue(DBTrue));

   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder ShortOr::evaluate (CallingContext & context) const
 {
   if (convertToBoolean(lhs->evaluate(context), context, lineNo) ||
       convertToBoolean(rhs->evaluate(context), context, lineNo))
      return ValueType::ValueHolder(new NumericValue(DBTrue));

   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder Equals::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::ARRAY == Lhs.data->type) ||
        (ValueType::ARRAY == Rhs.data->type) )
      DB_panic ("Bad data type in equality operation.", context, lineNo);

   if (Lhs.data->type != Rhs.data->type)
      DB_panic ("Type mismatch in equality operation.", context, lineNo);

   bool truth = false;
   switch (Lhs.data->type)
    {
      case ValueType::STRING:
         truth = static_cast<StringValue*>(Lhs.data)->val == static_cast<StringValue*>(Rhs.data)->val;
         break;
      case ValueType::NUMBER:
         truth = static_cast<NumericValue*>(Lhs.data)->val == static_cast<NumericValue*>(Rhs.data)->val;
         break;
      default:
         break;
    }

   if (true == truth) return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder NotEquals::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::ARRAY == Lhs.data->type) ||
        (ValueType::ARRAY == Rhs.data->type) )
      DB_panic ("Bad data type in inequality operation.", context, lineNo);

   if (Lhs.data->type != Rhs.data->type)
      DB_panic ("Type mismatch in inequality operation.", context, lineNo);

   bool truth = false;
   switch (Lhs.data->type)
    {
      case ValueType::STRING:
         truth = static_cast<StringValue*>(Lhs.data)->val != static_cast<StringValue*>(Rhs.data)->val;
         break;
      case ValueType::NUMBER:
         truth = static_cast<NumericValue*>(Lhs.data)->val != static_cast<NumericValue*>(Rhs.data)->val;
         break;
      default:
         break;
    }

   if (true == truth) return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder Greater::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::ARRAY == Lhs.data->type) ||
        (ValueType::ARRAY == Rhs.data->type) )
      DB_panic ("Bad data type in greater than operation.", context, lineNo);

   if (Lhs.data->type != Rhs.data->type)
      DB_panic ("Type mismatch in greater than operation.", context, lineNo);

   bool truth = false;
   switch (Lhs.data->type)
    {
      case ValueType::STRING:
         truth = static_cast<StringValue*>(Lhs.data)->val > static_cast<StringValue*>(Rhs.data)->val;
         break;
      case ValueType::NUMBER:
         truth = static_cast<NumericValue*>(Lhs.data)->val > static_cast<NumericValue*>(Rhs.data)->val;
         break;
      default:
         break;
    }

   if (true == truth) return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder Less::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::ARRAY == Lhs.data->type) ||
        (ValueType::ARRAY == Rhs.data->type) )
      DB_panic ("Bad data type in less than operation.", context, lineNo);

   if (Lhs.data->type != Rhs.data->type)
      DB_panic ("Type mismatch in less than operation.", context, lineNo);

   bool truth = false;
   switch (Lhs.data->type)
    {
      case ValueType::STRING:
         truth = static_cast<StringValue*>(Lhs.data)->val < static_cast<StringValue*>(Rhs.data)->val;
         break;
      case ValueType::NUMBER:
         truth = static_cast<NumericValue*>(Lhs.data)->val < static_cast<NumericValue*>(Rhs.data)->val;
         break;
      default:
         break;
    }

   if (true == truth) return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder GEQ::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::ARRAY == Lhs.data->type) ||
        (ValueType::ARRAY == Rhs.data->type) )
      DB_panic ("Bad data type in greater than or equals operation.", context, lineNo);

   if (Lhs.data->type != Rhs.data->type)
      DB_panic ("Type mismatch in greater than or equals operation.", context, lineNo);

   bool truth = false;
   switch (Lhs.data->type)
    {
      case ValueType::STRING:
         truth = static_cast<StringValue*>(Lhs.data)->val >= static_cast<StringValue*>(Rhs.data)->val;
         break;
      case ValueType::NUMBER:
         truth = static_cast<NumericValue*>(Lhs.data)->val >= static_cast<NumericValue*>(Rhs.data)->val;
         break;
      default:
         break;
    }

   if (true == truth) return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder LEQ::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::ARRAY == Lhs.data->type) ||
        (ValueType::ARRAY == Rhs.data->type) )
      DB_panic ("Bad data type in less than or equals operation.", context, lineNo);

   if (Lhs.data->type != Rhs.data->type)
      DB_panic ("Type mismatch in less than or equals operation.", context, lineNo);

   bool truth = false;
   switch (Lhs.data->type)
    {
      case ValueType::STRING:
         truth = static_cast<StringValue*>(Lhs.data)->val <= static_cast<StringValue*>(Rhs.data)->val;
         break;
      case ValueType::NUMBER:
         truth = static_cast<NumericValue*>(Lhs.data)->val <= static_cast<NumericValue*>(Rhs.data)->val;
         break;
      default:
         break;
    }

   if (true == truth) return ValueType::ValueHolder(new NumericValue(DBTrue));
   return ValueType::ValueHolder(new NumericValue(DBFalse));
 }

ValueType::ValueHolder DerefVar::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Lhs = lhs->evaluate(context);
   ValueType::ValueHolder Rhs = rhs->evaluate(context);

   if ( (NULL == Lhs.data) || (NULL == Rhs.data) ||
        (ValueType::ARRAY != Lhs.data->type) ||
        (ValueType::NUMBER != Rhs.data->type) )
      DB_panic ("Bad data type in variable dereference.", context, lineNo);

   return static_cast<ArrayValue*>(Lhs.data)->getIndex(fromFloat(static_cast<NumericValue*>(Rhs.data)->val));
 }

ValueType::ValueHolder StandardBinaryFunction::evaluate (CallingContext & context) const
 {
    // Standard Library functions do their own input checking.
   return function(lhs->evaluate(context), rhs->evaluate(context), context, lineNo);
 }

ValueType::ValueHolder StandardUnaryFunction::evaluate (CallingContext & context) const
 {
    // Standard Library functions do their own input checking.
   return function(arg->evaluate(context), context, lineNo);
 }

ValueType::ValueHolder StandardTernaryFunction::evaluate (CallingContext & context) const
 {
    // Standard Library functions do their own input checking.
   return function(first->evaluate(context), second->evaluate(context), third->evaluate(context), context, lineNo);
 }

ValueType::ValueHolder Not::evaluate (CallingContext & context) const
 {
   if (true == convertToBoolean(arg->evaluate(context), context, lineNo))
      return ValueType::ValueHolder(new NumericValue(DBFalse));

   return ValueType::ValueHolder(new NumericValue(DBTrue));
 }

ValueType::ValueHolder Abs::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Arg = arg->evaluate(context);

   if ( (NULL == Arg.data) || (ValueType::NUMBER != Arg.data->type) )
      DB_panic ("Bad data type in absolute value.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue( static_cast<NumericValue*>(Arg.data)->val.abs() ));
 }

ValueType::ValueHolder Negate::evaluate (CallingContext & context) const
 {
   ValueType::ValueHolder Arg = arg->evaluate(context);

   if ( (NULL == Arg.data) || (ValueType::NUMBER != Arg.data->type) )
      DB_panic ("Bad data type in negate.", context, lineNo);

   return ValueType::ValueHolder(new NumericValue( static_cast<NumericValue*>(Arg.data)->val.negate() ));
 }

ValueType::ValueHolder FunctionCall::evaluate (CallingContext & context) const
 {
   CallingContext nextContext(context, name, lineNo);
   ValueType::ValueHolder result;

   if (NULL == nextContext.FunDefs()[name])
      DB_panic("Function \"" + name + "\" declared but never defined.", context, lineNo);

   for (size_t index = 0; index < args.size(); ++index)
    {
      nextContext.Locals().insert(std::make_pair(nextContext.Functions()[name][index], args[index]->evaluate(context)));
    }
   for (std::map<std::string, std::vector<long> >::iterator iter = nextContext.FunLocals()[name].begin();
      iter != nextContext.FunLocals()[name].end(); ++iter)
    {
      nextContext.Locals().insert(std::make_pair(iter->first, nextContext.createVariable(iter->second)));
    }

   bool loop = false;
   do
    {
      FlowControl * res = nextContext.FunDefs()[name]->execute(nextContext);

      if (NULL == res)
         DB_panic("Function \"" + name + "\" never returned a value.", context, lineNo);

      if (FlowControl::RETURN == res->type)
       {
         ReturnFlow * e = static_cast<ReturnFlow*>(res);

         result = e->value;
         loop = false;

         delete res;
       }
      else if (FlowControl::TAIL_CALL == res->type)
       {
         TailCallFlow * e = static_cast<TailCallFlow*>(res);

         nextContext.Locals().clear();

         for (size_t index = 0; index < args.size(); ++index)
          {
            nextContext.Locals().insert(std::make_pair(nextContext.Functions()[name][index], e->newArgs[index]));
          }
         for (std::map<std::string, std::vector<long> >::iterator iter = nextContext.FunLocals()[name].begin();
            iter != nextContext.FunLocals()[name].end(); ++iter)
          {
            nextContext.Locals().insert(std::make_pair(iter->first, nextContext.createVariable(iter->second)));
          }

         loop = true;

         delete res;
       }
      else
         DB_panic("INTERPRETER ERROR!!! : break/continue propagated out of function.", context, lineNo);
    }
   while (true == loop);

   return result;
 }
