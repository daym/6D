/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#include "6D/Evaluators"
#include "SpecialForms/Unapplier"
#include "Numbers/Integer2"
BEGIN_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Values)
static NodeT unapplyN(NodeT accum, NodeT argument) {
	NodeT count = consHead(accum);
	NodeT t1 = consTail(accum);
	NodeT fn = consHead(t1);
	NodeT t2 = consTail(t1);
	if(integerCompareD(count, 0) <= 0)
		return dcall(fn, t2); /* order is reversed */
	else {
		return CLOSED_FN(Unapplier3, cons(integerSubU(count, 1U), cons(fn, cons(argument, t2))));
	}
}
static NodeT unapply(NodeT count, NodeT fn) {
	return CLOSED_FN(Unapplier3, cons(count, cons(fn, nil)));
}
DEFINE_SPECIAL_FORM(Unapplier3, unapplyN(env, argument))
DEFINE_SPECIAL_FORM(Unapplier2, unapply(env, argument))
DEFINE_SPECIAL_FORM(Unapplier, CLOSED_FN(Unapplier2, argument))
END_NAMESPACE_6D(SpecialForms)

/* unapply 4 fn a b c d => fn [a b c d] */
