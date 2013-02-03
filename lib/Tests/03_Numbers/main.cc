#include <stdio.h>
#include "Parsers/Lang5D"
#include "Formatters/TExpression"
#include "6D/Values"
#include "6D/Evaluators"
int main(int argc, char* argv[]) {
	Allocators::initAllocators();
	Values::initIntegers();
	Parsers::Lang5D lang5D;
	Evaluators::initEvaluator();
	Values::NodeT prog = argc > 1 ? lang5D.parse1(fopen(argv[1], "r"), argv[1]) : lang5D.parse1(stdin, "<stdin>");
	//Formatters::TExpression::print(stderr, prog);
	//fprintf(stderr, "\n");
	prog = Evaluators::annotate(lang5D.defaultDynEnv, lang5D.withDefaultEnv(prog));
	//Formatters::TExpression::print(stderr, prog);
	//fprintf(stderr, "\n");
	prog = Evaluators::eval(prog);
	Formatters::TExpression::print(stdout, prog);
	fflush(stdout);
	return 0;
}
