/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <assert.h>
#include "6D/Values"
#include "Values/Values"
#include "6D/FFIs"
#include "Numbers/Integer2"
#include "Numbers/Small"
/* assumes two's complement */
/* assumes the value is always stored in the shortest possible form */
/* a number is a list of the form [Integer Integer Integer Int] whereas the last Int is the most significant part. The last Int also includes a (two's complement) "sign" bit. */
/* disadvantage: you don't know whether a number is smaller than 0 or bigger than 0 without traversing the entire number */
BEGIN_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
/* cons-like list of NativeUInt, least significant first. */
/* TODO reuse common tails. */
bool intP(NodeT node) {
	return tagOfNode(node) == TAG_Int;
}
#define CACHED_INT_FRONTIER 256
static struct Int ints[CACHED_INT_FRONTIER];
static NodeT makeInt(NativeUInt value) {
	struct Int* result = NEW(Int);
	result->value = value;
	return refCXXInstance(result);
}
void initIntegers(void) {
	int i = -1;
	assert((unsigned int) i == ~0); /* two's complement */
	for(i = 0; i < CACHED_INT_FRONTIER; ++i) {
		Node_initTag((struct Node*) &ints[i], TAG_Int);
		ints[i].value = i;
	}
}
static INLINE NodeT intA(NativeUInt value) {
	if(value >= 0U && value < CACHED_INT_FRONTIER)
		return(refCXXInstance(&ints[value]));
	/* TODO cache small negative values */
	return makeInt(value);
}
#define HIGHBIT (NATIVEUINT_ONE<<(NATIVEINT_BITCOUNT - 1))
static INLINE bool negativeP(NativeUInt amount) {
	return (amount&HIGHBIT) != 0;
}
END_NAMESPACE_6D(FFIs)
