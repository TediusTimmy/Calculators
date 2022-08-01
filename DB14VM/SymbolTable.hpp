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
#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <map>
#include <string>
#include <vector>
#include <set>
#include "ValueType.hpp"
#include "BaseStackOp.hpp"

class StackFrame
 {
   public:
      std::vector<ValueType::ValueHolder> Locals;
      std::vector<ValueType::ValueHolder> & Statics;
      std::vector<ValueType::ValueHolder> & Globals;
      std::vector<std::vector<ValueType::ValueHolder> > & AllGlobals;
      std::vector<InstructionStream> & Functions;
      std::vector<std::string> & FunctionNames;
      std::vector<std::vector<ValueType::ValueHolder> > & FunLocals;

      const size_t FunctionIndex;
      const StackFrame * Parent;
      const size_t ParentLineNumber;

      StackFrame(StackFrame & src, size_t index, size_t lineNo) :
         Statics(src.AllGlobals[index]),
         Globals(src.Globals),
         AllGlobals(src.AllGlobals),
         Functions(src.Functions),
         FunctionNames(src.FunctionNames),
         FunLocals(src.FunLocals),
         FunctionIndex(index),
         Parent(&src),
         ParentLineNumber(lineNo)
       {
       }

      StackFrame(
         std::vector<std::vector<ValueType::ValueHolder> > & AllGlobals,
         std::vector<InstructionStream> & Functions,
         std::vector<std::string> & FunctionNames,
         std::vector<std::vector<ValueType::ValueHolder> > & FunLocals) :

         Statics(AllGlobals[0U]),
         Globals(AllGlobals[0U]),
         AllGlobals(AllGlobals),
         Functions(Functions),
         FunctionNames(FunctionNames),
         FunLocals(FunLocals),
         FunctionIndex(0U),
         Parent(NULL),
         ParentLineNumber(0U)
       {
       }
 };

typedef ValueType::ValueHolder (*ConstantFunctionPointer)(void);
typedef ValueType::ValueHolder (*UnaryFunctionPointer)(const ValueType::ValueHolder &, const StackFrame &, size_t);
typedef ValueType::ValueHolder (*BinaryFunctionPointer)
   (const ValueType::ValueHolder &, const ValueType::ValueHolder &, const StackFrame &, size_t);
typedef ValueType::ValueHolder (*TernaryFunctionPointer)
   (const ValueType::ValueHolder &, const ValueType::ValueHolder &, const ValueType::ValueHolder &,
    const StackFrame &, size_t);

class CallingContext
 {
   private:
      static std::map<std::string, ConstantFunctionPointer> s_constantFunctions;
      static std::map<std::string, UnaryFunctionPointer> s_unaryFunctions;
      static std::map<std::string, BinaryFunctionPointer> s_binaryFunctions;
      static std::map<std::string, TernaryFunctionPointer> s_ternaryFunctions;

   public:
      const std::string Name;
      const size_t FunctionIndex;
      std::map<std::string, size_t> Locals;
      std::map<std::string, size_t> Statics;
      std::map<std::string, size_t> & Globals;
      std::map<std::string, ValueType::ValueHolder> & Constants;
      std::map<std::string, std::vector<std::string> > & Functions;
      std::map<std::string, size_t> & FunDefs;
      //Need to pass these around for population.
      std::vector<std::vector<ValueType::ValueHolder> > & PopGlobals;
      std::vector<InstructionStream> & PopFunctions;
      std::vector<std::string> & PopFunNames;
      std::vector<std::vector<ValueType::ValueHolder> > & PopFunLocals;
      // Should be a stack, but I need random-access.
      std::vector<std::string> Labels;

      CallingContext(CallingContext & src, const std::string & name) :
         Name(name),
         FunctionIndex(src.FunDefs[name]),
         Globals(src.Globals),
         Constants(src.Constants),
         Functions(src.Functions),
         FunDefs(src.FunDefs),
         PopGlobals(src.PopGlobals),
         PopFunctions(src.PopFunctions),
         PopFunNames(src.PopFunNames),
         PopFunLocals(src.PopFunLocals)
       {
       }

      CallingContext(
         std::map<std::string, size_t> & globals,
         std::map<std::string, ValueType::ValueHolder> & constants,
         std::map<std::string, std::vector<std::string> > & functions,
         std::map<std::string, size_t> & funDefs,
         std::vector<std::vector<ValueType::ValueHolder> > & popGlobals,
         std::vector<InstructionStream> & popFunctions,
         std::vector<std::string> & popFunNames,
         std::vector<std::vector<ValueType::ValueHolder> > & popFunLocals) :

         Name(""),
         FunctionIndex(0U),
         Globals(globals),
         Constants(constants),
         Functions(functions),
         FunDefs(funDefs),
         PopGlobals(popGlobals),
         PopFunctions(popFunctions),
         PopFunNames(popFunNames),
         PopFunLocals(popFunLocals)
       {
       }

      enum IdentifierType
       {
         UNDEFINED,
         VARIABLE,
         CONSTANT,
         FUNCTION,
         STANDARD_FUNCTION,
         LABEL
       };

      enum VariableLocation
       {
         NOT_A_VARIABLE,
         LOCAL,
         STATIC,
         GLOBAL
       };

      enum StandardFunctionType
       {
         NOT_STANDARD_FUNCTION,
         CONSTANT_FUNCTION,
         UNARY_FUNCTION,
         BINARY_FUNCTION,
         TERNARY_FUNCTION
       };

      IdentifierType lookup (const std::string & name)
       {
         if (Locals.end() != Locals.find(name)) return VARIABLE;
         if (Statics.end() != Statics.find(name)) return VARIABLE;
         if (Globals.end() != Globals.find(name)) return VARIABLE;
         if (Constants.end() != Constants.find(name)) return CONSTANT;
         if (Functions.end() != Functions.find(name)) return FUNCTION;
         if (NOT_STANDARD_FUNCTION != getStandardFunctionType(name)) return STANDARD_FUNCTION;
         for (std::vector<std::string>::iterator iter = Labels.begin();
            iter != Labels.end(); ++iter)
          {
            if (name == *iter) return LABEL;
          }
         return UNDEFINED;
       }

      VariableLocation location (const std::string & name)
       {
         if (Locals.end() != Locals.find(name)) return LOCAL;
         if (Statics.end() != Statics.find(name)) return STATIC;
         if (Globals.end() != Globals.find(name)) return GLOBAL;
         return NOT_A_VARIABLE;
       }

      ValueType::ValueHolder createVariable (const std::vector<long> & indexes) const;

      static StandardFunctionType getStandardFunctionType (const std::string & name)
       {
         if (s_unaryFunctions.end() != s_unaryFunctions.find(name)) return UNARY_FUNCTION;
         if (s_binaryFunctions.end() != s_binaryFunctions.find(name)) return BINARY_FUNCTION;
         if (s_constantFunctions.end() != s_constantFunctions.find(name)) return CONSTANT_FUNCTION;
         if (s_ternaryFunctions.end() != s_ternaryFunctions.find(name)) return TERNARY_FUNCTION;
         return NOT_STANDARD_FUNCTION;
       }

      static ConstantFunctionPointer getConstantFunction (const std::string &);
      static UnaryFunctionPointer getUnaryFunction (const std::string &);
      static BinaryFunctionPointer getBinaryFunction (const std::string &);
      static TernaryFunctionPointer getTernaryFunction (const std::string &);
 };

#endif /* SYMBOLTABLE_HPP */
