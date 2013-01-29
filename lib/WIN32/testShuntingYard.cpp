// testShuntingYard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Parsers/ShuntingYardParser"
#include "Scanners/Scanner"
#include "Parsers/Lang5D"
#include "Formatters/TExpression"
#include "Formatters/Math"
int _tmain(int argc, _TCHAR* argv[]) {
	using namespace Scanners;
	using namespace Values;
	Lang5D lang;
	//FILE* f = (argc > 1) ? _wfopen(argv[1], _T("rb")) : stdin;
	FILE* f = (argc > 1) ? _wfopen(argv[1], _T("rb")) : _wfopen(_T("..\\Tests\\01_ShuntingYard\\25.5D"), _T("r"));
	NodeT value = lang.parse1(f, "<input file>");
	Formatters::TExpression::print(stdout, value);
	fflush(stdout);
	//NodeT value = 
	//Formatters::SExpression::p1rint(value);
	
	return 0;
}

