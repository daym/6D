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
	i = -1;
	assert((unsigned int) i == ~0); /* two's complement */
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
#define HIGHBIT (NATIVEUINT_ONE<<(NATIVEINT_BITCOUNT - 1))
#define ALL1 (~NATIVEUINT_ZERO)
#define SIGNEXTENSION(value) ((value&HIGHBIT)*ALL1)
#define LOAD_CHUNK1(a) \
	if(intP(a##V)) { \
		const struct Int* a = (const struct Int*) getCXXInstance(a##V); \
		a##value = a->value; \
		a##tail = nil; \
	} else if(integerP(a##V)) { \
		const struct Integer* a = (const struct Integer*) getCXXInstance(a##V); \
		a##value = a->value; \
		a##tail = a->tail; \
		assert(a##tail); \
	}
#define LOAD_CHUNK(a) \
	LOAD_CHUNK1(a) else \
		return evalError(strC("<integer>"), strC("<junk>"), a##V);
static INLINE bool negativeP(NativeUInt amount) {
	return (amount&HIGHBIT) != 0;
}
static bool negativeIntegerP(NodeT aV) {
	while(integerP(aV)) {
		const struct Integer* a = (const struct Integer*) getCXXInstance(aV);
		aV = a->tail;
	}
	assert(intP(aV));
	const struct Integer* a = (const struct Integer*) getCXXInstance(aV);
	return negativeP(a->value);
}
/* only suitable for adding "positive" amounts - the negative amounts would have to be sign-extended too much. */
NodeT integerAddU(NodeT aV, NativeUInt amount) {
	NativeUInt avalue;
	NodeT atail;
	if(amount == 0U)
		return aV;
	assert(!negativeP(amount));
	LOAD_CHUNK(a)
	NativeUInt value2 = avalue + amount;
	/* can we overflow so much it goes around all the way? Not with the precondition above. */
	if(UNLIKELY_6D(!atail && negativeP(value2) && !negativeP(avalue)/* precond && !negativeP(amount)*/)) { /* we flipped sign accidentally (middle) */
		/* this can only happen with the MSB of the entire number.  */
		assert(value2 >= avalue); /* Note that overflow cannot happen anyway because the sign switch was in the "middle" of the range. */
		NodeT newMS = intA(SIGNEXTENSION(avalue)); /* sign extend */
		return integerpart(value2, newMS);
	}
	return atail ? integerpart(value2, (value2 < avalue /*overflow*/) ? integerAddU(atail, 1) : atail) : intA(value2);
}
/* only suitable for adding "negative" amounts */
NodeT integerAddN(NodeT aV, NativeUInt amount) {
	NativeUInt avalue;
	NodeT atail;
	if(amount == 0U)
		return aV;
	assert(negativeP(amount));
	LOAD_CHUNK(a)
	NativeUInt value2 = avalue + amount;
	if(UNLIKELY_6D(!atail && !negativeP(value2) && negativeP(avalue) /*&& precond amount&HIGHBIT */)) { /* flipped sign accidentally */
		/* this can only happen with the MSB of the entire number */
		assert(value2 <= avalue); /* Note that overflow cannot happen anyway because the sign switch was in the "middle" of the range. */
		NodeT newMS = intA(SIGNEXTENSION(avalue)); /* sign extend */
		return integerpart(value2, newMS);
	}
	return atail ? integerpart(value2, integerAddN(integerAddN(atail, (value2 < avalue) ? 1 : 0), SIGNEXTENSION(avalue))) : intA(value2);
}
NodeT integerAdd(NodeT aV, NodeT bV) {
	NativeUInt avalue;
	NodeT atail;
	NativeUInt bvalue;
	NodeT btail;
	LOAD_CHUNK(a)
	LOAD_CHUNK(b)
	if(atail && !btail)
		btail = refCXXInstance(&zerozerozerozero);
	if(btail && !atail)
		atail = refCXXInstance(&zerozerozerozero);
	// here, either both tails are nil or both tails are not nil.
	NativeUInt value2 = avalue + bvalue;
	if(UNLIKELY_6D(!atail && 
	   ((negativeP(value2) && !negativeP(avalue) && !negativeP(bvalue)) || 
	   ((!negativeP(value2) && negativeP(avalue) && negativeP(bvalue)))))) { /* flipped sign accidentally */
		assert(value2 >= avalue); /* Note that overflow cannot happen anyway because the sign switch was in the "middle" of the range. */
		NodeT newMS = intA(SIGNEXTENSION(avalue)); /* sign extend */
		return integerpart(value2, newMS);
	}
	return atail ? integerpart(value2, integerAddU(integerAdd(atail, btail), (value2 < avalue) ? 1 : 0)) : intA(value2);
}
NODET integerSubU(NODET aV, NativeUInt amount) {
	NativeInt amv;
	if(amount == 0U)
		return aV;
	assert(!negativeP(amount));
	amv = -((NativeInt) amount);
	amount = (NativeUInt) amv;
	assert(negativeP(amount));
	return integerAddN(aV, amount);
}
NODET integerSub(NODET aP, NODET bP) {
	NativeInt b;
	if(toNativeInt(bP, &b)) {
		assert(b >= 0);
		return integerSubU(aP, b);
	} else {
		abort();
		return nil;
	}
}
NodeT integerSucc(NodeT aP) {
	return integerAddU(aP, 1);
}
NodeT integerMulU(NodeT aP, NativeInt b) {
	return integerMul(aP, intA(b));
}
/* TODO do more error handling! */
NodeT integerMul(NodeT aV, NodeT bV) {
	/* TODO prefer the shorter one */
	NodeT result = intA(0);
	NativeUInt bvalue;
	NodeT btail;
	LOAD_CHUNK(b)
	{
		NativeUInt mask;
		for(mask = HIGHBIT; mask; mask >>= 1) {
			result = integerShl(result, 1);
			if(bvalue&mask)
				result = integerAdd(result, aV);
		}
	}
	return integerAdd(result, integerShl(integerMul(aV, btail), NATIVEINT_BITCOUNT));
}
NodeT integerShl(NodeT aV, unsigned amount) {
	if(amount == 0)
		return aV;
	int remainderCount = NATIVEINT_BITCOUNT - amount;
	NativeUInt mask = (~NATIVEUINT_ZERO << remainderCount); /* TODO &maskall */
	NativeUInt c = 0;
	NativeUInt ct;
	NativeUInt avalue;
	NodeT atail;
	while(true) {
		LOAD_CHUNK(a)
		ct = (avalue&mask)>>remainderCount;
		avalue = (avalue << amount) | c;
		c = ct;
		aV = atail;
	}
	abort();
	return aV;
}
NodeT integerShr(NodeT aV, unsigned amount) {
	if(amount == 0)
		return aV;
	if(amount >= NATIVEINT_BITCOUNT)
		return intA(0);
	/*int remainderCount = NATIVEINT_BITCOUNT - amount;*/
	/*NativeUInt mask = (~NATIVEUINT_ZERO >> remainderCount);*/
	NativeUInt avalue;
	NodeT atail;
	while(true) {
		LOAD_CHUNK(a)
		avalue = (avalue >> amount) | 0/*FIXME ((atailvalue or 0)&m)<<remainderCount  */;
		aV = atail;
	}
	abort();
	return aV;
}
bool integerEqualsP(NodeT aV, NodeT bV) {
	NativeUInt avalue;
	NodeT atail;
	NativeUInt bvalue;
	NodeT btail;
	if(aV == bV)
		return true;
	if(aV && !bV)
		return false;
	if(bV && !aV)
		return false;
	LOAD_CHUNK1(a) else return false;
	LOAD_CHUNK1(b) else return false;
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
NativeInt integerCompare(NODET aV, NODET bV) {
	NodeT r = integerSub(aV, bV);
	return (r == intA(0)) ? 0 :
	       negativeIntegerP(r) ? (-1) : 
	       1;
}
NativeInt integerCompareU(NODET aP, NativeInt b) {
	NativeInt a;
	if(toNativeInt(aP, &a)) {
		/* FIXME make sure not to drop outside of the range */
		return a - b;
	} else
		abort();
}
NodeT integerDivremU(NODET aP, NativeInt b) {
	NativeInt a;
	if(toNativeInt(aP, &a)) {
		if(b == 0)
			return evalError(strC("<nonzero-divisor>"), strC("0"), aP);
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
		NativeUInt result2 = HIGHBIT;
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
	return pairFst(result);
}
NODET integerPow(NODET aP, NODET bP) {
	NativeInt b;
	if(toNativeInt(bP, &b)) {
		assert(b >= 0);
		return integerPowU(aP, b);
	} else
		abort();
}
NODET integerPowU(NODET aP, NATIVEUINT b) {
	if(b == 0)
		return intA(1);
	return integerPowU(integerMulU(aP, b), b);
}
END_NAMESPACE_6D(FFIs)
