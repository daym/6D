/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include "Parsers/Lang5D"
#include "6D/Values"
#include "6D/Evaluators"
#include "6D/Builtins"
#include "6D/Arithmetics"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Builtins)
USE_NAMESPACE_6D(Formatters::TExpression)
#include "../sillyprint.inc"
int main(int argc, char* argv[]) {
	initAllocators();
	initArithmetics();
	initEvaluators();
	NodeT defaultDynEnv = initLang();
	NodeT prog = argc > 1 ? Lang_parse1(fopen(argv[1], "r"), argv[1]) : Lang_parse1(stdin, "<stdin>");
	// TODO Memoize
	prog = closeOver(symbolFromStr("Builtins"), initBuiltins(), withArithmetics(Lang_withDefaultEnv(prog)));
	prog = annotate(defaultDynEnv, prog); //lang5D.withDefaultEnv(prog));
	prog = eval(prog);
	S_print(prog);
	fflush(stdout);
	if(errorP(prog))
		return 1;
	return 0;
}
