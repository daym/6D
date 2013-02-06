#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <alloca.h>
#include "6D/Evaluators"
#include "6D/Allocators"
#include "6D/Values"
#include "6D/Builtins"
#include "Values/Values"
#include "Parsers/Lang5D"
#include "Formatters/Math2"
#include "Parsers/ShuntingYardParser"
#include "SpecialForms/SpecialForms"
#include "Modulesystem/Modulesystem"
#include "Combinators/Combinators"
#include "Numbers/Integer2"
#include "6D/FFIs"
#include "6D/Modulesystem"
#include "OPLs/MinimalOPL"
#undef GETC
#undef UNGETC
#define GETC fgetc(file)
#define UNGETC(c) ungetc(c, file)
BEGIN_NAMESPACE_6D(Parsers)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Combinators)
static NodeT SLF;
static NodeT Sindent;
static NodeT Sdedent;
static NodeT SopeningParen;
static NodeT Sapply;
static NodeT Sbackslash;
static NodeT Slet;
static NodeT Sin;
static NodeT Serror;
static NodeT Scomma;
static NodeT Sequal;
static NodeT Sin;
static NodeT Sfrom;
static NodeT Shashexports;
static NodeT Sleftparen;
static NodeT Sleftcurly;
static NodeT Sleftbracket;
static NodeT Srightparen;
static NodeT Srightcurly;
static NodeT Srightbracket;
static NodeT Squote;
//NodeT defaultDynEnv;
static NodeT Sexports;
static NodeT Snil;
static NodeT Scolonequal;
static NodeT Scolon;
static NodeT Sdot;
static NodeT Shasht;
static NodeT Shashf;
static NodeT SEOF;
static NodeT Simport;
static NodeT SoperatorLevel;
static NodeT SoperatorArgcount;
static NodeT SoperatorPrefixNeutral;
static NodeT minimalOPLN;
static NodeT SminimalOPL;
#include "sillyprint.inc"
//NodeT Sdot;
BEGIN_STRUCT_6D(Lang)
	NodeT OPL;
	NodeT operatorLevel;
	NodeT operatorArgcount;
	NodeT operatorPrefixNeutral;
END_STRUCT_6D(Lang)
static INLINE NodeT merror(const char* expectedPart, const char* gotPart) {
	return parseError(strC(expectedPart), strC(gotPart));
}
static INLINE const char* nvl(const char* a, const char* b) {
	return a ? a : b;
}
static int digitInBase(int base, int c) {
	int result = (c >= '0' && c <= '9') ? 0 +  (c - '0') :
	             (c >= 'a' && c <= 'z') ? 10 + (c - 'a') : 
	             (c >= 'A' && c <= 'Z') ? 10 + (c - 'A') : 
	             (-1);
	if(result >= base)
		result = -1;
	return result;
}
static INLINE NodeT getNumber(int baseI, const char* name) {
	errno = 0;
	if(strchr(name, '.')) { /* TODO other bases for float */
		NativeFloat value = strtod(name, NULL);
		if(errno == 0)
			return internNativeInt(value);
	} else {
		NativeInt value = strtol(name, NULL, baseI);
		if(errno == 0)
			return internNativeInt(value);
		else {
			int off;
			NodeT base = internNativeInt((NativeInt) baseI);
			NodeT result = internNativeInt((NativeInt) 0);
			for(;(off = digitInBase(baseI, *name)) != -1;++name) {
				result = integerMul(result, base);
				result = integerAddU(result, off);
			}
			if(*name == 0)
				return result;
		}
	}
	return nil;
}
static INLINE NodeT getDynEnvEntry(NodeT sym) {
	const char* name = getSymbol1Name(sym);
	NodeT result = nil;
	if(name) {
		if(isdigit(name[0])) { /* since there is an infinite number of numbers, make sure not to precreate all of them :-) */
			result = getNumber(10, name);
		} else if(name[0] == '#' && isdigit(name[1])) {
			char* p;
			errno = 0;
			NativeInt base = strtol(&name[1], &p, 10);
			if(errno == 0 && p && *p) {
				++p; /* skip 'r' */
				result = getNumber(base, p);
			}
		}
	}
	if(result)
		return result;
	fprintf(stderr, "info: expression was: ");
	print(stderr, sym);
	fprintf(stderr, "\n");
	fflush(stderr);
	return merror("<dynamic-variable>", nvl(getSymbol1Name(sym), "???"));
}
DEFINE_STRICT_FN(DynEnv, getDynEnvEntry(argument))

static bool mathUnicodeOperatorInRangeP(int codepoint) {
	return (codepoint >= 0x2200 && codepoint < 0x2300) ||
	       (codepoint >= 0x27C0 && codepoint < 0x27F0) || 
		   (codepoint >= 0x2980 && codepoint < 0x2A00) || 
		   (codepoint >= 0x2A00 && codepoint < 0x2B00) || 
		   (codepoint >= 0x2100 && codepoint < 0x2150) || 
		   (codepoint >= 0x25A0 && codepoint < 0x2600) ||
		   (codepoint >= 0x2B30 && codepoint < 0x2B4D);
}
static bool digitCharP(int input) {
	return (input >= '0' && input <= '9');
}
static bool digitRestCharP(int input) {
	return (input >= '0' && input <= '9') || (input == '.');
}
static bool specialCodingCharP(int input) {
	return input == '#';
}
static bool asciiIDCharP(int input) {
	return (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z') || input == '?' || input == '!';
}
static bool asciiID2CharP(int input) {
	return (asciiIDCharP(input) || digitCharP(input));
}
static bool unicodeIDCharP(int input) {
	return (asciiIDCharP(input) || (input >= 128 && input != 226));
}
static bool unicodeIDRestCharP(int input) {
	return (asciiIDCharP(input) || digitCharP(input) || (input >= 128 && input != 226));
}
static bool unicodeMaybeOperator1CharP(int input) {
	return input == 226;
}
static bool operatorCharP(int input) {
	switch(input) {
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case ',':
	case '-':
	case '.':
	case '/':
	case ':':
	case ';':
	case '<':
	case '=':
	case '>':
	case '\\':
	case '^':
	case '_':
	case '`':
	case '|':
	case '~':
		return true;
	default:
		return false;
	}
}
static bool braceCharP(int input) {
	switch(input) {
	case '(':
	case '{':
	case '[':
	case ')':
	case '}':
	case ']':
		return true;
	default:
		return false;
	}
}
static bool shebangBodyCharP(int input) {
	return (input && input != '\n');
}
static bool keywordBodyCharP(int input) {
	return asciiIDCharP(input); // TODO unicode
	//return (input != EOF && input != ':' && input != ' ' && input != '\t');
}
static bool octalBodyCharP(int input) {
	return (input >= '0' && input < '8');
}
static bool hexadecimalBodyCharP(int input) {
	switch(input) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'a':
	case 'B':
	case 'b':
	case 'C':
	case 'c':
	case 'D':
	case 'd':
	case 'E':
	case 'e':
	case 'F':
	case 'f':
		return true;
	default:
		return false;
	}
}
static bool binaryBodyCharP(int input) {
	return (input == '0' || input == '1');
}
static bool stringBodyCharP(int input) {
	return (input && input != '"');
}
static bool raryBodyCharP(int input) {
	return (input >= '0' && input <= '9') || (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z');
}
static NodeT mcons(NodeT hd, NodeT tl) {
	// TODO prevent variable capture of the (:)
	return call2(Scolon, hd, tl);
}
static NodeT mpair(NodeT hd, NodeT tl) {
	// TODO prevent variable capture of the (:)
	return call2(Scomma, hd, tl);
}
static bool mnilP(NodeT c) {
	return nilP(c) || c == Snil;
}
static NodeT mgetConsHead(NodeT c) {
	NodeT c2 = getCallCallable(c);
	assert(c2);
	assert(getCallCallable(c2) == Scolon);
	NodeT a0 = getCallArgument(c2);
	return a0;
}
static NodeT mgetConsTail(NodeT c) {
	NodeT c2 = getCallCallable(c);
	assert(c2);
	assert(getCallCallable(c2) == Scolon);
	NodeT a1 = getCallArgument(c);
	return a1;
}
static NodeT mquote(NodeT a) {
	//return quote(a);
	return call(Squote, a);
}
static NodeT Lang_error(struct Lang* self, const char* expectedPart, const char* gotPart) {
	return merror(expectedPart, gotPart);
}
static int Lang_operatorLevel(struct Lang* self, NodeT node) {
	int result;
	if(!getSymbol1Name(node))
		return NO_OPERATOR;
	NodeT n = dcall(self->operatorLevel, node);
	if(!intFromNode(n, &result)) {
		S_print(n);
		fflush(stdout);
		printf("operatorLevel ");
		S_print(self->operatorLevel);
		fflush(stdout);
		abort();
		return NO_OPERATOR;
	}
	return result;
}
static int Lang_operatorArgcount(struct Lang* self, NodeT node) {
	int result;
	if(!getSymbol1Name(node))
		return NO_OPERATOR;
	NodeT n = dcall(self->operatorArgcount, node);
	if(!intFromNode(n, &result)) {
		abort();
		return NO_OPERATOR;
	}
	return result;
}
static NodeT Lang_operatorPrefixNeutral(struct Lang* self, NodeT node) {
	return dcall(self->operatorPrefixNeutral, node);
}
/* TODO make this configurable, too */
static bool Lang_openingParenP(struct Lang* self, NodeT node) {
	return (node == Sleftparen) || (node == Sleftcurly) || (node == Sleftbracket) || (node == Sindent);
}
static bool Lang_closingParenP(struct Lang* self, NodeT node) {
	return (node == Srightparen) || (node == Srightcurly) || (node == Srightbracket) || (node == Sdedent);
}
static NodeT Lang_openingParenOf(struct Lang* self, NodeT node) {
	return (node == Srightparen) ? Sleftparen : 
	       (node == Srightcurly) ? Sleftcurly : 
		   (node == Srightbracket) ? Sleftbracket : 
		   (node == Sdedent) ? Sindent : 
		   node;
}
static bool Lang_operatorP(struct Lang* self, NodeT node) {
	return Lang_operatorLevel(self, node) != NO_OPERATOR;
}
NodeT initLang(void) {
	if(Srightbracket == NULL) {
		NodeT Builtins = initBuiltins();
		SminimalOPL = symbolFromStr("minimalOPL");
		minimalOPLN = dcall(Builtins, SminimalOPL);
		if(errorP(minimalOPLN)) {
			fprintf(stderr, "error: could not load minimalOPL\n");
			abort();
		}
		SLF = symbolFromStr("<LF>");
		Sindent = symbolFromStr("<indent>");
		Sdedent = symbolFromStr("<dedent>");
		SopeningParen = symbolFromStr("(");
		Sapply = symbolFromStr(" ");
		Scolon = symbolFromStr(":");
		Slet = symbolFromStr("let");
		Sin = symbolFromStr("in");
		Sfrom = symbolFromStr("from");
		Simport = symbolFromStr("import");
		Sleftparen = symbolFromStr("(");
		Sleftcurly = symbolFromStr("{");
		Sleftbracket = symbolFromStr("[");
		Srightparen = symbolFromStr(")");
		Srightcurly = symbolFromStr("}");
		Srightbracket = symbolFromStr("]");
		SEOF = symbolFromStr("\32"); // EOF
		Serror = symbolFromStr("<error>");
		Sequal = symbolFromStr("=");
		Scolonequal = symbolFromStr(":=");
		Shashexports = symbolFromStr("#exports");
		Sexports = symbolFromStr("exports");
		Snil = symbolFromStr("nil");
		Sdot = symbolFromStr(".");
		Shasht = symbolFromStr("#t");
		Shashf = symbolFromStr("#f");
		Scomma = symbolFromStr(",");
		Sbackslash = symbolFromStr("\\");
		SoperatorLevel = symbolFromStr("operatorLevel");
		SoperatorArgcount = symbolFromStr("operatorArgcount");
		SoperatorPrefixNeutral = symbolFromStr("operatorPrefixNeutral");
		Squote = symbolFromStr("'");
		INIT_FN(DynEnv);
		/*defaultDynEnv = DynEnv;*/
		//L = box(NEW_NOGC(Lang));
		initMinimalOPL();
	}
	return DynEnv;
}
static bool Lang_macroStarterP(struct Lang* self, NodeT node) {
	return /*(node == Slet) || (node == Simport) ||*/ (node == Sbackslash) || (node == Sleftbracket) || (node == Sleftcurly);
}
static NodeT macroStandinOperator(NodeT c) {
	return getConsHead(c);
}
static NodeT macroStandinOperand(NodeT c) {
	return getConsHead(getConsTail(c));
}
static NodeT macroStandin(NodeT operator_, NodeT operand) {
	return cons(operator_, cons(operand, nil));
}
static bool macroStandinP(NodeT c) {
	return(consP(c));
}
static NodeT Lang_parse0(struct Lang* self, struct Scanner* scanner, NodeT endToken);
static NodeT Lang_parseValue(struct Lang* self, struct Scanner* scanner) {
	/* TODO quoted values */
	NodeT token = Scanner_getToken(scanner);
	if(token == Sleftparen) {
		return Lang_parse0(self, scanner, Srightparen);
	} else
		return Scanner_consume(scanner);
}
static NodeT Lang_parseListLiteral(struct Lang* self, NodeT endToken, struct Scanner* tokenizer) {
	if(Scanner_getToken(tokenizer) == endToken)
		return nil;
	else {
		NodeT hd = Lang_parseValue(self, tokenizer);
		if(errorP(hd) || hd == SEOF)
			return hd;
		NodeT tl = Lang_parseListLiteral(self, endToken, tokenizer);
		if(errorP(tl) || tl == SEOF)
			return tl;
		return mcons(hd, tl); // it is NOT allowed to construct these directly. The annotator won't see them.
	}
}
static NodeT Lang_startMacro(struct Lang* self, NodeT node, struct Scanner* tokenizer) {
	if(node == Sbackslash) {
		return macroStandin(node, Lang_parseValue(self, tokenizer));
	} else if(node == Sleftbracket) {
		return Lang_parseListLiteral(self, Srightbracket, tokenizer);
	}
	return node;
}
static bool Lang_operatorLE(struct Lang* self, NodeT a, NodeT b) {
	if(a == b) { /* speed optimization */
		return Lang_operatorArgcount(self, b) > 1;
	} else {
		return Lang_operatorLevel(self, a) < Lang_operatorLevel(self, b) || (Lang_operatorLevel(self, a) == Lang_operatorLevel(self, b) && Lang_operatorArgcount(self, b) > 1); // latter: leave right-associative operators on stack if in doubt.
	}
}
static NodeT collect(FILE* file, int* linenumber, int prefix, bool (*continueP)(int input)) {
	int c;
	FILE* sst;
	char s[2049];
	sst = fmemopen(s, 2048, "w");
	if(!sst)
		abort();
	/* FIXME open_memstream */
	fputc(prefix, sst);
	while((c = GETC) != EOF && (*continueP)(c))
		fputc(c, sst);
	fflush(sst);
	UNGETC(c);
	NodeT result = (s[0]) ? symbolFromStr(GCx_strdup(s)) : merror("<value>", "<nothing>");
	fclose(sst);
	return result;
}
static NodeT collect1(FILE* file, int* linenumber, bool (*continueP)(int input)) {
	int prefix;
	assert(!continueP('\n'));
	prefix = GETC;
	if(prefix == EOF)
		return merror("<value>", "<EOF>");
	else
		return collect(file, linenumber, prefix, continueP);
}
static NodeT collectC(FILE* file, int* linenumber, int prefix, bool (*continueP)(int input)) {
	int c;
	FILE* sst;
	char s[2049];
	sst = fmemopen(s, 2048, "w");
	if(!sst)
		abort();
	fputc(prefix, sst);
	while((c = GETC) != EOF && (*continueP)(c)) {
		if(c == '\\') {
			int c2 = GETC;
			switch(c2) {
			case '\\':
			case '"':
			case '\'':
			//case '?':
				fputc(c2, sst);
				break;
			case 'n':
				fputc('\n', sst);
				break;
			case 'r':
				fputc('\r', sst);
				break;
			case 'b':
				fputc('\b', sst);
				break;
			case 't':
				fputc('\t', sst);
				break;
			case 'f':
				fputc('\f', sst);
				break;
			case 'a':
				fputc('\a', sst);
				break;
			case 'v':
				fputc('\v', sst);
				break;
			case 'x':
				{
					GETC;
					GETC;
					abort();
				}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				{
					abort();
				}
			default:
				return merror("<escapedValue>", "<junk>");
			}
			continue;
		} else if(c == '\n')
			++(*linenumber);
		fputc(c, sst);
	}
	fflush(sst);
	UNGETC(c);
	NodeT result = (s[0]) ? symbolFromStr(GCx_strdup(s)) : merror("<value>", "<nothing>");
	fclose(sst);
	return result;
}
static NodeT collect1C(FILE* file, int* linenumber, bool (*continueP)(int input)) {
	int prefix;
	prefix = GETC;
	if(prefix == EOF)
		return merror("<value>", "<EOF>");
	else
		return collect(file, linenumber, prefix, continueP);
}
static NodeT collectUnicodeID(FILE* file, int* linenumber, int prefix, const char* prev) {
	int c;
	FILE* sst;
	char s[2049];
	sst = fmemopen(s, 2048, "w");
	if(!sst)
		abort();
	fputc(prefix, sst);
	fputs(prev, sst);
	while((c = GETC) != EOF && unicodeIDRestCharP(c)) {
		fputc(c, sst);
	}
	fflush(sst);
	UNGETC(c);
	NodeT result = s[0] ? symbolFromStr(GCx_strdup(s)) : merror("<value>", "<nothing>");
	fclose(sst);
	return result;
}
static NodeT readUnicodeOperator3(FILE* file, int* linenumber, int c) {
	char buf[5] = {0};
	assert(c == 0xE2);
	buf[0] = c;
	buf[1] = GETC;
	if(buf[1] == EOF)
		return merror("<something>", "<nothing>");
	buf[2] = GETC;
	if(buf[2] == EOF)
		return merror("<something>", "<nothing>");
	// TODO unicode decode buf
	int codepoint = buf[0]; // FIXME!!!
	if(mathUnicodeOperatorInRangeP(codepoint)) { /* standalone */
		return symbolFromStr(GCx_strdup(buf));
	} else
		return collectUnicodeID(file, linenumber, buf[0], &buf[1]);
}
static NodeT readOperator(FILE* file, int* linenumber, int c) {
	return collect(file, linenumber, c, operatorCharP);
}
static NodeT readDigits(FILE* file, int* linenumber, int c) {
	return collect(file, linenumber, c, digitRestCharP);
}
static int collectNumeric3(FILE* file, int* linenumber, int base, bool (*continueP)(int input)) {
	abort();
	return(2);
}
static NodeT collectNumeric2(FILE* file, int* linenumber, int base, bool (*continueP)(int input)) {
	FILE* sst;
	char s[2049];
	sst = fmemopen(s, 2048, "w");
	if(!sst)
		abort();
	int c, digit;
	fprintf(sst, "#%dr", base);
	while((c = GETC) != EOF && (digit = digitInBase(base, c)) != -1) {
		fputc(c, sst);
	}
	fflush(sst);
	UNGETC(c);
	NodeT result = s[0] ? symbolFromStr(GCx_strdup(s)) : merror("<value>", "<nothing>");
	fclose(sst);
	return(result);
}
static NodeT readShebang(FILE* file, int* linenumber, int c) {
	return collect(file, linenumber, c, shebangBodyCharP);
}
static NodeT readHashExports(FILE* file, int* linenumber, int c) {
	assert(c == 'e');
	char c0 = GETC;
	if(c0 == 'x') {
		char c0 = GETC;
		if(c0 == 'p') {
			char c0 = GETC;
			if(c0 == 'o') {
				char c0 = GETC;
				if(c0 == 'r') {
					char c0 = GETC;
					if(c0 == 't') {
						char c0 = GETC;
						if(c0 == 's') {
							char c0 = GETC;
							if(c0 == '[') {
								UNGETC(c0);
								return Shashexports;
							}
						}
					}
				}
			}
		}
	}
	return merror("<special-coding>", "<junk>");
}
static NodeT readKeyword(FILE* file, int* linenumber, int c) {
	return collect(file, linenumber, c, keywordBodyCharP);
}
static NodeT readSpecialCoding(FILE* file, int* linenumber, int c2) {
	int c = GETC;
	// #: for signalling an error
	switch(c) {
	case 'o':
		return collectNumeric2(file, linenumber, 8, octalBodyCharP);
	case 'x':
		return collectNumeric2(file, linenumber, 16, hexadecimalBodyCharP);
	case '*':
		return collectNumeric2(file, linenumber, 2, binaryBodyCharP);
	case 'b':
		return collectNumeric2(file, linenumber, 2, binaryBodyCharP);
	case '!':
		return readShebang(file, linenumber, c);
	case 'e':
		return readHashExports(file, linenumber, c);
	case 't':
		return Shasht;
	case 'f':
		return Shashf;
	default:
		if(digitCharP(c)) {
			int basis = collectNumeric3(file, linenumber, 10, digitCharP);
			int c2 = GETC;
			if(c2 == 'r' && basis >= 2 && basis <= 36) {
				return collectNumeric2(file, linenumber, basis, raryBodyCharP);
			} else
				return merror("<special-coding>", "<junk>");
		} else
				return merror("<special-coding>", "<junk>");
	}
}
static NodeT readString(FILE* file, int* linenumber, int c) {
	if(c == '"') { /* FIXME error handling */
		NodeT n = collect1C(file, linenumber, stringBodyCharP);
		int c2 = GETC;
		if(c2 != '"') {
			char buf[2] = {0,0};
			buf[0] = c2;
			return merror("<doublequote>", buf);
		} else if(errorP(n)) {
			return n;
		} else {
			char const* nn = getSymbol1Name(n);
			assert(nn);
			return strC(nn); // TODO maybe we should special-case those just as we did numbers (instead of creating the strings here)?
		}
	} else
		return merror("<string>", "<junk>");
}
static NodeT Lang_readToken(struct Lang* self, FILE* file, int* linenumber) {
	int c;
	c = GETC;
	if(c == EOF) 
		return SEOF;
	else if(digitCharP(c))
		return readDigits(file, linenumber, c);
	else if(specialCodingCharP(c))
		return readSpecialCoding(file, linenumber, c);
	else if(asciiIDCharP(c))
		return collectUnicodeID(file, linenumber, c, "");
	else if(unicodeMaybeOperator1CharP(c))
		return readUnicodeOperator3(file, linenumber, c);
	else if(operatorCharP(c))
		return readOperator(file, linenumber, c);
	else if(braceCharP(c)) {
		char buf[2] = {0, 0};
		buf[0] = c;
		return symbolFromStr(GCx_strdup(buf));
	} else if(c == '@')
		return readKeyword(file, linenumber, c);
	else if(c == '"')
		return readString(file, linenumber, c);
	else if(c == '\'')
		return Squote;
	else {
		char buf[2] = {0,0};
		buf[0] = c;
		return merror("<token>", GCx_strdup(buf));
	}
}
static NodeT reflectHashExports(NodeT entries) {
	return (mnilP(entries)) ? entries : mcons(mquote(mgetConsHead(entries)), reflectHashExports(mgetConsTail(entries)));
}
/* [a b c] => [('a, a) ('b, b) ('c, c)] */
static NodeT blowHashExportsUp(NodeT tl, NodeT entries) {
	if(mnilP(entries)) {
		return tl;
	} else {
		NodeT sym = mgetConsHead(entries);
		return mcons(mpair(mquote(sym), sym), blowHashExportsUp(tl, mgetConsTail(entries)));
	}
}
static NodeT mcall(NodeT a, NodeT b) {
	if(macroStandinP(a) && macroStandinOperator(a) == Sbackslash) {
		//Formatters::TExpression::print(stderr, a);
		//fprintf(stderr, "\n");
		//fflush(stderr);
		return fn(macroStandinOperand(a),b)/* CRASH HERE */;
	} else if(a == Squote) { // the usual masking doesn't work since it would get an De Bruijn index anyway and be replaced.
		return mquote(b);
	} else if(a == Simport)
		return macroStandin(a, b);
	else if(a == Shashexports) {
		// tl = ('exports, 'exports:b)
		return blowHashExportsUp(mcons(mpair(mquote(Sexports), mcons(mquote(Sexports), reflectHashExports(b))), NULL), b);
	} else
		return call(a,b);
}
static NodeT replaceIMPORT(NodeT body, NodeT source, NodeT symlist) {
	// these replace IMPORTS on a only-after-parsing level. At this point, the list is not constructed yet (otherwise the annotator wouldn't find it).
	if(symlist) {
		NodeT hd = mgetConsHead(symlist);
		NodeT accessor = call(source, mquote(hd)); /* better? */
		return closeOver(hd, accessor, replaceIMPORT(body, source, mgetConsTail(symlist)));
	} else
		return body;
}
static NodeT replaceIN(NodeT equation, NodeT body) {
	/* two possibilities: */
	/* importExpr <=> [import <source> [...]] */
	/* x = 5 <=> ((= x) 5) */
	if(!binaryOperationP(equation)) {
		NodeT fr, c2;
		if(macroStandinP(equation) && macroStandinOperator(equation) == Simport && (fr = macroStandinOperand(equation)) && (c2 = getCallCallable(fr))) {
			//consP(tl) && (tl2 = getConsTail(tl)) && consP(tl2)) {
			NodeT source = getCallArgument(fr);
			NodeT symlist = getCallArgument(c2);
			if(symlist) {
				/* for strict eval without caching, we could reuse one of the syms in symlist in order to stand for source. */
				return replaceIMPORT(body, source, symlist);
			} else { /* nothing to import: don't do anything. */
				return body;
			}
		} else
			return merror("<equation>", "<junk>");
	}
	if(getOperationOperator(equation) != Sequal && getOperationOperator(equation) != Scolonequal)
		return merror("<equation>", "<inequation>");
	NodeT formalParameter = getOperationArgument1(equation);
	NodeT value = getOperationArgument2(equation);
	return call(fn(formalParameter, body), value);
}
/* TODO make this configurable, too */
static NodeT moperation(NodeT operator_, NodeT a, NodeT b) {
	return //operator_ == Sbackslash ? fn(macroStandinOperand(a),b) :  /* CRASH HERE */
	       operator_ == Sin ? replaceIN(a ,b) :
	       operator_ == Sdot ? call(a, mquote(b)) : 
	       operation(operator_, a, b);
}
/** returns: growth of the values stack */
static int Lang_callRpnOperator(struct Lang* self, NodeT operator_, MNODET* values) {
	/* note that 2-operand macro operators leave their own result on #values */
	int argcount = Lang_operatorArgcount(self, operator_);
	if(argcount < 0)
		argcount = -argcount;
	if(operator_ == SLF || operator_ == Slet) { /* ignore for now */
		//fprintf(stderr, "LF NONE\n");
		return 0;
	}
	if(argcount == 1) {
		fprintf(stderr, "ONE ARG \"");
		print(stderr, operator_);
		fprintf(stderr, "\" ");
		if(nilP(*values)) {
			fprintf(stderr, "NOT ENOUGH 1\n");
			*values = cons(merror("<1-arguments>", "<too-little>"), *values);
			return 1;
		} else {
			NodeT a = getConsHead(*values);
			print(stderr, a);
			fprintf(stderr, "\n");
			*values = getConsTail(*values);
			if(!nilP(*values) && Lang_macroStarterP(self, operator_)) {
				operator_ = getConsHead(*values);
				*values = getConsTail(*values);
			}
			*values = cons(mcall(operator_,a), *values);
			return 0;
		}
	}
	assert(argcount == 2);
	if(nilP(*values) || nilP(getConsTail(*values))) {
		fprintf(stderr, "NOT ENOUGH for (%s)\n", getSymbol1Name(operator_));
		*values = cons(merror("<2-arguments>", "<too-little>"), *values);
		return 0;
	} else {
		fprintf(stderr, "TWO ARGS \"");
		print(stderr, operator_);
		fprintf(stderr, "\" ");
		NodeT b = getConsHead(*values);
		*values = getConsTail(*values);
		NodeT a = getConsHead(*values);
		*values = getConsTail(*values);
		print(stderr, a);
		fprintf(stderr, "!");
		print(stderr, b);
		fprintf(stderr, "\n");
		*values = cons(operator_ == Sapply ? mcall(a,b) : moperation(operator_, a, b), *values);
		return 1 - 2;
	}
}
static NodeT Lang_parse0(struct Lang* self, struct Scanner* scanner, NodeT endToken) {
	NodeT result;
	NodeT prog;
	prog = Parser_parse(scanner, box(self), endToken);
	if(prog && consP(prog)) {
		result = getConsHead(prog);
		if(!getConsTail(prog))
			return result;
		else {
			fprintf(stderr, "junk was: ");
			print(stderr, getConsTail(prog));
			fprintf(stderr, "\n");
			return merror("<nothing>", "<junk>");
		}
	} else
		return merror("<something>", "<nothing>");
}
static NodeT Lang_parse(struct Lang* self, struct Scanner* scanner) {
	return Lang_parse0(self, scanner, SEOF);
}
NodeT Lang_parse1OPL(NodeT OPL, FILE* f, const char* name) {
	NodeT result;
	struct Lang* lang;
	struct Scanner* scanner;
	scanner = alloca(Scanner_getSize());
	lang = alloca(Lang_getSize());
	Scanner_init(scanner, box(lang));
	Scanner_push(scanner, f, 1, name);
	Scanner_consume(scanner);
	if(OPL == nil) {
		const char* name = getSymbol1Name(Scanner_getToken(scanner));
		if(name && name[0] == '#') {
			; /* FIXME use */
		}
		OPL = minimalOPLN;
	}
	Lang_init(lang, OPL);
	result = Lang_parse(lang, scanner);
	if(errorP(result))
		fprintf(stderr, "Info: the following occured near line %d\n", Scanner_getLinenumber(scanner));
	return result;
}
NodeT Lang_parse1(FILE* f, const char* name) {
	return Lang_parse1OPL(nil, f, name);
}
NodeT Lang_withDefaultEnv(NodeT body) {
	return closeOver(Squote, /*SpecialForms::*/Quoter, 
	       closeOver(Shashexports, /*Combinators*/Identity, 
	       body));
}

//DEFINE_STRICT_MONADIC_FN(parse1, L_parse1((FILE*) pointerFromNode(argument), NULL)) /* FIXME more args */
//DEFINE_STRICT_FN(withDefaultEnv, L_withDefaultEnv(argument))
//DEFINE_MODULE(Lang, exports(parse1, withDefaultEnv))
/* FFI:
LangWrapper non-monadic
        NodeT parse1(FILE* f, const char* name) const; monadic
	withDefaultEnv non-monadic
*/
void Lang_init(struct Lang* self, NODET OPL) {
	initLang();
	self->OPL = OPL;
	self->operatorLevel = dcall(self->OPL, SoperatorLevel);
	self->operatorArgcount = dcall(self->OPL, SoperatorArgcount);
	self->operatorPrefixNeutral = dcall(self->OPL, SoperatorPrefixNeutral);
}
size_t Lang_getSize(void) {
	return sizeof(struct Lang);
}
struct Lang* Lang_new(NODET OPL) {
	struct Lang* result = GC_MALLOC(Lang_getSize());
	Lang_init(result, OPL);
	return result;
}
END_NAMESPACE_6D(Parsers)
#include "Parsers/ShuntingYardParser.inc"
