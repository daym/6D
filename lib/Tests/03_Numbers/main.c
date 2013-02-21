#include <stdio.h>
#include "Parsers/Lang5D"
#include "6D/Values"
#include "6D/Evaluators"
#include "6D/Builtins"
#include "6D/Arithmetics"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Builtins)
USE_NAMESPACE_6D(Formatters::TExpression)
#include "../sillyprint.inc"
int main(int argc, char* argv[]) {
	initAllocators();
	initArithmetics();
	initEvaluators();
	NodeT defaultDynEnv = initLang();
	NodeT prog = argc > 1 ? Lang_parse1(fopen(argv[1], "r"), argv[1]) : Lang_parse1(stdin, "<stdin>");
	// TODO Memoize
	prog = closeOver(symbolFromStr("Builtins"), initBuiltins(), withArithmetics(Lang_withDefaultEnv(prog)));
	prog = annotate(defaultDynEnv, prog); //lang5D.withDefaultEnv(prog));
	prog = eval(prog);
	S_print(prog);
	fflush(stdout);
	if(errorP(prog))
		return 1;
	return 0;
}
