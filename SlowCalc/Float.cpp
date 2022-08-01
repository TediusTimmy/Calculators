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
   File: Float.cpp
      Float builds a Column into a working floating point number, not just a
      lowsy big-integer. (Column would make a terrible BigInt)

      MODE_2 two originally had a precision of 100 digits, but 70 fits onto
      one line of the terminal.
*/

//HEADER:
// sstream and iomanip allow us to do easy conversion to/from macine numbers
// Float defines the class that these functions operate on.
#include <sstream>
#include <iomanip>
#include "Float.hpp"

/* The fancy logic for the two other modes of calculators. */
#ifndef MODE_1
 #ifndef MODE_2
  #define MODE_0
 #endif /* NOT MODE_2 */
#endif /* NOT MODE_1 */

/*
   Implements:
      All class functions
      All operators except ^ and %
*/

namespace DecFloat
 {

      //maxLength is a suggestion, not an absolute.
      //It needs to be for functions to return good results
      //for arguments of length maxLength.
#ifdef MODE_0
   int Float::maxLength = 256;
#endif
#ifdef MODE_1
   int Float::maxLength = 50;
#endif
#ifdef MODE_2
   int Float::maxLength = 70;
#endif

      //Finally! Defined constants...
      //These are implementation specific:
      //I could have used a Column for Exponent, too,
      //for an ungodly limit in exponents.
      //But, considering that the current limit allows expressing
      //the volume of the visable universe in cubic Plank lengths,
      //the overhead of using a Column unreasonable.
      //These values allow multiplication and division without
      //overflowing a 32 bit int.
#ifdef MODE_0
   const int maxExponent =  1000000000;
   const int minExponent = -1000000000;
#endif
#ifdef MODE_1
   const int maxExponent =  99;
   const int minExponent = -99;
#endif
#ifdef MODE_2
   const int maxExponent =  999;
   const int minExponent = -999;
#endif

      //These specify the number of extra digits to compute an expression
      //to for series and derived calculations. Ideally, we would just use
      //some function of length to determine how many digits of precision
      //were needed to get length + 1 accurate digits, however I know of no
      //such function.
   int Float::seriesExtra = 8;
   int Float::derivedExtra = 4;


/*
==============================================================================
   Function: getMaxLength
==============================================================================
*/
   int Float::getMaxLength (void) { return maxLength; }

/*
==============================================================================
   Function: setMaxLength
==============================================================================
*/
   int Float::setMaxLength (int newLength)
    {
      if (newLength < Col::hardMinLength) newLength = Col::hardMinLength;
      return (maxLength = newLength);
    }

/*
==============================================================================
   Function: getMinLength (Column function wrapper)
==============================================================================
*/
   int Float::getMinLength (void) { return Column::getMinLength(); }

/*
==============================================================================
   Function: setMinLength (Column function wrapper)
==============================================================================
*/
   int Float::setMinLength (int newLength) { return Column::setMinLength(newLength); }

/*
==============================================================================
   Function: getSeriesExtra
==============================================================================
*/
   int Float::getSeriesExtra (void) { return seriesExtra; }

/*
==============================================================================
   Function: setSeriesExtra
------------------------------------------------------------------------------
   NOTES:
      Poor error checking: don't shoot yourself in the foot!
==============================================================================
*/
   int Float::setSeriesExtra (int newLength)
      { if (newLength < 0) newLength = 0; return (seriesExtra = newLength); }

/*
==============================================================================
   Function: getDerivedExtra
==============================================================================
*/
   int Float::getDerivedExtra (void) { return derivedExtra; }

/*
==============================================================================
   Function: setDerivedExtra
------------------------------------------------------------------------------
   NOTES:
      Poor error checking: don't shoot yourself in the foot!
==============================================================================
*/
   int Float::setDerivedExtra (int newLength)
      { if (newLength < 0) newLength = 0; return (derivedExtra = newLength); }

/*
==============================================================================
   Function group: member variable accessors, state access
==============================================================================
*/
   int Float::exponent (void) const { return Exponent; }
   int Float::length (void) const { return Significand.length(); }
   bool Float::sign (void) const { return Significand.sign(); }
   bool Float::isZero (void) const
    {
      if (Infinity || NaN) return false;
      return Significand.isZero();
    }
   bool Float::isNaN (void) const { return NaN; }
   bool Float::isInfinity (void) const { return Infinity; }
   bool Float::locked (void) const { return Locked; }
   bool Float::isNegative (void) const
    {
      if (NaN || Significand.isZero()) return false;
      return Significand.sign();
    }

/*
==============================================================================
   Function: getDigits
------------------------------------------------------------------------------
   OUTPUT: a string of the digits, even if the number is zero
==============================================================================
*/
   std::string Float::getDigits (void) const
    {
      //If infinity or NaN, return an empty string.
      if (Infinity || NaN) return std::string ("");
      //If zero, return a string of 0s Length length
      std::string result;
      if (Significand.isZero())
       {
         for (int i = 0; i < Significand.length(); i++) result += '0';
       }
      else //return a string of ASCII digits
       {
         //gcc doesn't like me defining two loop variables of different types.
         const char * p = Significand.digits();
         for (int i = 0; i < Significand.length(); i++)
            result += static_cast<char>(p[i] + '0');
       }
      return result;
    }

/*
==============================================================================
   Function: toString
------------------------------------------------------------------------------
   OUTPUT: a string of the Float in scientific notation
==============================================================================
*/
   std::string Float::toString (void) const
    {
      if (NaN) return std::string ("NaN");
      if (Infinity)
       {
         if (Significand.sign()) return std::string ("-Inf");
         else return std::string ("Inf");
       }
      std::string result ("");
      if (Significand.sign()) result += "-";
      result += static_cast<char>(Significand.digitAt(0) + '0');
      result += '.';
      for (int i = 1; i < Significand.length(); i++)
         result += static_cast<char>(Significand.digitAt(i) + '0');
      result += "E";
      std::ostringstream temp;
      temp << Exponent;
      result += temp.str();
      return result;
    }

/*
==============================================================================
   Function: enforceLength
------------------------------------------------------------------------------
   NOTES:
      maxLength needs to be a suggestion for several functions to work
      correctly, so there is only need to enforce it on input.
==============================================================================
*/
   int Float::enforceLength (void)
    {
      if (Significand.length() > maxLength)
         return Significand.setLength(maxLength);
      return Significand.length();
    }

/*
==============================================================================
   Function group: basic length manipulations, wrappers for Column functions
==============================================================================
*/
   int Float::setLength (int newLength) { return Significand.setLength(newLength); }
   int Float::lengthen (int delta) { return Significand.lengthen(delta); }
   int Float::truncate (int delta) { return Significand.truncate(delta); }

/*
==============================================================================
   Function: basic state manipulations, wrappers for Column functions
==============================================================================
*/
   Float & Float::negate (void) { Significand.negate(); return *this; }
   Float & Float::abs (void) { Significand.absoluteValue(); return *this; }

/*
==============================================================================
   Function: roundAt
------------------------------------------------------------------------------
   NOTES:
      Rounds AT the argument index of the digit to the right, so the next
      digit to the right will be changed.
==============================================================================
*/
   void Float::roundAt (int digit)
    {
      if ((digit < 1) || (digit >= Significand.length())) return;
      if (Infinity || NaN || Significand.isZero()) return;
      if (Mode.decideRound(Significand.sign(),
         Significand.digitAt(digit - 1), Significand.digitAt(digit)))
       {
         Significand.incrementAt(digit - 1);
         Significand.setToZero(digit, Significand.length());
         if (Significand.overflow())
          {
            Exponent++;
            Significand.clearOverflow();
            if ((Exponent > maxExponent) && Locked)
             {
               Infinity = true;
               Significand.setToZero();
             }
          }
       }
      else
       {
         Significand.setToZero(digit, Significand.length());
       }
    }

/*
==============================================================================
   Function: roundPlace
------------------------------------------------------------------------------
   INPUT: the place to round AT expressed as a power of ten
   NOTES:
      Rounds AT the argument place.
   EXAMPLE:
      "1.123e0".roundPlace(-2) = "1.100"
==============================================================================
*/
   void Float::roundPlace (int place)
    {
      int digit = Exponent - place;
      if ((digit < 1) || (digit > Significand.length())) return;
      if (Infinity || NaN || Significand.isZero()) return;
      if (Mode.decideRound(Significand.sign(),
         Significand.digitAt(digit - 1), Significand.digitAt(digit)))
       {
         Significand.incrementAt(digit - 1);
         Significand.setToZero(digit, Significand.length());
         if (Significand.overflow())
          {
            Exponent++;
            Significand.clearOverflow();
            if ((Exponent > maxExponent) && Locked)
             {
               Infinity = true;
               Significand.setToZero();
               Exponent = 0;
             }
          }
       }
      else
       {
         Significand.setToZero(digit, Significand.length());
       }
    }

/*
==============================================================================
   Function: make integer
==============================================================================
*/
   void Float::Int (void)
    {
      if (Exponent < 0) Significand.setToZero();
      else Significand.setToZero(Exponent + 1, Significand.length());
    }

/*
==============================================================================
   Function: truncate integer portion
==============================================================================
*/
   void Float::Frac (void)
    {
      if (Exponent < 0) return;
      if (Exponent >= Significand.length())
       {
         Significand.setToZero();
         Exponent = 0;
       }
      else
       {
         Significand.setToZero(0, Exponent + 1);
         if (!Significand.isZero())
          {
            Significand <<= (Exponent + 1);
            Exponent = -1;
            int addedShift;
            for (addedShift = 0; Significand.digitAt(addedShift) == 0;
               addedShift++) ;
            Exponent -= addedShift;
            Significand <<= addedShift;

            if ((Exponent < minExponent) && Locked)
             {
               Significand.setToZero();
               Exponent = 0;
             }
          }
         else
          {
            Exponent = 0;
          }
       }
    }

/*
==============================================================================
   Function: Normalize
------------------------------------------------------------------------------
   NOTES:
      This function should only need to be used on input.
==============================================================================
*/
   void Float::Normalize (void)
    {
      if (Infinity || NaN) return;
      if (Significand.isZero())
       {
         Exponent = 0;
       }
      else
       {
         int shift;
         for (shift = 0; Significand.digitAt(shift) == 0; shift++) ;
         Exponent -= shift;
         Significand <<= shift;

         if ((Exponent < minExponent) && Locked)
          {
            Significand.setToZero();
            Exponent = 0;
          }
         if ((Exponent > maxExponent) && Locked)
          {
            Infinity = true;
            Significand.setToZero();
            Exponent = 0;
          }
       }
    }

/*
==============================================================================
   Function: assignment operator
==============================================================================
*/
   Float & Float::operator = (const Float & right)
    {
      if (&right == this) return (*this);
      Significand = right.Significand;
      Exponent = right.Exponent;
      Infinity = right.Infinity;
      NaN = right.NaN;
      Mode = right.Mode;
      Locked = right.Locked;
      return (*this);
    }

/*
==============================================================================
   Function: assign without length/rounding mode/locking change
------------------------------------------------------------------------------
   NOTES:
      This will make functions easier to implement.
==============================================================================
*/
   Float & Float::operator |= (const Float & right)
    {
      if (&right == this) return (*this);
      Exponent = right.Exponent;
      Infinity = right.Infinity;
      NaN = right.NaN;
      Significand |= right.Significand;
      if (right.length() > length())
       {
         if (Mode.decideRound(Significand.sign(),
            right.Significand.digitAt(length() - 1),
            right.Significand.digitAt(length())))
          {
            Significand++;
            if (Significand.overflow())
             {
               Exponent++;
               Significand.clearOverflow();
               if (Exponent > maxExponent)
                {
                  Infinity = true;
                  Significand.setToZero();
                  Exponent = 0;
                }
             }
          }
       }
      return (*this);
    }

/*
==============================================================================
   Function: no argument constructor
==============================================================================
*/
   Float::Float () : Exponent(0), Infinity(false), NaN(false), Locked(true)
    {
      //Significand will take care of its own thing.
      //So, too, will Mode.
    }

/*
==============================================================================
   Function: copy constructor
==============================================================================
*/
   Float::Float (const Float & arg)
    {
      Significand = arg.Significand;
      Exponent = arg.Exponent;
      Infinity = arg.Infinity;
      NaN = arg.NaN;
      Mode = arg.Mode;
      Locked = arg.Locked;
    }

/*
==============================================================================
   Function: constructor from string
==============================================================================
*/
   Float::Float (const std::string & arg)
    {
      Locked = true;
      if ((arg == "Inf") || (arg == "+Inf") || (arg == "INF") || (arg == "+INF"))
       {
         Exponent = 0;
         Infinity = true;
         NaN = false;
       }
      else if ((arg == "-Inf") || (arg == "-INF"))
       {
         Exponent = 0;
         Infinity = true;
         NaN = false;
         Significand.negate();
       }
      else if ((arg == "NAN") || (arg == "NaN"))
       {
         Exponent = 0;
         Infinity = false;
         NaN = true;
       }
      else if (arg.length() > 0)
       {
         std::string newArg ("");
         unsigned int i = 0;
         int newExponent = -1;
         bool computeExponent = true;

         if (arg[i] == '-') { newArg = "-"; i++; }
         else if (arg[i] == '+') i++;

         for (;(i < arg.length()) && (arg[i] != 'e') && (arg[i] != 'E'); i++)
          {
            if (arg[i] != '.') newArg += arg[i];
            else computeExponent = false;
            if (computeExponent) newExponent++;
          }
         Significand = newArg;

         if ((i < arg.length()) && ((arg[i] != 'e') || (arg[i] != 'E')))
          {
            std::istringstream temp (arg.c_str() + i + 1);
            temp >> Exponent;
            Exponent += newExponent;
          }
         else Exponent = newExponent;

         Infinity = false;
         NaN = false;
       }
      else
       {
         Exponent = 0;
         Infinity = false;
         NaN = false;
       }

      if (Exponent > maxExponent)
       {
         Infinity = true;
         Significand.setToZero();
       }
      if (Exponent < minExponent)
       {
         Significand.setToZero();
       }
    } //constructor (string)

/*
==============================================================================
   Function: partial comparison
------------------------------------------------------------------------------
   NOTES:
      This does not handle Infinity, NaN, or Zero (-0 < 0),
      that is the job of the relational operators.
==============================================================================
*/
   int Float::compare (const Float & right) const
    {
      if (&right == this) return 0;
      if (Exponent == right.Exponent)
       {
         Float temp1 (*this);
         Float temp2 (right);
         temp1 || temp2;
         return temp1.Significand.compare(temp2.Significand);
       }
      else if (Significand.sign() && right.Significand.sign())
       {
         if (Exponent > right.Exponent) return -1;
         return 1;
       }
      else if (!Significand.sign() && right.Significand.sign()) return 1;
      else if (Significand.sign() && !right.Significand.sign()) return -1;
      else
       {
         if (Exponent > right.Exponent) return 1;
         return -1;
       }
    } //compare

/*
==============================================================================
   Function group: equality and relational operators
------------------------------------------------------------------------------
   NOTES:
      If NaN is a value, the result is always false, except for
         NaN != NaN, which is always true.
      Inf == Inf is false, as Inf == Inf => Inf - Inf = 0,
         comparisons should do what you expect but Inf >= Inf is also false,
         as is -Inf <= -Inf.

         Another example: |Z| = Inf. |R| = Inf. |Z| != |R|.
==============================================================================
*/
   bool operator == (const Float & left, const Float & right)
    {
      if (left.isNaN() || right.isNaN()) return false;
      if (left.isInfinity() || right.isInfinity()) return false;
      if (left.isZero() && right.isZero()) return true;
      return (left.compare(right) == 0);
    }

   bool operator != (const Float & left, const Float & right)
    {
      if (left.isNaN() && right.isNaN()) return true;
      if (left.isNaN() || right.isNaN()) return false;
      if (left.isInfinity() || right.isInfinity()) return true;
      if (left.isZero() && right.isZero()) return false;
      return (left.compare(right) != 0);
    }

   bool operator > (const Float & left, const Float & right)
    {
      if (left.isNaN() || right.isNaN()) return false;
      if (left.isInfinity() || right.isInfinity())
       {
         if (left.isInfinity() && right.isInfinity())
          {
            if (left.sign() == right.sign()) return false;
            else if (left.sign()) return false;
            else return true;
          }
         else if (left.isInfinity()) return !left.sign();
         else return !right.sign();
       }
      if (left.isZero() && right.isZero()) return false;
      return (left.compare(right) > 0);
    }

   bool operator >= (const Float & left, const Float & right)
    {
      if (left.isNaN() || right.isNaN()) return false;
      if (left.isInfinity() || right.isInfinity())
       {
         if (left.isInfinity() && right.isInfinity())
          {
            if (left.sign() == right.sign()) return false;
            else if (left.sign()) return false;
            else return true;
          }
         else if (left.isInfinity()) return !left.sign();
         else return !right.sign();
       }
      if (left.isZero() && right.isZero()) return true;
      return (left.compare(right) >= 0);
    }

   bool operator < (const Float & left, const Float & right)
    {
      if (left.isNaN() || right.isNaN()) return false;
      if (left.isInfinity() || right.isInfinity())
       {
         if (left.isInfinity() && right.isInfinity())
          {
            if (left.sign() == right.sign()) return false;
            else if (left.sign()) return true;
            else return false;
          }
         else if (left.isInfinity()) return left.sign();
         else return right.sign();
       }
      if (left.isZero() && right.isZero()) return false;
      return (left.compare(right) < 0);
    }

   bool operator <= (const Float & left, const Float & right)
    {
      if (left.isNaN() || right.isNaN()) return false;
      if (left.isInfinity() || right.isInfinity())
       {
         if (left.isInfinity() && right.isInfinity())
          {
            if (left.sign() == right.sign()) return false;
            else if (left.sign()) return true;
            else return false;
          }
         else if (left.isInfinity()) return left.sign();
         else return right.sign();
       }
      if (left.isZero() && right.isZero()) return true;
      return (left.compare(right) <= 0);
    }

/*
==============================================================================
   Function: change operator
------------------------------------------------------------------------------
   NOTES:
      This handy operator will be the limiting test of most of the functions.
      Putting it into other terms, this returns true in such a manner that
      the absolute relative approximate error of a series has dropped to zero
      once it becomes false. It is not as good as calculating the percent
      error, but it is simpler in practice.
==============================================================================
*/
   bool operator && (const Float & left, const Float & right)
    {
      int useLength, diff;
      if (left.length() > right.length()) useLength = left.length();
      else useLength = right.length();

      if (left.isZero() || left.isInfinity() || left.isNaN() ||
          right.isZero() || right.isInfinity() || right.isNaN()) return false;

      diff = left.exponent() - right.exponent();
      if (diff < 0) diff = -diff;

      //We add one here to account for the extra digit in add that rounds.
      if (diff > (useLength + 1)) return false;
      return true;
    }

/*
==============================================================================
   Function: equalize length operator
==============================================================================
*/
   void operator || (Float & left, Float & right)
    {
      if (left.length() == right.length()) return;
      if (left.length() > right.length()) right.setLength(left.length());
      else left.setLength(right.length());
    }

/*
==============================================================================
   Function: unary negate operator
==============================================================================
*/
   Float operator - (const Float & arg)
    {
      Float ret (arg);
      ret.negate();
      return ret;
    }

/*
==============================================================================
   Function: add and assign operator
==============================================================================
*/
   Float & operator += (Float & left, const Float & right)
    {
      left = left + right;
      return left;
    }

/*
==============================================================================
   Function: subtract and assign operator
==============================================================================
*/
   Float & operator -= (Float & left, const Float & right)
    {
      left = left - right;
      return left;
    }

/*
==============================================================================
   Function: multiply and assign operator
==============================================================================
*/
   Float & operator *= (Float & left, const Float & right)
    {
      left = left * right;
      return left;
    }

/*
==============================================================================
   Function: divide and assign operator
==============================================================================
*/
   Float & operator /= (Float & left, const Float & right)
    {
      left = left / right;
      return left;
    }


      // NaNs: Inf-Inf 0*Inf 0/0 Inf/Inf 0^0 Inf^0 1^Inf
   const Float fNaN (std::string("NaN"));
   const Float fInf (std::string("Inf"));


/*
==============================================================================
   Function: addition operator
------------------------------------------------------------------------------
   NOTES:
      Handling of 0 + 0:
         +0 + +0 = +0
         +0 + -0 = +0
         -0 + +0 = +0
         -0 + -0 = -0
==============================================================================
*/
   Float operator + (const Float & left, const Float & right)
    {
      int shift;
         //Copy the args and make them the same size
      Float Temp1 (left), Temp2 (right);
      Temp1 || Temp2;
      Temp2.Mode = Temp1.Mode;
      Float Temp3 (Temp1);

         //First, handle NaNs, as they have the highest precedence.
         //The return is ugly, but it makes behavior consistant
      if (left.isNaN() || right.isNaN()) return (Temp1 |= fNaN);

         //Next, infinities.
      if (left.isInfinity() && right.isInfinity())
       {
            //Inf - Inf = NaN
         if (left.sign() != right.sign()) return (Temp1 |= fNaN);
         if (left.sign()) return (Temp1 |= -fInf);
         return (Temp1 |= fInf);
       }

            //Anything else with infinity is infinity
      if (left.isInfinity())
       {
         if (left.sign()) return (Temp1 |= -fInf);
         return (Temp1 |= fInf);
       }
      if (right.isInfinity())
       {
         if (right.sign()) return (Temp1 |= -fInf);
         return (Temp1 |= fInf);
       }

         //Finally, zeros.
      if (left.isZero() && right.isZero() && (left.sign() != right.sign()))
       {
         Temp1.abs();
         return Temp1;
       }
      if (right.isZero()) return Temp1;
      if (left.isZero()) return Temp2;

         //Add one digit of precision to handle rounding correctly
      Temp1.lengthen(1);
      Temp2.lengthen(1);
      Temp3.lengthen(1);

         //Decide on the result's exponent, and shift the argument with
         //the lesser exponent
      if (Temp1.Exponent > Temp2.Exponent)
       {
         shift = Temp1.Exponent - Temp2.Exponent;
         Temp3.Exponent = Temp1.Exponent;
         Temp2.Significand >>= shift;
       }
      else if (Temp1.Exponent < Temp2.Exponent)
       {
         shift = Temp2.Exponent - Temp1.Exponent;
         Temp3.Exponent = Temp2.Exponent;
         Temp1.Significand >>= shift;
       }
      else Temp3.Exponent = Temp1.Exponent;

         //Do the addition
      Temp3.Significand = Temp1.Significand + Temp2.Significand;

         //Handle overflow
      if (Temp3.Significand.overflow())
       {
         Temp3.Exponent++;
         Temp3.Significand.clearOverflow();
       }
         //If not zero, check and normalize number
         //If there was an overflow, normalization isn't required
      else if (!Temp3.Significand.isZero())
       {
         for (shift = 0; shift < Temp3.length(); shift++)
            if (Temp3.Significand.digitAt(shift) != 0) break;
         if (shift != 0)
          {
            Temp3.Significand <<= shift;
            Temp3.Exponent -= shift;
          }
       }

         //Now round
      if (Temp1.Mode.decideRound(Temp3.Significand.sign(),
         Temp3.Significand.digitAt(Temp3.length() - 2),
         Temp3.Significand.digitAt(Temp3.length() - 1)))
       {
         Temp3.truncate(1);
         Temp3.Significand++;
         if (Temp3.Significand.overflow())
          {
            Temp3.Exponent++;
            Temp3.Significand.clearOverflow();
          }
       }
      else Temp3.truncate(1);

         //Finally, check for exponent overflow
      if ((Temp3.Exponent > maxExponent) && Temp3.Locked)
       {
         Temp3.Infinity = true;
         Temp3.Significand.setToZero();
       }
      if ((Temp3.Exponent < minExponent) && Temp3.Locked)
       {
         Temp3.Significand.setToZero();
       }

      return Temp3;
    }

/*
==============================================================================
   Function: subtraction operator
------------------------------------------------------------------------------
   NOTES:
      Handling of 0 - 0:
         +0 - +0 = +0
         +0 - -0 = +0
         -0 - +0 = -0
         -0 - -0 = +0
==============================================================================
*/
   Float operator - (const Float & left, const Float & right)
    {
      int shift;
      Float Temp1 (left), Temp2 (right);
      Temp1 || Temp2;
      Temp2.Mode = Temp1.Mode;
      Float Temp3 (Temp1);

      if (left.isNaN() || right.isNaN()) return (Temp1 |= fNaN);

      if (left.isInfinity() && right.isInfinity())
       {
         if (left.sign() == right.sign()) return (Temp1 |= fNaN);
         if (left.sign()) return (Temp1 |= -fInf);
         return (Temp1 |= fInf);
       }

      if (left.isInfinity())
       {
         if (left.sign()) return (Temp1 |= -fInf);
         return (Temp1 |= fInf);
       }
      if (right.isInfinity())
       {
         if (right.sign()) return (Temp1 |= fInf);
         return (Temp1 |= -fInf);
       }

      if (left.isZero() && right.isZero() && (left.sign() == right.sign()))
       {
         Temp1.abs();
         return Temp1;
       }
      if (right.isZero()) return Temp1;
      if (left.isZero()) return -Temp2;

      Temp1.lengthen(1);
      Temp2.lengthen(1);
      Temp3.lengthen(1);

      if (Temp1.Exponent > Temp2.Exponent)
       {
         shift = Temp1.Exponent - Temp2.Exponent;
         Temp3.Exponent = Temp1.Exponent;
         Temp2.Significand >>= shift;
       }
      else if (Temp1.Exponent < Temp2.Exponent)
       {
         shift = Temp2.Exponent - Temp1.Exponent;
         Temp3.Exponent = Temp2.Exponent;
         Temp1.Significand >>= shift;
       }
      else Temp3.Exponent = Temp1.Exponent;

      Temp3.Significand = Temp1.Significand - Temp2.Significand;

      if (Temp3.Significand.overflow())
       {
         Temp3.Exponent++;
         Temp3.Significand.clearOverflow();
       }
      else if (!Temp3.Significand.isZero())
       {
         for (shift = 0; shift < Temp3.length(); shift++)
            if (Temp3.Significand.digitAt(shift) != 0) break;
         if (shift != 0)
          {
            Temp3.Significand <<= shift;
            Temp3.Exponent -= shift;
          }
       }

      if (Temp1.Mode.decideRound(Temp3.Significand.sign(),
         Temp3.Significand.digitAt(Temp3.length() - 2),
         Temp3.Significand.digitAt(Temp3.length() - 1)))
       {
         Temp3.truncate(1);
         Temp3.Significand++;
         if (Temp3.Significand.overflow())
          {
            Temp3.Exponent++;
            Temp3.Significand.clearOverflow();
          }
       }
      else Temp3.truncate(1);

      if ((Temp3.Exponent > maxExponent) && Temp3.Locked)
       {
         Temp3.Infinity = true;
         Temp3.Significand.setToZero();
       }
      if ((Temp3.Exponent < minExponent) && Temp3.Locked)
       {
         Temp3.Significand.setToZero();
       }

      return Temp3;
    }

/*
==============================================================================
   Function: multiply operator
==============================================================================
*/
   Float operator * (const Float & left, const Float & right)
    {
      /*
         So: x * y, if x and y are one digit numbers, = wz
            multtens is the w
            multones is the z
         These tables contain the same data as:
            multones -> (x * y) % 10
            multtens -> (x * y) / 10
         These were chosen because it was decided that the
         latency from memory access would be less than the time
         needed to do the division.
      */
      static const char multones [10][10] =
       {
         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
         { 0, 2, 4, 6, 8, 0, 2, 4, 6, 8},
         { 0, 3, 6, 9, 2, 5, 8, 1, 4, 7},
         { 0, 4, 8, 2, 6, 0, 4, 8, 2, 6},
         { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5},
         { 0, 6, 2, 8, 4, 0, 6, 2, 8, 4},
         { 0, 7, 4, 1, 8, 5, 2, 9, 6, 3},
         { 0, 8, 6, 4, 2, 0, 8, 6, 4, 2},
         { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1}
       };
      static const char multtens [10][10] =
       {
         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
         { 0, 0, 0, 0, 1, 1, 1, 2, 2, 2},
         { 0, 0, 0, 1, 1, 2, 2, 2, 3, 3},
         { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4},
         { 0, 0, 1, 1, 2, 3, 3, 4, 4, 5},
         { 0, 0, 1, 2, 2, 3, 4, 4, 5, 6},
         { 0, 0, 1, 2, 3, 4, 4, 5, 6, 7},
         { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8}
       };

      Float Temp1 (left), Temp2 (right);
      Temp1 || Temp2;
      Float Temp3 (Temp1);

         //Again, NaN has precedence
      if (left.isNaN() || right.isNaN()) return (Temp1 |= fNaN);

         //Inf*0 = NaN
      if ((left.isInfinity() && right.isZero()) ||
          (left.isZero() && right.isInfinity())) return (Temp1 |= fNaN);

         //Infinity handling
      if (left.isInfinity() || right.isInfinity())
       {
         if (left.sign() == right.sign()) return (Temp1 |= fInf);
         return (Temp1 |= -fInf);
       }

         //Zero handling: return a zero with the correct sign
      if (left.isZero() || right.isZero())
       {
         Temp1.Exponent = 0;
         Temp1.Significand.setToZero();
         Temp1.Significand.absoluteValue();
         if (left.sign() != right.sign()) Temp1.Significand.negate();
         return Temp1;
       }

         //Prepare for multiply: this is the basic shift-accumulate algorithm
         //It operates in the same manner as a fixed-point calculation
      Column x, y, z, a, b;

         // I never realized what a memory hog this algorithm is...
      x.setLength(Temp1.length() * 2);
      y.setLength(Temp1.length() * 2);
      z.setLength(Temp1.length() * 2);
      a.setLength(Temp1.length() * 2);
      b.setLength(Temp1.length() * 2);

      x &= Temp1.Significand;
      y &= Temp2.Significand;

         //Do the multiply... not a pretty algorithm
      for (int i = x.length() - 1; i >= (x.length() >> 1); i--)
       {
         if (y.digitAt(i) != 0)
          {
            //For each digit in y, we calculate that digit times
            //each digit in x, then add the results to the running total
            //The multiply results have the low digit in one array
            //and the high digit in another, to streamline things
            a.setDigit(0, multones[x.digitAt(0)][y.digitAt(i)]);
            for (int j = 1; j < x.length(); j++)
             {
               a.setDigit(j, multones[x.digitAt(j)][y.digitAt(i)]);
               b.setDigit(j - 1, multtens[x.digitAt(j)][y.digitAt(i)]);
             }

            z += a;
            z += b;
          }

            //The outer loop "shifts" y, reducing the need to move data
         x <<= 1;
       }

         //Calculate exponent
      Temp3.Exponent = Temp1.Exponent + Temp2.Exponent;

      if (z.digitAt(0) == 0) z <<= 1;
      else Temp3.Exponent++;

         //Assign significand
      Temp3.Significand |= z;

         //Sign is the xor of the signs of the opperands
         //This must occur after the assignment, or the sign will dissappear.
      Temp3.Significand.absoluteValue();
      if (left.sign() != right.sign()) Temp3.Significand.negate();

         //Handle rounds
      if (Temp1.Mode.decideRound(Temp3.Significand.sign(),
         z.digitAt(Temp3.length() - 1),
         z.digitAt(Temp3.length())))
       {
         Temp3.Significand++;
         if (Temp3.Significand.overflow())
          {
            Temp3.Exponent++;
            Temp3.Significand.clearOverflow();
          }
       }

         //Finish with exponent overflow
      if ((Temp3.Exponent > maxExponent) && Temp3.Locked)
       {
         Temp3.Infinity = true;
         Temp3.Significand.setToZero();
       }
      if ((Temp3.Exponent < minExponent) && Temp3.Locked)
       {
         Temp3.Significand.setToZero();
       }

      return Temp3;
    }

/*
==============================================================================
   Function: divide operator
------------------------------------------------------------------------------
   NOTES:
      Considering the stink that gradual underflow raised in the IEEE-754
      standardization process, it is humorous that it is not implemented
      here.
      Subnormals are not handled for the same reason that DEC didn't want
      to handle them: it slows down computation.
==============================================================================
*/
   Float operator / (const Float & left, const Float & right)
    {
      Float Temp1 (left), Temp2 (right);
      Temp1 || Temp2;
      Float Temp3(Temp1);

         //Finally, NaN has precedence
      if (left.isNaN() || right.isNaN()) return (Temp1 |= fNaN);

         //Handle the two Inf/Inf and 0/0 NaN cases
      if ((left.isZero() && right.isZero()) ||
          (left.isInfinity() && right.isInfinity())) return (Temp1 |= fNaN);

         //Return an infinity
      if (left.isInfinity() || right.isZero())
       {
         Temp1 |= fInf;
         if (left.sign() != right.sign()) Temp1.Significand.negate();
         return Temp1;
       }

         //Return a zero
      if (left.isZero() || right.isInfinity())
       {
         Temp1.Exponent = 0;
         Temp1.Significand.setToZero();
         Temp1.Significand.absoluteValue();
         if (left.sign() != right.sign()) Temp1.Significand.negate();
         return Temp1;
       }

         //Prepare for divide: this is the basic shift-decrement algorithm
         //It uses the same strategy as a fixed-point divide would use
         //Except it calculates two extra digits to ensure correct rounding
      Column x, y, z, a;

      x.setLength(Temp1.length() * 2 + 3); // +2 to add one extra digit
      y.setLength(Temp1.length() * 2 + 3); // in worst case divides
      z.setLength(Temp1.length() * 2 + 3); // +1 to add a zero pad
      a.setLength(Temp1.length() * 2 + 3); // to improve compares

      x *= Temp1.Significand; //Dividend
      y &= Temp2.Significand; //Our limiting condition
      a *= Temp2.Significand; //Multiplied divisor

      x.absoluteValue(); //Oh God!
      y.absoluteValue(); //I forgot about this!
      a.absoluteValue(); //Terrible things happened!!

      int i = (x.length() >> 1) - 2, m = 0; //i is our increment position
         //m is magic (magic compare optimizer)

         /*
            If the Multiplied Divisor is greater than the Dividend,
               Shift it down and
               Decrement the Current Multiplier.
            Incrementing i DECREMENTS the Current Multiplier.
         */
      if (a > x)
       {
         a >>= 1;
         i++;
       }

         /*
            While the Multiplied Divisor is less than the Dividend,
               Increment the Quotient at i and
               Subtract the Multiplied Divisor from the Dividend.
         */
      while (x >= a)
       {
         z.incrementAt(i);
         x -= a;
       }

         /*
            While the Dividend is greater than or equal to the Divisor,
            repeat the previous steps.
         */
      while (x >= y)
       {
         while (a.isGreaterThan(x, m))
          {
            a >>= 1;
            m++;
            i++;
          }

         while (x.isGreaterThanOrEqualTo(a, m))
          {
            z.incrementAt(i);
            x -= a;
          }
       }
      //The divide is over!

         //Calculate exponent
      Temp3.Exponent = Temp1.Exponent - Temp2.Exponent;

      if (z.digitAt(Temp1.length() - 1) == 0) Temp3.Exponent--;
      else z >>= 1;

         //Assign significand
      z <<= Temp3.length();
      Temp3.Significand |= z;

         //Sign is the xor of the signs of the opperands
      Temp3.Significand.absoluteValue();
      if (left.sign() != right.sign()) Temp3.Significand.negate();

         //Handle rounds
      if (Temp1.Mode.decideRound(Temp3.Significand.sign(),
         z.digitAt(Temp3.length() - 1),
         z.digitAt(Temp3.length())))
       {
         Temp3.Significand++;
         if (Temp3.Significand.overflow())
          {
            Temp3.Exponent++;
            Temp3.Significand.clearOverflow();
          }
       }

         //Finish with exponent overflow
      if ((Temp3.Exponent > maxExponent) && Temp3.Locked)
       {
         Temp3.Infinity = true;
         Temp3.Significand.setToZero();
       }
      if ((Temp3.Exponent < minExponent) && Temp3.Locked)
       {
         Temp3.Significand.setToZero();
       }

      return Temp3;
    }

/*
==============================================================================
   Function: conversion operator to long double
==============================================================================
*/
   Float::operator long double () const
    {
      std::istringstream temp (toString());
      long double result;
      temp >> result;
      return result;
    }

/*
==============================================================================
   Function: conversion constructor from long double
==============================================================================
*/
   Float::Float (long double from)
    {
      Locked = true;
      std::ostringstream temp;
      //Assuming Intel x87 long double:
      //64 bit significand means 21 digits are needed to reversibly
      //represent the number: ceil(64 * log(2) / log(10)) + 1
      //See Mike Cowlishaw's website:
      //   <http://speleotrove.com/decimal/decifaq6.html>
      temp << std::setiosflags(std::ios::scientific)
           << std::setprecision(21) << from;
      std::string arg = temp.str();
      if (arg.length() > 0)
       {
         std::string newArg ("");
         unsigned int i = 0;
         int newExponent = -1;
         bool computeExponent = true;

         if (arg[i] == '-') { newArg = "-"; i++; }
         else if (arg[i] == '+') i++;

         for (;(i < arg.length()) && (arg[i] != 'e') && (arg[i] != 'E'); i++)
          {
            if (arg[i] != '.') newArg += arg[i];
            else computeExponent = false;
            if (computeExponent) newExponent++;
          }
         Significand = newArg;

         if ((i < arg.length()) && ((arg[i] != 'e') || (arg[i] != 'E')))
          {
            std::istringstream temp (arg.c_str() + i + 1);
            temp >> Exponent;
            Exponent += newExponent;
          }
         else Exponent = newExponent;

         Infinity = false;
         NaN = false;
       }
      else
       {
         Exponent = 0;
         Infinity = false;
         NaN = false;
       }
    }

/*
==============================================================================
   Function: Lock exponent into range
==============================================================================
*/
   void Float::lock (void)
    {
      Locked = true;

      if (Exponent > maxExponent)
       {
         Infinity = true;
         Significand.setToZero();
       }
      if (Exponent < minExponent)
       {
         Significand.setToZero();
       }
    }

/*
==============================================================================
   Function: Unlock exponent range
==============================================================================
*/
   void Float::unlock (void) { Locked = false; }

/*
==============================================================================
   Function Group: Integer Information
------------------------------------------------------------------------------
   NOTES:
      isOdd and isEven do not check if the float is an integer
==============================================================================
*/
   bool Float::isInteger (void) const
    {
      int i;
      if ((Exponent > Significand.length()) || Significand.isZero())
         return true;
      if (Exponent < 0) return false;
      for (i = Exponent + 1; i < Significand.length(); i++)
         if (Significand.digitAt(i) != 0) break;
      if (i == Significand.length()) return true;
      return false;
    }

   bool Float::isOdd (void) const
    {
      if ((Exponent > Significand.length()) || (Exponent < 0)) return false;
      if (Significand.digitAt(Exponent) & 1) return true;
      return false;
    }

   bool Float::isEven (void) const
    {
      if (Exponent > Significand.length()) return true;
      if (Exponent < 0) return false;
      if (Significand.digitAt(Exponent) & 1) return false;
      return true;
    }

 } //namespace DecFloat
