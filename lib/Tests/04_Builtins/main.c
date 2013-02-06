#include <stdio.h>
#include "Parsers/Lang5D"
#include "Formatters/TExpression"
#include "6D/Values"
#include "6D/Evaluators"
#include "6D/Builtins"
#include "6D/Arithmetic"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Builtins)
USE_NAMESPACE_6D(Formatters::TExpression)
int main(int argc, char* argv[]) {
	initAllocators();
	initArithmetic();
	initEvaluator();
	NodeT defaultDynEnv = initLang();
	NodeT prog = argc > 1 ? Lang_parse1(fopen(argv[1], "r"), argv[1]) : Lang_parse1(stdin, "<stdin>");
	// TODO Memoize
	prog = closeOver(symbolFromStr("Builtins"), initBuiltins(), withArithmetic(L_withDefaultEnv(prog)));
	//print(stderr, prog);
	//fprintf(stderr, "\n");
	prog = annotate(defaultDynEnv, prog); //lang5D.withDefaultEnv(prog));
	//print(stderr, prog);
	//fprintf(stderr, "\n");
	prog = eval(prog);
	print(stdout, prog);
	fflush(stdout);
	if(errorP(prog))
		return 1;
	return 0;
}
