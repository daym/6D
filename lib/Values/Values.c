/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#include "Values/Values"
#include "Values/Symbol"
#include "Values/Keyword"
#include "Allocators/Allocators"
#include "Numbers/Integer2"
#include "Numbers/Real"
#include "Numbers/Ratio"
BEGIN_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
struct Symbol;
struct Keyword;
struct CFFIFn;
void str(NodeT node, FILE* destination) {
	/* FIXME */
	//fmemopen();
	abort();
}
NodeT operation(NodeT callable, NodeT argument1, NodeT argument2) {
	return call(call(callable, argument1), argument2);
}
NodeT operationArgument1(NodeT o) {
	NodeT o2 = callCallable(o);
	return callArgument(o2);
}
NodeT operationArgument2(NodeT o) {
	return callArgument(o);
}
NodeT operationOperator(NodeT o) {
	NodeT o2 = callCallable(o);
	return callCallable(o2);
}
bool binaryOperationP(NodeT o) {
	return callP(o) && callP(callCallable(o));
}
BEGIN_STRUCT_6D(Fn)
	NodeT parameter;
	NodeT body;
END_STRUCT_6D(Fn)
BEGIN_STRUCT_6D(Cons)
	NodeT head;
	NodeT tail;
END_STRUCT_6D(Cons)
bool nilP(NodeT node) {
	return node == NULL;
}
NodeT cons(NodeT head, NodeT tail) {
	struct Cons* result = NEW(Cons);
	result->head = head;
	result->tail = tail;
	return refCXXInstance(result);
}
/* given a Call, returns its callable. */
NodeT callCallable(NodeT node) {
	const struct Call* call = (const struct Call*) getCXXInstance(node);
	return call->callable;
}

/* given a Call, returns its argument. */
NodeT callArgument(NodeT node) {
	const struct Call* call = (const struct Call*) getCXXInstance(node);
	return call->argument;
}

NodeT fn(NodeT formalParameter, NodeT body) {
	struct Fn* result = NEW(Fn);
	result->parameter = formalParameter;
	result->body = body;
	return refCXXInstance(result);
}

/* given a callable and argument, returns a node that represents a Call */
NodeT call(NodeT callable, NodeT argument) {
	struct Call* result;
	result = NEW(Call);
	result->callable = callable;
	result->argument = argument;
	result->resultGeneration = -1;
	return refCXXInstance(result);
}

/* given a Fn, returns its formal parameter. */
NodeT fnParameter(NodeT node) {
	const struct Fn* fn = (const struct Fn*) getCXXInstance(node);
	return fn->parameter;
}

/* given a Fn, returns its body. */
NodeT fnBody(NodeT node) {
	const struct Fn* fn = (const struct Fn*) getCXXInstance(node);
	return fn->body;
}

BEGIN_STRUCT_6D(Box)
	void* nativePointer;
END_STRUCT_6D(Box)
/* boxes an unknown value so we don't get scared by it */
NodeT box(void* p) {
	struct Box* result;
	result = NEW(Box);
	result->nativePointer = p;
	return refCXXInstance(result);
}

BEGIN_STRUCT_6D(Str) /* TODO derive from Box? */
	void* nativePointer;
	size_t size;
END_STRUCT_6D(Str)
BEGIN_STRUCT_6D(CFFIFn)
	void* nativePointer;
	size_t size;
	NodeT env;
END_STRUCT_6D(CFFIFn)

void* boxValue(NodeT node) {
	const struct Box* box = (const struct Box*) getCXXInstance(node);
	return box->nativePointer;
}
char* strValue(NodeT node) {
	return (char*) boxValue(node);
}
NodeT strC(const char* value) {
	/* TODO not necessarily new. Pool strings? (see Symbols for where it's already done) */
	struct Str* result = NEW(Str);
	result->size = strlen(value);
	result->nativePointer = (void*) value;
	return refCXXInstance(result);
}
size_t strSize(NodeT n) {
	const struct Str* s = (const struct Str*) getCXXInstance(n);
	return s->size;
}
/* given a Cons, returns its head */
NodeT consHead(NodeT node) {
	const struct Cons* cons = (const struct Cons*) getCXXInstance(node);
	return cons->head;
}
/* given a Cons, returns its tail. Tail is USUALLY nil or a(nother) Cons */
NodeT consTail(NodeT node) {
	const struct Cons* cons = (const struct Cons*) getCXXInstance(node);
	return cons->tail;
}
NodeT pair(NodeT a, NodeT b) {
	return cons(a, b);
}
NodeT pairFst(NodeT p) {
	return consHead(p);
}
NodeT pairSnd(NodeT p) {
	return consTail(p);
}
NodeT FFIFn(FFIFnCallbackT callback, NodeT aEnv, const char* name) {
	/* TODO put name => this into some reflection hideout */
	// TODO not necessarily new
	struct CFFIFn* result = NEW(CFFIFn);
	result->env = aEnv;
	result->nativePointer = (void*) callback;
	return refCXXInstance(result);
}
NodeT FFIFnNoGC(FFIFnCallbackT callback, NodeT aEnv, const char* name) {
	/* TODO put name => this into some reflection hideout */
	// TODO not necessarily new
	struct CFFIFn* result = NEW_NOGC(CFFIFn);
	result->env = aEnv;
	result->nativePointer = (void*) callback;
	return refCXXInstance(result);
}
NodeT execFFIFn(NodeT node, NodeT argument) {
	const struct CFFIFn* f = (const struct CFFIFn*) getCXXInstance(node);
	FFIFnCallbackT callback = (FFIFnCallbackT) f->nativePointer;
	return (*callback)(argument, f->env);
}
int tagOfNode(NodeT n) {
#ifdef __cplusplus
	return dynamic_cast<const Symbolreference*>(node) ? TAG_Symbolreference : 
	       dynamic_cast<const Int*>(node) ? TAG_Int : 
	       dynamic_cast<const Integer*>(node) ? TAG_Integer : 
	       dynamic_cast<const Float*>(node) ? TAG_Float : 
	       dynamic_cast<const CFFIFn*>(node) ? TAG_CFFIFn : 
	       dynamic_cast<const Cons*>(node) ? TAG_Cons : 
	       dynamic_cast<const Fn*>(node) ? TAG_Fn : 
	       dynamic_cast<const Call*>(node) ? TAG_Call : 
	       dynamic_cast<const Symbol*>(node) ? TAG_Symbol : 
	       dynamic_cast<const Keyword*>(node) ? TAG_Keyword :
	       dynamic_cast<const Str*>(node) ? TAG_Str : 
	       dynamic_cast<const Box*>(node) ? TAG_Box : 
	       TAG_OPAQUE;
#else
	if(UNLIKELY_6D(n == NULL))
		return TAG_Nil;
	const struct Node* node = (const struct Node*) getCXXInstance(n);
	return node->tag;
#endif
}
static NodeT symbolreferences[200];
NodeT symbolreference(int index) {
	if(index >= 0 && index < 200) {
		if(!symbolreferences[index]) {
			struct Symbolreference* ref;
			ref = NEW_NOGC(Symbolreference);
			ref->index = index;
			symbolreferences[index] = refCXXInstance(ref);
		} /* Note: could also just stack-allocate them */
		return symbolreferences[index];
	} else {
		struct Symbolreference* ref = NEW(Symbolreference);
		ref->index = index;
		return refCXXInstance(ref);
	}
}
bool symbolreferenceP(NodeT n) {
	return tagOfNode(n) == TAG_Symbolreference;
}
/* returns the jump index of n if it is a Symbolreference, otherwise (-1) */
int symbolreferenceIndex(NodeT n) {
	if(symbolreferenceP(n)) {
		const struct Symbolreference* sr = (const struct Symbolreference*) getCXXInstance(n);
		return sr->index;
	} else
		return (-1);
}
bool FFIFnWithCallbackP(NodeT n, FFIFnCallbackT callback) { /* used "internally" only */
	const struct CFFIFn* box = (const struct CFFIFn*) getCXXInstance(n);
	return box->nativePointer == (void*) callback;
}
static int fGeneration = 0;
void setCallResult(NodeT call, NodeT result) {
	struct Call* app = (struct Call*) getCXXInstance(call);
	app->result = result;
	app->resultGeneration = fGeneration;
	/* mutable */
}
void increaseGeneration(void) {
	++fGeneration;
}
int getGeneration(void) {
	return fGeneration;
}
bool boxP(NodeT node) {
	return tagOfNode(node) == TAG_Box;
}
bool strP(NodeT node) {
	return tagOfNode(node) == TAG_Str;
}
bool FFIFnP(NodeT node) {
	return tagOfNode(node) == TAG_CFFIFn;
}
bool fnP(NodeT node) {
	return tagOfNode(node) == TAG_Fn;
}
bool callP(NodeT node) {
	return tagOfNode(node) == TAG_Call;
}
bool consP(NodeT node) {
	return tagOfNode(node) == TAG_Cons;
}
bool pairP(NodeT node) {
	if(!consP(node))
		return false;
	NodeT tl = consTail(node);
	return(!nilP(tl) && !consP(tl)); /* tradition :-( */
}
NodeT call5(NodeT a, NodeT b, NodeT c, NodeT d, NodeT e) {
	return call(call(call(call(a, b), c), d), e);
}
END_NAMESPACE_6D(Values)
