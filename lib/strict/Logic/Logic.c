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
#include "Logic/Logic"
#include "SpecialForms/SpecialForms"
#include "Combinators/Combinators"

BEGIN_NAMESPACE_6D(Logic)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Combinators)
USE_NAMESPACE_6D(FFIs)
NodeT trueN;
NodeT falseN;
static NodeT t;
static NodeT f;
static NodeT a;
static NodeT b;
static NodeT wrapL(NodeT n) {
	return n;
}
DEFINE_STRICT_FN(Conjunction, argument == f ? constanter2(f) : Identity)
DEFINE_STRICT_FN(Disjunction, argument != f ? constanter2(uargument) : Identity)
DEFINE_STRICT_FN(Negation, argument == f ? t : f)
void initLogic(void) {
	t = symbolFromStr("t");
	f = nil; // TODO symbolFromStr("f");
	a = symbolFromStr("a");
	b = symbolFromStr("b");
	trueN = wrapL(t);
	falseN = wrapL(f);
}

END_NAMESPACE_6D(Logic)
BEGIN_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Logic)
NodeT internNativeBool(bool value) {
	return value ? t : f;
}
bool booleanFromNode(NodeT n, bool* result) {
	*result = (n != f);
	return true;
}
END_NAMESPACE_6D(FFIs)
