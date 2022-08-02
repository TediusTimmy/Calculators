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
#include "Lexer.hpp"
#include <cctype>
#include <cstdlib>

#include <iostream>
#include <iomanip>

Lexer::Lexer (const std::string & input) : input(input), inputPosition(0), lineNumber(1), nextToken(0)
 {
   keyWords.insert(std::make_pair("const", CONST));
   keyWords.insert(std::make_pair("dim", DIM));
   keyWords.insert(std::make_pair("static", STATIC));
   keyWords.insert(std::make_pair("function", FUNCTION));
   keyWords.insert(std::make_pair("declare", DECLARE));
   keyWords.insert(std::make_pair("end", END));
   keyWords.insert(std::make_pair("if", IF));
   keyWords.insert(std::make_pair("then", THEN));
   keyWords.insert(std::make_pair("else", ELSE));
   keyWords.insert(std::make_pair("fi", FI));
   keyWords.insert(std::make_pair("do", DO));
   keyWords.insert(std::make_pair("while", WHILE));
   keyWords.insert(std::make_pair("until", UNTIL));
   keyWords.insert(std::make_pair("loop", LOOP));
   keyWords.insert(std::make_pair("break", BREAK));
   keyWords.insert(std::make_pair("continue", CONTINUE));
   keyWords.insert(std::make_pair("for", FOR));
   keyWords.insert(std::make_pair("to", TO));
   keyWords.insert(std::make_pair("downto", DOWNTO));
   keyWords.insert(std::make_pair("step", STEP));
   keyWords.insert(std::make_pair("next", NEXT));
   keyWords.insert(std::make_pair("select", SELECT));
   keyWords.insert(std::make_pair("case", CASE));
   keyWords.insert(std::make_pair("return", RETURN));
   keyWords.insert(std::make_pair("tailcall", TAILCALL));
   keyWords.insert(std::make_pair("call", CALL));
   keyWords.insert(std::make_pair("and", AND));
   keyWords.insert(std::make_pair("or", OR));
   keyWords.insert(std::make_pair("not", NOT));

   Get_NextToken();
 }

char Lexer::GetNextNonWhite (void)
 {
   char c = GetNextChar();
   while ((' ' == c) || ('\t' == c))
      c = GetNextChar(); // Return a '\0' on EOF
   return c;
 }

char Lexer::GetNextChar (void)
 {
   char ret;
   if (inputPosition >= input.length())
    {
      ret = '\0';
      ++inputPosition;
    }
   else ret = input[inputPosition++];

   if ('\n' == ret) ++lineNumber;

   return ret;
 }

void Lexer::UnGetNextChar (void)
 {
   --inputPosition;
   if ((inputPosition < input.length()) && ('\n' == input[inputPosition]))
      --lineNumber;
 }

void Lexer::Get_NextToken (void)
 {
   nextToken = Token(lineNumber);

   nextToken.lexeme = COMMENT;
   while (nextToken.lexeme == COMMENT)
    {
      int n = GetNextNonWhite();
      if (std::isalpha(n)) // Read a letter
       { // Read in an identifier or keyword -- loop until not letter, number, or '_'
         nextToken.text = static_cast<char>(n);
         n = GetNextChar();
         while (std::isalnum(n) || ('_' == n))
          {
            nextToken.text += static_cast<char>(n);
            n = GetNextChar();
          }
         UnGetNextChar();

         std::map<std::string, Lexeme>::iterator keyWord = keyWords.find(nextToken.text);

         if (keyWords.end() == keyWord)
          {
            nextToken.lexeme = IDENTIFIER;
          }
         else
          {
            nextToken.lexeme = keyWord->second;
          }
         // Get_NextToken doesn't do Token collapsing. That is GetNextToken's job.
       }
      else if (std::isdigit(n) || ('.' == n) || (',' == n)) //Read a number or the begining of one
       { //Read in a number
         nextToken.text = ""; // Clear a previous \\\n comment, if present.
         while (std::isdigit(n))
          {
            nextToken.text += static_cast<char>(n);
            n = GetNextChar();
          }
         if (('.' == n) || (',' == n))
          {
            nextToken.text += "."; // Convert ','s to '.'s.
            n = GetNextChar();
          }
         while (std::isdigit(n))
          {
            nextToken.text += static_cast<char>(n);
            n = GetNextChar();
          }

         if ("." == nextToken.text)
          {
            nextToken.lexeme = INVALID;
          }
         else
          {
            if (('e' == n) || ('E' == n))
             {
               std::string temp = "e";

               n = GetNextChar();
               if (('-' == n) || ('+' == n))
                {
                  temp += static_cast<char>(n);
                  n = GetNextChar();
                }
               while (std::isdigit(n))
                {
                  temp += static_cast<char>(n);
                  n = GetNextChar();
                }

               if (("e-" == temp) || ("e+" == temp))
                {
                  UnGetNextChar();
                  UnGetNextChar();
                }
               else if ("e" == temp)
                {
                  UnGetNextChar();
                }
               else
                {
                  nextToken.text += temp;
                }
             }

            nextToken.lexeme = NUMBER;
          }
         UnGetNextChar();
       }
      else if ('@' == n) // Read in a special number: NaN or Inf
       {
         nextToken.text = static_cast<char>(n);
         n = GetNextChar();
         while (std::isalnum(n))
          {
            nextToken.text += static_cast<char>(n);
            n = GetNextChar();
          }
         nextToken.text += static_cast<char>(n);
         nextToken.lexeme = NUMBER;

         if (("@Inf@" != nextToken.text) && ("@NaN@" != nextToken.text))
            Error("Bad special number.");
       }
      else
       { //DFA for all other tokens
         if ((n != '\0') && (n != '\n')) nextToken.text = static_cast<char>(n);
         switch (n)
          {
            case '<':
               switch (GetNextChar())
                {
                  case '-':
                     nextToken.text += '-';
                     nextToken.lexeme = ASSIGNMENT;
                     break;
                  case '>':
                     nextToken.text += '>';
                     nextToken.lexeme = INEQUALITY;
                     break;
                  case '=':
                     nextToken.text += '=';
                     nextToken.lexeme = LESS_THAN_OR_EQUAL_TO;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = LESS_THAN;
                     break;
                }
               break;

            case '*':
               switch (GetNextChar())
                {
                  case '*':
                     nextToken.text += '*';
                     nextToken.lexeme = POWER;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = MULTIPLY;
                     break;
                }
               break;

            case '/':
               switch (GetNextChar())
                {
                  case '/':
                     n = GetNextChar();
                     while ((n != '\n') && (n != '\0')) n = GetNextChar();
                     nextToken.text = "__NEWLINE__";
                     nextToken.lexeme = NEW_LINE;
                     break;
                  case '\\':
                     nextToken.text += '\\';
                     nextToken.lexeme = AND_OP;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = DIVIDE;
                     break;
                }
               break;

            case '>':
               switch (GetNextChar())
                {
                  case '=':
                     nextToken.text += '=';
                     nextToken.lexeme = GREATER_THAN_OR_EQUAL_TO;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = GREATER_THAN;
                     break;
                }
               break;

            case '\\':
               switch (GetNextChar())
                {
                  case '/':
                     nextToken.text += '/';
                     nextToken.lexeme = OR_OP;
                     break;
                  case '\n':
                     nextToken.lexeme = COMMENT;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = INVALID;
                     break;
                }
               break;

            case ':':
               switch (GetNextChar())
                {
                  case '=':
                     nextToken.text += '=';
                     nextToken.lexeme = ASSIGNMENT;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = COLON;
                     break;
                }
               break;

            case '!':
               switch (GetNextChar())
                {
                  case '=':
                     nextToken.text += '=';
                     nextToken.lexeme = INEQUALITY;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = NOT;
                     break;
                }
               break;

            case '#':
               switch (GetNextChar())
                {
                  case '#':
                     nextToken.text += '#';
                     nextToken.lexeme = STRING_CAT;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = INEQUALITY;
                     break;
                }
               break;

            case '=':
               switch (GetNextChar())
                {
                  case '=':
                     nextToken.text += '=';
                     nextToken.lexeme = EQUALITY;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = EQUAL_SIGN;
                     break;
                }
               break;

            case '&':
               switch (GetNextChar())
                {
                  case '&':
                     nextToken.text += '&';
                     nextToken.lexeme = SHORT_AND;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = AND_OP;
                     break;
                }
               break;

            case '|':
               switch (GetNextChar())
                {
                  case '|':
                     nextToken.text += '|';
                     nextToken.lexeme = SHORT_OR;
                     break;
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = OR_OP;
                     break;
                }
               break;

            case '(':
               switch (GetNextChar())
                {
                  case '*':
                   {
                     nextToken.text = "";
                     nextToken.lexeme = COMMENT;
                     bool done = false;
                     while (!done)
                      {
                        n = GetNextChar();
                        while ((n != '*') && (n != '\0')) n = GetNextChar();
                        if (n != '\0') n = GetNextChar();
                        while ((n == '*') && (n != '\0')) n = GetNextChar();
                        if ((n == ')') || (n == '\0')) done = true;
                      }
                     if (n == '\0') Error("End of File found before comment terminated.");
                     break;
                   }
                  default:
                     UnGetNextChar();
                     nextToken.lexeme = OPEN_PARENS;
                     break;
                }
               break;

            case ';':
               nextToken.lexeme = SEMICOLON;
               break;

            case '+':
               nextToken.lexeme = PLUS;
               break;

            case '-':
               nextToken.lexeme = MINUS;
               break;

            case '^':
               nextToken.lexeme = POWER;
               break;

            case ')':
               nextToken.lexeme = CLOSE_PARENS;
               break;

            case '[':
               nextToken.lexeme = LEFT_BRACKET;
               break;

            case ']':
               nextToken.lexeme = RIGHT_BRACKET;
               break;

            case '\0':
               nextToken.text = "__EOF__";
               nextToken.lexeme = END_OF_FILE;
               break;

            case '\n':
               nextToken.text = "__NEWLINE__";
               nextToken.lexeme = NEW_LINE;
               break;

            case '"':
               nextToken.text = "";
               n = GetNextChar();
               while ((n != '"') && (n != '\0') && (n != '\n'))
                {
                  nextToken.text += static_cast<char>(n);
                  n = GetNextChar();
                }
               if (n == '\n')
                {
                  Error("End of Line found before string terminated.");
                  UnGetNextChar();
                }
               if (n == '\0') Error("End of File found before string terminated.");

               nextToken.lexeme = STRING;
               break;

            case '\'':
               nextToken.text = "";
               n = GetNextChar();
               while ((n != '\'') && (n != '\0') && (n != '\n'))
                {
                  nextToken.text += static_cast<char>(n);
                  n = GetNextChar();
                }
               if (n == '\n')
                {
                  Error("End of Line found before string terminated.");
                  UnGetNextChar();
                }
               if (n == '\0') Error("End of File found before string terminated.");

               nextToken.lexeme = STRING;
               break;

            default:
               nextToken.lexeme = INVALID;
               break;
          }
       }
    }
 }

Token Lexer::GetNextToken (void)
 {
   Token result (nextToken);

   Get_NextToken();

   switch (result.lexeme)
    {
      case AND:
         switch (nextToken.lexeme)
          {
            case CASE:
               result.lexeme = CHOICE;
               result.text = result.text + " " + nextToken.text;

               Get_NextToken();

               if (ELSE == nextToken.lexeme)
                {
                  result.lexeme = DEFAULT;
                  result.text = result.text + " " + nextToken.text;

                  Get_NextToken();
                }

               break;
            case THEN:
               result.lexeme = SHORT_AND;
               result.text = result.text + " " + nextToken.text;

               Get_NextToken();

               break;
            default:
               result.lexeme = AND_OP;
               break;
          }
         break;

      case OR:
         if (ELSE == nextToken.lexeme)
          {
            result.lexeme = SHORT_OR;
            result.text = result.text + " " + nextToken.text;

            Get_NextToken();
          }
         else
          {
            result.lexeme = OR_OP;
          }
         break;

      case CASE:
         if (ELSE == nextToken.lexeme)
          {
            result.lexeme = CASE_ELSE;
            result.text = result.text + " " + nextToken.text;

            Get_NextToken();
          }
         break;

      case END:
         switch (nextToken.lexeme)
          {
            case SELECT:
               result.lexeme = END_SELECT;
               result.text = result.text + " " + nextToken.text;

               Get_NextToken();
               break;

            case IF:
               result.lexeme = FI;
               result.text = result.text + " " + nextToken.text;

               Get_NextToken();
               break;

            case FOR:
               result.lexeme = NEXT;
               result.text = result.text + " " + nextToken.text;

               Get_NextToken();
               break;

            case FUNCTION:
               result.lexeme = END_FUNCTION;
               result.text = result.text + " " + nextToken.text;

               Get_NextToken();
               break;

            default:
               Error("END without IF, FOR, SELECT, or FUNCTION");
               result.lexeme = INVALID;
               break;
          }
         break;

      default:
         break;
    }

   return result;
 }
