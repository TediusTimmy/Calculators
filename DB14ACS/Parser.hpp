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
#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"

class CallingContext;
class StatementSeq;
class Statement;
class Expression;

class Parser /* Syntax Analyzer */ /* Analyser for the Bri'ish */
 {
   private:

      void expect (const Lexeme &);

      void program (CallingContext &);
      void constants (CallingContext &);
      void globals (CallingContext &);
      void variables (CallingContext &);
      void functions (CallingContext &);

      Statement * statement (CallingContext &);

      Expression * expression (CallingContext &);
      Expression * boolean (CallingContext &);
      Expression * clause (CallingContext &);
      Expression * predicate (CallingContext &);
      Expression * relation (CallingContext &);
      Expression * simple (CallingContext &);
      Expression * term (CallingContext &);
      Expression * factor (CallingContext &);
      Expression * primary (CallingContext &);

      Lexer & src;

      Token nextToken;

      void GNT (void);

      std::string LN (void);

   public:

      Parser (Lexer & input) : src(input), nextToken(0) { GNT(); }
      ~Parser() { }

      void Parse (CallingContext & context) { program(context); }
      Expression * ParseExpression (CallingContext & context) { return expression(context); }
      void ParseFunction (CallingContext & context) { functions(context); }
 };

#endif /* PARSER_HPP */
