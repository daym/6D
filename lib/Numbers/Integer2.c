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
bool integerP(NodeT node) {
	return tagOfNode(node) == TAG_Integer;
}
static struct Int ints[256];
static NodeT makeInt(NativeUInt value) {
	struct Int* result = NEW(Int);
	result->value = value;
	return refCXXInstance(result);
}
static struct Integer zerozerozerozero; // self { value = 0, tail = self }
void initIntegers(void) {
	int i;
	zerozerozerozero.tail = refCXXInstance(&zerozerozerozero);
	zerozerozerozero.value = 0U;
	for(i = 0; i < 256; ++i) {
		Node_initTag((struct Node*) &ints[i], TAG_Int);
		ints[i].value = i;
	}
}
static INLINE NodeT intA(NativeUInt value) {
	if(value >= 0U && value < 256U)
		return(refCXXInstance(&ints[value]));
	return makeInt(value);
}
NodeT integerpart(NativeUInt value, NodeT tail) {
	struct Integer* result = NEW(Integer);
	assert(tail);
	result->tail = tail;
	result->value = value;
	return refCXXInstance(result);
}
/* FIXME determine size */
#define HIGHBIT 63
#define ALL1 (~0U)
#define SIGNEXTENSION(value) ((value&HIGHBIT)*ALL1)
/* only suitable for adding "positive" amounts - the negative amounts would have to be sign-extended too much. */
NodeT integerAddU(NodeT aP, NativeUInt amount) {
	NativeUInt value;
	NodeT tail;
	if(amount == 0U)
		return aP;
	if(intP(aP)) {
		const struct Int* a = (const struct Int*) getCXXInstance(aP);
		value = a->value;
		tail = nil;
	} else if(integerP(aP)) {
		const struct Integer* a = (const struct Integer*) getCXXInstance(aP);
		value = a->value;
		tail = a->tail;
		assert(tail);
	} else
		return evalError(strC("<integer>"), strC("<junk>"), aP);
	NativeUInt value2 = value + amount;
	if(UNLIKELY_6D(!tail && (value2&HIGHBIT) && !(value&HIGHBIT)/* precond && !(amount&HIGHBIT)*/)) { /* flipped sign accidentally (middle) */
		/* this can only happen with the MSB of the entire number.  */
		assert(value2 >= value); /* Note that overflow cannot happen anyway because the sign switch was in the "middle" of the range. */
		NodeT newMS = intA(SIGNEXTENSION(value)); /* sign extend */
		return integerpart(value2, newMS);
	}
	return tail ? integerpart(value2, (value2 < value) ? integerAddU(tail, 1) : tail) : intA(value2);
}
NODET integerSubU(NODET aP, NATIVEUINT amount) {
	NativeUInt value;
	//NodeT tail;
	if(amount == 0U)
		return aP;
	if(intP(aP)) {
		const struct Int* a = (const struct Int*) getCXXInstance(aP);
		value = a->value;
		//tail = nil;
	} else if(integerP(aP)) {
		abort();
	} else 
		return evalError(strC("<integer>"), strC("<junk>"), aP);
	NativeUInt value2 = value - amount;
	return intA(value2);
}
NODET integerSub(NODET aP, NODET bP) {
	if(intP(bP)) {
		const struct Int* b = (const struct Int*) getCXXInstance(bP);
		assert(b->value >= 0);
		return integerSubU(aP, b->value);
	} else {
		abort();
		return nil;
	}
}
/* only suitable for adding "negative" amounts */
NodeT integerAddN(NodeT aP, NativeUInt amount) {
	NativeUInt value;
	NodeT tail;
	if(intP(aP)) {
		const struct Int* a = (const struct Int*) getCXXInstance(aP);
		value = a->value;
		tail = nil;
	} else if(integerP(aP)) {
		const struct Integer* a = (const struct Integer*) getCXXInstance(aP);
		value = a->value;
		tail = a->tail;
		assert(tail);
	} else 
		return evalError(strC("<integer>"), strC("<junk>"), aP);
	NativeUInt value2 = value + amount;
	if(UNLIKELY_6D(!tail && !(value2&HIGHBIT) && (value&HIGHBIT) /*&& precond amount&HIGHBIT */)) { /* flipped sign accidentally */
		/* this can only happen with the MSB of the entire number */
		assert(value2 >= value); /* Note that overflow cannot happen anyway because the sign switch was in the "middle" of the range. */
		NodeT newMS = intA(SIGNEXTENSION(value)); /* sign extend */
		return integerpart(value2, newMS);
	}
	return tail ? integerpart(value2, integerAddN(integerAddN(tail, (value2 < value) ? 1 : 0), SIGNEXTENSION(value))) : intA(value2);
}
NodeT integerAdd(NodeT aP, NodeT bP) {
	NativeUInt avalue;
	NodeT atail;
	NativeUInt bvalue;
	NodeT btail;
	if(intP(aP)) {
		const struct Int* a = (const struct Int*) getCXXInstance(aP);
		avalue = a->value;
		atail = nil;
	} else if(integerP(aP)) {
		const struct Integer* a = (const struct Integer*) getCXXInstance(aP);
		avalue = a->value;
		atail = a->tail;
		assert(atail);
	} else
		return evalError(strC("<integer>"), strC("<junk>"), aP);
	if(intP(bP)) {
		const struct Int* b = (const struct Int*) getCXXInstance(bP);
		bvalue = b->value;
		btail = nil;
	} else if(integerP(bP)) {
		const struct Integer* b = (const struct Integer*) getCXXInstance(bP);
		bvalue = b->value;
		btail = b->tail;
		assert(btail);
	} else
		return evalError(strC("<integer>"), strC("<junk>"), bP);
	if(atail && !btail)
		btail = refCXXInstance(&zerozerozerozero);
	if(btail && !atail)
		atail = refCXXInstance(&zerozerozerozero);
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
		const struct Int* b = (const struct Int*) getCXXInstance(bP);
		bvalue = b->value;
		btail = nil;
	} else if(integerP(bP)) {
		const struct Integer* b = (const struct Integer*) getCXXInstance(bP);
		bvalue = b->value;
		btail = b->tail;
		assert(btail);
	} else
		return evalError(strC("<integer>"), strC("<junk>"), bP);
	{
		NativeUInt mask;
		for(mask = HIGHBIT; mask; mask >>= 1) {
			result = integerShl(result, 1);
			if(bvalue&mask)
				result = integerAdd(result, aP);
		}
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
		const struct Int* a = (const struct Int*) getCXXInstance(aP);
		avalue = a->value;
		atail = nil;
	} else if(integerP(aP)) {
		const struct Integer* a = (const struct Integer*) getCXXInstance(aP);
		avalue = a->value;
		atail = a->tail;
		assert(atail);
	} else
		return false; // evalError(strC("<integer>"), strC("<junk>"), aP);
	if(intP(bP)) {
		const struct Int* b = (const struct Int*) getCXXInstance(bP);
		bvalue = b->value;
		btail = nil;
	} else if(integerP(bP)) {
		const struct Integer* b = (const struct Integer*) getCXXInstance(bP);
		bvalue = b->value;
		btail = b->tail;
		assert(btail);
	} else
		return false; // evalError(strC("<integer>"), strC("<junk>"), bP);
	if(atail && !btail)
		return false;
	if(btail && !atail)
		return false;
	if(avalue != bvalue)
		return false;
	return integerEqualsP(atail, btail);
}
bool integerEqualsIntP(NodeT aP, NativeInt value) {
	NativeInt a;
	if(toNativeInt(aP, &a))
		return a == value;
	else
		return false;
}
NativeInt integerCompare(NODET aP, NODET bP) {
	if(intP(aP) && intP(bP)) {
		const struct Int* a = (const struct Int*) getCXXInstance(aP);
		const struct Int* b = (const struct Int*) getCXXInstance(bP);
		/* FIXME make sure not to drop outside of the range */
		return a->value - b->value;
	} else if(integerP(aP) && integerP(bP)) {
		const struct Integer* a = (const struct Integer*) getCXXInstance(aP);
		const struct Integer* b = (const struct Integer*) getCXXInstance(bP);
		if(a->value != a->value) {
			/* FIXME make sure not to drop outside of the range */
			return a->value - b->value;
		}
		return integerCompare(a->tail, b->tail);
	} else {
		/* FIXME signal that the integer is bigger */
		return 99;
	}
}
NativeInt integerCompareU(NODET aP, NativeInt b) {
	if(intP(aP)) {
		const struct Int* a = (const struct Int*) getCXXInstance(aP);
		/* FIXME make sure not to drop outside of the range */
		return a->value - b;
	} else
		abort();
}
NodeT integerDivremU(NODET aP, NativeInt b) {
	if(intP(aP)) {
		const struct Int* aI = (const struct Int*) getCXXInstance(aP);
		NativeInt a = aI->value;
		if(b == 0)
			return evalError(strC("<nonzero-divisor>"), strC("0"), aP);
		printf("A %ld B %ld\n", a, b);
		/* force divisor to be positive (will not have effect on result says the C standard): */
		if(b < 0) {
			b = -b;
			a = -a;
		}
		NativeInt quot = (NativeInt) (a/b);
		NativeInt rem = a  % b;
		/* C standard says that the remainder has the sign of the dividend(!), mathematics says the remainder is always positive. */
		if(rem < 0) {
			rem += b;
			--quot; /* FIXME out of range */
		}
		/*if(a->value < 0)
			rem = -rem;*/
		/* FIXME positive remainder */
		return pair(internNativeInt(quot), internNativeInt(rem));
	}
	abort();
	return aP;
}
END_NAMESPACE_6D(Values)
BEGIN_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Values)
bool toNativeInt(NodeT aP, NativeInt* result) {
	if(intP(aP)) {
		const struct Int* a = (const struct Int*) getCXXInstance(aP);
		*result = (NativeInt) a->value;
		return true;
	} else
		return false;
}
bool toNearestNativeInt(NodeT node, NativeInt* result) {
	if(intP(node))
		return toNativeInt(node, result);
	while(integerP(node)) {
		const struct Integer* a = (const struct Integer*) getCXXInstance(node);
		node = a->tail;
	}
	if(intP(node)) {
		const struct Int* a = (const struct Int*) getCXXInstance(node);
		NativeUInt result2 = 1ULL << HIGHBIT;
		if(a->value&HIGHBIT) { /* negative */
			*result = (NativeInt) result2;
		} else {
			--result2;
			*result = (NativeInt) result2;
		}
		return true;
	}
	return false;
}
NodeT internNativeInt(NativeInt value) {
	return intA((NativeUInt) value);
}
NodeT internNativeUInt(NativeUInt value) {
	if(value&HIGHBIT) {
		NativeUInt bvalue = value&~HIGHBIT;
		NodeT v = intA(bvalue);
		return integerAddU(v, HIGHBIT);
	} else
		return intA(value);
}
NodeT integerDivrem(NODET aP, NODET bP) { /* return pair */
	if(intP(bP)) {
		const struct Int* b = (const struct Int*) getCXXInstance(bP);
		return integerDivremU(aP, b->value);
	} else
		abort();
	return aP;
}
NodeT integerDiv(NODET aP, NODET bP) {
	NodeT result = integerDivrem(aP, bP);
	if(errorP(result))
		return result;
	return getPairFst(result);
}
END_NAMESPACE_6D(FFIs)
