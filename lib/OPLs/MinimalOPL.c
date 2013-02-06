#include "6D/Values"
#include "6D/Operations"
#include "6D/FFIs"
#include "OPLs/MinimalOPL"
BEGIN_NAMESPACE_6D(OPLs)
static NodeT Sunderscore;
static NodeT Scircumflex;
static NodeT Sstarstar;
static NodeT Scross;
static NodeT Scolon;
static NodeT Squote;
static NodeT Scomma;
static NodeT Sdollar;
static NodeT Selif;
static NodeT Sdash;
static NodeT Selse;
static NodeT Ssemicolon;
static NodeT Sbackslash;
static NodeT Slet;
static NodeT Sletexclam;
static NodeT Simport;
static NodeT Sleftbracket;
static NodeT Sleftcurly;
static NodeT Szero;
static NodeT Sif;
static NodeT Sin;
static NodeT Sfrom;
static NodeT SLF;
static NodeT Shashexports;
static NodeT SoperatorLevel;
static NodeT SoperatorArgcount;
static NodeT SoperatorPrefixNeutral;
static NodeT Sexports;
static int operatorLevel(NodeT n) {
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
	       n == symbolFromStr("from") ? 3 : 
	       n == symbolFromStr("import") ? 2 : 
	       n == symbolFromStr("let") ? 1 : 
	       n == symbolFromStr("in") ? 1 : 
	       n == symbolFromStr("let!") ? 0 : 
	       n == symbolFromStr(")") ? -1 : 
	       n == symbolFromStr("}") ? -1 : 
	       n == symbolFromStr("]") ? -1 : 
	       n == symbolFromStr("<dedent>") ? -1 :
	       NO_OPERATOR;
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
	       (node == Sletexclam) ? P :
	       (node == Simport) ? P :
	       (node == Sleftbracket) ? P :
	       (node == Sleftcurly) ? P :
	       (node == SLF) ? S : 
	       (node == Shashexports) ? P : 
	       2;
}
static NodeT operatorPrefixNeutral(NodeT node) {
        return (node == Sdash) ? Szero : parseError(strC("<prefix-operator>"), strC(getSymbol1Name(node)));
}
DEFINE_STRICT_FN(wrap_operatorLevel, internNativeInt(operatorLevel(argument)))
DEFINE_STRICT_FN(wrap_operatorArgcount, internNativeInt(operatorArgcount(argument)))
DEFINE_STRICT_FN(wrap_operatorPrefixNeutral, internNativeNode(operatorPrefixNeutral(argument)))
NodeT minimalOPL(NodeT sym) {
	if(sym == SoperatorLevel)
		return wrap_operatorLevel;
	else if(sym == SoperatorArgcount)
		return wrap_operatorArgcount;
	else if(sym == SoperatorPrefixNeutral)
		return wrap_operatorPrefixNeutral;
	else if(sym == Sexports)
		return cons(SoperatorLevel, cons(SoperatorArgcount, cons(SoperatorPrefixNeutral, nil)));
	else
		return evalError(strC("<OPL-member>"), strC(getSymbol1Name(sym)), sym);
}
void initMinimalOPL(void) {
	if(Sdash == NULL) {
		Scolon = symbolFromStr(":");
		Slet = symbolFromStr("let");
		Sletexclam = symbolFromStr("let!");
		Simport = symbolFromStr("import");
		Sleftbracket = symbolFromStr("[");
		Sleftcurly = symbolFromStr("{");
		Sdash = symbolFromStr("-");
		Szero = symbolFromStr("0");
		Sunderscore = symbolFromStr("_");
		Scircumflex = symbolFromStr("^");
		Sstarstar = symbolFromStr("**");
		Scross = symbolFromStr("⨯");
		Squote = symbolFromStr("'");
		Scomma = symbolFromStr(",");
		Sdollar = symbolFromStr("$");
		Selif = symbolFromStr("elif");
		Selse = symbolFromStr("else");
		Ssemicolon = symbolFromStr(";");
		Sbackslash = symbolFromStr("\\");
		Sif = symbolFromStr("if");
		Sin = symbolFromStr("in");
		Sfrom = symbolFromStr("from");
		SLF = symbolFromStr("<LF>");
		Shashexports = symbolFromStr("#exports");
		SoperatorLevel = symbolFromStr("operatorLevel");
		SoperatorArgcount = symbolFromStr("operatorArgcount");
		SoperatorPrefixNeutral = symbolFromStr("operatorPrefixNeutral");
		Sexports = symbolFromStr("exports");
		INIT_FN(wrap_operatorLevel);
		INIT_FN(wrap_operatorArgcount);
		INIT_FN(wrap_operatorPrefixNeutral);
	}
}
END_NAMESPACE_6D(OPLs)
