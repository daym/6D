#include "Values/Values"
#include "Formatters/TExpression"
#include "SpecialForms/SpecialForms"
namespace Formatters {
	namespace TExpression {
using namespace Values;
static NodeT getSymbolByIndex(int index, NodeT names) {
	if(names == nil)
		return nil;
	else if(index == 0)
		return getConsHead(names);
	else
		return getSymbolByIndex(index - 1, getConsTail(names));
}
/* TODO:
    track the bindings of variables and backsubstitute if possible.
*/
void print0(FILE* destination, NodeT names, NodeT node) {
	using namespace Values;
	using namespace SpecialForms;
	int i;
	if((i = getSymbolreferenceIndex(node)) != -1) {
		NodeT node = getSymbolByIndex(i, names);
		/* FIXME what if node is nil? Just make names up maybe (putting them at the end of #names would be safe, too - and mimic how it should have looked if they were there) */
		fprintf(destination, "%s", getSymbol1Name(node));
	} else if(symbolP(node)) {
		fprintf(destination, "%s", getSymbol1Name(node));
	} else if(keywordP(node)) {
		fprintf(destination, "%s", getKeyword1Name(node));
	} else if(callP(node)) {
		fprintf(destination, "(");
		NodeT callable = getCallCallable(node);
		if(callP(callable)) {
			print0(destination, names, getCallCallable(callable));
			fprintf(destination, " ");
			print0(destination, names, getCallArgument(callable));
		} else
			print0(destination, names, callable);
		fprintf(destination, " ");
		print0(destination, names, getCallArgument(node));
		fprintf(destination, ")");
	} else if(fnP(node)) {
		fprintf(destination, "(\\");
		print0(destination, names, getFnParameter(node));
		names = cons(getFnParameter(node), names);
		fprintf(destination, " ");
		NodeT body = getFnBody(node);
		if(fnP(body)) {
			fprintf(destination, "\\");
			print0(destination, names, getFnParameter(body));
			names = cons(getFnParameter(body), names);
			fprintf(destination, " ");
			print0(destination, names, getFnBody(body));
		} else
			print0(destination, names, body);
		fprintf(destination, ")");
	} else if(consP(node)) {
		fprintf(destination, "[");
		print0(destination, names, getConsHead(node));
		NodeT tail = getConsTail(node);
		if(consP(tail)) {
			for(; consP(tail); tail = getConsTail(tail)) {
				fprintf(destination, " ");
				print0(destination, names, getConsHead(tail));
			}
			if(tail) {
				fprintf(destination, ",");
				print0(destination, names, tail);
			}
		} else {
			fprintf(destination, " ");
			print0(destination, names, getConsTail(node));
		}
		fprintf(destination, "]");
	} else if(nilP(node))
		fprintf(destination, "[]");
	else if(ratioP(node)) {
		print(destination, call2(symbolFromStr("/"), getRatioA(node), getRatioB(node)));
	} else if(SPECIAL_FORM_EQUAL_P(node, Quoter)) {
		fprintf(destination, "'");
	} else {
		str(node, destination);
	}
}
void print(FILE* destination, NodeT node) {
	print0(destination, nil, node);
}
	}}