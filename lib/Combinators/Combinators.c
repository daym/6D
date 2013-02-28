/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#include "6D/Operations"
#include "6D/Evaluators"

BEGIN_NAMESPACE_6D(Combinators)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Evaluators)
NodeT Identity;
NodeT Konstant;
/* TODO Substitution */
void initCombinators(void) {
	NodeT a = symbolFromStr("a");
	NodeT b = symbolFromStr("b");
	Identity = annotate(nil, fn(a, a));
	Konstant = annotate(nil, fn(a, fn(b, a)));
}

END_NAMESPACE_6D(Combinators)
