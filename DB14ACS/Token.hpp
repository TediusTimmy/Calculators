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
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include "Lexemes.hpp"

class Token
 {
   private:
      Token();

   public:
      Lexeme lexeme;
      std::string text;
      size_t lineNumber;

      Token(size_t lineNo) : lexeme(INVALID), lineNumber(lineNo) { }
      Token(const Token & src) : lexeme(src.lexeme), text(src.text), lineNumber(src.lineNumber) { }
      Token(Lexeme lexeme, std::string text, size_t lineNo) : lexeme(lexeme), text(text), lineNumber(lineNo) { }

      Token& operator= (const Token & src)
       {
         if (this != &src)
          {
            lexeme = src.lexeme;
            text = src.text;
            lineNumber = src.lineNumber;
          }
         return *this;
       }
 };

#endif /* TOKEN_HPP */
