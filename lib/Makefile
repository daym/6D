
#LIBGC_LD_WRAP_CFLAGS = -D_REENTRANT -DGC_THREADS -DUSE_LD_WRAP
#LIBGC_LD_WRAP_LDFLAGS = -Wl,--wrap -Wl,read -Wl,--wrap -Wl,dlopen -Wl,--wrap -Wl,pthread_create -Wl,--wrap -Wl,pthread_join -Wl,--wrap -Wl,pthread_detach -Wl,--wrap -Wl,pthread_sigmask -Wl,--wrap -Wl,sleep

OBJDUMP ?= objdump
CC = gcc
PACKAGE = 6d
VERSION = 0.0.1
DISTFILES = \
$(PACKAGE)-$(VERSION)/Tests/03_Numbers/02.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/03_Numbers/02.5D \
$(PACKAGE)-$(VERSION)/Tests/03_Numbers/03.5D \
$(PACKAGE)-$(VERSION)/Tests/03_Numbers/main.c \
$(PACKAGE)-$(VERSION)/Tests/03_Numbers/01.5D \
$(PACKAGE)-$(VERSION)/Tests/03_Numbers/03.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/03_Numbers/01.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/02.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/05.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/2.gotResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/23.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/21.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/02.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/20.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/10.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/25.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/f.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/04.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/06.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/call2.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/07.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/17.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/15.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/1.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/11.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/12.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/6.gotResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/19.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/b.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/a.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/21.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/c.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/10.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/25.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/e.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/03.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/14.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/f.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/13.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/04.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/15.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/05.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/22.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/d.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/14.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/18.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/main.c \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/12.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/18.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/03.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/5.gotResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/01.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/11.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/d.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/4.gotResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/c.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/07.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/1.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/e.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/b.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/1.gotResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/22.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/call2.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/24.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/a.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/run \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/13.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/3.gotResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/19.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/24.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/16.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/06.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/23.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/17.5D \
$(PACKAGE)-$(VERSION)/Tests/01_ShuntingYard/16.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/02.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/05.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/02.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/10.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/04.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/06.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/07.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/08.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/09.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/10.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/08.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/03.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/04.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/05.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/09.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/main.c \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/01.5D \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/03.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/01.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/07.expectedResult \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/run \
$(PACKAGE)-$(VERSION)/Tests/02_Evaluator/06.5D \
$(PACKAGE)-$(VERSION)/Tests/sillyprint.inc \
$(PACKAGE)-$(VERSION)/include/6D/Operations \
$(PACKAGE)-$(VERSION)/include/6D/FFIs \
$(PACKAGE)-$(VERSION)/include/6D/Allocators \
$(PACKAGE)-$(VERSION)/include/6D/Evaluators \
$(PACKAGE)-$(VERSION)/include/6D/wstrings \
$(PACKAGE)-$(VERSION)/include/6D/Values \
$(PACKAGE)-$(VERSION)/include/6D/Builtins \
$(PACKAGE)-$(VERSION)/include/6D/Modulesystem \
$(PACKAGE)-$(VERSION)/include/6D/Arithmetics \
$(PACKAGE)-$(VERSION)/include/6D/Lang5D \
$(PACKAGE)-$(VERSION)/Allocators/Allocators.c \
$(PACKAGE)-$(VERSION)/Allocators/Allocators \
$(PACKAGE)-$(VERSION)/Combinators/Combinators.c \
$(PACKAGE)-$(VERSION)/Combinators/Combinators \
$(PACKAGE)-$(VERSION)/Evaluators/Evaluators.c \
$(PACKAGE)-$(VERSION)/Evaluators/Evaluators \
$(PACKAGE)-$(VERSION)/Makefile \
$(PACKAGE)-$(VERSION)/Numbers/Integer2.c \
$(PACKAGE)-$(VERSION)/Numbers/Real.c \
$(PACKAGE)-$(VERSION)/Numbers/Small \
$(PACKAGE)-$(VERSION)/Numbers/Integer2 \
$(PACKAGE)-$(VERSION)/Numbers/Ratio \
$(PACKAGE)-$(VERSION)/Numbers/Ratio.c \
$(PACKAGE)-$(VERSION)/Numbers/Real \
$(PACKAGE)-$(VERSION)/Arithmetics/Arithmetics.c \
$(PACKAGE)-$(VERSION)/Arithmetics/Arithmetics \
$(PACKAGE)-$(VERSION)/Strings/Strings \
$(PACKAGE)-$(VERSION)/Strings/Strings.c \
$(PACKAGE)-$(VERSION)/Formatters/Math2.c \
$(PACKAGE)-$(VERSION)/Formatters/Math2 \
$(PACKAGE)-$(VERSION)/Parsers/Lang5D.c \
$(PACKAGE)-$(VERSION)/Parsers/ShuntingYardParser \
$(PACKAGE)-$(VERSION)/Scanners/Scanner \
$(PACKAGE)-$(VERSION)/Parsers/Lang5D \
$(PACKAGE)-$(VERSION)/Scanners/Scanner.inc \
$(PACKAGE)-$(VERSION)/Scanners/Scanner.c \
$(PACKAGE)-$(VERSION)/Parsers/ShuntingYardParser.inc \
$(PACKAGE)-$(VERSION)/Values/Values.c \
$(PACKAGE)-$(VERSION)/Values/wstrings.c \
$(PACKAGE)-$(VERSION)/Values/Symbol \
$(PACKAGE)-$(VERSION)/Values/Keyword \
$(PACKAGE)-$(VERSION)/Values/Hashtable.c \
$(PACKAGE)-$(VERSION)/Values/Hashtable.inc \
$(PACKAGE)-$(VERSION)/Values/Keyword.c \
$(PACKAGE)-$(VERSION)/Values/Hashtable \
$(PACKAGE)-$(VERSION)/Values/Symbol.c \
$(PACKAGE)-$(VERSION)/Values/Values \
$(PACKAGE)-$(VERSION)/Values/Error.c \
$(PACKAGE)-$(VERSION)/Values/Error \
$(PACKAGE)-$(VERSION)/WIN32/wstrings.c \
$(PACKAGE)-$(VERSION)/WIN32/testShuntingYard.cpp \
$(PACKAGE)-$(VERSION)/WIN32/targetver.h \
$(PACKAGE)-$(VERSION)/WIN32/stdafx.h \
$(PACKAGE)-$(VERSION)/WIN32/ReadMe.txt \
$(PACKAGE)-$(VERSION)/WIN32/testShuntingYard.sln \
$(PACKAGE)-$(VERSION)/WIN32/stdafx.cpp \
$(PACKAGE)-$(VERSION)/WIN32/testShuntingYard.vcproj \
$(PACKAGE)-$(VERSION)/lazy/SpecialForms/SpecialForms \
$(PACKAGE)-$(VERSION)/lazy/SpecialForms/SpecialForms.c \
$(PACKAGE)-$(VERSION)/lazy/Logic/Logic \
$(PACKAGE)-$(VERSION)/lazy/Logic/Logic.c \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/SpecialForms \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/Quoter \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/Constanter \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/Constanter.c \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/Identer.c \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/Identer \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/Unapplier.c \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/Unapplier \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/Quoter.c \
$(PACKAGE)-$(VERSION)/strict/SpecialForms/SpecialForms.c \
$(PACKAGE)-$(VERSION)/strict/Logic/Logic \
$(PACKAGE)-$(VERSION)/strict/Logic/Logic.c \
$(PACKAGE)-$(VERSION)/design/symbols.py \
$(PACKAGE)-$(VERSION)/design/scanner.py \
$(PACKAGE)-$(VERSION)/design/shuntingyard.py.simple \
$(PACKAGE)-$(VERSION)/design/shuntingyard.py \
$(PACKAGE)-$(VERSION)/design/generate \
$(PACKAGE)-$(VERSION)/Builtins/Builtins.c \
$(PACKAGE)-$(VERSION)/Builtins/Builtins \
$(PACKAGE)-$(VERSION)/Modulesystem/Memoizer.c \
$(PACKAGE)-$(VERSION)/Modulesystem/Memoizer \
$(PACKAGE)-$(VERSION)/Modulesystem/Modulesystem \
$(PACKAGE)-$(VERSION)/Modulesystem/Exports.c \
$(PACKAGE)-$(VERSION)/Allocators/SillyGC \
$(PACKAGE)-$(VERSION)/Allocators/SillyGC.cc \
$(PACKAGE)-$(VERSION)/FFIs/Converters \
$(PACKAGE)-$(VERSION)/FFIs/Converters.c \
$(PACKAGE)-$(VERSION)/generateWrapper \
$(PACKAGE)-$(VERSION)/OPLs/MinimalOPL.c \
$(PACKAGE)-$(VERSION)/OPLs/MinimalOPL

BUILTINS_LIB = lib6dbuiltins.so.1
CFLAGS += $(shell pkg-config --cflags glib-2.0 gthread-2.0)
LDFLAGS += $(shell pkg-config --libs glib-2.0 gthread-2.0) -lm
CFLAGS += -Wall -Werror -fPIC -O0 -g3 -Wall -Istrict -I. -Iinclude -fno-strict-overflow -Wno-deprecated $(LIBGC_LD_WRAP_CFLAGS) -D_FILE_OFFSET_BITS=64 -Wno-unused-function -Ilib/Builtins/include -DSTRICT_EVAL
ifdef LIBGC_LD_WRAP_CFLAGS
LDFLAGS += -lgc
endif
LDFLAGS += -lpthread $(LIBGC_LD_WRAP_LDFLAGS)
STRICT_OBJECTS = strict/SpecialForms/Quoter.o strict/Logic/Logic.o strict/SpecialForms/Constanter.o strict/SpecialForms/Identer.o strict/SpecialForms/SpecialForms.o strict/SpecialForms/Unapplier.o

EVAL_OBJECTS = Evaluators/Evaluators.o Combinators/Combinators.o FFIs/Converters.o
NUMBERS_OBJECTS = Numbers/Integer2.o \
Numbers/Real.o \
Numbers/Ratio.o \
Arithmetics/Arithmetics.o
STRINGS = Strings/Strings.o
OBJECTS = Formatters/Math2.o \
Parsers/Lang5D.o \
Scanners/Scanner.o \
Values/Values.o \
Values/Keyword.o \
Values/Symbol.o \
Values/Error.o \
Allocators/Allocators.o \
Values/wstrings.o \
Values/Hashtable.o \
$(STRICT_OBJECTS) \
$(EVAL_OBJECTS) \
$(NUMBERS_OBJECTS) \
$(STRINGS) \
Modulesystem/Exports.o \
Modulesystem/Memoizer.o \
Builtins/Builtins.o \
Builtins/wrapper.o \
OPLs/MinimalOPL.o

SILLY_GC_OBJECTS = Allocators/SillyGC.o
LIB = lib6dbuiltins.so.1
STATIC_LIB = lib6dbuiltins1.a  

.SUFFIXES:            # Delete the default suffixes
%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
all: $(LIB) $(STATIC_LIB) 
#Tests/01_ShuntingYard/testShuntingYard
$(LIB): $(OBJECTS)
	$(CC) -shared -o $(LIB) $(OBJECTS) $(LDFLAGS)
$(STATIC_LIB): $(OBJECTS)
	$(AR) rcs $(STATIC_LIB) $(OBJECTS)
Tests/01_ShuntingYard/testShuntingYard: Tests/01_ShuntingYard/main.o $(OBJECTS) $(SILLY_GC_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
Tests/02_Evaluator/testEvaluator: Tests/02_Evaluator/main.o $(OBJECTS) $(SILLY_GC_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
Tests/03_Numbers/testNumbers: Tests/03_Numbers/main.o $(OBJECTS) $(SILLY_GC_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
Formatters/Math2.o: Formatters/Math2.c Formatters/Math2 Values/Values include/6D/Values include/6D/Operations include/6D/Evaluators include/6D/FFIs strict/SpecialForms/SpecialForms lazy/SpecialForms/SpecialForms strict/SpecialForms/Quoter Builtins/Builtins
Parsers/Lang5D.o: Parsers/Lang5D.c Scanners/Scanner Parsers/ShuntingYardParser Combinators/Combinators include/6D/Values include/6D/FFIs include/6D/Allocators Numbers/Integer2 include/6D/Operations Parsers/ShuntingYardParser.inc include/6D/Evaluators include/6D/Builtins Arithmetics/Arithmetics
Scanners/Scanner.o: Scanners/Scanner.c Scanners/Scanner  include/6D/Values include/6D/FFIs
Values/Values.c.proto Values/Values.o: Values/Values.c Values/Values Values/Symbol Values/Keyword Allocators/Allocators include/6D/Allocators include/6D/Values Numbers/Integer2 Numbers/Real Numbers/Ratio
	$(CC) -aux-info $<.proto.new -c -o $@ $< $(CFLAGS) && mv $<.proto.new $<.proto
Values/Keyword.o: Values/Keyword.c Values/Keyword Values/Hashtable include/6D/Allocators include/6D/Values
Values/Symbol.o: Values/Symbol.c Values/Symbol Values/Hashtable include/6D/Allocators include/6D/Values
Values/Error.o: Values/Error.c Values/Error Values/Values include/6D/Values
Allocators/Allocators.o: Allocators/Allocators.c Allocators/Allocators include/6D/Allocators
Tests/01_ShuntingYard/main.o: Tests/01_ShuntingYard/main.c Parsers/Lang5D Formatters/Math2 include/6D/Values include/6D/Allocators Tests/sillyprint.inc
Tests/02_Evaluator/main.o: Tests/02_Evaluator/main.c Parsers/Lang5D Formatters/Math2 include/6D/Allocators include/6D/Values include/6D/Evaluators include/6D/Builtins Tests/sillyprint.inc
Tests/03_Numbers/main.o: Tests/03_Numbers/main.c Parsers/Lang5D Formatters/Math2 include/6D/Allocators include/6D/Values include/6D/Evaluators Tests/sillyprint.inc include/6D/Arithmetics
Values/wstrings.o: Values/wstrings.c include/6D/wstrings Allocators/Allocators include/6D/Allocators
Evaluators/Evaluators.o: Evaluators/Evaluators.c strict/SpecialForms/SpecialForms include/6D/Operations  lazy/SpecialForms/SpecialForms Values/Values Values/Error
Numbers/Integer2.o Numbers/Integer2.c.proto: Numbers/Integer2.c Numbers/Integer2 Numbers/Small include/6D/FFIs include/6D/Values Values/Values
	$(CC) -aux-info $<.proto.new -c -o $@ $< $(CFLAGS) && mv $<.proto.new $<.proto
Numbers/Real.o: Numbers/Real.c Numbers/Real Numbers/Small Numbers/Integer2 include/6D/FFIs include/6D/Values Values/Values
Numbers/Ratio.o: Numbers/Ratio.c Numbers/Ratio Numbers/Small Numbers/Real Numbers/Integer2 include/6D/FFIs include/6D/Values Values/Values
strict/SpecialForms/Quoter.o: strict/SpecialForms/Quoter.c strict/SpecialForms/Quoter strict/SpecialForms/SpecialForms include/6D/Operations include/6D/Values
strict/SpecialForms/Constanter.o: strict/SpecialForms/Constanter.c strict/SpecialForms/Constanter strict/SpecialForms/SpecialForms include/6D/Operations include/6D/Values
strict/SpecialForms/Unapplier.o: strict/SpecialForms/Unapplier.c strict/SpecialForms/Unapplier strict/SpecialForms/SpecialForms include/6D/Operations include/6D/Values include/6D/Evaluators
strict/SpecialForms/SpecialForms.o: strict/SpecialForms/SpecialForms.c strict/SpecialForms/SpecialForms include/6D/FFIs include/6D/Values Values/Values
lazy/Logic/Logic.o: lazy/Logic/Logic.c lazy/Logic/Logic include/6D/Values include/6D/Operations include/6D/Evaluators include/6D/FFIs
strict/Logic/Logic.o: strict/Logic/Logic.c strict/Logic/Logic include/6D/Values include/6D/Operations include/6D/Evaluators strict/SpecialForms/SpecialForms include/6D/Operations  Combinators/Combinators
Combinators/Combinators.o: Combinators/Combinators.c Combinators/Combinators include/6D/Operations include/6D/Evaluators
Modulesystem/Memoizer.o: Modulesystem/Memoizer.c Modulesystem/Memoizer Values/Values Values/Hashtable include/6D/Evaluators include/6D/Values include/6D/FFIs
Modulesystem/Exports.o: Modulesystem/Exports.c include/6D/Operations include/6D/FFIs include/6D/Values include/6D/Modulesystem
Builtins/Builtins.o: Builtins/Builtins.c Builtins/Builtins strict/SpecialForms/SpecialForms include/6D/Operations  lazy/SpecialForms/SpecialForms include/6D/Builtins Modulesystem/Memoizer include/6D/Modulesystem Strings/Strings
FFIs/Converters.o: FFIs/Converters.c FFIs/Converters Numbers/Integer2 Numbers/Real Numbers/Ratio include/6D/Values include/6D/FFIs strict/Logic/Logic lazy/Logic/Logic
Values/Hashtable.o: Values/Hashtable.c Values/Hashtable Values/Values include/6D/Values include/6D/Allocators
Strings/Strings.o: Strings/Strings.c Strings/Strings include/6D/Values include/6D/Allocators Numbers/Integer2
Builtins/wrapper.c: Values/Values.o Numbers/Integer2.o $(STRICT_OBJECTS) OPLs/MinimalOPL.o Makefile generateWrapper Numbers/Integer2.c.proto Values/Values.c.proto
	$(OBJDUMP) -t Values/Values.o Numbers/Integer2.o $(STRICT_OBJECTS) OPLs/MinimalOPL.o |grep 'g.....F' | awk '{ print $$6 }' | ./generateWrapper Numbers/Integer2.c.proto Values/Values.c.proto OPLs/MinimalOPL.c.proto > Builtins/wrapper.c.new && mv Builtins/wrapper.c.new Builtins/wrapper.c
Builtins/wrapper.o: Builtins/wrapper.c include/6D/Values include/6D/Allocators Numbers/Integer2  
OPLs/MinimalOPL.c.proto OPLs/MinimalOPL.o: OPLs/MinimalOPL.c include/6D/Values include/6D/Allocators Numbers/Integer2 include/6D/Operations include/6D/Evaluators include/6D/FFIs
	$(CC) -aux-info $<.proto.new -c -o $@ $< $(CFLAGS) && mv $<.proto.new $<.proto
Arithmetics/Arithmetics.o: Arithmetics/Arithmetics.c Arithmetics/Arithmetics include/6D/Allocators Numbers/Integer2 include/6D/Values
	$(CC) -c -o $@ $< $(CFLAGS) && mv $<.proto.new $<.proto
Allocators/SillyGC.o: Allocators/SillyGC.cc Allocators/SillyGC include/6D/Values
	$(CXX) -o $@ -c $< $(CFLAGS)
Arithmetics/Arithmetics.o: Arithmetics/Arithmetics.c  Numbers/Integer2 include/6D/FFIs include/6D/Values Values/Values include/6D/Evaluators
test: Tests/01_ShuntingYard/testShuntingYard Tests/03_Numbers/testNumbers Tests/02_Evaluator/testEvaluator
	Tests/01_ShuntingYard/run
	Tests/03_Numbers/run
	Tests/02_Evaluator/run

all:
	$(MAKE) -C OPLs all
	$(MAKE) -C Arithmetics all
	$(MAKE) -C Trigonometry all
	$(MAKE) -C Logic all
	$(MAKE) -C Composition all
	$(MAKE) -C Lists all
	$(MAKE) -C Pairs all
	$(MAKE) -C Sets all
	$(MAKE) -C OS all
	$(MAKE) -C IO all
	$(MAKE) -C Maybe all
	$(MAKE) -C UI all
	$(MAKE) -C FFIs all
	$(MAKE) -C Strings all
	$(MAKE) -C Reflections all
	$(MAKE) -C Errors all
	$(MAKE) -C Objects all
	$(MAKE) -C Trees all
	$(MAKE) -C Evaluators all
	$(MAKE) -C Parsers all
	$(MAKE) -C Assemblers all
	$(MAKE) -C Formatters all

clean:
	rm -f $(OBJECTS) $(SILLY_GC_OBJECTS)
	$(MAKE) -C OPLs clean
	$(MAKE) -C Arithmetics clean
	$(MAKE) -C Trigonometry clean
	$(MAKE) -C Logic clean
	$(MAKE) -C Composition clean
	$(MAKE) -C Lists clean
	$(MAKE) -C Pairs clean
	$(MAKE) -C Sets clean
	$(MAKE) -C OS clean
	$(MAKE) -C IO clean
	$(MAKE) -C Maybe clean
	$(MAKE) -C UI clean
	$(MAKE) -C FFIs clean
	$(MAKE) -C Strings clean
	$(MAKE) -C Reflections clean
	$(MAKE) -C Errors clean
	$(MAKE) -C Objects clean
	$(MAKE) -C Trees clean
	$(MAKE) -C Evaluators clean
	$(MAKE) -C Parsers clean
	$(MAKE) -C Assemblers clean
	$(MAKE) -C Formatters clean

distclean:
	rm -f $(OBJECTS)
	rm -f $(LIB) $(STATIC_LIB)
	$(MAKE) -C OPLs distclean
	$(MAKE) -C Arithmetics distclean
	$(MAKE) -C Trigonometry distclean
	$(MAKE) -C Logic distclean
	$(MAKE) -C Composition distclean
	$(MAKE) -C Lists distclean
	$(MAKE) -C Pairs distclean
	$(MAKE) -C Sets distclean
	$(MAKE) -C OS distclean
	$(MAKE) -C IO distclean
	$(MAKE) -C Maybe distclean
	$(MAKE) -C UI distclean
	$(MAKE) -C FFIs distclean
	$(MAKE) -C Strings distclean
	$(MAKE) -C Reflections distclean
	$(MAKE) -C Errors distclean
	$(MAKE) -C Objects distclean
	$(MAKE) -C Trees distclean
	$(MAKE) -C Evaluators distclean
	$(MAKE) -C Parsers distclean
	$(MAKE) -C Assemblers distclean
	$(MAKE) -C Formatters distclean

install:
	$(MAKE) -C OPLs install
	$(MAKE) -C Arithmetics install
	$(MAKE) -C Trigonometry install
	$(MAKE) -C Logic install
	$(MAKE) -C Composition install
	$(MAKE) -C Lists install
	$(MAKE) -C Pairs install
	$(MAKE) -C Sets install
	$(MAKE) -C OS install
	$(MAKE) -C IO install
	$(MAKE) -C Maybe install
	$(MAKE) -C UI install
	$(MAKE) -C FFIs install
	$(MAKE) -C Strings install
	$(MAKE) -C Reflections install
	$(MAKE) -C Errors install
	$(MAKE) -C Objects install
	$(MAKE) -C Trees install
	$(MAKE) -C Evaluators install
	$(MAKE) -C Parsers install
	$(MAKE) -C Assemblers install
	$(MAKE) -C Formatters install

dist:
	rm -f $(PACKAGE)-$(VERSION)
	ln -s . $(PACKAGE)-$(VERSION)
	tar cvf $(PACKAGE)-$(VERSION).tar $(DISTFILES)
	rm -f $(PACKAGE)-$(VERSION)
