#include <assert.h>
#include "6D/Values"
#include "Values/Values"
#include "6D/FFIs"
#include "Numbers/Integer2"
/* assumes two's complement */
/* assumes the value is always stored in the shortest possible form */
/* a number is a list of the form [Integer Integer Integer Int] whereas the last Int is the most significant part. The last Int also includes a (two's complement) "sign" bit. */
/* disadvantage: you don't know whether a number is smaller than 0 or bigger than 0 without traversing the entire number */
BEGIN_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
/* cons-like list of NativeUInt, least significant first. */
/* TODO reuse common tails. */
BEGIN_STRUCT_6D(Integer)
	NodeT tail; /* Integer or Int */
	NativeUInt value;
END_STRUCT_6D(Integer)
BEGIN_STRUCT_6D(Int)
	NativeUInt value;
END_STRUCT_6D(Int)
bool intP(NodeT node) {
	return tagOfNode(node) == TAG_INT;
}
bool integerP(NodeT node) {
	return tagOfNode(node) == TAG_INTEGER;
}
static Int ints[256];
static NodeT makeInt(NativeUInt value) {
	Int* result = new Int;
	result->value = value;
	return result;
}
static Integer zerozerozerozero; // self { value = 0, tail = self }
void initIntegers(void) {
	int i;
	zerozerozerozero.tail = &zerozerozerozero;
	zerozerozerozero.value = 0U;
	for(i = 0; i < 256; ++i)
		ints[i].value = i;
}
static inline NodeT intA(NativeUInt value) {
	if(value >= 0U && value < 256U)
		return(&ints[value]);
	return makeInt(value);
}
NodeT integerpart(NativeUInt value, NodeT tail) {
	Integer* result = new Integer;
	assert(tail);
	result->tail = tail;
	result->value = value;
	return result;
}
/* FIXME determine size */
#define HIGHBIT 63
#define ALL1 (~0U)
#define SIGNEXTENSION(value) ((value&HIGHBIT)*ALL1)
/* only suitable for adding "positive" amounts - the negative amounts would have to be sign-extended too much. */
NodeT integerAddU(NodeT aP, NativeUInt amount) {
	NativeUInt value;
	NodeT tail;
	if(amount == 0)
		return aP;
	if(intP(aP)) {
		const Int* a = (const Int*) getCXXInstance(aP);
		value = a->value;
		tail = nil;
	} else if(integerP(aP)) {
		const Integer* a = (const Integer*) getCXXInstance(aP);
		value = a->value;
		tail = a->tail;
		assert(tail);
	} else
		return evalError(strCXX("<integer>"), strCXX("<junk>"), aP);
	NativeUInt value2 = value + amount;
	if(UNLIKELY_6D(!tail && (value2&HIGHBIT) && !(value&HIGHBIT)/* precond && !(amount&HIGHBIT)*/)) { /* flipped sign accidentally (middle) */
		/* this can only happen with the MSB of the entire number.  */
		assert(value2 >= value); /* Note that overflow cannot happen anyway because the sign switch was in the "middle" of the range. */
		NodeT newMS = intA(SIGNEXTENSION(value)); /* sign extend */
		return integerpart(value2, newMS);
	}
	return tail ? integerpart(value2, (value2 < value) ? integerAddU(tail, 1) : tail) : intA(value2);
}
/* only suitable for adding "negative" amounts */
NodeT integerAddN(NodeT aP, NativeUInt amount) {
	NativeUInt value;
	NodeT tail;
	if(intP(aP)) {
		const Int* a = (const Int*) getCXXInstance(aP);
		value = a->value;
		tail = nil;
	} else if(integerP(aP)) {
		const Integer* a = (const Integer*) getCXXInstance(aP);
		value = a->value;
		tail = a->tail;
		assert(tail);
	} else 
		return evalError(strCXX("<integer>"), strCXX("<junk>"), aP);
	NativeUInt value2 = value + amount;
	if(UNLIKELY_6D(!tail && !(value2&HIGHBIT) && (value&HIGHBIT) /*&& precond amount&HIGHBIT */)) { /* flipped sign accidentally */
		/* this can only happen with the MSB of the entire number */
		assert(value2 >= value); /* Note that overflow cannot happen anyway because the sign switch was in the "middle" of the range. */
		NodeT newMS = intA(SIGNEXTENSION(value)); /* sign extend */
		return integerpart(value2, newMS);
	}
	return tail ? integerpart(value2, integerAddN(integerAddU(tail, (value2 < value) ? 1 : 0), SIGNEXTENSION(value))) : intA(value2);
}
NodeT integerAdd(NodeT aP, NodeT bP) {
	NativeUInt avalue;
	NodeT atail;
	NativeUInt bvalue;
	NodeT btail;
	if(intP(aP)) {
		const Int* a = (const Int*) getCXXInstance(aP);
		avalue = a->value;
		atail = nil;
	} else if(integerP(aP)) {
		const Integer* a = (const Integer*) getCXXInstance(aP);
		avalue = a->value;
		atail = a->tail;
		assert(atail);
	} else
		return evalError(strCXX("<integer>"), strCXX("<junk>"), aP);
	if(intP(bP)) {
		const Int* b = (const Int*) getCXXInstance(bP);
		bvalue = b->value;
		btail = nil;
	} else if(integerP(bP)) {
		const Integer* b = (const Integer*) getCXXInstance(bP);
		bvalue = b->value;
		btail = b->tail;
		assert(btail);
	} else
		return evalError(strCXX("<integer>"), strCXX("<junk>"), bP);
	if(atail && !btail)
		btail = &zerozerozerozero;
	if(btail && !atail)
		atail = &zerozerozerozero;
	// here, either both tails are nil or both tails are not nil.
	NativeUInt value2 = avalue + bvalue;
	if(UNLIKELY_6D(!atail && 
	   (((value2&HIGHBIT) && !(avalue&HIGHBIT) && !(bvalue&HIGHBIT)) || 
	   ((!(value2&HIGHBIT) && (avalue&HIGHBIT) && (bvalue&HIGHBIT)))))) { /* flipped sign accidentally */
		assert(value2 >= avalue); /* Note that overflow cannot happen anyway because the sign switch was in the "middle" of the range. */
		NodeT newMS = intA(SIGNEXTENSION(avalue)); /* sign extend */
		return integerpart(value2, newMS);
	}
	return atail ? integerpart(value2, integerAddU(integerAdd(atail, btail), (value2 < avalue) ? 1 : 0)) : intA(value2);
}
NodeT integerSucc(NodeT aP) {
	return integerAddU(aP, 1);
}
NodeT integerMul(NodeT aP, NodeT bP) {
	/* TODO prefer the shorter one */
	NodeT result = intA(0);
	NativeUInt bvalue;
	NodeT btail;
	if(intP(bP)) {
		const Int* b = (const Int*) getCXXInstance(bP);
		bvalue = b->value;
		btail = nil;
	} else if(integerP(bP)) {
		const Integer* b = (const Integer*) getCXXInstance(bP);
		bvalue = b->value;
		btail = b->tail;
		assert(btail);
	} else
		return evalError(strCXX("<integer>"), strCXX("<junk>"), bP);
	for(NativeUInt mask = HIGHBIT; mask; mask >>= 1) {
		result = integerShl(result, 1);
		if(bvalue&mask)
			result = integerAdd(result, aP);
	}
	return integerAdd(result, integerShl(integerMul(aP, btail), HIGHBIT + 1));
}
NodeT integerShl(NodeT aP, unsigned amount) {
	abort();
	return aP;
}
NodeT integerShr(NodeT aP, unsigned amount) {
	abort();
	return aP;
}
bool integerEqualsP(NodeT aP, NodeT bP) {
	NativeUInt avalue;
	NodeT atail;
	NativeUInt bvalue;
	NodeT btail;
	if(intP(aP)) {
		const Int* a = (const Int*) getCXXInstance(aP);
		avalue = a->value;
		atail = nil;
	} else if(integerP(aP)) {
		const Integer* a = (const Integer*) getCXXInstance(aP);
		avalue = a->value;
		atail = a->tail;
		assert(atail);
	} else
		return evalError(strCXX("<integer>"), strCXX("<junk>"), aP);
	if(intP(bP)) {
		const Int* b = (const Int*) getCXXInstance(bP);
		bvalue = b->value;
		btail = nil;
	} else if(integerP(bP)) {
		const Integer* b = (const Integer*) getCXXInstance(bP);
		bvalue = b->value;
		btail = b->tail;
		assert(btail);
	} else
		return evalError(strCXX("<integer>"), strCXX("<junk>"), bP);
	if(atail && !btail)
		return false;
	if(btail && !atail)
		return false;
	if(avalue != bvalue)
		return false;
	return integerEqualsP(atail, btail);
}
bool integerEqualsIntP(NodeT aP, NativeInt value) {
	FFIs::NativeInt a;
	if(toNativeInt(aP, a))
		return a == value;
	else
		return false;
}
NodeT internNative(NativeInt value) {
	return intA((NativeUInt) value);
}

END_NAMESPACE_6D(Values)
BEGIN_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Values)
bool toNativeInt(NodeT aP, NativeInt& result) {
	if(intP(aP)) {
		const Int* a = (const Int*) getCXXInstance(aP);
		result = (NativeInt) a->value;
		return true;
	} else
		return false;
}
bool toNearestNativeInt(NodeT node, NativeInt& result) {
	if(intP(node))
		return toNativeInt(node, result);
	while(integerP(node)) {
		const Integer* a = (const Integer*) getCXXInstance(node);
		node = a->tail;
	}
	if(intP(node)) {
		const Int* a = (const Int*) getCXXInstance(node);
		NativeUInt result = 1U << HIGHBIT;
		if(a->value&HIGHBIT) { /* negative */
			return (NativeInt) result;
		} else {
			--result;
			return (NativeInt) result;
		}
		return true;
	}
	return false;
}
NodeT internNative(NativeInt value) {
	return intA((FFIs::NativeUInt) value);
}
NodeT internNativeU(NativeUInt value) {
	if(value&HIGHBIT) {
		FFIs::NativeUInt bvalue = value&~HIGHBIT;
		NodeT v = intA(bvalue);
		return integerAddU(v, HIGHBIT);
	} else
		return intA(value);
}

END_NAMESPACE_6D(FFIs)
