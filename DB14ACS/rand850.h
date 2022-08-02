/*
   This file is part of rand850.

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
   File: rand850.h
      srand850 takes a long by this file, but an unsigned long by its
      definition so as to quell warnings when it is seeded with time().
      rand850i takes a long by this file, but an unsigned long by its
      definition because the routine doesn't handle negative inputs.
*/

#ifndef RAND850_H
#define RAND850_H

#ifdef __cplusplus
extern "C"
 {
#endif

   void srand850 (long);
   void srand850s (char *);

   char * rand850s (void);
   unsigned long rand850ul (void);
   int rand850i (long);
   signed long rand850sl (signed long, signed long);

#ifdef __cplusplus
 }
#endif

#endif /* RAND850_H */
