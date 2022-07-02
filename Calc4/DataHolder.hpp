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

/*
   A reference-counted container class for an MPFR mpfr_t.
*/

#ifndef DATAHOLDER_HPP
#define DATAHOLDER_HPP

#include <string>
#include <mpfr.h>

namespace DecFloat
 {

   class DataHolder
    {
      private:
         mutable long Refs;
         unsigned long precision;
         unsigned long bits;
         mpfr_t Data;
//mutable Mutex

         DataHolder (unsigned long);
         DataHolder (const DataHolder &);
         DataHolder (const std::string &, unsigned long);
         ~DataHolder ();

      public:
         static DataHolder * build (const std::string & src, unsigned long usePrec = 0)
          { return new DataHolder (src, usePrec); }
         static DataHolder * build (unsigned long prec) { return new DataHolder (prec); }
         static DataHolder & ZERO (void);

         DataHolder * ref (void) const;
         void deref (void);
         DataHolder * own (void);

         mpfr_ptr getInternal (void);
         mpfr_srcptr get (void) const;

         unsigned long getPrecision (void) const;
    };

 } /* namespace DecFloat */

#endif /* DATAHOLDER_HPP */
