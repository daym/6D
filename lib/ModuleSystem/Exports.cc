#include <stdarg.h>
#include "6D/Values"
#include "6D/Allocators"
namespace ModuleSystem {
using namespace Values;
using namespace Allocators;
static NodeT entry(NodeT name, void* fn) {
	NodeT entry = pair(name, box(fn, name/*TODO replicate the entire accessor */));
	return(entry);
}
static NodeT exportsFVA(const char* fmt, char* names, va_list ap) {
	char buf[2049];
	while(*names && (*names == ' ' || *names == '&'))
		++names;
	if(*names && *names != ',') {
		char* x;
		NodeT name;
		x = strchr(names, ',');
		if(!x)
			x = names + strlen(names);
		else
			*x = 0;
		if(snprintf(buf, 2048, fmt, names) == -1)
			abort();
		name = symbolFromStr(buf); // makeStr(buf);
		names = x;
		void* fn = va_arg(ap, void*);
		return(cons(entry(name, fn), exportsFVA(fmt, names, ap)));
	} else
		return(NULL);
}
NodeT exportsQ(const char* names, ...) {
	NodeT result;
	va_list ap;
	va_start(ap, names);
	result = exportsFVA("%s", GCx_strdupNoGC(names), ap);
	va_end(ap);
	return(result);
}
NodeT exportsFQ(const char* fmt, const char* names, ...) {
	NodeT result;
	va_list ap;
	va_start(ap, names);
	result = exportsFVA(fmt, GCx_strdupNoGC(names), ap);
	va_end(ap);
	//result = makeCons(makePair(Sexports, makeCons(Sexports, makeReflector(result))), result); // exports are automatcially added by ModuleSystem dispatcher
	return(result);
}
}
