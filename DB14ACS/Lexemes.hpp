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
#ifndef LEXEME_HPP
#define LEXEME_HPP

enum Lexeme
 {
   INVALID,
   COMMENT,
   END_OF_FILE,

   CONST,
   DIM,
   STATIC,
   FUNCTION,
   END_FUNCTION, //       If END is followed by FUNCTION, change both to one END_FUNCTION.
   DECLARE,

   IDENTIFIER,
   NUMBER,
   STRING,

   LEFT_BRACKET,
   RIGHT_BRACKET,
   OPEN_PARENS,
   CLOSE_PARENS,

   SEMICOLON,
   COLON,

   NEW_LINE,

   END,        //         A naked END is a lexical error.
   IF,
   THEN,
   ELSE,
   FI,         //         If END is followed by IF, change both to one FI.
   DO,
   WHILE,
   UNTIL,
   LOOP,
   BREAK,
   CONTINUE,
   FOR,
   TO,
   DOWNTO,
   STEP,
   NEXT,       //         If END is followed by FOR, change both to one NEXT.
   SELECT,
   END_SELECT, //         If END is followed by SELECT, change both to one END_SELECT.
   CASE,
   CHOICE,
   DEFAULT,
   CASE_ELSE,  //         If CASE is followed by ELSE, change both to one CASE_ELSE.
   RETURN,
   TAILCALL,
   CALL,

   EQUAL_SIGN, // =       There is no good way to disabiguate EQUAL_SIGN.
   ASSIGNMENT, // <- :=
   EQUALITY,   // ==
   INEQUALITY,
   AND,        // and     There should never be a naked AND. If followed by CASE, change both to one CHOICE.
   AND_OP,     // & /\    If CHOICE is followed by ELSE, change both to one DEFAULT. If followed by THEN,
   SHORT_AND,  // &&      change both to one SHORT_AND. If none of these, mutate to AND_OP.
   OR,         // or      There should never be a naked OR. If followed by ELSE, change both to one SHORT_OR.
   OR_OP,      // | \/    Else, change to OR_OP.
   SHORT_OR,   // ||
   GREATER_THAN,
   LESS_THAN,
   GREATER_THAN_OR_EQUAL_TO,
   LESS_THAN_OR_EQUAL_TO,
   PLUS,
   MINUS,
   // No shifts.
   STRING_CAT,
   MULTIPLY,
   DIVIDE,
   POWER,
   NOT
 };

#endif /* LEXEME_HPP */
