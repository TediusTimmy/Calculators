# Turn off -Wold-style-cast because MPFR uses two in mpfr_zero_p, mpfr_nan_p, and mpfr_inf_p
g++ -Wall -Wextra -Wpedantic -Wconversion -fno-rtti -O3 -s -o DB14 DB14.cpp Parser.cpp SymbolTable.cpp StdLib.cpp Lexer.cpp Statement.cpp ValueType.cpp Expression.cpp ../Calc4/DataHolder.cpp ../Calc4/Functions.cpp ../Calc4/Float.cpp ../Calc4/Rand.cpp ../Calc4/rand850.c -lmpfr -lgmp
#g++ -g -Wall -Wextra -Wconversion DB14.cpp Parser.cpp SymbolTable.cpp StdLib.cpp Lexer.cpp Statement.cpp ValueType.cpp Expression.cpp ./Calc4/DataHolder.cpp ./Calc4/Functions.cpp ./Calc4/Float.cpp ./Calc4/Rand.cpp ./Calc4/rand850.c -lmpfr -lgmp
g++ -Wall -Wextra -Wpedantic -Wconversion -fno-rtti -O3 -s -o DBbc DBbc.cpp Parser.cpp SymbolTable.cpp StdLib.cpp Lexer.cpp Statement.cpp ValueType.cpp Expression.cpp ../Calc4/DataHolder.cpp ../Calc4/Functions.cpp ../Calc4/Float.cpp ../Calc4/Rand.cpp ../Calc4/rand850.c -lmpfr -lgmp