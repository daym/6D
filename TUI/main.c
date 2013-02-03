#include <stdio.h>
#include "6D/Allocators"
#include "6D/Values"
#include "6D/Evaluators"
#include "6D/Operations"
#include "6D/Lang5D"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Numbers)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(Parsers)
USE_NAMESPACE_6D(Formatters::TExpression)
void printPrompt(void) {
	fprintf(stderr, "eval $ ");
	fflush(stderr);
}
int main() {
	const char* s;
	char buffer[2049];
	initAllocators();
	initIntegers();
	NodeT defaultDynEnv = initLang5D();
	initEvaluator();
	//Values::NodeT annotate(Values::NodeT environment, Values::NodeT node);
	//Values::NodeT eval(Values::NodeT node);
	//Values::NodeT execute(Values::NodeT term);
	while((printPrompt(), s = fgets(buffer, 2048, stdin))) { /* TODO handle longer lines */
		NodeT prog;
		FILE* f = fmemopen(s, strlen(s), "r");
		prog = L_parse1(f, "<stdin>");
		fclose(f);
		print(stderr, prog);
		prog = annotate(defaultDynEnv, prog);
		prog = eval(prog);
		fprintf(stderr, "\n");
		fflush(stderr);
		print(stdout, prog);
		printf("\n");
		fflush(stdout);
		increaseGeneration();
	}
	return 0;
}
