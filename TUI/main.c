#include <stdio.h>
#include <editline/readline.h>
#include "6D/Allocators"
#include "6D/Values"
#include "6D/Evaluators"
#include "6D/Operations"
#include "6D/Lang5D"
#include "6D/Arithmetic"
#include "6D/Builtins"
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Evaluators)
USE_NAMESPACE_6D(Numbers)
USE_NAMESPACE_6D(Allocators)
USE_NAMESPACE_6D(Parsers)
USE_NAMESPACE_6D(Formatters::TExpression)
USE_NAMESPACE_6D(Arithmetic)
USE_NAMESPACE_6D(Strings)
void printPrompt(void) {
	fprintf(stderr, "eval $ ");
	fflush(stderr);
}
static NodeT builtins;
static NodeT input;
static NodeT Sexports;
static NodeT reflect(NodeT accessor, NodeT newTail) {
	if(!accessor)
		return newTail;
	NodeT exports = eval(annotate(nil, call(accessor, Sexports)));
	return concat(exports, newTail);
}
static NodeT getBoundNames(const char* text, int point) {
	// TODO check where exactly you are.
	//return cons(symbolFromStr("abba"), cons(symbolFromStr("beta"), nil));
	return reflect(builtins, nil);
}
static char* command_generator(const char* text, int state) {
	static NodeT boundNames;
	static int len;
	if(state == 0) { /* restart global */
		boundNames = getBoundNames(text, rl_point);
		len = strlen(text);
		//return strdup("A");
	}
	if(!boundNames)
		return NULL;
	while(consP(boundNames)) {
		NodeT key = getConsHead(boundNames);
		const char* ktext = getSymbol1Name(key);
		boundNames = getConsTail(boundNames);
		if(ktext && strncmp(ktext, text, len) == 0)
			return strdup(ktext);
	}
	/* TODO search wrapper envs (exports), then parsed text as far as possible */
	return(NULL);
}
static char** complete(const char* text, int start, int end) {
	char** matches = NULL;
	//if(start == 0) // or after a brace.
		matches = completion_matches(text, command_generator);
	return(matches);
}
int main() {
	const char* s;
	//char buffer[2049];
	rl_initialize();
	initAllocators();
	initArithmetic();
	NodeT defaultDynEnv = initLang();
	initEvaluator();
	builtins = initBuiltins();
	Sexports = symbolFromStr("exports");
	rl_readline_name = "6D";
	rl_attempted_completion_function = complete;
	rl_bind_key('\t',rl_complete);
	//Values::NodeT annotate(Values::NodeT environment, Values::NodeT node);
	//Values::NodeT eval(Values::NodeT node);
	//Values::NodeT execute(Values::NodeT term);
	while(s = readline("eval $ ")) {
		//(printPrompt(), s = fgets(buffer, 2048, stdin))) { /* TODO handle longer lines */
		NodeT prog;
		if(!s[0] || (s[0] == '\n' && !s[1]))
			continue;
		add_history(s);
		FILE* f = fmemopen((char*) s, strlen(s), "r");
		input = Lang_parse1(f, "<stdin>");
		fclose(f);
		input = closeOver(symbolFromStr("Builtins"), builtins, withArithmetic(Lang_withDefaultEnv(input)));
		//print(stderr, prog);
		//fprintf(stderr, "\n");
		//fflush(stderr);
		prog = annotate(defaultDynEnv, input);
		prog = eval(prog);
		print(stdout, prog);
		printf("\n");
		fflush(stdout);
		increaseGeneration();
	}
	return 0;
}
