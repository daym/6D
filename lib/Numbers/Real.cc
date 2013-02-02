#include <string.h>
#include <limits>
#include "Numbers/Real"
#include "6D/FFIs"
#include "Numbers/Integer"
#include "Values/Values"

namespace FFIs {
using namespace Values;
Values::NodeT internNative(NativeFloat value) {
	return(new Values::Float(value));
}
bool toNativeFloat(NodeT node, NativeFloat& result) {
	result = 0.0;
	//node = evaluate(node);
	if(node == NULL)
		return(false);
	else if(floatP(node)) {
		const Float* floatNode = (const Float*) getCXXInstance(node);
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
}
namespace Values {
using namespace FFIs;
using namespace Values;
bool floatP(Values::NodeT node) {
	return tagOfNode(node) == TAG_FLOAT;
}
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


#ifdef __GNUC__
#define LLVM_INF           __builtin_inff()
static Float nanFloat(__builtin_nanf(""));
static Float infinityFloat(__builtin_inff());
#else
#ifdef _MSC_VER
static inline double nanxx(void) {
	uint32_t nan[2]={0xffffffff, 0x7fffffff};
	double g = *( double* )nan;
	return g;
}
static inline double infxx(void) {
	uint32_t inf[2]={0x7FF00000, 0};
	double g = *( double* )inf;
	return g;
}
static Float nanFloat(nanxx());
static Float infinityFloat(infxx());
#else
static Float nanFloat(0.0/0.0);
static Float infinityFloat(1.0/0.0);
#endif
#endif

NodeT nan(void) {
	return(&nanFloat);
}
NodeT infinity(void) {
	return(&infinityFloat);
}

}; /* end namespace Values */
