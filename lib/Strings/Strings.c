#include "6D/Values"
#include "Strings/Strings"
#include "Numbers/Integer2"
BEGIN_NAMESPACE_6D(Strings)
static NodeT concatLists(NodeT newTail, NodeT a) {
	if(consP(a))
		return cons(getConsHead(a), concatLists(newTail, getConsTail(a)));
	else
		return newTail;
}
static NodeT listFromStr2(const char* s, size_t sz) {
	if(sz == 0)
		return nil;
	else {
		NodeT hd = internNativeUInt((unsigned char) *s);
		return cons(hd, listFromStr2(s + 1, sz - 1));
	}
}
NodeT listFromStr(NodeT str) {
	if(strP(str)) {
		char* s;
		if(!stringFromNode(str, &s))
			return evalError(strC("<str>"), strC("<junk>"), str);
		return listFromStr2(s, getStrSize(str));
	} else
		return str;
}
static int getListLength(NodeT a) {
	/* TODO handle overflow */
	if(consP(a))
		return 1 + getListLength(getConsTail(a));
	else
		return 0;
}
NodeT strFromList(NodeT l) {
	int len = getListLength(l);
	char* result = GC_MALLOC_ATOMIC(len + 1);
	char* p = result;
	for(; len > 0; --len, ++p, l = getConsTail(l)) {
		int v;
		if(!intFromNode(getConsHead(l), &v) || v < 0 || v > 255)
			return evalError(strC("<int-list>"), strC("<junk>"), l);
		*p = v;
	}
	return strC(p);
}
NodeT concat(NodeT a, NodeT b) {
	if(strP(a))
		a = listFromStr(a);
	if(strP(b))
		b = listFromStr(b);
	return concatLists(b, a);
}
DEFINE_STRICT_BINARY_FN(Bconcat, concat(env, argument))
DEFINE_STRICT_FN(BstrFromList, strFromList(argument))
void initStrings(void) {
	INIT_BINARY_FN(Bconcat)
	INIT_FFI_FN(BstrFromList)
}
END_NAMESPACE_6D(Strings)
