#include <stdio.h>
#include "6D/Values"
#include "6D/Allocators"
#include "Parsers/Lang5D"
#include "Formatters/TExpression"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
int main(int argc, char* argv[]) {
	initAllocators();
	initIntegers();
	Parsers::Lang5D lang5D;
	NodeT prog = argc > 1 ? lang5D.parse1(fopen(argv[1], "r"), argv[1]) : lang5D.parse1(stdin, "<stdin>");
	Formatters::TExpression::print(stdout, prog);
	fflush(stdout);
	return 0;
}
