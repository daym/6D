#include <stdio.h>
#include "6D/Values"
#include "6D/Allocators"
#include "Parsers/Lang5D"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(Formatters::TExpression)
USE_NAMESPACE_6D(Parsers)
#include "../sillyprint.inc"
int main(int argc, char* argv[]) {
	initAllocators();
	initIntegers();
	initLang();
	NodeT prog = argc > 1 ? Lang_parse1(fopen(argv[1], "r"), argv[1]) : Lang_parse1(stdin, "<stdin>");
	S_print(prog);
	fflush(stdout);
	return 0;
}
