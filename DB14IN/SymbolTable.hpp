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

class StatementSeq;
class CallingContext;

typedef ValueType::ValueHolder (*ConstantFunctionPointer)(void);
typedef ValueType::ValueHolder (*UnaryFunctionPointer)(const ValueType::ValueHolder &, const CallingContext &, size_t);
typedef ValueType::ValueHolder (*BinaryFunctionPointer)
   (const ValueType::ValueHolder &, const ValueType::ValueHolder &, const CallingContext &, size_t);
typedef ValueType::ValueHolder (*TernaryFunctionPointer)
   (const ValueType::ValueHolder &, const ValueType::ValueHolder &, const ValueType::ValueHolder &,
    const CallingContext &, size_t);

class CallingContext
 {
   private:
      std::map<std::string, ValueType::ValueHolder> m_locals;
      std::map<std::string, std::map<std::string, ValueType::ValueHolder> > & m_allGlobals;

      std::map<std::string, ValueType::ValueHolder> & m_statics;
      std::map<std::string, ValueType::ValueHolder> & m_globals;
      std::map<std::string, ValueType::ValueHolder> & m_constants;
      std::map<std::string, std::vector<std::string> > & m_functions;
      std::map<std::string, std::map<std::string, std::vector<long> > > & m_funLocals;
      std::map<std::string, StatementSeq*> & m_funDefs;

      static std::map<std::string, ConstantFunctionPointer> s_constantFunctions;
      static std::map<std::string, UnaryFunctionPointer> s_unaryFunctions;
      static std::map<std::string, BinaryFunctionPointer> s_binaryFunctions;
      static std::map<std::string, TernaryFunctionPointer> s_ternaryFunctions;

   public:
      const std::string Name;
      std::map<std::string, ValueType::ValueHolder> & Locals() { return m_locals; }
      std::map<std::string, ValueType::ValueHolder> & Statics() { return m_statics; }
      std::map<std::string, ValueType::ValueHolder> & Globals() { return m_globals; }
      std::map<std::string, ValueType::ValueHolder> & Constants() { return m_constants; }
      std::map<std::string, std::vector<std::string> > & Functions() { return m_functions; }
      std::map<std::string, std::map<std::string, std::vector<long> > > & FunLocals() { return m_funLocals; }
      std::map<std::string, StatementSeq*> & FunDefs() { return m_funDefs; }
      const CallingContext * Parent;
      size_t ParentLine;

      CallingContext(CallingContext & src, const std::string & name, size_t lineNo) :
         m_allGlobals(src.m_allGlobals),
         m_statics(src.m_allGlobals[name]),
         m_globals(src.m_globals),
         m_constants(src.m_constants),
         m_functions(src.m_functions),
         m_funLocals(src.m_funLocals),
         m_funDefs(src.m_funDefs),
         Name(name),
         Parent(&src),
         ParentLine(lineNo)
       {
       }

      CallingContext(
         std::map<std::string, std::map<std::string, ValueType::ValueHolder> > & allGlobals,
         std::map<std::string, ValueType::ValueHolder> & constants,
         std::map<std::string, std::vector<std::string> > & functions,
         std::map<std::string, std::map<std::string, std::vector<long> > > & funLocals,
         std::map<std::string, StatementSeq*> & funDefs) :

         m_allGlobals(allGlobals),
         m_statics(allGlobals[""]),
         m_globals(allGlobals[""]),
         m_constants(constants),
         m_functions(functions),
         m_funLocals(funLocals),
         m_funDefs(funDefs),
         Name("BaseContext"),
         Parent(NULL),
         ParentLine(0U)
       {
       }

   // Base CallingContext: CallingContext(Globals, Constants, FunctionArgs, FunctionVars, FunctionDefs);

      // Should be a stack, but I need random-access.
      std::vector<std::string> m_labels;

      enum IdentifierType
       {
         UNDEFINED,
         VARIABLE, // Global, Static, or Local
         CONSTANT,
         FUNCTION,
         STANDARD_FUNCTION,
         LABEL
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
         if (m_locals.end() != m_locals.find(name)) return VARIABLE;
         if (m_statics.end() != m_statics.find(name)) return VARIABLE;
         if (m_globals.end() != m_globals.find(name)) return VARIABLE;
         if (m_constants.end() != m_constants.find(name)) return CONSTANT;
         if (m_functions.end() != m_functions.find(name)) return FUNCTION;
         if (NOT_STANDARD_FUNCTION != getStandardFunctionType(name)) return STANDARD_FUNCTION;
         for (std::vector<std::string>::iterator iter = m_labels.begin();
            iter != m_labels.end(); ++iter)
          {
            if (name == *iter) return LABEL;
          }
         return UNDEFINED;
       }

      ValueType::ValueHolder getValue (const std::string &, size_t) const;
      void setValue (const std::string &, const ValueType::ValueHolder &, size_t);

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
