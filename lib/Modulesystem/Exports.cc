#include <stdarg.h>
#include "6D/Values"
#include "6D/Allocators"
#include "6D/Modulesystem"
#include "6D/FFIs"
BEGIN_NAMESPACE_6D(Modulesystem)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(FFIs)
static NodeT entry(NodeT name, NodeT fn) {
	// TODO FFIFnNoGC(FFIFnCallbackT callback, NodeT aData, const char* name) G_5D_PURE; ??
	NodeT entry = pair(name, fn); // box(fn, name/*TODO replicate the entire accessor */));
	return(entry);
}
static NodeT exportsFVA(const char* fmt, char* names, va_list ap) {
	char buf[2049];
	while(*names && (*names == ' ' || *names == '&'))
		++names;
	if(*names && *names != ',') {
		char* x;
		char* y;
		NodeT name;
		x = strchr(names, ',');
		y = strchr(names, ' ');
		if(y && (!x || y < x))
			x = y;
		if(!x)
			x = names + strlen(names);
		else
			*x = 0;
		if((y = strrchr(names, ':'))) 
			names = y + 1;
		if((y = strrchr(names, '_'))) 
			names = y + 1;
		if(names[strlen(names) - 1] == 'M')
			names[strlen(names) - 1] = '!';
		if(snprintf(buf, 2048, fmt, names) == -1)
			abort();
		name = symbolFromStr(GCx_strdup(buf)); // makeStr(buf);
		names = x;
		NodeT fn = (NodeT) va_arg(ap, void*);
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
static NodeT reflector(NodeT entries) {
	NodeT entry = getConsHead(entries);
	NodeT name = getPairFst(entry);
	return(cons(name, reflector(getConsTail(entries))));
}
NodeT exportsFQ(const char* fmt, const char* names, ...) {
	NodeT result;
	NodeT Sexports = symbolFromStr("exports");
	va_list ap;
	va_start(ap, names);
	result = exportsFVA(fmt, GCx_strdupNoGC(names), ap);
	va_end(ap);
	// TODO sort?
	result = cons(pair(Sexports, cons(Sexports, reflector(result))), result); // exports are automatcially added by Modulesystem dispatcher
	return(result);
}
NodeT dispatch42(NodeT key, NodeT list) {
	if(nilP(list)) {
		/* TODO error */
		return nil;
	} else {
		NodeT hd = getConsHead(list);
		NodeT xkey = getPairFst(hd);
		if(key == xkey)
			return getPairSnd(hd);
		NodeT tl = getConsTail(list);
		return dispatch42(key, tl);
	}
}
/* TODO memoize! */
NodeT dispatch1(NodeT key, NodeT list) {
	return dispatch42(key, list);
}
DEFINE_STRICT_FN(Dispatch, dispatch1(argument, env))

END_NAMESPACE_6D(Modulesystem)
