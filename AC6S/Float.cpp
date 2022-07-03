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

#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>
#include "Float.hpp"

#ifndef SEPERATOR
 #define SEPERATOR '.'
#endif

namespace BigInt
 {

    /*
      Remember: you will get digits + 1 of actual precision.
    */
   unsigned long Float::minPrecision = 7;
   unsigned long Float::maxPrecision = 511;

    /*
      2 * maxExponent and maxExponent - minExponent should not exceed LONGMAX.
      2 * minExponent and minExponent - maxExponent should not exceed LONGMIN.
    */
   long Float::minExponent = -999999999;
   long Float::maxExponent = 999999999;


   Float::Float (const std::string & from)
    {
      fromString(from);
    }

   Float::Float (const char * from)
    {
      fromString(from);
    }


   void Float::precisionChanger(unsigned long newPrecision)
    {
      bool checkOverflow = newPrecision < Data.getPrecision();

      Data.setSign(Sign);
      Data.changePrecision(newPrecision);
      Data.setSign(false);

      if (checkOverflow)
       {
         Fixed top (10, 0);
         if (Data >= top)
          {
               //Generally implies that the removed digit is '0'.
            Data.changePrecision(Data.getPrecision() - 1);
            Data.setPrecision(Data.getPrecision() + 1);
            Exponent++;

            if (Exponent > Float::maxExponent)
             {
               Infinity = true;
               Exponent = 0;
               Data = Fixed(0, Float::minPrecision);
             }
          }
       }
    }


   bool operator > (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return false;
            else return true;
          }
         else if (lhs.isInfinity()) return !lhs.isSigned();
         else return !rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) > 0;
    }

   bool operator < (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return true;
            else return false;
          }
         else if (lhs.isInfinity()) return lhs.isSigned();
         else return rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) < 0;
    }

   bool operator >= (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return false;
            else return true;
          }
         else if (lhs.isInfinity()) return !lhs.isSigned();
         else return !rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) >= 0;
    }

   bool operator <= (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return true;
            else return false;
          }
         else if (lhs.isInfinity()) return lhs.isSigned();
         else return rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) <= 0;
    }

   bool operator == (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity()) return false;
      if (lhs.isZero() && rhs.isZero()) return true;
      return lhs.compare(rhs) == 0;
    }

   bool operator != (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() && rhs.isNaN()) return true;
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity()) return true;
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) != 0;
    }


   Float & operator += (Float & lval, const Float & rval)
    {
      return (lval = lval + rval);
    }

   Float & operator -= (Float & lval, const Float & rval)
    {
      return (lval = lval - rval);
    }

   Float & operator *= (Float & lval, const Float & rval)
    {
      return (lval = lval * rval);
    }

   Float & operator /= (Float & lval, const Float & rval)
    {
      return (lval = lval / rval);
    }


   Float & Float::operator = (const Float & src)
    {
      if (&src == this) return *this;
      Data = src.Data;
      Sign = src.Sign;
      Exponent = src.Exponent;
      Infinity = src.Infinity;
      NaN = src.NaN;
      return *this;
    }

   Float & Float::operator |= (const Float & src)
    {
      if (&src == this) return *this;
      unsigned long Precision;
      Precision = Data.getPrecision();
      Data = src.Data;
      Sign = src.Sign;
      setPrecision(Precision);
      Exponent = src.Exponent;
      Infinity = src.Infinity;
      NaN = src.NaN;
      return *this;
    }


   int Float::compare (const Float & to) const
    {
      if (&to == this) return 0;

      if (Exponent == to.Exponent)
       {
         Fixed temp1 (Data);
         Fixed temp2 (to.Data);

         temp1.setSign(Sign);
         temp2.setSign(to.Sign);

         return temp1.compare(temp2);
       }
      else if (Sign && to.Sign)
       {
         if (Exponent > to.Exponent) return -1;
         else return 1;
       }
      else if (!Sign && to.Sign) return -1;
      else if (Sign && !to.Sign) return 1;
      else
       {
         if (Exponent > to.Exponent) return 1;
         else return -1;
       }
    }


   void match (Float & left, Float & right)
    {
      if (left.getPrecision() > right.getPrecision())
         right.setPrecision(left.getPrecision());
      else if (left.getPrecision() < right.getPrecision())
         left.setPrecision(right.getPrecision());
    }


   bool change (const Float & arg1, const Float & arg2)
    {
      if (!arg1.isUnSpecial() || !arg2.isUnSpecial())
         return false;

      unsigned long prec, diff;

      prec = arg1.getPrecision() >= arg2.getPrecision() ?
         arg1.getPrecision() :
         arg2.getPrecision();

      diff = arg1.exponent() > arg2.exponent() ?
         arg1.exponent() - arg2.exponent() :
         arg2.exponent() - arg1.exponent();

         //Account for a rounding digit.
      if (diff > (prec + 1)) return false;
      return true;
    }


   Float operator + (const Float & lhs, const Float & rhs)
    {
      Float temp;
       /*
         We don't bump temp's precision here because it wouldn't be
         meaningful. It isn't meaningful to have an Infinity with 500
         digits of precision.
       */

         //First, handle NaNs, as they have the highest precedence.
       /*
         As I write this, I still haven't convinced myself of the usefulness
         of diagnostic NaNs. I have never had language support for them.
         Undocumented: 1: Inf - Inf, 2: 0 * Inf, 4: Inf / Inf, 8: 0 / 0
         16: Inv Op (sqrt/ln(-)), 32: User Input / Initialized Value
       */
      if (lhs.isNaN() || rhs.isNaN())
       {
         if (lhs.isNaN() && rhs.isNaN())
            temp.NaN = lhs.isNaN() | rhs.isNaN();
         else if (lhs.isNaN())
            temp.NaN = lhs.isNaN();
         else
            temp.NaN = rhs.isNaN();
         return temp;
       }

         //Next, if both are infinities.
      if (lhs.isInfinity() && rhs.isInfinity())
       {
            //Inf - Inf = NaN
         if (lhs.isSigned() != rhs.isSigned())
          {
            temp.NaN = 1;
            return temp;
          }

         temp.copySign(lhs);
         temp.Infinity = true;
         return temp;
       }

         //Anything else with infinity is infinity
      if (lhs.isInfinity())
       {
         temp.copySign(lhs);
         temp.Infinity = true;
         return temp;
       }
      if (rhs.isInfinity())
       {
         temp.copySign(rhs);
         temp.Infinity = true;
         return temp;
       }

         //Finally, zeros.
      if (lhs.isZero() && rhs.isZero() && (lhs.Sign != rhs.Sign))
         return temp;
      if (rhs.isZero()) return lhs;
      if (lhs.isZero()) return rhs;

         //Do we need to compute anything?
      if (change(lhs, rhs))
       {
         Float temp1 (lhs), temp2 (rhs);
            //Denormalize the Precisions to take account of the Exponents
         if (temp1.Exponent > temp2.Exponent)
          {
            temp.Exponent = temp1.Exponent;
            temp2.Data.setPrecision
               (temp2.Data.getPrecision() + temp1.Exponent - temp2.Exponent);
          }
         else if (temp1.Exponent < temp2.Exponent)
          {
            temp.Exponent = temp2.Exponent;
            temp1.Data.setPrecision
               (temp1.Data.getPrecision() + temp2.Exponent - temp1.Exponent);
          }
         else temp.Exponent = temp1.Exponent;

            //Assign the correct signs before operating
         temp1.Data.setSign(temp1.Sign);
         temp2.Data.setSign(temp2.Sign);

         temp.Data = temp1.Data + temp2.Data;

            //Clear the sign
         temp.Sign = temp.Data.isSigned();
         temp.Data.abs();

            //Normalize result
         temp.normalize();
         temp.setPrecision(lhs.Data.getPrecision() > rhs.Data.getPrecision() ?
            lhs.Data.getPrecision() : rhs.Data.getPrecision());
       }
      else
       {
            //Just return either the lhs or the rhs
         temp = lhs.Exponent > rhs.Exponent ? lhs : rhs;
            //We don't change the precision of the result,
            //because we haven't added any meaningful information
            //For example: 2.0 + 1.000E-16 should not be 2.000
       }

      return temp;
    }

   Float operator - (const Float & lhs, const Float & rhs)
    {
      Float temp;

      if (lhs.isNaN() || rhs.isNaN())
       {
         if (lhs.isNaN() && rhs.isNaN())
            temp.NaN = lhs.isNaN() | rhs.isNaN();
         else if (lhs.isNaN())
            temp.NaN = lhs.isNaN();
         else
            temp.NaN = rhs.isNaN();
         return temp;
       }

      if (lhs.isInfinity() && rhs.isInfinity())
       {
         if (lhs.isSigned() == rhs.isSigned())
          {
            temp.NaN = 1;
            return temp;
          }

         temp.copySign(lhs);
         temp.Infinity = true;
         return temp;
       }

      if (lhs.isInfinity())
       {
         temp.copySign(lhs);
         temp.Infinity = true;
         return temp;
       }
      if (rhs.isInfinity())
       {
         temp.setSign(!rhs.Sign);
         temp.Infinity = true;
         return temp;
       }

      if (lhs.isZero() && rhs.isZero() && (lhs.Sign == rhs.Sign))
         return temp;
      if (rhs.isZero()) return lhs;
      if (lhs.isZero()) return -rhs;

      if (change(lhs, rhs))
       {
         Float temp1 (lhs), temp2 (rhs);
         if (temp1.Exponent > temp2.Exponent)
          {
            temp.Exponent = temp1.Exponent;
            temp2.Data.setPrecision
               (temp2.Data.getPrecision() + temp1.Exponent - temp2.Exponent);
          }
         else if (temp1.Exponent < temp2.Exponent)
          {
            temp.Exponent = temp2.Exponent;
            temp1.Data.setPrecision
               (temp1.Data.getPrecision() + temp2.Exponent - temp1.Exponent);
          }
         else temp.Exponent = temp1.Exponent;

         temp1.Data.setSign(temp1.Sign);
         temp2.Data.setSign(temp2.Sign);

         temp.Data = temp1.Data - temp2.Data;

         temp.Sign = temp.Data.isSigned();
         temp.Data.abs();

         temp.normalize();
         temp.setPrecision(lhs.Data.getPrecision() > rhs.Data.getPrecision() ?
            lhs.Data.getPrecision() : rhs.Data.getPrecision());
       }
      else
       {
         temp = lhs.Exponent > rhs.Exponent ? lhs : rhs;
       }

      return temp;
    }

   Float operator * (const Float & lhs, const Float & rhs)
    {
      Float temp;

         //Again, NaN has precedence
      if (lhs.isNaN() || rhs.isNaN())
       {
         if (lhs.isNaN() && rhs.isNaN())
            temp.NaN = lhs.isNaN() | rhs.isNaN();
         else if (lhs.isNaN())
            temp.NaN = lhs.isNaN();
         else
            temp.NaN = rhs.isNaN();
         return temp;
       }

         //Inf*0 = NaN
      if ((lhs.isInfinity() && rhs.isZero()) ||
          (lhs.isZero() && rhs.isInfinity()))
       {
         temp.NaN = 2;
         return temp;
       }

         //Put this here, as Zero and Infinity need the correct sign.
      temp.setSign(lhs.isSigned() ^ rhs.isSigned());

         //Infinity handling
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         temp.Infinity = true;
         return temp;
       }

         //Zero handling: return a zero with the correct sign
      if (lhs.isZero() || rhs.isZero()) return temp;

         // Do we need to do any computation?
      if ((lhs.Exponent + rhs.Exponent) > Float::maxExponent)
       {
            //No, the result will overflow anyway.
         temp.Infinity = true;
         temp.Exponent = 0;
         temp.Data = Fixed(0, Float::minPrecision);
       }
      else if ((lhs.Exponent + rhs.Exponent) < (Float::minExponent - 1))
       {
            //No, the result will underflow anyway.
         temp.Exponent = 0;
         temp.Data = Fixed(0, Float::minPrecision);
       }
      else
       {
            //Do the multiply
         temp.Data = lhs.Data * rhs.Data;
         temp.Exponent = lhs.Exponent + rhs.Exponent;

            //Normalize
         if (temp.Data.getSticky())
          {
            temp.Data.clearSticky();
            temp.Data.setPrecision(temp.Data.getPrecision() + 1);
            if ((temp.Exponent + 1) > Float::maxExponent)
             {
               temp.Infinity = true;
               temp.Exponent = 0;
               temp.Data = Fixed(0, Float::minPrecision);
             }
            else
               temp.Exponent++;
          }

            //Check for the edge-case underflow.
         else if (temp.Exponent < Float::minExponent)
          {
            temp.Exponent = 0;
            temp.Data = Fixed(0, Float::minPrecision);
          }
       }

      return temp;
    }

   Float operator / (const Float & lhs, const Float & rhs)
    {
      Float temp;

         //Finally, NaN has precedence
      if (lhs.isNaN() || rhs.isNaN())
       {
         if (lhs.isNaN() && rhs.isNaN())
            temp.NaN = lhs.isNaN() | rhs.isNaN();
         else if (lhs.isNaN())
            temp.NaN = lhs.isNaN();
         else
            temp.NaN = rhs.isNaN();
         return temp;
       }

         //Handle the two Inf/Inf and 0/0 NaN cases
      if (lhs.isZero() && rhs.isZero())
       {
         temp.NaN = 4;
         return temp;
       }
      if (lhs.isInfinity() && rhs.isInfinity())
       {
         temp.NaN = 8;
         return temp;
       }

         //Put this here, as Zero and Infinity need the correct sign.
      temp.setSign(lhs.isSigned() ^ rhs.isSigned());

         //Return an infinity
      if (lhs.isInfinity() || rhs.isZero())
       {
         temp.Infinity = true;
         return temp;
       }

         //Return a zero
      if (lhs.isZero() || rhs.isInfinity()) return temp;

         //Do we need to do any work?
      if ((lhs.Exponent - rhs.Exponent) > (Float::maxExponent + 1))
       {
            //No, the result will overflow anyway.
         temp.Infinity = true;
         temp.Exponent = 0;
         temp.Data = Fixed(0, Float::minPrecision);
       }
      else if ((lhs.Exponent - rhs.Exponent) < Float::minExponent)
       {
            //No, the result will underflow anyway.
         temp.Exponent = 0;
         temp.Data = Fixed(0, Float::minPrecision);
       }
      else
       {
            //Do the divide
         temp.Data = lhs.Data / rhs.Data;
         temp.Exponent = lhs.Exponent - rhs.Exponent;

            //Normalize
         if (temp.Data.getSticky())
          {
            temp.Data.clearSticky();
            temp.Data.setPrecision(temp.Data.getPrecision() - 1);
            if ((temp.Exponent - 1) < Float::minExponent)
             {
               temp.Exponent = 0;
               temp.Data = Fixed(0, Float::minPrecision);
             }
            else
               temp.Exponent--;
          }

            //Check for the edge-case overflow
         else if (temp.Exponent > Float::maxExponent)
          {
            temp.Infinity = true;
            temp.Exponent = 0;
            temp.Data = Fixed(0, Float::minPrecision);
          }
       }

      return temp;
    }


   std::string Float::toString (void) const
    {
      if (NaN) return std::string ("NaN");
      if (Infinity)
       {
         if (Sign) return std::string ("-Inf");
         else return std::string ("Inf");
       }
      std::string result ("");
      if (Sign) result += "-";
      result += Data.toString();
      result += "E";
      std::ostringstream temp;
      temp << Exponent;
      result += temp.str();
      return result;
    }


   void Float::fromString (const char * src)
    {
      if (!std::strcmp(src, "Inf") || !std::strcmp(src, "+Inf") ||
          !std::strcmp(src, "INF") || !std::strcmp(src, "+INF"))
       {
         Data = Fixed(0, minPrecision);
         Sign = false;
         Exponent = 0;
         Infinity = true;
         NaN = 0;
         return;
       }

      if (!std::strcmp(src, "-Inf") || !std::strcmp(src, "-INF"))
       {
         Data = Fixed(0, minPrecision);
         Sign = true;
         Exponent = 0;
         Infinity = true;
         NaN = 0;
         return;
       }

      if (!std::strcmp(src, "NaN") || !std::strcmp(src, "NAN"))
       {
         Data = Fixed(0, minPrecision);
         Sign = false;
         Exponent = 0;
         Infinity = false;
         NaN = 32;
         return;
       }

      if (*src == '\0')
       {
         Data = Fixed(0, minPrecision);
         Sign = false;
         Exponent = 0;
         Infinity = false;
         NaN = 0;
         return;
       }

      Sign = false;
      Infinity = false;
      NaN = 0;

      const char * iter = src;
      std::string conv;
      bool computeExponent = true, insertSep = true;
      long newExponent = -1;

      conv.reserve(2048);

         // The Fixed is "signless".
      if (*iter == '-')
       {
         Sign = true;
         iter++;
       }
      else if (*iter == '+') iter++;

       /*
         We strip the seperator from the string and insert our own to create
         a normalized Fixed portion. Floats created from strings have string
         defined precision.
       */
      if (*iter == SEPERATOR) newExponent++;
      for (;(*iter != '\0') && (*iter != 'e') && (*iter != 'E'); iter++)
       {
         if (*iter != SEPERATOR) conv += *iter;
         else computeExponent = false;
         if (computeExponent) newExponent++;
         if (insertSep)
          {
            conv += SEPERATOR;
            insertSep = false;
          }
       }
      Data.fromString(conv);

      if ((*iter != '\0') && ((*iter != 'e') || (*iter != 'E')))
       {
         std::istringstream temp (iter + 1);
         temp >> Exponent;
         if ((Exponent + newExponent) > maxExponent)
          {
            Infinity = true;
            Exponent = 0;
            Data = Fixed(0, minPrecision);
          }
         else if ((Exponent + newExponent) < minExponent)
          {
            Exponent = 0;
            Data = Fixed(0, minPrecision);
          }
         else
            Exponent += newExponent;
       }
      else Exponent = newExponent;

      if (Data.isZero()) Exponent = 0;
    }


    /*
      This function seems to be orphaned now, as I inlined all
      of the normalization in +, -, *, and / to take advantage
      of a priori knowledge of the results.

      Not really, as + and - really need all this work.
      So does input.
    */
   Float & Float::normalize (void)
    {
      Fixed upper ((long long)10, 0), lower ((long long)1, 0);

      if (Data.isZero())
       {
         Exponent = 0;
         Data.setPrecision(minPrecision);
       }

      else if (Data >= upper)
       {
         Data.setPrecision(Data.getPrecision() + 1);
         if ((Exponent + 1) > maxExponent)
          {
            Infinity = true;
            Exponent = 0;
            Data = Fixed(0, minPrecision);
          }
         else
            Exponent++;
       }
      else
       {
         while (Data < lower)
          {
            Data.setPrecision(Data.getPrecision() - 1);
            if ((Exponent - 1) < minExponent)
             {
               Exponent = 0;
               Data = Fixed(0, minPrecision);
             }
            else
               Exponent--;
          }
       }

      return *this;
    }


   bool Float::isInteger (void) const
    {
      if (Exponent < 0) return false;
      if ((((unsigned long)Exponent) >= Data.getPrecision()) || Data.isZero()) return true;

      Fixed temp (Data);

      temp.setPrecision(temp.getPrecision() - Exponent);

      return temp.isInteger();
    }

   bool Float::isOdd (void) const
    {
      if (Exponent < 0) return false;
      if ((((unsigned long)Exponent) > Data.getPrecision()) || Data.isZero()) return false;

      Fixed temp (Data);

      temp.setPrecision(temp.getPrecision() - Exponent);

      return temp.isOdd();
    }

   bool Float::isEven (void) const
    {
      if (Exponent < 0) return false;
      if ((((unsigned long)Exponent) > Data.getPrecision()) || Data.isZero()) return true;

      Fixed temp (Data);

      temp.setPrecision(temp.getPrecision() - Exponent);

      return temp.isEven();
    }


 } /* namespace BigInt */
