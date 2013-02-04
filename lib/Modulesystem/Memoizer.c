#include "Modulesystem/Memoizer"
#include "Values/Hashtable"
#include "6D/Evaluators"
#include "6D/FFIs"
/* TODO equal FN */
BEGIN_NAMESPACE_6D(Modulesystem)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Evaluators)
static NodeT dummy;
/* memoized: given an environment and argument, tries to find existing memo table entry. If not there, */
static NodeT memoized(NodeT env, NodeT key) {
	NodeT hashtable = getPairFst(env);
	NodeT fn = getPairSnd(env);
	NodeT result = getHashtableValueByKey(hashtable, key, dummy);
	if(result == dummy) {
		result = eval(call(fn, key)); /* XXX is this closed over all variables? */
		/* check for error? makes no difference, just cache the error, too. */
		setHashtableEntry(hashtable, key, result);
	}
	return result;
}
DEFINE_STRICT_FN(Memoizer2, memoized(env, argument))

/* memoizer: given a function, results in a memoizer for that function. */
NodeT memoizer(NodeT fn) {
	if(!dummy)
		dummy = NEW(Call);
	return CLOSED_FN(Memoizer2, pair(makeHashtable(), fn));
}
DEFINE_STRICT_FN(Memoizer, memoizer(argument))
END_NAMESPACE_6D(Modulesystem)
