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
#include <vector>
#include <iostream>

#include "Lexer.hpp"

void Lexer::Error (const std::string & errorMessage)
 {
   std::cerr << "Error: " << errorMessage << std::endl;
 }

int main (void)
 {
   std::vector<std::pair<std::string, Lexeme> > tests;

   tests.push_back(std::make_pair("12", NUMBER));
   tests.push_back(std::make_pair("12.", NUMBER));
   tests.push_back(std::make_pair(".12", NUMBER));
   tests.push_back(std::make_pair("1.2", NUMBER));
   tests.push_back(std::make_pair("12,", NUMBER));
   tests.push_back(std::make_pair(",12", NUMBER));
   tests.push_back(std::make_pair("1,2", NUMBER));
   tests.push_back(std::make_pair("12e3", NUMBER));
   tests.push_back(std::make_pair("12.e3", NUMBER));
   tests.push_back(std::make_pair(".12e3", NUMBER));
   tests.push_back(std::make_pair("1.2e3", NUMBER));
   tests.push_back(std::make_pair("12,e3", NUMBER));
   tests.push_back(std::make_pair(",12e3", NUMBER));
   tests.push_back(std::make_pair("1,2e3", NUMBER));
   tests.push_back(std::make_pair("12e+3", NUMBER));
   tests.push_back(std::make_pair("12.e+3", NUMBER));
   tests.push_back(std::make_pair(".12e+3", NUMBER));
   tests.push_back(std::make_pair("1.2e+3", NUMBER));
   tests.push_back(std::make_pair("12,e+3", NUMBER));
   tests.push_back(std::make_pair(",12e+3", NUMBER));
   tests.push_back(std::make_pair("1,2e+3", NUMBER));
   tests.push_back(std::make_pair("12e-3", NUMBER));
   tests.push_back(std::make_pair("12.e-3", NUMBER));
   tests.push_back(std::make_pair(".12e-3", NUMBER));
   tests.push_back(std::make_pair("1.2e-3", NUMBER));
   tests.push_back(std::make_pair("12,e-3", NUMBER));
   tests.push_back(std::make_pair(",12e-3", NUMBER));
   tests.push_back(std::make_pair("1,2e-3", NUMBER));
   tests.push_back(std::make_pair("12E3", NUMBER));
   tests.push_back(std::make_pair("12.E3", NUMBER));
   tests.push_back(std::make_pair(".12E3", NUMBER));
   tests.push_back(std::make_pair("1.2E3", NUMBER));
   tests.push_back(std::make_pair("12,E3", NUMBER));
   tests.push_back(std::make_pair(",12E3", NUMBER));
   tests.push_back(std::make_pair("1,2E3", NUMBER));
   tests.push_back(std::make_pair("12E+3", NUMBER));
   tests.push_back(std::make_pair("12.E+3", NUMBER));
   tests.push_back(std::make_pair(".12E+3", NUMBER));
   tests.push_back(std::make_pair("1.2E+3", NUMBER));
   tests.push_back(std::make_pair("12,E+3", NUMBER));
   tests.push_back(std::make_pair(",12E+3", NUMBER));
   tests.push_back(std::make_pair("1,2E+3", NUMBER));
   tests.push_back(std::make_pair("12E-3", NUMBER));
   tests.push_back(std::make_pair("12.E-3", NUMBER));
   tests.push_back(std::make_pair(".12E-3", NUMBER));
   tests.push_back(std::make_pair("1.2E-3", NUMBER));
   tests.push_back(std::make_pair("12,E-3", NUMBER));
   tests.push_back(std::make_pair(",12E-3", NUMBER));
   tests.push_back(std::make_pair("1,2E-3", NUMBER));
   tests.push_back(std::make_pair("\"Hello\"", STRING));
   tests.push_back(std::make_pair("\"Hello There\"", STRING));
   tests.push_back(std::make_pair("'Hello'", STRING));
   tests.push_back(std::make_pair("'Hello There'", STRING));
   tests.push_back(std::make_pair("program", IDENTIFIER));
   tests.push_back(std::make_pair("const", CONST));
   tests.push_back(std::make_pair("dim", DIM));
   tests.push_back(std::make_pair("static", STATIC));
   tests.push_back(std::make_pair("function", FUNCTION));
   tests.push_back(std::make_pair("end function", END_FUNCTION));
   tests.push_back(std::make_pair("declare", DECLARE));
   tests.push_back(std::make_pair("if", IF));
   tests.push_back(std::make_pair("then", THEN));
   tests.push_back(std::make_pair("else", ELSE));
   tests.push_back(std::make_pair("fi", FI));
   tests.push_back(std::make_pair("end if", FI));
   tests.push_back(std::make_pair("do", DO));
   tests.push_back(std::make_pair("while", WHILE));
   tests.push_back(std::make_pair("until", UNTIL));
   tests.push_back(std::make_pair("loop", LOOP));
   tests.push_back(std::make_pair("break", BREAK));
   tests.push_back(std::make_pair("continue", CONTINUE));
   tests.push_back(std::make_pair("for", FOR));
   tests.push_back(std::make_pair("step", STEP));
   tests.push_back(std::make_pair("to", TO));
   tests.push_back(std::make_pair("downto", DOWNTO));
   tests.push_back(std::make_pair("next", NEXT));
   tests.push_back(std::make_pair("end for", NEXT));
   tests.push_back(std::make_pair("select", SELECT));
   tests.push_back(std::make_pair("end select", END_SELECT));
   tests.push_back(std::make_pair("case", CASE));
   tests.push_back(std::make_pair("and case", CHOICE));
   tests.push_back(std::make_pair("case else", CASE_ELSE));
   tests.push_back(std::make_pair("and case else", DEFAULT));
   tests.push_back(std::make_pair("return", RETURN));
   tests.push_back(std::make_pair("tailcall", TAILCALL));
   tests.push_back(std::make_pair("call", CALL));
   tests.push_back(std::make_pair("=", EQUAL_SIGN));
   tests.push_back(std::make_pair(":=", ASSIGNMENT));
   tests.push_back(std::make_pair("<-", ASSIGNMENT));
   tests.push_back(std::make_pair("==", EQUALITY));
   tests.push_back(std::make_pair("!=", INEQUALITY));
   tests.push_back(std::make_pair("<>", INEQUALITY));
   tests.push_back(std::make_pair("#", INEQUALITY));
   tests.push_back(std::make_pair("&", AND_OP));
   tests.push_back(std::make_pair("/\\", AND_OP));
   tests.push_back(std::make_pair("and", AND_OP));
   tests.push_back(std::make_pair("&&", SHORT_AND));
   tests.push_back(std::make_pair("|", OR_OP));
   tests.push_back(std::make_pair("or", OR_OP));
   tests.push_back(std::make_pair("\\/", OR_OP));
   tests.push_back(std::make_pair("||", SHORT_OR));
   tests.push_back(std::make_pair(">", GREATER_THAN));
   tests.push_back(std::make_pair("<", LESS_THAN));
   tests.push_back(std::make_pair(">=", GREATER_THAN_OR_EQUAL_TO));
   tests.push_back(std::make_pair("<=", LESS_THAN_OR_EQUAL_TO));
   tests.push_back(std::make_pair("+", PLUS));
   tests.push_back(std::make_pair("-", MINUS));
   tests.push_back(std::make_pair("##", STRING_CAT));
   tests.push_back(std::make_pair("*", MULTIPLY));
   tests.push_back(std::make_pair("/", DIVIDE));
   tests.push_back(std::make_pair("**", POWER));
   tests.push_back(std::make_pair("^", POWER));
   tests.push_back(std::make_pair("!", NOT));
   tests.push_back(std::make_pair("not", NOT));
   tests.push_back(std::make_pair(":", COLON));
   tests.push_back(std::make_pair(";", SEMICOLON));
   tests.push_back(std::make_pair("(", OPEN_PARENS));
   tests.push_back(std::make_pair(")", CLOSE_PARENS));
   tests.push_back(std::make_pair("[", LEFT_BRACKET));
   tests.push_back(std::make_pair("]", RIGHT_BRACKET));
   tests.push_back(std::make_pair("\n", NEW_LINE));
   tests.push_back(std::make_pair("//   Hello There!\n", NEW_LINE));
   tests.push_back(std::make_pair("(* Hi there *) hello", IDENTIFIER));
   tests.push_back(std::make_pair("\\\n   12", NUMBER));

   for (std::vector<std::pair<std::string, Lexeme> >::iterator iter = tests.begin();
      iter != tests.end(); ++iter)
    {
      Lexer lexi(iter->first);
      Token test = lexi.GetNextToken();
      if (test.lexeme == iter->second)
         std::cout << "\t\tPassed: " << iter->first << " : " << test.text << std::endl;
      else
         std::cout << "Failed: " << iter->first << " : " << test.text << std::endl;
    }

   for (std::vector<std::pair<std::string, Lexeme> >::iterator iter = tests.begin();
      iter != tests.end(); ++iter)
    {
      Lexer lexi("   " + iter->first + "   ");
      Token test = lexi.GetNextToken();
      if (test.lexeme == iter->second)
         std::cout << "\t\tPassed: " << iter->first << " : " << test.text << std::endl;
      else
         std::cout << "Failed: " << iter->first << " : " << test.text << std::endl;
    }

   for (std::vector<std::pair<std::string, Lexeme> >::iterator iter = tests.begin();
      iter != tests.end(); ++iter)
    {
      Lexer lexi("\t\t\t" + iter->first + "\t\t\t");
      Token test = lexi.GetNextToken();
      if (test.lexeme == iter->second)
         std::cout << "\t\tPassed: " << iter->first << " : " << test.text << std::endl;
      else
         std::cout << "Failed: " << iter->first << " : " << test.text << std::endl;
    }

   return 0;
 }
