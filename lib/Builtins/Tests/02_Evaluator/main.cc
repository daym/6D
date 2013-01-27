#include <stdio.h>
#include "Scanners/Lang5D"
#include "Formatters/TExpression"
#include "6D/Values"
#include "6D/Evaluators"
int main(int argc, char* argv[]) {
	Scanners::Lang5D lang5D;
	Values::NodeT prog = argc > 1 ? lang5D.parse1(fopen(argv[1], "r"), argv[1]) : lang5D.parse1(stdin, "<stdin>");
	Formatters::TExpression::print(stderr, prog);
	fprintf(stderr, "\n");
	prog = Evaluators::annotate(lang5D.defaultDynEnv, lang5D.withDefaultEnv(prog));
	Formatters::TExpression::print(stderr, prog);
	fprintf(stderr, "\n");
	prog = Evaluators::eval(prog);
	Formatters::TExpression::print(stdout, prog);
	fflush(stdout);
	return 0;
}
