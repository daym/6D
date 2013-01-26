#include <stdio.h>
#include "Scanners/Lang5D"
#include "Formatters/TExpression"
#include "Evaluators/Evaluators"
int main(int argc, char* argv[]) {
	Scanners::Lang5D lang5D;
	Values::NodeT prog = argc > 1 ? lang5D.parse1(fopen(argv[1], "r"), argv[1]) : lang5D.parse1(stdin, "<stdin>");
	prog = Evaluators::eval(NULL, prog);
	Formatters::TExpression::print(stdout, prog);
	fflush(stdout);
	return 0;
}
