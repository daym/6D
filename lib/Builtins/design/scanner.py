#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import symbols
from symbols import intern
try:
	import StringIO as io
except ImportError:
	import io
mathUnicodeRanges = [
	(0x2200, 0x2300),
	(0x27C0, 0x27F0),
	(0x2980, 0x2A00),
	(0x2A00, 0x2B00),
	(0x2100, 0x2150),
	#(0x2308, 0x230C),
	(0x25A0, 0x2600),
	(0x2B30, 0x2B4D),
	#(0x1D400, 0x1D800),
]
"""
>>> unichr(0x800).encode("utf-8")
'\xe0\xa0\x80'
>>> unichr(0x00).encode("utf-8")
'\x00'
>>> unichr(0x900).encode("utf-8")
'\xe0\xa4\x80'
>>> unichr(0x1000).encode("utf-8")
'\xe1\x80\x80'
>>> unichr(0x4000).encode("utf-8")
'\xe4\x80\x80'
>>> unichr(0x9000).encode("utf-8")
'\xe9\x80\x80'
>>> unichr(0x10000).encode("utf-8")
'\xf0\x90\x80\x80'
"""
def mathUnicodeOperatorInRangeP(codepoint):
	for s, e in mathUnicodeRanges:
		if codepoint >= s and codepoint < e:
			return True
	return False
def UNISKIP(codepoint):
	return 1 if codepoint < 0x80 else \
	       2 if codepoint < 0x800 else \
	       3 if codepoint < 0x10000 else \
	       4
for s,e in mathUnicodeRanges:
	assert(UNISKIP(s) == UNISKIP(e))
	assert(UNISKIP(s) == 3)

manydigits = "0123456789abcdefghijklmnopqrstuvwxyz"

# TODO special-case UTF-8 math operators (that they end after the operator char and optional trailing compositing chars)

# FIXME parse "b⋅" properly
def digitCharP(input):
	return input and input in "0123456789"
def digitRestCharP(input):
	return input and input in "0123456789." # TODO E+-
def specialCodingCharP(input):
	return input == '#'
def asciiIDCharP(input):
	return input and input in "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ?!"
def asciiID2CharP(input):
	return asciiIDCharP(input) or digitCharP(input)
def unicodeIDCharP(input):
	return asciiIDCharP(input) or (ord(input) >= 0x80 and ord(input) != 0xE2) # FIXME don't disallow all U+2xxx
def unicodeIDRestCharP(input):
	return asciiIDCharP(input) or digitCharP(input) or (ord(input) >= 0x80 and ord(input) != 0xE2) # FIXME don't disallow all U+2xxx
def unicodeMaybeOperator1CharP(input):
	return ord(input) == 0xE2 # U+2xxx   # TODO 0x1D400 once Python supports it.
def operatorCharP(input):
	return input and input in "$%&*+,-./:;<=>@\\^_`|~"
def braceCharP(input):
	return input and input in "()[]{}"
def shebangBodyCharP(input):
	return input and input != '\n'
def octalBodyCharP(input):
	return input and input in "01234567"
def hexadecimalBodyCharP(input):
	return input and input.lower() in "0123456789abcdef"
def binaryBodyCharP(input):
	return input and input in "01"
def stringBodyCharP(input):
	return input and input != '"' # FIXME escape
def raryBodyCharP(digits):
	return lambda input: input and input.lower() in digits
def tokenize(inputFile, env):
	error = env(intern("error"))
	def collect(c, condition, text = ""):
		result = io.StringIO(text)
		while True:
			if c == "":
				break # TODO maybe signal some error
			else:
				if condition(c):
					result.write(c)
					c = inputFile.read(1)
				else:
					break
		v = result.getvalue()
		if len(v) > 0:
			return intern(v), c
		else:
			return error("<value>"), c
	def collect1(condition):
		c = inputFile.read(1)
		return collect(c, condition)
	def collectUnicodeID(c, text):
		""" it is assumed that text is in entire unicode characters, i.e. not incomplete. """
		result = io.StringIO(text)
		codepoint = 0
		while True:
			if c == "":
				break # TODO maybe signal some error
			else:
				# < 0xC0 continuing byte
				# >= 0xE0 first byte
				# < 0x80 first and only byte
				#FIXME then use mathUnicodeOperatorInRangeP to check. What to do if it is? We just overread a lot of bytes and are probably in the middle of a normal identifier parsing step, too...
				if unicodeIDRestCharP(c):
					result.write(c)
					c = inputFile.read(1)
				else:
					break
		v = result.getvalue()
		if len(v) > 0:
			return intern(v), c
		else:
			return error("<value>"), c
	def skipWhitespace(c, indents):
		""" it is assumed that this function sees all the \\n that concern it. Only after seeing a \\n, it will fiddle with indentation at all. """
		indent = None # indents[-1]
		while True:
			if c == ' ' or c == '\t':
				if c == '\t':
					if indent is None:
						indent = 0
					indent += 1
				c = inputFile.read(1)
			elif c == '\n':
				indent = 0
				val = intern("<LF>")
				c = inputFile.read(1)
				yield val, c
			else:
				if indent is not None:
					if indent > indents[-1]:	
						indents.append(indent)	
						val = intern("<indent>")
						yield val, c
					elif indent < indents[-1]:
						while indent < indents[-1]:
							indents.pop()
						val = intern("<dedent>")
						yield val, c
					else:
						yield None, c
						return
				else:
					yield None, c
					return
	def readShebang():
		val, c = collect1(shebangBodyCharP)
		return val, c
	def collectSpecialCoding(c):
		c = inputFile.read(1)
		if c == 'o':
			return collect2(octalBodyCharP)
		elif c == 'x':
			return collect2(hexadecimalBodyCharP )
		elif c == '*':
			return collect2(binaryBodyCharP)
		elif c == 'b':
			return collect2(binaryBodyCharP)
		elif digitCharP(c): # r
			basis, c = collect(c, digitCharP)
			if c == 'r' and basis >= 2 and basis <= 36:
				digits = manydigits[:basis]
				return collect2(raryBodyCharP(digits))
			else:
				return error("<special-coding>"), c
		elif c == '!':
			val, c = readShebang()
			val = env(intern("#!"))(val)
			return val, c
		else:
			val, c = env(intern("#"))(inputFile, c)
			return val, c
	def collectString(c):
		val, c = collect1(stringBodyCharP) # FIXME quote
		if c == '"':
			c = inputFile.read(1)
			return val, c
		else:
			return error("<doublequote>"), c
	def collectUnicodeOperator3(c):
		assert(ord(c) == 0xE2) # meaning: 3 bytes total
		c0 = c
		c = inputFile.read(1)
		if c == "":
			return error("<something>", "<nothing>")
		else:
			c1 = c
			c = inputFile.read(1)
			if c == "":
				return error("<something>", "<nothing>")
			else:
				c2 = c
				vals = (c0 + c1 + c2)
				codepoint = ord(vals.decode("utf-8"))
				if mathUnicodeOperatorInRangeP(codepoint):
					c = inputFile.read(1)
					return intern(vals), c
				else:
					c = inputFile.read(1)
					return collectUnicodeID(c, vals)
		# TODO combining suffixes?
	indents = [0]
	while True:
		c = inputFile.read(1)
		while True:
			for val, cc in skipWhitespace(c, indents):
				if val:
					yield val
				c = cc
			if c == "":
				return
			if digitCharP(c):
				val, c = collect(c, digitRestCharP)
			elif specialCodingCharP(c):
				val, c = collectSpecialCoding(c)
			elif asciiIDCharP(c):
				val, c = collectUnicodeID(c, "")
			elif unicodeMaybeOperator1CharP(c):
				val, c = collectUnicodeOperator3(c)
			elif unicodeIDCharP(c):
				val, c = collectUnicodeID(c, "")
			elif operatorCharP(c):
				val, c = collect(c, operatorCharP)
			elif braceCharP(c):
				val = intern(c)
				c = inputFile.read(1)
			elif c == '@':
				val, c = env(intern("#"))(inputFile, c)
				#val, c = collect1(lambda input: input and input != ':') # TODO less special chars?
			elif c == '"':
				val, c = collectString(c)
				val = env(intern("stringFromSymbol"))(val)
			elif c == '\'':
				val = intern("'")
				c = inputFile.read(1)
			else:
				print(c),ord(c)
				assert(False)
				c = inputFile.read(1)
				#yield intern(c)
			yield val

if __name__ == "__main__":
	def readHash(inputFile, c):
		if c == 'e': # exports, probably.
			if inputFile.read(len("xports")) == "xports":
				#if inputFile.read(1) == '[':
				print("FIXME run the entire parser")
				return None, inputFile.read(1)
					
		return (env("error")("<value>"), "")
	def env(name):
		return {
			intern("#"): readHash,
			intern("#!"): lambda val: None,
			intern("stringFromSymbol"): lambda val: val.text,
			intern("error"): lambda *args: (intern("error"), args),
		}[name]
	inputFile = open(sys.argv[1], "r") if len(sys.argv) > 1 else sys.stdin
	co = 31
	def str1(val):
		if isinstance(val, str):
			return "%r" % val
		else:
			return str(val)
	for val in tokenize(inputFile, env):
		sys.stdout.write("\033[%dm" % co)
		co = 32 if co == 31 else 31
		sys.stdout.write(str1(val) + " ")
		if(str(val) == "<LF>"):
			sys.stdout.write("\n")
	sys.stdout.write("\033[m")
