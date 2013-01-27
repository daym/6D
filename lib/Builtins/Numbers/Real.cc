#include <string.h>
#include <limits>
#include "Numbers/Real"
#include "6D/FFIs"
#include "Numbers/Integer"
#include "Values/Values"

namespace FFIs {
Values::NodeT internNative(NativeFloat value) {
	return(new Numbers::Float(value));
}
}
namespace Numbers {
using namespace FFIs;
using namespace Values;
REGISTER_STR(Float, {
	std::stringstream sst;
	sst.precision(std::numeric_limits<NativeFloat>::digits10 + 1);
	sst << node->value;
	std::string v = sst.str();
	const char* vc = v.c_str();
	if(*vc == '-')
		++vc;
	if(strpbrk(vc, ".eE") == NULL && (vc[0] >= '0' && vc[0] <= '9')) { /* not a special value like inf or nan */
		sst << ".0";
		v = sst.str();
	}
	fprintf(destination, "%s", v.c_str());
})

bool floatP(NodeT node) {
	return(dynamic_cast<const Float*>(node) != NULL);
}
DEFINE_STRICT_FN(FloatP, floatP(argument))

NodeT operator+(const Float& a, const Float& b) {
	return(internNative(a.value + b.value)); /* FIXME */
}
NodeT operator-(const Float& a, const Float& b) {
	return(internNative(a.value - b.value)); /* FIXME */
}
NodeT operator*(const Float& a, const Float& b) {
	return(internNative(a.value*b.value)); /* FIXME */
}
NodeT operator/(const Float& a, const Float& b) {
	return(internNative(a.value/b.value)); /* FIXME */
}
NodeT operator<=(const Float& a, const Float& b) {
	return(internNative(a.value <= b.value));
}

REGISTER_BUILTIN(FloatP, 1, 0, symbolFromStr("float?"))

bool toNativeFloat(NodeT node, NativeFloat& result) {
	const Float* floatNode;
	result = 0.0;
	//node = evaluate(node);
	if(node == NULL)
		return(false);
	else if((floatNode = dynamic_cast<const Float*>(node)) != NULL) {
		result = floatNode->value;
		return(true);
	} else {
		// only coerce integers to float if there is no information loss
		NativeInt value = 0;
		if(!FFIs::toNativeInt(node, value))
			return(false);
		result = (NativeFloat) value;
		return((NativeInt) result == value);
	}
}

#ifdef __GNUC__
#define LLVM_INF           __builtin_inff()
static Float nanFloat(__builtin_nanf(""));
static Float infinityFloat(__builtin_inff());
#else
static Float nanFloat(0.0/0.0);
static Float infinityFloat(1.0/0.0);
#endif

NodeT nan(void) {
	return(&nanFloat);
}
NodeT infinity(void) {
	return(&infinityFloat);
}

}; /* end namespace Numbers */
