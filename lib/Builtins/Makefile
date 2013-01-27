
BUILTINS_LIB = lib6dbuiltins.so.1

LIBGC_LD_WRAP_CFLAGS = -D_REENTRANT -DGC_THREADS -DUSE_LD_WRAP
#LIBGC_LD_WRAP_LDFLAGS = -Wl,--wrap -Wl,read -Wl,--wrap -Wl,dlopen -Wl,--wrap -Wl,pthread_create -Wl,--wrap -Wl,pthread_join -Wl,--wrap -Wl,pthread_detach -Wl,--wrap -Wl,pthread_sigmask -Wl,--wrap -Wl,sleep
CXXFLAGS += -Wall -fPIC -O0 -g3 -Wall -Istrict -I. -Iinclude -fno-strict-overflow -Wno-deprecated -Wno-div-by-zero `pkg-config --cflags glib-2.0 gthread-2.0 libxml-2.0 libffi` $(LIBGC_LD_WRAP_CFLAGS) -D_FILE_OFFSET_BITS=64 -Wno-unused-function -Ilib/Builtins/include -DSTRICT_EVAL
LDFLAGS += -ldl -lgc -lpthread `pkg-config --libs glib-2.0 gthread-2.0 libxml-2.0 libffi` $(LIBGC_LD_WRAP_LDFLAGS)
STRICT_OBJECTS = strict/SpecialForms/Quoter.o strict/Logic/Logic.o strict/SpecialForms/Constanter.o
EVAL_OBJECTS = Evaluators/Evaluators.o Combinators/Combinators.o
NUMBERS = Numbers/Integer.o \
Numbers/Real.o \
Numbers/BigUnsigned.o \
Numbers/Ratio.o
OBJECTS = Formatters/Math.o \
Formatters/SExpression.o \
Formatters/TExpression.o \
Scanners/Lang5D.o \
Scanners/Scanner.o \
Scanners/ShuntingYardParser.o \
Values/Values.o \
Values/Keyword.o \
Values/Symbol.o \
Allocators/Allocators.o \
Values/wstrings.o \
Values/Hashtable.o \
Evaluators/Evaluators.o \
$(STRICT_OBJECTS) \
$(NUMBERS)
LIB = lib6dbuiltins.so.1
STATIC_LIB = lib6dbuiltins1.a  

.SUFFIXES:            # Delete the default suffixes
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
all: $(LIB) $(STATIC_LIB) 
#Tests/01_ShuntingYard/testShuntingYard
$(LIB): $(OBJECTS)
	$(CXX) -shared -o $(LIB) $(OBJECTS) $(LDFLAGS)
$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $(STATIC_LIB) $(OBJECTS)
Tests/01_ShuntingYard/testShuntingYard: Tests/01_ShuntingYard/main.o Formatters/Math.o Formatters/SExpression.o Formatters/TExpression.o Scanners/Lang5D.o Scanners/Scanner.o Scanners/ShuntingYardParser.o Values/Values.o Values/Keyword.o Values/Symbol.o Allocators/Allocators.o
	$(CXX) -o $@ $^ $(LDFLAGS)
Tests/02_Evaluator/testEvaluator: Tests/02_Evaluator/main.o Formatters/Math.o Formatters/SExpression.o Formatters/TExpression.o Scanners/Lang5D.o Scanners/Scanner.o Scanners/ShuntingYardParser.o Values/Values.o Values/Keyword.o Values/Symbol.o Allocators/Allocators.o $(EVAL_OBJECTS) $(STRICT_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
Formatters/Math.o: Formatters/Math.cc Formatters/Math Values/Values include/6D/Values Formatters/GenericFormatter.inc
Formatters/SExpression.o: Formatters/SExpression.cc Formatters/SExpression Values/Values include/6D/Values 
Formatters/TExpression.o: Formatters/TExpression.cc Formatters/TExpression Values/Values  include/6D/Values
Scanners/Lang5D.o: Scanners/Lang5D.cc Scanners/Scanner Scanners/ShuntingYardParser
Scanners/Scanner.o: Scanners/Scanner.cc Scanners/Scanner 
Scanners/ShuntingYardParser.o: Scanners/ShuntingYardParser.cc Scanners/ShuntingYardParser 
Values/Values.o: Values/Values.cc Values/Values Values/Symbol Values/Keyword Allocators/Allocators include/6D/Allocators include/6D/Values
Values/Keyword.o: Values/Keyword.cc Values/Keyword Values/Hashtable
Values/Symbol.o: Values/Symbol.cc Values/Symbol Values/Hashtable
Allocators/Allocators.o: Allocators/Allocators.cc Allocators/Allocators include/6D/Allocators
Tests/01_ShuntingYard/main.o: Tests/01_ShuntingYard/main.cc Scanners/Lang5D Formatters/TExpression include/6D/Values
Tests/02_Evaluator/main.o: Tests/02_Evaluator/main.cc Scanners/Lang5D Formatters/TExpression include/6D/Allocators include/6D/Values include/6D/Evaluators
Values/wstrings.o: Values/wstrings.cc include/6D/wstrings Allocators/Allocators include/6D/Allocators
Evaluators/Evaluators.o: Evaluators/Evaluators.cc strict/SpecialForms/SpecialForms lazy/SpecialForms/SpecialForms Values/Values
Numbers/Integer.o: Numbers/Integer.cc Numbers/Integer Numbers/Small
Numbers/Real.o: Numbers/Real.cc Numbers/Real Numbers/Small Numbers/Integer
Numbers/BigUnsigned.o: Numbers/BigUnsigned.cc Numbers/BigUnsigned Numbers/Small
Numbers/Ratio.o: Numbers/Ratio.cc Numbers/Ratio Numbers/Small Numbers/Real Numbers/Integer
strict/SpecialForms/Quoter.o: strict/SpecialForms/Quoter.cc strict/SpecialForms/Quoter strict/SpecialForms/SpecialForms include/6D/Operations
strict/SpecialForms/Constanter.o: strict/SpecialForms/Constanter.cc strict/SpecialForms/Constanter strict/SpecialForms/SpecialForms include/6D/Operations
lazy/Logic/Logic.o: lazy/Logic/Logic.cc lazy/Logic/Logic include/6D/Values include/6D/Operations include/6D/Evaluators
strict/Logic/Logic.o: strict/Logic/Logic.cc strict/Logic/Logic include/6D/Values include/6D/Operations include/6D/Evaluators strict/SpecialForms/SpecialForms Combinators/Combinators
Combinators/Combinators.o: Combinators/Combinators.cc Combinators/Combinators include/6D/Operations include/6D/Evaluators
test: Tests/01_ShuntingYard/testShuntingYard Tests/02_Evaluator/testEvaluator
	Tests/01_ShuntingYard/run
	Tests/02_Evaluator/run

clean:
	rm -f $(OBJECTS)
distclean: clean
	rm -f $(LIB) $(STATIC_LIB)
