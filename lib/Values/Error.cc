#include "Values/Error"
namespace Values {
NodeT parseError(NodeT aExpectedInput, NodeT aGotInput) {
	return new ParseError(aExpectedInput, aGotInput);
}
bool errorP(NodeT n) {
	return tagOfNode(n) == TAG_ERROR;
}
NodeT evalError(NodeT aExpectedInput, NodeT aGotInput, NodeT aContext) {
	return new EvalError(aExpectedInput, aGotInput, aContext);
}

}
