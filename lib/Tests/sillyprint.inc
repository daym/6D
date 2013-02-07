#include "6D/FFIs"
#include "SpecialForms/SpecialForms"
static void S_print(NodeT n);

static void S_printSymbolreference(int i) {
	printf("%d", i);
}
static void S_printSymbol(NodeT n) {
	printf("%s", symbolName(n));
}
static void S_printKeyword(NodeT n) {
	printf("@%s", keywordName(n));
}
static void S_printCall1(NodeT n) {
	NodeT fn = callCallable(n);
	NodeT arg = callArgument(n);
	if(callP(fn))
		S_printCall1(fn);
	else
		S_print(fn);
	printf(" ");
	S_print(arg);
}
static void S_printCall(NodeT n) {
	printf("(");
	S_printCall1(n);
	printf(")");
}
static void S_printFn1(NodeT n) {
	NodeT param = fnParameter(n);
	NodeT body = fnBody(n);
	printf("\\");
	S_print(param);
	printf(" ");
	if(fnP(body))
		S_printFn1(body);
	else
		S_print(body);
}
static void S_printFn(NodeT n) {
	printf("(");
	S_printFn1(n);
	printf(")");
}
static void S_printCons(NodeT n) {
	NodeT hd = consHead(n);
	NodeT tl = consTail(n);
	printf("(");
	S_print(hd);
	printf(" ");
	S_print(tl);
	printf(")");
}
static void S_printNil(NodeT n) {
	printf("[]");
}
static void S_printRatio(NodeT n) {
	NodeT a = ratioA(n);
	NodeT b = ratioB(n);
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
	printf(NATIVEINT_FORMAT, value);
}
static void S_printFloat(NodeT n) {
	NativeFloat value;
	if(!toNativeFloat(n, &value))
		abort();
	printf(NATIVEFLOAT_FORMAT, value);
}
static void S_printInteger(NodeT n) {
	abort();
}
static void S_printStr(NodeT n) {
	char* value;
	if(!stringFromNode(n, &value))
		abort();
	printf("\"%s\"", value); /* FIXME escape */
}
static void S_printError(NodeT n) {
	NodeT kind = errorKind(n);
	NodeT expectedInput = errorExpectedInput(n);
	NodeT gotInput = errorGotInput(n);
	NodeT context = errorContext(n);
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
static void S_printUnknown(NodeT n) {
	printf("<unknown>");
}
static void S_print(NodeT node) {
	int i;
	(i = symbolreferenceIndex(node)) != -1 ? S_printSymbolreference(i) : 
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
	       S_printUnknown(node);
}
