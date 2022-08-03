# Calculators

I saw One Lone Coder's video about fixed-point numbers (https://www.youtube.com/watch?v=ZMsrZvBmQnU), and I wanted to share some calculators that I have created that construct floating-point numbers from operations on `int`s and `char`s. I will fill this out later as I go through my garbage to find them.

One thing that I need to note: if you couldn't tell by the copyright dates, much of this code precedes C++11. In fact, all of it was written using a version of GCC where the features of C++11 were enabled by the flag `--std=c++0x`. If you are wondering why I rolled my own reference-counting classes: I didn't know `<tr1/memory>` existed, and wouldn't have brought in Boost.

Note August 2022: I found a bug in the comparison code that affects almost everything here. However, the bug would never be realized in any of these calculators, as the code in question isn't meaningfully exercised anywhere (hence why it took me so long to find, see note at the bottom). Also, I updated the code to be truly 64-bit in the "Alt" calcs. Define `BIG_INT_QUAD_BYTE` to get the old behavior (it may not compile with a MS compiler).

## SlowCalc

SlowCalc started as a Winter-break project in college in Java. I had a class that possibly would have some programming tasks in Java (it actually didn't) in the Spring, so I made a calculator in Java to learn some fundamentals of the language. I had been playing around with Analytical Engine emulations, and the floating-point representation immediately follows from those. Also that Spring semester, I took a C++ course, and decided to rewrite the calculator I had written over the Winter as a C++ program for that class's course project. I haven't changed the code from where I left it in 2010: I'm proud that such old code compiles with six warnings and seems to run fine.

SlowCalc's concept of a number starts with the `Column`. The `Column` is a sequence of digits that provide operations between two sequences that are consistent with addition and subtraction of arbitrary-precision numbers. It also implements operations that allow rounding and ease the implementation of multiplication and division. The `Column` is, essentially, a sequence of digits (characters), the length of that sequence (the number of digits or precision of the number), and a flag indicating whether the sequence is positive or negative. The absence of the sequence indicates that the sequence is all zero (but I also have a flag that indicates zero). Column also has some logic that implies that its primary purpose is to implement a normalized fixed-point number.

Upon `Column` is built `Float`, which is a collection of algorithms on `Columns` that implement the semantics of floating-point numbers. A `Float` is a `Column`, an integer exponent, and a flag (two) to indicate whether the `Float` is the special infinity value, or the special Not-A-Number value.

The program demonstrates building arbitrary-precision floating-point numbers from basic types: booleans, characters, and integers.

I'm going to go into the math a little more: the `Column` class works a lot like the `bc_num` of GNU bc. It is a base 10, sign-magnitude number. The Plan 9 implementation of bc uses a base 100, 100's complement number, but I can't reason about that system. Anyway: the core logic in `basicAddSub()` is the same core logic in `_bc_do_add()` and `_bc_do_sub()`: iterate over the digits, doing the operation and keeping track of the carry.  
For addition, the base case is that there is no carry, and the maximum sum is `9` plus `9`. That gives `18` or `8` with a carry out of `1`. For the next case the maximum sum is `9` plus `9` with a carry in of `1`, to make `19`. It should be apparent that every other addition is just the second case repeated. So, we can just look at the result and if it is over ten, set the carry out and subtract ten: for all cases, a simple loop.  
Subtraction is similar. The base case is `0` minus `9`, which gives us `-9`, which is `1` with a borrow out of `1`. We need to borrow a "ten" from the "ten's" place and then subtract nine from ten. For the second case, we also have `0` minus `9` with a borrow in, giving us `-10`. So, we can just look at the result and if it is less than zero, set the borrow out and add ten: for all cases, a simple loop.  
You may notice that, from this logic, if we subtracted a larger number from a smaller number, we would reach a point where we would constantly be trying to borrow in from nowhere, and end up with a leading sequence of nines. The sign-magnitude logic prevents this: it does comparisons of the magnitudes of the numbers and then does addition or subtraction based on the magnitudes and signs of the numbers. However, we could utilize this property and remove the sign flag by recognizing a number with a leading `9` as negative and a number with a leading `0` as positive: that would give us a ten's complement number. The logic for ten's complement is actually much simpler, however I can't reason about that system (it is an intellectual limitation of the author).  
Multiplication, implemented in `Float`, does something a little funny: it has lookup tables for the ten's place and one's place of the result of multiplication. It goes over every digit of one of the `Column`s and does a single digit multiply by the other `Column`. While using multiplication and division would probably be more efficient in modern processors, the implemented algorithm has no multiplications that cannot be removed: multiplies by two are bit shifts, and multiplies by ten can be converted to shifts and adds because ten is a constant.  
I'm not going into division much: it is basically the division algorithm you can read about for implementing division in binary on computers that don't have a divider. Note that, because I am using normalized numbers and how I initialize the `Column`s, it assumes that all of the shifting at the start of the algorithm doesn't need to be performed. Maybe I will say a little. Say you have seven divided by three. We're storing it in a weird way, so like `70` and `30`. We want at least two digits, so we actually do 7000 and 3000, with the first digit in the tens place. These iterations would follow:
* 7000, 3000, 0
* 4000, 3000, 10
* 1000, 3000, 20
* 1000, 300, 20
* 700, 300, 21
* 400, 300, 22
* 100, 300, 23

We have successfully gotten to a result of 2.3. In practice, we add two guard digits to the operation: one in case we generate a leading zero (a case we can check for beforehand, but don't), and one to round the result on (we can interpret the remainder to do _correct_ rounding, but I didn't want to figure it out using `Column`s, and it needs that check for a leading zero).

## AltSlowCalc

This was copied from my ArbitraryPrecision repository and checked with older sources that I have. The code has been reorganized to fit in one folder. There was something wrong in the math: I was not correctly handling cases where the next digit approximation in division was the radix (or greater). Basically, my test case had during division a next digit approximation of 0x100000001, when the next digit was 0xFFFFFFFF; the approximation got truncated to 1 and thus my catastrophic loss of precision.

Anyway, this will be a fun adventure. Before I rewrote GNU bc to use GMP, I had already gone through the motions of building the logic that bc needed on the foundation of an arbitrary-precision integer library.

We'll start at the base of the class hierarchy: `BitField`. The `BitField` class represents an arbitrarily-large Whole number. It can be zero or some positive value. It differs from a `Column` in these important ways: it is of completely dynamic size, and it operates on digits in base 4294967296. That's 2^32, or four bytes. The interesting thing is that addition and subtraction are almost exactly the same: we don't need to add or subtract the base on borrow/carry out, and we shift the result to get the carry. The only other notable thing that the `BitField` class provides are tools for implementing multiplication and printing out numbers: multiplication by a single "digit", and division by a single "digit".

Upon `BitField` is built the `Integer` class. This is a fully-independent arbitrarily-large integer (it was designed to stand alone). It adds a sign flag and implements algorithms to turn a Whole number into an Integer.

Next up is the `Fixed` class, a fixed-point number class. The significand of a floating-point number is just a fixed-point number, so I implemented this class to hide that layer of operations (the scaled integer math) from the floating-point class. This is just algorithms that wrap integers, and is composed of an `Integer` that stores the digits, and a precision field that says where the decimal point is.

Next, but mysteriously not finally, we have the `Float` class. It has a `Fixed` significand. It has a sign flag, an exponent field, and a flag for infinity or Not-A-Number. A disgusting amount of code in this class is devoted to handling zeros, NaNs, and infinities correctly.

The final thing is the `DecFloat` class. This thing is responsible for enforcing limitations on precision and exponents. Some of the library routines make numbers with precision greater than is permitted by the library so that the result can be accurate. Some terms in series we don't want to flush to zero when they could change the result. It is the hard separation between the developer of the floating-point library and the developer using the floating-point library.

## AltCalc5Slimmed

At some point, I merged the `Float` and `DecFloat` classes and changed the semantics of the `Fixed` class to produce this. It also rips out everything from `BitField`, `Integer`, and `Fixed` that isn't used to construct `Float`.

## Calc4

Calc4 only marginally belongs here: it's `Float` is a wrapper over MPFR. It really only gives warm-fuzzies that other calculators are implemented correctly, by allowing one to compare the results with a library that is considered "good". It is always a good sign that nothing is amiss when multiple implementations of math agree on a result. A strange result can find corner cases in implemented algorithms in the math library, or find reliance on the treatment of corner cases in the algorithm being run.

The nice thing about arbitrary-precision floating-point is that one can run an algorithm with increasing precision to brute-force away rounding errors and numerical instability. Remember, though, there are trade-offs. A rule-of-thumb is that software floating-point is ten times slower than hardware floating-point. At the same precision. Then, at least linearly scale the speed with the added precision: doubled precision is twice as slow or half as fast. It's worse if the algorithms have lots of multiplication and division, then quadratically scale the slowdown: doubled precision takes four times as long.

## AC6 and AC6S

These are versions of AltSlowCalc and AltCalc5Slimmed that have been converted to use GMP as their arbitrary-precision integer library. From testing this, it was obvious that the culprit in the aforementioned numerical issues is the `Integer`/`BitField` classes. I mis-handled an edge case in division, **AGAIN**. This is the nice thing about doing the math in multiple ways: finding where the source of an issue is. When I switch out the BigInt library, the Float library built upon it starts producing consistent results. Progress.

# DUMBBASIC 14

I decided to add the mythical DUMBBASIC 14 to this repo. I've talked about it in other places. As I said before: it is written to C++03, and there are some interesting choices because of that. It actually didn't compile with a modern C++ compiler: the construct `#define CATSTR(x) "1"x"2"` is no longer valid C++, and the compiler complains that x"2" is a conversion operator to "1". As such, I also decided to fix the constant bug and clean up warnings. The DB14IN uses an interpreter pattern to interpret the code, while DB14VM compiles to virtual instructions. The version of the interpreter chosen has extra functions that support adding to the program during runtime, and the DBbc program was made to have a bc-like REPL.

I must note two example programs: FiveSixSevenEight.txt and KahanTest.txt. I think that it is very important that one understands what their outputs tell you.

## DB14ACS

This one uses the AltCalc5Slimmed number routines. Using these routines is _a lot_ slower, by an order or two of magnitude. However, you also don't need GMP or MPFR. Moving to the AC6S Float should see a speed boost. But, why would you do this? Run FiveSixSevenEight.txt and understand the results.

Okay, let me just spill it. FiveSixSevenEight is a program that tests the associativity of a math operation, and gives the precisions at which that math operation is associative. For binary floating point, the associativity is pretty random. The operation randomly fails to be associative at certain precisions. For decimal floating point, the associativity fails in one place: at nine digits of precision, where the ten digit result gets rounded differently depending on which multiplication happens first (this is serendipitous: it could fail at lower precisions, but this example does not; what is important is that it cannot fail at precisions greater than or equal to the number of digits in the product).

KahanTest is a pathological function designed to have a catastrophic loss of precision.

Making sure this version compiled correctly helped me find two bugs in the floating-point implementation: zero was not handled as being equal to itself in less than or equal and greater than or equal; and the signs of results were flipped in comparisons when the exponents were different and the signs are different. Neither of these bugs should ever have been exercised by the calculator programs. Their wrongness stretches all the way back to SlowCalc. When I rewrote the logic from Java to C++, I seem to have "fixed" it, as the Java is correct and the earlier C code that the Java was based on is correct (the earlier Analytical Engine emulators were written in C, and their code structure follows from the physical description of the machine: a column of mechanical wheels labeled '0' to '9').
