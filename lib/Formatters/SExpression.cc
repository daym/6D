#include "Values/Values"
#include "Formatters/SExpression"
BEGIN_NAMESPACE_6D(Formatters)
BEGIN_NAMESPACE_6D(SExpression)
void print(FILE* destination, Values::NodeT node) {
	if(symbolP(node)) {
		fprintf(destination, "%s", getSymbol1Name(node));
	} else if(keywordP(node)) {
		fprintf(destination, "%s", getKeyword1Name(node));
	} else if(callP(node)) {
		fprintf(destination, "(");
		print(destination, getCallCallable(node));
		fprintf(destination, " ");
		print(destination, getCallArgument(node));
		fprintf(destination, ")");
	} else if(fnP(node)) {
		fprintf(destination, "(\\");
		print(destination, getFnParameter(node));
		fprintf(destination, " ");
		print(destination, getFnBody(node));
		fprintf(destination, ")");
	} else {
		Values::str(node, destination);
	}
}
END_NAMESPACE_6D(SExpression)
END_NAMESPACE_6D(Formatters)
