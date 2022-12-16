/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Scanners/Scanner"
#include <stdio.h>
#include <assert.h>
#include "6D/Values"
#include "6D/FFIs"
#include "Values/Values"

BEGIN_NAMESPACE_6D(Scanners)
#undef GETC
#undef UNGETC
#define GETC fgetc(self->file)
#define UNGETC(c) ungetc(c, self->file)
BEGIN_STRUCT_6D(Scanner)
	NODET value;
	FILE* file;
	const char* name;
	int linenumber;
	bool bBeginningOfLine;
	NodeT env;
	NODET indents;
	NativeUInt indent; /* of current line */
	NODET SEOF;
	bool bEOF;
END_STRUCT_6D(Scanner)
	static INLINE int integerCompareD(NODET a, NATIVEINT bvalue) {
		NATIVEINT avalue = 0;
		if(!toNativeInt(a, &avalue))
			abort();
		return (avalue < bvalue) ? (-1) :
		       (avalue == bvalue) ? 0 :
		       1;
	}
	/** \return whether we already injected a token or not. */
	static bool Scanner_skipWhitespace(struct Scanner* self) {
		int c;
		if(!self->bEOF)
		while((c = GETC) != EOF && c != 26/*EOF*/) {
			if(c == ' ' || c == '\t') {
				if(self->bBeginningOfLine) {
					self->indent += 1;
					if(self->indent == 0) { /* overflow */
						abort();
					}
				}
			} else if(c == '\r') {
			} else if(c == '\n') {
				self->bBeginningOfLine = true;
				self->indent = 0;
				//self->value = env.SLF;
				++self->linenumber;
				//return true;
			} else if(self->bBeginningOfLine) { /* actual text for the first time on the line */
				UNGETC(c); /* make sure the user will see the actual text */
				self->bBeginningOfLine = false;
				int status = integerCompareD(consHead(self->indents), self->indent);
				if(status < 0) {
					self->indents = cons(internNativeInt(self->indent), self->indents);
					self->value = env_Sindent(self->env);
					return true;
				} else if(status > 0) {
					int diff;
					self->value = env_Sdedent(self->env);
					self->indents = consTail(self->indents); /* decrease indentation by ONE level */
					assert(!nilP(self->indents)); /* 0 will never be removed */
					diff = integerCompareD(consHead(self->indents), self->indent);
					if(diff < 0) { /* self indentation does not exist - it would have been in-between */
						self->value = env_error(self->env, "<known-indentation>", "<unknown-indentation>");
						return true;
					} else if(diff > 0) { /* there are others to be closed (will be done at the next iteration). */
						self->bBeginningOfLine = true; /* make sure to check at the next iteration, too */
						return true;
					} else { /* self one is the right one */
						return true;
					}
				}
				return false;
			} else {
				UNGETC(c); /* make sure the user will see the actual text */
				return false;
			}
		}
		self->bEOF = true;
		return false;
	}
	void Scanner_init(struct Scanner* self, NodeT aEnv) {
		initIntegers();
		self->value = nil;
		self->name = NULL;
		self->linenumber = 1;
		self->bBeginningOfLine = true;
		self->env = aEnv;
		self->indent = 0;
		self->indents = cons(internNativeInt((NativeInt) 0), nil);
		self->SEOF = env_SEOF(self->env);
		self->bEOF = false;
	}
	void Scanner_push(struct Scanner* self, FILE* file, int linenumber, const char* name) {
		/* TODO support "include" files? */
		self->file = file;
		self->linenumber = linenumber;
		self->name = name;
	}
	void Scanner_pop(struct Scanner* self) {
	}
        size_t Scanner_getSize(void) {
		return sizeof(struct Scanner);
	}
	NODET Scanner_consume(struct Scanner* self) {
		NODET previousValue = self->value;
		if(!Scanner_skipWhitespace(self)) {
			if(self->bEOF) { /* TODO unlikely */
				if(self->indents && consTail(self->indents)) {
					self->indents = consTail(self->indents);
					self->value = env_Sdedent(self->env);
				} else
					self->value = env_SEOF(self->env);
			} else {
				/* one of the less nice things is that self will not report back that it found EOF so we will try to read again. */
				self->value = env_readToken(self->env, self->file, &self->linenumber);
			}
		}
		return previousValue;
	}
	/*NODET consume1(NODET expectedToken) {
		if(self->value != expectedToken)
			return env.error(symbolName(expectedToken), symbolName(self->value));
		else
			return consume(expectedToken);
	}*/
	NODET Scanner_getToken(const struct Scanner* self) {
		return self->value;
	}
	int Scanner_getLinenumber(const struct Scanner* self) {
		return self->linenumber;
	}

END_NAMESPACE_6D(Scanners)
