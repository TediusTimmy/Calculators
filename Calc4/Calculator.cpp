/*
Copyright (c) 2013 Thomas DiModica.
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
#include "Calculator.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <map>

#ifdef USE_RAND850
namespace DecFloat
 {
   Float rand (void);
 }
#endif

using namespace DecFloat;

const int Calculator::totalStacks = actualTotalStacks;

Calculator::Calculator() : currentStack(0) { }

enum SupportedOperations
 {
   NEG,
   ABS,

   ADDITION,
   SUBTRACTION,
   MULTIPLICATION,
   DIVISION,
   REMAINDER,
   EXPONENTIATION,

//   MODULUS,

   SQRT,
   CBRT,
   SQR,
   EXP,
   LN,

   SIN,
   COS,
   TAN,
   ASIN,
   ACOS,
   ATAN,

   SINCOS,
   ATAN2,

   SINH,
   COSH,
   TANH,
   ASINH,
   ACOSH,
   ATANH,

   HYPOT,
   ERF,
   ERFC,
   GAMMA,
   LNGAMMA,
//   LGAMMA,
   FMA,

   ROUND,
   TRUNC,
   CEIL,
   FLOOR,
   FRAC,

   LOG,
   ALOG,
   EXPM1,
   LOG1P,

   LENGTHEN,
   SHORTEN,
   PREC,

   RAND,

   COPY,
   SWAP,
   POP,
   FLUSH,
   FLUSHALL,
   ROT,

   PRINT,
   PRINT_POP,

   PI_32,
   PI_128,
   PI_256,

   ROUND_MODE,

   STACK,
   PUSHTO,
   POPTO,
   PUSHFROM,
   POPFROM,

   HELP,

   INVALID
 };

static std::map<std::string, SupportedOperations> buildOperationsMap (void)
 {
   std::map<std::string, SupportedOperations> ret;

   ret.insert(std::make_pair("neg", NEG));
   ret.insert(std::make_pair("abs", ABS));

   ret.insert(std::make_pair("+", ADDITION));
   ret.insert(std::make_pair("-", SUBTRACTION));
   ret.insert(std::make_pair("*", MULTIPLICATION));
   ret.insert(std::make_pair("/", DIVISION));
   ret.insert(std::make_pair("%", REMAINDER));
   ret.insert(std::make_pair("^", EXPONENTIATION));
   ret.insert(std::make_pair("**", EXPONENTIATION));

   ret.insert(std::make_pair("sqrt", SQRT));
   ret.insert(std::make_pair("cubrt", CBRT));
   ret.insert(std::make_pair("sqr", SQR));
   ret.insert(std::make_pair("exp", EXP));
   ret.insert(std::make_pair("ln", LN));

   ret.insert(std::make_pair("sin", SIN));
   ret.insert(std::make_pair("cos", COS));
   ret.insert(std::make_pair("tan", TAN));
   ret.insert(std::make_pair("asin", ASIN));
   ret.insert(std::make_pair("acos", ACOS));
   ret.insert(std::make_pair("atan", ATAN));

   ret.insert(std::make_pair("sincos", SINCOS));
   ret.insert(std::make_pair("atan2", ATAN2));

   ret.insert(std::make_pair("sinh", SINH));
   ret.insert(std::make_pair("cosh", COSH));
   ret.insert(std::make_pair("tanh", TANH));
   ret.insert(std::make_pair("asinh", ASINH));
   ret.insert(std::make_pair("acosh", ACOSH));
   ret.insert(std::make_pair("atanh", ATANH));

   ret.insert(std::make_pair("hypot", HYPOT));
   ret.insert(std::make_pair("erf", ERF));
   ret.insert(std::make_pair("erfc", ERFC));
   ret.insert(std::make_pair("gamma", GAMMA));
   ret.insert(std::make_pair("lngamma", LNGAMMA));
   ret.insert(std::make_pair("mac", FMA));

   ret.insert(std::make_pair("round", ROUND));
   ret.insert(std::make_pair("trunc", TRUNC));
   ret.insert(std::make_pair("ceil", CEIL));
   ret.insert(std::make_pair("floor", FLOOR));
   ret.insert(std::make_pair("frac", FRAC));

   ret.insert(std::make_pair("log", LOG));
   ret.insert(std::make_pair("alog", ALOG));
   ret.insert(std::make_pair("expm1", EXPM1));
   ret.insert(std::make_pair("log1p", LOG1P));

   ret.insert(std::make_pair("shorten", SHORTEN));
   ret.insert(std::make_pair("lengthen", LENGTHEN));
   ret.insert(std::make_pair("length", PREC));

   ret.insert(std::make_pair("rand", RAND));

   ret.insert(std::make_pair("copy", COPY));
   ret.insert(std::make_pair("swap", SWAP));
   ret.insert(std::make_pair("pop", POP));
   ret.insert(std::make_pair("flush", FLUSH));
   ret.insert(std::make_pair("flushall", FLUSHALL));
   ret.insert(std::make_pair("rot", ROT));

   ret.insert(std::make_pair("print", PRINT));
   ret.insert(std::make_pair("Print", PRINT_POP));

   ret.insert(std::make_pair("pi", PI_32));
   ret.insert(std::make_pair("Pi", PI_128));
   ret.insert(std::make_pair("PI", PI_256));

   ret.insert(std::make_pair("mode", ROUND_MODE));

   ret.insert(std::make_pair("stack", STACK));
   ret.insert(std::make_pair("pushto", PUSHTO));
   ret.insert(std::make_pair("popto", POPTO));
   ret.insert(std::make_pair("pushfrom", PUSHFROM));
   ret.insert(std::make_pair("popfrom", POPFROM));

   ret.insert(std::make_pair("help", HELP));

   return ret;
 }

static const std::map<std::string, SupportedOperations> OperationsMap (buildOperationsMap());

static std::map<std::string, SupportedRoundModes> buildSupportedRoundModesMap (void)
 {
   std::map<std::string, SupportedRoundModes> ret;

   ret.insert(std::make_pair("ROUND_TO_NEAREST_TIES_TO_EVEN", ROUND_TO_NEAREST_TIES_TO_EVEN));
   ret.insert(std::make_pair("ROUND_TO_POSITIVE_INFINITY", ROUND_TO_POSITIVE_INFINITY));
   ret.insert(std::make_pair("ROUND_TO_NEGATIVE_INFINITY", ROUND_TO_NEGATIVE_INFINITY));
   ret.insert(std::make_pair("ROUND_TO_ZERO", ROUND_TO_ZERO));
   ret.insert(std::make_pair("ROUND_AWAY_FROM_ZERO", ROUND_AWAY_FROM_ZERO));

   return ret;
 }

static const std::map<std::string, SupportedRoundModes> SupportedRoundModesMap (buildSupportedRoundModesMap());

void Calculator::doStuff (const std::string & doThis)
 {
   Float left, right, other; //To be used in the math
   int someThing; //Has random uses.

   std::map<std::string, SupportedOperations>::const_iterator operation = OperationsMap.find(doThis);

   SupportedOperations op = INVALID;
   if (OperationsMap.end() != operation)
    {
      op = operation->second;
    }

   switch (op)
    {
      case ADDITION:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(left + right);
         break;

      case SUBTRACTION:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(left - right);
         break;

      case MULTIPLICATION:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(left * right);
         break;

      case DIVISION:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(left / right);
         break;

      case REMAINDER:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(left % right);
         break;

      case EXPONENTIATION:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(pow(left, right));
         break;



      case SQRT:
         stacks[currentStack].push(sqrt(stacks[currentStack].pop()));
         break;

      case CBRT:
         stacks[currentStack].push(cbrt(stacks[currentStack].pop()));
         break;

      case SQR:
         stacks[currentStack].push(sqr(stacks[currentStack].pop()));
         break;

      case EXP:
         stacks[currentStack].push(exp(stacks[currentStack].pop()));
         break;

      case LN:
         stacks[currentStack].push(log(stacks[currentStack].pop()));
         break;

      case LOG:
         stacks[currentStack].push(log10(stacks[currentStack].pop()));
         break;

      case ALOG:
         stacks[currentStack].push(exp10(stacks[currentStack].pop()));
         break;

      case EXPM1:
         stacks[currentStack].push(expm1(stacks[currentStack].pop()));
         break;

      case LOG1P:
         stacks[currentStack].push(log1p(stacks[currentStack].pop()));
         break;


      case SIN:
         stacks[currentStack].push(sin(stacks[currentStack].pop()));
         break;

      case COS:
         stacks[currentStack].push(cos(stacks[currentStack].pop()));
         break;

      case TAN:
         stacks[currentStack].push(tan(stacks[currentStack].pop()));
         break;

      case ASIN:
         stacks[currentStack].push(asin(stacks[currentStack].pop()));
         break;

      case ACOS:
         stacks[currentStack].push(acos(stacks[currentStack].pop()));
         break;

      case ATAN:
         stacks[currentStack].push(atan(stacks[currentStack].pop()));
         break;

      case SINH:
         stacks[currentStack].push(sinh(stacks[currentStack].pop()));
         break;

      case COSH:
         stacks[currentStack].push(cosh(stacks[currentStack].pop()));
         break;

      case TANH:
         stacks[currentStack].push(tanh(stacks[currentStack].pop()));
         break;

      case ASINH:
         stacks[currentStack].push(asinh(stacks[currentStack].pop()));
         break;

      case ACOSH:
         stacks[currentStack].push(acosh(stacks[currentStack].pop()));
         break;

      case ATANH:
         stacks[currentStack].push(atanh(stacks[currentStack].pop()));
         break;


      case ATAN2:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(atan2(left, right));
         break;

      case SINCOS:
         sincos(stacks[currentStack].pop(), left, right);
         stacks[currentStack].push(right);
         stacks[currentStack].push(left);
         break;



      case NEG:
         stacks[currentStack].top().negate();
         break;

      case ABS:
         stacks[currentStack].top().negate();
         break;



      case HYPOT:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(hypot(left, right));
         break;

      case ERF:
         stacks[currentStack].push(erf(stacks[currentStack].pop()));
         break;

      case ERFC:
         stacks[currentStack].push(erfc(stacks[currentStack].pop()));
         break;

      case GAMMA:
         stacks[currentStack].push(gamma(stacks[currentStack].pop()));
         break;

      case LNGAMMA:
         stacks[currentStack].push(lngamma(stacks[currentStack].pop()));
         break;

      case FMA:
         other = stacks[currentStack].pop();
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(fma(left, right, other));
         break;



      case ROUND:
         stacks[currentStack].push(round(stacks[currentStack].pop()));
         break;

      case TRUNC:
         stacks[currentStack].push(trunc(stacks[currentStack].pop()));
         break;

      case CEIL:
         stacks[currentStack].push(ceil(stacks[currentStack].pop()));
         break;

      case FLOOR:
         stacks[currentStack].push(floor(stacks[currentStack].pop()));
         break;

      case FRAC:
         stacks[currentStack].push(frac(stacks[currentStack].pop()));
         break;



      case LENGTHEN:
         std::cin >> someThing;
         stacks[currentStack].top().changePrecision(stacks[currentStack].top().getPrecision() + someThing);
         stacks[currentStack].top().enforcePrecision();
         break;

      case SHORTEN:
         std::cin >> someThing;
         stacks[currentStack].top().changePrecision(stacks[currentStack].top().getPrecision() - someThing);
         stacks[currentStack].top().enforcePrecision();
         break;

      case PREC:
         std::cin >> someThing;
         stacks[currentStack].top().changePrecision(someThing);
         stacks[currentStack].top().enforcePrecision();
         break;



      case RAND:
#ifdef USE_RAND850
         stacks[currentStack].push(DecFloat::rand());
         stacks[currentStack].top().enforcePrecision();
#else
         std::cout << "Not implemented yet." << std::endl;
#endif
         break;



      case POP:
         stacks[currentStack].pop();
         break;

      case FLUSH:
         stacks[currentStack].flush();
         break;

      case FLUSHALL:
         for (someThing = 0; someThing < totalStacks; someThing++)
            stacks[someThing].flush();
         break;

      case COPY:
         stacks[currentStack].push(stacks[currentStack].top());
         break;

      case SWAP:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         stacks[currentStack].push(right);
         stacks[currentStack].push(left);
         break;

      case ROT:
         right = stacks[currentStack].pop();
         left = stacks[currentStack].pop();
         other = stacks[currentStack].pop();
         stacks[currentStack].push(right);
         stacks[currentStack].push(other);
         stacks[currentStack].push(left);
         break;



      case PRINT:
         if (stacks[currentStack].isEmpty())
            std::cout << "Stack is empty." << std::endl;
         else
            std::cout << stacks[currentStack].top().toString() << std::endl;
         break;

      case PRINT_POP:
         if (stacks[currentStack].isEmpty())
            std::cout << "Stack is empty." << std::endl;
         else
            std::cout << stacks[currentStack].pop().toString() << std::endl;
         break;



      case PI_32:
         stacks[currentStack].push(pi(32U));
         stacks[currentStack].top().enforcePrecision();
         break;

      case PI_128:
         stacks[currentStack].push(pi(128U));
         stacks[currentStack].top().enforcePrecision();
         break;

      case PI_256:
         stacks[currentStack].push(pi(256U));
         stacks[currentStack].top().enforcePrecision();
         break;



      case STACK:
         std::cin >> someThing;
         if ((someThing < 0) || (someThing >= totalStacks))
            std::cerr << "Invalid stack number." << std::endl;
         else
            currentStack = someThing;
         stacks[currentStack].focus();
         break;

      case PUSHTO:
         std::cin >> someThing;
         if ((someThing < 0) || (someThing >= totalStacks))
            std::cerr << "Invalid stack number." << std::endl;
         else
            stacks[someThing].push(stacks[currentStack].top());
         break;

      case POPTO:
         std::cin >> someThing;
         if ((someThing < 0) || (someThing >= totalStacks))
            std::cerr << "Invalid stack number." << std::endl;
         else
            stacks[someThing].push(stacks[currentStack].pop());
         break;

      case PUSHFROM:
         std::cin >> someThing;
         if ((someThing < 0) || (someThing >= totalStacks))
            std::cerr << "Invalid stack number." << std::endl;
         else
            stacks[currentStack].push(stacks[someThing].top());
         break;

      case POPFROM:
         std::cin >> someThing;
         if ((someThing < 0) || (someThing >= totalStacks))
            std::cerr << "Invalid stack number." << std::endl;
         else
            stacks[currentStack].push(stacks[someThing].pop());
         break;



      case HELP:
         help();
         break;



      case ROUND_MODE:
       {
         std::string mode;
         std::cin >> mode;
         std::map<std::string, SupportedRoundModes>::const_iterator modePtr = SupportedRoundModesMap.find(mode);
         if (SupportedRoundModesMap.end() != modePtr)
          {
            stacks[currentStack].setRoundMode(modePtr->second);
          }
         else
          {
            std::cerr << "Invalid round mode." << std::endl;
          }
         stacks[currentStack].focus();
       }
         break;



      case INVALID:
         left = Float(doThis);
         if (left.getPrecision() < Float::getMinPrecision())
          {
            left = pi(Float::getMinPrecision());
            left.fromString(doThis);
          }
         else if (left.getPrecision() > Float::getMaxPrecision())
          {
            left = pi(Float::getMaxPrecision());
            left.fromString(doThis);
          }
         stacks[currentStack].push(left);
         break;

    }
 }

void Calculator::help (void)
 {
   std::ifstream topics ("HELPTEXT.txt");
   std::string curline, selection;

   if (!topics)
    {
      std::cout << "Cannot open help file." << std::endl;
      return;
    }

   getline(topics, curline);
   while (curline[0] != '%')
    {
      if (curline[0] != '!') std::cout << curline << std::endl;
      getline(topics, curline);
    }

   std::cin >> selection;
   selection = '#' + selection;
   while (selection != "#quit")
    {
      topics.seekg(0);
      getline(topics, curline);
      while (curline[0] != '%') getline(topics, curline);

      getline(topics, curline);
      while ((curline != selection) && (curline[0] != '&'))
         getline(topics, curline);

      getline(topics, curline);
      while (curline[0] != '%')
       {
         if ((curline[0] != '#') && (curline[0] != '!'))
            std::cout << curline << std::endl;
         getline(topics, curline);
       }

      std::cin >> selection;
      selection = '#' + selection;
    }

   return;
 }
