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
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "ValueType.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SymbolTable.hpp"
#include "StackOp.hpp"

ValueType::ValueHolder RecursiveInterpreter (const InstructionStream &, StackFrame &, bool = true);

void DB_panic (const std::string & msg) __attribute__ ((__noreturn__));

void DB_panic (const std::string & msg)
 {
   std::cerr << msg << std::endl;
   std::exit(1);
 }

void DB_panic (const std::string & msg, const StackFrame & stack, size_t lineNo)
 {
   std::cerr << msg << std::endl;

   std::cerr << "At line " << lineNo << " in \"" << stack.FunctionNames[stack.FunctionIndex] << "\"" << std::endl;

   const StackFrame * next = &stack;
   while (NULL != next && (NULL != next->Parent) && (0U != next->Parent->FunctionIndex))
    {
      std::cerr << "\tfrom line " << next->ParentLineNumber << " in \"";
      next = next->Parent;
      std::cerr << stack.FunctionNames[next->FunctionIndex] << "\"" << std::endl;
    }

   std::exit(1);
 }

void Lexer::Error (const std::string & errorMessage)
 {
   DB_panic(errorMessage);
 }

static inline void push_back(InstructionStream & dest, StackOperation * item)
 {
   dest.push_back(std::make_pair(item, item->type));
 }

#ifdef DEBUGSKI
void DumpContext (const CallingContext &);
#endif

int main (int argc, char ** argv)
 {
   if (argc < 2)
      DB_panic("Usage: DB14 source_file {args}");

   std::map<std::string, size_t> globals;
   std::map<std::string, ValueType::ValueHolder> constants;
   std::map<std::string, std::vector<std::string> > functions;
   std::map<std::string, size_t> funDefs;

   std::vector<std::vector<ValueType::ValueHolder> > VMGlobals;
   std::vector<InstructionStream> VMFunctions;
   std::vector<std::string> VMFunNames;
   std::vector<std::vector<ValueType::ValueHolder> > VMFunLocals;

   VMGlobals.reserve(16U);
   VMGlobals.resize(1U);
   VMFunctions.reserve(16U);
   VMFunctions.resize(1U);
   VMFunNames.reserve(16U);
   VMFunNames.resize(1U);
   VMFunLocals.reserve(16U);
   VMFunLocals.resize(1U);

   CallingContext TheContext (globals, constants, functions, funDefs,
      VMGlobals, VMFunctions, VMFunNames, VMFunLocals);

   std::ifstream file (argv[1]);
   std::string input;

   if (true == file.good())
    {
      input.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

   if (true == file.bad())
      DB_panic(std::string("Error opening file \"") + argv[1] + "\".");

   Lexer lex (input);
   Parser parse (lex);

   parse.Parse(TheContext);

#ifdef DEBUGSKI
   DumpContext(TheContext);
   return 0;
#endif

   Constant argVec;
   FunCall programCall;
   Return returnStub;

   if ((funDefs.end() == funDefs.find("program")) ||
       (0U == VMFunctions[funDefs["program"]].size()))
      DB_panic("Function \"program\" was never defined.");

   programCall.index = funDefs["program"];
   programCall.nargs = 0;

   if (1U < functions["program"].size())
      DB_panic("Too many arguments for function \"program\".");

   if (1U == functions["program"].size())
    {
      ArrayValue * args = new ArrayValue(argc - 2);
      for (size_t i = 0; i < static_cast<size_t>(argc - 2); ++i)
       {
         args->setIndex(i, ValueType::ValueHolder(new StringValue(argv[i + 2])));
       }

      programCall.nargs = 1;
      argVec.value = ValueType::ValueHolder(args);
      push_back(VMFunctions[0], &argVec);
    }

   push_back(VMFunctions[0], &programCall);
   push_back(VMFunctions[0], &returnStub);

   StackFrame TheFrame (VMGlobals, VMFunctions, VMFunNames, VMFunLocals);

   (void) RecursiveInterpreter (VMFunctions[0], TheFrame);

   return 0;
 }
