/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#include "Strings/Strings"
#include "Numbers/Integer2"
BEGIN_NAMESPACE_6D(Strings)
static NodeT concatLists(NodeT newTail, NodeT a) {
	if(consP(a))
		return cons(consHead(a), concatLists(newTail, consTail(a)));
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
			return evaluationError(strC("<str>"), strC("<junk>"), str);
		return listFromStr2(s, strSize(str));
	} else
		return str;
}
static int getListLength(NodeT a) {
	/* TODO handle overflow */
	if(consP(a))
		return 1 + getListLength(consTail(a));
	else
		return 0;
}
NodeT strFromList(NodeT l) {
	int len = getListLength(l);
	char* result = GC_MALLOC_ATOMIC(len + 1);
	char* p = result;
	for(; len > 0; --len, ++p, l = consTail(l)) {
		int v;
		if(!intFromNode(consHead(l), &v) || v < 0 || v > 255)
			return evaluationError(strC("<int-list>"), strC("<junk>"), l);
		*p = v;
	}
	return strC(p);
}
NodeT concat(NodeT a, NodeT b) {
	if(strP(a))
		a = listFromStr(a);
	if(strP(b))
		b = listFromStr(b);
	if(nilP(b))
		return a;
	else
		return concatLists(b, a);
}
DEFINE_STRICT_BINARY_FN(Bconcat, concat(env, argument))
DEFINE_STRICT_FN(BstrFromList, strFromList(argument))
void initStrings(void) {
	INIT_BINARY_FN(Bconcat)
	INIT_FN(BstrFromList)
}
END_NAMESPACE_6D(Strings)
