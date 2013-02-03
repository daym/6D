#include "6D/Values"
#include "Strings/Strings"
#include "Numbers/Integer2"
BEGIN_NAMESPACE_6D(Strings)
static NodeT concatLists(NodeT newTail, NodeT a) {
	if(errorP(a))
		return a;
	else if(nilP(a))
		return newTail;
	else
		return cons(getConsHead(a), concatLists(newTail, getConsTail(a)));
}
static NodeT listFromStr2(const char* s, size_t sz) {
	if(sz == 0)
		return nil;
	else {
		NodeT hd = internNativeUInt((unsigned char*) *s);
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
NodeT concat(NodeT a, NodeT b) {
	if(strP(a))
		a = listFromStr(a);
	if(strP(b))
		b = listFromStr(b);
	return concatLists(b, a);
}
DEFINE_STRICT_BINARY_FN(Bconcat, concat(env, argument))
void initStrings(void) {
}
END_NAMESPACE_6D(Strings)
