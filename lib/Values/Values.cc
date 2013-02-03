#include <string>
#include "6D/Values"
#include "Values/Values"
#include "Values/Symbol"
#include "Values/Keyword"
#include "Allocators/Allocators"
#include "Formatters/TExpression"
#include "Numbers/Integer2"
#include "Numbers/Real"
#include "Numbers/Ratio"
BEGIN_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
struct Symbol;
struct Keyword;
struct CFFIFn;
Node::~Node(void) {
}
void Node::str(FILE* destination) const {
	fputs("<node>", destination);
}
void str(NodeT node, FILE* destination) {
	//fmemopen();
	if(!node)
		fprintf(destination, "[]");
	else
		node->str(destination);
}
NodeT operation(NodeT callable, NodeT argument1, NodeT argument2) {
	return call(call(callable, argument1), argument2);
}
NodeT getOperationArgument1(NodeT o) {
	NodeT o2 = getCallCallable(o);
	return getCallArgument(o2);
}
NodeT getOperationArgument2(NodeT o) {
	return getCallArgument(o);
}
NodeT getOperationOperator(NodeT o) {
	NodeT o2 = getCallCallable(o);
	return getCallCallable(o2);
}
bool operationP(NodeT o) {
	return callP(o) && callP(getCallCallable(o));
}
struct Fn : Node {
	NodeT parameter;
	NodeT body;
	virtual ~Fn() {
	}
};
struct Cons : Node {
	NodeT head;
	NodeT tail;
	virtual ~Cons() {
	}
};
bool nilP(NodeT node) {
	return node == NULL;
}
NodeT cons(NodeT head, NodeT tail) {
	Cons* result = new Cons;
	result->head = head;
	result->tail = tail;
	return result;
}
/* given a Call, returns its callable. */
NodeT getCallCallable(NodeT node) {
	return static_cast<const Call*>(node)->callable;
}

/* given a Call, returns its argument. */
NodeT getCallArgument(NodeT node) {
	return static_cast<const Call*>(node)->argument;
}

NodeT fn(NodeT formalParameter, NodeT body) {
	Fn* result = new Fn;
	result->parameter = formalParameter;
	result->body = body;
	return result;
}

/* given a callable and argument, returns a node that represents a Call */
NodeT call(NodeT callable, NodeT argument) {
	return new Call(callable, argument);
}

/* given a Fn, returns its formal parameter. */
NodeT getFnParameter(NodeT node) {
	return static_cast<const Fn*>(node)->parameter;
}

/* given a Fn, returns its body. */
NodeT getFnBody(NodeT node) {
	return static_cast<const Fn*>(node)->body;
}

struct Box : Node {
	void* nativePointer;
	Box(void* aNativePointer) : 
		nativePointer(aNativePointer)
	{
	}
	virtual ~Box(void) {
	}
};
void* getBoxValue(NodeT node) {
	const Box* box = (const Box*) getCXXInstance(node);
	return box->nativePointer;
}
char* getStrValue(NodeT node) {
	return (char*) getBoxValue(node);
}
struct Str : Box {
	size_t size;
	Str(const char* aNativePointer, size_t aSize) : 
		Box((void*) aNativePointer),
		size(aSize)
	{
	}
	Str(const std::string& value) :
		Box((void*) GCx_strdup(value.c_str())),
		size(value.length())
	{
	}
	virtual ~Str(void) {
	}
	virtual void str(FILE* destination) const;
};
NodeT strCXX(const std::string& value) {
	/* TODO not necessarily new. Pool strings? (see Symbols for where it's already done) */
	return new Str(value);
}
size_t getStrSize(NodeT n) {
	const Str* s = (const Str*) getCXXInstance(n);
	return s->size;
}
/* given a Cons, returns its head */
NodeT getConsHead(NodeT node) {
	return static_cast<const Cons*>(node)->head;
}
/* given a Cons, returns its tail. Tail is USUALLY nil or a(nother) Cons */
NodeT getConsTail(NodeT node) {
	return static_cast<const Cons*>(node)->tail;
}
void Str::str(FILE* destination) const {
	const char* s = (const char*) nativePointer;
	fputc('"', destination);
	for(size_t sz = size; sz > 0; --sz, ++s) {
		char c = *s;
		if(c < 32) {
			switch(c) {
			case '\n':
				fprintf(destination, "\\n");
				break;
			case '\r':
				fprintf(destination, "\\r");
				break;
			case '\b':
				fprintf(destination, "\\b");
				break;
			case '\t':
				fprintf(destination, "\\t");
				break;
			case '\f':
				fprintf(destination, "\\f");
				break;
			case '\a':
				fprintf(destination, "\\a");
				break;
			case '\v':
				fprintf(destination, "\\v");
				break;
			default:
				fprintf(destination, "\\x%02X", c);
			}
		} else if(c == '\\' || c == '"' /*|| c == '\''*/) {
			fputc('\\', destination);
			fputc(c, destination);
		} else
			fputc(c, destination);
	}
	fputc('"', destination);
}
NodeT pair(NodeT a, NodeT b) {
	return cons(a, b);
}
NodeT getPairFst(NodeT p) {
	return getConsHead(p);
}
NodeT getPairSnd(NodeT p) {
	return getConsTail(p);
}
struct CFFIFn : Box {
	NodeT env;
	CFFIFn(NodeT aEnv, FFIFnCallbackT aCallback) : 
		Box((void*) aCallback),
		env(aEnv)
	{
	}
};

NodeT FFIFn(FFIFnCallbackT callback, NodeT aEnv, const char* name) {
	/* TODO put name => this into some reflection hideout */
	// TODO not necessarily new
	return new CFFIFn(aEnv, callback);
}
NodeT FFIFnNoGC(FFIFnCallbackT callback, NodeT aEnv, const char* name) {
	/* TODO put name => this into some reflection hideout */
	// TODO not necessarily new
	return new (NoGC) CFFIFn(aEnv, callback);
}
NodeT execFFIFn(NodeT node, NodeT argument) {
	CFFIFn* f = (CFFIFn*) getCXXInstance(node);
	FFIFnCallbackT callback = (FFIFnCallbackT) f->nativePointer;
	return (*callback)(argument, f->env);
}
int tagOfNode(NodeT node) {
	return dynamic_cast<const Symbolreference*>(node) ? TAG_SYMBOLREFERENCE : 
	       dynamic_cast<const Int*>(node) ? TAG_INT : 
	       dynamic_cast<const Integer*>(node) ? TAG_INTEGER : 
	       dynamic_cast<const Float*>(node) ? TAG_FLOAT : 
	       dynamic_cast<const CFFIFn*>(node) ? TAG_FFI_FN : 
	       dynamic_cast<const Cons*>(node) ? TAG_CONS : 
	       dynamic_cast<const Fn*>(node) ? TAG_FN : 
	       dynamic_cast<const Call*>(node) ? TAG_CALL : 
	       dynamic_cast<const Symbol*>(node) ? TAG_SYMBOL : 
	       dynamic_cast<const Keyword*>(node) ? TAG_KEYWORD :
	       dynamic_cast<const Str*>(node) ? TAG_STR : 
	       dynamic_cast<const Box*>(node) ? TAG_BOX : 
	       TAG_OPAQUE;
}
static NodeT symbolreferences[200];
NodeT symbolreference(int index) {
	if(index >= 0 && index < 200) {
		if(!symbolreferences[index])
			symbolreferences[index] = new (NoGC) Symbolreference(index); /* Note: could also just stack-allocate them */
		return symbolreferences[index];
	} else
		return new Symbolreference(index);
}
bool symbolreferenceP(NodeT n) {
	return tagOfNode(n) == TAG_SYMBOLREFERENCE;
}
/* returns the jump index of n if it is a Symbolreference, otherwise (-1) */
int getSymbolreferenceIndex(NodeT n) {
	if(symbolreferenceP(n)) {
		const Symbolreference* sr = (const Symbolreference*) getCXXInstance(n);
		return sr->index;
	} else
		return (-1);
}
bool FFIFnWithCallbackP(NodeT n, FFIFnCallbackT callback) { /* used "internally" only */
	return ((const Box*) getCXXInstance(n))->nativePointer == callback;
}
static int fGeneration = 0;
void setCallResult(NodeT call, NodeT result) {
	Call* app = (Call*) getCXXInstance(call);
	app->result = result;
	app->resultGeneration = fGeneration;
}
void increaseGeneration(void) {
	++fGeneration;
}
int getGeneration(void) {
	return fGeneration;
}
bool boxP(NodeT node) {
	return tagOfNode(node) == TAG_BOX;
}
bool strP(NodeT node) {
	return tagOfNode(node) == TAG_STR;
}
bool FFIFnP(NodeT node) {
	return tagOfNode(node) == TAG_FFI_FN;
}
bool fnP(NodeT node) {
	return tagOfNode(node) == TAG_FN;
}
bool callP(NodeT node) {
	return tagOfNode(node) == TAG_CALL;
}
bool consP(NodeT node) {
	return tagOfNode(node) == TAG_CONS;
}

END_NAMESPACE_6D(Values)
