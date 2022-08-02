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
#ifndef LEXER_HPP
#define LEXER_HPP

#include "Lexemes.hpp"
#include "Token.hpp"
#include <string>
#include <map>

class Lexer /* Lex Me Up, Scotty */
 {
   private:
      std::string input; // All of input, passed in to constructor.
      size_t inputPosition; // Position in input string.
      size_t lineNumber; // Current line number in the input string.

      Token nextToken; // The next token that will be returned.

      std::map<std::string, Lexeme> keyWords; // The map of keywords to Lexemes.

       /*
         Internal functions for operating on buffered input.
       */
      char GetNextNonWhite (void);
      char GetNextChar (void);
      void UnGetNextChar (void);

      void Get_NextToken (void); // Updates nextToken.

      Lexer();

      // Print a message and die.
      void Error (const std::string & errorMessage) __attribute__ ((__noreturn__));

   public:

      Token GetNextToken (void); // Returns the next token.
      size_t LineNo (void) const { return lineNumber; } // Return the current line number.

      Lexer (const std::string & input);
 };

#endif /* LEXER_HPP */
