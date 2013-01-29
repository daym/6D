#include "6D/Values"
#include "6D/FFIs"
namespace FFIs {
typedef long long longLong;
typedef long double longDouble;
#define IMPLEMENT_NATIVE_INT_GETTER(typ) \
typ typ##FromNode(NodeT root) { \
	NativeInt result2 = 0; \
	typ result = 0; \
	if(!Numbers::toNativeInt(root, result2) || (result = result2) != result2) \
		throw Evaluators::EvaluationException("value out of range for " #typ); \
	return(result); \
}
// TODO support Ratio - at least for the floats, maybe.
#define IMPLEMENT_NATIVE_FLOAT_GETTER(typ) \
typ typ##FromNode(NodeT root) { \
	NativeFloat result2 = 0.0; \
	typ result = 0; \
	if(Numbers::ratio_P(root)) \
		root = Evaluators::divideA(Ratio_getA(root), Ratio_getB(root), NULL); \
	if(!Numbers::toNativeFloat(root, result2) || (result = result2) != result2) \
		throw Evaluators::EvaluationException("value out of range for " #typ); \
	return(result); \
}
int nearestIntFromNode(NodeT root) {
	NativeInt result2 = 0;
	int result = 0;
	if(!Numbers::toNearestNativeInt(root, result2))
		throw Evaluators::EvaluationException("cannot convert to int");
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

long long sizedIntFromNode(int bitCount, NodeT root) {
	long long result = longLongFromNode(root);
	if(result & ~((1 << bitCount) - 1)) {
		std::stringstream sst;
		sst << "value out of range for " << "bits";
		throw Evaluators::EvaluationException(GCx_strdup(sst.str().c_str()));
	}
	return(result);
}


void* pointerFromNode(const NodeT root) {
	Box* rootBox = dynamic_cast<Box*>(root);
	if(rootBox)
		return(rootBox->value);
	else {
		std::stringstream sst;
		sst << "cannot get native pointer for " << str(root);
		std::string v = sst.str();
		throw Evaluators::EvaluationException(v.c_str());
	}
}
static Int int01(1);
static Int int00(0);
bool booleanFromNode(NodeT root) {
	NodeT result = Evaluators::reduce(makeApplication(makeApplication(root, &int01), &int00));
	if(result == NULL)
		throw Evaluators::EvaluationException("that cannot be reduced to a boolean");
	return(result == &int01);
}
char* stringFromNode(NodeT root) {
	Str* rootString = dynamic_cast<Str*>(root);
	if(rootString) {
		// TODO maybe check terminating zero? Maybe not.
		return((char*) rootString->value);
	} else {
		std::stringstream sst;
		sst << "cannot get native string for " << str(root);
		std::string v = sst.str();
		throw Evaluators::EvaluationException(v.c_str());
		//Str* v = makeStrCXX(str(root));
		//return((char*) v->native);
	}
}
char* stringOrNilFromNode(NodeT root) {
	if(root)
		return(stringFromNode(root));
	else
		return(NULL);
}
size_t stringSizeFromNode(NodeT root) {
	Str* rootString = dynamic_cast<Str*>(root);
	if(rootString) {
		return(rootString->size);
	} else {
		std::stringstream sst;
		sst << "cannot get string length of " << str(root);
		std::string v = sst.str();
		throw Evaluators::EvaluationException(v.c_str());
		//Str* v = makeStrCXX(str(root));
		//return((char*) v->native);
	}
}

}
