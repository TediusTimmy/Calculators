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
#include "Parser.hpp"

#include "SymbolTable.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "PointerWrapper.hpp"

#include <sstream>

void DB_panic (const std::string &) __attribute__ ((__noreturn__));

extern const BigInt::Float DBTrue , DBFalse;

void Parser::GNT (void)
 {
   nextToken = src.GetNextToken();
 }

std::string Parser::LN (void)
 {
   std::stringstream str;
   // This should always work.
   str << nextToken.lineNumber;
   return str.str();
 }

void Parser::expect (const Lexeme & tok)
 {
   if (tok != nextToken.lexeme)
    {
      DB_panic("Token \"" + nextToken.text + "\" was not expected on " + LN() + ".");
    }
   GNT();
 }





static void AssignNext (PointerWrapper<BinaryOperation> & oldOb, BinaryOperation * newOb, PointerWrapper<Expression> & lhs)
 {
   if (true == oldOb.isNULL())
    {
      oldOb = newOb;
      oldOb->lhs = lhs.get();
    }
   else
    {
      newOb->lhs = oldOb.get();
      oldOb = newOb;
    }
 }

Expression * Parser::expression (CallingContext & context)
 {
   PointerWrapper<BinaryOperation> ret (NULL);

   PointerWrapper<Expression> lhs (boolean(context));

   while ((AND_OP == nextToken.lexeme) || (OR_OP == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case AND_OP:
            AssignNext(ret, new AndOp(), lhs);
            break;
         case OR_OP:
            AssignNext(ret, new OrOp(), lhs);
            break;
         default: break;
       }

      ret->lineNo = nextToken.lineNumber;
      GNT();

      ret->rhs = boolean(context);
    }

   if (true == ret.isNULL())
      return lhs.get();
   return ret.get();
 }

Expression * Parser::boolean (CallingContext & context)
 {
   PointerWrapper<BinaryOperation> ret (NULL);

   PointerWrapper<Expression> lhs (clause(context));

   while (SHORT_OR == nextToken.lexeme)
    {
      AssignNext(ret, new ShortOr(), lhs);

      ret->lineNo = nextToken.lineNumber;
      GNT();

      ret->rhs = clause(context);
    }

   if (true == ret.isNULL())
      return lhs.get();
   return ret.get();
 }

Expression * Parser::clause (CallingContext & context)
 {
   PointerWrapper<BinaryOperation> ret (NULL);

   PointerWrapper<Expression> lhs (predicate(context));

   while (SHORT_AND == nextToken.lexeme)
    {
      AssignNext(ret, new ShortAnd(), lhs);

      ret->lineNo = nextToken.lineNumber;
      GNT();

      ret->rhs = predicate(context);
    }

   if (true == ret.isNULL())
      return lhs.get();
   return ret.get();
 }

Expression * Parser::predicate (CallingContext & context)
 {
   PointerWrapper<BinaryOperation> ret (NULL);

   PointerWrapper<Expression> lhs (relation(context));

   while ((EQUALITY == nextToken.lexeme) || (INEQUALITY == nextToken.lexeme) || (EQUAL_SIGN == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case EQUAL_SIGN:
         case EQUALITY:
            AssignNext(ret, new Equals(), lhs);
            break;
         case INEQUALITY:
            AssignNext(ret, new NotEquals(), lhs);
            break;
         default: break;
       }

      ret->lineNo = nextToken.lineNumber;
      GNT();

      ret->rhs = relation(context);
    }

   if (true == ret.isNULL())
      return lhs.get();
   return ret.get();
 }

Expression * Parser::relation (CallingContext & context)
 {
   PointerWrapper<BinaryOperation> ret (NULL);

   PointerWrapper<Expression> lhs (simple(context));

   while ((GREATER_THAN == nextToken.lexeme) || (LESS_THAN_OR_EQUAL_TO == nextToken.lexeme) ||
          (GREATER_THAN_OR_EQUAL_TO == nextToken.lexeme) || (LESS_THAN == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case GREATER_THAN:
            AssignNext(ret, new Greater(), lhs);
            break;
         case LESS_THAN:
            AssignNext(ret, new Less(), lhs);
            break;
         case GREATER_THAN_OR_EQUAL_TO:
            AssignNext(ret, new GEQ(), lhs);
            break;
         case LESS_THAN_OR_EQUAL_TO:
            AssignNext(ret, new LEQ(), lhs);
            break;
         default: break;
       }

      ret->lineNo = nextToken.lineNumber;
      GNT();

      ret->rhs = simple(context);
    }

   if (true == ret.isNULL())
      return lhs.get();
   return ret.get();
 }

Expression * Parser::simple (CallingContext & context)
 {
   PointerWrapper<BinaryOperation> ret (NULL);

   PointerWrapper<Expression> lhs (term(context));

   while ((PLUS == nextToken.lexeme) || (MINUS == nextToken.lexeme) || (STRING_CAT == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case PLUS:
            AssignNext(ret, new Plus(), lhs);
            break;
         case MINUS:
            AssignNext(ret, new Minus(), lhs);
            break;
         case STRING_CAT:
            AssignNext(ret, new StringCat(), lhs);
            break;
         default: break;
       }

      ret->lineNo = nextToken.lineNumber;
      GNT();

      ret->rhs = term(context);
    }

   if (true == ret.isNULL())
      return lhs.get();
   return ret.get();
 }

Expression * Parser::term (CallingContext & context)
 {
   PointerWrapper<BinaryOperation> ret (NULL);

   PointerWrapper<Expression> lhs (factor(context));

   while ((MULTIPLY == nextToken.lexeme) || (DIVIDE == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case MULTIPLY:
            AssignNext(ret, new Multiply(), lhs);
            break;
         case DIVIDE:
            AssignNext(ret, new Divide(), lhs);
            break;
         default: break;
       }

      ret->lineNo = nextToken.lineNumber;
      GNT();

      ret->rhs = factor(context);
    }

   if (true == ret.isNULL())
      return lhs.get();
   return ret.get();
 }

Expression * Parser::factor (CallingContext & context)
 {
   PointerWrapper<BinaryOperation> ret (NULL);

   PointerWrapper<Expression> lhs (primary(context));

   while (POWER == nextToken.lexeme)
    {
      AssignNext(ret, new Power(), lhs);

      ret->lineNo = nextToken.lineNumber;
      GNT();

      ret->rhs = factor(context);
    }

   if (true == ret.isNULL())
      return lhs.get();
   return ret.get();
 }

Expression * Parser::primary (CallingContext & context)
 {
   PointerWrapper<Expression> ret (NULL);

   switch(nextToken.lexeme)
    {
      case IDENTIFIER:
         switch(context.lookup(nextToken.text))
          {
            case CallingContext::VARIABLE:
                {
                  Variable * op = new Variable();
                  ret = op;

                  op->referent = nextToken.text;

                  ret->lineNo = nextToken.lineNumber;
                  GNT();
                }
               break;
            case CallingContext::CONSTANT:
                {
                  Constant * op = new Constant();
                  ret = op;

                  op->value = context.getValue(nextToken.text, nextToken.lineNumber);

                  ret->lineNo = nextToken.lineNumber;
                  GNT();
                }
               break;
            case CallingContext::FUNCTION:
                {
                  FunctionCall * op = new FunctionCall();
                  ret = op;

                  op->name = nextToken.text;

                  ret->lineNo = nextToken.lineNumber;
                  GNT();

                  expect(OPEN_PARENS);

                  if (CLOSE_PARENS != nextToken.lexeme)
                   {
                     op->args.push_back(expression(context));

                     while (SEMICOLON == nextToken.lexeme)
                      {
                        GNT();
                        op->args.push_back(expression(context));
                      }
                   }

                  expect(CLOSE_PARENS);

                  if (op->args.size() != context.Functions().find(op->name)->second.size())
                     DB_panic("Call to \"" + op->name + "\" with bad number of arguments on " + LN() + ".");
                }
               break;
            case CallingContext::STANDARD_FUNCTION:
               switch(context.getStandardFunctionType(nextToken.text))
                {
                  case CallingContext::CONSTANT_FUNCTION:
                      {
                        StandardConstantFunction * op = new StandardConstantFunction();
                        ret = op;

                        op->function = context.getConstantFunction(nextToken.text);
                        ret->lineNo = nextToken.lineNumber;
                        GNT();

                        expect(OPEN_PARENS);
                        expect(CLOSE_PARENS);
                      }
                     break;
                  case CallingContext::UNARY_FUNCTION:
                      {
                        StandardUnaryFunction * op = new StandardUnaryFunction();
                        ret = op;

                        op->function = context.getUnaryFunction(nextToken.text);
                        ret->lineNo = nextToken.lineNumber;
                        GNT();

                        expect(OPEN_PARENS);
                        op->arg = expression(context);
                        expect(CLOSE_PARENS);
                      }
                     break;
                  case CallingContext::BINARY_FUNCTION:
                      {
                        StandardBinaryFunction * op = new StandardBinaryFunction();
                        ret = op;

                        op->function = context.getBinaryFunction(nextToken.text);
                        ret->lineNo = nextToken.lineNumber;
                        GNT();

                        expect(OPEN_PARENS);
                        op->lhs = expression(context);
                        expect(SEMICOLON);
                        op->rhs = expression(context);
                        expect(CLOSE_PARENS);
                      }
                     break;
                  case CallingContext::TERNARY_FUNCTION:
                      {
                        StandardTernaryFunction * op = new StandardTernaryFunction();
                        ret = op;

                        op->function = context.getTernaryFunction(nextToken.text);
                        ret->lineNo = nextToken.lineNumber;
                        GNT();

                        expect(OPEN_PARENS);
                        op->first = expression(context);
                        expect(SEMICOLON);
                        op->second = expression(context);
                        expect(SEMICOLON);
                        op->third = expression(context);
                        expect(CLOSE_PARENS);
                      }
                     break;
                  case CallingContext::NOT_STANDARD_FUNCTION:
                     break; // If this happens, the hardware must be hosed.
                }
               break;
            case CallingContext::LABEL:
               DB_panic("Label \"" + nextToken.text + "\" used in expression on " + LN() + ".");
            case CallingContext::UNDEFINED:
               DB_panic("Undefined identifier \"" + nextToken.text + "\" used in expression on " + LN() + ".");
          }
         break;
      case PLUS:
          {
            Abs * op = new Abs();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->arg = primary(context);
          }
         break;
      case MINUS:
          {
            Negate * op = new Negate();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->arg = primary(context);
          }
         break;
      case NOT:
          {
            Not * op = new Not();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->arg = primary(context);
          }
         break;
      case NUMBER:
          {
            Constant * op = new Constant();
            ret = op;

            BigInt::Float val = BigInt::Float(nextToken.text);
            val.normalize();
            if (val.getPrecision() < BigInt::Float::getMinPrecision())
             {
               val.setPrecision(BigInt::Float::getMinPrecision());
             }
            else if (val.getPrecision() > BigInt::Float::getMaxPrecision())
             {
               val.setPrecision(BigInt::Float::getMaxPrecision());
             }

            op->value = ValueType::ValueHolder(new NumericValue(val));

            ret->lineNo = nextToken.lineNumber;
            GNT();
          }
         break;
      case STRING:
          {
            Constant * op = new Constant();
            ret = op;

            op->value = ValueType::ValueHolder(new StringValue(nextToken.text));

            ret->lineNo = nextToken.lineNumber;
            GNT();
          }
         break;
      case OPEN_PARENS:
         GNT();
         ret = expression(context);
         expect(CLOSE_PARENS);
         break;
      default:
         DB_panic("Expected primary expression, found \"" + nextToken.text + "\" on " + LN() + ".");
    }

   // Handle this here, despite NUMEROUS instances where this DOES NOT MAKE SENSE.
   //    Chewbacca statement: "Hello, World!"[5]
   while (LEFT_BRACKET == nextToken.lexeme)
    {
      DerefVar * op = new DerefVar();

      op->lhs = ret.get();
      ret = op;

      ret->lineNo = nextToken.lineNumber;
      GNT();

      op->rhs = expression(context);

      expect(RIGHT_BRACKET);
    }

   return ret.get();
 }





void Parser::program (CallingContext & context)
 {
   constants(context);
   globals(context);
   functions(context);
   expect(END_OF_FILE);
 }

void Parser::constants (CallingContext & context)
 {
   while ((CONST == nextToken.lexeme) || (NEW_LINE == nextToken.lexeme))
    {
      if (CONST == nextToken.lexeme)
       {
         GNT();

         std::string name = nextToken.text;
         expect(IDENTIFIER);

         if (CallingContext::UNDEFINED != context.lookup(name))
            DB_panic("Reuse of identifier \"" + name + "\" on " + LN() + ".");

         if (EQUAL_SIGN == nextToken.lexeme)
            GNT();
         else
            expect(ASSIGNMENT);

         PointerWrapper<Expression> temp (expression(context));

         ValueType::ValueHolder val = temp->evaluate(context);

         context.Constants().insert(std::make_pair(name, val));

         expect(NEW_LINE);
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }

void Parser::globals (CallingContext & context)
 {
   while ((DIM == nextToken.lexeme) || (NEW_LINE == nextToken.lexeme))
    {
      if (DIM == nextToken.lexeme)
       {
         do
          {
            GNT();

            std::string name = nextToken.text;
            expect(IDENTIFIER);

            if (CallingContext::UNDEFINED != context.lookup(name))
               DB_panic("Reuse of identifier \"" + name + "\" on " + LN() + ".");

            ValueType::ValueHolder val;

            if (LEFT_BRACKET == nextToken.lexeme)
             {
               std::vector<long> depth;
               do
                {
                  GNT();

                  PointerWrapper<Expression> temp (expression(context));

                  ValueType::ValueHolder index = temp->evaluate(context);

                  if ( (NULL == index.data) || (ValueType::NUMBER != index.data->type) )
                     DB_panic ("Bad data type in array subscript on " + LN() + ".");

                  depth.push_back(BigInt::fromFloat(static_cast<NumericValue*>(index.data)->val));

                  expect(RIGHT_BRACKET);
                }
               while (LEFT_BRACKET == nextToken.lexeme);

               val = context.createVariable(depth);
             }

            context.Globals().insert(std::make_pair(name, val));
          }
         while (SEMICOLON == nextToken.lexeme);

         expect(NEW_LINE);
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }

void Parser::variables (CallingContext & context)
 {
   while ((DIM == nextToken.lexeme) || (NEW_LINE == nextToken.lexeme))
    {
      if (DIM == nextToken.lexeme)
       {
         do
          {
            GNT();

            bool isStatic = false;
            if (STATIC == nextToken.lexeme)
             {
               GNT();
               isStatic = true;
             }

            std::string name = nextToken.text;
            expect(IDENTIFIER);

            if (CallingContext::UNDEFINED != context.lookup(name))
               DB_panic("Reuse of identifier \"" + name + "\" on " + LN() + ".");

            std::vector<long> depth;
            if (LEFT_BRACKET == nextToken.lexeme)
             {
               do
                {
                  GNT();

                  PointerWrapper<Expression> temp (expression(context));

                  ValueType::ValueHolder index = temp->evaluate(context);

                  if ( (NULL == index.data) || (ValueType::NUMBER != index.data->type) )
                     DB_panic ("Bad data type in array subscript on " + LN() + ".");

                  depth.push_back(BigInt::fromFloat(static_cast<NumericValue*>(index.data)->val));

                  expect(RIGHT_BRACKET);
                }
               while (LEFT_BRACKET == nextToken.lexeme);
             }

            if (true == isStatic)
             {
               ValueType::ValueHolder val = context.createVariable(depth);
               context.Statics().insert(std::make_pair(name, val));
             }
            else
             {
               context.FunLocals()[context.Name].insert(std::make_pair(name, depth));
             }
          }
         while (SEMICOLON == nextToken.lexeme);

         expect(NEW_LINE);
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }

void Parser::functions (CallingContext & context)
 {
   while ((FUNCTION == nextToken.lexeme) || (DECLARE == nextToken.lexeme) || (NEW_LINE == nextToken.lexeme))
    {
      if ((DECLARE == nextToken.lexeme) || (FUNCTION == nextToken.lexeme))
       {
         bool isDeclare = (DECLARE == nextToken.lexeme);
         GNT();

         std::string name = nextToken.text;
         expect(IDENTIFIER);

         bool wasDeclared = false;
         if (CallingContext::FUNCTION == context.lookup(name))
          {
            wasDeclared = true;

            // If we have a definition and this is not a declare, then we have a problem.
            if ((context.FunDefs().end() != context.FunDefs().find(name)) && (false == isDeclare))
               DB_panic("Redefinition of function \"" + name + "\" on " + LN() + ".");
          }
         else if (CallingContext::UNDEFINED != context.lookup(name))
            DB_panic("Reuse of identifier \"" + name + "\" on " + LN() + ".");

         std::vector<std::string> args;

         expect(OPEN_PARENS);
         if (IDENTIFIER == nextToken.lexeme)
          {
            std::string temp = nextToken.text;

            GNT();

            args.push_back(temp);

            if (CallingContext::UNDEFINED != context.lookup(temp))
               DB_panic("Reuse of identifier \"" + temp + "\" on " + LN() + ".");

            while (SEMICOLON == nextToken.lexeme)
             {
               GNT();

               temp = nextToken.text;
               expect(IDENTIFIER);

               args.push_back(temp);

               if (CallingContext::UNDEFINED != context.lookup(temp))
                  DB_panic("Reuse of identifier \"" + temp + "\" on " + LN() + ".");
             }
          }
         expect(CLOSE_PARENS);

         if (true == wasDeclared)
          {
            // Check the previous definition/declaration and make sure
            // it matches the current declaration/definition.
            std::vector<std::string> & check = context.Functions().find(name)->second;

            if (check.size() != args.size())
               DB_panic("Declaration or definition of function \"" + name + "\" on " + LN() +
                  "\ndoes not match previous declaration(s) or definition(s).");

            for (std::vector<std::string>::iterator cIter = check.begin(), aIter = args.begin();
               cIter != check.end(); ++cIter, ++aIter)
             {
               if (*cIter != *aIter)
                  DB_panic("Declaration or definition of function \"" + name + "\" on " + LN() +
                     "\ndoes not match previous declaration(s) or definition(s).");
             }
          }
         else
          {
            context.Functions().insert(std::make_pair(name, args));
          }
         expect(NEW_LINE);

         // This is a define, so define the function.
         if (false == isDeclare)
          {
            // Build a new calling context, inserting dummy locals.
            CallingContext newFunction (context, name, nextToken.lineNumber);

            for (std::vector<std::string>::iterator iter = args.begin();
               args.end() != iter; ++iter)
             {
               newFunction.Locals().insert(std::make_pair(*iter, ValueType::ValueHolder()));
             }

            variables(newFunction);

            for (std::map<std::string, std::vector<long> >::iterator iter = newFunction.FunLocals()[name].begin();
               iter != newFunction.FunLocals()[name].end(); ++iter)
             {
               newFunction.Locals().insert(std::make_pair(iter->first, ValueType::ValueHolder()));
             }

            PointerWrapper<StatementSeq> seq (new StatementSeq());

            while (END_FUNCTION != nextToken.lexeme)
             {
               while (NEW_LINE == nextToken.lexeme)
                  GNT();

               Statement * statmnt = statement(newFunction);

               if (NULL != statmnt)
                  seq->statements.push_back(statmnt);

               expect(NEW_LINE);
             }
            GNT();

            context.FunDefs().insert(std::make_pair(name, seq.get()));
          }
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }





Statement * Parser::statement (CallingContext & context)
 {
   PointerWrapper<Statement> ret (NULL);

   std::string label;
   if (COLON == nextToken.lexeme)
    {
      GNT();

      if (CallingContext::UNDEFINED != context.lookup(nextToken.text))
         DB_panic("Identifier \"" + nextToken.text + "\" redefined as label on " + LN() + ".");

      label = nextToken.text;

      expect(IDENTIFIER);
      expect(COLON);
    }

   switch (nextToken.lexeme)
    {
      case IDENTIFIER: // Assignment Statement
          {
            Assignment * op = new Assignment();
            ret = op;

            if (CallingContext::VARIABLE != context.lookup(nextToken.text))
               DB_panic("L-value \"" + nextToken.text + "\" not a variable on " + LN() + ".");

            op->lhs = nextToken.text;
            ret->lineNo = nextToken.lineNumber;
            GNT();

            RecAssignState ** cur = &(op->index);
            while (LEFT_BRACKET == nextToken.lexeme)
             {
               GNT();

               *cur = new RecAssignState();
               (*cur)->index = expression(context);
               cur = &((*cur)->next);

               expect(RIGHT_BRACKET);
             }

            if (EQUAL_SIGN == nextToken.lexeme)
               GNT();
            else
               expect(ASSIGNMENT);

            op->rhs = expression(context);
          }
         break;

      case IF:
          {
            IfStatement * op = new IfStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->condition = expression(context);

            expect(THEN);

            if (NEW_LINE == nextToken.lexeme)
             {
               GNT();

               PointerWrapper<StatementSeq> thenSeq (new StatementSeq());

               while ((ELSE != nextToken.lexeme) && (FI != nextToken.lexeme))
                {
                  Statement * statmnt = statement(context);

                  if (NULL != statmnt)
                     thenSeq->statements.push_back(statmnt);

                  expect(NEW_LINE);
                }

               if (0 < thenSeq->statements.size())
                  op->thenSeq = thenSeq.get();

               if (ELSE == nextToken.lexeme)
                {
                  GNT();

                  Statement ** cur = &(op->elseSeq);

                  while (IF == nextToken.lexeme)
                   {
                     GNT();

                     IfStatement * temp = new IfStatement();
                     *cur = temp;
                     cur = &(temp->elseSeq);

                     temp->condition = expression(context);

                     expect(THEN);
                     expect(NEW_LINE);

                     PointerWrapper<StatementSeq> tempThenSeq (new StatementSeq());
                     while ((ELSE != nextToken.lexeme) && (FI != nextToken.lexeme))
                      {
                        Statement * statmnt = statement(context);

                        if (NULL != statmnt)
                           tempThenSeq->statements.push_back(statmnt);

                        expect(NEW_LINE);
                      }

                     if (0 < tempThenSeq->statements.size())
                        temp->thenSeq = tempThenSeq.get();

                     if (ELSE == nextToken.lexeme)
                        GNT();
                   }

                   // At this point, we are either at FI
                   // Or a NEW_LINE. NEW_LINE implies ELSE.
                  if (NEW_LINE == nextToken.lexeme)
                   {
                     GNT();

                     PointerWrapper<StatementSeq> elseSeq (new StatementSeq());

                     while (FI != nextToken.lexeme)
                      {
                        Statement * statmnt = statement(context);

                        if (NULL != statmnt)
                           elseSeq->statements.push_back(statmnt);

                        expect(NEW_LINE);
                      }

                     if (0 < elseSeq->statements.size())
                        *cur = elseSeq.get();
                   }
                }

               expect(FI);
             }
            else
             {
               op->thenSeq = statement(context);

               if (ELSE == nextToken.lexeme)
                {
                  GNT();
                  op->elseSeq = statement(context);
                }
             }
          }
         break;

      case DO:
          {
            DoStatement * op = new DoStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            // Push a new label to the stack so the symbol table can remember these.
            context.m_labels.push_back(label);
            op->label = label;
            label = "";

            if (WHILE == nextToken.lexeme)
             {
               GNT();

               op->preCondition = expression(context);
             }
            else if (UNTIL == nextToken.lexeme)
             {
               GNT();

               Not * neg = new Not();
               op->preCondition = neg;
               neg->arg = expression(context);
             }

            expect(NEW_LINE);

            op->seq = new StatementSeq();

            while (LOOP != nextToken.lexeme)
             {
               Statement * statmnt = statement(context);

               if (NULL != statmnt)
                  op->seq->statements.push_back(statmnt);

               expect(NEW_LINE);
             }
            GNT();

            if (WHILE == nextToken.lexeme)
             {
               GNT();

               op->postCondition = expression(context);
             }
            else if (UNTIL == nextToken.lexeme)
             {
               GNT();

               Not * neg = new Not();
               op->postCondition = neg;
               neg->arg = expression(context);
             }

            // Pop the label stack.
            context.m_labels.pop_back();
          }
         break;

      case BREAK:
          {
            BreakStatement * op = new BreakStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (0 == context.m_labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op->label = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op->label))
                  DB_panic("Use of undefined label \"" + op->label + "\" on " + LN() + ".");

               expect(COLON);
             }
          }
         break;

      case CONTINUE:
          {
            BreakStatement * op = new BreakStatement();
            op->toContinue = true;
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (0 == context.m_labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op->label = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op->label))
                  DB_panic("Use of undefined label \"" + op->label + "\" on " + LN() + ".");

               expect(COLON);
             }
          }
         break;

      case WHILE:
          {
            BreakStatement * op = new BreakStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (0 == context.m_labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op->label = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op->label))
                  DB_panic("Use of undefined label \"" + op->label + "\" on " + LN() + ".");

               expect(COLON);
             }

            op->condition = expression(context);
          }
         break;

      case UNTIL:
          {
            BreakStatement * op = new BreakStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (0 == context.m_labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op->label = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op->label))
                  DB_panic("Use of undefined label \"" + op->label + "\" on " + LN() + ".");

               expect(COLON);
             }

            Not * neg = new Not();
            neg->arg = expression(context);
            op->condition = neg;
          }
         break;

      case FOR:
          {
            ForStatement * op = new ForStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            // Push a new label to the stack so the symbol table can remember these.
            context.m_labels.push_back(label);
            op->label = label;
            label = "";

            if (CallingContext::VARIABLE != context.lookup(nextToken.text))
               DB_panic("L-value \"" + nextToken.text + "\" not a variable on " + LN() + ".");

            op->lhs = nextToken.text;
            GNT();

            RecAssignState ** cur = &(op->index);
            while (LEFT_BRACKET == nextToken.lexeme)
             {
               GNT();

               *cur = new RecAssignState();
               (*cur)->index = expression(context);
               cur = &((*cur)->next);

               expect(RIGHT_BRACKET);
             }

            if (EQUAL_SIGN == nextToken.lexeme)
               GNT();
            else
               expect(ASSIGNMENT);

            op->initialValue = expression(context);

            op->to = (TO == nextToken.lexeme);
            if (DOWNTO == nextToken.lexeme)
               GNT();
            else
               expect(TO);

            op->termValue = expression(context);

            if (STEP == nextToken.lexeme)
             {
               GNT();
               op->stepSize = expression(context);
             }
            else
             {
               Constant * step = new Constant();
               op->stepSize = step;
               if (true == op->to)
                {
                  step->value = ValueType::ValueHolder(new NumericValue(DBTrue));
                }
               else
                {
                  step->value = ValueType::ValueHolder(new NumericValue(-DBTrue));
                }
             }
            expect(NEW_LINE); // Home Video

            op->seq = new StatementSeq();

            while (NEXT != nextToken.lexeme)
             {
               Statement * statmnt = statement(context);

               if (NULL != statmnt)
                  op->seq->statements.push_back(statmnt);

               expect(NEW_LINE);
             }
            GNT();

            // Pop the label stack.
            context.m_labels.pop_back();
          }
         break;

      case SELECT:
          {
            SelectStatement * op = new SelectStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->control = expression(context);

            expect(NEW_LINE);

            while ( (END_SELECT != nextToken.lexeme) &&
                    (DEFAULT != nextToken.lexeme) && (CASE_ELSE != nextToken.lexeme) )
             {
               CaseContainer * newCase = new CaseContainer();
               op->cases.push_back(newCase);

               newCase->breaking = (CHOICE == nextToken.lexeme);
               newCase->seq = new StatementSeq();

               if (CHOICE == nextToken.lexeme)
                  GNT();
               else
                  expect(CASE);
               newCase->condition = expression(context);
               expect(NEW_LINE);

               while ( (CASE != nextToken.lexeme) && (CHOICE != nextToken.lexeme) &&
                       (DEFAULT != nextToken.lexeme) && (CASE_ELSE != nextToken.lexeme) &&
                       (END_SELECT != nextToken.lexeme) )
                {
                  Statement * statmnt = statement(context);

                  if (NULL != statmnt)
                     newCase->seq->statements.push_back(statmnt);

                  expect(NEW_LINE);
                }
             }

            if ( (DEFAULT == nextToken.lexeme) || (CASE_ELSE == nextToken.lexeme) )
             {
               CaseContainer * newCase = new CaseContainer();
               op->cases.push_back(newCase);

               newCase->breaking = (DEFAULT == nextToken.lexeme);
               newCase->seq = new StatementSeq();

               if (DEFAULT == nextToken.lexeme)
                  GNT();
               else
                  expect(CASE_ELSE);
               expect(NEW_LINE);

               while (END_SELECT != nextToken.lexeme)
                {
                  Statement * statmnt = statement(context);

                  if (NULL != statmnt)
                     newCase->seq->statements.push_back(statmnt);

                  expect(NEW_LINE);
                }
             }

            expect(END_SELECT);
          }
         break;

      case RETURN:
          {
            ReturnStatement * op = new ReturnStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (NEW_LINE != nextToken.lexeme)
             {
               op->value = expression(context);
             }
          }
         break;

      case CALL:
          {
            CallStatement * op = new CallStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (IDENTIFIER == nextToken.lexeme)
             {
               if ( (CallingContext::FUNCTION == context.lookup(nextToken.text)) ||
                    (CallingContext::STANDARD_FUNCTION == context.lookup(nextToken.text)) )
                {
                  op->fun = primary(context);
                }
             }

            if (NULL == op->fun)
               DB_panic("Invalid call to \"" + nextToken.text + "\" on " + LN() + ".");
          }
         break;

      case TAILCALL:
          {
            TailCallStatement * op = new TailCallStatement();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (NEW_LINE != nextToken.lexeme)
             {
               op->args.push_back(expression(context));

               while (SEMICOLON == nextToken.lexeme)
                {
                  GNT();

                  op->args.push_back(expression(context));
                }
             }

            if (context.Functions().find(context.Name)->second.size() != op->args.size())
               DB_panic("Wrong number of arguments in tailcall for \"" + context.Name + "\" on " + LN() + ".");
          }
         break;

      default: break;
    }

   if ("" != label)
      DB_panic("Invalid label on or before " + LN() + ".");

   return ret.get();
 }

void DB_panic (const std::string &, const CallingContext &, size_t) __attribute__ ((__noreturn__));

ValueType::ValueHolder DB_eval (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in eval.", context, lineNo);

   Lexer lex (static_cast<StringValue*>(arg.data)->val);
   Parser parse (lex);
   PointerWrapper<Expression> expression (parse.ParseExpression(const_cast<CallingContext&>(context)));
   return expression->evaluate(const_cast<CallingContext&>(context));
 }

ValueType::ValueHolder DB_define (const ValueType::ValueHolder & arg, const CallingContext & context, size_t lineNo)
 {
   if ( (NULL == arg.data) || (ValueType::STRING != arg.data->type) )
      DB_panic("Bad data type in define.", context, lineNo);

   Lexer lex (static_cast<StringValue*>(arg.data)->val);
   Parser parse (lex);
   parse.ParseFunction(const_cast<CallingContext&>(context));
   return ValueType::ValueHolder();
 }
