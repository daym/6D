#include "6D/Allocators"
#include "Values/Values"
#include "Values/Hashtable"
BEGIN_NAMESPACE_6D(Values)

static inline NodeT listFromHashtable(Hashtable::const_iterator iter, Hashtable::const_iterator endIter) {
	if(iter == endIter)
		return(NULL);
	else {
		NodeT k = iter->first;
		NodeT v = iter->second;
		++iter;
		return(cons(pair(k, v), listFromHashtable(iter, endIter)));
	}
}
NodeT keysOfHashtable(Hashtable::const_iterator iter, Hashtable::const_iterator endIter) {
	if(iter == endIter)
		return(NULL);
	else {
		NodeT k = iter->first;
		++iter;
		return(cons(k, keysOfHashtable(iter, endIter)));
	}
}
NodeT getHashtableEntryByKey(NodeT hashtableP, NodeT key, NodeT fallback) {
	const Hashtable* hashtable = (const Hashtable*) getCXXInstance(hashtableP);
	const Hashtable::const_iterator iter = hashtable->find(key);
	if(iter != hashtable->end())
		return iter->second;
	else
		return fallback;
}
void setHashtableEntry(NodeT hashtableP, NodeT key, NodeT value) {
	Hashtable* hashtable = (Hashtable*) getCXXInstance(hashtableP); /* const incorrect */
	(*hashtable)[key] = value;
}

END_NAMESPACE_6D(Values)
