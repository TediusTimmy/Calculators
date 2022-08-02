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
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

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

int main (int argc, char ** argv)
 {
   if (argc < 2)
    {
      std::cerr << "Usage: DB14 source_file {args}" << std::endl;
      return 1;
    }

   std::map<std::string, std::map<std::string, ValueType::ValueHolder> > allGlobals;
   std::map<std::string, ValueType::ValueHolder> constants;
   std::map<std::string, std::vector<std::string> > functions;
   std::map<std::string, std::map<std::string, std::vector<long> > > funLocals;
   std::map<std::string, StatementSeq*> funDefs;

   CallingContext TheContext (allGlobals, constants, functions, funLocals, funDefs);

   std::ifstream file (argv[1]);
   std::string input;

   if (true == file.good())
    {
      input.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

   if (true == file.bad())
    {
      std::cerr << "Error opening file \"" << argv[1] << "\"." << std::endl;
      return 1;
    }

   Lexer lex (input);
   Parser parse (lex);

   try
    {
      parse.Parse(TheContext);
    }
   catch (const std::string & msg)
    {
      std::cerr << msg;
      return 1;
    }

   FunctionCall programCall;
   programCall.name = "program";
   programCall.lineNo = 0U;

   if (1 < functions["program"].size())
    {
      std::cerr << "Too many arguments for function \"program\"." << std::endl;
      return 1;
    }

   if (1 == functions["program"].size())
    {
      ArrayValue * args = new ArrayValue(argc - 2);
      for (size_t i = 0; i < static_cast<size_t>(argc - 2); ++i)
       {
         args->setIndex(i, ValueType::ValueHolder(new StringValue(argv[i + 2])));
       }

      Constant * argVec = new Constant();
      argVec->value = ValueType::ValueHolder(args);
      programCall.args.push_back(argVec);
    }

   try
    {
      (void) programCall.evaluate(TheContext);
    }
   catch (const std::string & msg)
    {
      std::cerr << msg;
      return 1;
    }

   return 0;
 }
