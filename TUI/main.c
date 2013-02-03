#include <stdio.h>
#include "6D/Allocators"
#include "6D/Values"
#include "6D/Evaluators"
#include "6D/Operations"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Numbers)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(Parsers)
USE_NAMESPACE_6D(Formatters::TExpression)
int main() {
	const char* s;
	char buffer[2049];
	initAllocators();
	initIntegers();
	initLang5D();
	initEvaluator();
	//Values::NodeT annotate(Values::NodeT environment, Values::NodeT node);
	//Values::NodeT eval(Values::NodeT node);
	//Values::NodeT execute(Values::NodeT term);
	while((s = fgets(buffer, 2048, stdin))) { /* TODO handle longer lines */
		NodeT prog;
		FILE* f = fmemopen(s, strlen(s), "r");
		prog = L_parse1(f, "<stdin>");
		fclose(f);
		print(stderr, prog);
		fprintf(stderr, "\n");
		fflush(stderr);
		increaseGeneration();
	}
	return 0;
}
