
BUILTINS_LIB = lib6dbuiltins.so.1

LIBGC_LD_WRAP_CFLAGS = -D_REENTRANT -DGC_THREADS -DUSE_LD_WRAP
#LIBGC_LD_WRAP_LDFLAGS = -Wl,--wrap -Wl,read -Wl,--wrap -Wl,dlopen -Wl,--wrap -Wl,pthread_create -Wl,--wrap -Wl,pthread_join -Wl,--wrap -Wl,pthread_detach -Wl,--wrap -Wl,pthread_sigmask -Wl,--wrap -Wl,sleep
CXXFLAGS += -Wall -fPIC -O0 -g3 -Wall -Istrict -I. -Iinclude -fno-strict-overflow -Wno-deprecated -Wno-div-by-zero `pkg-config --cflags glib-2.0 gthread-2.0 libxml-2.0 libffi` $(LIBGC_LD_WRAP_CFLAGS) -D_FILE_OFFSET_BITS=64 -Wno-unused-function -Ilib/Builtins/include -DSTRICT_EVAL
LDFLAGS += -ldl -lgc -lpthread `pkg-config --libs glib-2.0 gthread-2.0 libxml-2.0 libffi` $(LIBGC_LD_WRAP_LDFLAGS)
STRICT_OBJECTS = strict/SpecialForms/Quoter.o strict/Logic/Logic.o strict/SpecialForms/Constanter.o strict/SpecialForms/Identer.o

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
$(STRICT_OBJECTS) \
$(EVAL_OBJECTS) \
$(NUMBERS) \
Builtins/Builtins.o
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
Tests/01_ShuntingYard/testShuntingYard: Tests/01_ShuntingYard/main.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
Tests/02_Evaluator/testEvaluator: Tests/02_Evaluator/main.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
Tests/03_Numbers/testNumbers: Tests/03_Numbers/main.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
Formatters/Math.o: Formatters/Math.cc Formatters/Math Values/Values include/6D/Values Formatters/GenericFormatter.inc
Formatters/SExpression.o: Formatters/SExpression.cc Formatters/SExpression Values/Values include/6D/Values 
Formatters/TExpression.o: Formatters/TExpression.cc Formatters/TExpression Values/Values  include/6D/Values strict/SpecialForms/SpecialForms lazy/SpecialForms/SpecialForms
Scanners/Lang5D.o: Scanners/Lang5D.cc Scanners/Scanner Scanners/ShuntingYardParser Combinators/Combinators include/6D/Values include/6D/FFIs include/6D/Allocators Numbers/Integer
Scanners/Scanner.o: Scanners/Scanner.cc Scanners/Scanner 
Scanners/ShuntingYardParser.o: Scanners/ShuntingYardParser.cc Scanners/ShuntingYardParser 
Values/Values.o: Values/Values.cc Values/Values Values/Symbol Values/Keyword Allocators/Allocators include/6D/Allocators include/6D/Values
Values/Keyword.o: Values/Keyword.cc Values/Keyword Values/Hashtable
Values/Symbol.o: Values/Symbol.cc Values/Symbol Values/Hashtable
Allocators/Allocators.o: Allocators/Allocators.cc Allocators/Allocators include/6D/Allocators
Tests/01_ShuntingYard/main.o: Tests/01_ShuntingYard/main.cc Scanners/Lang5D Formatters/TExpression include/6D/Values include/6D/Allocators
Tests/02_Evaluator/main.o: Tests/02_Evaluator/main.cc Scanners/Lang5D Formatters/TExpression include/6D/Allocators include/6D/Values include/6D/Evaluators include/6D/Builtins
Tests/03_Numbers/main.o: Tests/03_Numbers/main.cc Scanners/Lang5D Formatters/TExpression include/6D/Allocators include/6D/Values include/6D/Evaluators
Values/wstrings.o: Values/wstrings.cc include/6D/wstrings Allocators/Allocators include/6D/Allocators
Evaluators/Evaluators.o: Evaluators/Evaluators.cc strict/SpecialForms/SpecialForms lazy/SpecialForms/SpecialForms Values/Values
Numbers/Integer.o: Numbers/Integer.cc Numbers/Integer Numbers/Small include/6D/FFIs include/6D/Values
Numbers/Real.o: Numbers/Real.cc Numbers/Real Numbers/Small Numbers/Integer include/6D/FFIs include/6D/Values
Numbers/BigUnsigned.o: Numbers/BigUnsigned.cc Numbers/BigUnsigned Numbers/Small
Numbers/Ratio.o: Numbers/Ratio.cc Numbers/Ratio Numbers/Small Numbers/Real Numbers/Integer include/6D/FFIs include/6D/Values
strict/SpecialForms/Quoter.o: strict/SpecialForms/Quoter.cc strict/SpecialForms/Quoter strict/SpecialForms/SpecialForms include/6D/Operations
strict/SpecialForms/Constanter.o: strict/SpecialForms/Constanter.cc strict/SpecialForms/Constanter strict/SpecialForms/SpecialForms include/6D/Operations
lazy/Logic/Logic.o: lazy/Logic/Logic.cc lazy/Logic/Logic include/6D/Values include/6D/Operations include/6D/Evaluators include/6D/FFIs
strict/Logic/Logic.o: strict/Logic/Logic.cc strict/Logic/Logic include/6D/Values include/6D/Operations include/6D/Evaluators strict/SpecialForms/SpecialForms Combinators/Combinators
Combinators/Combinators.o: Combinators/Combinators.cc Combinators/Combinators include/6D/Operations include/6D/Evaluators
ModuleSystem/Memoizer.o: ModuleSystem/Memoizer.cc ModuleSystem/Memoizer Values/Values Values/Hashtable
Builtins/Builtins.o: Builtins/Builtins.cc Builtins/Builtins strict/SpecialForms/SpecialForms lazy/SpecialForms/SpecialForms include/6D/Builtins
test: Tests/01_ShuntingYard/testShuntingYard Tests/03_Numbers/testNumbers Tests/02_Evaluator/testEvaluator
	Tests/01_ShuntingYard/run
	Tests/03_Numbers/run
	Tests/02_Evaluator/run

all:
	$(MAKE) -C OPLs all
	$(MAKE) -C Arithmetic all
	$(MAKE) -C Trigonometry all
	$(MAKE) -C Logic all
	$(MAKE) -C Composition all
	$(MAKE) -C List all
	$(MAKE) -C Pair all
	$(MAKE) -C Set all
	$(MAKE) -C OS all
	$(MAKE) -C IO all
	$(MAKE) -C Maybe all
	$(MAKE) -C UI all
	$(MAKE) -C FFI all
	$(MAKE) -C String all
	$(MAKE) -C Reflection all
	$(MAKE) -C Error all
	$(MAKE) -C LinearAlgebra all
	$(MAKE) -C OO all
	$(MAKE) -C Tree all
	$(MAKE) -C Evaluators all
	$(MAKE) -C Parsers all
	$(MAKE) -C Assemblers all
	$(MAKE) -C Compilers all
	$(MAKE) -C Formatters all

clean:
	rm -f $(OBJECTS)
	$(MAKE) -C OPLs clean
	$(MAKE) -C Arithmetic clean
	$(MAKE) -C Trigonometry clean
	$(MAKE) -C Logic clean
	$(MAKE) -C Composition clean
	$(MAKE) -C List clean
	$(MAKE) -C Pair clean
	$(MAKE) -C Set clean
	$(MAKE) -C OS clean
	$(MAKE) -C IO clean
	$(MAKE) -C Maybe clean
	$(MAKE) -C UI clean
	$(MAKE) -C FFI clean
	$(MAKE) -C String clean
	$(MAKE) -C Reflection clean
	$(MAKE) -C Error clean
	$(MAKE) -C LinearAlgebra clean
	$(MAKE) -C OO clean
	$(MAKE) -C Tree clean
	$(MAKE) -C Evaluators clean
	$(MAKE) -C Parsers clean
	$(MAKE) -C Assemblers clean
	$(MAKE) -C Compilers clean
	$(MAKE) -C Formatters clean

distclean:
	rm -f $(OBJECTS)
	rm -f $(LIB) $(STATIC_LIB)
	$(MAKE) -C OPLs distclean
	$(MAKE) -C Arithmetic distclean
	$(MAKE) -C Trigonometry distclean
	$(MAKE) -C Logic distclean
	$(MAKE) -C Composition distclean
	$(MAKE) -C List distclean
	$(MAKE) -C Pair distclean
	$(MAKE) -C Set distclean
	$(MAKE) -C OS distclean
	$(MAKE) -C IO distclean
	$(MAKE) -C Maybe distclean
	$(MAKE) -C UI distclean
	$(MAKE) -C FFI distclean
	$(MAKE) -C String distclean
	$(MAKE) -C Reflection distclean
	$(MAKE) -C Error distclean
	$(MAKE) -C LinearAlgebra distclean
	$(MAKE) -C OO distclean
	$(MAKE) -C Tree distclean
	$(MAKE) -C Evaluators distclean
	$(MAKE) -C Parsers distclean
	$(MAKE) -C Assemblers distclean
	$(MAKE) -C Compilers distclean
	$(MAKE) -C Formatters distclean

install:
	$(MAKE) -C OPLs install
	$(MAKE) -C Arithmetic install
	$(MAKE) -C Trigonometry install
	$(MAKE) -C Logic install
	$(MAKE) -C Composition install
	$(MAKE) -C List install
	$(MAKE) -C Pair install
	$(MAKE) -C Set install
	$(MAKE) -C OS install
	$(MAKE) -C IO install
	$(MAKE) -C Maybe install
	$(MAKE) -C UI install
	$(MAKE) -C FFI install
	$(MAKE) -C String install
	$(MAKE) -C Reflection install
	$(MAKE) -C Error install
	$(MAKE) -C LinearAlgebra install
	$(MAKE) -C OO install
	$(MAKE) -C Tree install
	$(MAKE) -C Evaluators install
	$(MAKE) -C Parsers install
	$(MAKE) -C Assemblers install
	$(MAKE) -C Compilers install
	$(MAKE) -C Formatters install

dist:
