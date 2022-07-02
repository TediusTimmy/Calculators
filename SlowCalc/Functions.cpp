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
   File: Functions.cpp
      I wanted fact() to be tgamma() instead, but I don't know a good
      approximation for the required precision. Numerical integration
      methods weren't helping.
*/

//HEADER:
// sstream allows us to do easy conversion from integers.
// ctime will initialize the random number generator.
// Float defines the class that these functions operate on.
// Constants contains constants which will make the program run faster.
// rand850 is the random number generator
#include <sstream>
#include <ctime>
#include "Float.hpp"
#include "Constants.hpp"
#include "rand850.h"

/* More funny logic */
#ifdef MODE_1
 #define ALT_REDUCE
#endif
#ifdef MODE_2
 #define ALT_REDUCE
#endif

/*
   Implements:
      All non-class functions
      Operator ^ and %

   A word of explanation:
      These are procedures and not class methods as they aim to be
      syntactically like the procedural C functions which they come from.
      They just operate on 'Float's instead of 'double's.
*/

namespace DecFloat
 {

/*
==============================================================================
   Function: square root
------------------------------------------------------------------------------
   INPUT: number to take the square root of
   OUTPUT: the square root of the input
   NOTES:
      Caved to IEEE: sqrt(-0) = -0 now.

      Newton-Rhapson is so much faster than x ^ .5,
      which becomes e ^ (.5 * ln(x)).

      In my Java implementation, sqrt was not defined until after arctan,
      as it is used by arccos and arcsin. It is here because it is SO
      MUCH MORE IMPORTANT than any other function.

      I also tried to change the series from (x^2 + a) / 2x to
      x(3 - (1/a)(x^2)) / 2 which replaces an add and a divide with
      three multiplies. This was suggested by Hennessey and Patterson's
      book _Computer Architecture_. As the first series starts at x^2 = a,
      this starts at 1/(x^2) = 1/a.
==============================================================================
*/
   Float sqrt (const Float & opp)
    {
      Float copyOpp (opp);

      if (opp.isZero()) return opp;
      if (opp.sign()) return (copyOpp |= fNaN);
      if (opp.isNaN() || opp.isInfinity()) return opp;

      copyOpp.lengthen(opp.getSeriesExtra());

      int diff, term = opp.length() + opp.getDerivedExtra();

      Float temp (copyOpp);
      Float curApprox (copyOpp);
      Float lastApprox (copyOpp);

         //This helps to produce a good first estimate, and is why
         //this function has to be a friend.
      curApprox.Exponent /= 2;
      do
       {
         lastApprox = curApprox;

         curApprox = (curApprox * curApprox + copyOpp) / (curApprox + curApprox);

         temp = curApprox - lastApprox;

         diff = curApprox.Exponent - temp.Exponent;
         if (diff < 0) diff = -diff;
       }
      while ((diff <= term) && !temp.isZero());

      curApprox.roundAt(opp.length());
      curApprox.truncate(opp.getSeriesExtra());
      return curApprox;
    }

/*
==============================================================================
   Function: exponential function, Euler's constant to a real power
------------------------------------------------------------------------------
   NOTES:
      Basically: the Maclaurin series approximation for e ^ x

      I made one slight change: I noticed poor convergence and
      premature loop termination with negative exponents, so I
      force the exponent to be positive, then divide one by
      the result if it was originally negative.
      This has fixed my problem.

      I had a similar problem with sine and cosine with all large
      numbers (numbers with great magnitude), and the problem
      is/was that as the series goes on, the terms become the very
      small difference between very large numbers, and so there is
      a very big error due to round-off.

      I had an epiphany today, thanks to R. Brent in "Fast Multiple-Precision
      Evaluation of Elementary Functions":
         exp(x * 10 ^ y) = (exp(x * 10 ^ (y - 1))) ^ 10 = (exp(x)) ^ (10 ^ y)
      This works SO MUCH BETTER for base 2. It reduces the range and would be
      easy to implement as loops. My concern is the accuracy after repeated
      multiplications (probably negligable) and if it really speeds things up.
==============================================================================
*/
   Float exp (const Float & opp)
    {
      Float one (opp);

      if (opp.isZero()) return (one |= M_1);
      else if (opp.isNaN()) return opp;
      else if (opp.isInfinity())
       {
         if (!opp.sign()) return opp;
         one |= Float(0);
         return one;
       }

      one.lengthen(opp.getSeriesExtra() * 2);
      one |= M_1;

      Float currentFactorial (one);
      Float currentDivisor (one);
      Float currentPower (one);
      Float copyOpp (one);
      Float res (one);
      Float temp (one);
      bool special = opp.Exponent > -4; //Four is specially chosen to maximize
      int loops = opp.Exponent + 4; //speed within the current extra precision

      currentFactorial |= Float(2);
      currentDivisor |= Float(2);
      currentPower |= opp;
      copyOpp |= opp;

      currentPower.abs();
      copyOpp.abs();
      if (special)
       {
         currentPower.Exponent = -4;
         copyOpp.Exponent = -4;
       }


      res += currentPower;
      currentPower *= copyOpp;

      temp = currentPower / currentDivisor;

         /*
            While the use of && is not intuitive, || is less intuitive,
            and at least it returns a bool, which is intuitive.

            As this is the first occurance of this,
            I will describe what it does agian:
            This is an indirect measure of approximate error. As
            calculating the actual absolute relavtive approximate error
            would be expensive, time-wise as it includes a divide,
            we work around that. This operator returns true when the
            following term of a summation would have an absolute
            relative approximate error of 0%. This ALWAYS occurs
            because eventually a term will become so small that it
            does not effect the previous term, at least for every
            summation implemented.
         */
      while (temp && res)
       {
         res += temp;
         currentFactorial += one;
         currentDivisor *= currentFactorial;
         currentPower *= copyOpp;
         temp = currentPower / currentDivisor;
       }

      if (special)
       {
         /*
            This loop can only execute ten times before the value within
            exceeds the limit of "MODE_0": the exponent exceeds 1000000000.
            Also, with this there is no need for the unlock() and lock()
            mechanism, which will only be kept for sinh() and cosh().
            This just requires extra precision.
         */
         while (loops-- && !res.Infinity)
          {
            temp = res;
            temp *= temp; //2
            temp *= temp; //4
            res *= temp;  //5
            res *= res;   //10
          }
       }

      if (opp.sign()) res = one / res;

      res.roundAt(opp.length());
      res.truncate(opp.getSeriesExtra() * 2);
      return res;
    }

/*
==============================================================================
   Function: natural logrithm
------------------------------------------------------------------------------
   NOTES:
      log(x) = 2 * sum (i = 1 to Inf)  of
         (((i - 1) / (i + 1)) ^ (2 * i - 1)) / (2 * i - 1)

      This started general-purpose, but the series for ln has poor
      convergence outside of (.5,2). Remove all areas labeled new code
      to get the log function that can compute ln(10) itself.
      You should probably use the equation in the header, though,
      it converges faster and can be used here,
      without changing the function.

      So, by basic laws of logrithms:
         ln(x * 10 ^ y) = ln(x) + y * ln(10)
      Predefining ln(10) may limit the precision to that definition,
      but it speeds things up.

      As convergence is best at one and decays around one such that
      the convergence of x is as bad as 1 / x, we try to force
      numbers into an area of better convergence. As it is,
      x > sqrt(10) converges slower than x / 10. To speed things up,
      if (x > 3) we use x / 10 instead of x. This puts pi in the
      region of worst convergence, but everywhere else will be faster.

      Update: for version 2, we will use one of three modes:
         [1, 1.78)      ln(x) + y * ln(10)
         [1.78, 5.62)   ln(x / sqrt(10)) + (y + .5) * ln(10)
         [5.62, 10)     ln(x / 10) + (y + 1) * ln(10)

      We can continue to use reductions to the argument to keep the actual
      calculation within some range of fast convergence, however it has
      diminishing returns.
==============================================================================
*/
   Float log (const Float & opp)
    {
      Float one (opp);

      if (opp.isZero()) return (one |= -fInf);
      if (opp.sign()) return (one |= fNaN);
      if (opp.isNaN() || opp.isInfinity()) return opp;

      one.lengthen(opp.getSeriesExtra());
      one |= M_1;

      Float currentDivisor (one);
      currentDivisor |= Float(3);
      Float copyOpp (one);
      copyOpp |= opp;

      Float currentPower (one);
      Float res (one);
      Float temp (one);
      int mode;

      mode = opp.Significand.digitAt(0) * 100 + opp.Significand.digitAt(1)
         * 10 + opp.Significand.digitAt(2);

      if (mode < 178) /* 100 .. 177 */
       {
         copyOpp.Exponent = 0;
         mode = 1;
       }
      else if (mode < 562) /* 178 .. 561 */
       {
         copyOpp.Exponent = 0;
         temp |= M_1_SQRT10;
         copyOpp *= temp;
         mode = 2;
       }
      else /* 562 .. 999 */
       {
         copyOpp.Exponent = -1;
         mode = 3;
       }

      res = copyOpp - one;
      temp = copyOpp + one;

      currentPower = res / temp;
      copyOpp = currentPower;
      res = currentPower;
      one |= Float(2);

      copyOpp *= copyOpp;
      currentPower *= copyOpp;

      temp = currentPower / currentDivisor;
      while (temp && res)
       {
         res += temp;
         currentDivisor += one; //which is now 2
         currentPower *= copyOpp;
         temp = currentPower / currentDivisor;
       }
      res *= one;

      switch (mode)
       {
         case 1:
            if (opp.Exponent == 0) break;
            temp |= Float(opp.Exponent);
            one |= M_LN10;

            temp *= one; //which is now ln(10)
            res += temp;
            break;
         case 2:
            temp |= (Float(opp.Exponent) + Float(.5));
            one |= M_LN10;

            temp *= one;
            res += temp;
            break;
         case 3:
            if (opp.Exponent == -1) break;
            temp |= Float(opp.Exponent + 1);
            one |= M_LN10;

            temp *= one;
            res += temp;
            break;
       }

      res.roundAt(opp.length());
      res.truncate(opp.getSeriesExtra());
      return res;
    }

/*
==============================================================================
   Function: exponentiation operator
------------------------------------------------------------------------------
   NOTES:
      This is simply x ^ y = e ^ (y * ln(x))
      It doesn't work for -x, so (-1) ^ 2 = NaN.

      1^Inf becomes an NaN naturally by this definition.

      We define 0^0 = 1 because:
         1. It follows that this must be the case from the empty product
            definition for x ^ 0.
         2. The functions exp and cos use this identity in their Maclaurin
            series definitions implicitly.
         3. This is the most useful behavior for this function.

      I realize that this isn't the C definition for this operator,
      however many languages use this token for this operation.

      This now works rudimentarily for negative numbers. To be better, it
      would have to rationalize the fractional part and decide if that
      exponent is real, however this implies that the rational part is
      precise, which is incorrect for most purposes.
==============================================================================
*/
   Float operator ^ (const Float & left, const Float & right)
    {
      Float base (left), exponent (right), res;
      base || exponent;

         //exceptions to the rules
      if (left.isInfinity() && right.isZero()) return (base |= fNaN);
      if (left.isZero() && right.isInfinity() && right.sign())
         return (base |= fInf);

      if (right.isZero()) return (base |= M_1);
      if (left.isZero()) return (base |= Float(0));

      if (left.sign() && !right.isInteger()) return (base |= fNaN);

      base.lengthen(left.getDerivedExtra());
      exponent.lengthen(left.getDerivedExtra());

      if (left.sign())
       {
         base.abs();
         if (right.isOdd()) res = -exp(log(base) * exponent);
         else res = exp(log(base) * exponent);
       }
      else
         res = exp(log(base) * exponent); //exponent must be on the right.

      res.roundAt(base.length() - base.getDerivedExtra());
      res.truncate(base.getDerivedExtra());
      return res;
    }

/*
==============================================================================
   Function: modulus operator
------------------------------------------------------------------------------
   NOTES:
      This is not IEEE 754 and may never be. The math is too much for me.
      This probably does what people expect it to anyway, unlike IEEE.
==============================================================================
*/
   Float operator % (const Float & left, const Float & right)
    {
      Float res;

      if (right.isInfinity() || right.isZero()) return left;

      res = left / right;
      res.lengthen(res.length());
      res.Int();

      res *= right;
      res = left - res;

      res.truncate(res.length() / 2);
      return res;
    }

/*
==============================================================================
   Function: reduce (source file scope only)
------------------------------------------------------------------------------
   NOTES:
      Reduce for sine and cosine. Should reduce a number 10^31 before
      great rounding errors take place. I was using IEEE rounding, but just
      using Int(), as Intel does, produces easier results.

      This doesn't produce accurate results for numbers with great magnitude:
      x > 10^7. It seems I need exponent extra digits of precision to get the
      correct accuracy.
==============================================================================
*/
   static void reduce (Float & arg)
    {
#ifndef ALT_REDUCE
      Rounder temp (arg.Mode);

      arg.Mode = Rounder(Round::ROUND_TO_ZERO, Round::INVARIANT);

      Float twopi (arg), oneovertwopi (arg), res;

      twopi |= M_2PI;
      oneovertwopi |= M_1_2PI;

      for (int loops = 0; (loops < 64) && (arg > twopi); loops++)
       {
         res = arg * oneovertwopi;
         res.lengthen(res.length());
         res.Int();

         res *= twopi;
         res = arg - res;

         res.truncate(res.length() / 2);
         arg = res;
       }

      arg.Mode = temp;

      if (arg > twopi)
       {
         arg *= oneovertwopi;
         arg.Frac();
         arg *= twopi;
       }
#else /* ALT_REDUCE */
      int retLength = arg.length();

      if (arg.exponent() > 0) arg.lengthen(arg.exponent());

      Float twopi (arg), oneovertwopi (arg), res;

      twopi |= M_2PI;
      oneovertwopi |= M_1_2PI;

      arg *= oneovertwopi;
      arg.Frac();
      arg *= twopi;

      arg.setLength(retLength);
#endif /* NOT ALT_REDUCE */
    }

/*
==============================================================================
   Function: sine of an angle in radians
------------------------------------------------------------------------------
   NOTES:
      An implementation of the Maclaurin series for sine.
==============================================================================
*/
   Float sin (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isInfinity()) return (one |= fNaN);

      one.lengthen(opp.getSeriesExtra());
      one |= M_1;

      Float currentFactorial (one);
      currentFactorial |= Float(3);
      Float currentDivisor (one);
      currentDivisor |= Float(6);

      Float temp (one);
      Float copyOpp (one);

      copyOpp |= opp;
      reduce(copyOpp.abs()); //We will replace the sign later.

      Float currentPower (copyOpp);
      Float res (copyOpp);

      copyOpp *= copyOpp;
      currentPower *= copyOpp;

      bool alt = true;
      temp = currentPower / currentDivisor;
      while (temp && res)
       {
         if (alt) res -= temp;
         else res += temp;
         alt = !alt;

         currentFactorial += one;
         currentDivisor *= currentFactorial;
         currentFactorial += one;
         currentDivisor *= currentFactorial;

         currentPower *= copyOpp;

         temp = currentPower / currentDivisor;
       }

      if (opp.sign()) res.negate(); //Sign replaced.

      res.roundAt(opp.length());
      res.truncate(opp.getSeriesExtra());
      return res;
    }

/*
==============================================================================
   Function: cosine of an angle in radians
------------------------------------------------------------------------------
   NOTES:
      An implementation of the Maclaurin series for cosine.
==============================================================================
*/
   Float cos (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isInfinity()) return (one |= fNaN);

      one.lengthen(opp.getSeriesExtra());
      one |= M_1;

      Float currentFactorial (one);
      currentFactorial |= Float(2);
      Float currentDivisor (one);
      currentDivisor |= Float(2);

      Float temp (one);
      Float copyOpp (one);

      copyOpp |= opp; //We don't need to replace the sign as
      reduce(copyOpp.abs()); //cos is symmetric about 0.

      Float currentPower (copyOpp);
      Float res (one);

      currentPower *= copyOpp;
      copyOpp *= copyOpp;

      bool alt = true;
      temp = currentPower / currentDivisor;
      while (temp && res)
       {
         if (alt) res -= temp;
         else res += temp;
         alt = !alt;

         currentFactorial += one;
         currentDivisor *= currentFactorial;
         currentFactorial += one;
         currentDivisor *= currentFactorial;

         currentPower *= copyOpp;

         temp = currentPower / currentDivisor;
       }

      res.roundAt(opp.length());
      res.truncate(opp.getSeriesExtra());
      return res;
    }

/*
==============================================================================
   Function: tangent of an angle in radians
------------------------------------------------------------------------------
   NOTES:
      tan(x) = sin(x) / cos(x).
==============================================================================
*/
   Float tan (const Float & opp)
    {
      Float copyOpp (opp);

      if (opp.isNaN() || opp.isInfinity()) return (copyOpp |= fNaN);

      copyOpp.lengthen(opp.getDerivedExtra());

      reduce(copyOpp.abs()); //Reduce only once.
      if (opp.sign()) copyOpp.negate();

      copyOpp = sin(copyOpp) / cos(copyOpp);

      copyOpp.roundAt(opp.length());
      copyOpp.truncate(opp.getDerivedExtra());
      return copyOpp;
    }

/*
==============================================================================
   Function: principle arctangent
------------------------------------------------------------------------------
   NOTES:
      Returns +- pi / 4 on +- 1
      Computes:
         |x| < 1 : sum (i = 1 to Inf)
            ((-1) ^ (i + 1)) * (x ^ (2 * i - 1)) / (2 * i - 1)
         |x| > 1 : pi / 2 - sum (i = 1 to Inf)
            ((-1) ^ i) / (x ^ (2 * i - 1)) / (2 * i - 1)

      Yes, atan really is more important than asin and acos.
      Both will be derived from atan.
==============================================================================
*/
   Float atan (const Float & opp)
    {
      Float one (opp);

      if (opp.isZero()) return one;
      if (opp.isNaN()) return (one |= fNaN);
      if (opp.isInfinity())
       {
         if (opp.sign()) return (one |= -M_PI_2);
         return (one |= M_PI_2);
       }

      one.lengthen(opp.getSeriesExtra());
      one |= M_1;

      Float currentFactor (one);
      currentFactor |= Float(3);

      Float copyOpp (one);
      copyOpp |= opp;
      copyOpp.abs(); //Sign will be restored later.
      int Series = copyOpp.compare(one); //This is why atan is a friend.

      Float res (copyOpp);
      Float currentPower (copyOpp);
      copyOpp *= copyOpp;
      currentPower *= copyOpp;

      Float temp (one);
      temp |= M_PI_2;

      if (Series == 0)
       {
         //Both of the following series can compute this,
         //but it takes FOREVER!
         res |= M_PI_4;
       }
      else if (Series > 0)
       {
         bool alt = false;
         res = one / res;
         res = temp - res;

         temp = one / (currentPower * currentFactor);
         while (temp && res)
          {
            if (alt) res -= temp;
            else res += temp;
            alt = !alt;

               //Well, we either make a two, wasting memory,
               //or we do this, which is questionable.
            one.Significand.setDigit(0, 2);
            currentFactor += one;

            currentPower *= copyOpp;

            one.Significand.setDigit(0, 1);
            temp = one / (currentPower * currentFactor);
          }
       }
      else
       {
         bool alt = true;
         one.Significand.setDigit(0, 2);
         temp = currentPower / currentFactor;
         while (temp && res)
          {
            if (alt) res -= temp;
            else res += temp;
            alt = !alt;

            currentFactor += one; //which is now 2

            currentPower *= copyOpp;

            temp = currentPower / currentFactor;
          }
       }

      if(opp.sign()) res.negate(); //restore sign

      res.roundAt(opp.length());
      res.truncate(opp.getSeriesExtra());
      return res;
    }

/*
==============================================================================
   Function: principle inverse sine
------------------------------------------------------------------------------
   NOTES:
      asin(x) = atan(x / sqrt(1 - x ^ 2))
==============================================================================
*/
   Float asin (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isInfinity()) return (one |= fNaN);

      one.lengthen(opp.getDerivedExtra());
      one |= M_1;

      Float copyOpp (one);
      copyOpp |= opp;

      if (copyOpp == one)
       {
         Float res (opp);
         res |= M_PI_2;
         return res;
       }
      if (copyOpp == -one)
       {
         Float res (opp);
         res |= -M_PI_2;
         return res;
       }

      Float res (one);

      res = atan(copyOpp / sqrt(one - (copyOpp * copyOpp)));

      res.roundAt(opp.length());
      res.truncate(opp.getDerivedExtra());
      return res;
    }

/*
==============================================================================
   Function: principle inverse cosine
------------------------------------------------------------------------------
   NOTES:
      acos(x) = atan(sqrt(1 - x ^ 2) / x)
==============================================================================
*/
   Float acos (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isInfinity()) return (one |= fNaN);

      one.lengthen(opp.getDerivedExtra());
      one |= M_1;

      Float copyOpp (one);
      copyOpp |= opp;

         //This is necessary, or acos(-0) would return (-pi/2)
      if (copyOpp.isZero()) copyOpp.abs();

      if (copyOpp == one)
       {
         Float res (opp);
         res |= Float(0);
         return res;
       }
      if (copyOpp == -one)
       {
         Float res (opp);
         res |= M_PI;
         return res;
       }

      Float res (one);

      res = atan(sqrt(one - (copyOpp * copyOpp)) / copyOpp);

      res.roundAt(opp.length());
      res.truncate(opp.getDerivedExtra());
      return res;
    }

/*
==============================================================================
   Function: common (base 10) logarithm
------------------------------------------------------------------------------
   NOTES:
      log10(x) = log(x) / log(10) = log10(e) * log(x)
==============================================================================
*/
   Float log10 (const Float & opp)
    {
      Float top (opp);
      top.lengthen(opp.getDerivedExtra());

      Float bottom (top);
      bottom |= M_LOG10E;

      top = bottom * log(top);

      top.roundAt(opp.length());
      top.truncate(opp.getDerivedExtra());
      return top;
    }

/*
==============================================================================
   Function: ten to a real power
------------------------------------------------------------------------------
   NOTES:
      a ^ u = e ^ (u * log(a))
         so
      10 ^ x = e ^ (x * log(10))

      I like the name alog: antilogrithm.
      The name may be antiquated, but it gets to the point and is,
      in my opinion, more self-descriptive than pow and better looking
      than exp10 or pow10.

      Funny side note: as of March 2010, the help files for gcalctool,
      the GNOME calculator, states that it does not support antilogarithms.
      The calculator has 10 ^ x and e ^ x buttons, however, so only the
      mathematically disinclined are fooled.
==============================================================================
*/
   Float exp10 (const Float & opp)
    {
      Float copyOpp (opp);
      copyOpp.lengthen(opp.getDerivedExtra());

      Float multiplier (copyOpp);
      multiplier |= M_LN10;

      copyOpp = exp(copyOpp * multiplier);

      copyOpp.roundAt(opp.length());
      copyOpp.truncate(opp.getDerivedExtra());
      return copyOpp;
    }

/*
==============================================================================
   Function: exponential function, Euler's constant to a real power, minus one
------------------------------------------------------------------------------
   NOTES:
      This is to, hopefully, improve the accuracy of sinh when |x| is small.

      Basically, I gutted all of the speed hacks, and made the series start
      at currentPower, instead of 1 + currentPower.

      sinh(x) = (exp(x) - exp(-x)) / 2 = ((exp(x) - 1) - (exp(-x) - 1)) / 2
==============================================================================
*/
   static Float expm1 (const Float & opp)
    {
      Float one (opp);

      one.lengthen(opp.getSeriesExtra() * 2);
      one |= M_1;

      Float currentFactorial (one);
      Float currentDivisor (one);
      Float currentPower (one);
      Float copyOpp (one);
      Float res (one);
      Float temp (one);

      currentFactorial |= Float(2);
      currentDivisor |= Float(2);
      currentPower |= opp;
      copyOpp |= opp;

      res = currentPower;
      currentPower *= copyOpp;

      temp = currentPower / currentDivisor;
      while (temp && res)
       {
         res += temp;
         currentFactorial += one;
         currentDivisor *= currentFactorial;
         currentPower *= copyOpp;
         temp = currentPower / currentDivisor;
       }

      res.roundAt(opp.length());
      res.truncate(opp.getSeriesExtra() * 2);
      return res;
    }

/*
==============================================================================
   Function: hyperbolic sine
------------------------------------------------------------------------------
   NOTES:
      sinh(x) = (exp(x) - exp(-x)) / 2
      Should be faster than series now due to optimizations in exp.
==============================================================================
*/
   Float sinh (const Float & opp)
    {
      Float copyOpp (opp);

      if (opp.isZero() || opp.isNaN() || opp.isInfinity()) return opp;

      copyOpp.lengthen(opp.getDerivedExtra());

      if (opp.exponent() > -4) //half on right
         copyOpp = (exp(copyOpp) - exp(-copyOpp)) * Float(".5");
      else
         copyOpp = (expm1(copyOpp) - expm1(-copyOpp)) * Float(".5");

      copyOpp.roundAt(opp.length());
      copyOpp.truncate(opp.getDerivedExtra());
      return copyOpp;
    }

/*
==============================================================================
   Function: hyperbolic cosine
------------------------------------------------------------------------------
   NOTES:
      cosh(x) = (exp(x) + exp(-x)) / 2
      Should be faster than series now due to optimizations in exp.
==============================================================================
*/
   Float cosh (const Float & opp)
    {
      Float copyOpp (opp);

      if (opp.isNaN() || opp.isInfinity()) return opp;

      copyOpp.lengthen(opp.getDerivedExtra());

      copyOpp = (exp(copyOpp) + exp(-copyOpp)) * Float(".5"); //half on right

      copyOpp.roundAt(opp.length());
      copyOpp.truncate(opp.getDerivedExtra());
      return copyOpp;
    }

/*
==============================================================================
   Function: hyperbolic tangent
------------------------------------------------------------------------------
   NOTES:
      tanh(x) = sinh(x) / cosh(x) , just like the circular functions.
==============================================================================
*/
   Float tanh (const Float & opp)
    {
      Float sine (opp);

      if (opp.isNaN()) return opp;
      if (opp.isInfinity())
       {
         if (opp.sign()) return (sine |= -M_1);
         return (sine |= M_1);
       }

      sine.lengthen(opp.getDerivedExtra());

      sine = sinh(sine) / cosh(sine);

      sine.roundAt(opp.length());
      sine.truncate(opp.getDerivedExtra());
      return sine;
    }

/*
==============================================================================
   Function: inverse hyperbolic tangent
------------------------------------------------------------------------------
   NOTES:
      atanh(x) = 1/2 * log((1 + x) / (1 - x))

      I had a series for this, but it was too slow.
      And when this went to an ln definition, it made no sense to
      keep defining asinh and acosh with atanh: they went to ln
      definitions, too.
      The ln definition uses magic to converge faster.
      The atanh definition was a specialized ln definition in (1,-1),
      and its mathematically equivalent series was slower, so I didn't
      use it in ln, and I didn't use it here.
==============================================================================
*/
   Float atanh (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isZero()) return opp;
      if (opp.isInfinity()) return (one |= fNaN);

      one.lengthen(opp.getDerivedExtra());
      one |= M_1;

      Float copyOpp (one);
      copyOpp |= opp;

      copyOpp = log((one + copyOpp) / (one - copyOpp)) * Float(".5");

      copyOpp.roundAt(opp.length());
      copyOpp.truncate(opp.getDerivedExtra());
      return copyOpp;
    }

/*
==============================================================================
   Function: inverse hyperbolic sine
------------------------------------------------------------------------------
   NOTES:
      asinh(x) = log(x + sqrt(x^2 + 1))
==============================================================================
*/
   Float asinh (const Float & opp)
    {
      Float one (opp);

      if (opp.isNaN() || opp.isZero() || opp.isInfinity()) return opp;

      one.lengthen(opp.getDerivedExtra());
      one |= M_1;

      Float copyOpp (one);
      copyOpp |= opp;

      copyOpp = log(copyOpp + sqrt(copyOpp * copyOpp + one));

      copyOpp.roundAt(opp.length());
      copyOpp.truncate(opp.getDerivedExtra());
      return copyOpp;
    }

/*
==============================================================================
   Function: inverse hyperbolic cosine
------------------------------------------------------------------------------
   NOTES:
      acosh(x) = log(x +- sqrt(x^2 - 1))
      We use only the positive form here.
==============================================================================
*/
   Float acosh (const Float & opp)
    {
      Float one (opp);

      if (opp.sign() || opp.isZero()) return (one |= fNaN);
      if (opp.isNaN() || opp.isInfinity()) return opp;

      one.lengthen(opp.getDerivedExtra());
      one |= M_1;

      Float copyOpp (one);
      copyOpp |= opp;

      copyOpp = log (copyOpp + sqrt(copyOpp * copyOpp - one));

      copyOpp.roundAt(opp.length());
      copyOpp.truncate(opp.getDerivedExtra());
      return copyOpp;
    }

/*
==============================================================================
   Function: two argument inverse tangent
------------------------------------------------------------------------------
   NOTES:
      The arctangent of top / bottom, situated in the correct quadrant.
      Returns a value in [-pi, pi].

      Written to comply with, as best as possible, the description
      of the fpatan instruction in the _Intel 64 and IA-32 Architectures
      Software Developers' Manual: Volume 2A: Instruction Set Reference
      A-M_, order number 253666. I write as best as possible, as there is
      an undefined symbol (p) in the table that I guess stands for pi,
      in my pdf copy of the text aforementioned.

         My interpretation of the results table:
                -Inf    -X    -0    +0    +X  +Inf
         -Inf -3pi/4 -pi/2 -pi/2 -pi/2 -pi/2 -pi/4
           -X    -pi     * -pi/2 -pi/2     *    -0
           -0    -pi   -pi   -pi    -0    -0    -0
           +0    +pi   +pi   +pi    +0    +0    +0
           +X    +pi     * +pi/2 +pi/2     *    +0
         +Inf +3pi/4 +pi/2 +pi/2 +pi/2 +pi/2 +pi/4

            **top is rows, bottom is columns
      Of the 36 entries on this table, only the four marked * will
      actually be computed.
==============================================================================
*/
   Float atan2 (const Float & top, const Float & bottom)
    {
      Float one (top), two (bottom);
      one || two;

      if (top.isNaN() || bottom.isNaN()) return (one |= fNaN);

      if (top.isInfinity())
       {
         if (bottom.isInfinity())
          {
            one |= M_PI_4;
            if (bottom.sign())
             {
               two |= Float(3);
               one *= two;
             }
            if (top.sign()) one.negate();
            return one;
          }
         else
          {
            one |= M_PI_2;
            if (top.sign()) one.negate();
            return one;
          }
       }  //twelve down

      if (top.isZero())
       {
         if (bottom.sign()) one |= M_PI;
         one.abs();
         if (top.sign()) one.negate();
         return one;
       } //now twenty-four!

      if (bottom.isInfinity())
       {
         if (bottom.sign()) one |= M_PI;
         one.abs();
         if (top.sign()) one.negate();
         return one;
       } //twenty-eight

      if (bottom.isZero())
       {
         one |= M_PI_2;
         one.abs();
         if (top.sign()) one.negate();
         return one;
       } //All thirty-two precomputed entries.

      one.lengthen(top.getDerivedExtra());
      two.lengthen(top.getDerivedExtra());

      one = atan(one / two);

         //Now fix the result based on the signs of the opperands.
      if (bottom.sign())
       {
         two.Mode = one.Mode;
         two |= M_PI;
         if (top.sign()) two.negate();
         one += two;
       }

      one.roundAt(one.length() - top.getDerivedExtra());
      one.truncate(top.getDerivedExtra());
      return one;
    }

/*
==============================================================================
   Function: cube root
------------------------------------------------------------------------------
   NOTES:
      I thought that I'd derive this on-the-fly, but I realized that
      (-x) ^ (1/3) = -(x ^ (1/3)), and that e ^ (log(x) / 3) would return
      NaN, so this "wrapper" was written to ensure correct behavior.

      Consider that (-x) ^ n has real results only when n is an
      integer, or when n can be represented as a rational number with an
      odd denominator. None of the exponential functions here take this
      into account due to the added complexity.
==============================================================================
*/
   Float cbrt (const Float & opp)
    {
      Float temp (opp);

      temp.lengthen(opp.getDerivedExtra());
      temp.abs();

      temp = exp(log(temp) / Float("3"));

      if (opp.sign()) temp.negate();

      temp.roundAt(opp.length());
      temp.truncate(opp.getDerivedExtra());
      return temp;
    }

/*
==============================================================================
   Function: fused multiply and accumulate
==============================================================================
*/
   Float fma (const Float & left, const Float & right, const Float & add)
    {
      Float res (left), temp (right), dummy (add);
      res || temp;
      res || dummy;

      res.lengthen(res.length());

      res *= temp;
      res += dummy;

      res.roundAt(res.length() / 2);
      res.truncate(res.length() / 2);
      return res;
    }

/*
==============================================================================
   Function: factorial
------------------------------------------------------------------------------
   NOTES:
      Truncates input to integer
==============================================================================
*/
   Float fact (const Float & opp)
    {
      Float one (opp);

         //exponent > length or negative, then don't even try
      if ((opp.exponent() > (opp.length() + opp.getSeriesExtra())) ||
          opp.isNegative() || opp.isNaN())
         return (one |= fNaN);

      if (opp.isInfinity()) return opp;

      one.lengthen(one.getSeriesExtra());
      one |= M_1;

      Float curr (one), res (one);
      curr |= opp;
      curr.Int();

      while (!curr.isZero())
       {
         res *= curr;
         curr -= one;
       }

      res.roundAt(opp.length());
      res.truncate(opp.getSeriesExtra());
      return res;
    }

/*
==============================================================================
   Function: random number
------------------------------------------------------------------------------
   NOTES:
      Calls an optimized C function that does all of the math.
==============================================================================
*/

   Float rand (void)
    {
      static bool seeded = false;

      if (!seeded)
       {
         seeded = true;
         srand850(time(NULL));
       }

      Float x (std::string(rand850s()) + "e-256");
      x.Normalize();

      return x;
    }

 } //namespace DecFloat
