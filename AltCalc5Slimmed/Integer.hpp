/*
Copyright (c) 2010, 2011 Thomas DiModica.
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

/*
   An arbitrary precision integer class.

   The only issues to note are that fromString() depends on ASCII.
   [ASCII stupid question, get a stupid ANSI]
*/

#ifndef INTEGER_HPP
#define INTEGER_HPP

#include <string>
#include "BitField.hpp"

namespace BigInt
 {

   class Integer
    {

      private:
         BitField Digits;
         bool Sign;

         static Integer adder (const Integer &, const Integer &);

      public:
         Integer ();
         Integer (long long);
         Integer (Unit);
         Integer (long);
         Integer (int);
         Integer (const Integer &);
         Integer (const std::string &, int base = 10);
         Integer (const char *, int base = 10);
         ~Integer ();

         bool isSigned (void) const { return Sign; }
         bool isZero (void) const { return Digits.isZero(); }
         bool isOdd (void) const;
         bool isEven (void) const;

         std::string toString (int base = 10) const;

         void fromString (const std::string &, int base = 10);
         void fromString (const char *, int base = 10);

         Integer & negate (void);
         Integer & abs (void);
         Integer & setSign (bool);

         friend Integer operator + (const Integer &, const Integer &);
         friend Integer operator - (const Integer &, const Integer &);
         friend Integer operator * (const Integer &, const Integer &);

         Integer & operator = (const Integer &);

         Integer & operator += (const Integer &);
         Integer & operator -= (const Integer &);
         Integer & operator *= (const Integer &);

         Integer operator - (void) const;

         long toInt (void) const; //Not perfect, but not terrible.
         long msb (void) const { return Digits.msb(); }
         Unit getDigit (long digit) const { return Digits.getDigit(digit); }

         int compare (const Integer &) const;

          /*
            Note: Aliased Behavior:
               If any combination of inputs are aliased, the result will be
               correct. If quotient and remainder are aliased, however, the
               remainder will be stored in both of them.
          */
         static void divmod (const Integer & dividend,
                             const Integer & divisor,
                                   Integer & quotient,
                                   Integer & remainder);

         friend Integer pow (const Integer &, const Integer &);

    }; /* class Integer */

   Integer operator + (const Integer &, const Integer &);
   Integer operator - (const Integer &, const Integer &);
   Integer operator * (const Integer &, const Integer &);
   Integer pow (const Integer &, const Integer &);

 } /* namespace BigInt */

#endif /* INTEGER_HPP */
