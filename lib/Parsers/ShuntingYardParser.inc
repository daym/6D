#include "Parsers/ShuntingYardParser"
/* BEGIN env interface */
static void* unbox(NodeT n) {
	void* result;
	if(!pointerFromNode(n, &result))
		abort();
	return result;
}
static INLINE int env_operatorArgcount(NodeT env, NodeT operator_) { return Lang_operatorArgcount(unbox(env), operator_); }
static INLINE NodeT env_operatorPrefixNeutral(NodeT env, NodeT operator_) { return Lang_operatorPrefixNeutral(unbox(env), operator_); }
static INLINE bool env_errorP(NodeT env, NodeT node) { return errorP(node); }
static INLINE bool env_closingParenP(NodeT env, NodeT node) { return Lang_closingParenP(unbox(env), node); }
static INLINE bool env_openingParenP(NodeT env, NodeT node) { return Lang_openingParenP(unbox(env), node); }
static INLINE bool env_operatorP(NodeT env, NodeT node) { return Lang_operatorP(unbox(env), node); }
static INLINE bool env_operatorLE(NodeT env, NodeT a, NodeT b) { return Lang_operatorLE(unbox(env), a, b); }
static INLINE int env_callRpnOperator(NodeT env, NodeT operator_, MNODET* result) { return Lang_callRpnOperator(unbox(env), operator_, result); }
static INLINE NodeT env_openingParenOf(NodeT env, NodeT operator_) { return Lang_openingParenOf(unbox(env), operator_); }
static INLINE NodeT env_error(NodeT env, const char* expected, const char* got) { return Lang_error(unbox(env), expected, got); }
static INLINE bool env_macroStarterP(NodeT env, NodeT operator_) { return Lang_macroStarterP(unbox(env), operator_); }
static INLINE NodeT env_startMacro(NodeT env, NodeT node, struct Scanner* tokenizer) { return Lang_startMacro(unbox(env), node, tokenizer); }
static INLINE NodeT env_SopeningParen(NodeT env) { return SopeningParen; }
static INLINE NodeT env_SEOF(NodeT env) { return SEOF; }
static INLINE NodeT env_Sapply(NodeT env) { return Sapply; }
static INLINE NodeT env_Sleftparen(NodeT env) { return Sleftparen; }
static INLINE NodeT env_Srightparen(NodeT env) { return Srightparen; }
static INLINE NodeT env_Sindent(NodeT env) { return Sindent; }
static INLINE NodeT env_Sdedent(NodeT env) { return Sdedent; }
static INLINE NodeT env_readToken(NodeT env, FILE* file, int* linenumber) { return Lang_readToken(unbox(env), file, linenumber); }
/* END env interface */

#include "Scanners/Scanner.inc"
BEGIN_NAMESPACE_6D(Parsers)
USE_NAMESPACE_6D(Values)


/* poor man's closure: */
struct Parser GC_BASECLASS {
	NodeT result; /* not actually a stack? */
	NodeT operators; /* stack */
	struct Scanner* tokenizer;
	NodeT env;
	int executionResultCount;
	NodeT prevInput;
};
bool Parser_unaryP(struct Parser* self, NODET input) {
	NodeT env = self->env;
	int argcount = env_operatorArgcount(env, input);
	if (argcount > 1) {
		NODET neutral = env_operatorPrefixNeutral(env, input);
		return !env_errorP(env, neutral);
	} else
		return (argcount == 1 || argcount == -1);
}
NODET Parser_applyAndPush(struct Parser* self, int argcount, bool bUnary, NODET input) {
	/* Note: there is no case for binary suffix operators abused as unary operator. */
	NodeT env = self->env;
	bool bClosingParen = env_closingParenP(env, input);
	NodeT pendingOperator;
	while (!nilP(self->operators) && (pendingOperator = consHead(self->operators), (!env_openingParenP(env, pendingOperator) && (!bUnary || Parser_unaryP(self, pendingOperator))))) { // keep in sync with below.
		if (env_operatorLE(env, input, pendingOperator)) {
			self->executionResultCount += env_callRpnOperator(env, pendingOperator, &self->result);
			self->operators = consTail(self->operators);
		} else
			break;
	}
	if (bClosingParen) {
		if ((!nilP(self->operators) && consHead(self->operators) == env_openingParenOf(env, input)))
			self->operators = consTail(self->operators);
		else {
			input = /*yield */ env_error(env, symbolName(env_openingParenOf(env, input)), (!nilP(self->operators)) ? (symbolName(consHead(self->operators))) : ("<value>"));
			return input;
		}
	} else if(argcount == -1) { /* suffix */
		self->executionResultCount += env_callRpnOperator(env, input, &self->result);
		input = self->prevInput; /* make sure we don't see the suffix operator there on its own (since it was already applied) */
	} else 
		self->operators = cons(input, self->operators);
	if (env_macroStarterP(env, input)) { /* assumes macros all start at prefix loc */
		self->result = cons(/*yield */ env_startMacro(env, input, self->tokenizer), self->result);
		// FIXME just for '\\'
		//input = nil; /* for the purposes of operator detection, the value was not an operator. Note that we cannot write result.back() or similar here since otherwise macro standins could be misdetected. */
		++self->executionResultCount;
	}
	return input;
}
static INLINE void Parser_pushOperand(struct Parser* self, NODET input) {
	self->result = cons(input, self->result);
	++self->executionResultCount;
}
void Parser_init(struct Parser* self, struct Scanner* aTokenizer, NodeT aEnv) {
	self->tokenizer = aTokenizer;
	self->env = aEnv;
	self->operators = nil;
	self->result = nil;
	self->executionResultCount = 0;
	self->prevInput = nil;
}
#define CHECK(x) { \
	x; \
	if(env_errorP(env, self->prevInput)) \
		return self->prevInput; \
}
NodeT Parser_parse0(struct Parser* self, NODET endToken) {
	//"\n\tThis works like the following:\n\t\t#operators is a stack of operators. It is always kept in order of ascending precedence. \n\t\t(non-operator) values are just emitted.\n\t\tAn opening parenthesis would just be put on the operator stack (without popping anything off), EVEN THOUGH it is recorded with the lowest precedence ever.\n\t\tAn operator is handled like this: if we try to push a new operator with lower precedence on top, it will keep popping (and emitting) existing operators off the operator stack until the invariant is satisfied. Then the operator is pushed on the operator stack.\n\t\tA closing parenthesis is an " operator     " which is not being put on the operator stack (it would do nothing anyway), otherwise handled like any other operator.\n\t";
	NodeT input;
	NodeT env;
	int argcount;
	env = self->env;
	self->prevInput = env_SopeningParen(env); // intern("(");
	NodeT SEOF = env_SEOF(env);
	// TODO maybe don't auto-apply on any kind of closing paren.
	while((input = Scanner_getToken(self->tokenizer)), !env_errorP(env, input) && input != SEOF) {
		bool bOstensibleEnd = input == endToken;
		if(bOstensibleEnd && nilP(self->operators))
			break;
		Scanner_consume(self->tokenizer);
		if (env_openingParenP(env, input)) { /* special case of the stuff below in order to declutter the stuff below */
			if((!env_operatorP(env, self->prevInput) || env_closingParenP(env, self->prevInput))) { /* implicit application before unary operator */
				CHECK(self->prevInput = Parser_applyAndPush(self, 2, false, env_Sapply(env)));
			}
			self->operators = cons(input, self->operators);
			if (env_macroStarterP(env, input)) {
				Parser_pushOperand(self, /*yield */ env_startMacro(env, input, self->tokenizer)); /* this is the final result for the macro, probably */
				//input = nil; /* for the purposes of operator detection, the value was not an operator. Note that we cannot write result.back() or similar here since otherwise macro standins could be misdetected. */
			}
		} else if(self->prevInput == env_Sleftparen(env) && Scanner_getToken(self->tokenizer) == env_Srightparen(env) && env_operatorP(env, input)) { /* (+) */
			Parser_pushOperand(self, input);
			CHECK(self->prevInput = Parser_applyAndPush(self, 0, false, Scanner_consume(self->tokenizer)));
			input = nil; // not an operator and not first a ) and then a prefix operator
			if(env_Srightparen(env) == endToken && nilP(self->operators))
				break;
		} else if(env_operatorP(env, input)) {
			bool bUnary = false;
			argcount = env_operatorArgcount(env, input);
			if(argcount == 1 && (!env_operatorP(env, self->prevInput) || env_closingParenP(env, self->prevInput))) { /* implicit application before unary operator */
				CHECK(self->prevInput = Parser_applyAndPush(self, 2, false, env_Sapply(env)));
			}
			if (env_operatorP(env, self->prevInput) && (!env_closingParenP(env, self->prevInput) || Parser_unaryP(self, input)) && !env_closingParenP(env, input)) { /* either prefix operator or binary operator that has been abused as an unary operator (f.e. -3). Parens are not unary. */
				bUnary = true;
				if (argcount > 1) { /* binary operator has been abused as an unary operator */
					NODET neutral = env_operatorPrefixNeutral(env, input);
					Parser_pushOperand(self, /* yield */ neutral);
					if (env_errorP(env, neutral)) { /* binary operator is not allowed to be abused as an unary operator */
						return self->result;
					}
				}
			} else if (argcount == -1) { /* unary suffix operator */
				bUnary = true;
			}
			input = Parser_applyAndPush(self, argcount, bUnary, input);
			if(env_errorP(env, input))
				return self->result;
		} else { /* current token not an operator: might be either a value or a function call argument. In both cases it will be food for some pending operator. */
			if (!env_operatorP(env, self->prevInput) || env_closingParenP(env, self->prevInput)) { /* function call */
				CHECK(self->prevInput = Parser_applyAndPush(self, 2, false, env_Sapply(env)));
			}
			Parser_pushOperand(self, /*yield */ input);
		}
		self->prevInput = input; // (!env_closingParenP(env, input)) ? (input) : nil;
		if(bOstensibleEnd && nilP(self->operators))
			break;
	}
	while (!nilP(self->operators)) {
		self->executionResultCount += env_callRpnOperator(env, consHead(self->operators), &self->result);
		self->operators = consTail(self->operators);
	}
	return self->result;
}
NodeT Parser_parse(struct Scanner* tokenizer, NodeT env, NODET endToken) {
	struct Parser parser;
	Parser_init(&parser, tokenizer, env);
	return Parser_parse0(&parser, endToken);
}
END_NAMESPACE_6D(Parsers)
