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
   File: Column.hpp
*/

#ifndef COLUMN_HPP
#define COLUMN_HPP

//HEADER:
// string as we allow the construction of a column from a string
#include <string>

namespace Col
 {

   extern const int hardMinLength;

   class Column
    {
      private:
         char * Digits;
         bool Sign;
         int Length;
         bool Zero;
         bool Overflow;

         static int minLength;

         static void addSubHelper (bool, char *, bool, char *, Column &, int);

      public:
         //All operators must occur with opperands of the same length, except all assignment
         //These are all defined so the logic in the code for the basic ops shows through
         //without relying too much on strange method calls
         Column operator + (const Column &) const;
         Column operator - (const Column &) const;

         Column & operator += (const Column &);
         Column & operator -= (const Column &);

         Column & operator >>= (int);
         Column & operator <<= (int);

         //Sign independent increment for rounding (increment away from zero)
         Column operator ++ (int);

         Column & operator = (const Column &);
         Column & operator |= (const Column &); //Left-justified, no grow or shrink
         Column & operator &= (const Column &); //Right-justified, no length change
         Column & operator *= (const Column &); //One zero-padded, left-justified

         bool operator > (const Column &);
         bool operator >= (const Column &);
         bool operator <= (const Column &);
         bool operator < (const Column &);
         bool operator == (const Column &);
         bool operator != (const Column &);

         const char * digits (void) const;
         int length (void) const;
         bool sign (void) const;
         bool isZero (void) const;
         bool overflow (void) const;

         int compare (const Column &);
         int compare (const Column &, int);

         void setToZero (void);
         void setToZero (int from, int upToButNotIncluding);
         void clearOverflow (void);

         void incrementAt (int);

         void negate (void);
         void absoluteValue (void);

         int lengthen (int delta); //Always grows and shrinks on the right
         int truncate (int delta);
         int setLength (int absolute);

         bool isGreaterThan (const Column &, int startingHere); //for Division
         bool isGreaterThanOrEqualTo (const Column &, int startingHere);

         int digitAt (int) const;
         int setDigit (int, int);

         Column ();
         Column (const Column &);
         Column (const std::string &);

         ~Column();

         static int getMinLength (void);
         static int setMinLength (int);

    }; // class Column

 } // namespace Col

#endif /* COLUMN_HPP */
