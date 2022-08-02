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
#include "Statement.hpp"
#include "Expression.hpp"
#include "RunTimeFlowControl.hpp"

void DB_panic (const std::string &) __attribute__ ((__noreturn__));

ValueType::ValueHolder RecAssignState::evaluate
   (CallingContext & context, ValueType::ValueHolder lhs, const Expression * rhs) const
 {
   if (NULL == next)
    {
      if (NULL == rhs)
       {
         lhs = getIndex(lhs, index->evaluate(context));
       }
      else
       {
         lhs = setIndex(lhs, index->evaluate(context), rhs->evaluate(context));
       }
    }
   else
    {
      ValueType::ValueHolder arrayIndex = index->evaluate(context);
      if (NULL == rhs)
       {
         lhs = next->evaluate(context, getIndex(lhs, arrayIndex), rhs);
       }
      else
       {
         lhs = setIndex(lhs, arrayIndex, next->evaluate(context, getIndex(lhs, arrayIndex), rhs));
       }
    }
   return lhs;
 }

ValueType::ValueHolder RecAssignState::getIndex (ValueType::ValueHolder array, ValueType::ValueHolder index)
 {
   if ( (NULL == array.data) || (NULL == index.data) ||
        (ValueType::ARRAY != array.data->type) ||
        (ValueType::NUMBER != index.data->type) )
      DB_panic ("Bad data type in variable dereference.");

   return static_cast<ArrayValue*>(array.data)->getIndex(fromFloat(static_cast<NumericValue*>(index.data)->val));
 }

ValueType::ValueHolder RecAssignState::setIndex
   (ValueType::ValueHolder array, ValueType::ValueHolder index, ValueType::ValueHolder value)
 {
   if ( (NULL == array.data) || (NULL == index.data) ||
        (ValueType::ARRAY != array.data->type) ||
        (ValueType::NUMBER != index.data->type) )
      DB_panic ("Bad data type in variable dereference.");

   static_cast<ArrayValue*>(array.data)->setIndex(fromFloat(static_cast<NumericValue*>(index.data)->val), value);

   return array;
 }

RecAssignState::~RecAssignState()
 {
   if (NULL != index) { delete index; index = NULL; }
   if (NULL != next) { delete next; next = NULL; }
 }

FlowControl * Assignment::execute (CallingContext & context) const
 {
   if (NULL == index)
    {
      context.setValue(lhs, rhs->evaluate(context), lineNo);
    }
   else
    {
      context.setValue(lhs, index->evaluate(context, context.getValue(lhs, lineNo), rhs), lineNo);
    }
   return NULL; // Assignment never returns or breaks.
 }

Assignment::~Assignment()
 {
   if (NULL != index) { delete index; index = NULL; }
   if (NULL != rhs) { delete rhs; rhs = NULL; }
 }

FlowControl * IfStatement::execute (CallingContext & context) const
 {
   FlowControl * ret = NULL;
   if (true == Expression::convertToBoolean(condition->evaluate(context), context, lineNo))
    {
      if (NULL != thenSeq) ret = thenSeq->execute(context);
    }
   else if (NULL != elseSeq)
    {
      ret = elseSeq->execute(context);
    }
   return ret;
 }

IfStatement::~IfStatement()
 {
   if (NULL != condition) { delete condition; condition = NULL; }
   if (NULL != thenSeq) { delete thenSeq; thenSeq = NULL; }
   if (NULL != elseSeq) { delete elseSeq; elseSeq = NULL; }
 }

FlowControl * DoStatement::execute (CallingContext & context) const
 {
   while (true)
    {
      if ( (NULL != preCondition) &&
           (false == Expression::convertToBoolean(preCondition->evaluate(context), context, lineNo)) )
         return NULL;

      FlowControl * test = seq->execute(context);
      if (NULL != test)
       {
         if (FlowControl::BREAK == test->type)
          {
            BreakFlow * e = static_cast<BreakFlow*>(test);
            if (("" == e->alias) || (label == e->alias))
             {
               bool continued = e->isContinue;
               delete test;
               if (false == continued)
                  return NULL;
             }
            else
             {
               return test;
             }
          }
         else
          {
            return test;
          }
       }

      if ( (NULL != postCondition) &&
           (false == Expression::convertToBoolean(postCondition->evaluate(context), context, lineNo)) )
         return NULL;
    }
 }

DoStatement::~DoStatement()
 {
   if (NULL != preCondition) { delete preCondition; preCondition = NULL; }
   if (NULL != postCondition) { delete postCondition; postCondition = NULL; }
   if (NULL != seq) { delete seq; seq = NULL; }
 }

FlowControl * BreakStatement::execute (CallingContext & context) const
 {
   FlowControl * ret = NULL;
   if ((NULL == condition) || (false == Expression::convertToBoolean(condition->evaluate(context), context, lineNo)))
    {
      ret = new BreakFlow(label, toContinue);
    }
   return ret;
 }

BreakStatement::~BreakStatement()
 {
   if (NULL != condition) { delete condition; condition = NULL; }
 }

FlowControl * ForStatement::execute (CallingContext & context) const
 {
      //   <location> <assign> <expression>
   if (NULL == index)
    {
      context.setValue(lhs, initialValue->evaluate(context), lineNo);
    }
   else
    {
      context.setValue(lhs, index->evaluate(context, context.getValue(lhs, lineNo), initialValue), lineNo);
    }

      //   ( "to" | "downto" ) <expression>
   Constant * left = new Constant();
   Constant * right = new Constant();
   LEQ forTo; // These are on the stack so they are exception safe.
   GEQ forDownTo;
   BinaryOperation * comparator;
   if (true == to)
    {
      comparator = &forTo;
    }
   else
    {
      comparator = &forDownTo;
    }
   comparator->lhs = left;
   comparator->rhs = right;

      //   [ "step" <expression> ]
   Constant * lcv = new Constant();
   Constant * del = new Constant();
   Plus nextVal;
   nextVal.lhs = lcv;
   nextVal.rhs = del;

   while (true)
    {
      // Assign lcv to left.
      if (NULL == index)
       {
         left->value = context.getValue(lhs, lineNo);
       }
      else
       {
         left->value = index->evaluate(context, context.getValue(lhs, lineNo), NULL);
       }
      // Assign term condition to right
      right->value = termValue->evaluate(context);

      // If we're too far, exit loop.
      if (false == Expression::convertToBoolean(comparator->evaluate(context), context, lineNo))
         return NULL;

         //   <statements>
      FlowControl * test = seq->execute(context);
      if (NULL != test)
       {
         if (FlowControl::BREAK == test->type)
          {
            BreakFlow * e = static_cast<BreakFlow*>(test);
            if (("" == e->alias) || (label == e->alias))
             {
               bool continued = e->isContinue;
               delete test;
               if (false == continued)
                  return NULL;
             }
            else
             {
               return test;
             }
          }
         else
          {
            return test;
          }
       }

      // Increment lcv for next iteration
      if (NULL == index)
       {
         // Assign lcv to lcv.
         lcv->value = context.getValue(lhs, lineNo);
         // Assign step size to del
         del->value = stepSize->evaluate(context);
         // Assign next value
         context.setValue(lhs, nextVal.evaluate(context), lineNo);
       }
      else
       {
         lcv->value = index->evaluate(context, context.getValue(lhs, lineNo), NULL);
         del->value = stepSize->evaluate(context);
         context.setValue(lhs, index->evaluate(context, context.getValue(lhs, lineNo), &nextVal), lineNo);
       }
    }
 }

ForStatement::~ForStatement()
 {
   if (NULL != index) { delete index; index = NULL; }
   if (NULL != initialValue) { delete initialValue; initialValue = NULL; }
   if (NULL != termValue) { delete termValue; termValue = NULL; }
   if (NULL != stepSize) { delete stepSize; stepSize = NULL; }
   if (NULL != seq) { delete seq; seq = NULL; }
 }

FlowControl * ReturnStatement::execute (CallingContext & context) const
 {
   ReturnFlow * e = new ReturnFlow();
   if (NULL != value) e->value = value->evaluate(context);
   return e;
 }

ReturnStatement::~ReturnStatement()
 {
   if (NULL != value) { delete value; value = NULL; }
 }

FlowControl * TailCallStatement::execute (CallingContext & context) const
 {
   TailCallFlow * e = new TailCallFlow();
   for (std::vector<Expression*>::const_iterator iter = args.begin();
      iter != args.end(); ++iter)
    {
      e->newArgs.push_back((*iter)->evaluate(context));
    }
   return e;
 }

TailCallStatement::~TailCallStatement()
 {
   for (std::vector<Expression*>::iterator iter = args.begin();
      iter != args.end(); ++iter)
    {
      delete *iter;
    }
 }

FlowControl * CallStatement::execute (CallingContext & context) const
 {
   (void) fun->evaluate(context);
   return NULL;
 }

CallStatement::~CallStatement()
 {
   if (NULL != fun) { delete fun; fun = NULL; }
 }

CaseContainer::~CaseContainer()
 {
   if (NULL != condition) { delete condition; condition = NULL; }
   if (NULL != seq) { delete seq; seq = NULL; }
 }

FlowControl * SelectStatement::execute (CallingContext & context) const
 {
   Constant * controlVal = new Constant();
   Constant * testVal = new Constant();
   Equals tester;

   tester.lhs = controlVal;
   tester.rhs = testVal;

   controlVal->value = control->evaluate(context);

 /*
   end = false;
   for (iter = cases.begin(); (false == end) && (iter != cases.end()); +iter)
      if control->evaluate == iter->condition->evaluate
         do
            iter->seq->execute
            ++iter
         while false == iter->breaking
         end = true
      fi
 */

   bool end = false;
   for (std::vector<CaseContainer*>::const_iterator iter = cases.begin();
      (false == end) && (cases.end() != iter); ++iter)
    {
      if (NULL != (*iter)->condition)
         testVal->value = (*iter)->condition->evaluate(context);

      if ((NULL == (*iter)->condition) ||
          (true == Expression::convertToBoolean(tester.evaluate(context), context, lineNo)))
       {
         do
          {
            FlowControl * test = (*iter)->seq->execute(context);
            if (NULL != test) return test;
            ++iter;
          }
         while ((cases.end() != iter) && (true == (*iter)->breaking));
         end = true;
       }
    }
   return NULL;
 }

SelectStatement::~SelectStatement ()
 {
   if (NULL != control) { delete control; control = NULL; }
   for (std::vector<CaseContainer*>::iterator iter = cases.begin();
      iter != cases.end(); ++iter)
    {
      delete *iter;
    }
 }
