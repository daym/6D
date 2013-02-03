#include "6D/Values"
#include "Values/Values"
#include "Formatters/TExpression"
#include "SpecialForms/SpecialForms"
BEGIN_NAMESPACE_6D(Formatters)
BEGIN_NAMESPACE_6D(TExpression)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(SpecialForms)
static NodeT getSymbolByIndex(int index, NodeT names) {
	if(names == nil)
		return nil;
	else if(index == 0)
		return getConsHead(names);
	else
		return getSymbolByIndex(index - 1, getConsTail(names));
}
static void printStr(FILE* destination, const char* s, size_t size) {
	fputc('"', destination);
	size_t sz;
	for(sz = size; sz > 0; --sz, ++s) {
		char c = *s;
		if(c < 32) {
			switch(c) {
			case '\n':
				fprintf(destination, "\\n");
				break;
			case '\r':
				fprintf(destination, "\\r");
				break;
			case '\b':
				fprintf(destination, "\\b");
				break;
			case '\t':
				fprintf(destination, "\\t");
				break;
			case '\f':
				fprintf(destination, "\\f");
				break;
			case '\a':
				fprintf(destination, "\\a");
				break;
			case '\v':
				fprintf(destination, "\\v");
				break;
			default:
				fprintf(destination, "\\x%02X", c);
			}
		} else if(c == '\\' || c == '"' /*|| c == '\''*/) {
			fputc('\\', destination);
			fputc(c, destination);
		} else
			fputc(c, destination);
	}
	fputc('"', destination);
}
/* TODO:
	- track the bindings of variables and backsubstitute values if possible (WTF).
	- synthetisize let forms (always or just if the entire thing is smaller than some minimum size?)
*/
void print0(FILE* destination, NodeT names, NodeT node) {
	int i;
	if((i = getSymbolreferenceIndex(node)) != -1) {
		NodeT node = getSymbolByIndex(i, names);
		/* FIXME what if node is nil? Just make names up maybe (putting them at the end of #names would be safe, too - and mimic how it should have looked if they were there) */
		if(node)
			fprintf(destination, "%s", getSymbol1Name(node));
		else
			fprintf(destination, "<can't figure it out>");
	} else if(symbolP(node)) {
		fprintf(destination, "%s", getSymbol1Name(node));
	} else if(keywordP(node)) {
		fprintf(destination, "%s", getKeyword1Name(node));
	} else if(callP(node)) {
		fprintf(destination, "(");
		NodeT callable = getCallCallable(node);
		if(callP(callable)) {
			// TODO escape fn parameter if it is an operator
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
		// TODO escape fn parameter if it is an operator
		print0(destination, names, getFnParameter(node));
		names = cons(getFnParameter(node), names);
		fprintf(destination, " ");
		NodeT body = getFnBody(node);
		if(fnP(body)) {
			fprintf(destination, "\\");
			// TODO escape fn parameter if it is an operator
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
	} else if(strP(node)) {
		const struct Str* str = (const struct Str*) getCXXInstance(node);
		char* p;
		if(!stringFromNode(str, &p))
			abort();
		printStr(destination, p, getStrSize(str));
	} else {
		str(node, destination);
	}
}
void print(FILE* destination, NodeT node) {
	print0(destination, nil, node);
}
END_NAMESPACE_6D(TExpression)
END_NAMESPACE_6D(Formatter)
