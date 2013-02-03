#include <stdio.h>
#include "6D/Builtins"
#include "6D/Values"
#include "6D/FFIs"
#include "SpecialForms/SpecialForms"
#include "Modulesystem/Memoizer"
#include "6D/Modulesystem"
BEGIN_NAMESPACE_6D(Builtins)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(SpecialForms)
static NodeT SConstanter;
static NodeT Srem;
static NodeT Sexports;
static NodeT Smemoize;
static NodeT builtin(NodeT node) {
	//Modulesystem::exportsQ("Constanter rem memoize", SpecialForms::Constanter, SpecialForms::Identer, Modulesystem::Memoizer);
	if(node == SConstanter)
		return Constanter;
	else if(node == Srem)
		return Identer;
	else if(node == Smemoize)
		return Memoizer;
	else if(node == Sexports)
		return cons(SConstanter, cons(Srem, cons(Smemoize, nil)));
	else
		return nil; // FIXME error
}
// TODO Modulesystem::memoize
//DEFINE_STRICT_FN(Builtins, dispatch(Modulesystem::exportsQ("Constanter rem memoize", SpecialForms::Constanter, SpecialForms::Identer, Modulesystem::Memoizer)))
NodeT initBuiltins(void) {
	SConstanter = symbolFromStr("Constanter");
	Srem = symbolFromStr("rem");
	return nil; // Builtins;
}
END_NAMESPACE_6D(Builtins)
