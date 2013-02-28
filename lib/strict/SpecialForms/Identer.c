/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#include "SpecialForms/Identer"
BEGIN_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Values)
DEFINE_SPECIAL_FORM(Identer2, env)
NodeT identer2(NodeT argument) {
	return CLOSED_FN(Identer2, argument); // call(CLOSED_FN(), argument);
}
/* mostly for debugging */
DEFINE_SPECIAL_FORM(Identer, identer2(argument))

END_NAMESPACE_6D(SpecialForms)
