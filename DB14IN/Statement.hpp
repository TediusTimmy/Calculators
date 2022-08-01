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
/*
   Statements:

 **  ASSIGNMENT
 **     ASSIGN_DEREFERENCE_VARIABLE Expression
 **  STATEMENT_SEQ (A simple container: iterates over contents)
 **  IF (Expression CONDITION, StatementSeq THEN, StatementSeq ELSE)
 **  DO (All of them, pre/post while/until/unconditional)
 **  BREAK (while/until/unconditional, also continue)
 **  FOR
 **  SELECT
 **     CASE (std::pair<Expression, StatementSeq>)
 **  RETURN
 **  TAILCALL
 **  CALL


   Exceptions for non-local control jumps:
      BREAK (std::string label, "" for inner-most loop)
      CONTINUE (Just like break. Maybe add a bool to break to make both the same.)
      RETURN (ValueType::ValueHolder returnValue)

   FUNCTION_CALL dies if its function never throws Exception::RETURN.
*/
#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "ValueType.hpp"
#include <string>
#include <vector>
#include <set>

class Expression;
class FunctionCall;
class CallingContext;
class FlowControl;

class Statement
 {
   public:
      size_t lineNo;

      virtual FlowControl * execute (CallingContext &) const = 0;
      virtual ~Statement() { }
 };

class RecAssignState
 {
   public:
      Expression * index;
      RecAssignState * next;

      // If rhs is NULL, just return the value. This is for ForStatement.
      ValueType::ValueHolder evaluate (CallingContext &, ValueType::ValueHolder lhs, const Expression * rhs) const;
      RecAssignState() : index(NULL), next(NULL) { }
      ~RecAssignState();

      static ValueType::ValueHolder getIndex (ValueType::ValueHolder array, ValueType::ValueHolder index);
      static ValueType::ValueHolder setIndex
         (ValueType::ValueHolder array, ValueType::ValueHolder index, ValueType::ValueHolder value);
 };

class Assignment : public Statement
 {
   public:
      std::string lhs;
      RecAssignState * index;
      Expression * rhs;

      FlowControl * execute (CallingContext &) const;
      Assignment() : index(NULL), rhs(NULL) { }
      ~Assignment();
 };

class StatementSeq : public Statement
 {
   public:
      std::vector<Statement*> statements;

      FlowControl * execute (CallingContext & context) const
       {
         for (std::vector<Statement*>::const_iterator iter = statements.begin();
            iter != statements.end(); ++iter)
          {
            FlowControl * test = (*iter)->execute(context);
            if (NULL != test) return test;
          }
         return NULL;
       }
      ~StatementSeq()
       {
         for (std::vector<Statement*>::iterator iter = statements.begin();
            iter != statements.end(); ++iter)
          {
            delete *iter;
          }
       }
 };

class IfStatement : public Statement
 {
   public:
      Expression * condition;
      Statement * thenSeq;
      Statement * elseSeq;

      FlowControl * execute (CallingContext &) const;
      IfStatement() : condition(NULL), thenSeq(NULL), elseSeq(NULL) { }
      ~IfStatement();
 };

class DoStatement : public Statement
 {
   public:
      std::string label;

      Expression * preCondition;
      Expression * postCondition;
      StatementSeq * seq;

      FlowControl * execute (CallingContext &) const;
      DoStatement() : preCondition(NULL), postCondition(NULL), seq(NULL) { }
      ~DoStatement();
 };

class BreakStatement : public Statement
 {
   public:
      Expression * condition;
      std::string label;
      bool toContinue;

      FlowControl * execute (CallingContext &) const;
      BreakStatement() : condition(NULL), toContinue(false) { }
      ~BreakStatement();
 };

class ForStatement : public Statement
 {
   public:
      std::string label;

      std::string lhs;
      RecAssignState * index;
      Expression * initialValue;
      bool to;
      Expression * termValue;
      Expression * stepSize;
      StatementSeq * seq;

      FlowControl * execute (CallingContext &) const;
      ForStatement() : index(NULL), initialValue(NULL), termValue(NULL), stepSize(NULL), seq(NULL) { }
      ~ForStatement();
 };

class ReturnStatement : public Statement
 {
   public:
      Expression * value;

      FlowControl * execute (CallingContext &) const;
      ReturnStatement() : value(NULL) { }
      ~ReturnStatement();
 };

class TailCallStatement : public Statement
 {
   public:
      std::vector<Expression*> args;

      FlowControl * execute (CallingContext &) const;
      TailCallStatement() { }
      ~TailCallStatement();
 };

class CallStatement : public Statement
 {
   public:
      Expression * fun;

      FlowControl * execute (CallingContext &) const;
      CallStatement() : fun(NULL) { }
      ~CallStatement();
 };

class CaseContainer
 {
   public:
      bool breaking;
      Expression * condition;
      StatementSeq * seq;

      CaseContainer() : breaking(true), condition(NULL), seq(NULL) { }
      ~CaseContainer();
 };

class SelectStatement : public Statement
 {
   public:
      Expression * control;
      std::vector<CaseContainer*> cases;

      FlowControl * execute (CallingContext &) const;
      SelectStatement() : control(NULL) { }
      ~SelectStatement();
 };

#endif /* STATEMENT_HPP */
