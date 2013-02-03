#include <limits>
#include <stdexcept>
#include "6D/Values"
#include "6D/FFIs"
#include "Numbers/Integer2"
#include "Numbers/Real"
#include "Numbers/Ratio"
#include "Logic/Logic"
BEGIN_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Values)
typedef long long longLong;
typedef long double longDouble;
#define IMPLEMENT_NATIVE_INT_GETTER(typ) \
typ typ##FromNode(NodeT root) { \
	NativeInt result2 = 0; \
	typ result = 0; \
	if(!FFIs::toNativeInt(root, result2) || (result = result2) != result2) \
		throw std::range_error("value out of range for " #typ); \
	return(result); \
}
// TODO support Ratio - at least for the floats, maybe.
#define IMPLEMENT_NATIVE_FLOAT_GETTER(typ) \
typ typ##FromNode(NodeT root) { \
	NativeFloat result2 = 0.0; \
	typ result = 0; \
	if(ratioP(root)) \
		/* FIXME root = Evaluators::divideA(getRatioA(root), getRatioB(root), NULL)*/; \
	if(!FFIs::toNativeFloat(root, result2) || (result = result2) != result2) \
		throw std::range_error("value out of range for " #typ); \
	return(result); \
}
int nearestIntFromNode(NodeT root) {
	NativeInt result2 = 0;
	int result = 0;
	if(!FFIs::toNearestNativeInt(root, result2))
		throw std::range_error("cannot convert to int");
	if((result = result2) != result2) { /* doesn't fit */
		return (result2 < 0) ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
	}
	return(result);
}
IMPLEMENT_NATIVE_INT_GETTER(int)
IMPLEMENT_NATIVE_INT_GETTER(long)
IMPLEMENT_NATIVE_INT_GETTER(longLong)
IMPLEMENT_NATIVE_INT_GETTER(short)
IMPLEMENT_NATIVE_FLOAT_GETTER(float)
IMPLEMENT_NATIVE_FLOAT_GETTER(double)
IMPLEMENT_NATIVE_FLOAT_GETTER(longDouble)

long long sizedIntFromNode(int bitCount, NodeT root) {
	long long result = longLongFromNode(root);
	if(result & ~((1 << bitCount) - 1)) {
		std::stringstream sst;
		sst << "value out of range for " << "bits";
		throw std::range_error(GCx_strdup(sst.str().c_str()));
	}
	return(result);
}
void* pointerFromNode(const NodeT root) {
	if(boxP(root))
		return getBoxValue(root);
	else
		throw std::logic_error("not a Box");
}
char* stringFromNode(NodeT root) {
	if(strP(root))
		return getStrValue(root);
	else if(boxP(root))
		return (char*) getBoxValue(root);
	else
		throw std::range_error("not a Str");
}
char* stringOrNilFromNode(NodeT root) {
	if(root)
		return(stringFromNode(root));
	else
		return(NULL);
}
size_t stringSizeFromNode(NodeT root) {
	if(strP(root))
		return getStrSize(root);
	else
		throw std::logic_error("not a Str");
}

END_NAMESPACE_6D(FFIs)
