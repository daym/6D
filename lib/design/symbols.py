#!/usr/bin/env python
"""
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

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
