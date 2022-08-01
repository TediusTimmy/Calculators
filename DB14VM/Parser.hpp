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
#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

#include "Lexer.hpp"
#include "ValueType.hpp"
#include "BaseStackOp.hpp"

class CallingContext;
class BackReferenceJumpFill;

class Parser /* Syntax Analyzer */ /* Analyser for the Bri'ish */
 {
   private:

      void expect (const Lexeme &);

      void program (CallingContext &);
      void constants (CallingContext &);
      void globals (CallingContext &);
      void variables (CallingContext &, size_t);
      void functions (CallingContext &);

      void fillSubscripts (CallingContext &, std::vector<InstructionStream> &);
      void writeCodeLoadForStore
         (InstructionStream &, std::vector<InstructionStream> &, StackOperation*, bool);
      void writeCodeStore
         (InstructionStream &, std::vector<InstructionStream> &, StackOperation*, bool);
      void writeCodeLoadForRead (InstructionStream &, std::vector<InstructionStream> &, size_t);

      void statement (CallingContext &, InstructionStream &, std::vector<BackReferenceJumpFill> &);

      ValueType::ValueHolder getConstantExpressionValue(CallingContext &);

      void expression (CallingContext &, InstructionStream &);
      void boolean (CallingContext &, InstructionStream &);
      void clause (CallingContext &, InstructionStream &);
      void predicate (CallingContext &, InstructionStream &);
      void relation (CallingContext &, InstructionStream &);
      void simple (CallingContext &, InstructionStream &);
      void term (CallingContext &, InstructionStream &);
      void factor (CallingContext &, InstructionStream &);
      void primary (CallingContext &, InstructionStream &);

      Lexer & src;

      Token nextToken;

      void GNT (void);

      std::string LN (void);

   public:

      Parser (Lexer & input) : src(input), nextToken(0) { GNT(); }
      ~Parser() { }

      void Parse (CallingContext & context) { program(context); }
      ValueType::ValueHolder TestParseExpression(CallingContext & context)
       { return getConstantExpressionValue(context); }
 };

#endif /* PARSER_HPP */
