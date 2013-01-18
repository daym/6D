#include "Values/Values"
#include "Formatters/TExpression"
namespace Formatters {
	namespace TExpression {
void print(FILE* destination, Values::NodeT node) {
	using namespace Values;
	if(symbolP(node)) {
		fprintf(destination, "%s", getSymbol1Name(node));
	} else if(keywordP(node)) {
		fprintf(destination, "%s", getKeyword1Name(node));
	} else if(callP(node)) {
		fprintf(destination, "(");
		NodeT callable = getCallCallable(node);
		if(callP(callable)) {
			print(destination, getCallCallable(callable));
			fprintf(destination, " ");
			print(destination, getCallArgument(callable));
		} else
			print(destination, callable);
		fprintf(destination, " ");
		print(destination, getCallArgument(node));
		fprintf(destination, ")");
	} else if(fnP(node)) {
		fprintf(destination, "(\\");
		print(destination, getFnParameter(node));
		fprintf(destination, " ");
		NodeT body = getFnBody(node);
		if(fnP(body)) {
			fprintf(destination, "\\");
			print(destination, getFnParameter(body));
			fprintf(destination, " ");
			print(destination, getFnBody(body));
		} else
			print(destination, body);
		fprintf(destination, ")");
	} else if(consP(node)) {
		fprintf(destination, "[");
		print(destination, getConsHead(node));
		Values::NodeT tail = getConsTail(node);
		if(consP(tail)) {
			for(; consP(tail); tail = getConsTail(tail)) {
				fprintf(destination, " ");
				print(destination, getConsHead(tail));
			}
			if(tail) {
				fprintf(destination, ",");
				print(destination, tail);
			}
		} else {
			fprintf(destination, " ");
			print(destination, getConsTail(node));
		}
		fprintf(destination, "]");
	} else if(nilP(node))
		fprintf(destination, "[]");
	else {
		Values::str(node, destination);
	}
}
	}}