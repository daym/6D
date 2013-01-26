#include <string>
#include "6D/Values"
#include "Values/Values"
#include "Values/Symbol"
#include "Values/Keyword"
#include "Allocators/Allocators"
#include "Formatters/TExpression"
namespace Values {

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
	Fn(NodeT aParameter, NodeT aBody) :
		parameter(aParameter),
		body(aBody)
	{
	}
	virtual ~Fn() {
	}
};
struct Cons : Node {
	NodeT head;
	NodeT tail;
	Cons(NodeT aHead, NodeT aTail) :
		head(aHead),
		tail(aTail)
	{
	}
	virtual ~Cons() {
	}
};
bool fnP(NodeT node) {
	return dynamic_cast<const Fn*>(node) != NULL;
}
bool callP(NodeT node) {
	return dynamic_cast<const Call*>(node) != NULL;
}
bool consP(NodeT node) {
	return dynamic_cast<const Cons*>(node) != NULL;
}
bool nilP(NodeT node) {
	return node == NULL;
}
NodeT cons(NodeT head, NodeT tail) {
	return new Cons(head, tail);
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
	return new Fn(formalParameter, body);
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
bool boxP(NodeT node) {
	return dynamic_cast<const Box*>(node) != NULL;
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
bool FFIFnP(NodeT node) {
	return tagFromNode(node) == TAG_FFI_FN;
}
NodeT execFFIFn(NodeT node, NodeT argument) {
	CFFIFn* f = (CFFIFn*) node;
	FFIFnCallbackT callback = (FFIFnCallbackT) f->nativePointer;
	return (*callback)(argument, f->env);
}
/* FIXME Numbers (especially Int, Float) */
int tagFromNode(NodeT node) {
	return dynamic_cast<const Symbol*>(node) ? TAG_SYMBOL : 
	       dynamic_cast<const Keyword*>(node) ? TAG_KEYWORD :
		   dynamic_cast<const Symbolreference*>(node) ? TAG_SYMBOLREFERENCE : 
		   dynamic_cast<const CFFIFn*>(node) ? TAG_FFI_FN : 
		   TAG_OPAQUE;
}
static NodeT symbolreferences[200];
Values::NodeT symbolreference(int index) {
	if(index >= 0 && index < 200) {
		if(!symbolreferences[index])
			symbolreferences[index] = new Symbolreference(index);
		return symbolreferences[index];
	} else
		return new Symbolreference(index);
}
/* returns the jump index of n if it is a Symbolreference, otherwise (-1) */
int getSymbolreferenceIndex(Values::NodeT n) {
	const Symbolreference* sr = dynamic_cast<const Symbolreference*>(n); // dynamic on purpose
	return sr  ? sr->index : -1;
}
bool FFIFnWithCallbackP(NodeT n, FFIFnCallbackT callback) { /* used "internally" only */
	return ((const Box*) n)->nativePointer == callback;
}
};
