#include <stdlib.h>
#include <assert.h>
#include <map>
#include <stack>
#include <sstream>
#include <6D/Allocators>
#include <errno.h>
#include "Values/Values"
#include "Scanners/Lang5D"
#include "Formatters/TExpression"
#include "Scanners/ShuntingYardParser"
#include "SpecialForms/SpecialForms"
#include "ModuleSystem/ModuleSystem"
#include "Combinators/Combinators"
#include "6D/FFIs"
#undef GETC
#undef UNGETC
#define GETC fgetc(file)
#define UNGETC(c) ungetc(c, file)
namespace Scanners {
using namespace Values;
using namespace FFIs;
using namespace Allocators;
NodeT Lang5D::SLF;
NodeT Lang5D::Sindent;
NodeT Lang5D::Sdedent;
NodeT Lang5D::SopeningParen;
NodeT Lang5D::Sapply;
NodeT Lang5D::Sdash;
NodeT Lang5D::Szero;
NodeT Lang5D::Sunderscore;
NodeT Lang5D::Scircumflex;
NodeT Lang5D::Sstarstar;
NodeT Lang5D::Scross;
NodeT Lang5D::Scolon;
NodeT Lang5D::Squote;
NodeT Lang5D::Scomma;
NodeT Lang5D::Sdollar;
NodeT Lang5D::Selif;
NodeT Lang5D::Selse;
NodeT Lang5D::Ssemicolon;
NodeT Lang5D::Sbackslash;
NodeT Lang5D::Slet;
NodeT Lang5D::Sletexclam;
NodeT Lang5D::Simport;
NodeT Lang5D::Sleftparen;
NodeT Lang5D::Sleftcurly;
NodeT Lang5D::Sleftbracket;
NodeT Lang5D::Srightparen;
NodeT Lang5D::Srightcurly;
NodeT Lang5D::Srightbracket = NULL;
NodeT Lang5D::SEOF;
NodeT Lang5D::Serror;
NodeT Lang5D::Sequal;
NodeT Lang5D::Sin;
NodeT Lang5D::Sfrom;
NodeT Lang5D::Shashexports;
NodeT Lang5D::Sif;
NodeT Lang5D::defaultDynEnv;
NodeT Lang5D::Sexports;
NodeT Lang5D::Snil;
using namespace SpecialForms;
//NodeT Lang5D::Sdot;
static inline NodeT merror(const std::string& expectedPart, const std::string& gotPart) {
	// FIXME nicer
	return cons(symbolFromStr("error"), cons(strCXX(expectedPart), cons(strCXX(gotPart), nil)));
}
static inline std::string nvl(const char* a, const char* b) {
	return a ? a : b;
}
static inline NodeT getDynEnvEntry(NodeT sym) {
	const char* name = getSymbol1Name(sym);
	if(name) {
		if(isdigit(name[0])) { /* since there is an infinite number of numbers, make sure not to precreate all of them :-) */
			errno = 0;
			if(strchr(name, '.')) {
				FFIs::NativeFloat value = strtod(name, NULL);
				if(errno == 0)
					return internNative(value);
			} else {
				FFIs::NativeInt value = strtol(name, NULL, 10);
				if(errno == 0)
					return internNative(value);
			}
		}
	}
	return merror("<dynamic-variable>", nvl(getSymbol1Name(sym), "???"));
}
DEFINE_STRICT_FN(DynEnv, getDynEnvEntry(argument))

/* TODO just use a function */
static std::map<NodeT, int> levels;
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
Lang5D::Lang5D(void) {
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
		Shashexports = symbolFromStr("#exports");
		Sif = symbolFromStr("if");
		Srightbracket = symbolFromStr("]");
		Sexports = symbolFromStr("exports");
		Snil = symbolFromStr("nil");
		levels[symbolFromStr("(")] = -1,
		levels[symbolFromStr("{")] = -1, // pseudo-operators
		levels[symbolFromStr("[")] = -1,
		levels[symbolFromStr("<indent>")] = -1,
		levels[SLF] = 41,
		levels[symbolFromStr("#exports")] = 40,
		levels[symbolFromStr(".")] = 33,
		levels[symbolFromStr("_")] = 32,
		levels[symbolFromStr("^")] = 32,
		levels[symbolFromStr("!")] = 31,
		levels[symbolFromStr("**")] = 30,
		levels[symbolFromStr("*")] = 29,
		levels[symbolFromStr("⋅")] = 29,
		levels[symbolFromStr("/")] = 29,
		levels[symbolFromStr("⨯")] = 28,
		levels[symbolFromStr(":")] = 27,
		levels[symbolFromStr("'")] = 26,
		levels[symbolFromStr("if")] = 25,
		levels[symbolFromStr(" ")] = 24,
		levels[symbolFromStr("++")] = 23,
		levels[symbolFromStr("+")] = 22,
		levels[symbolFromStr("‒")] = 22,
		levels[symbolFromStr("-")] = 22,
		levels[symbolFromStr("%")] = 21,
		levels[symbolFromStr("∪")] = 17,
		levels[symbolFromStr("∩")] = 16,
		levels[symbolFromStr("∈")] = 16,
		levels[symbolFromStr("⊂")] = 16,
		levels[symbolFromStr("⊃")] = 16,
		levels[symbolFromStr("⊆")] = 16,
		levels[symbolFromStr("⊇")] = 16,
		levels[symbolFromStr("=")] = 14,
		levels[symbolFromStr("≟")] = 14,
		levels[symbolFromStr("/=")] = 14,
		levels[symbolFromStr("<")] = 13,
		levels[symbolFromStr("<=")] = 13,
		levels[symbolFromStr(">")] = 13,
		levels[symbolFromStr(">=")] = 13,
		levels[symbolFromStr("≤")] = 13,
		levels[symbolFromStr("≥")] = 13,
		levels[symbolFromStr("&&")] = 12,
		levels[symbolFromStr("∧")] = 12,
		levels[symbolFromStr("||")] = 11,
		levels[symbolFromStr("∨")] = 11,
		levels[symbolFromStr(",")] = 10,
		levels[symbolFromStr("$")] = 9,
		levels[symbolFromStr("elif")] = 8,
		levels[symbolFromStr("else")] = 8,
		levels[symbolFromStr("|")] = 6,
		levels[symbolFromStr("=>")] = 5,
		levels[symbolFromStr(";")] = 5,
		levels[symbolFromStr("?;")] = 5,
		levels[symbolFromStr("\\")] = 4,
		levels[symbolFromStr("import")] = 2,
		levels[symbolFromStr("from")] = 3,
		levels[symbolFromStr("let")] = 1,
		levels[symbolFromStr("in")] = 1,
		levels[symbolFromStr("let!")] = 0,
		levels[symbolFromStr(")")] = -1,
		levels[symbolFromStr("}")] = -1,
		levels[symbolFromStr("]")] = -1,
		levels[symbolFromStr("<dedent>")] = -1;
		defaultDynEnv = DynEnv;
	}
}
NodeT Lang5D::error(std::string expectedPart, std::string gotPart) const {
	return merror(expectedPart, gotPart);
}
bool Lang5D::errorP(NodeT node) const {
	return consP(node) && getConsHead(node) == Serror;
}
bool Lang5D::operatorP(NodeT node) const {
	return levels.find(node) != levels.end();
}
int Lang5D::operatorArgcount(NodeT node) const {
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
	       (node == Sbackslash) ? R :
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
//bool Lang5D::operatorPrefixNeutralP(NodeT node) const {
NodeT Lang5D::operatorPrefixNeutral(NodeT node) const {
	return (node == Sdash) ? Szero : error("<prefix-operator>", getSymbol1Name(node));
}
bool Lang5D::macroStarterP(NodeT node) const {
	return /*(node == Slet) || (node == Simport) ||*/ (node == Sbackslash) || (node == Sleftbracket) || (node == Sleftcurly);
}
static NodeT macroStandinOperator(NodeT c) {
	return getConsHead(c);
}
static NodeT macroStandinOperand(NodeT c) {
	return getConsHead(getConsTail(c));
}
NodeT Lang5D::macroStandin(NodeT operator_, NodeT operand) const {
	return cons(operator_, cons(operand, nil));
}
static bool macroStandinP(NodeT c) {
	return(consP(c));
}
NodeT Lang5D::parseListLiteral(NodeT endToken, Scanner<Lang5D>& tokenizer) const {
	if(tokenizer.getToken() == endToken)
		return nil;
	else {
		NodeT hd = parseValue(tokenizer);
		if(errorP(hd))
			return hd;
		NodeT tl = parseListLiteral(endToken, tokenizer);
		if(errorP(tl))
			return tl;
		return mcons(hd, tl); // it is NOT allowed to construct these directly. The annotator won't see them.
	}
}
NodeT Lang5D::startMacro(NodeT node, Scanner<Lang5D>& tokenizer) const {
	if(node == Sbackslash) {
		return macroStandin(node, tokenizer.consume());
	} else if(node == Sleftbracket) {
		return parseListLiteral(Srightbracket, tokenizer);
		// tokenizer.consume(); // right bracket will be consumed by the Shunting Yard Parser (it has '[' on its 'operators stack)
		//std::vector<NodeT ALLOCATOR_VECTOR> items = Scanners::parse(tokenizer, *this, Srightbracket);
	}
	return node;
}
bool Lang5D::openingParenP(NodeT node) const {
	return (node == Sleftparen) || (node == Sleftcurly) || (node == Sleftbracket) || (node == Sindent);
}
bool Lang5D::closingParenP(NodeT node) const {
	return (node == Srightparen) || (node == Srightcurly) || (node == Srightbracket) || (node == Sdedent);
}
NodeT Lang5D::openingParenOf(NodeT node) const {
	return (node == Srightparen) ? Sleftparen : 
	       (node == Srightcurly) ? Sleftcurly : 
		   (node == Srightbracket) ? Sleftbracket : 
		   (node == Sdedent) ? Sindent : 
		   node;
}
bool Lang5D::operatorLE(NodeT a, NodeT b) const {
	if(a == b) { /* speed optimization */
		return operatorArgcount(b) > 0;
	} else {
		return levels[a] < levels[b] || (levels[a] == levels[b] && operatorArgcount(b) > 0); // latter: leave right-associative operators on stack if in doubt.
	}
}
NodeT Lang5D::collect(FILE* file, int& linenumber, int prefix, bool (*continueP)(int input)) const {
	int c;
	std::stringstream sst;
	sst << (char) prefix;
	while((c = GETC) != EOF && (*continueP)(c)) {
		sst << (char) c;
	}
	std::string s = sst.str();
	UNGETC(c);
	if(s.length() > 0) {
		return symbolFromStr(GCx_strdup(s.c_str()));
	} else
		return error("<value>", "<nothing>");
}
NodeT Lang5D::collect1(FILE* file, int& linenumber, bool (*continueP)(int input)) const {
	int prefix;
	assert(!continueP('\n'));
	prefix = GETC;
	if(prefix == EOF)
		return error("<value>", "<EOF>");
	else
		return collect(file, linenumber, prefix, continueP);
}
NodeT Lang5D::collectC(FILE* file, int& linenumber, int prefix, bool (*continueP)(int input)) const {
	int c;
	std::stringstream sst;
	sst << (char) prefix;
	while((c = GETC) != EOF && (*continueP)(c)) {
		if(c == '\\') {
			int c2 = GETC;
			switch(c2) {
			case '\\':
			case '"':
			case '\'':
			//case '?':
				sst << (char) c2;
				break;
			case 'n':
				sst << (char) '\n';
				break;
			case 'r':
				sst << (char) '\r';
				break;
			case 'b':
				sst << (char) '\b';
				break;
			case 't':
				sst << (char) '\t';
				break;
			case 'f':
				sst << (char) '\f';
				break;
			case 'a':
				sst << (char) '\a';
				break;
			case 'v':
				sst << (char) '\v';
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
			++linenumber;
		sst << (char) c;
	}
	std::string s = sst.str();
	UNGETC(c);
	if(s.length() > 0) {
		return symbolFromStr(GCx_strdup(s.c_str()));
	} else
		return error("<value>", "<nothing>");
}
NodeT Lang5D::collect1C(FILE* file, int& linenumber, bool (*continueP)(int input)) const {
	int prefix;
	prefix = GETC;
	if(prefix == EOF)
		return error("<value>", "<EOF>");
	else
		return collect(file, linenumber, prefix, continueP);
}
NodeT Lang5D::collectUnicodeID(FILE* file, int& linenumber, int prefix, const std::string& prev) const {
	int c;
	std::stringstream sst;
	sst << (char) prefix << prev;
	while((c = GETC) != EOF && unicodeIDRestCharP(c)) {
		sst << (char) c;
	}
	std::string s = sst.str();
	UNGETC(c);
	if(s.length() > 0)
		return symbolFromStr(GCx_strdup(s.c_str()));
	else
		return error("<value>", "<nothing>");
}
NodeT Lang5D::readUnicodeOperator3(FILE* file, int& linenumber, int c) const {
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
NodeT Lang5D::readOperator(FILE* file, int& linenumber, int c) const {
	return collect(file, linenumber, c, operatorCharP);
}
NodeT Lang5D::readDigits(FILE* file, int& linenumber, int c) const {
	return collect(file, linenumber, c, digitRestCharP);
}
int Lang5D::collectNumeric3(FILE* file, int& linenumber, int base, bool (*continueP)(int input)) const {
	abort();
	return(2);
}
NodeT Lang5D::collectNumeric2(FILE* file, int& linenumber, int base, bool (*continueP)(int input)) const {
	abort(); // FIXME
	return(nil);
}
NodeT Lang5D::readShebang(FILE* file, int& linenumber, int c) const {
	return collect(file, linenumber, c, shebangBodyCharP);
}
NodeT Lang5D::readHashExports(FILE* file, int& linenumber, int c) const {
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
NodeT Lang5D::readKeyword(FILE* file, int& linenumber, int c) const {
	return collect(file, linenumber, c, keywordBodyCharP);
}
NodeT Lang5D::readSpecialCoding(FILE* file, int& linenumber, int c2) const {
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
NodeT Lang5D::readString(FILE* file, int& linenumber, int c) const {
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
			return strCXX(nn);
		}
	} else
		return error("<string>", "<junk>");
}
NodeT Lang5D::readToken(FILE* file, int& linenumber) const {
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
		return error("<token>", std::string(buf));
	}
}
NodeT Lang5D::reflectHashExports(NodeT entries) const {
	return (mnilP(entries)) ? entries : mcons(mquote(mgetConsHead(entries)), reflectHashExports(mgetConsTail(entries)));
}
/* [a b c] => [('a, a) ('b, b) ('c, c)] */
NodeT Lang5D::blowHashExportsUp(NodeT tl, NodeT entries) const {
	if(mnilP(entries)) {
		return tl;
	} else {
		NodeT sym = mgetConsHead(entries);
		return mcons(mpair(mquote(sym), sym), blowHashExportsUp(tl, mgetConsTail(entries)));
	}
}
NodeT Lang5D::mcall(NodeT a, NodeT b) const {
	if(a == Squote) { // the usual masking doesn't work since it would get an De Bruijn index anyway and be replaced.
		return mquote(b);
	} else if(a == Simport)
		return macroStandin(a, b);
	else if(a == Shashexports) {
		// tl = ('exports, 'exports:b)
		return blowHashExportsUp(mcons(mpair(mquote(Sexports), mcons(mquote(Sexports), reflectHashExports(b))), NULL), b);
	} else
		return call(a,b);
}
NodeT Lang5D::replaceIMPORT(NodeT body, NodeT source, NodeT symlist) const {
	// these replace IMPORTS on a only-after-parsing level. At this point, the list is not constructed yet (otherwise the annotator wouldn't find it).
	if(symlist) {
		//NodeT close(NodeT /* symbol */ parameter, NodeT argument, NodeT body) {
		//NodeT accessor = operation(Sdot, source, quote(getConsHead(symlist))); /* technically this is bad since it captures the (.) that is in scope. */
		NodeT hd = mgetConsHead(symlist);
		NodeT accessor = call(source, mquote(hd)); /* better? */
		return Values::close(hd, accessor, replaceIMPORT(body, source, mgetConsTail(symlist)));
	} else
		return body;
}
Values::NodeT Lang5D::mcons(Values::NodeT hd, Values::NodeT tl) const {
	// TODO prevent variable capture of the (:)
	return call2(Scolon, hd, tl);
}
Values::NodeT Lang5D::mpair(Values::NodeT hd, Values::NodeT tl) const {
	// TODO prevent variable capture of the (:)
	return call2(Scomma, hd, tl);
}
bool Lang5D::mnilP(Values::NodeT c) const {
	return nilP(c) || c == Snil;
}
Values::NodeT Lang5D::mgetConsHead(Values::NodeT c) const {
	NodeT c2 = getCallCallable(c);
	assert(c2);
	assert(getCallCallable(c2) == Scolon);
	NodeT a0 = getCallArgument(c2);
	return a0;
}
Values::NodeT Lang5D::mgetConsTail(Values::NodeT c) const {
	NodeT c2 = getCallCallable(c);
	assert(c2);
	assert(getCallCallable(c2) == Scolon);
	NodeT a1 = getCallArgument(c);
	return a1;
}
Values::NodeT Lang5D::mquote(Values::NodeT a) const {
	return quote(a);
	// NOT return call(Squote, a);
}
NodeT Lang5D::replaceIN(NodeT equation, NodeT body) const {
	/* two possibilities: */
	/* importExpr <=> [import <source> [...]] */
	/* x = 5 <=> ((= x) 5) */
	if(!operationP(equation)) {
		NodeT fr, c2;
		if(macroStandinP(equation) && macroStandinOperator(equation) == Simport && (fr = macroStandinOperand(equation)) && (c2 = getCallCallable(fr))) {
			//consP(tl) && (tl2 = getConsTail(tl)) && consP(tl2)) {
			return replaceIMPORT(body, getCallArgument(fr), getCallArgument(c2));
		} else
			return error("<equation>", "<junk>");
	}
	if(getOperationOperator(equation) != Sequal)
		return error("<equation>", "<inequation>");
	NodeT formalParameter = getOperationArgument1(equation);
	NodeT value = getOperationArgument2(equation);
	return call(fn(formalParameter, body), value);
}
NodeT Lang5D::moperation(NodeT operator_, NodeT a, NodeT b) const {
	return operator_ == Sbackslash ? fn(macroStandinOperand(a),b) :  /* CRASH HERE */
	       operator_ == Sin ? replaceIN(a ,b) :
		   operation(operator_, a, b);
}
/** returns: growth of the values stack */
int Lang5D::callRpnOperator(NodeT operator_, std::vector<NodeT ALLOCATOR_VECTOR>& values) const {
	/* note that 2-operand macro operators leave their own result on #values */
	int argcount = operatorArgcount(operator_);
	if(argcount < 0)
		argcount = -argcount;
	if(operator_ == SLF || operator_ == Slet) { /* ignore for now */
		//fprintf(stderr, "LF NONE\n");
		return 0;
	}
	if(argcount == 1) {
		//fprintf(stderr, "ONE ARG \"");
		//Formatters::TExpression::print(stderr, operator_);
		//fprintf(stderr, "\" ");
		if(values.size() < 1) {
			fprintf(stderr, "NOT ENOUGH 1\n");
			values.push_back(error("<1-arguments>", "<too-little>"));
			return 1;
		} else {
			NodeT a = values.back();
			//Formatters::TExpression::print(stderr, a);
			//fprintf(stderr, "\n");
			values.pop_back();
			values.push_back(mcall(operator_,a));
			return 0;
		}
	}
	assert(argcount == 2);
	if(values.size() < 2) {
		//fprintf(stderr, "NOT ENOUGH\n");
		values.push_back(error("<2-arguments>", "<too-little>"));
		return 0;
	} else {
		//fprintf(stderr, "TWO ARGS \"");
		//Formatters::TExpression::print(stderr, operator_);
		//fprintf(stderr, "\" ");
		NodeT b = values.back();
		values.pop_back();
		NodeT a = values.back();
		values.pop_back();
		//Formatters::TExpression::print(stderr, a);
		//fprintf(stderr, "!");
		//Formatters::TExpression::print(stderr, b);
		//fprintf(stderr, "\n");
		values.push_back(operator_ == Sapply ? mcall(a,b) : moperation(operator_, a, b));
		return 1 - 2;
	}
}
NodeT Lang5D::parseValue(Scanner<Lang5D>& scanner) const {
	NodeT token = scanner.getToken();
	if(token == Sleftparen) {
		return parse0(scanner, Srightparen);
	} else
		return scanner.consume();
}
NodeT Lang5D::parse0(Scanner<Lang5D>& scanner, NodeT endToken) const {
	NodeT result;
	std::vector<NodeT ALLOCATOR_VECTOR> prog;
	prog = Scanners::parse(scanner, *this, endToken);
	std::vector<NodeT ALLOCATOR_VECTOR>::const_iterator iter = prog.begin(); 
	if(iter != prog.end()) {
		result = *iter;
		++iter;
		if(iter == prog.end())
			return result;
		else {
			fprintf(stderr, "junk was: ");
			Formatters::TExpression::print(stderr, *iter);
			fprintf(stderr, "\n");
			return error("<nothing>", "<junk>");
		}
	} else
		return error("<something>", "<nothing>");
}
NodeT Lang5D::parse(Scanner<Lang5D>& scanner) const {
	return parse0(scanner, SEOF);
}
NodeT Lang5D::parse1(FILE* f, const char* name) const {
	NodeT result;
	Scanner<Lang5D> scanner(*this);
	scanner.push(f, 1, name);
	scanner.consume();
	result = parse(scanner);
	if(errorP(result))
		fprintf(stderr, "Info: the following occured near line %d\n", scanner.getLinenumber());
	return result;
}
NodeT Lang5D::error(NodeT expectedPart, NodeT gotPart) const {
	/* FIXME */
	return error("???", "???");
}
NodeT Lang5D::withDefaultEnv(NodeT body) const {
	using namespace Values;
        return close(Squote, SpecialForms::Quoter, 
	       close(Shashexports, Combinators::Identity, 
	       body));
}

}
