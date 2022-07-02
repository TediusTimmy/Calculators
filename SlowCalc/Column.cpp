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
   File: Column.cpp
      I stole the name column from the Analytic Engine:
      picture a column of digit wheels that can be added and subtracted.
*/

//HEADER:
// Column, as we need the definition of a Column
//   and Column includes <string>
// cstring for the memory manipulation functions contained there in
#include "Column.hpp"
#include <cstring>

using std::memcpy;
using std::memmove;
using std::memset;

/* The fancy logic for the two other modes of calculators. */
#ifndef MODE_1
 #ifndef MODE_2
  #define MODE_0
 #endif /* NOT MODE_2 */
#endif /* NOT MODE_1 */

namespace Col
 {
      //No Column can be smaller than this.
   const int hardMinLength = 1;
      //The smallest Column, by current definition.
#ifdef MODE_0
   int Column::minLength = 8;
#endif
#ifdef MODE_1
   int Column::minLength = 50;
#endif
#ifdef MODE_2
   int Column::minLength = 70;
#endif

/*
==============================================================================
   Function: getMinLength
==============================================================================
*/
   int Column::getMinLength (void) { return minLength; }

/*
==============================================================================
   Function: setMinLength
==============================================================================
*/
   int Column::setMinLength (int newLength)
    {
      if (newLength < hardMinLength) newLength = hardMinLength;
      return (minLength = newLength);
    }

/*
==============================================================================
   Function: no argument Column constructor
==============================================================================
*/
   Column::Column () :
      Digits(NULL), Sign(false), Length(minLength), Zero(true), Overflow(false)
    {

    }

/*
==============================================================================
   Function: Column destructor
==============================================================================
*/
   Column::~Column ()
    {
      if (Digits != NULL) delete [] Digits;
      Digits = NULL;
    }

/*
==============================================================================
   Function: copy constructor
==============================================================================
*/
   Column::Column (const Column & src)
    {
      if (src.Zero) //Then Digits is NULL and we don't need to allocate it.
       {
         Digits = NULL;
         Sign = src.Sign;
         Length = src.Length;
         Zero = src.Zero;
         Overflow = src.Overflow;
       }
      else //Allocate Digits and copy the payload
       {
         Digits = new char [src.Length];
         memcpy(Digits, src.Digits, src.Length);
         Sign = src.Sign;
         Length = src.Length;
         Zero = src.Zero;
         Overflow = src.Overflow;
       }
    } //Column (const Column &)

/*
==============================================================================
   Function: constructor from a string
==============================================================================
*/
   Column::Column (const std::string & srcString)
    {
      const char * src = srcString.c_str();
      const char * p = src;
      if ((p == NULL) || (srcString.length() == 0))
       { //zero it
         Digits = NULL;
         Sign = false;
         Length = minLength;
         Zero = true;
         Overflow = false;
       }
      else
       {
         Overflow = false;
         if (*p == '-')
          {
            Sign = true;
            p++;
            src++;
          }
         else Sign = false;
         while ((*p >= '0') && (*p <= '9')) p++; //0-9 are in order in ASCII and EBCDIC
         int tempLength = p - src;
         if (tempLength < minLength) Length = minLength;
         else Length = tempLength;
         Digits = new char [Length];
         memset(Digits, 0, Length);
         int i;
         for (i = 0; i < tempLength; i++) Digits[i] = src[i] - '0';
         for (i = 0; i < Length; i++) if (Digits[i]) break;
         if (i == Length) //We are zero
          {
            Zero = true;
            delete [] Digits;
            Digits = NULL;
          }
         else Zero = false;
       }
    } //Column (string)

/*
==============================================================================
   Function group: member variable access functions
==============================================================================
*/
   const char * Column::digits (void) const { return Digits; }
   int Column::length (void) const { return Length; }
   bool Column::sign (void) const { return Sign; }
   bool Column::isZero (void) const { return Zero; }
   bool Column::overflow (void) const { return Overflow; }

/*
==============================================================================
   Function group: member variable manipulation functions
==============================================================================
*/
   void Column::negate (void) { Sign = !Sign; }
   void Column::absoluteValue (void) { Sign = false; }
   void Column::clearOverflow (void) { Overflow = false; }

/*
==============================================================================
   Function: setToZero
==============================================================================
*/
   void Column::setToZero (void)
    {
      if (Digits != NULL)
       {
         delete [] Digits;
         Digits = NULL;
       }
      Zero = true;
    } //setToZero (void)

/*
==============================================================================
   Function: setToZero range
------------------------------------------------------------------------------
   NOTES:
      Works on a range rather than setting the whole Column.
==============================================================================
*/
   void Column::setToZero (int from, int to)
    {
      if (Zero) return;
      int i;
      if (from < 0) from = 0;
      else if (from > Length) from = Length;
      if (to > Length) to = Length;
      if ((from == 0) && (to == Length)) //regular setToZero
       {
         if (Digits != NULL)
          {
            delete [] Digits;
            Digits = NULL;
          }
         Zero = true;
         return;
       }
      else for (i = from; i < to; i++) Digits[i] = 0;

      for (i = 0; i < Length; i++) if (Digits[i]) break;
      if (i == Length) //number is zero
       {
         Zero = true;
         delete [] Digits;
         Digits = NULL;
       }
    } //setToZero (int, int)

/*
==============================================================================
   Function: lengthen by an amount
==============================================================================
*/
   int Column::lengthen (int delta)
    {
      if (Zero) Length += delta;
      else
       {
         int newLength = Length + delta;
         char * newDigits = new char [newLength];
         memcpy(newDigits, Digits, Length);
         memset(newDigits + Length, 0, delta);
         Length = newLength;
         delete [] Digits;
         Digits = newDigits;
       }
      return Length;
    } //lengthen

/*
==============================================================================
   Function: truncate (shorten by an amount)
==============================================================================
*/
   int Column::truncate (int delta)
    {
      if (Zero)
       {
         Length -= delta;
         if (Length < minLength) Length = minLength;
       }
      else
       {
         int newLength = Length - delta;
         if (newLength < minLength) newLength = minLength;
         if (newLength != Length)
          {
            char * newDigits = new char [newLength];
            memcpy(newDigits, Digits, newLength);
            Length = newLength;
            delete [] Digits;
            Digits = newDigits;
          }
       }
      return Length;
    } //truncate

/*
==============================================================================
   Function: setLength
==============================================================================
*/
   int Column::setLength (int absolute)
    {
      if (absolute < minLength) absolute = minLength;
      if (Zero) Length = absolute;
      else if (absolute != Length)
       {
         char * newDigits = new char [absolute];
         if (absolute > Length)
          {
            memcpy(newDigits, Digits, Length);
            memset(newDigits + Length, 0, absolute - Length);
          }
         else memcpy(newDigits, Digits, absolute);
         Length = absolute;
         delete [] Digits;
         Digits = newDigits;
       }
      return Length;
    } //setLength

/*
==============================================================================
   Function: basicCompare (file scope only)
------------------------------------------------------------------------------
   OUTPUT:
      <0  if left <  right
      0   if left == right
      >0  if left >  right
   NOTES:
      Array contents comparison.
==============================================================================
*/
   static int basicCompare (char * left, char * right, int len)
    {
      int i;
      for (i = 0; i < len; i++) if (left[i] != right[i]) break;
      if (i == len) return 0; //equal
      return (left[i] - right[i]); //not equal
    }

/*
==============================================================================
   Function: compare
------------------------------------------------------------------------------
   OUTPUT:
      <0  if left <  right
      0   if left == right
      >0  if left >  right
   NOTES:
      Returns 0 if the two columns are of different lengths.
      It does not even try a comparison.
==============================================================================
*/
   int Column::compare (const Column & right)
    {
      if (Length != right.Length) return 0;
      if (Zero && right.Zero) return 0;

      if (Sign && right.Sign)
       {
         if (Zero && !right.Zero) return 1;
         else if (!Zero && right.Zero) return -1;
         return -basicCompare(Digits, right.Digits, Length);
       }
      else if (!Sign && right.Sign) return -1;
      else if (Sign && !right.Sign) return 1;

      if (Zero && !right.Zero) return -1;
      else if (!Zero && right.Zero) return 1;
      return basicCompare(Digits, right.Digits, Length);
    }

/*
==============================================================================
   Function: compare
------------------------------------------------------------------------------
   OUTPUT:
      <0  if left <  right
      0   if left == right
      >0  if left >  right
   NOTES:
      Allows specification of a starting index to do the compare.
      This will speed up division so that it does not have to compare
      the leading zeros in the fields it works upon.

      Returns 0 if the two columns are of different lengths.
      It does not even try a comparison.
==============================================================================
*/
   int Column::compare (const Column & right, int start)
    {
      if (Length != right.Length) return 0;
      if (Zero && right.Zero) return 0;

      if (Sign && right.Sign)
       {
         if (Zero && !right.Zero) return 1;
         else if (!Zero && right.Zero) return -1;
         return -basicCompare(Digits + start, right.Digits + start, Length - start);
       }
      else if (!Sign && right.Sign) return -1;
      else if (Sign && !right.Sign) return 1;

      if (Zero && !right.Zero) return -1;
      else if (!Zero && right.Zero) return 1;
      return basicCompare(Digits + start, right.Digits + start, Length - start);
    }

/*
==============================================================================
   Function group: equivalence and relational operators
------------------------------------------------------------------------------
   NOTES:
      Returns 0 if the two columns are of different lengths.
      It does not even try a comparison.
==============================================================================
*/
   bool Column::operator == (const Column & right) { return (compare(right) == 0); }
   bool Column::operator != (const Column & right) { return (compare(right) != 0); }
   bool Column::operator > (const Column & right) { return (compare(right) > 0); }
   bool Column::operator >= (const Column & right) { return (compare(right) >= 0); }
   bool Column::operator < (const Column & right) { return (compare(right) < 0); }
   bool Column::operator <= (const Column & right) { return (compare(right) <= 0); }

/*
==============================================================================
   Function: isGreaterThan (used in divide like the two argument compare)
------------------------------------------------------------------------------
   NOTES:
      Returns 0 if the two columns are of different lengths.
      It does not even try a comparison.
==============================================================================
*/
   bool Column::isGreaterThan (const Column & right, int start)
    {
      if ((start > Length) || (start < 0)) return false;
      return (compare(right, start) > 0);
    }

/*
==============================================================================
   Function: isGreaterThanOrEqualTo (used in divide)
------------------------------------------------------------------------------
   NOTES:
      Returns 0 if the two columns are of different lengths.
      It does not even try a comparison.
==============================================================================
*/
   bool Column::isGreaterThanOrEqualTo (const Column & right, int start)
    {
      if ((start > Length) || (start < 0)) return false;
      return (compare(right, start) >= 0);
    }

/*
==============================================================================
   Function: left shift operator (achieves Column *= 10)
==============================================================================
*/
   Column & Column::operator <<= (int shift)
    {
      if (Zero) return *this;
      if (shift == 0) return *this;
      if (shift < 0) return *this >>= -shift;
      if (shift > Length)
       {
         Zero = true;
         delete [] Digits;
         Digits = NULL;
       }
      else
       {
         int i; //Are we wasing our time?
         for (i = shift; i < Length; i++) if (Digits[i]) break;
         if (i == Length) //Yes: number is zero
          {
            Zero = true;
            delete [] Digits;
            Digits = NULL;
          }
         else //No: actually move the data
          {
            memmove(Digits, Digits + shift, Length - shift);
            memset(Digits + Length - shift, 0, shift);
          }
       }

      return *this;
    } //operator <<=

/*
==============================================================================
   Function: right shift operator (achieves Column /= 10)
==============================================================================
*/
   Column & Column::operator >>= (int shift)
    {
      if (Zero) return *this;
      if (shift == 0) return *this;
      if (shift < 0) return *this <<= -shift;
      if (shift > Length)
       {
         Zero = true;
         delete [] Digits;
         Digits = NULL;
       }
      else
       {
         int i; //Are we wasing our time?
         for (i = 0; i < Length - shift; i++) if (Digits[i]) break;
         if (i == (Length - shift)) //Yes: number is zero
          {
            Zero = true;
            delete [] Digits;
            Digits = NULL;
          }
         else //No: actually move the data
          {
            memmove(Digits + shift, Digits, Length - shift);
            memset(Digits, 0, shift);
          }
       }

      return *this;
    } //operator >>=

/*
==============================================================================
   Function: digitAt
------------------------------------------------------------------------------
   NOTES:
      Wrapper in case Digits is NULL.
==============================================================================
*/
   int Column::digitAt (int index) const
    {
      if (Zero || (index < 0) || (index > Length)) return 0;
      return static_cast<int>(Digits[index]);
    }

/*
==============================================================================
   Function: setDigit
------------------------------------------------------------------------------
   NOTES:
      The only thing that could be improved is testing if setting a digit
      makes the column zero. This would considerably slow down the code
      for a case that never occurs in the Float class that this supports.
==============================================================================
*/
   int Column::setDigit (int index, int value)
    {
      if ((index < 0) || (index > Length) || (value < 0) || (value > 9)) return -1;
      if (Zero && (value == 0)) return 0;
      if (Zero)
       {
         Digits = new char [Length];
         memset(Digits, 0, Length);
         Zero = false;
       }
      return static_cast<int>((Digits[index] = static_cast<char>(value)));
    }

/*
==============================================================================
   Function: basicAddSub (file scope only)
------------------------------------------------------------------------------
   NOTES:
      sub controls whether it performs addition or subtraction.
      This function will be called for subtraction far more than for addition,
      as division uses more subtractions than anything uses additions.

      THIS FUNCTION WILL !DOMINATE! CPU TIME
==============================================================================
*/
   static void basicAddSub (const char * left, const char * right, char * dest, int n, bool sub)
    {
      char carry = 0;

      if (sub) //subtracting
       {
         for (int i = n - 1; i >= 0; i--)
          {
            dest[i] = left[i] - right[i] - carry;
            if (dest[i] < 0)
             {
               dest[i] += 10;
               carry = 1;
             }
            else carry = 0;
          }
       }
      else //adding
       {
         for (int i = n - 1; i >= 0; i--)
          {
            dest[i] = left[i] + right[i] + carry;
            if (dest[i] > 9)
             {
               dest[i] -= 10;
               carry = 1;
             }
            else carry = 0;
          }
       }
    } //basicAddSub

/*
==============================================================================
   Function: addSubHelper
------------------------------------------------------------------------------
   NOTES:
      This does all of the complex logic involved in doing a signed-magnitude
      addition/subtraction.
==============================================================================
*/
   void Column::addSubHelper (bool lSign, char * lDigits, bool rSign, char * rDigits,
      Column & dest, int n)
    {
      int res;
      char * temp1 = new char [n + 1];
      char * temp2 = new char [n + 1];

      temp1[0] = 0;
      temp2[0] = 0;
      memcpy(temp1 + 1, lDigits, n);
      memcpy(temp2 + 1, rDigits, n);

      if (lSign && rSign)
       {
         dest.Sign = true;
         basicAddSub(temp1, temp2, temp1, n, false);
         if (temp1[0] == 0) memcpy(dest.Digits, temp1 + 1, n);
         else memcpy(dest.Digits, temp1, n);
         dest.Overflow = (temp1[0] != 0);
       }
      else if (lSign && !rSign)
       {
         res = basicCompare(lDigits, rDigits, n);
         if (res > 0)
          {
            dest.Sign = true;
            basicAddSub(temp1, temp2, temp1, n, true);
            memcpy(dest.Digits, temp1 + 1, n);
          }
         else if (res < 0)
          {
            dest.Sign = false;
            basicAddSub(temp2, temp1, temp1, n, true);
            memcpy(dest.Digits, temp1 + 1, n);
          }
         else
          {
            dest.Sign = false;
            delete [] dest.Digits;
            dest.Digits = NULL;
            dest.Zero = true;
          }
       }
      else if (!lSign && rSign)
       {
         res = basicCompare(lDigits, rDigits, n);
         if (res > 0)
          {
            dest.Sign = false;
            basicAddSub(temp1, temp2, temp1, n, true);
            memcpy(dest.Digits, temp1 + 1, n);
          }
         else if (res < 0)
          {
            dest.Sign = true;
            basicAddSub(temp2, temp1, temp1, n, true);
            memcpy(dest.Digits, temp1 + 1, n);
          }
         else
          {
            dest.Sign = false;
            delete [] dest.Digits;
            dest.Digits = NULL;
            dest.Zero = true;
          }
       }
      else
       {
         dest.Sign = false;
         basicAddSub(temp1, temp2, temp1, n, false);
         if (temp1[0] == 0) memcpy(dest.Digits, temp1 + 1, n);
         else memcpy(dest.Digits, temp1, n);
         dest.Overflow = (temp1[0] != 0);
       }

      delete [] temp1;
      delete [] temp2;
    } //addSubHelper

/*
==============================================================================
   Function: add and assign operator
------------------------------------------------------------------------------
   NOTES:
      This function will only work on Columns of the same size.
==============================================================================
*/
   Column & Column::operator += (const Column & right)
    {
      if ((Length != right.Length) || right.Zero) return *this; //do nothing
      if (Zero)
       {
         Digits = new char [right.Length];
         memcpy(Digits, right.Digits, right.Length);
         Sign = right.Sign;
         Zero = false;
         Overflow = false;
       }
      else addSubHelper (Sign, Digits, right.Sign, right.Digits, *this, Length);
      return *this;
    }

/*
==============================================================================
   Function: subtract and assign operator
------------------------------------------------------------------------------
   NOTES:
      This function will only work on Columns of the same size.
==============================================================================
*/
   Column & Column::operator -= (const Column & right)
    {
      if ((Length != right.Length) || right.Zero) return *this; //do nothing
      if (Zero)
       {
         Digits = new char [right.Length];
         memcpy(Digits, right.Digits, right.Length);
         Sign = !right.Sign;
         Zero = false;
         Overflow = false;
       }
      else addSubHelper (Sign, Digits, !right.Sign, right.Digits, *this, Length);
      return *this;
    }

/*
==============================================================================
   Function: addition operator
------------------------------------------------------------------------------
   NOTES:
      This function will only work on Columns of the same size.
==============================================================================
*/
   Column Column::operator + (const Column & right) const
    {
      Column tempColumn (*this);
      return tempColumn += right;
    }

/*
==============================================================================
   Function: subtraction operator
------------------------------------------------------------------------------
   NOTES:
      This function will only work on Columns of the same size.
==============================================================================
*/
   Column Column::operator - (const Column & right) const
    {
      Column tempColumn (*this);
      return tempColumn -= right;
    }

/*
==============================================================================
   Function: assignment operator
==============================================================================
*/
   Column & Column::operator = (const Column & right)
    {
         //Nowhere in my code does this case ever actually pop up.
         //However, I handle it for sake of completion.
      if (&right == this) return *this;
      if (right.Zero) //Then Digits is NULL and we don't need to allocate it.
       {
         if (Digits) delete [] Digits;
         Digits = NULL;
         Sign = right.Sign;
         Length = right.Length;
         Zero = right.Zero;
         Overflow = right.Overflow;
       }
      else //Allocate Digits and copy the payload
       {
         if (Digits && (right.Length != Length))
          {
            delete [] Digits;
            Digits = new char [right.Length];
          }
         else if (!Digits) Digits = new char [right.Length];
         memcpy(Digits, right.Digits, right.Length);
         Sign = right.Sign;
         Length = right.Length;
         Zero = right.Zero;
         Overflow = right.Overflow;
       }
      return *this;
    } //operator = (Column)

/*
==============================================================================
   Function: left justified assignment operator
------------------------------------------------------------------------------
   NOTES:
      This function originally had no purpose other than symmetry with &=.
      Float got |= from this, and it uses this.
==============================================================================
*/
   Column & Column::operator |= (const Column & right)
    {
      if (&right == this) return *this;
      Sign = right.Sign;
      if (right.Zero && Zero) return *this;
      if (right.Zero)
       {
         int i;
         if (right.Length < Length)
          {
            for (i = 0; i < right.Length; i++) Digits[i] = 0;
            for (i = 0; i < Length; i++) if (Digits[i]) break;
            if (i == Length) //We are now zero, too
             {
               Zero = true;
               delete [] Digits;
               Digits = NULL;
             }
          }
         else
          {
            Zero = true;
            delete [] Digits;
            Digits = NULL;
          }
       }
      else
       {
         if (Zero)
          {
            Digits = new char [Length];
            Zero = false;
          }
         int useLength;
         if (right.Length > Length) useLength = Length; else useLength = right.Length;
         memcpy(Digits, right.Digits, useLength);
         if (Length > right.Length) memset(Digits + right.Length, 0, Length - right.Length);
       }
      return *this;
    } //operator |=

/*
==============================================================================
   Function: right justified assignment statement
------------------------------------------------------------------------------
   NOTES:
      Division uses this to assign the divisor to a variable representing
      the stopping criteria.

      This function will only work if the length of the argument being
      assigned is less than the length of the Column it is assigned to.
==============================================================================
*/
      //This is going to be used for division
   Column & Column::operator &= (const Column & right)
    {
      if (&right == this) return *this;
      Sign = right.Sign;
      if (right.Length > Length) return *this;
      if (right.Zero && Zero) return *this;
      if (right.Zero)
       {
         int i;
         for (i = Length - right.Length; i < Length; i++) Digits[i] = 0;
         for (i = 0; i < Length; i++) if (Digits[i]) break;
         if (i == Length) //We are now zero, too
          {
            Zero = true;
            delete [] Digits;
            Digits = NULL;
          }
       }
      else
       {
         if (Zero)
          {
            Digits = new char [Length];
            Zero = false;
          }
         memcpy(Digits + Length - right.Length, right.Digits, right.Length);
         if (Length > right.Length) memset(Digits, 0, Length - right.Length);
       }
      return *this;
    } //operator &=

/*
==============================================================================
   Function: zero-padded left-justified assignment statement
------------------------------------------------------------------------------
   NOTES:
      Division will use this to assign the divisor and dividend to temporary
      variables.

      This function will only work if the length of the argument being
      assigned is less than the length of the Column it is assigned to.
==============================================================================
*/
   Column & Column::operator *= (const Column & right)
    {
      if (&right == this) return *this;
      Sign = right.Sign;
      if (right.Length >= Length) return *this;
      if (right.Zero && Zero) return *this;
      if (right.Zero)
       {
         int i;
         for (i = 0; i < (right.Length + 1); i++) Digits[i] = 0;
         for (i = 0; i < Length; i++) if (Digits[i]) break;
         if (i == Length) //We are now zero, too
          {
            Zero = true;
            delete [] Digits;
            Digits = NULL;
          }
       }
      else
       {
         if (Zero)
          {
            Digits = new char [Length];
            Zero = false;
          }
         Digits[0] = 0;
         memcpy(Digits + 1, right.Digits, right.Length);
         if (Length > right.Length)
            memset(Digits + right.Length + 1, 0, Length - right.Length - 1);
       }
      return *this;
    } //operator *=

/*
==============================================================================
   Function: increment operator
------------------------------------------------------------------------------
   NOTES:
      Rounding will use this.

      Increments away from zero.
==============================================================================
*/
   Column Column::operator++ (int)
    {
      if (Zero)
       {
         Digits = new char [Length];
         memset(Digits, 0, Length - 1);
         Digits[Length - 1] = 1;
         Zero = false;
       }
      else
       {
         char carry = 1;
         for (int i = Length - 1; (i >= 0) && (carry != 0); i--)
          {
            Digits[i] = Digits[i] + carry;
            if (Digits[i] > 9)
             {
               Digits[i] -= 10;
               carry = 1;
             }
            else carry = 0;
         }
        if (carry != 0) //we ran off of the end
         {
           Digits[0] = 1;
           Overflow = 1;
         }

        /*
           That deserves some explanation:
           carry == 1 at the end iff the Column was all 9's,
           at which point, the entire number will be 0's,
           so we just stick in the 1.

           Overflow will NEVER occur UNLESS the column is all 9's
           and this operation will NEVER leave the column being zero.
        */
       }
      return Column(*this);
    }

/*
==============================================================================
   Function: incrementAt
------------------------------------------------------------------------------
   INPUT: digit index to increment at
   NOTES:
      Increments away from zero.

      Rounding and divide will use this.
==============================================================================
*/
   void Column::incrementAt (int digit)
    {
      if ((digit < 0) || (digit > Length)) return;
      if (Zero)
       {
         Digits = new char [Length];
         memset(Digits, 0, Length - 1);
         Digits[digit] = 1;
         Zero = false;
       }
      else
       {
         char carry = 1;
         for (int i = digit; (i >= 0) && (carry != 0); i--)
          {
            Digits[i] = Digits[i] + carry;
            if (Digits[i] > 9)
             {
               Digits[i] -= 10;
               carry = 1;
             }
            else carry = 0;
         }
        if (carry != 0) //we ran off of the end
         {
           *this >>= 1;
           Digits[0] = 1;
           Overflow = 1;
         }
       }
    } //incrementAt

 } //namespace Col
