/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
#define S(x) symbolFromStr(x)
static int operatorLevel(NodeT n) {
	return n == S("(") || n == S("{") || n == S("[") ? -1 : // pseudo-operators
	       n == S("<indent>") ? -1 : 
	       n == S("<LF>") ? 41 : 
	       n == S("#exports") ? 40 : 
	       n == S(".") ? 33 : 
	       n == S("_") || n == S("^") ? 32 : 
	       n == S("!") ? 31 : 
	       n == S("**") ? 30 : 
	       n == S("*") || n == S("⋅") || n == S("/") ? 29 : 
	       n == S("⨯") ? 28 : 
	       n == S(":") ? 27 : 
	       n == S("'") ? 26 : 
	       n == S("if") ? 25 : 
	       n == S(" ") ? 24 : 
	       n == S("++") ? 23 : 
	       n == S("+") || n == S("-") ? 22 : 
	       n == S("%") ? 21 : 
	       n == S("∩") ? 17 : 
	       n == S("∪") ? 16 : 
	       n == S("∈") || n == S("⊂") || n == S("⊃") || n == S("⊆") || n == S("⊇") ? 15 : 
	       n == S("=") || n == S("≟") || n == S("/=") ? 14 : 
	       n == S("<") || n == S("<=") || n == S(">") || n == S(">=") || n == S("≤") || n == S("≥") ? 13 : 
	       n == S("&&") || n == S("∧") ? 12 : 
	       n == S("||") || n == S("||") ? 11 : 
	       n == S(",") ? 10 : 
	       n == S("$") ? 9 : 
	       n == S("elif") || n == S("else") ? 8 : 
	       n == S("|") ? 6 : 
	       n == S("=>") || n == S(";") || n == S("?;") ? 5 : 
	       n == S("\\") ? 4 : 
	       n == S(":=") || n == S("from") ? 3 : 
	       n == S("import") ? 2 : 
	       n == S("let") || n == S("in") ? 1 : 
	       n == S("let!") ? 0 : 
	       n == S(")") || n == S("}") || n == S("]") ? -1 : 
	       n == S("<dedent>") ? -1 :
	       NO_OPERATOR;
}
#undef S
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
        return (node == Sdash) ? Szero : parseError(strC("<prefix-operator>"), strC(symbolName(node)));
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
		return evalError(strC("<OPL-member>"), strC(symbolName(sym)), sym);
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
