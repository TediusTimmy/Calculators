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
#include <iostream>
#include <string>
#include <sstream>

#include "ValueType.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Expression.hpp"
#include "Statement.hpp"

void DB_panic (const std::string & msg) __attribute__ ((__noreturn__));

void DB_panic (const std::string & msg)
 {
   std::stringstream errmsg;
   errmsg << msg << std::endl;
   throw errmsg.str();
 }

void DB_panic (const std::string & msg, const CallingContext & stack, size_t lineNo)
 {
   std::stringstream errmsg;
   errmsg << msg << std::endl;

   errmsg << "At line " << lineNo << " in \"" << stack.Name << "\"" << std::endl;

   const CallingContext * next = &stack;
   while (NULL != next && (NULL != next->Parent) && ("BaseContext" != next->Parent->Name))
    {
      errmsg << "\tfrom line " << next->ParentLine << " in \"";
      next = next->Parent;
      errmsg << next->Name << "\"" << std::endl;
    }

   throw errmsg.str();
 }

void Lexer::Error (const std::string & errorMessage)
 {
   DB_panic(errorMessage);
 }

const char * myProgram =
"function printFun (value;depth)\n"
"   dim i\n"
"   for i = 0 to depth-1\n"
"      call print (\"   \")\n"
"   next\n"

"   if isnull(value) then\n"
"      call print (\"NULL\"##eolstr())\n"
"   else if isval(value) then\n"
"      call print (str(value)##eolstr())\n"
"   else if isstr(value) then\n"
"      call print (value##eolstr())\n"
"   else\n"
"      call print (\"ARRAY : \"##str(size(value))##eolstr())\n"
"      for i = 0 to size(value)-1\n"
"         call printFun(value[i];depth+1)\n"
"      next\n"
"   end if\n"
"   return\n"
"end function\n"

"function makeFunction ( )\n"
"   dim resStr ; tempStr ; lastEmpty\n"
"   lastEmpty = 0\n"
"   resStr = \"\"\n"
"   do\n"
"      tempStr = instr()\n"
"      if tempStr = \"\" then\n"
"         if lastEmpty = 1 then break\n"
"         lastEmpty = 1\n"
"      else\n"
"         lastEmpty = 0\n"
"      fi\n"
"      resStr = resStr ## tempStr ## eolstr()\n"
"   loop\n"
"   return resStr\n"
"end function\n"

"function program ( )\n"
"   dim doStuff\n"
"   do\n"
"      doStuff = instr()\n"
"   while not isnull(doStuff)\n"
"      doStuff = rtrim(ltrim(doStuff))\n"
"   while doStuff # \"exit\"\n"
"      if doStuff != \"\" then call printFun(eval(doStuff); 0)\n"
"   loop\n"
"   return\n"
"end function\n";

int main (void)
 {
   std::map<std::string, std::map<std::string, ValueType::ValueHolder> > allGlobals;
   std::map<std::string, ValueType::ValueHolder> constants;
   std::map<std::string, std::vector<std::string> > functions;
   std::map<std::string, std::map<std::string, std::vector<long> > > funLocals;
   std::map<std::string, StatementSeq*> funDefs;

   CallingContext TheContext (allGlobals, constants, functions, funLocals, funDefs);

   Lexer lex (myProgram);
   Parser parse (lex);

   // Hard-coded program: parse cannot fail.
   parse.Parse(TheContext);

   FunctionCall programCall;
   programCall.name = "program";
   programCall.lineNo = 0U;

   bool returned = false;
   do
    {
      try
       {
         (void) programCall.evaluate(TheContext);
         returned = true;
       }
      catch (const std::string & msg)
       {
         std::cerr << msg;
       }
    }
   while (false == returned);

   return 0;
 }
