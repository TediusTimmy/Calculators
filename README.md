# Calculators

I saw One Lone Coder's video about fixed-point numbers (https://www.youtube.com/watch?v=ZMsrZvBmQnU), and I wanted to share some calculators that I have created that construct floating-point numbers from operations on `int`s and `char`s. I will fill this out later as I go through my garbage to find them.

## SlowCalc

SlowCalc started as a Winter-break project in college in Java. I had a class that possibly would have some programming tasks in Java (it actually didn't) in the Spring, so I made a calculator in Java to learn some fundamentals of the language. I had been playing around with Analytical Engine emulations, and the floating-point representation immediately follows from those. Also that Spring semester, I took a C++ course, and decided to rewrite the calculator I had written over the Winter as a C++ program for that class's course project. I haven't changed the code from where I left it in 2010: I'm proud that such old code compiles with six warnings and seems to run fine.

SlowCalc's concept of a number starts with the `Column`. The `Column` is a sequence of digits that provide operations between two sequences that are consistent with addition and subtraction of arbitrary-precision numbers. It also implements operations that allow rounding and ease the implementation of multiplication and division. The `Column` is, essentially, a sequence of digits (characters), the length of that sequence (the number of digits or precision of the number), and a flag indicating whether the sequence is positive or negative. The absence of the sequence indicates that the sequence is all zero (but I also have a flag that indicates zero).

Upon `Column` is built `Float`, which is a collection of algorithms on `Columns` that implement the semantics of floating-point numbers. A `Float` is a `Column`, an integer exponent, and a flag (two) to indicate whether the `Float` is the special infinity value, or the special Not-A-Number value.

The program demonstrates building arbitrary-precision floating-point numbers from basic types: booleans, characters, and integers.
