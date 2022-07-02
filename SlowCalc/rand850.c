/*
   rand850: a linear congruential pseudo-random number generator with
      approximately 850 bits of state.

   Copyright (C) 2010 Thomas DiModica <ricinwich@yahoo.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
   File: rand850.c
      This is a random number generator that generates pseudo-random numbers
      on a period 10 ^ 256 using a linear congruential generator.

      It has two invocations: rand850s, which returns the digits as a string,
      and rand850ul, which returns an unsigned long.

      rand850s DEPENDS on a little-endian architecture and ASCII encoding,
      basically Intel.

      To get a standard long from rand850ul, bitshift it right one (>> 1).
      rand850ul SHOULD have a period of about 2 ^ 820 (~ 10 ^ 246).

      This was originally written to be part of a C++ project, so it is
      already wrappered to be compiled by either a C or C++ compiler.
*/

//HEADER:
// rand850tables has the HUGE (2 x 10k) multiplication tables.
#include "rand850tables.h"

#ifdef __cplusplus
extern "C"
 {
#endif

static TYPE state [STATE_LEN];

static TYPE constant_a [STATE_LEN] =
 {
// 4 * 5 * 32771 ^ 53 * 65537 ^ 3 + 1
11,78,80,90,16,74,37,92,
36, 10, 14, 53, 31, 1, 82, 38, 1, 5, 62, 35, 10, 10, 55, 59, 17, 65, 72, 62,
48, 32, 16, 23, 69, 83, 40, 85, 23, 38, 55, 21, 48, 45, 76, 50, 64, 36, 41, 44,
63, 38, 85, 31, 66, 77, 4, 87, 58, 2, 36, 97, 1, 44, 38, 32, 38, 20, 68, 93,
95, 98, 89, 99, 47, 10, 39, 18, 30, 59, 66, 13, 41, 84, 66, 73, 70, 8, 89, 37,
96, 35, 57, 23, 34, 70, 39, 33, 98, 21, 37, 73, 36, 68, 39, 40, 73, 67, 11, 61,
47, 60, 61, 43, 91, 8, 60, 23, 72, 61, 6, 20, 13, 5, 23, 72, 89, 7, 36, 61

/*
// 4 * 5 * 32771 ^ 53 + 1
// Under this formula, only the lowest bits of the first value are effected by
// the initial seed, the high order bits always are all the same upon seeding.
// So I killed it.
0, 0, 0, 0, 0, 0, 0, 4,
18, 77, 79, 21, 42, 34, 88, 39, 91, 95, 56, 38, 95, 5, 58, 22, 64, 96, 57, 37,
20, 49, 59, 24, 51, 18, 77, 29, 73, 12, 88, 35, 83, 23, 99, 91, 46, 99, 8, 39,
38, 10, 98, 51, 81, 57, 45, 12, 89, 24, 99, 49, 19, 76, 25, 59, 9, 68, 52, 37,
71, 1, 3, 95, 3, 1, 60, 89, 85, 51, 71, 29, 97, 29, 10, 93, 95, 86, 28, 15,
30, 51, 21, 75, 61, 96, 62, 96, 62, 71, 2, 58, 0, 61, 96, 59, 69, 37, 3, 58,
45, 52, 80, 38, 60, 19, 90, 1, 11, 68, 64, 46, 30, 65, 81, 40, 36, 2, 2, 21
*/
 };

static TYPE constant_c [STATE_LEN] =
 {
18, 78, 39, 76, 71, 44, 41, 90,
49, 20, 7, 52, 93, 40, 2, 53, 38, 98, 56, 19, 19, 52, 62, 8, 43, 61, 87, 11,
26, 26, 5, 90, 59, 35, 53, 42, 40, 35, 69, 57, 56, 3, 91, 11, 97, 6, 29, 49,
74, 63, 8, 47, 36, 44, 28, 72, 6, 41, 81, 76, 38, 52, 64, 42, 41, 62, 50, 15,
4, 18, 43, 72, 48, 70, 67, 14, 65, 90, 4, 33, 0, 55, 8, 60, 29, 96, 39, 2,
66, 69, 77, 74, 37, 7, 94, 93, 91, 93, 89, 64, 28, 70, 66, 58, 0, 94, 89, 49,
0, 66, 19, 29, 53, 20, 90, 43, 77, 40, 17, 99, 38, 6, 68, 5, 20, 19, 60, 97
 };

static char retbuf [STATE_LEN * MULT + 1];

/*
==============================================================================
   Function: seed random number generator
------------------------------------------------------------------------------
   NOTES:
      There aren't enough degrees of freedom from this random seed, however
      I don't believe that there is a good way to generate a seed that has
      sufficient freedom.
      Basically, upon seeding, you will only see one of four billion initial
      sequences. The only way this would be a problem is if you want EVERY
      possible shuffle of a deck of cards to be equiprobable upon seeding.
      As it is now, only about 10^9 of 10^67 shuffles will show up UPON
      SEEDING. I don't know enough math to make predictions upon shuffles
      after the first.
==============================================================================
*/
void srand850 (unsigned long a)
 {
   int i;
   for (i = 0; i < STATE_LEN; i++) state[i] = 0;
   i = STATE_LEN;
   while (a) { state[--i] = a % BASE; a /= BASE; }
   retbuf[STATE_LEN * MULT] = '\0';
 }

/*
==============================================================================
   Function: add
------------------------------------------------------------------------------
   NOTES:
      This is an even more optimized addition function.
      More optimized, that is, than the addition routine in SlowCalc.
      This, however, only operates on even numbers of digits.
==============================================================================
*/
static void add (TYPE * a, TYPE * b, int l)
 {
   int c, i;
   for (c = 0, i = l - 1; i >= 0; i--)
    {
      a[i] += b[i] + c;
      if (a[i] > (BASE - 1))
       {
         a[i] -= BASE;
         c = 1;
       }
      else c = 0;
    }
 }

/*
==============================================================================
   Function: multiply
------------------------------------------------------------------------------
   NOTES:
      This is an even more optimized multiplication function.
      More optimized, that is, than the multiplication routine in SlowCalc.
==============================================================================
*/
static void mult (TYPE * a, TYPE * b, int c)
 {
   int i, j, l;
   TYPE x [LONG_LEN], y [LONG_LEN], z [LONG_LEN], * s;

   s = a;
   l = c;
   y[l - 1] = 0;

   for (i = 0; i < c; i++) z[i] = 0;

   for (i = c - 1; i > 0; i--)
    {
      if (b[i] != 0)
       {
         x[0] = mult_low[a[0]][b[i]];
         for (j = 1; j < l; j++)
          {
            x[j] = mult_low[a[j]][b[i]];
            y[j - 1] = mult_high[a[j]][b[i]];
          }

         add(z, x, c);
         add(z, y, c);
       }

      l--;
      x[l] = 0;
      y[l - 1] = 0;
      a++;
    }

   if ((b[0] != 0) && (a[0] != 0))
    {
      x[0] = mult_low[a[0]][b[0]];
      add(z, x, c);
    }

   for (i = 0; i < c; i++) s[i] = z[i];
 }


/*
==============================================================================
   Function: random number string
------------------------------------------------------------------------------
   NOTES:
      From Knuth, TAOCP Vol. 2:
         x = (a * x + c) mod m
         c MUST be relatively prime to m
         a - 1 MUST be divisable by the prime factors of m
         a - 1 MUST be divisable by 4, if m is divisable by 4
      So:
         a = 4 * 5 * 32771 ^ 53 * 65537 ^ 3 + 1
         c = 65537 ^ 53
         m = 10 ^ 256 (which means we have about 850 bits of state)

         c and m are relatively prime, as 65537 is prime.
==============================================================================
*/
char * rand850s (void)
 {
   int i;
   short * s;
   s = (short *) retbuf;

   mult(state, constant_a, STATE_LEN);
   add(state, constant_c, STATE_LEN);

   for (i = 0; i < STATE_LEN; i++) s[i] = conv_table[state[i]];

   return retbuf;
 }


#ifdef __cplusplus
 }
#endif
