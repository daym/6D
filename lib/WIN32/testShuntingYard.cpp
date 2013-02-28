/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
// testShuntingYard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "6D/Values"
#include "Parsers/ShuntingYardParser"
#include "Scanners/Scanner"
#include "Parsers/Lang5D"
#include "Formatters/TExpression"
#include "Formatters/Math"
int _tmain(int argc, _TCHAR* argv[]) {
	USE_NAMESPACE_6D(Scanners)
	USE_NAMESPACE_6D(Values)
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

