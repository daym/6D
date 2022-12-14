/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "Evaluators/Evaluators"
#include "Values/Values"
#include "6D/Values"
#include "6D/Operations"
#include "6D/Allocators"
#include "6D/Evaluators"
#include "Values/Hashtable"
#include "Values/Symbol"
#include "SpecialForms/SpecialForms"
#include "Combinators/Combinators"
#include "Logic/Logic"
#include "Arithmetics/Arithmetics"
#include "Values/Error"

BEGIN_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Combinators)
USE_NAMESPACE_6D(Logic)
USE_NAMESPACE_6D(SpecialForms)

static INLINE const char* nvl(const char* a, const char* b) {
	return a ? a : b;
}
static int digitInBase(int base, int c) {
	int result = (c >= '0' && c <= '9') ? 0 +  (c - '0') :
	             (c >= 'a' && c <= 'z') ? 10 + (c - 'a') : 
	             (c >= 'A' && c <= 'Z') ? 10 + (c - 'A') : 
	             (-1);
	if(result >= base)
		result = -1;
	return result;
}
static NodeT scientificE(NodeT base, NativeInt exp, NodeT value) {
	assert(exp >= 0);
	/*return integerPowU(base, exp);*/
	int i;
	for(i = 0; i < exp; ++i)
		value = integerMultiply(value, base);
	return value;
}
static NodeT getNumber(int baseI, const char* name) {
	char* p;
	errno = 0;
	NativeInt value = strtol(name, &p, baseI);
	if(errno == 0) {
		NativeInt exp = 0;
		if(*p == 'e' || *p == 'E') {
			++p;
			errno = 0;
			exp = strtol(p, &p, baseI);
		}
		/* TODO fraction for negative exponents? */
		if(errno || *p)
			exp = -1;
		if(exp > 0) {
			NodeT base = internNativeInt((NativeInt) baseI);
			return scientificE(base, exp, internNativeInt(value));
		} else if(exp == 0)
			return internNativeInt(value);
	} else { /* too big */
		int off;
		int exp = 0;
		NodeT base = internNativeInt((NativeInt) baseI);
		NodeT result = internNativeInt((NativeInt) 0);
		p = (char*) name;
		for(;(off = digitInBase(baseI, *p)) != -1;++p) {
			result = integerMultiply(result, base);
			result = integerAddU(result, off);
		}
		if(*p == 'e' || *p == 'E') {
			++p;
			errno = 0;
			exp = strtol(p, &p, baseI);
			if(errno != 0)
				exp = -1;
		} else if(*p)
			exp = -1;
		if(exp >= 0)
			return scientificE(base, exp, result);
	}
	{
		errno = 0;
		NativeFloat value = strtod(name, NULL);
		if(errno == 0)
			return internNativeFloat(value);
		else
			return nil;
	}
}
static INLINE NodeT getDynEnvEntry(NodeT sym) {
	const char* name = symbolName(sym);
	NodeT result = nil;
	if(name) {
		if(digitInBase(10, name[0])) { /* since there is an infinite number of numbers, make sure not to precreate all of them :-) */
			result = getNumber(10, name);
		} else if(name[0] == '#' && digitInBase(10, name[1])) {
			char* p;
			errno = 0;
			NativeInt base = strtol(&name[1], &p, 10);
			if(errno == 0 && p && *p) {
				++p; /* skip 'r' */
				result = getNumber(base, p);
			}
		}
	}
	if(result)
		return result;
	//fprintf(stderr, "info: expression was: ");
	//print(stderr, sym);
	//fprintf(stderr, "\n");
	//fflush(stderr);
	return evaluationError(strC("<dynamic-variable>"), strC(nvl(symbolName(sym), "???")), sym); /* TODO more context */
}
DEFINE_STRICT_FN(DynEnv, getDynEnvEntry(argument))

static NodeT Squote;
NodeT initEvaluators(void) {
	Squote = symbolFromStr("'");
	initCombinators();
	initLogic();
	initSpecialForms();
	INIT_FN(DynEnv);
	return DynEnv;
}
static bool quoteP(NodeT n) {
	return n == Squote;
}
/* also could just use a list instead of the hashtable. Would be slower. */
static void getFreeVariablesImpl(NodeT boundNames, int boundNamesCount/*includes shadowed*/, NodeT freeNames, int* freeNamesCount/*includes name unknown*/, NodeT root) {
	int i;
	if(fnP(root)) {
		NodeT parameterNode = fnParameter(root);
		NodeT body = fnBody(root);
		if(getHashtableValueByKey(boundNames, parameterNode, root) == root) { // not bound yet
			setHashtableEntry(boundNames, parameterNode, nil);
			getFreeVariablesImpl(boundNames, boundNamesCount + 1, freeNames, freeNamesCount, body);
			removeHashtableEntry(boundNames, parameterNode);
		} else // already bound to something else: make sure not to get rid of it.
			getFreeVariablesImpl(boundNames, boundNamesCount + 1, freeNames, freeNamesCount, body);
	} else if(callP(root)) {
		getFreeVariablesImpl(boundNames, boundNamesCount, freeNames, freeNamesCount, callCallable(root));
		getFreeVariablesImpl(boundNames, boundNamesCount, freeNames, freeNamesCount, callArgument(root));
	} else if(symbolP(root)) {
		if(getHashtableValueByKey(boundNames, root, nil) == nil) { // not bound
			setHashtableEntry(freeNames, root, nil);
			++(*freeNamesCount);
		}
	} else if((i = symbolreferenceIndex(root)) != -1) {
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
static INLINE NodeT error(const char* expectedText, const char* gotText, NodeT context) {
	return evaluationError(strC(expectedText), strC(gotText), context);
}

static NodeT annotateImpl(NodeT dynEnv, NodeT boundNames, NodeT boundNamesSet, NodeT root) {
	// TODO maybe traverse cons etc? maybe not.
	NodeT result;
	if(fnP(root)) {
		NodeT parameterNode = fnParameter(root);
		NodeT body = fnBody(root);
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
		NodeT operator_ = callCallable(root);
		NodeT operand = callArgument(root);
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
			NodeT v = annotate(nil, closeOver(Squote, Quoter, call(dynEnv, call(Squote, root)))); /* TODO this quote is hardcoded and probably shouldn't be (not that bad, though). */
			return eval(v); // make very VERY sure that that is not annotated again.
		}
	} // else other stuff.
	return(root);
}
NodeT annotate(NodeT dynEnv, NodeT root) {
	NodeT boundNamesSet = makeHashtable();
	return(annotateImpl(dynEnv, nil, boundNamesSet, root));
}
static INLINE NodeT ensureCall(NodeT term, NodeT fn, NodeT argument) {
	return REUSE_6D((callCallable(term) == fn && callArgument(term) == argument) ? term : )call(fn, argument);
}
static NodeT shift(NodeT argument, int index, NodeT term) {
	int x_index = symbolreferenceIndex(term);
	if(x_index != -1) {
		if(x_index == index)
			return(argument);
		else if(x_index > index)
			return(symbolreference(x_index - 1));
		else
			return(term);
	} else if(callP(term)) {
		NodeT x_fn = callCallable(term);
		NodeT x_argument = callArgument(term);
		NodeT new_fn = shift(argument, index, x_fn);
		NodeT new_argument = shift(argument, index, x_argument);
		return REUSE_6D((new_fn == x_fn && new_argument == x_argument) ? term :) call(new_fn, new_argument);
	} else if(fnP(term)) {
		NodeT body = fnBody(term);
		NodeT parameter = fnParameter(term);
		NodeT new_body = shift(argument, index + 1, body);
		return REUSE_6D((body == new_body) ? term :) fn(parameter, new_body);
	} else 
		return(term);
}
static int recursionLevel = 0;
static INLINE NodeT remember(NodeT app, NodeT result) {
	setCallResult(app, result);
	return(result);
}
NodeT eval1(NodeT term) {
	if(!callP(term))
		return term;
	const struct Call* call = (const struct Call*)getCXXInstance(term);
	if(call->resultGeneration == getGeneration())
		return call->result;
	NodeT fn = callCallable(term);
	NodeT argument = callArgument(term);
	++recursionLevel;
	NodeT x_fn = eval1(fn);
	NodeT x_argument = fnWantsItsArgumentReducedP(x_fn) ? eval1(argument) : argument;
	--recursionLevel;
	if(errorP(x_argument))
		return x_argument;
	else if(fnP(x_fn)) {
		NodeT body = fnBody(x_fn);
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
	NodeT r = dcall(term, WORLD);
	// TODO error check
	return(consHead(r));
}

END_NAMESPACE_6D(Evaluators)
