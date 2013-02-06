#!/usr/bin/env python
import sys
import libxml2
def childElements(pnode):
	for node in pnode.children or []:
		if node.type != "element":
			continue
		yield node
elementCacheById = {}
def elementById(dom, id):
	result = elementCacheById.get(id)
	if result is not None:
		return result
	result = None
	for element in childElements(dom):
		if element.prop("id") == id:
			result = element
			break
	elementCacheById[id] = result
	return result
def members(dom, pnode):
	ids = pnode.prop("members")
	if ids is None:
		return
	ids = ids.strip()
	if ids == "":
		return
	for id in ids.split(" "):
		id = id.strip() 
		result = elementById(dom, id)
		if result is not None:
			yield result
		else:
			print >>sys.stderr, "warning: did not find element with id %r" % (id, )
baseTypesById = {}
def fundamentalTypeP(element):
	return element.name == "FundamentalType"
def pointerTypeP(element):
	return element.name == "PointerType"
def calcBaseTypeById(dom, id):
	element = elementById(dom, id)
	if fundamentalTypeP(element):
		return element.prop("name")
	elif pointerTypeP(element):
		return "void*"
	else:
		print element.name, "unknown"
		assert(False)
		
def baseTypeById(dom, id):
	global baseTypesById
	if id:
		result = baseTypesById.get(id)
		if result:
			return result
		result = calcBaseTypeById(dom, id)
		baseTypesById[id] = result
		return result
	else:
		print >>sys.stderr, "warning: ignored empty type id"
def namespaceP(node):
	return node.name == "Namespace"
def classP(node):
	return node.name == "Class"
def structP(node):
	return node.name == "Struct"
def functionP(node):
	return node.name == "Function"
def methodP(node): 
	return node.name == "Method"
def argumentP(node):
	return node.name == "Argument"
# TODO constructor, destructor
def processArgument(dom, indentation, pnode):
	name = pnode.prop("name") # can be None
	print "%*s%s : %s" % (indentation*2, "", name, baseTypeById(dom, pnode.prop("type")))
def processFunction(dom, indentation, pnode):
	returns = pnode.prop("returns")
	mangled = pnode.prop("mangled")
	rtypeStr = baseTypeById(dom, returns)
	print "%*s%s => %s" % (indentation*2, "", pnode.prop("name"), rtypeStr)
	for argument in childElements(pnode):
		if argumentP(argument):
			processArgument(dom, indentation + 1, argument)
def processMethod(dom, indentation, pnode):
	return processFunction(dom, indentation, pnode)
def processNamespace(dom, indentation, pnode): # or class
	print "%*s%s" % (indentation*2, "", pnode.prop("name"))
	for element in members(dom, pnode):
		if namespaceP(element):
			processNamespace(dom, indentation + 1, element)
		elif classP(element):
			processNamespace(dom, indentation + 1, element)
		elif structP(element):
			processNamespace(dom, indentation + 1, element)
		elif functionP(element):
			processFunction(dom, indentation + 1, element)
		elif methodP(element):
			processMethod(dom, indentation + 1, element)
def parse(name):
	dom = libxml2.parseFile(name)
	rootNamespace = [node for node in childElements(dom.children) if namespaceP(node) and node.prop("name") == "::"][0]
	processNamespace(dom, 0, rootNamespace)

if __name__ == "__main__":
	parse(sys.argv[-1] if len(sys.argv) > 1 else "test.xml")
