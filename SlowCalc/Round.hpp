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
   File: Round.hpp
*/

#ifndef ROUND_HPP
#define ROUND_HPP

namespace Round
 {

   enum RoundMode
    {
         // Standard IEEE 754 Modes
      ROUND_TO_NEAREST_TIES_TO_EVEN,
      ROUND_TO_NEAREST_TIES_AWAY_FROM_ZERO, //IEEE 754-2008
      ROUND_TO_POSITIVE_INFINITY,
      ROUND_TO_NEGATIVE_INFINITY,
      ROUND_TO_ZERO,

         // Non-standard Modes, Implemented For Symmetry
      ROUND_TO_NEAREST_TIES_TO_ODD,
      ROUND_TO_NEAREST_TIES_TO_ZERO,
      ROUND_AWAY_FROM_ZERO
    }; //RoundMode

   enum RoundModeVariance
    {
      VARIANT,
      INVARIANT
    }; //RoundModeVariance

   class Rounder
    {
      private:
         RoundMode currentMode;
         RoundModeVariance currentVariance;

         static RoundMode globalRoundMode;
         static RoundMode defaultRoundMode;
         static RoundModeVariance defaultVariance;
         static RoundModeVariance globalVariance;
         static bool globalMode;

      public:
         RoundMode getRoundMode (void) const;
         RoundMode setRoundMode (RoundMode);
         RoundMode copyRoundMode (const Rounder &);

         RoundModeVariance getVariance (void) const;
         RoundModeVariance setVariance (RoundModeVariance);
         RoundModeVariance copyVariance (const Rounder &);

         static RoundMode getGlobalRoundMode (void);
         static RoundMode setGlobalRoundMode (RoundMode);
         static void clearGlobalRoundMode (void);

         static RoundMode getDefaultRoundMode (void);
         static RoundMode setDefaultRoundMode (RoundMode);

         static RoundModeVariance getDefaultRoundModeVariance (void);
         static RoundModeVariance setDefaultRoundModeVariance (RoundModeVariance);

         static RoundModeVariance getGlobalRoundModeVariance (void);
         static RoundModeVariance setGlobalRoundModeVariance (RoundModeVariance);

            // This function is the purpose of this class: decide a round based
            // on: the sign of the number, the value being rounded to, and the
            // value being rounded at. Returns true if the value being rounded to
            // needs to change, with an increment away from zero.
         bool decideRound (bool, int, int) const;

         Rounder ();
         Rounder (const Rounder &);
         Rounder (const Rounder &, RoundMode);
         Rounder (const Rounder &, RoundModeVariance);
         Rounder (RoundMode);
         Rounder (RoundModeVariance);
         Rounder (RoundMode, RoundModeVariance);

         Rounder & operator= (const Rounder &);
    }; //class Rounder

 } //namespace Round

#endif /* ROUND_HPP */
