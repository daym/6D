#include <stdio.h>
#include "6D/Values"
#include "6D/Allocators"
#include "Parsers/Lang5D"
#include "Formatters/TExpression"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(Formatters::TExpression)
USE_NAMESPACE_6D(Parsers)
int main(int argc, char* argv[]) {
	initAllocators();
	initIntegers();
	initLang5D();
	NodeT prog = argc > 1 ? L_parse1(fopen(argv[1], "r"), argv[1]) : L_parse1(stdin, "<stdin>");
	print(stdout, prog);
	fflush(stdout);
	return 0;
}
