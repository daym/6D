#include <stdio.h>
#include "6D/Builtins"
#include "6D/Values"
#include "6D/FFIs"
#include "SpecialForms/SpecialForms"
#include "Modulesystem/Memoizer"
#include "6D/Modulesystem"
#include "Numbers/Integer2"
#include "Strings/Strings"
BEGIN_NAMESPACE_6D(Builtins)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Strings)
static NodeT SConstanter;
static NodeT Srem;
static NodeT Shashexports;
static NodeT Smemoize;
static NodeT SsymbolsEqP;
static NodeT SsymbolsLEP;
static NodeT SsymbolFromStr;
static NodeT SkeywordFromStr;
static NodeT Ssymbol1Name;
static NodeT Skeyword1Name;
static NodeT SnilP;
static NodeT Snil;
static NodeT Scolon;
//static NodeT SstrC;
static NodeT SstrSize;
static NodeT Sfn;
static NodeT Scall;
static NodeT Shead;
static NodeT Stail;
static NodeT ScallCallable;
static NodeT ScallArgument;
static NodeT SfnParameter;
static NodeT SfnBody;
static NodeT SsymbolP;
static NodeT SkeywordP;
static NodeT SfnP;
static NodeT ScallP;
static NodeT SconsP;
static NodeT SboxP;
static NodeT SstrP;
static NodeT SintP;
static NodeT SintegerP;
static NodeT SfloatP;
static NodeT Splusplus;
//static NodeT SboxValue;
//static NodeT SstrValue;
/* operation */
/*bool operationP(NodeT o) G_5D_PURE;*/
/*extern NodeT nil;*/
static NodeT Spair;
static NodeT SpairFst;
static NodeT SpairSnd;
static NodeT SsymbolreferenceIndex;
static NodeT Ssymbolreference;
static NodeT SratioP;
static NodeT SratioA;
static NodeT SratioB;
static NodeT SparseError;
static NodeT SevalError;
static NodeT SerrorP;
static NodeT SerrorKind;
static NodeT SerrorExpectedInput;
static NodeT SerrorGotInput;
static NodeT SerrorContext;
static NodeT SstrFromList;
static bool symbolsEqP(NodeT a, NodeT b) {
	return a == b;
}
static bool symbolsLEP(NodeT a, NodeT b) {
	return a <= b;
}
static INLINE NodeT marshalStr(NodeT (*cb)(const char* s), NodeT argument) {
	char* value;
	if(!stringFromNode(argument, &value))
		return evalError(strC("<str>"), strC("<junk>"), argument);
	else
		return (*cb)(value);
}
static INLINE NodeT marshalInt(NodeT (*cb)(int s), NodeT argument) {
	int value;
	if(!intFromNode(argument, &value))
		return evalError(strC("<int>"), strC("<junk>"), argument);
	else
		return (*cb)(value);
}
DEFINE_STRICT_BINARY_FN(BsymbolsEqP, internNativeBool(symbolsEqP(env, argument)))
DEFINE_STRICT_BINARY_FN(BsymbolsLEP, internNativeBool(symbolsLEP(env, argument)))
DEFINE_STRICT_FN(BsymbolFromStr, marshalStr(symbolFromStr, argument))
DEFINE_STRICT_FN(BkeywordFromStr, marshalStr(keywordFromStr, argument))
DEFINE_STRICT_FN(Bsymbol1Name, strC(getSymbol1Name(argument)))
DEFINE_STRICT_FN(Bkeyword1Name, strC(getKeyword1Name(argument)))
DEFINE_STRICT_FN(BnilP, internNativeBool(nilP(argument)))
DEFINE_STRICT_BINARY_FN(Bcolon, cons(env, argument))
DEFINE_STRICT_FN(BstrSize, internNativeUInt(getStrSize(argument)))
DEFINE_STRICT_BINARY_FN(Bfn, fn(env, argument))
DEFINE_STRICT_BINARY_FN(Bcall, call(env, argument))
DEFINE_STRICT_FN(Bhead, getConsHead(argument))
DEFINE_STRICT_FN(Btail, getConsTail(argument))
DEFINE_STRICT_FN(BcallCallable, getCallCallable(argument))
DEFINE_STRICT_FN(BcallArgument, getCallArgument(argument))
DEFINE_STRICT_FN(BfnParameter, getFnParameter(argument))
DEFINE_STRICT_FN(BfnBody, getFnBody(argument))
DEFINE_STRICT_FN(BsymbolP, internNativeBool(symbolP(argument)))
DEFINE_STRICT_FN(BkeywordP, internNativeBool(keywordP(argument)))
DEFINE_STRICT_FN(BfnP, internNativeBool(fnP(argument)))
DEFINE_STRICT_FN(BcallP, internNativeBool(callP(argument)))
DEFINE_STRICT_FN(BconsP, internNativeBool(consP(argument)))
DEFINE_STRICT_FN(BboxP, internNativeBool(boxP(argument)))
DEFINE_STRICT_FN(BstrP, internNativeBool(strP(argument)))
DEFINE_STRICT_FN(BintP, internNativeBool(intP(argument)))
DEFINE_STRICT_FN(BintegerP, internNativeBool(integerP(argument)))
DEFINE_STRICT_FN(BfloatP, internNativeBool(floatP(argument)))
DEFINE_STRICT_BINARY_FN(Bpair, pair(env, argument))
DEFINE_STRICT_FN(BpairFst, getPairFst(argument))
DEFINE_STRICT_FN(BpairSnd, getPairSnd(argument))
DEFINE_STRICT_FN(BsymbolreferenceIndex, internNativeInt(getSymbolreferenceIndex(argument)))
DEFINE_STRICT_FN(Bsymbolreference, marshalInt(symbolreference, argument))
DEFINE_STRICT_FN(BratioP, internNativeBool(ratioP(argument)))
DEFINE_STRICT_FN(BratioA, getRatioA(argument))
DEFINE_STRICT_FN(BratioB, getRatioB(argument))
DEFINE_STRICT_BINARY_FN(BparseError, parseError(env, argument))
DEFINE_STRICT_TERNARY_FN(BevalError, evalError(getPairFst(env), getPairSnd(env), argument))
DEFINE_STRICT_FN(BerrorP, internNativeBool(errorP(argument)))
DEFINE_STRICT_FN(BerrorKind, getErrorKind(argument))
DEFINE_STRICT_FN(BerrorExpectedInput, getErrorExpectedInput(argument))
DEFINE_STRICT_FN(BerrorGotInput, getErrorGotInput(argument))
DEFINE_STRICT_FN(BerrorContext, getErrorContext(argument))

static NodeT builtin(NodeT node) {
	//Modulesystem::exportsQ("Constanter rem memoize", SpecialForms::Constanter, SpecialForms::Identer, Modulesystem::Memoizer);
	return (node == SConstanter) ? Constanter : 
	       (node == Srem) ? Identer : 
	       (node == Smemoize) ? Memoizer : 
	       (node == Shashexports) ? cons(SConstanter, cons(Srem, cons(Smemoize, nil))) : 
	       (node == SsymbolsEqP) ? BsymbolsEqP : 
	       (node == SsymbolsLEP) ? BsymbolsLEP : 
	       (node == SsymbolFromStr) ? BsymbolFromStr : 
	       (node == SkeywordFromStr) ? BkeywordFromStr : 
	       (node == Ssymbol1Name) ? Bsymbol1Name : 
	       (node == Skeyword1Name) ? Bkeyword1Name : 
	       (node == SnilP) ? BnilP : 
	       (node == Snil) ? nil : 
	       (node == Scolon) ? Bcolon : 
	       (node == SstrSize) ? BstrSize : 
	       (node == Sfn) ? Bfn : 
	       (node == Scall) ? Bcall : 
	       (node == Shead) ? Bhead : 
	       (node == Stail) ? Btail : 
	       (node == ScallCallable) ? BcallCallable : 
	       (node == ScallArgument) ? BcallArgument : 
	       (node == SfnParameter) ? BfnParameter : 
	       (node == SfnBody) ? BfnBody : 
	       (node == SsymbolP) ? BsymbolP : 
	       (node == SkeywordP) ? BkeywordP : 
	       (node == SfnP) ? BfnP : 
	       (node == ScallP) ? BcallP : 
	       (node == SconsP) ? BconsP : 
	       (node == SboxP) ? BboxP : 
	       (node == SstrP) ? BstrP : 
	       (node == SintP) ? BintP : 
	       (node == SintegerP) ? BintegerP : 
	       (node == SfloatP) ? BfloatP : 
	       (node == Spair) ? Bpair : 
	       (node == SpairFst) ? BpairFst : 
	       (node == SpairSnd) ? BpairSnd : 
	       (node == SsymbolreferenceIndex) ? BsymbolreferenceIndex : 
	       (node == Ssymbolreference) ? Bsymbolreference : 
	       (node == SratioP) ? BratioP : 
	       (node == SratioA) ? BratioA : 
	       (node == SratioB) ? BratioB : 
	       (node == SparseError) ? BparseError : 
	       (node == SevalError) ? BevalError : 
	       (node == SerrorP) ? BerrorP : 
	       (node == SerrorKind) ? BerrorKind : 
	       (node == SerrorExpectedInput) ? BerrorExpectedInput : 
	       (node == SerrorGotInput) ? BerrorGotInput : 
	       (node == SerrorContext) ? BerrorContext : 
	       (node == Splusplus) ? Bconcat : 
	       (node == SstrFromList) ? BstrFromList : 
	       nil; /* FIXME error */
}
// TODO Modulesystem::memoize
//DEFINE_STRICT_FN(Builtins, dispatch(Modulesystem::exportsQ("Constanter rem memoize", SpecialForms::Constanter, SpecialForms::Identer, Modulesystem::Memoizer)))
DEFINE_STRICT_FN(Builtins, builtin(argument))
NodeT initBuiltins(void) {
	initStrings();
	SConstanter = symbolFromStr("Constanter");
	Srem = symbolFromStr("rem");
	Shashexports = symbolFromStr("#exports");
	Smemoize = symbolFromStr("memoize");
	SsymbolsEqP = symbolFromStr("symbolsEq?");
	SsymbolsLEP = symbolFromStr("symbolsLE?");
	SsymbolFromStr = symbolFromStr("symbolFromStr");
	SkeywordFromStr = symbolFromStr("keywordFromStr");
	Ssymbol1Name = symbolFromStr("symbol1Name");
	Skeyword1Name = symbolFromStr("keyword1Name");
	SnilP = symbolFromStr("nil?");
	Snil = symbolFromStr("nil");
	Scolon = symbolFromStr(":");
	SstrSize = symbolFromStr("strSize");
	Sfn = symbolFromStr("fn");
	Scall = symbolFromStr("call");
	Shead = symbolFromStr("head");
	Stail = symbolFromStr("tail");
	ScallCallable = symbolFromStr("callCallable");
	ScallArgument = symbolFromStr("callArgument");
	SfnParameter = symbolFromStr("fnParameter");
	SfnBody = symbolFromStr("fnBody");
	SsymbolP = symbolFromStr("symbol?");
	SkeywordP = symbolFromStr("keyword?");
	SfnP = symbolFromStr("fn?");
	ScallP = symbolFromStr("call?");
	SconsP = symbolFromStr("cons?");
	SboxP = symbolFromStr("box?");
	SstrP = symbolFromStr("str?");
	SintP = symbolFromStr("int?");
	SintegerP = symbolFromStr("integer?");
	SfloatP = symbolFromStr("float?");
	//SboxValue = symbolFromStr("boxValue");
	//SstrValue = symbolFromStr("strValue");
	Spair = symbolFromStr("pair");
	SpairFst = symbolFromStr("pairFst");
	SpairSnd = symbolFromStr("pairSnd");
	SsymbolreferenceIndex = symbolFromStr("symbolreferenceIndex");
	Ssymbolreference = symbolFromStr("symbolreference");
	SratioP = symbolFromStr("ratio?");
	SratioA = symbolFromStr("ratioA");
	SratioB = symbolFromStr("ratioB");
	SparseError = symbolFromStr("parseError");
	SevalError = symbolFromStr("evalError");
	SerrorP = symbolFromStr("error?");
	SerrorKind = symbolFromStr("errorKind");
	SerrorExpectedInput = symbolFromStr("errorExpectedInput");
	SerrorGotInput = symbolFromStr("errorGotInput");
	SerrorContext = symbolFromStr("errorContext");
	Splusplus = symbolFromStr("++");
	SstrFromList = symbolFromStr("strFromList");
	INIT_BINARY_FN(BsymbolsEqP)
	INIT_BINARY_FN(BsymbolsLEP)
	INIT_FFI_FN(BsymbolFromStr)
	INIT_FFI_FN(BkeywordFromStr)
	INIT_FFI_FN(Bsymbol1Name)
	INIT_FFI_FN(Bkeyword1Name)
	INIT_FFI_FN(BnilP)
	INIT_BINARY_FN(Bcolon)
	INIT_FFI_FN(BstrSize)
	INIT_BINARY_FN(Bfn)
	INIT_BINARY_FN(Bcall)
	INIT_FFI_FN(Bhead)
	INIT_FFI_FN(Btail)
	INIT_FFI_FN(BcallCallable)
	INIT_FFI_FN(BcallArgument)
	INIT_FFI_FN(BfnParameter)
	INIT_FFI_FN(BfnBody)
	INIT_FFI_FN(BsymbolP)
	INIT_FFI_FN(BkeywordP)
	INIT_FFI_FN(BfnP)
	INIT_FFI_FN(BcallP)
	INIT_FFI_FN(BconsP)
	INIT_FFI_FN(BboxP)
	INIT_FFI_FN(BstrP)
	INIT_FFI_FN(BintP)
	INIT_FFI_FN(BintegerP)
	INIT_FFI_FN(BfloatP)
	INIT_BINARY_FN(Bpair)
	INIT_FFI_FN(BpairFst)
	INIT_FFI_FN(BpairSnd)
	INIT_FFI_FN(BsymbolreferenceIndex)
	INIT_FFI_FN(Bsymbolreference)
	INIT_FFI_FN(BratioP)
	INIT_FFI_FN(BratioA)
	INIT_FFI_FN(BratioB)
	INIT_BINARY_FN(BparseError)
	INIT_FFI_FN(BerrorP)
	INIT_FFI_FN(BerrorKind)
	INIT_FFI_FN(BerrorExpectedInput)
	INIT_FFI_FN(BerrorGotInput)
	INIT_FFI_FN(BerrorContext)
	INIT_FFI_FN(Builtins)
	INIT_TERNARY_FN(BevalError)
	/* FIXME exports */
	return Builtins;
}
END_NAMESPACE_6D(Builtins)
