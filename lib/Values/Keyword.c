/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#include "Values/Values"
#include "Values/Keyword"
#include "Values/Hashtable"

BEGIN_NAMESPACE_6D(Values)

static NodeT keywords;
bool keywordP(NodeT node) {
	return tagOfNode(node) == TAG_Keyword;
}
NodeT keywordFromStr(const char* name) {
	if(UNLIKELY_6D(keywords == NULL))
		keywords = makeHashtableNoGC();
	NODET key = symbolFromStr(name);
	NODET keyword = getHashtableValueByKey(keywords, key, nil);
	if(keyword)
		return keyword;
	{
		struct Keyword* keyword;
		NODET result;
		keyword = NEW_NOGC(Keyword);
		keyword->text = name;
		result = refCXXInstance(keyword);
		setHashtableEntry(keywords, key, result);
		return result;
	}
	
}
const char* keywordName(NodeT node) {
	if(keywordP(node)) {
		const struct Keyword* s = (const struct Keyword*) getCXXInstance(node);
		return s->text;
	} else
		return NULL;
}

END_NAMESPACE_6D(Values)
