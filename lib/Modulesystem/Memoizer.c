/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
	NodeT hashtable = pairFst(env);
	NodeT fn = pairSnd(env);
	NodeT result = getHashtableValueByKey(hashtable, key, dummy);
	if(result == dummy) {
		result = dcall(fn, key); /* XXX is this closed over all variables? */
		/* check for error? makes no difference, just cache the error, too. */
		setHashtableEntry(hashtable, key, result);
	}
	return result;
}
DEFINE_STRICT_FN(Memoizer2, memoized(env, argument))

/* memoizer: given a function, results in a memoizer for that function. */
NodeT memoizer(NodeT fn) {
	if(!dummy)
		dummy = refCXXInstance(NEW(Call));
	return CLOSED_FN(Memoizer2, pair(makeHashtable(), fn));
}
DEFINE_STRICT_FN(Memoizer, memoizer(argument))
END_NAMESPACE_6D(Modulesystem)
