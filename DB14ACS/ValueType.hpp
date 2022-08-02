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
#ifndef VALUETYPE_HPP
#define VALUETYPE_HPP

#include <string>
#include <vector>

#include "../AltCalc5Slimmed/Float.hpp"
#include "Missing.hpp"


class ValueType
 {
   private:
      ValueType();
      ValueType & operator= (const ValueType &);

   public:
      enum TYPE
       {
         NUMBER,
         STRING,
         ARRAY
       };
      class ValueHolder
       {
         public:
            ValueType * data;

            ValueHolder(ValueType * src) : data(src) { }
            ValueHolder(const ValueHolder & src) : data(NULL) { if (NULL != src.data) data = src.data->Clone(); }
            ValueHolder() : data(NULL) { }
            ~ValueHolder() { if (NULL != data) { delete data; data = NULL; } }
            ValueHolder & operator= (const ValueHolder & src)
             {
               if (src.data != data)
                {
                  if (NULL != data) { delete data; data = NULL; }
                  if (NULL != src.data) data = src.data->Clone();
                }
               return *this;
             }
       };

      const TYPE type;

      ValueType(TYPE type) : type(type) { }
      ValueType(const ValueType & src) : type(src.type) { }

      virtual ValueType * Clone() const = 0;
      virtual ~ValueType() { }
 };

class NumericValue : public ValueType
 {
   private:
      NumericValue();
      NumericValue & operator= (const NumericValue &);

   public:
      BigInt::Float val;

      NumericValue(const BigInt::Float & val) : ValueType(NUMBER), val(val) { }
      NumericValue(const NumericValue & src) : ValueType(src), val(src.val) { }

      NumericValue * Clone() const { return new NumericValue(*this); }
 };

class StringValue : public ValueType
 {
   private:
      StringValue();
      StringValue & operator= (const StringValue &);

   public:
      std::string val;

      StringValue(const std::string & val) : ValueType(STRING), val(val) { }
      StringValue(const StringValue & src) : ValueType(src), val(src.val) { }

      StringValue * Clone() const { return new StringValue(*this); }
 };

class ArrayValue : public ValueType
 {
   private:
      class ArrayHolder
       {
         private:
            mutable long Refs;

            ArrayHolder ();
            ArrayHolder & operator= (const ArrayHolder &);

         public:
            std::vector<ValueType::ValueHolder> Contents;

            ArrayHolder (long elements);
            ArrayHolder (const ArrayHolder &);
            ~ArrayHolder ();

            ArrayHolder * ref (void) const;
            void deref (void);
            ArrayHolder * own (void);
       };

      ArrayHolder * val;

      ArrayValue();
      ArrayValue & operator= (const ArrayValue &);

   public:

      ArrayValue(long elements);
      ArrayValue(const ArrayValue & src);
      ~ArrayValue();

      ArrayValue * Clone() const { return new ArrayValue(*this); }

      ValueType::ValueHolder getIndex(long index) const;
      void setIndex(long index, ValueType::ValueHolder value);

      size_t size (void) const { return val->Contents.size(); }
 };

#endif /* VALUETYPE_HPP */
