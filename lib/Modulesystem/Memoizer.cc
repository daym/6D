#include "Modulesystem/Memoizer"
#include "Values/Hashtable"
/* TODO equal FN */
namespace Modulesystem {
using namespace Values;
static NodeT dummy;
/* memoized: given an environment and argument, tries to find existing memo table entry. If not there, */
static NodeT memoized(NodeT env, NodeT key) {
	NodeT hashtable = getPairFst(env);
	NodeT fn = getPairSnd(env);
	NodeT result = getHashtableEntryByKey(hashtable, key, dummy);
	if(result == dummy) {
		result = eval(fn, key);
		setHashtableEntry(hashtable, key, result);
	}
	return result;
}
DEFINE_STRICT_FN(Memoizer2, memoized(env, argument))

/* memoizer: given a function, results in a memoizer for that function. */
Values::NodeT memoizer(Values::NodeT fn) {
	if(!dummy)
		dummy = new Node;
	return CLOSED_FFI_FN(Memoizer2, pair(new Hashtable, fn));
}
DEFINE_STRICT_FN(Memoizer, memoizer(argument))
}
