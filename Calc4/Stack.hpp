/*
Copyright (c) 2010 Thomas DiModica.
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

#ifndef STACK_HPP
#define STACK_HPP

#include <stack>
#include "Float.hpp"

//This exists in namespace DecFloat as it is a stack of Floats.

namespace DecFloat
 {

    /*
      This is a wrapper for the STL stack, as I don't like
      its default behavior when asked for top() of an empty
      stack: undefined. The STL stack also has undefined
      behavior when asked to pop an empty stack.

      This also wraps up the stack-based round mode.

      With the exception of the added round mode data,
      this is a drop-in replacement for the STL stack.
    */
   class Stack
    {
      private:
         std::stack<Float> myStack;
         SupportedRoundModes myMode;

      public:
         Float & top (void);
         void push (const Float & me) { myStack.push(me); }
         Float pop (void);

         SupportedRoundModes getRoundMode (void) { return myMode; }
         SupportedRoundModes setRoundMode (SupportedRoundModes toThis)
          { return (myMode = toThis); }

         //Called when the stack is selected.
         void focus (void) { Float::setRoundMode(myMode); }

         void flush (void);

         bool isEmpty(void) { return (myStack.size() == 0); }

         Stack () : myMode(ROUND_TO_NEAREST_TIES_TO_EVEN) { }
         ~Stack () { }
    };

 } //namespace DecFloat

#endif /* STACK_HPP */
