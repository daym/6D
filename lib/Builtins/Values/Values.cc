#include <string>
#include "Values/Values"
#include "Values/Symbol"
#include "Values/Keyword"
#include "Allocators/Allocators"
namespace Values {

struct Symbol;
struct Keyword;
int tagFromNode(NodeT node) {
	return dynamic_cast<const Symbol*>(node) ? TAG_SYMBOL : 
	       dynamic_cast<const Keyword*>(node) ? TAG_KEYWORD :
		   TAG_OPAQUE;

}
Node::~Node(void) {
}
void Node::str(FILE* destination) const {
	fputs("<node>", destination);
}
void str(NodeT node, FILE* destination) {
	//fmemopen();
	node->str(destination);
}
NodeT operation(NodeT callable, NodeT argument1, NodeT argument2) {
	return call(call(callable, argument1), argument2);
}

struct Call : Node {
	NodeT callable;
	NodeT argument;
	Call(NodeT aCallable, NodeT aArgument) :
		callable(aCallable),
		argument(aArgument)
	{
		this->callable = callable;
		this->argument = argument;
	}
	virtual ~Call() {
	}
};
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
			fprintf(destination, "\\x%02X", c);
		} else
			fputc(c, destination);
	}
	fputc('"', destination);
}
NodeT pair(NodeT a, NodeT b) {
	return cons(a, b);
}

};
