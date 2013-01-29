#!/usr/bin/env python
import sys
import symbols
from symbols import intern
def parse(tokenizer, OPL, env):
	error = env(intern("error"))
	operatorP = OPL(intern("operator?"))
	operatorArgcount = OPL(intern("operatorArgcount"))
	operators = [] # deferred operators
	operatorLE = OPL(intern("operator<="))
	openingParenP = OPL(intern("openingParen?"))
	closingParenP = OPL(intern("closingParen?"))
	openingParenOf = OPL(intern("openingParenOf"))
	argcount = 0
	# TODO check argcount and scream at the right moment if necessary
	# TODO support two-argument prefix operators ("if", "let", "import", "\\")
	for input in tokenizer:
		# TODO right-associative operators, suffix operators, prefix operators
		if operatorP(input):
			if not openingParenP(input):
				bClosingParen = closingParenP(input)
				insideP = (lambda o: not openingParenP(o)) if bClosingParen else (lambda o: True)
				while len(operators) > 0 and insideP(operators[-1]):
					pendingOperator = operators[-1]
					if operatorLE(input, pendingOperator):
						yield operators.pop()
					else:
						break
				if bClosingParen:
					if len(operators) > 0 and (operators[-1] is openingParenOf(input)):
						operators.pop()
					else: # unbalanced paren, wrong paren
						yield error(openingParenOf(input), operators[-1].text if len(operators) > 0 else "<value>")
						return
				else:
					operators.append(input)
			else:
					operators.append(input)
			argcount = operatorArgcount(input)
		else:
			yield input
	while len(operators) > 0:
		yield operators.pop()

if __name__ == "__main__":
	import scanner
	import StringIO as io
	#inputFile = io.StringIO("2+2")
	inputFile = open(sys.argv[1], "r")
	co = 31
	def str1(val):
		if isinstance(val, str):
			return "%r" % val
		else:
			return str(val)
	def OPL(name):
		level = {
			intern("("): -1,
			intern("{"): -1,
			intern("["): -1,
			intern("+"): 22,
			intern("-"): 22,
			intern("*"): 29,
			intern("/"): 29,
			intern(")"): -1,
			intern("}"): -1,
			intern("]"): -1,
		}
		def operatorP(node):
			return node in level.keys() # is intern("+") or node is intern("-") or node is intern("*") or node is intern("/")
		def operatorArgcount(node): # positive, > 1: left assoc, negative: right assoc. = 1: prefix, = -1: suffix
			return 2
		def operatorLE(a,b):
			return level[a] <= level[b]
		def openingParenP(node):
			return node is intern("(") or node is intern("{") or node is intern("[")
		def closingParenP(node):
			return node is intern(")") or node is intern("}") or node is intern("]")
		def openingParenOf(node):
			return intern("(") if node is intern(")") else intern("{") if node is intern("}") else intern("[") if node is intern("]") else intern("<none>")
		#def closingParenOfP(node):
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
		else:
			return env(intern("error"))("<envitem>", name)
	def env(name):
		def shebang(v):
			return None
		def stringFromSymbol(v):
			return v.text
		if name == intern("#!"):
			return shebang
		elif name == intern("stringFromSymbol"):
			return stringFromSymbol
		elif name == intern("#"):
			def readHash(inputFile, c):
				if c == 'e': # exports, probably.
					if inputFile.read(len("xports")) == "xports":
						#if inputFile.read(1) == '[':
						print "FIXME run the entire parser"
						return None, inputFile.read(1)
				elif c == 't':
					return intern("#t"), inputFile.read(1)
				elif c == 'f':
					return intern("#f"), inputFile.read(1)
				return (env(intern("error"))("<value>"), "")
			return readHash
		elif name == intern("error"):
			return lambda *args: (intern("error"), "expected \"%s\", got \"%s\"" % (args[0], args[1]))
		else:
			return lambda *args: (env(intern("error"))("<envitem>", name), "")
	for val in scanner.tokenize(inputFile, env):
		sys.stdout.write("\033[%dm" % co)
		co = 32 if co == 31 else 31
		sys.stdout.write(str1(val) + " ")
		if(str(val) == "<LF>"):
			sys.stdout.write("\n")
	sys.stdout.write("\033[m")
	inputFile.seek(0)
	print "---"
	def errorP(node):
		return isinstance(node, tuple) and len(node) > 0 and node[0] is intern("error")
	def test1(text, expected):
		got = [x for x in parse(scanner.tokenize(io.StringIO(text), env), OPL, env)]
		print text, got
		assert got == map(intern, expected)
	def test1Error(text, expected):
		f = io.StringIO(text)
		got = [x for x in parse(scanner.tokenize(f, env), OPL, env) if errorP(x)]
		assert(len(got) > 0)
		print "position", f.tell(), got[-1]
	test1("2+3", ["2", "3", "+"])
	test1("2+3*5", ["2", "3", "5", "*", "+"])
	test1("2*3+5", ["2", "3", "*", "5", "+"])
	test1("2*(3+5)", ["2", "3", "5", "+", "*"])
	test1("2*3*4+5", ["2", "3", "*", "4", "*", "5", "+"])
	test1("2*3*4+5-10/3", ["2", "3", "*", "4", "*", "5", "+", "10", "3", "/", "-"])
	test1Error(")", [])
	for val in parse(scanner.tokenize(inputFile, env), OPL, env):
		print val,

