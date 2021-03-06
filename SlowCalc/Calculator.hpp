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
   File: Calculator.hpp
*/

#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

//HEADER:
// Stack includes our stack class.
// string includes strings
#include "Stack.hpp"
#include <string>

const int actualTotalStacks = 4;

class Calculator
 {
   private:
      DecFloat::Stack stacks [actualTotalStacks];
      int currentStack;

      static const int totalStacks;

   public:
      void doStuff (std::string);
      void help (void);

      Calculator();

 };

#endif /* CALCULATOR_HPP */
