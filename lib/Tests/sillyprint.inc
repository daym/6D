#include "6D/FFIs"
#include "SpecialForms/SpecialForms"
static void S_print(NodeT n);

static void S_printSymbolreference(int i) {
	printf("%d", i);
}
static void S_printSymbol(NodeT n) {
	printf("%s", getSymbol1Name(n));
}
static void S_printKeyword(NodeT n) {
	printf("@%s", getKeyword1Name(n));
}
static void S_printCall(NodeT n) {
	NodeT fn = getCallCallable(n);
	NodeT arg = getCallArgument(n);
	printf("(");
	S_print(fn);
	printf(" ");
	S_print(arg);
	printf(")");
}
static void S_printFn(NodeT n) {
	NodeT param = getFnParameter(n);
	NodeT body = getFnBody(n);
	printf("(\\");
	S_print(param);
	printf(" ");
	S_print(body);
	printf(")");
}
static void S_printCons(NodeT n) {
	NodeT hd = getConsHead(n);
	NodeT tl = getConsTail(n);
	printf("(");
	S_print(hd);
	printf(" ");
	S_print(tl);
	printf(")");
}
static void S_printNil(NodeT n) {
	printf("nil");
}
static void S_printRatio(NodeT n) {
	NodeT a = getRatioA(n);
	NodeT b = getRatioB(n);
	printf("(ratio ");
	S_print(a);
	printf(" ");
	S_print(b);
	printf(")");
}
static void S_printQuote2(NodeT n) {
	printf("Q");
}
static void S_printInt(NodeT n) {
	NativeInt value;
	if(!toNativeInt(n, &value))
		abort();
	printf("%ld", value);
}
static void S_printFloat(NodeT n) {
	NativeFloat value;
	if(!toNativeFloat(n, &value))
		abort();
	printf("%Lf", value);
}
static void S_printInteger(NodeT n) {
	abort();
}
static void S_printStr(NodeT n) {
	char* value;
	if(!stringFromNode(n, &value))
		abort();
	printf("%s", value);
}
static void S_printError(NodeT n) {
	NodeT kind = getErrorKind(n);
	NodeT expectedInput = getErrorExpectedInput(n);
	NodeT gotInput = getErrorGotInput(n);
	NodeT context = getErrorContext(n);
	printf("(error ");
	S_print(kind);
	printf(" ");
	S_print(expectedInput);
	printf(" ");
	S_print(gotInput);
	printf(" ");
	S_print(context);
	printf(")");
}
static void S_printBox(NodeT n) {
	printf("<box>");
}
static void S_printFFIFn(NodeT n) {
	printf("<FFIFn>");
}
static void S_print(NodeT node) {
	int i;
	(i = getSymbolreferenceIndex(node)) != -1 ? S_printSymbolreference(i) : 
	       symbolP(node) ? S_printSymbol(node) : 
	       keywordP(node) ? S_printKeyword(node) : 
	       callP(node) ? S_printCall(node) : 
	       fnP(node) ? S_printFn(node) : 
	       consP(node) ? S_printCons(node) : 
	       nilP(node) ? S_printNil(node) : 
	       ratioP(node) ? S_printRatio(node) : 
	       quote2P(node) ? S_printQuote2(node) : 
	       intP(node) ? S_printInt(node) : 
	       floatP(node) ? S_printFloat(node) : 
	       integerP(node) ? S_printInteger(node) : 
	       strP(node) ? S_printStr(node) : 
	       errorP(node) ? S_printError(node) : 
	       boxP(node) ? S_printBox(node) : 
	       FFIFnP(node) ? S_printFFIFn(node) : 
	       evalError(strC("<printable>"), strC("<unprintable>"), node);
}
