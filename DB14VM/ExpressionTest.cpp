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
#include <string>
#include <cstdlib>

#include "ValueType.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SymbolTable.hpp"
#include "StackOp.hpp"


void DB_panic (const std::string & msg) __attribute__ ((__noreturn__));

void DB_panic (const std::string & msg)
 {
   std::cerr << msg << std::endl;
   throw std::exception();
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

void printResult (ValueType::ValueHolder value, int depth)
 {
   for (int i = 0; i < depth; ++i)
      std::cout << "   ";

   if (NULL == value.data)
    {
      std::cout << "NULL" << std::endl;
    }
   else if (ValueType::NUMBER == value.data->type)
    {
      std::cout << static_cast<NumericValue*>(value.data)->val.toString() << std::endl;
    }
   else if (ValueType::STRING == value.data->type)
    {
      std::cout << static_cast<StringValue*>(value.data)->val << std::endl;
    }
   else if (ValueType::ARRAY == value.data->type)
    {
      size_t count = static_cast<ArrayValue*>(value.data)->size();
      std::cout << "ARRAY : " << count << std::endl;
      for (size_t i = 0; i < count; ++i)
         printResult(static_cast<ArrayValue*>(value.data)->getIndex(i), depth + 1);
    }
 }

int main (void)
 {
   std::map<std::string, size_t> globals;
   std::map<std::string, ValueType::ValueHolder> constants;
   std::map<std::string, std::vector<std::string> > functions;
   std::map<std::string, size_t> funDefs;

   std::vector<std::vector<ValueType::ValueHolder> > VMGlobals;
   std::vector<InstructionStream> VMFunctions;
   std::vector<std::string> VMFunNames;
   std::vector<std::vector<ValueType::ValueHolder> > VMFunLocals;

   CallingContext NullContext (globals, constants, functions, funDefs,
      VMGlobals, VMFunctions, VMFunNames, VMFunLocals);

   std::string input;
   getline(std::cin, input);
   while ("exit" != input)
    {
      try
       {
         Lexer lex (input);
         Parser parse (lex);
         ValueType::ValueHolder result = parse.TestParseExpression(NullContext);
         printResult(result, 0);
       }
      catch (...)
       {
       }

      getline(std::cin, input);
    }

   return 0;
 }
