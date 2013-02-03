#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "6D/Allocators"
#include "Values/Values"
#include "Parsers/Lang5D"
#include "Formatters/TExpression"
#include "Parsers/ShuntingYardParser"
#include "SpecialForms/SpecialForms"
#include "Modulesystem/Modulesystem"
#include "Combinators/Combinators"
#include "Numbers/Integer2"
#include "6D/FFIs"
#include "6D/Modulesystem"
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
NodeT SLF;
NodeT Sindent;
NodeT Sdedent;
NodeT SopeningParen;
NodeT Sapply;
NodeT Sdash;
NodeT Szero;
NodeT Sunderscore;
NodeT Scircumflex;
NodeT Sstarstar;
NodeT Scross;
NodeT Scolon;
NodeT Squote;
NodeT Scomma;
NodeT Sdollar;
NodeT Selif;
NodeT Selse;
NodeT Ssemicolon;
NodeT Sbackslash;
NodeT Slet;
NodeT Sletexclam;
NodeT Simport;
NodeT Sleftparen;
NodeT Sleftcurly;
NodeT Sleftbracket;
NodeT Srightparen;
NodeT Srightcurly;
NodeT Srightbracket = NULL;
NodeT SEOF;
NodeT Serror;
NodeT Sequal;
NodeT Sin;
NodeT Sfrom;
NodeT Shashexports;
NodeT Sif;
NodeT defaultDynEnv;
NodeT Sexports;
NodeT Snil;
NodeT Scolonequal;
NodeT Sdot;
NodeT Shasht;
NodeT Shashf;
//NodeT Sdot;
static inline NodeT merror(const char* expectedPart, const char* gotPart) {
	return parseError(strC(expectedPart), strC(gotPart));
}
static inline const char* nvl(const char* a, const char* b) {
	return a ? a : b;
}
static inline NodeT getDynEnvEntry(NodeT sym) {
	const char* name = getSymbol1Name(sym);
	if(name) {
		if(isdigit(name[0])) { /* since there is an infinite number of numbers, make sure not to precreate all of them :-) */
			errno = 0;
			if(strchr(name, '.')) {
				NativeFloat value = strtod(name, NULL);
				if(errno == 0)
					return internNativeInt(value);
			} else {
				NativeInt value = strtol(name, NULL, 10);
				if(errno == 0)
					return internNativeInt(value);
				else {
					NodeT base = internNativeInt((NativeInt) 10);
					NodeT result = internNativeInt((NativeInt) 0);
					for(;isdigit(*name);++name) {
						char c = *name;
						int off = c - '0';
						result = integerMul(result, base);
						result = integerAddU(result, off);
					}
					return result;
				}
			}
		}
	}
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
	return quote(a);
	// NOT return call(Squote, a);
}
static NodeT error(const char* expectedPart, const char* gotPart) {
	return merror(expectedPart, gotPart);
}
#define NO_OPERATOR (-2)
static int level(NodeT n) {
	return n == symbolFromStr("(") ? -1 :
	       n == symbolFromStr("{") ? -1 :  // pseudo-operators
	       n == symbolFromStr("[") ? -1 : 
	       n == symbolFromStr("<indent>") ? -1 : 
	       n == symbolFromStr("<LF>") ? 41 : 
	       n == symbolFromStr("#exports") ? 40 : 
	       n == symbolFromStr(".") ? 33 : 
	       n == symbolFromStr("_") ? 32 : 
	       n == symbolFromStr("^") ? 32 : 
	       n == symbolFromStr("!") ? 31 : 
	       n == symbolFromStr("**") ? 30 : 
	       n == symbolFromStr("*") ? 29 : 
	       n == symbolFromStr("⋅") ? 29 : 
	       n == symbolFromStr("/") ? 29 : 
	       n == symbolFromStr("⨯") ? 28 : 
	       n == symbolFromStr(":") ? 27 : 
	       n == symbolFromStr("'") ? 26 : 
	       n == symbolFromStr("if") ? 25 : 
	       n == symbolFromStr(" ") ? 24 : 
	       n == symbolFromStr("++") ? 23 : 
	       n == symbolFromStr("+") ? 22 : 
	       n == symbolFromStr("‒") ? 22 : 
	       n == symbolFromStr("-") ? 22 : 
	       n == symbolFromStr("%") ? 21 : 
	       n == symbolFromStr("∪") ? 17 : 
	       n == symbolFromStr("∩") ? 16 : 
	       n == symbolFromStr("∈") ? 16 : 
	       n == symbolFromStr("⊂") ? 16 : 
	       n == symbolFromStr("⊃") ? 16 : 
	       n == symbolFromStr("⊆") ? 16 : 
	       n == symbolFromStr("⊇") ? 16 : 
	       n == symbolFromStr("=") ? 14 : 
	       n == symbolFromStr("≟") ? 14 : 
	       n == symbolFromStr("/=") ? 14 : 
	       n == symbolFromStr("<") ? 13 : 
	       n == symbolFromStr("<=") ? 13 : 
	       n == symbolFromStr(">") ? 13 : 
	       n == symbolFromStr(">=") ? 13 : 
	       n == symbolFromStr("≤") ? 13 : 
	       n == symbolFromStr("≥") ? 13 : 
	       n == symbolFromStr("&&") ? 12 : 
	       n == symbolFromStr("∧") ? 12 : 
	       n == symbolFromStr("||") ? 11 : 
	       n == symbolFromStr("∨") ? 11 : 
	       n == symbolFromStr(",") ? 10 : 
	       n == symbolFromStr("$") ? 9 : 
	       n == symbolFromStr("elif") ? 8 : 
	       n == symbolFromStr("else") ? 8 : 
	       n == symbolFromStr("|") ? 6 : 
	       n == symbolFromStr("=>") ? 5 : 
	       n == symbolFromStr(";") ? 5 : 
	       n == symbolFromStr("?;") ? 5 : 
	       n == symbolFromStr("\\") ? 4 : 
	       n == symbolFromStr(":=") ? 3 : 
	       n == symbolFromStr("import") ? 2 : 
	       n == symbolFromStr("from") ? 3 : 
	       n == symbolFromStr("let") ? 1 : 
	       n == symbolFromStr("in") ? 1 : 
	       n == symbolFromStr("let!") ? 0 : 
	       n == symbolFromStr(")") ? -1 : 
	       n == symbolFromStr("}") ? -1 : 
	       n == symbolFromStr("]") ? -1 : 
	       n == symbolFromStr("<dedent>") ? -1 :
	       NO_OPERATOR;
}
static bool operatorP(NodeT node) {
	return level(node) != NO_OPERATOR;
}
void initLang5D(void) {
	if(Srightbracket == NULL) {
		SLF = symbolFromStr("<LF>");
		Sindent = symbolFromStr("<indent>");
		Sdedent = symbolFromStr("<dedent>");
		SopeningParen = symbolFromStr("(");
		Sapply = symbolFromStr(" ");
		Sdash = symbolFromStr("-");
		Szero = symbolFromStr("0");
		Sunderscore = symbolFromStr("_");
		Scircumflex = symbolFromStr("^");
		Sstarstar = symbolFromStr("**");
		Scross = symbolFromStr("⨯");
		Scolon = symbolFromStr(":");
		Squote = symbolFromStr("'");
		Scomma = symbolFromStr(",");
		Sdollar = symbolFromStr("$");
		Selif = symbolFromStr("elif");
		Selse = symbolFromStr("else");
		Ssemicolon = symbolFromStr(";");
		Sbackslash = symbolFromStr("\\");
		Slet = symbolFromStr("let");
		Sin = symbolFromStr("in");
		Sfrom = symbolFromStr("from");
		Sletexclam = symbolFromStr("let!");
		Simport = symbolFromStr("import");
		Sleftparen = symbolFromStr("(");
		Sleftcurly = symbolFromStr("{");
		Sleftbracket = symbolFromStr("[");
		Srightparen = symbolFromStr(")");
		Srightcurly = symbolFromStr("}");
		SEOF = symbolFromStr("\32"); // EOF
		Serror = symbolFromStr("<error>");
		Sequal = symbolFromStr("=");
		Scolonequal = symbolFromStr(":=");
		Shashexports = symbolFromStr("#exports");
		Sif = symbolFromStr("if");
		Srightbracket = symbolFromStr("]");
		Sexports = symbolFromStr("exports");
		Snil = symbolFromStr("nil");
		Sdot = symbolFromStr(".");
		Shasht = symbolFromStr("#t");
		Shashf = symbolFromStr("#f");
		assert(operatorP(symbolFromStr("+")));
		assert(operatorP(symbolFromStr("(")));
		defaultDynEnv = DynEnv;
	}
}
static bool macroStarterP(NodeT node) {
	return /*(node == Slet) || (node == Simport) ||*/ (node == Sbackslash) || (node == Sleftbracket) || (node == Sleftcurly);
}
static int operatorArgcount(NodeT node) {
	int R = -2;
	//int N = -2; // TODO?
	int P = 1;
	int S = -1;
	return (node == Sunderscore) ? R :
	       (node == Scircumflex) ? R : 
	       (node == Sstarstar) ? R : 
	       (node == Scross) ? R : 
	       (node == Scolon) ? R : 
	       (node == Squote) ? P : 
	       (node == Scomma) ? R : 
	       (node == Sdollar) ? R : 
	       (node == Selif) ? R : 
	       (node == Selse) ? R : 
	       (node == Ssemicolon) ? R : 
	       (node == Sbackslash) ? P :
	       (node == Slet) ? P :
	       (node == Sif) ? P :
	       (node == Sin) ? R :
	       (node == Sfrom) ? R :
	       (node == Sletexclam) ? R :
	       (node == Simport) ? P :
	       macroStarterP(node) ? P :
	       (node == SLF) ? S : 
	       (node == Shashexports) ? P : 
	       2;
}
//bool operatorPrefixNeutralP(NodeT node) {
static NodeT operatorPrefixNeutral(NodeT node) {
	return (node == Sdash) ? Szero : error("<prefix-operator>", getSymbol1Name(node));
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
static NodeT parse0(struct Scanner* scanner, NodeT endToken);
static NodeT parseValue(struct Scanner* scanner) {
	NodeT token = Scanner_getToken(scanner);
	if(token == Sleftparen) {
		return parse0(scanner, Srightparen);
	} else
		return Scanner_consume(scanner);
}
static NodeT parseListLiteral(NodeT endToken, struct Scanner* tokenizer) {
	if(Scanner_getToken(tokenizer) == endToken)
		return nil;
	else {
		NodeT hd = parseValue(tokenizer);
		if(errorP(hd) || hd == SEOF)
			return hd;
		NodeT tl = parseListLiteral(endToken, tokenizer);
		if(errorP(tl) || tl == SEOF)
			return tl;
		return mcons(hd, tl); // it is NOT allowed to construct these directly. The annotator won't see them.
	}
}
static NodeT startMacro(NodeT node, struct Scanner* tokenizer) {
	if(node == Sbackslash) {
		return macroStandin(node, parseValue(tokenizer));
	} else if(node == Sleftbracket) {
		return parseListLiteral(Srightbracket, tokenizer);
	}
	return node;
}
static bool openingParenP(NodeT node) {
	return (node == Sleftparen) || (node == Sleftcurly) || (node == Sleftbracket) || (node == Sindent);
}
static bool closingParenP(NodeT node) {
	return (node == Srightparen) || (node == Srightcurly) || (node == Srightbracket) || (node == Sdedent);
}
static NodeT openingParenOf(NodeT node) {
	return (node == Srightparen) ? Sleftparen : 
	       (node == Srightcurly) ? Sleftcurly : 
		   (node == Srightbracket) ? Sleftbracket : 
		   (node == Sdedent) ? Sindent : 
		   node;
}
static bool operatorLE(NodeT a, NodeT b) {
	if(a == b) { /* speed optimization */
		return operatorArgcount(b) > 1;
	} else {
		return level(a) < level(b) || (level(a) == level(b) && operatorArgcount(b) > 1); // latter: leave right-associative operators on stack if in doubt.
	}
}
static NodeT collect(FILE* file, int* linenumber, int prefix, bool (*continueP)(int input)) {
	int c;
	FILE* sst;
	char s[2049];
	sst = fmemopen(s, 2048, "w");
	/* FIXME open_memstream */
	fputc(prefix, sst);
	while((c = GETC) != EOF && (*continueP)(c))
		fputc(c, sst);
	fflush(sst);
	UNGETC(c);
	NodeT result = (s[0]) ? symbolFromStr(GCx_strdup(s)) : error("<value>", "<nothing>");
	fclose(sst);
	return result;
}
static NodeT collect1(FILE* file, int* linenumber, bool (*continueP)(int input)) {
	int prefix;
	assert(!continueP('\n'));
	prefix = GETC;
	if(prefix == EOF)
		return error("<value>", "<EOF>");
	else
		return collect(file, linenumber, prefix, continueP);
}
static NodeT collectC(FILE* file, int* linenumber, int prefix, bool (*continueP)(int input)) {
	int c;
	FILE* sst;
	char s[2049];
	sst = fmemopen(s, 2048, "w");
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
				return error("<escapedValue>", "<junk>");
			}
			continue;
		} else if(c == '\n')
			++(*linenumber);
		fputc(c, sst);
	}
	fflush(sst);
	UNGETC(c);
	NodeT result = (s[0]) ? symbolFromStr(GCx_strdup(s)) : error("<value>", "<nothing>");
	fclose(sst);
	return result;
}
static NodeT collect1C(FILE* file, int* linenumber, bool (*continueP)(int input)) {
	int prefix;
	prefix = GETC;
	if(prefix == EOF)
		return error("<value>", "<EOF>");
	else
		return collect(file, linenumber, prefix, continueP);
}
static NodeT collectUnicodeID(FILE* file, int* linenumber, int prefix, const char* prev) {
	int c;
	FILE* sst;
	char s[2049];
	sst = fmemopen(s, 2048, "w");
	fputc(prefix, sst);
	fputs(prev, sst);
	while((c = GETC) != EOF && unicodeIDRestCharP(c)) {
		fputc(c, sst);
	}
	fflush(sst);
	UNGETC(c);
	NodeT result = s[0] ? symbolFromStr(GCx_strdup(s)) : error("<value>", "<nothing>");
	fclose(sst);
	return result;
}
static NodeT readUnicodeOperator3(FILE* file, int* linenumber, int c) {
	char buf[5] = {0};
	assert(c == 0xE2);
	buf[0] = c;
	buf[1] = GETC;
	if(buf[1] == EOF)
		return error("<something>", "<nothing>");
	buf[2] = GETC;
	if(buf[2] == EOF)
		return error("<something>", "<nothing>");
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
	abort(); // FIXME
	return(nil);
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
	return error("<special-coding>", "<junk>");
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
				return error("<special-coding>", "<junk>");
		} else
				return error("<special-coding>", "<junk>");
	}
}
static NodeT readString(FILE* file, int* linenumber, int c) {
	if(c == '"') { /* FIXME error handling */
		NodeT n = collect1C(file, linenumber, stringBodyCharP);
		int c2 = GETC;
		if(c2 != '"') {
			char buf[2] = {0,0};
			buf[0] = c2;
			return error("<doublequote>", buf);
		} else if(errorP(n)) {
			return n;
		} else {
			char const* nn = getSymbol1Name(n);
			assert(nn);
			return strC(nn); // TODO maybe we should special-case those just as we did numbers (instead of creating the strings here)?
		}
	} else
		return error("<string>", "<junk>");
}
static NodeT readToken(FILE* file, int* linenumber) {
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
		return error("<token>", GCx_strdup(buf));
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
		return close(hd, accessor, replaceIMPORT(body, source, mgetConsTail(symlist)));
	} else
		return body;
}
static NodeT replaceIN(NodeT equation, NodeT body) {
	/* two possibilities: */
	/* importExpr <=> [import <source> [...]] */
	/* x = 5 <=> ((= x) 5) */
	if(!operationP(equation)) {
		NodeT fr, c2;
		if(macroStandinP(equation) && macroStandinOperator(equation) == Simport && (fr = macroStandinOperand(equation)) && (c2 = getCallCallable(fr))) {
			//consP(tl) && (tl2 = getConsTail(tl)) && consP(tl2)) {
			NodeT source = getCallArgument(fr);
			return replaceIMPORT(body, source, getCallArgument(c2));
		} else
			return error("<equation>", "<junk>");
	}
	if(getOperationOperator(equation) != Sequal && getOperationOperator(equation) != Scolonequal)
		return error("<equation>", "<inequation>");
	NodeT formalParameter = getOperationArgument1(equation);
	NodeT value = getOperationArgument2(equation);
	return call(fn(formalParameter, body), value);
}
static NodeT moperation(NodeT operator_, NodeT a, NodeT b) {
	return //operator_ == Sbackslash ? fn(macroStandinOperand(a),b) :  /* CRASH HERE */
	       operator_ == Sin ? replaceIN(a ,b) :
	       operator_ == Sdot ? call(a, quote(b)) : 
	       operation(operator_, a, b);
}
/** returns: growth of the values stack */
static int callRpnOperator(NodeT operator_, MNODET* values) {
	/* note that 2-operand macro operators leave their own result on #values */
	int argcount = operatorArgcount(operator_);
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
			*values = cons(error("<1-arguments>", "<too-little>"), *values);
			return 1;
		} else {
			NodeT a = getConsHead(*values);
			print(stderr, a);
			fprintf(stderr, "\n");
			*values = getConsTail(*values);
			if(macroStarterP(operator_)) {
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
		*values = cons(error("<2-arguments>", "<too-little>"), *values);
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
static NodeT L; /* FIXME dispatcher for all this stuff here */
static NodeT parse0(struct Scanner* scanner, NodeT endToken) {
	NodeT result;
	NodeT prog;
	prog = Parser_parse(scanner, L, endToken);
	if(prog && consP(prog)) {
		result = getConsHead(prog);
		if(!getConsTail(prog))
			return result;
		else {
			fprintf(stderr, "junk was: ");
			print(stderr, getConsTail(prog));
			fprintf(stderr, "\n");
			return error("<nothing>", "<junk>");
		}
	} else
		return error("<something>", "<nothing>");
}
static NodeT parse(struct Scanner* scanner) {
	return parse0(scanner, SEOF);
}
NodeT L_parse1(FILE* f, const char* name) {
	NodeT result;
	struct Scanner* scanner;
	scanner = alloca(Scanner_getSize());
	Scanner_init(scanner, L);
	Scanner_push(scanner, f, 1, name);
	Scanner_consume(scanner);
	result = parse(scanner);
	if(errorP(result))
		fprintf(stderr, "Info: the following occured near line %d\n", Scanner_getLinenumber(scanner));
	return result;
}
static NodeT L_withDefaultEnv(NodeT body) {
        return //close(Squote, SpecialForms::Quoter, 
	       close(Shashexports, /*Combinators*/Identity, 
	       body);
}

//DEFINE_STRICT_MONADIC_FN(parse1, L_parse1((FILE*) pointerFromNode(argument), NULL)) /* FIXME more args */
//DEFINE_STRICT_FN(withDefaultEnv, L_withDefaultEnv(argument))
//DEFINE_MODULE(Lang5D, exports(parse1, withDefaultEnv))
/* FFI:
Lang5DWrapper non-monadic
        NodeT parse1(FILE* f, const char* name) const; monadic
	withDefaultEnv non-monadic
*/

END_NAMESPACE_6D(Parsers)
#include "Parsers/ShuntingYardParser.inc"
