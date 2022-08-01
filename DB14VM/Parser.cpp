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
#include "Parser.hpp"

#include "SymbolTable.hpp"
#include "StackOp.hpp"

#include <sstream>

ValueType::ValueHolder RecursiveInterpreter (const InstructionStream &, StackFrame &, bool = true);

void DB_panic (const std::string &) __attribute__ ((__noreturn__));

extern const DecFloat::Float DBTrue , DBFalse;

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

class BackReferenceJumpFill
 {
   public:
      IndexedStackOperation * op;
      std::string alias;
      bool isContinue;

      BackReferenceJumpFill (const BackReferenceJumpFill & src) :
         op(src.op), alias(src.alias), isContinue(src.isContinue) { }

      BackReferenceJumpFill (IndexedStackOperation * op, bool isContinue) :
         op(op), isContinue(isContinue) { }

      BackReferenceJumpFill & operator= (const BackReferenceJumpFill & src)
       { op = src.op; alias = src.alias; isContinue = src.isContinue; return *this; }
 };

static inline void push_back(InstructionStream & dest, StackOperation * item)
 {
   dest.push_back(std::make_pair(item, item->type));
 }





void Parser::expression (CallingContext & context, InstructionStream & dest)
 {
   StackOperation * temp = NULL;

   boolean(context, dest);

   while ((AND_OP == nextToken.lexeme) || (OR_OP == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case AND_OP:
            temp =  new AndOp(nextToken.lineNumber);
            break;
         case OR_OP:
            temp = new OrOp(nextToken.lineNumber);
            break;
         default: break;
       }

      GNT();

      boolean(context, dest);
      push_back(dest, temp);
    }
 }

void Parser::boolean (CallingContext & context, InstructionStream & dest)
 {
   IndexedStackOperation * branch;

   clause(context, dest);

   while (SHORT_OR == nextToken.lexeme)
    {
      branch = new Branch(nextToken.lineNumber);
      push_back(dest, new Copy(nextToken.lineNumber));
      push_back(dest, new Not(nextToken.lineNumber));
      push_back(dest, branch);
      push_back(dest, new Pop(nextToken.lineNumber));

      GNT();

      clause(context, dest);
      // Branch to next instruction.
      branch->index = dest.size();
      push_back(dest, new ForceLogical(branch->lineNumber));
    }
 }

void Parser::clause (CallingContext & context, InstructionStream & dest)
 {
   IndexedStackOperation * branch;

   predicate(context, dest);

   while (SHORT_AND == nextToken.lexeme)
    {
      branch = new Branch(nextToken.lineNumber);
      push_back(dest, new Copy(nextToken.lineNumber));
      push_back(dest, branch);
      push_back(dest, new Pop(nextToken.lineNumber));

      GNT();

      predicate(context, dest);
      branch->index = dest.size();
      push_back(dest, new ForceLogical(branch->lineNumber));
    }
 }

void Parser::predicate (CallingContext & context, InstructionStream & dest)
 {
   StackOperation * temp = NULL;

   relation(context, dest);

   while ((EQUALITY == nextToken.lexeme) || (INEQUALITY == nextToken.lexeme) || (EQUAL_SIGN == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case EQUAL_SIGN:
         case EQUALITY:
            temp = new Equals(nextToken.lineNumber);
            break;
         case INEQUALITY:
            temp = new NotEquals(nextToken.lineNumber);
            break;
         default: break;
       }

      GNT();

      relation(context, dest);
      push_back(dest, temp);
    }
 }

void Parser::relation (CallingContext & context, InstructionStream & dest)
 {
   StackOperation * temp = NULL;

   simple(context, dest);

   while ((GREATER_THAN == nextToken.lexeme) || (LESS_THAN_OR_EQUAL_TO == nextToken.lexeme) ||
          (GREATER_THAN_OR_EQUAL_TO == nextToken.lexeme) || (LESS_THAN == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case GREATER_THAN:
            temp = new Greater(nextToken.lineNumber);
            break;
         case LESS_THAN:
            temp = new Less(nextToken.lineNumber);
            break;
         case GREATER_THAN_OR_EQUAL_TO:
            temp = new GEQ(nextToken.lineNumber);
            break;
         case LESS_THAN_OR_EQUAL_TO:
            temp = new LEQ(nextToken.lineNumber);
            break;
         default: break;
       }

      GNT();

      simple(context, dest);
      push_back(dest, temp);
    }
 }

void Parser::simple (CallingContext & context, InstructionStream & dest)
 {
   StackOperation * temp = NULL;

   term(context, dest);

   while ((PLUS == nextToken.lexeme) || (MINUS == nextToken.lexeme) || (STRING_CAT == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case PLUS:
            temp = new Plus(nextToken.lineNumber);
            break;
         case MINUS:
            temp = new Minus(nextToken.lineNumber);
            break;
         case STRING_CAT:
            temp = new StringCat(nextToken.lineNumber);
            break;
         default: break;
       }

      GNT();

      term(context, dest);
      push_back(dest, temp);
    }
 }

void Parser::term (CallingContext & context, InstructionStream & dest)
 {
   StackOperation * temp = NULL;

   factor(context, dest);

   while ((MULTIPLY == nextToken.lexeme) || (DIVIDE == nextToken.lexeme) || (REMAINDER == nextToken.lexeme))
    {
      switch(nextToken.lexeme)
       {
         case MULTIPLY:
            temp = new Multiply(nextToken.lineNumber);
            break;
         case DIVIDE:
            temp = new Divide(nextToken.lineNumber);
            break;
         case REMAINDER:
            temp = new Remainder(nextToken.lineNumber);
            break;
         default: break;
       }

      GNT();

      factor(context, dest);
      push_back(dest, temp);
    }
 }

void Parser::factor (CallingContext & context, InstructionStream & dest)
 {
   StackOperation * temp = NULL;

   primary(context, dest);

   while (POWER == nextToken.lexeme)
    {
      temp = new Power(nextToken.lineNumber);

      GNT();

      primary(context, dest);
      push_back(dest, temp);
    }
 }

void Parser::primary (CallingContext & context, InstructionStream & dest)
 {
   switch(nextToken.lexeme)
    {
      case IDENTIFIER:
         switch(context.lookup(nextToken.text))
          {
            case CallingContext::VARIABLE:
               switch (context.location(nextToken.text))
                {
                  case CallingContext::LOCAL:
                      {
                        LdLocalVar * op = new LdLocalVar(nextToken.lineNumber);
                        op->index = context.Locals[nextToken.text];
                        GNT();
                        push_back(dest, op);
                      }
                     break;
                  case CallingContext::STATIC:
                      {
                        LdStaticVar * op = new LdStaticVar(nextToken.lineNumber);
                        op->index = context.Statics[nextToken.text];
                        GNT();
                        push_back(dest, op);
                      }
                     break;
                  case CallingContext::GLOBAL:
                      {
                        LdGlobalVar * op = new LdGlobalVar(nextToken.lineNumber);
                        op->index = context.Globals[nextToken.text];
                        GNT();
                        push_back(dest, op);
                      }
                     break;
                  case CallingContext::NOT_A_VARIABLE:
                     break;
                }
               break;
            case CallingContext::CONSTANT:
                {
                  Constant * op = new Constant(nextToken.lineNumber);
                  op->value = context.Constants[nextToken.text];
                  GNT();
                  push_back(dest, op);
                }
               break;
            case CallingContext::FUNCTION:
                {
                  FunCall * op = new FunCall(nextToken.lineNumber);
                  std::string funName (nextToken.text);
                  op->index = context.FunDefs[funName];
                  op->nargs = context.Functions[funName].size();
                  GNT();

                  expect(OPEN_PARENS);

                  size_t argCount = 0;
                  if (CLOSE_PARENS != nextToken.lexeme)
                   {
                     expression(context, dest);
                     ++argCount;

                     while (SEMICOLON == nextToken.lexeme)
                      {
                        GNT();
                        expression(context, dest);
                        ++argCount;
                      }
                   }

                  expect(CLOSE_PARENS);

                  if (argCount != op->nargs)
                     DB_panic("Call to \"" + funName + "\" with bad number of arguments on " + LN() + ".");
                  push_back(dest, op);
                }
               break;
            case CallingContext::STANDARD_FUNCTION:
               switch(context.getStandardFunctionType(nextToken.text))
                {
                  case CallingContext::CONSTANT_FUNCTION:
                      {
                        StdConstFun * op = new StdConstFun(nextToken.lineNumber);
                        op->function = context.getConstantFunction(nextToken.text);
                        GNT();

                        expect(OPEN_PARENS);
                        expect(CLOSE_PARENS);
                        push_back(dest, op);
                      }
                     break;
                  case CallingContext::UNARY_FUNCTION:
                      {
                        StdUnaryFun * op = new StdUnaryFun(nextToken.lineNumber);
                        op->function = context.getUnaryFunction(nextToken.text);
                        GNT();

                        expect(OPEN_PARENS);
                        expression(context, dest);
                        expect(CLOSE_PARENS);
                        push_back(dest, op);
                      }
                     break;
                  case CallingContext::BINARY_FUNCTION:
                      {
                        StdBinaryFun * op = new StdBinaryFun(nextToken.lineNumber);
                        op->function = context.getBinaryFunction(nextToken.text);
                        GNT();

                        expect(OPEN_PARENS);
                        expression(context, dest);
                        expect(SEMICOLON);
                        expression(context, dest);
                        expect(CLOSE_PARENS);
                        push_back(dest, op);
                      }
                     break;
                  case CallingContext::TERNARY_FUNCTION:
                      {
                        StdTernaryFun * op = new StdTernaryFun(nextToken.lineNumber);
                        op->function = context.getTernaryFunction(nextToken.text);
                        GNT();

                        expect(OPEN_PARENS);
                        expression(context, dest);
                        expect(SEMICOLON);
                        expression(context, dest);
                        expect(SEMICOLON);
                        expression(context, dest);
                        expect(CLOSE_PARENS);
                        push_back(dest, op);
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
            Abs * op = new Abs(nextToken.lineNumber);
            GNT();

            primary(context, dest);
            push_back(dest, op);
          }
         break;
      case MINUS:
          {
            Negate * op = new Negate(nextToken.lineNumber);
            GNT();

            primary(context, dest);
            push_back(dest, op);
          }
         break;
      case NOT:
          {
            Not * op = new Not(nextToken.lineNumber);
            GNT();

            primary(context, dest);
            push_back(dest, op);
          }
         break;
      case NUMBER:
          {
            Constant * op = new Constant(nextToken.lineNumber);

            DecFloat::Float val = DecFloat::Float(nextToken.text);
            if (val.getPrecision() < DecFloat::Float::getMinPrecision())
             {
               val = DecFloat::pi(DecFloat::Float::getMinPrecision());
               val.fromString(nextToken.text);
             }
            else if (val.getPrecision() > DecFloat::Float::getMaxPrecision())
             {
               val = DecFloat::pi(DecFloat::Float::getMaxPrecision());
               val.fromString(nextToken.text);
             }

            op->value = ValueType::ValueHolder(new NumericValue(val));

            GNT();

            push_back(dest, op);
          }
         break;
      case STRING:
          {
            Constant * op = new Constant(nextToken.lineNumber);

            op->value = ValueType::ValueHolder(new StringValue(nextToken.text));

            GNT();

            push_back(dest, op);
          }
         break;
      case OPEN_PARENS:
         GNT();
         expression(context, dest);
         expect(CLOSE_PARENS);
         break;
      default:
         DB_panic("Expected primary expression, found \"" + nextToken.text + "\" on " + LN() + ".");
    }

   // Handle this here, despite NUMEROUS instances where this DOES NOT MAKE SENSE.
   //    Chewbacca statement: "Hello, World!"[5]
   while (LEFT_BRACKET == nextToken.lexeme)
    {
      LDI * op = new LDI(nextToken.lineNumber);
      GNT();

      expression(context, dest);
      push_back(dest, op);

      expect(RIGHT_BRACKET);
    }
 }





void Parser::program (CallingContext & context)
 {
   constants(context);
   globals(context);
   functions(context);
   expect(END_OF_FILE);
 }

ValueType::ValueHolder Parser::getConstantExpressionValue(CallingContext & context)
 {
   InstructionStream tempDest;

   expression(context, tempDest);

   StackFrame tempFrame (context.PopGlobals, context.PopFunctions, context.PopFunNames, context.PopFunLocals);

   ValueType::ValueHolder val = RecursiveInterpreter(tempDest, tempFrame, false);

   for (InstructionStream::iterator iter = tempDest.begin();
      tempDest.end() != iter; ++iter)
    {
      delete (*iter).first;
    }

   return val;
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

         ValueType::ValueHolder val = getConstantExpressionValue(context);

         context.Constants.insert(std::make_pair(name, val));

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

                  ValueType::ValueHolder index = getConstantExpressionValue(context);

                  if ( (NULL == index.data) || (ValueType::NUMBER != index.data->type) )
                     DB_panic ("Bad data type in array subscript on " + LN() + ".");

                  depth.push_back(DecFloat::fromFloat(static_cast<NumericValue*>(index.data)->val));

                  expect(RIGHT_BRACKET);
                }
               while (LEFT_BRACKET == nextToken.lexeme);

               val = context.createVariable(depth);
             }

            context.Globals.insert(std::make_pair(name, context.PopGlobals[0].size()));
            context.PopGlobals[0].push_back(val);
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

void Parser::variables (CallingContext & context, size_t numberOfArguments)
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

                  ValueType::ValueHolder index = getConstantExpressionValue(context);

                  if ( (NULL == index.data) || (ValueType::NUMBER != index.data->type) )
                     DB_panic ("Bad data type in array subscript on " + LN() + ".");

                  depth.push_back(DecFloat::fromFloat(static_cast<NumericValue*>(index.data)->val));

                  expect(RIGHT_BRACKET);
                }
               while (LEFT_BRACKET == nextToken.lexeme);
             }

            ValueType::ValueHolder val = context.createVariable(depth);

            if (true == isStatic)
             {
               context.Statics.insert(std::make_pair(name, context.PopGlobals[context.FunctionIndex].size()));
               context.PopGlobals[context.FunctionIndex].push_back(val);
             }
            else
             {
               context.Locals.insert(std::make_pair(name,
                  context.PopFunLocals[context.FunctionIndex].size() + numberOfArguments));
               context.PopFunLocals[context.FunctionIndex].push_back(val);
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
            if ((0U != context.PopFunctions[context.FunDefs[name]].size()) && (false == isDeclare))
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
            std::vector<std::string> & check = context.Functions.find(name)->second;

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
            context.Functions.insert(std::make_pair(name, args));
            context.FunDefs.insert(std::make_pair(name, context.PopFunctions.size()));
            context.PopFunctions.resize(context.PopFunctions.size() + 1);
            context.PopFunNames.push_back(name);
            context.PopFunLocals.resize(context.PopFunLocals.size() + 1);
          }
         expect(NEW_LINE);

         // This is a define, so define the function.
         if (false == isDeclare)
          {
            // Build a new calling context, inserting dummy locals.
            CallingContext newFunction (context, name);

            for (size_t iter = 0U; iter < args.size(); ++iter)
             {
               newFunction.Locals.insert(std::make_pair(args[iter], iter));
             }

            variables(newFunction, args.size());

            std::vector<BackReferenceJumpFill> emptyVec;
            while (END_FUNCTION != nextToken.lexeme)
             {
               while (NEW_LINE == nextToken.lexeme)
                  GNT();

               statement(newFunction, context.PopFunctions[newFunction.FunctionIndex], emptyVec);

               expect(NEW_LINE);
             }
            GNT();

            if (0U != emptyVec.size())
               DB_panic("Break or continue propagated out of function \"" + name + "\" ending on " + LN() + ".");
            if (0U == context.PopFunctions[newFunction.FunctionIndex].size())
               DB_panic("Empty definition of function \"" + name + "\" ending on " + LN() + ".");
          }
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }





void Parser::statement
   (CallingContext & context, InstructionStream & dest, std::vector<BackReferenceJumpFill> & backRefs)
 {
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
            IndexedStackOperation * ld = NULL;
            IndexedStackOperation * st = NULL;

            if (CallingContext::VARIABLE != context.lookup(nextToken.text))
               DB_panic("L-value \"" + nextToken.text + "\" not a variable on " + LN() + ".");

            switch (context.location(nextToken.text))
             {
               case CallingContext::LOCAL:
                  ld = new LdLocalVar(nextToken.lineNumber);
                  st = new StLocalVar(nextToken.lineNumber);
                  ld->index = context.Locals[nextToken.text];
                  st->index = ld->index;
                  break;
               case CallingContext::STATIC:
                  ld = new LdStaticVar(nextToken.lineNumber);
                  st = new StStaticVar(nextToken.lineNumber);
                  ld->index = context.Statics[nextToken.text];
                  st->index = ld->index;
                  break;
               case CallingContext::GLOBAL:
                  ld = new LdGlobalVar(nextToken.lineNumber);
                  st = new StGlobalVar(nextToken.lineNumber);
                  ld->index = context.Globals[nextToken.text];
                  st->index = ld->index;
                  break;
               default: break;
             }
            GNT();


            std::vector<InstructionStream> subs;

            fillSubscripts(context, subs);
            // false means dest takes ownership of instructions
            writeCodeLoadForStore(dest, subs, ld, false);

            if (EQUAL_SIGN == nextToken.lexeme)
               GNT();
            else
               expect(ASSIGNMENT);

            expression(context, dest);

            writeCodeStore(dest, subs, st, false);

            // Delete load if it is unused.
            if (true == subs.empty())
               delete ld;
          }
         break;

      case IF:
          {
            Branch * op = new Branch(nextToken.lineNumber);
            GNT();

            expression(context, dest);
            push_back(dest, op);

            expect(THEN);

            if (NEW_LINE == nextToken.lexeme)
             {
               GNT();

               while ((ELSE != nextToken.lexeme) && (FI != nextToken.lexeme))
                {
                  statement(context, dest, backRefs);

                  expect(NEW_LINE);
                }

               if (ELSE == nextToken.lexeme)
                {
                  Jump * overElse = new Jump(nextToken.lineNumber);
                  push_back(dest, overElse);
                  op->index = dest.size();
                  GNT();

                  std::vector<Jump*> overElses;
                  overElses.push_back(overElse);

                  while (IF == nextToken.lexeme)
                   {
                     op = new Branch(nextToken.lineNumber);
                     GNT();

                     expression(context, dest);
                     push_back(dest, op);

                     expect(THEN);
                     expect(NEW_LINE);

                     while ((ELSE != nextToken.lexeme) && (FI != nextToken.lexeme))
                      {
                        statement(context, dest, backRefs);

                        expect(NEW_LINE);
                      }

                     if (ELSE == nextToken.lexeme)
                      {
                        Jump * overElse = new Jump(nextToken.lineNumber);
                        push_back(dest, overElse);
                        overElses.push_back(overElse);
                        GNT();
                      }
                     op->index = dest.size();
                   }

                   // At this point, we are either at FI
                   // Or a NEW_LINE. NEW_LINE implies ELSE.
                  if (NEW_LINE == nextToken.lexeme)
                   {
                     GNT();

                     while (FI != nextToken.lexeme)
                      {
                        statement(context, dest, backRefs);

                        expect(NEW_LINE);
                      }
                   }

                  for (std::vector<Jump*>::iterator iter = overElses.begin();
                     overElses.end() != iter; ++iter)
                   {
                     (*iter)->index = dest.size();
                   }
                }
               else
                {
                  op->index = dest.size();
                }

               expect(FI);
             }
            else
             {
               statement(context, dest, backRefs);

               if (ELSE == nextToken.lexeme)
                {
                  Jump * overElse = new Jump(nextToken.lineNumber);
                  push_back(dest, overElse);
                  op->index = dest.size();
                  GNT();
                  statement(context, dest, backRefs);
                  overElse->index = dest.size();
                }
               else
                {
                  op->index = dest.size();
                }
             }
          }
         break;

      case DO:
          {
            size_t tol = dest.size();
            GNT();

            // Push a new label to the stack so the symbol table can remember these.
            context.Labels.push_back(label);

            Branch * preCondition = NULL;
            if (WHILE == nextToken.lexeme)
             {
               preCondition = new Branch(nextToken.lineNumber);
               GNT();

               expression(context, dest);
               push_back(dest, preCondition);
             }
            else if (UNTIL == nextToken.lexeme)
             {
               preCondition = new Branch(nextToken.lineNumber);
               GNT();

               expression(context, dest);
               push_back(dest, new Not(preCondition->lineNumber));
               push_back(dest, preCondition);
             }

            expect(NEW_LINE);

            std::vector<BackReferenceJumpFill> references;
            while (LOOP != nextToken.lexeme)
             {
               statement(context, dest, references);

               expect(NEW_LINE);
             }
            GNT();

            // Fill the continues.
            for (std::vector<BackReferenceJumpFill>::iterator iter = references.begin();
               references.end() != iter; ++iter)
             {
               if ((((*iter).alias == "") || ((*iter).alias == label)) && (true == (*iter).isContinue))
                  (*iter).op->index = dest.size();
             }

            // Generate post-condition and jump/branch.
            if (WHILE == nextToken.lexeme)
             {
               Branch * postCondition = new Branch(nextToken.lineNumber);
               postCondition->index = tol;
               GNT();

               expression(context, dest);
               push_back(dest, new Not(postCondition->lineNumber));
               push_back(dest, postCondition);
             }
            else if (UNTIL == nextToken.lexeme)
             {
               Branch * postCondition = new Branch(nextToken.lineNumber);
               postCondition->index = tol;
               GNT();

               expression(context, dest);
               push_back(dest, postCondition);
             }
            else
             {
               Jump * toTop = new Jump(nextToken.lineNumber);
               toTop->index = tol;
               push_back(dest, toTop);
             }

            for (std::vector<BackReferenceJumpFill>::iterator iter = references.begin();
               references.end() != iter; ++iter)
             {
               // Fill the breaks.
               if ((((*iter).alias == "") || ((*iter).alias == label)) && (false == (*iter).isContinue))
                  (*iter).op->index = dest.size();
               // Pass on everything else
               if (((*iter).alias != "") && ((*iter).alias != label))
                  backRefs.push_back(*iter);
             }

            // Fill the pre-condition.
            if (NULL != preCondition)
               preCondition->index = dest.size();

            // Pop the label stack.
            context.Labels.pop_back();
            // Flag the label as used.
            label = "";
          }
         break;

      case BREAK:
          {
            BackReferenceJumpFill op (new Jump(nextToken.lineNumber), false);
            GNT();

            if (0 == context.Labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op.alias = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op.alias))
                  DB_panic("Use of undefined label \"" + op.alias + "\" on " + LN() + ".");

               expect(COLON);
             }

            push_back(dest, op.op);
            backRefs.push_back(op);
          }
         break;

      case CONTINUE:
          {
            BackReferenceJumpFill op (new Jump(nextToken.lineNumber), true);
            GNT();

            if (0 == context.Labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op.alias = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op.alias))
                  DB_panic("Use of undefined label \"" + op.alias + "\" on " + LN() + ".");

               expect(COLON);
             }

            push_back(dest, op.op);
            backRefs.push_back(op);
          }
         break;

      case WHILE:
          {
            BackReferenceJumpFill op (new Branch(nextToken.lineNumber), false);
            GNT();

            if (0 == context.Labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op.alias = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op.alias))
                  DB_panic("Use of undefined label \"" + op.alias + "\" on " + LN() + ".");

               expect(COLON);
             }

            expression(context, dest);
            push_back(dest, op.op);
            backRefs.push_back(op);
          }
         break;

      case UNTIL:
          {
            BackReferenceJumpFill op (new Branch(nextToken.lineNumber), false);
            GNT();

            if (0 == context.Labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op.alias = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op.alias))
                  DB_panic("Use of undefined label \"" + op.alias + "\" on " + LN() + ".");

               expect(COLON);
             }

            expression(context, dest);
            push_back(dest, new Not(op.op->lineNumber));
            push_back(dest, op.op);
            backRefs.push_back(op);
          }
         break;

      case FOR:
/*
   Code structure:
    1 Load initial value with expression
    2 Compare control value to termination expression
    3 Jump to Statements (6) or End Of Loop (8)
    4 Increment Control Value by Delta Value
    5 Jump to Comparison (2)
    6 Statements
    7 Jump to Increment (4)
    8 End Of Loop
*/
          {
            GNT();

            // Push a new label to the stack so the symbol table can remember these.
            context.Labels.push_back(label);

            if (CallingContext::VARIABLE != context.lookup(nextToken.text))
               DB_panic("L-value \"" + nextToken.text + "\" not a variable on " + LN() + ".");

            IndexedStackOperation * ld = NULL;
            IndexedStackOperation * st = NULL;
            switch (context.location(nextToken.text))
             {
               case CallingContext::LOCAL:
                  ld = new LdLocalVar(nextToken.lineNumber);
                  st = new StLocalVar(nextToken.lineNumber);
                  ld->index = context.Locals[nextToken.text];
                  st->index = ld->index;
                  break;
               case CallingContext::STATIC:
                  ld = new LdStaticVar(nextToken.lineNumber);
                  st = new StStaticVar(nextToken.lineNumber);
                  ld->index = context.Statics[nextToken.text];
                  st->index = ld->index;
                  break;
               case CallingContext::GLOBAL:
                  ld = new LdGlobalVar(nextToken.lineNumber);
                  st = new StGlobalVar(nextToken.lineNumber);
                  ld->index = context.Globals[nextToken.text];
                  st->index = ld->index;
                  break;
               default: break;
             }
            GNT();


            std::vector<InstructionStream> subs;
            fillSubscripts(context, subs);
            // false means dest takes ownership of instructions
            writeCodeLoadForStore(dest, subs, ld, false);

            if (EQUAL_SIGN == nextToken.lexeme)
               GNT();
            else
               expect(ASSIGNMENT);

            expression(context, dest);

            writeCodeStore(dest, subs, st, false);


            // Create while (lcv O ltv) [loop control variable, loop termination value]
            StackOperation * comparison = NULL;
            if (DOWNTO == nextToken.lexeme) // Do the opposite
             { // DOWNTO -> lcv >= ltv
               comparison = new GEQ(nextToken.lineNumber);
               GNT();
             }
            else
             { // TO -> lcv <= ltv
               comparison = new LEQ(nextToken.lineNumber);
               expect(TO);
             }

            size_t beginingOfComparison = dest.size();

            if (true == subs.empty())
               push_back(dest, ld);
            else
               push_back(dest, ld->Clone());
            writeCodeLoadForRead(dest, subs, nextToken.lineNumber);

            expression(context, dest);

            push_back(dest, comparison);

            Branch * toEndOfLoop = new Branch(nextToken.lineNumber);
            Jump * toStatements = new Jump(nextToken.lineNumber);

            push_back(dest, toEndOfLoop);
            push_back(dest, toStatements);

            size_t beginingOfDelta = dest.size();
            // Create lcv = lcv + ldv [loop delta value]
            // Somewhere else alread owns the instruction, so clone them.
            writeCodeLoadForStore(dest, subs, ld, true);
            push_back(dest, ld->Clone());
            writeCodeLoadForRead(dest, subs, nextToken.lineNumber);
            if (STEP == nextToken.lexeme)
             {
               GNT();
               expression(context, dest);
             }
            else
             {
               Constant * step = new Constant(nextToken.lineNumber);
               if (StackOperation::LESS_THAN_OR_EQUAL_TO == comparison->type)
                {
                  step->value = ValueType::ValueHolder(new NumericValue(DBTrue));
                }
               else
                {
                  step->value = ValueType::ValueHolder(new NumericValue(-DBTrue));
                }
               push_back(dest, step);
             }
            push_back(dest, new Plus(nextToken.lineNumber));
            writeCodeStore(dest, subs, st, true);
            Jump * toComparison = new Jump (nextToken.lineNumber);
            toComparison->index = beginingOfComparison;
            push_back(dest, toComparison);
            expect(NEW_LINE); // Home Video

            toStatements->index = dest.size();
            std::vector<BackReferenceJumpFill> references;
            while (NEXT != nextToken.lexeme)
             {
               statement(context, dest, references);

               expect(NEW_LINE);
             }
            Jump * toDelta = new Jump(nextToken.lineNumber);
            toDelta->index = beginingOfDelta;
            push_back(dest, toDelta);
            GNT();

            toEndOfLoop->index = dest.size();

            for (std::vector<BackReferenceJumpFill>::iterator iter = references.begin();
               references.end() != iter; ++iter)
             {
               // Fill the breaks.
               if ((((*iter).alias == "") || ((*iter).alias == label)) && (false == (*iter).isContinue))
                  (*iter).op->index = dest.size();
               // Fill the continues: jump to the delta.
               if ((((*iter).alias == "") || ((*iter).alias == label)) && (true == (*iter).isContinue))
                  (*iter).op->index = beginingOfDelta;
               // Pass on everything else
               if (((*iter).alias != "") && ((*iter).alias != label))
                  backRefs.push_back(*iter);
             }

            // Pop the label stack.
            context.Labels.pop_back();
            label = "";
          }
         break;

      case SELECT:
          {
            GNT();

            expression(context, dest);

            IndexedStackOperation * lastCase = new Jump(nextToken.lineNumber);
            push_back(dest, lastCase);
            expect(NEW_LINE);

            std::vector<IndexedStackOperation *> toEnds;
            while ( (END_SELECT != nextToken.lexeme) &&
                    (DEFAULT != nextToken.lexeme) && (CASE_ELSE != nextToken.lexeme) )
             {
               Jump * overTest = NULL;
               if (CHOICE == nextToken.lexeme)
                {
                  overTest = new Jump (nextToken.lineNumber);
                  push_back(dest, overTest);
                  GNT();
                }
               else
                {
                  Jump * toEnd = new Jump (nextToken.lineNumber);
                  toEnds.push_back(toEnd);
                  push_back(dest, toEnd);
                  expect(CASE);
                }

               lastCase->index = dest.size();

               push_back(dest, new Copy(nextToken.lineNumber));
               expression(context, dest);
               push_back(dest, new Equals(nextToken.lineNumber));
               lastCase = new Branch(nextToken.lineNumber);
               push_back(dest, lastCase);
               push_back(dest, new Pop(nextToken.lineNumber));

               if (NULL != overTest)
                {
                  overTest->index = dest.size();
                }

               expect(NEW_LINE);

               while ( (CASE != nextToken.lexeme) && (CHOICE != nextToken.lexeme) &&
                       (DEFAULT != nextToken.lexeme) && (CASE_ELSE != nextToken.lexeme) &&
                       (END_SELECT != nextToken.lexeme) )
                {
                  statement(context, dest, backRefs);

                  expect(NEW_LINE);
                }
             }

            if ( (DEFAULT == nextToken.lexeme) || (CASE_ELSE == nextToken.lexeme) )
             {
               Jump * overPop = NULL;
               if (DEFAULT == nextToken.lexeme)
                {
                  overPop = new Jump (nextToken.lineNumber);
                  push_back(dest, overPop);
                  GNT();
                }
               else
                {
                  Jump * toEnd = new Jump (nextToken.lineNumber);
                  toEnds.push_back(toEnd);
                  push_back(dest, toEnd);
                  expect(CASE_ELSE);
                }

               lastCase->index = dest.size();

               push_back(dest, new Pop(nextToken.lineNumber));

               if (NULL != overPop)
                {
                  overPop->index = dest.size();
                }

               expect(NEW_LINE);

               while (END_SELECT != nextToken.lexeme)
                {
                  statement(context, dest, backRefs);

                  expect(NEW_LINE);
                }
             }
            else
             {
               Jump * overPop = new Jump (nextToken.lineNumber);
               push_back(dest, overPop);
               lastCase->index = dest.size();
               push_back(dest, new Pop(nextToken.lineNumber));
               overPop->index = dest.size();
             }

            for (std::vector<IndexedStackOperation *>::iterator iter = toEnds.begin();
               toEnds.end() != iter; ++iter)
             {
               (*iter)->index = dest.size();
             }

            expect(END_SELECT);
          }
         break;

      case RETURN:
          {
            Return * op = new Return(nextToken.lineNumber);
            GNT();

            if (NEW_LINE != nextToken.lexeme)
             {
               expression(context, dest);
             }
            else
             {
               push_back(dest, new Constant(nextToken.lineNumber));
             }
            push_back(dest, op);
          }
         break;

      case CALL:
          {
            Pop * op = new Pop(nextToken.lineNumber);
            GNT();

            if (IDENTIFIER == nextToken.lexeme)
             {
               if ( (CallingContext::FUNCTION == context.lookup(nextToken.text)) ||
                    (CallingContext::STANDARD_FUNCTION == context.lookup(nextToken.text)) )
                {
                  primary(context, dest);
                }
             }

            push_back(dest, op);
          }
         break;

      case TAILCALL:
          {
            TailCall * op = new TailCall(nextToken.lineNumber);
            GNT();

            size_t nargs = 0U;
            if (NEW_LINE != nextToken.lexeme)
             {
               expression(context, dest);
               ++nargs;

               while (SEMICOLON == nextToken.lexeme)
                {
                  GNT();

                  expression(context, dest);
                  ++nargs;
                }
             }

            if (context.Functions[context.Name].size() != nargs)
               DB_panic("Wrong number of arguments in tailcall for \"" + context.Name + "\" on " + LN() + ".");
            op->nargs = nargs;
            push_back(dest, op);
          }
         break;

      default: break;
    }

   if ("" != label)
      DB_panic("Invalid label on or before " + LN() + ".");
 }




void Parser::fillSubscripts (CallingContext & context, std::vector<InstructionStream> & stack)
 {
   while (LEFT_BRACKET == nextToken.lexeme)
    {
      GNT();

      stack.resize(stack.size() + 1);
      expression(context, stack[stack.size() - 1]);

      expect(RIGHT_BRACKET);
    }
 }

void Parser::writeCodeLoadForStore
   (InstructionStream & dest, std::vector<InstructionStream> & stack,
    StackOperation* load, bool clone)
 {
   // If there are no indexes, leave.
   if (0 == stack.size())
    {
      return;
    }

   // Generate load
   if (true == clone)
      push_back(dest, load->Clone());
   else
      push_back(dest, load);

   // For all but the last index : COPY, compute_index, COPY, ROTATE, LDI
   for (size_t iter = 0; iter < (stack.size() - 1); ++iter)
    {
      push_back(dest, new Copy(load->lineNumber));
      for (InstructionStream::iterator copier = stack[iter].begin();
         stack[iter].end() != copier; ++copier)
       {
         if (true == clone)
            push_back(dest, (*copier).first->Clone());
         else
            push_back(dest, (*copier).first);
       }
      push_back(dest, new Copy(load->lineNumber));
      push_back(dest, new Rot(load->lineNumber));
      push_back(dest, new LDI(load->lineNumber));
    }
   // For the last index, just compute the index.
   for (InstructionStream::iterator copier = stack[stack.size() - 1].begin();
      stack[stack.size() - 1].end() != copier; ++copier)
    {
      if (true == clone)
         push_back(dest, (*copier).first->Clone());
      else
         push_back(dest, (*copier).first);
    }
 }

void Parser::writeCodeStore
   (InstructionStream & dest, std::vector<InstructionStream> & stack,
    StackOperation* store, bool clone)
 {
   for (size_t iter = 0; iter < stack.size(); ++iter)
    {
      push_back(dest, new STI(store->lineNumber));
    }

   if (true == clone)
      push_back(dest, store->Clone());
   else
      push_back(dest, store);
 }

void Parser::writeCodeLoadForRead
   (InstructionStream & dest, std::vector<InstructionStream> & stack, size_t lineNo)
 {
   for (std::vector<InstructionStream>::iterator iter = stack.begin();
      stack.end() != iter; ++iter)
    {
      for (InstructionStream::iterator copier = iter->begin();
         iter->end() != copier; ++copier)
       {
         push_back(dest, (*copier).first->Clone());
       }
      push_back(dest, new LDI(lineNo));
    }
 }
