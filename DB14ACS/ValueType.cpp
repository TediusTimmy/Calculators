/*
Copyright (c) 2015 Thomas DiModica.
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
#include "ValueType.hpp"

void DB_panic (const std::string &) __attribute__ ((__noreturn__));

ArrayValue::ArrayHolder::ArrayHolder () : Refs(1) { }

ArrayValue::ArrayHolder::ArrayHolder (long elements) : Refs(1)
 {
   Contents.resize(elements);
 }

ArrayValue::ArrayHolder::ArrayHolder (const ArrayValue::ArrayHolder & src) : Refs(1), Contents(src.Contents) { }

ArrayValue::ArrayHolder::~ArrayHolder() { }

ArrayValue::ArrayHolder * ArrayValue::ArrayHolder::ref (void) const
 {
   ++Refs;
   return const_cast<ArrayValue::ArrayHolder *>(this);
 }

void ArrayValue::ArrayHolder::deref (void)
 {
   long res = --Refs;
   if (res == 0)
    {
      delete this;
    }
 }

ArrayValue::ArrayHolder * ArrayValue::ArrayHolder::own (void)
 {
   ArrayValue::ArrayHolder * newThis = this;
   if (Refs != 1)
    {
      --Refs;
      newThis = new ArrayValue::ArrayHolder(*this);
    }
   return newThis;
 }

ArrayValue::ArrayValue (long elements) : ValueType(ARRAY), val(new ArrayHolder(elements)) { }

ArrayValue::ArrayValue (const ArrayValue & src) : ValueType(src), val(src.val->ref()) { }

ArrayValue::~ArrayValue()
 {
   val->deref();
   val = NULL;
 }

ValueType::ValueHolder ArrayValue::getIndex(long index) const
 {
   if ((index < 0) || (static_cast<size_t>(index) >= val->Contents.size()))
      DB_panic("Array index out of bounds.");

   return val->Contents[index];
 }

void ArrayValue::setIndex(long index, ValueType::ValueHolder value)
 {
   if ((index < 0) || (static_cast<size_t>(index) >= val->Contents.size()))
      DB_panic("Array index out of bounds.");

   val = val->own();
   val->Contents[index] = value;
 }
