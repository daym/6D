/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#include "SpecialForms/SpecialForms"
BEGIN_NAMESPACE_6D(SpecialForms)
void initSpecialForms(void) {
	INIT_FN(Quoter);
	INIT_FN(Constanter);
	INIT_FN(Identer);
	INIT_FN(Unapplier);
}
END_NAMESPACE_6D(SpecialForms)
