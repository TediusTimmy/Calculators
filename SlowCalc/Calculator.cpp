/*
   This file is part of SlowCalc.

   Copyright (C) 2010 Thomas DiModica <ricinwich@yahoo.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
   File: Calculator.cpp
      The primary member-function definitions for the calculator class.
*/

//HEADER:
// Calculator is the structure we're dealing with.
// Constants has constants we need
// Round allows us to do rounding manipulations
// string is a data type we'll use
// iostream define cin, cout, and cerr to do IO
// fstream is used by help for my help file
#include "Calculator.hpp"
#include "Constants.hpp"
#include "Round.hpp"
#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::ifstream;

using namespace DecFloat;

const int Calculator::totalStacks = actualTotalStacks;

/*
==============================================================================
   Function: no argument Calculator constructor
==============================================================================
*/
Calculator::Calculator() : currentStack(0) { }

/*
==============================================================================
   Function: doStuff
------------------------------------------------------------------------------
   INPUT: string command to do
   NOTES:
      This is the main driving function of the Calculator class.
      It could be more client/server and never have cin or cout,
      but that would add complexity that is unnecessary at this point.
==============================================================================
*/
void Calculator::doStuff (string doThis)
 {
   Float left, right, other; //To be used in the math
   int someThing; //Has random uses.

   if (doThis == "+")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left + right);
    }
   else if (doThis == "-")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left - right);
    }
   else if (doThis == "*")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left * right);
    }
   else if (doThis == "/")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left / right);
    }
   else if (doThis == "%")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left % right);
    }
   else if (doThis == "^")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left ^ right);
    }
   else if (doThis == "sqrt")
    {
      //If I had an atomic top/pop, then these would all just be one line.
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(sqrt(left));
    }
   else if (doThis == "exp")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(exp(left));
    }
   else if (doThis == "ln")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(log(left));
    }
   else if (doThis == "sin")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(sin(left));
    }
   else if (doThis == "cos")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(cos(left));
    }
   else if (doThis == "tan")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(tan(left));
    }
   else if (doThis == "asin")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(asin(left));
    }
   else if (doThis == "acos")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(acos(left));
    }
   else if (doThis == "atan")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(atan(left));
    }
   else if (doThis == "log")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(log10(left));
    }
   else if (doThis == "alog")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(exp10(left));
    }
   else if (doThis == "sinh")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(sinh(left));
    }
   else if (doThis == "cosh")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(cosh(left));
    }
   else if (doThis == "tanh")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(tanh(left));
    }
   else if (doThis == "asinh")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(asinh(left));
    }
   else if (doThis == "acosh")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(acosh(left));
    }
   else if (doThis == "atanh")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(atanh(left));
    }
   else if (doThis == "atan2")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(atan2(left, right));
    }
   else if (doThis == "cubrt")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(cbrt(left));
    }
   else if (doThis == "mac")
    {
      other = stacks[currentStack].top(); stacks[currentStack].pop();
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(fma(left, right, other));
    }
   else if (doThis == "neg")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left.negate());
    }
   else if (doThis == "abs")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left.abs());
    }
   else if (doThis == "int")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      left.Int();
      stacks[currentStack].push(left);
    }
   else if (doThis == "frac")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      left.Frac();
      stacks[currentStack].push(left);
    }
   else if (doThis == "sqr")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(left * left);
    }
   else if (doThis == "roundd")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.roundAt(someThing);
      stacks[currentStack].push(left);
    }
   else if (doThis == "roundp")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.roundPlace(someThing);
      stacks[currentStack].push(left);
    }
   else if (doThis == "shorten")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.roundAt(left.length() - someThing);
      left.truncate(someThing);
      stacks[currentStack].push(left);
    }
   else if (doThis == "truncate")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.truncate(someThing);
      stacks[currentStack].push(left);
    }
   else if (doThis == "lengthen")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.lengthen(someThing);
      left.enforceLength();
      stacks[currentStack].push(left);
    }
   else if (doThis == "length")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      cin >> someThing;
      left.setLength(someThing);
      left.enforceLength();
      stacks[currentStack].push(left);
    }
   else if (doThis == "pop")
    {
      stacks[currentStack].pop();
    }
   else if (doThis == "flush")
    {
      stacks[currentStack].flush();
    }
   else if (doThis == "flushall")
    {
      for (someThing = 0; someThing < totalStacks; someThing++) stacks[someThing].flush();
    }
   else if (doThis == "copy")
    {
      stacks[currentStack].push(stacks[currentStack].top());
    }
   else if (doThis == "swap")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(right);
      stacks[currentStack].push(left);
    }
   else if (doThis == "rot")
    {
      right = stacks[currentStack].top(); stacks[currentStack].pop();
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      other = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(right);
      stacks[currentStack].push(other);
      stacks[currentStack].push(left);
    }
   else if (doThis == "print")
    {
      if (stacks[currentStack].isEmpty())
         cout << "Stack is empty." << endl;
      else
         cout << stacks[currentStack].top().toString() << endl;
    }
   else if (doThis == "Print")
    {
      if (stacks[currentStack].isEmpty())
         cout << "Stack is empty." << endl;
      else
         cout << stacks[currentStack].top().toString() << endl;
      stacks[currentStack].pop();
    }
   else if (doThis == "pi")
    {
      right = DecFloat::M_PI;
      right.roundAt(32);
      right.setLength(32);
      stacks[currentStack].push(right);
    }
   else if (doThis == "Pi")
    {
      right = DecFloat::M_PI;
      right.roundAt(128);
      right.setLength(128);
      stacks[currentStack].push(right);
    }
   else if (doThis == "PI")
    {
      right = DecFloat::M_PI;
      right.roundAt(256);
      right.setLength(256);
      stacks[currentStack].push(right);
    }
   else if (doThis == "mode")
    {
      cin >> doThis;
      if (doThis == "ROUND_TO_NEAREST_TIES_TO_EVEN")
         stacks[currentStack].setRoundMode(Round::ROUND_TO_NEAREST_TIES_TO_EVEN);
      else if (doThis == "ROUND_TO_NEAREST_TIES_AWAY_FROM_ZERO")
         stacks[currentStack].setRoundMode(Round::ROUND_TO_NEAREST_TIES_AWAY_FROM_ZERO);
      else if (doThis == "ROUND_TO_POSITIVE_INFINITY")
         stacks[currentStack].setRoundMode(Round::ROUND_TO_POSITIVE_INFINITY);
      else if (doThis == "ROUND_TO_NEGATIVE_INFINITY")
         stacks[currentStack].setRoundMode(Round::ROUND_TO_NEGATIVE_INFINITY);
      else if (doThis == "ROUND_TO_ZERO")
         stacks[currentStack].setRoundMode(Round::ROUND_TO_ZERO);
      else if (doThis == "ROUND_TO_NEAREST_TIES_TO_ODD")
         stacks[currentStack].setRoundMode(Round::ROUND_TO_NEAREST_TIES_TO_ODD);
      else if (doThis == "ROUND_TO_NEAREST_TIES_TO_ZERO")
         stacks[currentStack].setRoundMode(Round::ROUND_TO_NEAREST_TIES_TO_ZERO);
      else if (doThis == "ROUND_AWAY_FROM_ZERO")
         stacks[currentStack].setRoundMode(Round::ROUND_AWAY_FROM_ZERO);
      else
         cerr << "Invalid round mode." << endl;
      stacks[currentStack].focus();
    }
   else if (doThis == "stack")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
         currentStack = someThing;
      stacks[currentStack].focus();
    }
   else if (doThis == "pushto")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
         stacks[someThing].push(stacks[currentStack].top());
    }
   else if (doThis == "popto")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
       {
         stacks[someThing].push(stacks[currentStack].top());
         stacks[currentStack].pop();
       }
    }
   else if (doThis == "pushfrom")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
         stacks[currentStack].push(stacks[someThing].top());
    }
   else if (doThis == "popfrom")
    {
      cin >> someThing;
      if ((someThing < 0) || (someThing >= totalStacks))
         cerr << "Invalid stack number." << endl;
      else
       {
         stacks[currentStack].push(stacks[someThing].top());
         stacks[someThing].pop();
       }
    }
   else if (doThis == "rand")
    {
      stacks[currentStack].push(DecFloat::rand());
    }
   else if (doThis == "fact")
    {
      left = stacks[currentStack].top(); stacks[currentStack].pop();
      stacks[currentStack].push(fact(left));
    }
   else if (doThis == "help")
    {
      help();
    }
   else
    {
      left = Float(doThis);
      left.Normalize();
      left.enforceLength();
      stacks[currentStack].push(left);
    }
 }

/*
==============================================================================
   Function: help
------------------------------------------------------------------------------
   NOTES:
      This is an interactive on-line help system for the calculator
      class.
==============================================================================
*/
void Calculator::help (void)
 {
   ifstream topics ("HELPTEXT.txt");
   string curline, selection;

   if (!topics)
    {
      cout << "Cannot open help file." << endl;
      return;
    }

   getline(topics, curline);
   while (curline[0] != '%')
    {
      if (curline[0] != '!') cout << curline << endl;
      getline(topics, curline);
    }

   cin >> selection;
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
            cout << curline << endl;
         getline(topics, curline);
       }

      cin >> selection;
      selection = '#' + selection;
    }

   return;
 }
