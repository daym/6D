#!/usr/bin/env python
"""
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""
import re
import string
rCComment = re.compile("/\*.*?\*/",re.DOTALL )
class FunctionSignature(object):
	def __init__(self, ret, args):
		self.ret = ret
		self.args = args
class Argsignature(object):
	def __init__(self, typ, name):
		self.typ = typ
		self.name = name
def removeComments(string):
	string = re.sub(rCComment, "" ,string) # remove all occurance streamed comments (/*COMMENT */) from string
	#string = re.sub(re.compile("//.*?\n" ), "" ,string) # remove all occurance singleline comments (//COMMENT\n ) from string
	return string
def loadProtoFile(name):
	result = {}
	with open(name, "r") as f:
		for line in f.readlines():
			line = removeComments(line).strip()
			if line.startswith("extern "):
				line = line[len("extern "):].strip()
			parts = line.split(" ")
			if len(parts) < 3:
				continue
			ag = line.split("(", 1)[1]
			args = [x.strip() for x in ag.split(")")[0].split(",")]
			#if any([x != "NODET" for x in uargs]): # unsupported
			#	continue
			result[parts[1]] = (parts, line, args)
	return result
def loadProtoFiles(names):
	protos = {}
	for n in names:
		d = loadProtoFile(n)
		protos.update(d)
	return protos
