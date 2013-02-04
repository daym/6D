#include <limits.h>
#include <sys/types.h>
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
#define ERRF(typ) \
	return nanxx();
#define IMPLEMENT_NATIVE_INT_GETTER(typ) \
bool typ##FromNode(NodeT root, typ* result) { \
	NativeInt result2 = 0; \
	*result = 0; \
	if(!toNativeInt(root, &result2) || (*result = result2) != result2) \
		return false; \
	return(true); \
}
// TODO support Ratio - at least for the floats, maybe.
#define IMPLEMENT_NATIVE_FLOAT_GETTER(typ) \
bool typ##FromNode(NodeT root, typ* result) { \
	NativeFloat result2 = 0.0; \
	if(ratioP(root)) \
		/* FIXME root = Evaluators::divideA(getRatioA(root), getRatioB(root), NULL)*/; \
	if(!toNativeFloat(root, &result2) || (*result = result2) != result2) \
		return false; \
	return(true); \
}
int nearestIntFromNode(NodeT root) {
	NativeInt result2 = 0;
	int result = 0;
	if(!toNearestNativeInt(root, &result2))
		return 0;
	if((result = result2) != result2) { /* doesn't fit */
		return (result2 < 0) ? INT_MIN : INT_MAX;
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

bool sizedIntFromNode(int bitCount, NodeT root, long long* result) {
	if(!longLongFromNode(root, result))
		return false;
	if((*result) & ~((1 << bitCount) - 1))
		return false;
	return(true);
}
bool pointerFromNode(NodeT root, void** result) {
	if(boxP(root)) {
		*result = getBoxValue(root);
		return true;
	} else 
		return false;
}
bool stringFromNode(NodeT root, char** result) {
	if(strP(root)) {
		*result = getStrValue(root);
		return true;
	} else if(boxP(root)) {
		*result = (char*) getBoxValue(root);
		return true;
	} else
		return false;
}
bool stringOrNilFromNode(NodeT root, char** result) {
	if(nilP(root)) {
		*result = NULL;
		return true;
	} else
		return stringFromNode(root, result);
}
bool stringSizeFromNode(NodeT root, size_t* result) {
	if(strP(root)) {
		*result = getStrSize(root);
		return true;
	} else
		return false;
}
NODET internNativeSize_t(size_t value) {
	return internNativeUInt(value); /* FIXME more general, bigger version? */
}
END_NAMESPACE_6D(FFIs)
