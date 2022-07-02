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
   File: Round.cpp
      The jackhammer solution to rounding that I wrote when I needed a tack
      hammer.
*/

//HEADER:
// Round contains the definition of the class.
#include "Round.hpp"

namespace Round
 {

      /*
         Change these if you want the rounding system to behave differently
         by default.

         defaultRoundMode is the mode all new Rounders are given.
         globalRoundMode is the mode used if a Rounder is variant.

         globalVariance forces ALL calculations to use globalRoundMode.
            Use it sparingly and wisely.

         globalMode flags whether there is a global mode to be used or not.
      */
   RoundMode Rounder::globalRoundMode = ROUND_TO_NEAREST_TIES_TO_EVEN;
   RoundMode Rounder::defaultRoundMode = ROUND_TO_NEAREST_TIES_TO_EVEN;
   RoundModeVariance Rounder::globalVariance = VARIANT;
   RoundModeVariance Rounder::defaultVariance = VARIANT;
   bool Rounder::globalMode = true;

/*
==============================================================================
   Function: getDefaultRoundMode
==============================================================================
*/
   RoundMode Rounder::getDefaultRoundMode (void)
    { return defaultRoundMode; }

/*
==============================================================================
   Function: setDefaultRoundMode
==============================================================================
*/
   RoundMode Rounder::setDefaultRoundMode (RoundMode mode)
    { return (defaultRoundMode = mode); }

/*
==============================================================================
   Function: getDefaultRoundModeVariance
==============================================================================
*/
   RoundModeVariance Rounder::getDefaultRoundModeVariance (void)
    { return defaultVariance; }

/*
==============================================================================
   Function: setDefaultRoundModeVariance
==============================================================================
*/
   RoundModeVariance Rounder::setDefaultRoundModeVariance
      (RoundModeVariance variance)
    { return (defaultVariance = variance); }

/*
==============================================================================
   Function: getGlobalRoundModeVariance
==============================================================================
*/
   RoundModeVariance Rounder::getGlobalRoundModeVariance (void)
    { return globalVariance; }

/*
==============================================================================
   Function: setGlobalRoundModeVariance
==============================================================================
*/
   RoundModeVariance Rounder::setGlobalRoundModeVariance
      (RoundModeVariance variance)
    { return (globalVariance = variance); }

/*
==============================================================================
   Function: getGlobalRoundMode
==============================================================================
*/
   RoundMode Rounder::getGlobalRoundMode (void)
    { return globalRoundMode; }

/*
==============================================================================
   Function: setGlobalRoundMode
==============================================================================
*/
   RoundMode Rounder::setGlobalRoundMode (RoundMode mode)
    { globalMode = true; return (globalRoundMode = mode); }

/*
==============================================================================
   Function: clearGlobalRoundMode
------------------------------------------------------------------------------
   NOTES:
      Clears the globalMode flag to stop using the global round mode.
==============================================================================
*/
   void Rounder::clearGlobalRoundMode (void) { globalMode = false; return; }

/*
==============================================================================
   Function: getRoundMode
==============================================================================
*/
   RoundMode Rounder::getRoundMode (void) const
    { return currentMode; }

/*
==============================================================================
   Function: setRoundMode
==============================================================================
*/
   RoundMode Rounder::setRoundMode (RoundMode mode)
    { return (currentMode = mode); }

/*
==============================================================================
   Function: copyRoundMode
==============================================================================
*/
   RoundMode Rounder::copyRoundMode (const Rounder & ofThis)
    { return (currentMode = ofThis.currentMode); }

/*
==============================================================================
   Function: getVariance
==============================================================================
*/
   RoundModeVariance Rounder::getVariance (void) const
    { return currentVariance; }

/*
==============================================================================
   Function: setVariance
==============================================================================
*/
   RoundModeVariance Rounder::setVariance (RoundModeVariance variance)
    { return (currentVariance = variance); }

/*
==============================================================================
   Function: copyVariance
==============================================================================
*/
   RoundModeVariance Rounder::copyVariance (const Rounder & ofThis)
    { return (currentVariance = ofThis.currentVariance); }

/*
==============================================================================
   Function: decideRound
------------------------------------------------------------------------------
   OUTPUT:
      True if roundTo needs to be incremented away from zero.
      False if roundTo does not need to be incremented away from
         zero before truncating the digits from roundAt on.
==============================================================================
*/
   bool Rounder::decideRound (bool sign, int roundTo, int roundAt) const
    {
      RoundMode useMode;
      if ((globalMode && (currentVariance == VARIANT)) ||
          (globalVariance == INVARIANT))
         useMode = globalRoundMode;
      else useMode = currentMode;

      switch (useMode)
       {
         case ROUND_TO_NEAREST_TIES_TO_EVEN:
            if ((roundAt > 5) || ((roundAt == 5) && ((roundTo & 1) == 1)))
               return true;
            break;
         case ROUND_TO_NEAREST_TIES_AWAY_FROM_ZERO:
            if (roundAt > 4) return true;
            break;
         case ROUND_TO_POSITIVE_INFINITY:
            if (!sign && (roundAt != 0)) return true;
            break;
         case ROUND_TO_NEGATIVE_INFINITY:
            if (sign && (roundAt != 0)) return true;
            break;
         case ROUND_TO_ZERO:
            break;
         case ROUND_TO_NEAREST_TIES_TO_ODD:
            if ((roundAt > 5) || ((roundAt == 5) && ((roundTo & 1) == 0)))
               return true;
            break;
         case ROUND_TO_NEAREST_TIES_TO_ZERO:
            if (roundAt > 5) return true;
            break;
         case ROUND_AWAY_FROM_ZERO:
            if (roundAt != 0) return true;
            break;
       }

      return false;
    } //Rounder::decideRound

/*
==============================================================================
   Function: no argument constructor
==============================================================================
*/
   Rounder::Rounder ()
    {
      currentMode = defaultRoundMode;
      currentVariance = defaultVariance;
    }

/*
==============================================================================
   Function: copy constructor
==============================================================================
*/
   Rounder::Rounder (const Rounder & fromThis)
    {
      currentMode = fromThis.currentMode;
      currentVariance = fromThis.currentVariance;
    }

/*
==============================================================================
   Function: copy constructor that redefines mode
==============================================================================
*/
   Rounder::Rounder (const Rounder & fromThis, RoundMode mode)
    {
      currentMode = mode;
      currentVariance = fromThis.currentVariance;
    }

/*
==============================================================================
   Function: copy constructor that redefines variance
==============================================================================
*/
   Rounder::Rounder (const Rounder & fromThis, RoundModeVariance variance)
    {
      currentMode = fromThis.currentMode;
      currentVariance = variance;
    }

/*
==============================================================================
   Function: constructor defining mode
==============================================================================
*/
   Rounder::Rounder (RoundMode mode)
    {
      currentMode = mode;
      currentVariance = defaultVariance;
    }

/*
==============================================================================
   Function: constructor defining variance
==============================================================================
*/
   Rounder::Rounder (RoundModeVariance variance)
    {
      currentMode = defaultRoundMode;
      currentVariance = variance;
    }

/*
==============================================================================
   Function: constructor defining mode and variance
==============================================================================
*/
   Rounder::Rounder (RoundMode mode, RoundModeVariance variance)
    {
      currentMode = mode;
      currentVariance = variance;
    }

/*
==============================================================================
   Function: assignment operator
==============================================================================
*/
   Rounder & Rounder::operator= (const Rounder & fromThis)
    {
      if (&fromThis == this) return *this;
      currentMode = fromThis.currentMode;
      currentVariance = fromThis.currentVariance;
      return *this;
    }

 } //namespace Round
