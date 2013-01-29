#!/usr/bin/env python

class Symbol(object):
	def __init__(self, text):
		self.text = text
	def __str__(self):
		return self.text
	def __repr__(self):
		return self.text
	def __hash__(self):
		return hash(self.text)
	def __eq__(self, other):
		return self is other
symbols = {}
def intern(name):
	if name not in symbols:
		symbols[name] = Symbol(name)
	return symbols[name]
