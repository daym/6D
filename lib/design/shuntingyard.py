#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import symbols
from symbols import intern
def parse(tokenizer, env):
	"""
	This works like the following:
		#operators is a stack of operators. It is always kept in order of ascending precedence. 
		(non-operator) values are just emitted.
		An opening parenthesis would just be put on the operator stack (without popping anything off), EVEN THOUGH it is recorded with the lowest precedence ever.
		An operator is handled like this: if we try to push a new operator with lower precedence on top, it will keep popping (and emitting) existing operators off the operator stack until the invariant is satisfied. Then the operator is pushed on the operator stack.
		A closing parenthesis is an "operator" which is not being put on the operator stack (it would do nothing anyway), otherwise handled like any other operator.
	"""
	OPL = env
	error = env(intern("error"))
	errorP = env(intern("error?"))
	operatorP = OPL(intern("operator?"))
	operatorArgcount = OPL(intern("operatorArgcount"))
	operatorPrefixNeutral = OPL(intern("operatorPrefixNeutral"))
	operators = [] # deferred operators
	operatorLE = OPL(intern("operator<="))
	openingParenP = OPL(intern("openingParen?"))
	closingParenP = OPL(intern("closingParen?"))
	openingParenOf = OPL(intern("openingParenOf"))
	macroStarterP = OPL(intern("macroStarter?"))
	argcount = 0
	# TODO check argcount and scream at the right moment if necessary
	# TODO support two-argument prefix operators ("if", "let", "import", "\\")
	# TODO in principle, there would also be non-associative operators like relational operators. We handle them as left-associative.
	prevInput = intern("(")
	def unaryP(input):
		argcount = operatorArgcount(input)
		if argcount > 1:
			neutral = operatorPrefixNeutral(input)
			return not errorP(neutral)
		else:
			return argcount == 1 or argcount == -1
	for input in tokenizer:
		if openingParenP(input):
			operators.append(input)
		elif operatorP(input):
			bUnary = False
			argcount = operatorArgcount(input)
			if operatorP(prevInput): # special-case prefix operators
				bUnary = True
				if argcount > 1:
					neutral = operatorPrefixNeutral(input)
					yield neutral
					if errorP(neutral):
						return
			elif argcount == -1:
				bUnary = True
			if macroStarterP(input):
				yield input
			# else (quasi-)binary, postfix
			bClosingParen = closingParenP(input)
			insideP = (lambda o, bUnary=bUnary: not openingParenP(o) and (not bUnary or unaryP(o))) if bClosingParen else (lambda o, bUnary=bUnary: not bUnary or unaryP(o))
			while len(operators) > 0 and insideP(operators[-1]): # keep in sync with below
				pendingOperator = operators[-1]
				if operatorLE(input, pendingOperator):
					yield operators.pop()
				else:
					break
			if bClosingParen: # TODO (5*)
				if len(operators) > 0 and (operators[-1] is openingParenOf(input)):
					operators.pop()
				else: # unbalanced paren, wrong paren
					yield error(openingParenOf(input), operators[-1].text if len(operators) > 0 else "<value>")
					return
			else:
				operators.append(input)
			argcount = operatorArgcount(input)
		else:
			if operatorP(prevInput): # normal value
				yield input
			else: # call, keep in sync with above.
				o = input
				yield input
				input = intern(" ")
				# binary
				insideP = lambda o: True
				while len(operators) > 0 and insideP(operators[-1]):
					pendingOperator = operators[-1]
					if operatorLE(input, pendingOperator):
						yield operators.pop()
					else:
						break
				operators.append(input)
				argcount = operatorArgcount(input)
				input = o
		prevInput = input if not closingParenP(input) else None
	while len(operators) > 0:
		yield operators.pop()
	# TODO check that in the end there is exactly one item on the result stack
if __name__ == "__main__":
	import scanner
	try:
		import StringIO as io
	except ImportError:
		import io
	def str1(val):
		if isinstance(val, str):
			return "%r" % val
		else:
			return str(val)
	def env(name):
		level = {
			intern("("): -1,
			intern("{"): -1,
			intern("["): -1,
			intern("<indent>"): -1,
			intern("."): 33,
			intern("_"): 32,
			intern("^"): 32,
			intern("!"): 31,
			intern("**"): 30,
			intern("*"): 29,
			intern("⋅"): 29,
			intern("/"): 29,
			intern("⨯"): 28,
			intern(":"): 27,
			intern("'"): 26,
			intern(" "): 24,
			intern("++"): 23,
			intern("+"): 22,
			intern("‒"): 22,
			intern("-"): 22,
			intern("%"): 21,
			intern("∩"): 16,
			intern("∪"): 15,
			intern("∈"): 14,
			intern("⊂"): 14,
			intern("⊃"): 14,
			intern("⊆"): 14,
			intern("⊇"): 14,
			intern("="): 10,
			intern("≟"): 10,
			intern("/="): 10,
			intern("<"): 9,
			intern("<="): 9,
			intern(">"): 9,
			intern(">="): 9,
			intern("≤"): 9,
			intern("≥"): 9,
			intern("&&"): 8,
			intern("∧"): 8,
			intern("||"): 7,
			intern("∨"): 7,
			intern(","): 6,
			intern("$"): 5,
			#intern("if"): 4,
			intern("elif"): 4,
			intern("else"): 4,
			intern("|"): 3,
			intern("=>"): 2,
			intern(";"): 2,
			intern("?;"): 2,
			intern("\\"): 1,
			intern("let"): 0,
			intern("let!"): 0,
			intern("import"): 0,
			intern(")"): -1,
			intern("}"): -1,
			intern("]"): -1,
			intern("<dedent>"): -1,
		}
		def errorP(node):
			return isinstance(node, tuple) and len(node) > 0 and node[0] is intern("error")
		def operatorPrefixNeutral(node):
			return intern("0") if node is intern("-") else \
			       env(intern("error"))("<prefix-operator>", str1(node))
		def operatorP(node):
			return node in level.keys() # is intern("+") or node is intern("-") or node is intern("*") or node is intern("/")
		def macroStarterP(node):
			return node is intern("let") or node is intern("import") or node is intern("\\")
		def operatorArgcount(node): # positive, > 1: left assoc, negative: right assoc. = 1: prefix, = -1: suffix
			R = -2
			N = 2 # FIXME
			P = 1
			S = -1
			return R if node is intern("_") else \
			       R if node is intern("^") else \
			       R if node is intern("**") else \
			       R if node is intern("⨯") else \
			       R if node is intern(":") else \
			       P if node is intern("'") else \
			       N if node is intern("∈") else \
			       N if node is intern("⊂") else \
			       N if node is intern("⊃") else \
			       N if node is intern("⊆") else \
			       N if node is intern("⊇") else \
			       N if node is intern("=") else \
			       N if node is intern("≟") else \
			       N if node is intern("/=") else \
			       N if node is intern("<") else \
			       N if node is intern("<=") else \
			       N if node is intern(">") else \
			       N if node is intern(">=") else \
			       N if node is intern("≤") else \
			       N if node is intern("≥") else \
			       R if node is intern(",") else \
			       R if node is intern("$") else \
			       R if node is intern("elif") else \
			       R if node is intern("else") else \
			       R if node is intern(";") else \
			       P if node is intern("\\") else \
			       P if node is intern("let") else \
			       P if node is intern("let!") else \
			       P if node is intern("import") else \
			       S if node is intern("!") else \
		               P if macroStarterP(node) else \
			       2
		def operatorLE(a,b):
			return level[a] < level[b] or (level[a] == level[b] and operatorArgcount(b) > 0) # latter: leave right-associative operators on stack if in doubt.
		def openingParenP(node):
			return node is intern("(") or node is intern("{") or node is intern("[") or node is intern("<indent>")
		def closingParenP(node):
			return node is intern(")") or node is intern("}") or node is intern("]") or node is intern("<dedent>")
		def openingParenOf(node):
			return intern("(") if node is intern(")") else intern("{") if node is intern("}") else intern("[") if node is intern("]") else intern("<indent>") if node is intern("<dedent>") else intern("<none>")
		#def closingParenOfP(node):
		def shebang(v):
			return None
		def stringFromSymbol(v):
			return v.text
		if name == intern("operator?"):
			return operatorP
		elif name == intern("operatorArgcount"):
			return operatorArgcount
		elif name == intern("operator<="):
			return operatorLE
		elif name == intern("openingParen?"):
			return openingParenP
		elif name == intern("closingParen?"):
			return closingParenP
		elif name == intern("openingParenOf"):
			return openingParenOf
		elif name == intern("operatorPrefixNeutral"):
			return operatorPrefixNeutral
		elif name == intern("#!"):
			return shebang
		elif name == intern("stringFromSymbol"):
			return stringFromSymbol
		elif name == intern("#"):
			def readHash(inputFile, c):
				if c == 'e': # exports, probably.
					if inputFile.read(len("xports")) == "xports":
						#if inputFile.read(1) == '[':
						print("FIXME run the entire parser")
						return None, inputFile.read(1)
				elif c == 't':
					return intern("#t"), inputFile.read(1)
				elif c == 'f':
					return intern("#f"), inputFile.read(1)
				return (env(intern("error"))("<value>"), "")
			return readHash
		elif name is intern("error"):
			return lambda *args: (intern("error"), "expected \"%s\", got \"%s\"" % (args[0], args[1]))
		elif name is intern("error?"):
			return errorP
		elif name is intern("macroStarter?"):
			return macroStarterP
		else:
			#return env(intern("error"))("<envitem>", name)
			return lambda *args: (env(intern("error"))("<envitem>", name), "")
	print("---")
	errorP = env(intern("error?"))
	error = env(intern("error"))
	operatorP = env(intern("operator?"))
	def test1(text, expected):
		got = [x for x in parse(scanner.tokenize(io.StringIO(text), env), env)]
		print(text, got, [x for x in map(intern, expected)])
		assert got == [x for x in map(intern, expected)]
	def test1Error(text, expected):
		f = io.StringIO(text)
		got = [x for x in parse(scanner.tokenize(f, env), env) if errorP(x)]
		assert(len(got) > 0)
		print("positionx", f.tell(), got[-1], errorP(got[-1]))
	def testEval(text, expected):
		program = [x for x in parse(scanner.tokenize(io.StringIO(text), env), env)]
		def operation(name):
			def add():
				b = data.pop()
				a = data.pop()
				data.append(a + b)
			def subtract():
				b = data.pop()
				a = data.pop()
				data.append(a - b)
			def multiply():
				b = data.pop()
				a = data.pop()
				data.append(a*b)
			def divide():
				b = data.pop()
				a = data.pop()
				data.append(a/b)
			if name is intern("+"):
				return add
			elif name is intern("-"):
				return subtract
			elif name is intern("*"):
				return multiply
			elif name is intern("/"):
				return divide
			else:
				return lambda *args: error("<operation>", name)
		data = []
		for item in program:
			if operatorP(item):
				operation(item)()
			else:
				data.append(eval(str1(item))) # ugh.
		assert(len(data) == 1)
		assert(data[0] == expected)
	test1("2+3", ["2", "3", "+"])
	test1("2+3*5", ["2", "3", "5", "*", "+"])
	test1("2*3+5", ["2", "3", "*", "5", "+"])
	test1("2*(3+5)", ["2", "3", "5", "+", "*"])
	test1("2*(3+(5+4)+2)", ["2", "3", "5", "4", "+", "+", "2", "+", "*"])
	test1("2*3*4+5", ["2", "3", "*", "4", "*", "5", "+"])
	test1("2*3*4+5-10/3", ["2", "3", "*", "4", "*", "5", "+", "10", "3", "/", "-"])
	test1("-3", ["0", "3", "-"])
	test1("2** -3", ["2", "0", "3", "-", "**"])
	test1("let x = 5", ["let", "x", "5", "=", "let"])
	test1("\\x 5", ["\\", "x", "5", " ", "\\"])
	test1("(\\x 5) 3", ["\\", "x", "5", " ", "\\", "3", " "])
	test1("(\\(x) 5) 3", ["\\", "x", "5", " ", "\\", "3", " "])
	test1("f x", ["f", "x", " "])
	test1("f x y", ["f", "x", "y", " ", " "])
	test1("3 + f x y", ["3", "f", "x", "y", " ", " ", "+"])
	test1("5!", ["5", "!"])
	test1("foo (+)", ["foo", "-", " "])
	testEval("3+5", 8)
	testEval("3+5*3", 18)
	testEval("(3+5)*3", 24)
	test1Error(")", [])
	#test1Error("3*", [])
	#inputFile = io.StringIO("2+2")
	co = 31
	inputFile = open(sys.argv[1], "r")
	for val in scanner.tokenize(inputFile, env):
		sys.stdout.write("\033[%dm" % co)
		co = 32 if co == 31 else 31
		sys.stdout.write(str1(val) + " ")
		if(str(val) == "<LF>"):
			sys.stdout.write("\n")
	sys.stdout.write("\033[m")
	inputFile.seek(0)
	for val in parse(scanner.tokenize(inputFile, env), env):
		sys.stdout.write("%s " % (val, ))
