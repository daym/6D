
LIBGC_LD_WRAP_CFLAGS = -D_REENTRANT -DGC_THREADS -DUSE_LD_WRAP
CXXFLAGS += -O1 -g3 -Wall -I. -fno-strict-overflow -Wno-deprecated -Wno-div-by-zero `pkg-config --cflags glib-2.0 gthread-2.0 libxml-2.0 libffi` $(LIBGC_LD_WRAP_CFLAGS) -D_FILE_OFFSET_BITS=64 -Wno-unused-function -Ilib/Builtins/include
OBJECTS = Formatters/Math.o Formatters/SExpression.o Formatters/TExpression.o Scanners/Lang5D.o Scanners/Scanner.o Scanners/ShuntingYardParser.o Values/Values.o Values/Keyword.o Values/Symbol.o Allocators/Allocators.o
.SUFFIXES:            # Delete the default suffixes
%.o: %.cc
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
all: Tests/01_ShuntingYard/testShuntingYard
Tests/01_ShuntingYard/testShuntingYard: Tests/01_ShuntingYard/main.o Formatters/Math.o Formatters/SExpression.o Formatters/TExpression.o Scanners/Lang5D.o Scanners/Scanner.o Scanners/ShuntingYardParser.o Values/Values.o Values/Keyword.o Values/Symbol.o Allocators/Allocators.o
	$(CXX) -o $@ $^
Formatters/Math.o: Formatters/Math.cc Formatters/Math Values/Values Formatters/GenericFormatter.inc
Formatters/SExpression.o: Formatters/SExpression.cc Formatters/SExpression Values/Values 
Formatters/TExpression.o: Formatters/TExpression.cc Formatters/TExpression Values/Values 
Scanners/Lang5D.o: Scanners/Lang5D.cc Scanners/Scanner Scanners/ShuntingYardParser
Scanners/Scanner.o: Scanners/Scanner.cc Scanners/Scanner 
Scanners/ShuntingYardParser.o: Scanners/ShuntingYardParser.cc Scanners/ShuntingYardParser 
Values/Values.o: Values/Values.cc Values/Values Values/Symbol Values/Keyword Allocators/Allocators
Values/Keyword.o: Values/Keyword.cc Values/Keyword
Values/Symbol.o: Values/Symbol.cc Values/Symbol
Allocators/Allocators.o: Allocators/Allocators.cc Allocators/Allocators
Tests/01_ShuntingYard/main.o: Tests/01_ShuntingYard/main.cc Scanners/Lang5D Formatters/TExpression
test: Tests/01_ShuntingYard/testShuntingYard
	Tests/01_ShuntingYard/run

clean:
	rm -f $(OBJECTS)
