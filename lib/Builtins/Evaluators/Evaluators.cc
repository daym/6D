#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <deque>
#include "Evaluators/Evaluators"
#include "Values/Values"
#include "Formatters/Math"
#include "6D/Values"
#include "6D/Operations"
#include "6D/Allocators"
#include "6D/Evaluators"
#include "Values/Hashtable"
#include "Values/Symbol"
#include "SpecialForms/SpecialForms"
#include "Combinators/Combinators"
#include "Logic/Logic"

namespace Evaluators {
using namespace Values;

/* BEGIN Binary Operations (see Operation) */

bool builtinCallP(Values::NodeT node);
Values::NodeT callBuiltin(Values::NodeT fn, Values::NodeT argument);

/* END Binary Operations */

void initEvaluator(void) {
	Combinators::initCombinators();
	Logic::initLogic();
}

static void getFreeVariablesImpl(Hashtable& boundNames, int boundNamesCount/*includes shadowed*/, Hashtable& freeNames, int& freeNamesCount/*includes name unknown*/, NodeT root) {
	int i;
	if(fnP(root)) {
		NodeT parameterNode = getFnParameter(root);
		NodeT body = getFnBody(root);
		if(!boundNames.containsKeyP(parameterNode)) { // not bound yet
			boundNames[parameterNode] = NULL;
			getFreeVariablesImpl(boundNames, boundNamesCount + 1, freeNames, freeNamesCount, body);
			boundNames.removeByKey(parameterNode);
		} else // already bound to something else: make sure not to get rid of it.
			getFreeVariablesImpl(boundNames, boundNamesCount + 1, freeNames, freeNamesCount, body);
	} else if(callP(root)) {
		getFreeVariablesImpl(boundNames, boundNamesCount, freeNames, freeNamesCount, getCallCallable(root));
		getFreeVariablesImpl(boundNames, boundNamesCount, freeNames, freeNamesCount, getCallArgument(root));
	} else if(symbolP(root)) {
		if(!boundNames.containsKeyP(root))
			freeNames[root] = NULL;
	} else if((i = getSymbolreferenceIndex(root)) != -1) {
		if(i >= boundNamesCount) {
			// FIXME whoops? what if we know there are free variables but not what they are called?
		}
	}
	// else other stuff.
}
int getFreeVariables(Hashtable& freeNames, NodeT root) {
	Hashtable boundNames;
	int freeNamesCount;
	getFreeVariablesImpl(boundNames, 0, freeNames, freeNamesCount, root);
	return freeNamesCount;
}
static inline NodeT error(NodeT context, const char* expectedText, const char* gotText) {
	std::stringstream sst;
	sst << "error: expected \"" << expectedText << "\" but got \"" << gotText << "\"";
	return cons(cons(symbolFromStr("error"), strCXX(sst.str())), nil);
}
static inline bool errorP(NodeT term) {
	// FIXME
	return false;
}
static NodeT annotateImpl(Values::NodeT dynEnv, Values::NodeT boundNames, Hashtable& boundNamesSet, NodeT root) {
	// TODO maybe traverse cons etc? maybe not.
	NodeT result;
	if(fnP(root)) {
		NodeT parameterNode = getFnParameter(root);
		NodeT body = getFnBody(root);
		NodeT parameterSymbolNode = parameterNode;
		assert(parameterSymbolNode);
		if(boundNamesSet.find(parameterSymbolNode) == boundNamesSet.end()) { // not bound yet
			boundNamesSet[parameterSymbolNode] = root; // nice for debugging. Faster would be: NULL;
			result = annotateImpl(dynEnv, cons(parameterSymbolNode, boundNames), boundNamesSet, body);
			boundNamesSet.removeByKey(parameterSymbolNode);
		} else // already bound to something else: make sure not to get rid of it.
			result = annotateImpl(dynEnv, cons(parameterSymbolNode, boundNames), boundNamesSet, body);
		return (result == body) ? root : errorP(result) ? result : fn(parameterNode, result);
	} else if(callP(root)) {
		NodeT operator_ = getCallCallable(root);
		NodeT operand = getCallArgument(root);
		/*if(operator_ == &Reducer || operator_ == Symbols::Sinline) { // ideally this would be auto-detected, but it isn't right now.
			return(annotateImpl(boundNames, boundNamesSet, reduce1(operand)));
		}*/
		NodeT newOperatorNode = annotateImpl(dynEnv, boundNames, boundNamesSet, operator_);
		NodeT newOperandNode = SpecialForms::quoteP(newOperatorNode) ? operand : annotateImpl(dynEnv, boundNames, boundNamesSet, operand);
		return (operator_ == newOperatorNode && operand == newOperandNode) ? root : errorP(newOperatorNode) ? newOperatorNode : errorP(newOperandNode) ? newOperandNode : call(newOperatorNode, newOperandNode);
	} else if(symbolP(root)) {
		int i = indexOfSymbol(root, 0, boundNames);
		if(i != -1) { /* found */
			//std::distance(boundNames.begin(), std::find(boundNames.begin(), boundNames.end(), symbolNode));
			return symbolreference(i); /* root */
		} else {
			//return eval(call(dynEnv, quote(root)));
			// can be error.
			NodeT v = annotate(nil, call(dynEnv, SpecialForms::quote(root)));
			return eval(v); // make very VERY sure that that is not annotated again.
			//return error(root, "<bound-identifier>", getSymbol1Name(root));
		}
	} // else other stuff.
	return(root);
}
NodeT annotate(NodeT dynEnv, NodeT root) {
	Hashtable boundNamesSet;
	return(annotateImpl(dynEnv, nil, boundNamesSet, root));
}
static inline NodeT ensureCall(NodeT term, NodeT fn, NodeT argument) {
	return (getCallCallable(term) == fn && getCallArgument(term) == argument) ? term : call(fn, argument);
}
static NodeT shift(NodeT argument, int index, NodeT term) {
	int x_index = getSymbolreferenceIndex(term);
	if(x_index != -1) {
		if(x_index == index)
			return(argument);
		else if(x_index > index)
			return(symbolreference(x_index - 1));
		else
			return(term);
	} else if(callP(term)) {
		NodeT x_fn = getCallCallable(term);
		NodeT x_argument = getCallArgument(term);
		NodeT new_fn = shift(argument, index, x_fn);
		NodeT new_argument = shift(argument, index, x_argument);
		return (new_fn == x_fn && new_argument == x_argument) ? term : call(new_fn, new_argument);
	} else if(fnP(term)) {
		NodeT body = getFnBody(term);
		NodeT parameter = getFnParameter(term);
		NodeT new_body = shift(argument, index + 1, body);
		return (body == new_body) ? term : fn(parameter, new_body);
	} else 
		return(term);
}
static int fGeneration = 0;
static int recursionLevel = 0;
static inline NodeT remember(NodeT app, NodeT result) {
	((Call*) app)->result = result;
	((Call*) app)->resultGeneration = fGeneration;
	return(result);
}
NodeT eval1(NodeT term) {
	if(!callP(term))
		return term; //return error(term, "<call>", "<noncall>"); // term;
	Call* call = (Call*)(term);
	if(call->resultGeneration == fGeneration)
		return call->result;
	NodeT fn = getCallCallable(term);
	NodeT argument = getCallArgument(term);
	++recursionLevel;
	NodeT x_fn = eval1(fn);
	NodeT x_argument = SpecialForms::fnWantsItsArgumentReducedP(x_fn) ? eval1(argument) : argument;
	--recursionLevel;
	if(errorP(x_argument))
		return x_argument;
	if(fnP(x_fn)) {
		NodeT body = getFnBody(fn);
		body = shift(x_argument, 0, body);
		if(errorP(body))
			return body;
		++recursionLevel;
		body = eval1(body);
		--recursionLevel;
		return(remember(term, body));
	} else if(FFIFnP(x_fn)) {
		NodeT body = execFFIFn(x_fn, x_argument);
		return(remember(term, body));
	} else { // TODO builtins
		if(errorP(x_fn))
			return x_fn;
		return error(x_fn, "<function>", "<junk>");
	}
}

/* public interface */
Values::NodeT eval(Values::NodeT node) {
	try {
		return(eval1(node));
	} catch(std::exception& exception) {
		return(error(node, "<valid-expr>", exception.what()));
	}
}

/* only here for speed. Think of it as FFI. */
#define WORLD nil
Values::NodeT execute(NodeT term) {
	Values::NodeT r = eval(call(term, WORLD));
	// TODO error check
	return(getConsHead(r));
}

}; // end namespace Evaluators.
