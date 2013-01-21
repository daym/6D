#include "6D/Allocators"
#include "Values/Hashtable"
namespace Values {

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

}
