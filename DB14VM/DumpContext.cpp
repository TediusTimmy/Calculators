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
#include "SymbolTable.hpp"
#include "StackOp.hpp"
#include "ValueType.hpp"

#include <iostream>
#include <iomanip>

void printResult (ValueType::ValueHolder value, int depth)
 {
   for (int i = 0; i < depth; ++i)
      std::cerr << "   ";

   if (NULL == value.data)
    {
      std::cerr << "NULL" << std::endl;
    }
   else if (ValueType::NUMBER == value.data->type)
    {
      std::cerr << static_cast<NumericValue*>(value.data)->val.toString() << std::endl;
    }
   else if (ValueType::STRING == value.data->type)
    {
      std::cerr << static_cast<StringValue*>(value.data)->val << std::endl;
    }
   else if (ValueType::ARRAY == value.data->type)
    {
      size_t count = static_cast<ArrayValue*>(value.data)->size();
      std::cerr << "ARRAY : " << count << std::endl;
      for (size_t i = 0; i < count; ++i)
         printResult(static_cast<ArrayValue*>(value.data)->getIndex(i), depth + 1);
    }
 }

void DumpContext (const CallingContext & src)
 {
   std::cerr << "Functions: " << src.PopFunctions.size() - 1 << std::endl << std::endl;

   for (size_t curFun = 1; src.PopFunctions.size() > curFun; ++curFun)
    {
      std::cerr << "Function: \"" << src.PopFunNames[curFun] << "\"" << std::endl;
      std::cerr << " Locals: " << src.PopFunLocals[curFun].size() << std::endl;
      for (size_t opCode = 0; src.PopFunctions[curFun].size() > opCode; ++opCode)
       {
         std::cerr << "   " << std::setw(4) << opCode << " : ";
         switch(src.PopFunctions[curFun][opCode].second)
          {
            case StackOperation::AND_OP:
               std::cerr << "AND_OP" << std::endl; break;
            case StackOperation::OR_OP:
               std::cerr << "OR_OP" << std::endl; break;
            case StackOperation::EQUALITY:
               std::cerr << "EQUALITY" << std::endl; break;
            case StackOperation::INEQUALITY:
               std::cerr << "INEQUALITY" << std::endl; break;
            case StackOperation::GREATER_THAN:
               std::cerr << "GREATER_THAN" << std::endl; break;
            case StackOperation::LESS_THAN:
               std::cerr << "LESS_THAN" << std::endl; break;
            case StackOperation::GREATER_THAN_OR_EQUAL_TO:
               std::cerr << "GREATER_THAN_OR_EQUAL_TO" << std::endl; break;
            case StackOperation::LESS_THAN_OR_EQUAL_TO:
               std::cerr << "LESS_THAN_OR_EQUAL_TO" << std::endl; break;
            case StackOperation::PLUS:
               std::cerr << "PLUS" << std::endl; break;
            case StackOperation::MINUS:
               std::cerr << "MINUS" << std::endl; break;
            case StackOperation::STRING_CAT:
               std::cerr << "STRING_CAT" << std::endl; break;
            case StackOperation::MULTIPLY:
               std::cerr << "MULTIPLY" << std::endl; break;
            case StackOperation::DIVIDE:
               std::cerr << "DIVIDE" << std::endl; break;
            case StackOperation::REMAINDER:
               std::cerr << "REMAINDER" << std::endl; break;
            case StackOperation::POWER:
               std::cerr << "POWER" << std::endl; break;
            case StackOperation::NOT:
               std::cerr << "NOT" << std::endl; break;
            case StackOperation::ABS:
               std::cerr << "ABS" << std::endl; break;
            case StackOperation::NEGATE:
               std::cerr << "NEGATE" << std::endl; break;
            case StackOperation::FORCE_LOGICAL:
               std::cerr << "FORCE_LOGICAL" << std::endl; break;
            case StackOperation::CONSTANT:
               std::cerr << "CONSTANT" << std::endl;
             {
               Constant * op = static_cast<Constant*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << "      Value:" << std::endl;
               printResult(op->value, 3);
             }
               break;
            case StackOperation::LOAD_GLOBAL_VARIABLE:
               std::cerr << "LOAD_GLOBAL_VARIABLE";
             {
               IndexedStackOperation * op = static_cast<IndexedStackOperation*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : " << op->index << std::endl;
             }
               break;
            case StackOperation::LOAD_STATIC_VARIABLE:
               std::cerr << "LOAD_STATIC_VARIABLE";
             {
               IndexedStackOperation * op = static_cast<IndexedStackOperation*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : " << op->index << std::endl;
             }
               break;
            case StackOperation::LOAD_LOCAL_VARIABLE:
               std::cerr << "LOAD_LOCAL_VARIABLE";
             {
               IndexedStackOperation * op = static_cast<IndexedStackOperation*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : " << op->index << std::endl;
             }
               break;
            case StackOperation::STORE_GLOBAL_VARIABLE:
               std::cerr << "STORE_GLOBAL_VARIABLE";
             {
               IndexedStackOperation * op = static_cast<IndexedStackOperation*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : " << op->index << std::endl;
             }
               break;
            case StackOperation::STORE_STATIC_VARIABLE:
               std::cerr << "STORE_STATIC_VARIABLE";
             {
               IndexedStackOperation * op = static_cast<IndexedStackOperation*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : " << op->index << std::endl;
             }
               break;
            case StackOperation::STORE_LOCAL_VARIABLE:
               std::cerr << "STORE_LOCAL_VARIABLE";
             {
               IndexedStackOperation * op = static_cast<IndexedStackOperation*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : " << op->index << std::endl;
             }
               break;
            case StackOperation::LOAD_INDIRECT:
               std::cerr << "LOAD_INDIRECT" << std::endl; break;
            case StackOperation::STORE_INDIRECT:
               std::cerr << "STORE_INDIRECT" << std::endl; break;
            case StackOperation::FUNCTION_CALL:
               std::cerr << "FUNCTION_CALL";
             {
               FunCall * op = static_cast<FunCall*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : (index) " << op->index << ", (nargs) " << op->nargs << std::endl;
             }
               break;
            case StackOperation::STANDARD_CONSTANT_FUNCTION:
               std::cerr << "STANDARD_CONSTANT_FUNCTION" << std::endl; break;
            case StackOperation::STANDARD_UNARY_FUNCTION:
               std::cerr << "STANDARD_UNARY_FUNCTION" << std::endl; break;
            case StackOperation::STANDARD_BINARY_FUNCTION:
               std::cerr << "STANDARD_BINARY_FUNCTION" << std::endl; break;
            case StackOperation::STANDARD_TERNARY_FUNCTION:
               std::cerr << "STANDARD_TERNARY_FUNCTION" << std::endl; break;
            case StackOperation::COPY:
               std::cerr << "COPY" << std::endl; break;
            case StackOperation::ROTATE:
               std::cerr << "ROTATE" << std::endl; break;
            case StackOperation::SWAP:
               std::cerr << "SWAP" << std::endl; break;
            case StackOperation::POP:
               std::cerr << "POP" << std::endl; break;
            case StackOperation::JUMP:
               std::cerr << "JUMP";
             {
               IndexedStackOperation * op = static_cast<IndexedStackOperation*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : " << op->index << std::endl;
             }
               break;
            case StackOperation::BRANCH:
               std::cerr << "BRANCH";
             {
               IndexedStackOperation * op = static_cast<IndexedStackOperation*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : " << op->index << std::endl;
             }
               break;
            case StackOperation::RETURN:
               std::cerr << "RETURN" << std::endl; break;
            case StackOperation::TAILCALL:
               std::cerr << "TAILCALL";
             {
               TailCall * op = static_cast<TailCall*>(src.PopFunctions[curFun][opCode].first);
               std::cerr << " : (nargs) " << op->nargs << std::endl;
             }
               break;
          }
       }
    }
 }
