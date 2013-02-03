#include "6D/Values"
#include "Values/Error"
BEGIN_NAMESPACE_6D(Values)
NodeT parseError(NodeT aExpectedInput, NodeT aGotInput) {
	struct Error* result = NEW(Error);
	result->kind = symbolFromStr("ParseError");
	result->expectedInput = aExpectedInput;
	result->gotInput = aGotInput;
	result->context = nil;
	return result;
}
bool errorP(NodeT n) {
	return tagOfNode(n) == TAG_Error;
}
NodeT evalError(NodeT aExpectedInput, NodeT aGotInput, NodeT aContext) {
	struct Error* result = NEW(Error);
	result->kind = symbolFromStr("EvalError");
	result->expectedInput = aExpectedInput;
	result->gotInput = aGotInput;
	result->context = aContext;
	return result;
}

END_NAMESPACE_6D(Values)
