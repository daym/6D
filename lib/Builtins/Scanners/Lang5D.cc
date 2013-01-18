#include <stdlib.h>
#include <assert.h>
#include <map>
#include <stack>
#include <sstream>
#include "Values/Values"
#include "Scanners/Lang5D"
#include "Scanners/ShuntingYardParser"
#include "Formatters/TExpression"
#undef GETC
#undef UNGETC
#define GETC fgetc(file)
#define UNGETC(c) ungetc(c, file)
namespace Scanners {
using namespace Values;

Values::NodeT Lang5D::SLF;
Values::NodeT Lang5D::Sindent;
Values::NodeT Lang5D::Sdedent;
Values::NodeT Lang5D::SopeningParen;
Values::NodeT Lang5D::Sapply;
Values::NodeT Lang5D::Sdash;
Values::NodeT Lang5D::Szero;
Values::NodeT Lang5D::Sunderscore;
Values::NodeT Lang5D::Scircumflex;
Values::NodeT Lang5D::Sstarstar;
Values::NodeT Lang5D::Scross;
Values::NodeT Lang5D::Scolon;
Values::NodeT Lang5D::Squote;
Values::NodeT Lang5D::Scomma;
Values::NodeT Lang5D::Sdollar;
Values::NodeT Lang5D::Selif;
Values::NodeT Lang5D::Selse;
Values::NodeT Lang5D::Ssemicolon;
Values::NodeT Lang5D::Sbackslash;
Values::NodeT Lang5D::Slet;
Values::NodeT Lang5D::Sletexclam;
Values::NodeT Lang5D::Simport;
Values::NodeT Lang5D::Sleftparen;
Values::NodeT Lang5D::Sleftcurly;
Values::NodeT Lang5D::Sleftbracket;
Values::NodeT Lang5D::Srightparen;
Values::NodeT Lang5D::Srightcurly;
Values::NodeT Lang5D::Srightbracket = NULL;
Values::NodeT Lang5D::SEOF;
Values::NodeT Lang5D::Serror;
Values::NodeT Lang5D::Sequal;
Values::NodeT Lang5D::Sin;
Values::NodeT Lang5D::Sfrom;
Values::NodeT Lang5D::Shashexports;

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
	if(Srightbracket == NULL) {
		Srightbracket = symbolFromStr("]");
		levels[symbolFromStr("(")] = -1,
		levels[symbolFromStr("{")] = -1, // pseudo-operators
		levels[symbolFromStr("[")] = -1,
		levels[symbolFromStr("<indent>")] = -1,
		levels[SLF] = 40,
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
		levels[symbolFromStr(" ")] = 24,
		levels[symbolFromStr("++")] = 23,
		levels[symbolFromStr("+")] = 22,
		levels[symbolFromStr("‒")] = 22,
		levels[symbolFromStr("-")] = 22,
		levels[symbolFromStr("%")] = 21,
		levels[symbolFromStr("∩")] = 16,
		levels[symbolFromStr("∪")] = 15,
		levels[symbolFromStr("∈")] = 14,
		levels[symbolFromStr("⊂")] = 14,
		levels[symbolFromStr("⊃")] = 14,
		levels[symbolFromStr("⊆")] = 14,
		levels[symbolFromStr("⊇")] = 14,
		levels[symbolFromStr("=")] = 11,
		levels[symbolFromStr("≟")] = 11,
		levels[symbolFromStr("/=")] = 11,
		levels[symbolFromStr("<")] = 10,
		levels[symbolFromStr("<=")] = 10,
		levels[symbolFromStr(">")] = 10,
		levels[symbolFromStr(">=")] = 10,
		levels[symbolFromStr("≤")] = 10,
		levels[symbolFromStr("≥")] = 10,
		levels[symbolFromStr("&&")] = 9,
		levels[symbolFromStr("∧")] = 9,
		levels[symbolFromStr("||")] = 8,
		levels[symbolFromStr("∨")] = 8,
		levels[symbolFromStr(",")] = 7,
		levels[symbolFromStr("$")] = 6,
		//#intern("if")] = 4,
		levels[symbolFromStr("elif")] = 5,
		levels[symbolFromStr("else")] = 5,
		levels[symbolFromStr("|")] = 4,
		levels[symbolFromStr("=>")] = 3,
		levels[symbolFromStr(";")] = 3,
		levels[symbolFromStr("?;")] = 3,
		levels[symbolFromStr("\\")] = 2,
		levels[symbolFromStr("let")] = 0,
		levels[symbolFromStr("in")] = 1,
		levels[symbolFromStr("from")] = 1,
		levels[symbolFromStr("let!")] = 0,
		levels[symbolFromStr("import")] = 0,
		levels[symbolFromStr(")")] = -1,
		levels[symbolFromStr("}")] = -1,
		levels[symbolFromStr("]")] = -1,
		levels[symbolFromStr("<dedent>")] = -1;
	}
}
Values::NodeT Lang5D::error(std::string expectedPart, std::string gotPart) const {
	// FIXME nicer
	return cons(Serror, cons(strCXX(expectedPart), strCXX(gotPart)));
}
bool Lang5D::errorP(Values::NodeT node) const {
	return consP(node) && getConsHead(node) == Serror;
}
bool Lang5D::operatorP(Values::NodeT node) const {
	return levels.find(node) != levels.end();
}
int Lang5D::operatorArgcount(Values::NodeT node) const {
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
		   (node == Sin) ? R :
		   (node == Sfrom) ? R :
		   (node == Sletexclam) ? R :
		   (node == Simport) ? P :
		   macroStarterP(node) ? P :
		   (node == SLF) ? S : 
		   (node == Shashexports) ? P : 
		   2;
}
//bool Lang5D::operatorPrefixNeutralP(Values::NodeT node) const {
Values::NodeT Lang5D::operatorPrefixNeutral(Values::NodeT node) const {
	return (node == Sdash) ? Szero : error("<prefix-operator>", getSymbol1Name(node));
}
bool Lang5D::macroStarterP(Values::NodeT node) const {
	return /*(node == Slet) || (node == Simport) ||*/ (node == Sbackslash) || (node == Sleftbracket) || (node == Sleftcurly);
}
static Values::NodeT macroStandinOperator(NodeT c) {
	return getConsHead(c);
}
static Values::NodeT macroStandinOperand(NodeT c) {
	return getConsHead(getConsTail(c));
}
static Values::NodeT macroStandin(NodeT operator_, NodeT operand) {
	return cons(operator_, cons(operand, nil));
}
static bool macroStandinP(NodeT c) {
	return(consP(c));
}
Values::NodeT Lang5D::parseListLiteral(Values::NodeT endToken, Scanner<Lang5D>& tokenizer) const {
	if(tokenizer.getToken() == endToken)
		return nil;
	else {
		NodeT hd = parseValue(tokenizer);
		return cons(hd, parseListLiteral(endToken, tokenizer));
	}
}
Values::NodeT Lang5D::startMacro(Values::NodeT node, Scanner<Lang5D>& tokenizer) const {
	if(node == Sbackslash) {
		return macroStandin(node, tokenizer.consume());
	} else if(node == Sleftbracket) {
		return parseListLiteral(Srightbracket, tokenizer);
		// tokenizer.consume(); // right bracket will be consumed by the Shunting Yard Parser (it has '[' on its 'operators stack)
		//std::vector<NodeT ALLOCATOR_VECTOR> items = Scanners::parse(tokenizer, *this, Srightbracket);
	}
	return node;
}
bool Lang5D::openingParenP(Values::NodeT node) const {
	return (node == Sleftparen) || (node == Sleftcurly) || (node == Sleftbracket) || (node == Sindent);
}
bool Lang5D::closingParenP(Values::NodeT node) const {
	return (node == Srightparen) || (node == Srightcurly) || (node == Srightbracket) || (node == Sdedent);
}
Values::NodeT Lang5D::openingParenOf(Values::NodeT node) const {
	return (node == Srightparen) ? Sleftparen : 
	       (node == Srightcurly) ? Sleftcurly : 
		   (node == Srightbracket) ? Sleftbracket : 
		   (node == Sdedent) ? Sindent : 
		   node;
}
bool Lang5D::operatorLE(Values::NodeT a, Values::NodeT b) const {
	if(a == b) { /* speed optimization */
		return operatorArgcount(b) > 0;
	} else {
		return levels[a] < levels[b] || (levels[a] == levels[b] && operatorArgcount(b) > 0); // latter: leave right-associative operators on stack if in doubt.
	}
}
Values::NodeT Lang5D::collect(FILE* file, int& linenumber, int prefix, bool (*continueP)(int input)) const {
	int c;
	std::stringstream sst;
	sst << (char) prefix;
	while((c = GETC) != EOF && (*continueP)(c)) {
		sst << (char) c;
	}
	std::string s = sst.str();
	UNGETC(c);
	if(s.length() > 0) {
		return symbolFromStr(s.c_str());
	} else
		return error("<value>", "<nothing>");
}
Values::NodeT Lang5D::collect1(FILE* file, int& linenumber, bool (*continueP)(int input)) const {
	int prefix;
	prefix = GETC;
	if(prefix == EOF)
		return error("<value>", "<EOF>");
	else
		return collect(file, linenumber, prefix, continueP);
}
Values::NodeT Lang5D::collectC(FILE* file, int& linenumber, int prefix, bool (*continueP)(int input)) const {
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
		}
		sst << (char) c;
	}
	std::string s = sst.str();
	UNGETC(c);
	if(s.length() > 0) {
		return symbolFromStr(s.c_str());
	} else
		return error("<value>", "<nothing>");
}
Values::NodeT Lang5D::collect1C(FILE* file, int& linenumber, bool (*continueP)(int input)) const {
	int prefix;
	prefix = GETC;
	if(prefix == EOF)
		return error("<value>", "<EOF>");
	else
		return collect(file, linenumber, prefix, continueP);
}
Values::NodeT Lang5D::collectUnicodeID(FILE* file, int& linenumber, int prefix, const std::string& prev) const {
	int c;
	std::stringstream sst;
	sst << (char) prefix << prev;
	while((c = GETC) != EOF && unicodeIDRestCharP(c)) {
		sst << (char) c;
	}
	std::string s = sst.str();
	UNGETC(c);
	if(s.length() > 0)
		return symbolFromStr(s.c_str());
	else
		return error("<value>", "<nothing>");
}
Values::NodeT Lang5D::readUnicodeOperator3(FILE* file, int& linenumber, int c) const {
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
		return symbolFromStr(buf);
	} else
		return collectUnicodeID(file, linenumber, buf[0], &buf[1]);
}
Values::NodeT Lang5D::readOperator(FILE* file, int& linenumber, int c) const {
	return collect(file, linenumber, c, operatorCharP);
}
Values::NodeT Lang5D::readDigits(FILE* file, int& linenumber, int c) const {
	return collect(file, linenumber, c, digitRestCharP);
}
int Lang5D::collectNumeric3(FILE* file, int& linenumber, int base, bool (*continueP)(int input)) const {
	abort();
	return(2);
}
Values::NodeT Lang5D::collectNumeric2(FILE* file, int& linenumber, int base, bool (*continueP)(int input)) const {
	abort(); // FIXME
	return(nil);
}
Values::NodeT Lang5D::readShebang(FILE* file, int& linenumber, int c) const {
	return collect(file, linenumber, c, shebangBodyCharP);
}
Values::NodeT Lang5D::readHashExports(FILE* file, int& linenumber, int c) const {
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
Values::NodeT Lang5D::readKeyword(FILE* file, int& linenumber, int c) const {
	return collect(file, linenumber, c, keywordBodyCharP);
}
Values::NodeT Lang5D::readSpecialCoding(FILE* file, int& linenumber, int c2) const {
	int c = GETC;
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
Values::NodeT Lang5D::readString(FILE* file, int& linenumber, int c) const {
	if(c == '"') { /* FIXME error handling */
		Values::NodeT n = collect1C(file, linenumber, stringBodyCharP);
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
Values::NodeT Lang5D::readToken(FILE* file, int& linenumber) const {
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
		return symbolFromStr(buf);
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
Values::NodeT Lang5D::mcall(Values::NodeT a, Values::NodeT b) const {
	return call(a,b);
}
Values::NodeT Lang5D::replaceIN(Values::NodeT equation, Values::NodeT body) const {
	/* x = 5 => ((= x) 5) */
	if(!operationP(equation))
		return error("<equation>", "<junk>");
	if(getOperationOperator(equation) != Sequal)
		return error("<equation>", "<inequation>");
	NodeT formalParameter = getOperationArgument1(equation);
	NodeT value = getOperationArgument2(equation);
	return call(fn(formalParameter, body), value);
}
Values::NodeT Lang5D::moperation(Values::NodeT operator_, Values::NodeT a, Values::NodeT b) const {
	return operator_ == Sbackslash ? fn(macroStandinOperand(a),b) : 
	       operator_ == Sin ? replaceIN(a ,b) :
		   operation(operator_, a, b);
}
void Lang5D::callRpnOperator(NodeT operator_, std::vector<NodeT ALLOCATOR_VECTOR>& values) const {
	int argcount = operatorArgcount(operator_);
	if(argcount < 0)
		argcount = -argcount;
	if(operator_ == SLF || operator_ == Slet || operator_ == Simport) { /* ignore for now */
		//fprintf(stderr, "LF NONE\n");
		return;
	}
	if(argcount == 1) {
		NodeT a = values.back();
		values.pop_back();
		values.push_back(mcall(operator_,a));
		return;
	}
	assert(argcount == 2);
	if(values.size() < 2) {
		fprintf(stderr, "NOT ENOUGH\n");
		values.push_back(error("<2-arguments>", "<too-little>"));
	} else {
		fprintf(stderr, "TWO ARGS \"");
		Formatters::TExpression::print(stderr, operator_);
		fprintf(stderr, "\" ");
		NodeT b = values.back();
		values.pop_back();
		NodeT a = values.back();
		values.pop_back();
		Formatters::TExpression::print(stderr, a);
		//str(a, stdout);
		fprintf(stderr, "!");
		Formatters::TExpression::print(stderr, b);
		//str(b, stdout);
		fprintf(stderr, "\n");
		values.push_back(operator_ == Sapply ? mcall(a,b) : moperation(operator_, a, b));
	}
}
Values::NodeT Lang5D::parseValue(Scanner<Lang5D>& scanner) const {
	NodeT token = scanner.getToken();
	if(token == Sleftparen) {
		return parse0(scanner, Srightparen);
	} else
		return scanner.consume();
}
Values::NodeT Lang5D::parse0(Scanner<Lang5D>& scanner, NodeT endToken) const {
	Values::NodeT result;
	std::vector<NodeT ALLOCATOR_VECTOR> prog;
	prog = Scanners::parse(scanner, *this, endToken);
	std::vector<NodeT ALLOCATOR_VECTOR>::const_iterator iter = prog.begin(); 
	if(iter != prog.end()) {
		result = *iter;
		++iter;
		if(iter == prog.end())
			return result;
		else
			return error("<nothing>", "<junk>");
	} else
		return error("<something>", "<nothing>");
}
Values::NodeT Lang5D::parse(Scanner<Lang5D>& scanner) const {
	return parse0(scanner, SEOF);
}
Values::NodeT Lang5D::parse1(FILE* f, const char* name) const {
	Scanner<Lang5D> scanner(*this);
	scanner.push(f, 1, name);
	scanner.consume();
	return parse(scanner);
}
Values::NodeT Lang5D::error(Values::NodeT expectedPart, Values::NodeT gotPart) const {
	/* FIXME */
	return error("???", "???");
}

}
