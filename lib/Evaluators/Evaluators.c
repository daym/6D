#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "Evaluators/Evaluators"
#include "Values/Values"
#include "Formatters/Math"
#include "Formatters/TExpression"
#include "6D/Values"
#include "6D/Operations"
#include "6D/Allocators"
#include "6D/Evaluators"
#include "Values/Hashtable"
#include "Values/Symbol"
#include "SpecialForms/SpecialForms"
#include "Combinators/Combinators"
#include "Logic/Logic"

BEGIN_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Combinators)
USE_NAMESPACE_6D(Logic)
USE_NAMESPACE_6D(SpecialForms)

static NodeT Squote;
void initEvaluator(void) {
	Squote = symbolFromStr("'");
	initCombinators();
	initLogic();
	initSpecialForms();
}
static bool quoteP(NodeT n) {
	return n == Squote;
}
/* also could just use a list instead of the hashtable. Would be slower. */
static void getFreeVariablesImpl(NodeT boundNames, int boundNamesCount/*includes shadowed*/, NodeT freeNames, int* freeNamesCount/*includes name unknown*/, NodeT root) {
	int i;
	if(fnP(root)) {
		NodeT parameterNode = getFnParameter(root);
		NodeT body = getFnBody(root);
		if(getHashtableValueByKey(boundNames, parameterNode, root) == root) { // not bound yet
			setHashtableEntry(boundNames, parameterNode, nil);
			getFreeVariablesImpl(boundNames, boundNamesCount + 1, freeNames, freeNamesCount, body);
			removeHashtableEntry(boundNames, parameterNode);
		} else // already bound to something else: make sure not to get rid of it.
			getFreeVariablesImpl(boundNames, boundNamesCount + 1, freeNames, freeNamesCount, body);
	} else if(callP(root)) {
		getFreeVariablesImpl(boundNames, boundNamesCount, freeNames, freeNamesCount, getCallCallable(root));
		getFreeVariablesImpl(boundNames, boundNamesCount, freeNames, freeNamesCount, getCallArgument(root));
	} else if(symbolP(root)) {
		if(getHashtableValueByKey(boundNames, root, nil) == nil) { // not bound
			setHashtableEntry(freeNames, root, nil);
			++(*freeNamesCount);
		}
	} else if((i = getSymbolreferenceIndex(root)) != -1) {
		if(i >= boundNamesCount) {
			// FIXME whoops? what if we know there are free variables but not what they are called?
			++(*freeNamesCount);
		}
	}
	// else other stuff.
}
int getFreeVariables(NodeT freeNames, NodeT root) {
	NodeT boundNames = makeHashtable();
	int freeNamesCount = 0;
	getFreeVariablesImpl(boundNames, 0, freeNames, &freeNamesCount, root);
	return freeNamesCount;
}
static inline NodeT error(const char* expectedText, const char* gotText, NodeT context) {
	return evalError(strC(expectedText), strC(gotText), context);
}
static NodeT annotateImpl(NodeT dynEnv, NodeT boundNames, NodeT boundNamesSet, NodeT root) {
	// TODO maybe traverse cons etc? maybe not.
	NodeT result;
	if(fnP(root)) {
		NodeT parameterNode = getFnParameter(root);
		NodeT body = getFnBody(root);
		NodeT parameterSymbolNode = parameterNode;
		assert(parameterSymbolNode);
		if(getHashtableValueByKey(boundNamesSet, parameterSymbolNode, nil) == nil) { // not bound yet
			setHashtableEntry(boundNamesSet, parameterSymbolNode, root); // nice for debugging. 
			result = annotateImpl(dynEnv, cons(parameterSymbolNode, boundNames), boundNamesSet, body);
			removeHashtableEntry(boundNamesSet, parameterSymbolNode);
		} else // already bound to something else: make sure not to get rid of it.
			result = annotateImpl(dynEnv, cons(parameterSymbolNode, boundNames), boundNamesSet, body);
		return REUSE_6D((result == body) ? root : )errorP(result) ? result : fn(parameterNode, result);
	} else if(callP(root)) {
		NodeT operator_ = getCallCallable(root);
		NodeT operand = getCallArgument(root);
		/*if(operator_ == &Reducer || operator_ == Symbols::Sinline) { // ideally this would be auto-detected, but it isn't right now.
			return(annotateImpl(boundNames, boundNamesSet, reduce1(operand)));
		}*/
		NodeT newOperatorNode = annotateImpl(dynEnv, boundNames, boundNamesSet, operator_);
		NodeT newOperandNode = quoteP(/*ugh*/operator_) ? operand : annotateImpl(dynEnv, boundNames, boundNamesSet, operand);
		return REUSE_6D((operator_ == newOperatorNode && operand == newOperandNode) ? root : )errorP(newOperatorNode) ? newOperatorNode : errorP(newOperandNode) ? newOperandNode : call(newOperatorNode, newOperandNode);
	} else if(symbolP(root)) {
		int i = indexOfSymbol(root, 0, boundNames);
		if(i != -1) { /* found */
			//std::distance(boundNames.begin(), std::find(boundNames.begin(), boundNames.end(), symbolNode));
			return symbolreference(i); /* root */
		} else {
			// can be error.
			NodeT v = annotate(nil, close(Squote, Quoter, call(dynEnv, call(Squote, root)))); /* TODO this quote is hardcoded and probably shouldn't be (not that bad, though). */
			return eval(v); // make very VERY sure that that is not annotated again.
		}
	} // else other stuff.
	return(root);
}
NodeT annotate(NodeT dynEnv, NodeT root) {
	NodeT boundNamesSet = makeHashtable();
	return(annotateImpl(dynEnv, nil, boundNamesSet, root));
}
static inline NodeT ensureCall(NodeT term, NodeT fn, NodeT argument) {
	return REUSE_6D((getCallCallable(term) == fn && getCallArgument(term) == argument) ? term : )call(fn, argument);
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
		return REUSE_6D((new_fn == x_fn && new_argument == x_argument) ? term :) call(new_fn, new_argument);
	} else if(fnP(term)) {
		NodeT body = getFnBody(term);
		NodeT parameter = getFnParameter(term);
		NodeT new_body = shift(argument, index + 1, body);
		return REUSE_6D((body == new_body) ? term :) fn(parameter, new_body);
	} else 
		return(term);
}
static int recursionLevel = 0;
static inline NodeT remember(NodeT app, NodeT result) {
	setCallResult(app, result);
	return(result);
}
NodeT eval1(NodeT term) {
	//Formatters::TExpression::print(stderr, term);
	//fprintf(stderr, "\n");
	//fflush(stderr);
	if(!callP(term))
		return term;
	const struct Call* call = (const struct Call*)getCXXInstance(term);
	if(call->resultGeneration == getGeneration())
		return call->result;
	NodeT fn = getCallCallable(term);
	NodeT argument = getCallArgument(term);
	++recursionLevel;
	NodeT x_fn = eval1(fn);
	NodeT x_argument = fnWantsItsArgumentReducedP(x_fn) ? eval1(argument) : argument;
	--recursionLevel;
	if(errorP(x_argument))
		return x_argument;
	if(fnP(x_fn)) {
		NodeT body = getFnBody(x_fn);
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
		return error("<callable>", "<junk>", x_fn);
	}
}

/* public interface */
NodeT eval(NodeT node) {
#ifdef __cplusplus
	try {
		return(eval1(node));
	} catch(std::exception& exception) {
		return(error("<valid-expr>", GCx_strdup(exception.what()), node));
	}
#else
	return(eval1(node));
#endif
}

/* only here for speed. Think of it as FFI. */
#define WORLD nil
NodeT execute(NodeT term) {
	NodeT r = eval(call(term, WORLD));
	// TODO error check
	return(getConsHead(r));
}

END_NAMESPACE_6D(Evaluators)
