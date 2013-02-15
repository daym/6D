
LIBGC_LD_WRAP_CFLAGS = -D_REENTRANT -DGC_THREADS -DUSE_LD_WRAP

CFLAGS += -Ilib/include $(LIBGC_LD_WRAP_CFLAGS) $(shell pkg-config --cflags glib-2.0 gthread-2.0) -g3 -O0
LDFLAGS += lib/lib6dbuiltins1.a -lgc -lm $(shell pkg-config --libs glib-2.0 gthread-2.0) -lreadline
all: TUI/T5D
TUI/T5D: TUI/main.o lib/lib6dbuiltins1.a
	$(CC) -o $@ $^ $(LDFLAGS)
TUI/main.o: TUI/main.c lib/include/6D/Arithmetics lib/include/6D/Values lib/include/6D/Operations lib/include/6D/Lang5D lib/include/6D/Allocators lib/include/6D/Evaluators lib/include/6D/Builtins
	$(CC) -c -o $@ $< $(CFLAGS)
#lib/GCs/SillyGC.o: lib/GCs/SillyGC.cc lib/GCs/SillyGC
