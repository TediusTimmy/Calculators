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

#include "Integer.hpp"
#include <gmp.h>
#include <cstdlib>

namespace BigInt
 {

   class DataHolder
    {
      private:
         mutable long Refs;
//mutable Mutex

      public:
         mpz_t Data;

         DataHolder () : Refs(1)
          {
//Initialize Mutex
            mpz_init(Data);
          }

         DataHolder (unsigned long src) : Refs(1)
          {
//Initialize Mutex
            mpz_init_set_ui(Data, src);
          }

         DataHolder (const DataHolder & src) : Refs(1)
          {
//Initialize Mutex
            mpz_init_set(Data, src.Data);
          }

         DataHolder (const char * src, int base) : Refs(1)
          {
//Initialize Mutex
            mpz_init_set_str(Data, src, base);
          }

         void ref (void) const
          {
//Lock Mutex
            Refs++;
//Unlock Mutex
          }

         void deref (void)
          {
//Lock Mutex
            long res = --Refs;
//Unlock Mutex
            if (res == 0)
             {
//Finalize Mutex
               mpz_clear(Data);
               delete this;
             }
          }

         DataHolder * own (void)
          {
            DataHolder * ret = this;
//Lock Mutex
            if (Refs != 1)
             {
               --Refs;
               ret = new DataHolder(*this);
             }
//Unlock Mutex
            return ret;
          }
    };

   Integer::Integer () : Data (NULL), Sign (false) { }

   Integer::Integer (long input) : Data (NULL), Sign (false)
    {
      if (input != 0)
       {
         if (input < 0)
          {
            Sign = true;
            input = -input;
          }

         Data = new DataHolder ((unsigned long)input);
       }
    }

   Integer::Integer (const std::string & input, int base) :
      Data (NULL), Sign (false)
    {
      fromString(input.c_str(), base);
    }

   Integer::Integer (const char * input, int base) : Data (NULL), Sign (false)
    {
      fromString(input, base);
    }

   Integer::Integer (const Integer & input) :
      Data (input.Data), Sign (input.Sign)
    {
      if (!isZero()) Data->ref();
    }

   Integer::~Integer ()
    {
      if (!isZero()) Data->deref();
      Data = NULL;
      Sign = false;
    }



   bool Integer::isOdd (void) const
    {
      return isZero() ? false : mpz_odd_p(Data->Data);
    }

   bool Integer::isEven (void) const
    {
      return isZero() ? true : mpz_even_p(Data->Data);
    }



   long Integer::toInt (void) const //It works for its purpose.
    {
      if (isZero() || (0 == mpz_fits_sint_p(Data->Data))) return 0;
      return Sign ? -mpz_get_si(Data->Data) : mpz_get_si(Data->Data);
    }



   Integer & Integer::negate (void)
    {
      if (isZero()) Sign = false;
      else Sign = !Sign;
      return *this;
    }

   Integer & Integer::abs (void)
    {
      Sign = false;
      return *this;
    }

   Integer & Integer::copySign (const Integer & src)
    {
      if (isZero()) Sign = false;
      else Sign = src.Sign;
      return *this;
    }

   Integer & Integer::setSign (bool newsign)
    {
      if (isZero()) Sign = false;
      else Sign = newsign;
      return *this;
    }



   Integer Integer::operator - (void) const
    {
      Integer returnedInteger(*this);
      return returnedInteger.negate();
    }



   Integer & Integer::operator = (const Integer & src)
    {
      if (&src == this) return *this;
      if (!isZero()) Data->deref();
      Data = src.Data;
      Sign = src.Sign;
      if (!isZero()) Data->ref();
      return *this;
    }



   int Integer::compare (const Integer & to) const
    {
       /*
         Positive or zero is greater than negative.
         After this test, we know that both numbers have the same sign.
       */
      if (Sign != to.Sign)
       {
         if (Sign) return -1;
         return 1;
       }

       /*
         Test for zeros, which are always positive.
       */
      if (!Sign)
       {
         if (isZero())
          {
             // Both are Zero.
            if (to.isZero()) return 0;
             // I'm Zero, he isn't.
            return -1;
         }
        if (to.isZero())
         {
            // I'm not zero.
            return 1;
         }
       }

       /*
         Return the unsigned comparison, remembering that if we are
         negative, then the result is negated.
       */
      if (Sign) return -mpz_cmp(Data->Data, to.Data->Data);
      return mpz_cmp(Data->Data, to.Data->Data);
    }



   bool operator > (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) > 0;
    }

   bool operator < (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) < 0;
    }

   bool operator >= (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) >= 0;
    }

   bool operator <= (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) <= 0;
    }

   bool operator == (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) == 0;
    }

   bool operator != (const Integer & lhs, const Integer & rhs)
    {
      return lhs.compare(rhs) != 0;
    }



   Integer & Integer::operator += (const Integer & rval)
    {
      if (rval.isZero())
         return *this;

      if (isZero())
       {
         Data = rval.Data;
         Sign = rval.Sign;
         Data->ref();
         return *this;
       }

      Data = Data->own();

      if (Sign == rval.Sign) mpz_add(Data->Data, Data->Data, rval.Data->Data);
      else mpz_sub(Data->Data, Data->Data, rval.Data->Data);

      int sign = mpz_sgn(Data->Data);
      switch (sign)
       {
         case -1:
            Sign = !Sign;
            mpz_abs(Data->Data, Data->Data);
            break;

         case 0:
            Sign = false;
            Data->deref();
            Data = NULL;
            break;

         default:
            break;
       }

      return *this;
    }

   Integer & Integer::operator -= (const Integer & rval)
    {
      if (rval.isZero())
         return *this;

      if (isZero())
       {
         Data = rval.Data;
         Sign = !rval.Sign;
         Data->ref();
         return *this;
       }

      Data = Data->own();

      if (Sign == rval.Sign) mpz_sub(Data->Data, Data->Data, rval.Data->Data);
      else mpz_add(Data->Data, Data->Data, rval.Data->Data);

      int sign = mpz_sgn(Data->Data);
      switch (sign)
       {
         case -1:
            Sign = !Sign;
            mpz_abs(Data->Data, Data->Data);
            break;

         case 0:
            Sign = false;
            Data->deref();
            Data = NULL;
            break;

         default:
            break;
       }

      return *this;
    }

   Integer & Integer::operator *= (const Integer & rval)
    {
      if (isZero())
         return *this;

      if (rval.isZero())
       {
         Data->deref();
         Data = NULL;
         Sign = false;
         return *this;
       }

      Data = Data->own();

      mpz_mul(Data->Data, Data->Data, rval.Data->Data);

      Sign = Sign ^ rval.Sign;

      return *this;
    }

   Integer & Integer::operator /= (const Integer & rval)
    {
      if (isZero())
         return *this;

      if (rval.isZero())
       {
         Data->deref();
         Data = NULL;
         Sign = false;
         return *this;
       }

      Data = Data->own();

      mpz_tdiv_q(Data->Data, Data->Data, rval.Data->Data);

      int sign = mpz_sgn(Data->Data);
      switch (sign)
       {
         case 0:
            Sign = false;
            Data->deref();
            Data = NULL;
            break;

         default:
            Sign = Sign ^ rval.Sign;
            break;
       }

      return *this;
    }

   Integer & Integer::operator %= (const Integer & rval)
    {
      if (isZero() || rval.isZero())
         return *this;

      Data = Data->own();

      mpz_tdiv_r(Data->Data, Data->Data, rval.Data->Data);

      int sign = mpz_sgn(Data->Data);
      switch (sign)
       {
         case 0:
            Sign = false;
            Data->deref();
            Data = NULL;
            break;

         default:
            break;
       }

      return *this;
    }



   Integer operator + (const Integer & lhs, const Integer & rhs)
    {
      Integer result (lhs);
      return (result += rhs);
    }

   Integer operator - (const Integer & lhs, const Integer & rhs)
    {
      Integer result (lhs);
      return (result -= rhs);
    }

   Integer operator * (const Integer & lhs, const Integer & rhs)
    {
      Integer result (lhs);
      return (result *= rhs);
    }

   Integer operator / (const Integer & lhs, const Integer & rhs)
    {
      Integer result (lhs);
      return (result /= rhs);
    }

   Integer operator % (const Integer & lhs, const Integer & rhs)
    {
      Integer result (lhs);
      return (result %= rhs);
    }



   Integer Integer::operator++ ()
    {
      return (*this += (Integer(1)));
    }

   Integer Integer::operator++ (int)
    {
      Integer ret (*this);
      *this += (Integer(1));
      return ret;
    }

   Integer Integer::operator-- ()
    {
      return (*this -= (Integer(1)));
    }

   Integer Integer::operator-- (int)
    {
      Integer ret (*this);
      *this -= (Integer(1));
      return ret;
    }



   void Integer::fromString (const std::string & src, int base)
    {
      fromString(src.c_str(), base);
    }

   void Integer::fromString (const char * src, int base)
    {
      if (!isZero())
       {
         Data->deref();
         Data = NULL;
         Sign = false;
       }

      Data = new DataHolder(src, base);

      int sign = mpz_sgn(Data->Data);
      switch (sign)
       {
         case -1:
            Sign = !Sign;
            mpz_abs(Data->Data, Data->Data);
            break;

         case 0:
            Sign = false;
            Data->deref();
            Data = NULL;
            break;

         default:
            break;
       }
    }



   std::string Integer::toString (int base) const
    {
      std::string result;

      if (isZero()) return std::string("0");
      if (isSigned()) result = "-";

      char * rstring = mpz_get_str(NULL, base, Data->Data);
      result += rstring;
      std::free(rstring);

      return result;
    }



   void quotrem (const Integer & lhs, const Integer & rhs,
                       Integer & q, Integer & r)
    {
      if (&q == &r)
       {
         r = lhs % rhs;
         return;
       }

      if (lhs.isZero())
       {
         q = Integer();
         r = Integer();
         return;
       }
      if (rhs.isZero())
       {
         r = lhs;
         q = Integer();
         return;
       }

      bool
         qSign = lhs.isSigned() ^ rhs.isSigned(),
         rSign = lhs.isSigned();
      DataHolder * quot, * rem;

      quot = new DataHolder();
      rem = new DataHolder();

      mpz_tdiv_qr(quot->Data, rem->Data, lhs.Data->Data, rhs.Data->Data);

      q = Integer();
      r = Integer();

      int sign = mpz_sgn(quot->Data);
      switch (sign)
       {
         case 0:
            quot->deref();
            break;

         default:
            q.Data = quot;
            q.Sign = qSign;
            break;
       }

      sign = mpz_sgn(rem->Data);
      switch (sign)
       {
         case 0:
            rem->deref();
            break;

         default:
            r.Data = rem;
            r.Sign = rSign;
            break;
       }
    }



   Integer pow (const Integer & lhs, const Integer & rhs)
    {
      Integer result, one;

      one = Integer(1);

      if (lhs == one)
       {
         result = lhs;
         return result;
       }

      if (rhs.isSigned() || lhs.isZero()) return result;

      if (rhs.isZero()) return one;

      if (0 == mpz_fits_ulong_p(rhs.Data->Data)) return result;

      result.Sign = lhs.isSigned() & rhs.isOdd();
      result.Data = new DataHolder();

      mpz_pow_ui(result.Data->Data, lhs.Data->Data, mpz_get_ui(rhs.Data->Data));

      return result;
    }



    /*
      Find the truncated square root via Newton's Method.
    */
   Integer sqrt (const Integer & src)
    {
      Integer temp, last;

       // Range errors? My stove's fine.
      if (src.isSigned() || src.isZero()) return last;

      temp.Data = new DataHolder();
      mpz_sqrt(temp.Data->Data, src.Data->Data);

      if ((temp * temp) > src) temp--;

      return temp;
    }



    /*
      Factorial. Return 0 on negative arg.
    */
   Integer fact (const Integer & ofThis)
    {
      Integer result;

      if (ofThis.isSigned()) return result;

       // Don't handle numbers > 2^32
      if (0 == mpz_fits_ulong_p(ofThis.Data->Data))
         return result;

      result.Data = new DataHolder();

      mpz_fac_ui(result.Data->Data, mpz_get_ui(ofThis.Data->Data));

      return result;
    }

   Integer permutation (const Integer & items, const Integer & taken)
    {
      Integer i (items), limit (items - taken);

      if (items.isSigned() || taken.isSigned() || limit.isSigned())
         return Integer();

      return fact(items) / fact(limit);
    }

   Integer combination (const Integer & items, const Integer & taken)
    {
      Integer p;

      p = permutation(items, taken);

      if (p.isZero()) return p;

      return p / fact (taken);
    }

   Integer fib (const Integer & number)
    {
      Integer result;

      if (number.isSigned() || number.isZero())
         return result;

       // Don't handle numbers > 2^32
      if (0 == mpz_fits_ulong_p(number.Data->Data))
         return result;

      result.Data = new DataHolder();

      mpz_fib_ui(result.Data->Data, mpz_get_ui(number.Data->Data));

      return result;
    }

    /*
      I put these in the header and the compiler exploded.
    */
   Integer abs (const Integer & of)
      { return (of.isSigned()) ? -of: of; }
   Integer min (const Integer & a, const Integer & b)
      { return (b < a) ? b : a; }
   Integer max (const Integer & a, const Integer & b)
      { return (a < b) ? b : a; }

    /*
      Mathematically proper division.
    */
   Integer div (const Integer & lhs, const Integer & rhs)
    {
      Integer result, remainder;
      quotrem(lhs, rhs, result, remainder);

       /*
          By the Division Algorithm: 0 <= remainder < |divisor|
          So, if the remainder is negative, we increment the magnitude of the
          quotient.
          We only break  0 <= remainder < |divisor|  when the divisor is zero.
          Then the quotient is zero, and the remainder is the dividend,
          retaining the relation:
             divident = divisor * quotient + remainder
       */
      if (remainder.isSigned() && !(rhs.isZero()))
       {
         if (lhs.isSigned() != rhs.isSigned()) result--;
         else result++;
       }

      return result;
    }

    /*
      Mathematically proper modulo.
    */
   Integer mod (const Integer & lhs, const Integer & rhs)
    {
      Integer result;

      result = lhs % rhs;

       /*
          By the Division Algorithm: 0 <= remainder < |divisor|
          So, if this is negative, we reinterpret the result as its positive
          value mod the absolute value of the divisor.
       */
      if (result.isSigned())
       {
         if (rhs.isSigned()) result -= rhs;
         else result += rhs;
       }

      return result;
    }

   void divmod (const Integer & lhs, const Integer & rhs,
      Integer & quot, Integer & rem)
    {
      Integer Rhs (rhs);
      bool lhsS;

      lhsS = lhs.isSigned();

      quotrem(lhs, rhs, quot, rem);


      if (rem.isSigned() && !(Rhs.isZero()))
       {
         if (lhsS != Rhs.isSigned()) quot--;
         else quot++;
       }

      if (rem.isSigned())
       {
         if (Rhs.isSigned()) rem -= Rhs;
         else rem += Rhs;
       }

      return;
    }



 } /* namespace BigInt */
