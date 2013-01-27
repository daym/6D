#include <algorithm>
#include <limits>
#include "6D/Operations"
#include "Numbers/Integer"
#include "Values/Values"
#include "6D/FFIs"

namespace Numbers {
using namespace Values;
using namespace FFIs;

void Integer::operator =(const Integer &x) {
	// Calls like a = a have no effect
	if (this == &x)
		return;
	// Copy sign
	sign = x.sign;
	// Copy the rest
	mag = x.mag;
}

#if 0
Integer::Integer(const Blk *b, Index blen, Sign s) : mag(b, blen) {
	switch (s) {
	case zero:
		if (!mag.isZero())
			throw EvaluationException("Integer::Integer(const Blk *, Index, Sign): Cannot use a sign of zero with a nonzero magnitude");
		sign = zero;
		break;
	case positive:
	case negative:
		// If the magnitude is zero, force the sign to zero.
		sign = mag.isZero() ? zero : s;
		break;
	default:
		/* g++ seems to be optimizing out this case on the assumption
		 * that the sign is a valid member of the enumeration.  Oh well. */
		throw EvaluationException("Integer::Integer(const Blk *, Index, Sign): Invalid sign");
	}
}
#endif

Integer::Integer(const BigUnsigned &x, Sign s) : mag(x) {
	switch (s) {
	case zero:
		if (!mag.isZero())
			throw std::logic_error("Integer::Integer(const BigUnsigned &, Sign): Cannot use a sign of zero with a nonzero magnitude");
		sign = zero;
		break;
	case positive:
	case negative:
		// If the magnitude is zero, force the sign to zero.
		sign = mag.isZero() ? zero : s;
		break;
	default:
		/* g++ seems to be optimizing out this case on the assumption
		 * that the sign is a valid member of the enumeration.  Oh well. */
		throw std::logic_error("Integer::Integer(const BigUnsigned &, Sign): Invalid sign");
	}
}

/* CONSTRUCTION FROM PRIMITIVE INTEGERS
 * Same idea as in BigUnsigned.cc, except that negative input results in a
 * negative Integer instead of an exception. */

// Done longhand to let us use initialization.
Integer::Integer(unsigned long  x) : mag(x) { sign = mag.isZero() ? zero : positive; }
Integer::Integer(unsigned int   x) : mag(x) { sign = mag.isZero() ? zero : positive; }
Integer::Integer(unsigned short x) : mag(x) { sign = mag.isZero() ? zero : positive; }
Integer::Integer(unsigned long long x) : mag(x) { sign = mag.isZero() ? zero : positive; }

// For signed input, determine the desired magnitude and sign separately.

namespace {
	template <class X, class UX>
	Integer::Blk magOf(X x) {
		/* UX(...) cast needed to stop short(-2^15), which negates to
		 * itself, from sign-extending in the conversion to Blk. */
		return Integer::Blk(x < 0 ? UX(-x) : x);
	}
	template <class X>
	Integer::Sign signOf(X x) {
		return (x == 0) ? Integer::zero
			: (x > 0) ? Integer::positive
			: Integer::negative;
	}
}

Integer::Integer(long  x) : sign(signOf(x)), mag(magOf<long , unsigned long >(x)) {}
Integer::Integer(int   x) : sign(signOf(x)), mag(magOf<int  , unsigned int  >(x)) {}
Integer::Integer(short x) : sign(signOf(x)), mag(magOf<short, unsigned short>(x)) {}
Integer::Integer(long long x) : sign(signOf(x)), mag(magOf<long long  , unsigned long long >(x)) {}

// CONVERSION TO PRIMITIVE INTEGERS

/* Reuse BigUnsigned's conversion to an unsigned primitive integer.
 * The friend is a separate function rather than
 * Integer::convertToUnsignedPrimitive to avoid requiring BigUnsigned to
 * declare Integer. */
template <class X>
inline X convertBigUnsignedToPrimitiveAccess(const BigUnsigned &a) {
	return a.convertToPrimitive<X>();
}

template <class X>
X Integer::convertToUnsignedPrimitive() const {
	if (sign == negative)
		throw std::logic_error("Integer::to<Primitive>: Cannot convert a negative integer to an unsigned type");
	else
		return mag.convertToPrimitive<X>(); // convertBigUnsignedToPrimitiveAccess<X>(mag);
}

/* Similar to BigUnsigned::convertToPrimitive, but split into two cases for
 * nonnegative and negative numbers. */
template <class X, class UX>
X Integer::convertToSignedPrimitive() const {
	if (sign == zero)
		return 0;
	else if (mag.size() == 1) {
		// The single block might fit in an X.  Try the conversion.
		Blk b = mag.getBlock(0);
		if (sign == positive) {
			X x = X(b);
			if (x >= 0 && Blk(x) == b)
				return x;
		} else {
			X x = -X(b);
			/* UX(...) needed to avoid rejecting conversion of
			 * -2^15 to a short. */
			if (x < 0 && Blk(UX(-x)) == b)
				return x;
		}
		// Otherwise fall through.
	}
	throw std::range_error("Integer::to<Primitive>: Value is too big to fit in the requested type");
}

NativeInt Integer::toNativeInt() const {
	return convertToSignedPrimitive<NativeInt, NativeUInt>();
}
NativeUInt Integer::toNativeUInt() const {
	return convertToUnsignedPrimitive<NativeUInt>();
}

//unsigned long  Integer::toUnsignedLong () const { return convertToUnsignedPrimitive<unsigned long >       (); }
//unsigned int   Integer::toUnsignedInt  () const { return convertToUnsignedPrimitive<unsigned int  >       (); }
//unsigned short Integer::toUnsignedShort() const { return convertToUnsignedPrimitive<unsigned short>       (); }
//long           Integer::toLong         () const { return convertToSignedPrimitive  <long , unsigned long> (); }
//int            Integer::toInt          () const { return convertToSignedPrimitive  <int  , unsigned int>  (); }
//short          Integer::toShort        () const { return convertToSignedPrimitive  <short, unsigned short>(); }

// COMPARISON
Integer::CmpRes Integer::compareTo(const Integer &x) const {
	// A greater sign implies a greater number
	if (sign < x.sign)
		return less;
	else if (sign > x.sign)
		return greater;
	else switch (sign) {
		// If the signs are the same...
	case zero:
		return equal; // Two zeros are equal
	case positive:
		// Compare the magnitudes
		return mag.compareTo(x.mag);
	case negative:
		// Compare the magnitudes, but return the opposite result
		return CmpRes(-mag.compareTo(x.mag));
	default:
		throw std::logic_error("Integer internal error");
	}
}

/* COPY-LESS OPERATIONS
 * These do some messing around to determine the sign of the result,
 * then call one of BigUnsigned's copy-less operations. */

// See remarks about aliased calls in BigUnsigned.cc .
#define DTRT_ALIASED(cond, op) \
	if (cond) { \
		Integer tmpThis; \
		tmpThis.op; \
		*this = tmpThis; \
		return; \
	}

void Integer::add(const Integer &a, const Integer &b) {
	DTRT_ALIASED(this == &a || this == &b, add(a, b));
	// If one argument is zero, copy the other.
	if (a.sign == zero)
		operator =(b);
	else if (b.sign == zero)
		operator =(a);
	// If the arguments have the same sign, take the
	// common sign and add their magnitudes.
	else if (a.sign == b.sign) {
		sign = a.sign;
		mag.add(a.mag, b.mag);
	} else {
		// Otherwise, their magnitudes must be compared.
		switch (a.mag.compareTo(b.mag)) {
		case equal:
			// If their magnitudes are the same, copy zero.
			mag = 0;
			sign = zero;
			break;
			// Otherwise, take the sign of the greater, and subtract
			// the lesser magnitude from the greater magnitude.
		case greater:
			sign = a.sign;
			mag.subtract(a.mag, b.mag);
			break;
		case less:
			sign = b.sign;
			mag.subtract(b.mag, a.mag);
			break;
		}
	}
}

void Integer::subtract(const Integer &a, const Integer &b) {
	// Notice that this routine is identical to Integer::add,
	// if one replaces b.sign by its opposite.
	DTRT_ALIASED(this == &a || this == &b, subtract(a, b));
	// If a is zero, copy b and flip its sign.  If b is zero, copy a.
	if (a.sign == zero) {
		mag = b.mag;
		// Take the negative of _b_'s, sign, not ours.
		// Bug pointed out by Sam Larkin on 2005.03.30.
		sign = Sign(-b.sign);
	} else if (b.sign == zero)
		operator =(a);
	// If their signs differ, take a.sign and add the magnitudes.
	else if (a.sign != b.sign) {
		sign = a.sign;
		mag.add(a.mag, b.mag);
	} else {
		// Otherwise, their magnitudes must be compared.
		switch (a.mag.compareTo(b.mag)) {
			// If their magnitudes are the same, copy zero.
		case equal:
			mag = 0;
			sign = zero;
			break;
			// If a's magnitude is greater, take a.sign and
			// subtract a from b.
		case greater:
			sign = a.sign;
			mag.subtract(a.mag, b.mag);
			break;
			// If b's magnitude is greater, take the opposite
			// of b.sign and subtract b from a.
		case less:
			sign = Sign(-b.sign);
			mag.subtract(b.mag, a.mag);
			break;
		}
	}
}

void Integer::multiply(const Integer &a, const Integer &b) {
	DTRT_ALIASED(this == &a || this == &b, multiply(a, b));
	// If one object is zero, copy zero and return.
	if (a.sign == zero || b.sign == zero) {
		sign = zero;
		mag = 0;
		return;
	}
	// If the signs of the arguments are the same, the result
	// is positive, otherwise it is negative.
	sign = (a.sign == b.sign) ? positive : negative;
	// Multiply the magnitudes.
	mag.multiply(a.mag, b.mag);
}

/*
 * DIVISION WITH REMAINDER
 * Please read the comments before the definition of
 * `BigUnsigned::divideWithRemainder' in `BigUnsigned.cc' for lots of
 * information you should know before reading this function.
 *
 * Following Knuth, I decree that x / y is to be
 * 0 if y==0 and floor(real-number x / y) if y!=0.
 * Then x % y shall be x - y*(integer x / y).
 *
 * Note that x = y * (x / y) + (x % y) always holds.
 * In addition, (x % y) is from 0 to y - 1 if y > 0,
 * and from -(|y| - 1) to 0 if y < 0.  (x % y) = x if y = 0.
 *
 * Examples: (q = a / b, r = a % b)
 *	a	b	q	r
 *	===	===	===	===
 *	4	3	1	1
 *	-4	3	-2	2
 *	4	-3	-2	-2
 *	-4	-3	1	-1
 */
void Integer::divideWithRemainder(const Integer &b, Integer &q) {
	// Defend against aliased calls;
	// same idea as in BigUnsigned::divideWithRemainder .
	if (this == &q)
		throw std::logic_error("Integer::divideWithRemainder: Cannot write quotient and remainder into the same variable");
	if (this == &b || &q == &b) {
		Integer tmpB(b);
		divideWithRemainder(tmpB, q);
		return;
	}

	// Division by zero gives quotient 0 and remainder *this
	if (b.sign == zero) {
		q.mag = 0;
		q.sign = zero;
		return;
	}
	// 0 / b gives quotient 0 and remainder 0
	if (sign == zero) {
		q.mag = 0;
		q.sign = zero;
		return;
	}

	// Here *this != 0, b != 0.

	// Do the operands have the same sign?
	if (sign == b.sign) {
		// Yes: easy case.  Quotient is zero or positive.
		q.sign = positive;
	} else {
		// No: harder case.  Quotient is negative.
		q.sign = negative;
		// Decrease the magnitude of the dividend by one.
		mag--;
		/*
		 * We tinker with the dividend before and with the
		 * quotient and remainder after so that the result
		 * comes out right.  To see why it works, consider the following
		 * list of examples, where A is the magnitude-decreased
		 * a, Q and R are the results of BigUnsigned division
		 * with remainder on A and |b|, and q and r are the
		 * final results we want:
		 *
		 *	a	A	b	Q	R	q	r
		 *	-3	-2	3	0	2	-1	0
		 *	-4	-3	3	1	0	-2	2
		 *	-5	-4	3	1	1	-2	1
		 *	-6	-5	3	1	2	-2	0
		 *
		 * It appears that we need a total of 3 corrections:
		 * Decrease the magnitude of a to get A.  Increase the
		 * magnitude of Q to get q (and make it negative).
		 * Find r = (b - 1) - R and give it the desired sign.
		 */
	}

	// Divide the magnitudes.
	mag.divideWithRemainder(b.mag, q.mag);

	if (sign != b.sign) {
		// More for the harder case (as described):
		// Increase the magnitude of the quotient by one.
		q.mag++;
		// Modify the remainder.
		mag.subtract(b.mag, mag);
		mag--;
	}

	// Sign of the remainder is always the sign of the divisor b.
	sign = b.sign;

	// Set signs to zero as necessary.  (Thanks David Allen!)
	if (mag.isZero())
		sign = zero;
	if (q.mag.isZero())
		q.sign = zero;

	// WHEW!!!
}

// Negation
void Integer::negate(const Integer &a) {
	DTRT_ALIASED(this == &a, negate(a));
	// Copy a's magnitude
	mag = a.mag;
	// Copy the opposite of a.sign
	sign = Sign(-a.sign);
}

// INCREMENT/DECREMENT OPERATORS

// Prefix increment
void Integer::operator ++() {
	if (sign == negative) {
		mag--;
		if (mag == 0)
			sign = zero;
	} else {
		mag++;
		sign = positive; // if not already
	}
}

// Postfix increment: same as prefix
void Integer::operator ++(int) {
	operator ++();
}

// Prefix decrement
void Integer::operator --() {
	if (sign == positive) {
		mag--;
		if (mag == 0)
			sign = zero;
	} else {
		mag++;
		sign = negative;
	}
}

// Postfix decrement: same as prefix
void Integer::operator --(int) {
	operator --();
}

static const NativeInt minValue = std::numeric_limits<NativeInt>::min();
static const NativeInt maxValue = std::numeric_limits<NativeInt>::max();

NodeT operator+(const Int& a, const Int& b) {
	//std::cout << "max " << maxValue << std::endl;
	if(b.value < 0 ? minValue - b.value > a.value : maxValue - b.value < a.value) {
		Integer result = Integer(a.value) + Integer(b.value);
		return(new Integer(result));
	}
	/* TODO if we assumed addition with overflow was not undefined (and violoated C standard), we could check
	     if( ((a ^ result) & (b ^ result)) < 0)
	       OVERFLOW
	   after the fact. */
	NativeInt smallResult = a.value + b.value;
	return(internNative(smallResult));
}
NodeT operator-(const Int& a, const Int& b) {
	if(b.value < 0 ? maxValue + b.value < a.value : minValue + b.value > a.value) {
		Integer result = Integer(a.value) - Integer(b.value);
		return(new Integer(result));
	}
	NativeInt smallResult = a.value - b.value;
        return(internNative(smallResult));
}
NodeT operator*(const Int& a, const Int& b) {
	/*if(av == 0 || bv == 0)
		return(&integers[0]);*/
	if(b.value > 0 ? a.value > maxValue/b.value || a.value < minValue/b.value
	               : b.value < -1 ? a.value > minValue/b.value || a.value < maxValue/b.value
	                        : b.value == -1 && a.value == minValue) {
		Integer result = Integer(a.value) * Integer(b.value);
		return(new Integer(result));
	}
	NativeInt smallResult = a.value * b.value;
        return(internNative(smallResult));
}
/*
For multiplication of ints do:

#include <iostream>
#include <ostream>
#include <iomanip>
#include <limits>
#include <cmath>

bool multiply( int &c, int a, int b)
{
	c = a * b;
	double d = (std::log(std::abs(double(a))) + std::log(std::abs(double(b))))/log(2);
	return d < std::numeric_limits< int >::digits;
}


int main()
{
	using namespace std;
	int const M = -numeric_limits<int>::max();
	int r;

	cout << boolalpha << hex;

	cout << multiply( r, M / 4, M / 4 ) << ": " << r << endl;
	cout << multiply( r, 10, 4 ) << ": " << r << endl;
	cout << multiply( r, (M/ + 1, 2 ) << ": " << r << endl;
	cout << multiply( r, (M/2) + 1, 4 ) << ": " << r << endl;
}

You can do that without the double-precision math using bit scanning, as int( std::log(1 << N)/std::log(2)) == N.
*/

bool operator<=(const Int& a, const Int& b) {
        return(a.value <= b.value);
}
/*NodeT operator<=(const Integer& a, const Integer& b) {
        return(Evaluators::internNative(a.compareTo(b) != Integer::greater));
}*/
static Integer xinteger1(1);
static inline NodeT intASucc(NodeT argument) {
	const Int* int1 = dynamic_cast<const Int*>(argument);
	if(int1) {
		NativeInt value = int1->value;
		if(value + 1 < value) /* overflow */
			return new Integer(Integer(value) + xinteger1);
		return(internNative(value + 1));
	} else
		throw std::invalid_argument("intASucc invalid argument");
}
DEFINE_STRICT_FN(IntSucc, intASucc(argument))
static inline NodeT integerASucc(NodeT argument) {
	const Integer* integer1 = dynamic_cast<const Integer*>(argument);
	if(integer1) {
		return(new Integer((*integer1) + xinteger1));
	}
	const Int* int1 = dynamic_cast<const Int*>(argument);
	if(int1) {
		NativeInt value = int1->value;
		if(value + 1 < value) /* overflow */
			return new Integer(Integer(value) + xinteger1);
		return(internNative(value + 1));
	} else
		throw std::invalid_argument("integerASucc invalid argument");
}
DEFINE_STRICT_FN(IntegerSucc, integerASucc(argument))
REGISTER_STR(Int, {
        std::stringstream sst;
	if(node->value < 0)
		sst << '(' << node->value << ')';
	else
	        sst << node->value;
        return(sst.str());
})
static std::string strInteger(Integer* node) {
	Integer& v = *node;
	std::stringstream sst;
	BigUnsigned q(v.getMagnitude());
	BigUnsigned divisor(10);
	BigUnsigned r;
	if (v.getSign() == Integer::negative)
		sst << ')';
	for(int i = 0; i < 10000; ++i) {
		if(q.isZero())
			break;
		r = q;
		r.divideWithRemainder(divisor, q);
		NativeInt rf = r.convertToSignedPrimitive<NativeInt>();
		sst << rf;
	}
	if(sst.str().empty())
		sst << '0';
	if (v.getSign() == Integer::negative)
		sst << "-(";
	std::string result = sst.str();
	std::reverse(result.begin(), result.end());
	return(result);
}
REGISTER_STR(Integer, {
	return(strInteger(node));
})

DEFINE_STRICT_FN(IntP, (dynamic_cast<const Int*>(argument) != NULL))
DEFINE_STRICT_FN(IntegerP, (dynamic_cast<const Int*>(argument) != NULL||dynamic_cast<const Integer*>(argument) != NULL))

REGISTER_BUILTIN(IntP, 1, 0, symbolFromStr("int?"))
REGISTER_BUILTIN(IntegerP, 1, 0, symbolFromStr("integer?"))
REGISTER_BUILTIN(IntSucc, 1, 0, symbolFromStr("intSucc"))
REGISTER_BUILTIN(IntegerSucc, 1, 0, symbolFromStr("integerSucc"))

}; /* namespace Numbers */
namespace FFIs {
using namespace Numbers;
static Int integers[256] = {
        Int(0),
        Int(1),
        Int(2),
        Int(3),
        Int(4),
        Int(5),
        Int(6),
        Int(7),
        Int(8),
        Int(9),
        Int(10),
        Int(11),
        Int(12),
        Int(13),
        Int(14),
        Int(15),
        Int(16),
        Int(17),
        Int(18),
        Int(19),
        Int(20),
        Int(21),
        Int(22),
        Int(23),
        Int(24),
        Int(25),
        Int(26),
        Int(27),
        Int(28),
        Int(29),
        Int(30),
        Int(31),
        Int(32),
        Int(33),
        Int(34),
        Int(35),
        Int(36),
        Int(37),
        Int(38),
        Int(39),
        Int(40),
        Int(41),
        Int(42),
        Int(43),
        Int(44),
        Int(45),
        Int(46),
        Int(47),
        Int(48),
        Int(49),
        Int(50),
        Int(51),
        Int(52),
        Int(53),
        Int(54),
        Int(55),
        Int(56),
        Int(57),
        Int(58),
        Int(59),
        Int(60),
        Int(61),
        Int(62),
        Int(63),
        Int(64),
        Int(65),
        Int(66),
        Int(67),
        Int(68),
        Int(69),
        Int(70),
        Int(71),
        Int(72),
        Int(73),
        Int(74),
        Int(75),
        Int(76),
        Int(77),
        Int(78),
        Int(79),
        Int(80),
        Int(81),
        Int(82),
        Int(83),
        Int(84),
        Int(85),
        Int(86),
        Int(87),
        Int(88),
        Int(89),
        Int(90),
        Int(91),
        Int(92),
        Int(93),
        Int(94),
        Int(95),
        Int(96),
        Int(97),
        Int(98),
        Int(99),
        Int(100),
        Int(101),
        Int(102),
        Int(103),
        Int(104),
        Int(105),
        Int(106),
        Int(107),
        Int(108),
        Int(109),
        Int(110),
        Int(111),
        Int(112),
        Int(113),
        Int(114),
        Int(115),
        Int(116),
        Int(117),
        Int(118),
        Int(119),
        Int(120),
        Int(121),
        Int(122),
        Int(123),
        Int(124),
        Int(125),
        Int(126),
        Int(127),
        Int(128),
        Int(129),
        Int(130),
        Int(131),
        Int(132),
        Int(133),
        Int(134),
        Int(135),
        Int(136),
        Int(137),
        Int(138),
        Int(139),
        Int(140),
        Int(141),
        Int(142),
        Int(143),
        Int(144),
        Int(145),
        Int(146),
        Int(147),
        Int(148),
        Int(149),
        Int(150),
        Int(151),
        Int(152),
        Int(153),
        Int(154),
        Int(155),
        Int(156),
        Int(157),
        Int(158),
        Int(159),
        Int(160),
        Int(161),
        Int(162),
        Int(163),
        Int(164),
        Int(165),
        Int(166),
        Int(167),
        Int(168),
        Int(169),
        Int(170),
        Int(171),
        Int(172),
        Int(173),
        Int(174),
        Int(175),
        Int(176),
        Int(177),
        Int(178),
        Int(179),
        Int(180),
        Int(181),
        Int(182),
        Int(183),
        Int(184),
        Int(185),
        Int(186),
        Int(187),
        Int(188),
        Int(189),
        Int(190),
        Int(191),
        Int(192),
        Int(193),
        Int(194),
        Int(195),
        Int(196),
        Int(197),
        Int(198),
        Int(199),
        Int(200),
        Int(201),
        Int(202),
        Int(203),
        Int(204),
        Int(205),
        Int(206),
        Int(207),
        Int(208),
        Int(209),
        Int(210),
        Int(211),
        Int(212),
        Int(213),
        Int(214),
        Int(215),
        Int(216),
        Int(217),
        Int(218),
        Int(219),
        Int(220),
        Int(221),
        Int(222),
        Int(223),
        Int(224),
        Int(225),
        Int(226),
        Int(227),
        Int(228),
        Int(229),
        Int(230),
        Int(231),
        Int(232),
        Int(233),
        Int(234),
        Int(235),
        Int(236),
        Int(237),
        Int(238),
        Int(239),
        Int(240),
        Int(241),
        Int(242),
        Int(243),
        Int(244),
        Int(245),
        Int(246),
        Int(247),
        Int(248),
        Int(249),
        Int(250),
        Int(251),
        Int(252),
        Int(253),
        Int(254),
        Int(255),
}; /* TODO ptr */
NodeT internNative(NativeInt value) {
        if(value >= 0 && value < 256)
                return(&integers[value]);
        return(new Int(value));
}

bool toNativeInt(NodeT node, NativeInt& result) {
	const Int* intNode;
	const Integer* integerNode;
	result = 0;
	if(node == NULL)
		return(false);
	else if((intNode = dynamic_cast<const Int*>(node)) != NULL) {
		result = intNode->value;
		return(true);
	} else if((integerNode = dynamic_cast<const Integer*>(node)) != NULL) {
		try {
			result = integerNode->toNativeInt();
		} catch(std::exception& exception) { /* too big etc */
			return(false);
		}
		return(true);
	} else
		return(false);
}
bool toNearestNativeInt(NodeT node, NativeInt& result) {
	const Int* intNode;
	const Integer* integerNode;
	result = 0;
	if(node == NULL)
		return(false);
	else if((intNode = dynamic_cast<const Int*>(node)) != NULL) {
		result = intNode->value;
		return(true);
	} else if((integerNode = dynamic_cast<const Integer*>(node)) != NULL) {
		try {
			result = integerNode->toNativeInt();
		} catch(std::exception& exception) { /* too big etc */
			switch(integerNode->getSign()) {
			case Integer::negative:
				result = std::numeric_limits<NativeInt>::min();
				return(true);
			case Integer::zero:
				result = 0;
				return(true);
			case Integer::positive:
				result = std::numeric_limits<NativeInt>::max();
				return(true);
			default:
				return(false);
			}
		}
		return(true);
	} else
		return(false);
}
NodeT internUnsignedLongLong(unsigned long long value, bool B_negative) {
	Integer result(0);
	for(; value != 0; value <<= 1) {
		result *= 2;
		if(value & ((unsigned long long) std::numeric_limits<long long>::max() + 1))
			result += 1;
	}
	return new Integer(result);
}
NodeT internNativeU(NativeUInt value) {
        if(value >= 0 && value < 256)
                return(&integers[value]);
	return (value < 0) ? internUnsignedLongLong((unsigned long long) (-(long long) /* FIXME out of range */ value), true) : internUnsignedLongLong((unsigned long long) value, false);
}
#ifdef INTERN_NATIVE_NEED_LONG_LONG
NodeT internNative(long long value) {
	return (value < 0) ? internUnsignedLongLong((unsigned long long) (-value), true) : internUnsignedLongLong((unsigned long long) value, false);
}
NodeT internNativeU(unsigned long long value) {
	return internUnsignedLongLong(value, false);
}
#endif

};
