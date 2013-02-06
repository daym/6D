#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Numbers/Integer2"
#include "6D/Values"
#include "6D/Operations"
#include "6D/Evaluators"
#include "6D/FFIs"
#include "Formatters/Math2"
#include "SpecialForms/SpecialForms"
#include "Builtins/Builtins"
#include "OPLs/MinimalOPL"
BEGIN_NAMESPACE_6D(Formatters)
USE_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
static NodeT Sbackslash;
static NodeT Sspace;
static NodeT Sslash;
static NodeT Scomma;
static NodeT SevalError;
static NodeT evalErrorF;
static NodeT SminimalOPLLevel;
static NodeT SminimalOPLArgcount;
/*
things to synthesize:
	operations
	'let forms, if 1) long enough, 2) not to deep already.
	'[] lists: keep short lists on one line, split long lists
	'import forms if possible (would require sorting let forms by something like an expression equality).
	keep track of operator precedence and associativity.
	indents and dedents instead of braces within longer functions.
things to keep track of:
	subexpression (horizontal maximum) hypothetical length
	names of all the bound variables.
	external names, if any.
	operator precedence
*/
struct Formatter {
	FILE* outputStream;
	int hposition;
	int maxWidthAccu;
	NodeT names;
	int operatorPrecedenceLimit;
	NodeT levelOfOperator;
	NodeT argcountOfOperator;
	int indentation;
};
static int Formatter_levelOfOperator(struct Formatter* self, NodeT operator_) {
	int result;
	if(!self->levelOfOperator)
		return (NO_OPERATOR);
	NodeT node = dcall(self->levelOfOperator, operator_);
	if(!intFromNode(node, &result))
		abort();
	return result;
}
static int Formatter_argcountOfOperator(struct Formatter* self, NodeT operator_) {
	int result;
	NodeT node = dcall(self->argcountOfOperator, operator_);
	if(!intFromNode(node, &result))
		abort();
	return result;
}
// TODO static struct Formatter silentFormatter;
static INLINE NodeT Formatter_printChar(struct Formatter* self, char c) {
	if(fputc(c, self->outputStream) == EOF)
		return evalError(strC("<stream>"), strC("<broken-stream>"), nil);
	if(c == '\n')
		self->hposition = 0;
	else {
		++self->hposition;
		if(self->hposition > self->maxWidthAccu)
			self->maxWidthAccu = self->hposition;
	}
	return nil;
}
static const char* hexdigits = "0123456789abcdef";
static NodeT Formatter_printStr2(struct Formatter* self, size_t sz, const char* data) {
	NodeT status = nil;
	for(; sz > 0; ++data, --sz) {
		unsigned char c = *data;
		switch(c) {
		case '\n':
			status = status ? status : Formatter_printChar(self, '\\');
			status = status ? status : Formatter_printChar(self, 'n');
			break;
		case '\r':
			status = status ? status : Formatter_printChar(self, '\\');
			status = status ? status : Formatter_printChar(self, 'r');
			break;
		case '\t':
			status = status ? status : Formatter_printChar(self, '\\');
			status = status ? status : Formatter_printChar(self, 't');
			break;
		case '\b':
			status = status ? status : Formatter_printChar(self, '\\');
			status = status ? status : Formatter_printChar(self, 'b');
			break;
		case '\a':
			status = status ? status : Formatter_printChar(self, '\\');
			status = status ? status : Formatter_printChar(self, 'a');
			break;
		case '\v':
			status = status ? status : Formatter_printChar(self, '\\');
			status = status ? status : Formatter_printChar(self, 'v');
			break;
		default:
			if(c < 32) {
				status = status ? status : Formatter_printChar(self, '\\');
				status = status ? status : Formatter_printChar(self, 'x');
				status = status ? status : Formatter_printChar(self, hexdigits[(c >> 4)&0xF]);
				status = status ? status : Formatter_printChar(self, hexdigits[c&0xF]);
			} else
				status = status ? status : Formatter_printChar(self, c);
			break;
		}
		if(status)
			return status;
	}
	return nil;
}
static NodeT getSymbolByIndex(int index, NodeT names) {
	if(names == nil)
		return nil;
	else if(index == 0)
		return getConsHead(names);
	else
		return getSymbolByIndex(index - 1, getConsTail(names));
}
static NodeT Formatter_printSymbol(struct Formatter* self, NodeT node) {
	const char* s;
	/* TODO support escaping? */
	s = getSymbol1Name(node);
	if(!s)
		return evalError(strC("<symbol>"), strC("<junk>"), node);
	return Formatter_printStr2(self, strlen(s), s);
}
static NodeT Formatter_printSymbolreference(struct Formatter* self, int index) {
	NodeT sym = getSymbolByIndex(index, self->names);
	return Formatter_printSymbol(self, sym);
}
static NodeT Formatter_printBinaryOperation(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	NodeT o = getOperationOperator(node);
	NodeT a = getOperationArgument1(node);
	NodeT b = getOperationArgument2(node);
	int oldLimit = self->operatorPrecedenceLimit;
	int precedence = Formatter_levelOfOperator(self, o);
	bool bParen = (precedence < oldLimit);
	self->operatorPrecedenceLimit = precedence;
	if(bParen)
		status = status ? status : Formatter_printChar(self, '(');
	status = status ? status : Formatter_print(self, a);
	status = status ? status : Formatter_printSymbol(self, o);
	status = status ? status : Formatter_print(self, b);
	if(bParen)
		status = status ? status : Formatter_printChar(self, ')');
	self->operatorPrecedenceLimit = oldLimit;
	return status;
}
static NodeT Formatter_printCall(struct Formatter* self, NodeT node) {
	if(binaryOperationP(node) && Formatter_levelOfOperator(self, node) != NO_OPERATOR)
		return Formatter_printBinaryOperation(self, node);
	else {
		NodeT callable = getCallCallable(node);
		NodeT argument = getCallArgument(node);
		return Formatter_printBinaryOperation(self, operation(Sspace, callable, argument));
	}
}
static NodeT Formatter_printFn(struct Formatter* self, NodeT node) {
	NodeT parameter = getFnParameter(node);
	NodeT body = getFnBody(node);
	/* TODO special-case */
	return Formatter_printBinaryOperation(self, operation(Sbackslash, parameter, body));
}
static NodeT Formatter_printKeyword(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	const char* s;
	/* TODO support escaping? */
	s = getKeyword1Name(node);
	if(!s)
		return evalError(strC("<symbol>"), strC("<junk>"), node);
	status = status ? status : Formatter_printChar(self, '@');
	status = status ? status : Formatter_printStr2(self, strlen(s), s);
	return status;
}
static NodeT Formatter_printStr(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	char* s;
	if(!stringFromNode(node, &s))
		status = evalError(strC("<str>"), strC("<junk>"), node);
	status = status ? status : Formatter_printChar(self, '"');
	status = status ? status : Formatter_printStr2(self, getStrSize(node), s);
	status = status ? status : Formatter_printChar(self, '"');
	return status;
}
static NodeT Formatter_printBox(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	status = status ? status : Formatter_printChar(self, '<');
	status = status ? status : Formatter_printChar(self, 'b');
	status = status ? status : Formatter_printChar(self, 'o');
	status = status ? status : Formatter_printChar(self, 'x');
	status = status ? status : Formatter_printChar(self, '>');
	return status;
}
static NodeT Formatter_printCons(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	NodeT tl;
	status = status ? status : Formatter_printChar(self, '[');
	assert(consP(node));
	tl = getConsTail(node);
	if(pairP(tl)) {
		return Formatter_print(self, operation(Scomma, getPairFst(node), getPairSnd(node)));
	}
	/* FIXME (!!!) proper parentheses (i.e. everywhere except for single-symbol) */
	status = status ? status : Formatter_print(self, getConsHead(node));
	if(!status) {
		for(node = getConsTail(node); consP(node); node = getConsTail(node)) {
			/* FIXME newline */
			status = status ? status : Formatter_printChar(self, ' ');
			if(status)
				return status;
			status = status ? status : Formatter_print(self, getConsHead(node));
		}
		if(!nilP(node)) {
			status = status ? status : Formatter_printChar(self, ',');
			status = status ? status : Formatter_print(self, node);
		}
	}
	status = status ? status : Formatter_printChar(self, ']');
	return status;
}
static NodeT Formatter_printNil(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	status = status ? status : Formatter_printChar(self, '[');
	status = status ? status : Formatter_printChar(self, ']');
	return status;
}
static NodeT Formatter_printRatio(struct Formatter* self, NodeT node) {
	NodeT a = getRatioA(node);
	NodeT b = getRatioB(node);
	return Formatter_printBinaryOperation(self, operation(Sslash, a, b));
}
static NodeT Formatter_printQuote2(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	abort();
	return status;
}
static NodeT Formatter_printInt2(struct Formatter* self, NativeInt value) {
	NodeT status = nil;
	NativeInt quot, rem;
	quot = (NativeInt) value/10;
	rem = value%10;
	/* C oddity */
	if(rem < 0)
		rem = -rem;
	if(quot != 0)
		status = status ? status : Formatter_printInt2(self, quot);
	status = status ? status : Formatter_printChar(self, hexdigits[rem]);
	return status;
}
static NodeT Formatter_printInteger2(struct Formatter* self, NodeT value) {
	NodeT status = nil;
	NodeT remN;
	NativeInt rem;
	NodeT qr;
	qr = integerDivmodU(value, 10);
	value = getPairFst(qr);
	remN = getPairSnd(qr);
	if(!toNativeInt(remN, &rem) || rem < 0)
		return evalError(strC("<integer-remainder>"), strC("<junk>"), remN);
	if(value != 0)
		status = status ? status : Formatter_printInteger2(self, value);
	status = status ? status : Formatter_printChar(self, hexdigits[rem]);
	return status;
}
static NodeT Formatter_printInt(struct Formatter* self, NodeT node) {
	NativeInt value;
	NodeT status = nil;
	if(!toNativeInt(node, &value))
		return evalError(strC("<int>"), strC("<junk>"), node);
	/* FIXME extra parens if needed */
	if(value < 0)
		status = status ? status : Formatter_printChar(self, '-');
	else { /* two's complement has one more on the negative side. So sorry, that's the only safe direction. */
		value = -value;
	}
	status = status ? status : Formatter_printInt2(self, value);
	return status;
}
static NodeT Formatter_printFloat(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	abort();
	return status;
}
static NodeT Formatter_printInteger(struct Formatter* self, NodeT node) {
	NodeT status = nil;
	if(integerCompareU(node, 0) < 0) {
		/* FIXME extra parens if needed */
		status = status ? status : Formatter_printChar(self, '-');
	}
	status = status ? status : Formatter_printInteger2(self, node);
	return status;
}
static NodeT Formatter_printError(struct Formatter* self, NodeT node) {
	NodeT kind = getErrorKind(node);
	NodeT expectedInput = getErrorExpectedInput(node);
	NodeT gotInput = getErrorGotInput(node);
	NodeT context = getErrorContext(node);
	return Formatter_print(self, call5(evalErrorF, kind, expectedInput, gotInput, context));
}
NodeT Formatter_print(struct Formatter* self, NodeT node) {
	int i;
	return (i = getSymbolreferenceIndex(node)) != -1 ? Formatter_printSymbolreference(self, i) : 
	       symbolP(node) ? Formatter_printSymbol(self, node) : 
	       keywordP(node) ? Formatter_printKeyword(self, node) : 
	       callP(node) ? Formatter_printCall(self, node) : 
	       fnP(node) ? Formatter_printFn(self, node) : 
	       consP(node) ? Formatter_printCons(self, node) : 
	       nilP(node) ? Formatter_printNil(self, node) : 
	       ratioP(node) ? Formatter_printRatio(self, node) : 
	       quote2P(node) ? Formatter_printQuote2(self, node) : 
	       intP(node) ? Formatter_printInt(self, node) : 
	       floatP(node) ? Formatter_printFloat(self, node) : 
	       integerP(node) ? Formatter_printInteger(self, node) : 
	       strP(node) ? Formatter_printStr(self, node) : 
	       errorP(node) ? Formatter_printError(self, node) : 
	       boxP(node) ? Formatter_printBox(self, node) : 
	       evalError(strC("<printable>"), strC("<unprintable>"), node);
}
static NodeT minimalOperatorLevel;
static NodeT minimalOperatorArgcount;
void Formatter_init(struct Formatter* self, FILE* outputStream, int hposition, int maxWidthAccu, NodeT names, int operatorPrecedenceLimit, NodeT levelOfOperator, NodeT argcountOfOperator, int indentation) {
	self->outputStream = outputStream;
	self->hposition = hposition;
	self->maxWidthAccu = maxWidthAccu;
	self->names = names;
	self->operatorPrecedenceLimit = operatorPrecedenceLimit;
	self->levelOfOperator = levelOfOperator;
	self->argcountOfOperator = argcountOfOperator;
	self->indentation = indentation;
}
void initMathFormatters(void) {
	if(!Sspace) {
		NodeT builtins = initBuiltins();
		Sspace = symbolFromStr(" ");
		Sbackslash = symbolFromStr("\\");
		Sslash = symbolFromStr("/");
		Scomma = symbolFromStr(",");
		SevalError = symbolFromStr("evalError");
		SminimalOPLLevel = symbolFromStr("minimalOPLLevel");
		SminimalOPLArgcount = symbolFromStr("minimalOPLArgcount");
		evalErrorF = dcall(builtins, quote2(SevalError));
		minimalOperatorLevel = dcall(builtins, SminimalOPLLevel);
		minimalOperatorArgcount = dcall(builtins, SminimalOPLArgcount);
	}
}
//include "Parsers/sillyprint.inc"
/* just for backwards compat, not exactly fast or correct or reliable: */
void print(FILE* f, NodeT node) {
	struct Formatter fmt;
	initMathFormatters();
	Formatter_init(&fmt, f, 0, 0, nil, 0, nil/*minimalOperatorLevel*/, minimalOperatorArgcount, 0);
	Formatter_print(&fmt, node);
}
END_NAMESPACE_6D(Formatters)
