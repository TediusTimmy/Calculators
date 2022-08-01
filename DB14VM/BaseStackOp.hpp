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
#ifndef BASE_STACKOP_HPP
#define BASE_STACKOP_HPP

#include <vector>
#include <utility>

class StackOperation
 {
   public:
      enum TYPE
       {
         AND_OP,
         OR_OP,
         EQUALITY,
         INEQUALITY,
         GREATER_THAN,
         LESS_THAN,
         GREATER_THAN_OR_EQUAL_TO,
         LESS_THAN_OR_EQUAL_TO,
         PLUS,
         MINUS,
         STRING_CAT,
         MULTIPLY,
         DIVIDE,
         REMAINDER,
         POWER,
         NOT,
         ABS,
         NEGATE,
         FORCE_LOGICAL,
         CONSTANT,
         LOAD_GLOBAL_VARIABLE,
         LOAD_STATIC_VARIABLE,
         LOAD_LOCAL_VARIABLE,
         STORE_GLOBAL_VARIABLE,
         STORE_STATIC_VARIABLE,
         STORE_LOCAL_VARIABLE,
         LOAD_INDIRECT,
         FUNCTION_CALL,
         STANDARD_CONSTANT_FUNCTION,
         STANDARD_UNARY_FUNCTION,
         STANDARD_BINARY_FUNCTION,
         STANDARD_TERNARY_FUNCTION,
         STORE_INDIRECT,
         COPY,
         ROTATE,
         SWAP,
         POP,
         JUMP,
         BRANCH, // ON FALSE
         RETURN,
         TAILCALL
       };

      const TYPE type;
      std::size_t lineNumber;

      virtual StackOperation * Clone (void) = 0;

      virtual ~StackOperation() { };

   protected:
      StackOperation(const StackOperation & src) : type(src.type), lineNumber(src.lineNumber) { }
      StackOperation(TYPE type, std::size_t lineNo) : type(type), lineNumber(lineNo) { }
 };

typedef std::vector<std::pair<StackOperation*, StackOperation::TYPE> > InstructionStream;

#endif /* BASE_STACKOP_HPP */
