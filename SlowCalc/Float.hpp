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
   File: Float.hpp
*/

#ifndef FLOAT_HPP
#define FLOAT_HPP

//HEADER:
// Column will hold the significant digits.
// Round contains the encapsulated rounding device.
// string gives us a better way to construct Floats.
#include "Column.hpp"
#include "Round.hpp"
#include <string>

using Col::Column;
using Round::Rounder;

namespace DecFloat
 {

   class Float
    {
      private:
         Column Significand;
         int Exponent;
         bool Infinity;
         bool NaN;
         bool Locked;

         static int maxLength;
         static int seriesExtra;
         static int derivedExtra;

            //This is private as it doesn't work correctly.
            //It is incomplete, but allows the relational
            //operators to work correctly.
         int compare (const Float &) const;

      public:

            //The rounding data was encapsulated to allow this to be a public field
         Rounder Mode;

         std::string getDigits (void) const;
         int exponent (void) const;
         int length (void) const;
         bool sign (void) const;
         bool isZero (void) const;
         bool isInfinity (void) const;
         bool isNaN (void) const;
         bool isNegative (void) const;

         bool isInteger (void) const;
         bool isOdd (void) const;
         bool isEven (void) const;

         std::string toString (void) const;

            //Enforces maxLength:
            //Numbers can always be greater than maxLength,
            //or else we couldn't get maxLength precision
            //in transcendental functions
         int enforceLength (void);
         int setLength (int);
         int lengthen (int);
         int truncate (int);
         void roundAt (int digit);
         void roundPlace (int place);

         Float & negate (void);
         Float & abs (void);

         void Int (void);
         void Frac (void);
         void Normalize (void);

         void unlock (void);
         bool locked (void) const;
         void lock (void);

         static int getMaxLength (void);
         static int setMaxLength (int);
         static int getMinLength (void); //These invoke the Column functions
         static int setMinLength (int);

         static int getSeriesExtra (void);
         static int setSeriesExtra (int);
         static int getDerivedExtra (void);
         static int setDerivedExtra (int);

            //These all need access to the Significand and Exponent
         friend Float operator + (const Float &, const Float &);
         friend Float operator - (const Float &, const Float &);
         friend Float operator * (const Float &, const Float &);
         friend Float operator / (const Float &, const Float &);

            //These need access to compare
         friend bool operator > (const Float &, const Float &);
         friend bool operator >= (const Float &, const Float &);
         friend bool operator < (const Float &, const Float &);
         friend bool operator <= (const Float &, const Float &);
         friend bool operator == (const Float &, const Float &);
         friend bool operator != (const Float &, const Float &);

         Float & operator = (const Float &);
            //Copy digits, not round mode, don't change size:
            //round or zero pad
         Float & operator |= (const Float &);

         Float ();
         Float (const Float &);
         Float (const std::string &);
         explicit Float (long double);

         operator long double () const;


            //These do some mysterious manipulations to get good results
         friend Float sqrt (const Float &);
         friend Float exp (const Float &);
         friend Float log (const Float &);
         friend Float atan (const Float &); //needs compare
    }; //class Float

   //operators: + - * / % ^ (exp not xor) > < >= <= != ==
   //note: exp and ln must be implemented before ^

   Float operator ^ (const Float &, const Float &);

      //The floating point remainder: exact result of (x - int(x/y)*y).
   Float operator % (const Float &, const Float &);

      //Change operator:
      // If adding or subtracting arg1 to arg2
      // returns one of the arguments unchanged,
      // because either arg1 or arg2 is zero, or
      // the difference between the magnitudes
      // of arg1 or arg2 is greater than the
      // length of the longest, then this returns false.
   bool operator && (const Float &, const Float &);

      //Equalize operator:
      // Make arg1 and arg2 the same length by lengthening
      // the shorter one. ALL arithmetic operators use this operator.
   void operator || (Float &, Float &);

   Float operator - (const Float &);

   Float & operator += (Float &, const Float &);
   Float & operator -= (Float &, const Float &);
   Float & operator *= (Float &, const Float &);
   Float & operator /= (Float &, const Float &);

   //functions will be named after their <math.h> counterparts
   //what I will call them will be listed, if different

   //functions, in order of importance:
   //   sqrt exp ln sin cos tan atan asin acos
   //   log alog sinh cosh tanh asinh acosh atanh

   Float sqrt (const Float &);

   Float exp (const Float &);
   Float log (const Float &); //ln

   Float sin (const Float &);
   Float cos (const Float &);
   Float tan (const Float &);

   Float atan (const Float &);
   Float asin (const Float &);
   Float acos (const Float &);

   Float log10 (const Float &); //log
   Float exp10 (const Float &); //alog

   Float sinh (const Float &);
   Float cosh (const Float &);
   Float tanh (const Float &);

   Float asinh (const Float &);
   Float acosh (const Float &);
   Float atanh (const Float &);

   Float atan2 (const Float &, const Float &);

   Float cbrt (const Float &); //cubrt
   Float fma (const Float &, const Float &, const Float &); //mac

   Float rand (void);
   Float fact (const Float &);

   extern const Float fNaN; //Not a number
   extern const Float fInf; //Positive infinity

 } //namespace DecFloat

#endif /* FLOAT_HPP */
