#!/usr/bin/env python
from xml.dom import minidom
def childElements(pnode):
	for node in pnode.childNodes:
		if node.nodeType != node.ELEMENT_NODE:
			continue
		yield node
def elementById(dom, id):
	for element in childElements(dom.documentElement):
		if element.getAttribute("id") == id:
			return element
	return None
def members(dom, pnode):
	ids = pnode.getAttribute("members")
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
def namespaceP(node):
	return node.tagName == "Namespace"
def classP(node):
	return node.tagName == "Class"
def structP(node):
	return node.tagName == "Struct"
def functionP(node):
	return node.tagName == "Function"
def methodP(node): 
	return node.tagName == "Method"
# TODO constructor, destructor
def processArgument(dom, indentation, pnode):
	print "%*s%s" % (indentation*2, "", pnode.getAttribute("name"))
def processFunction(dom, indentation, pnode):
	print "%*s%s" % (indentation*2, "", pnode.getAttribute("name"))
	for argument in childElements(pnode):
		if argument.tagName != "Argument":
			continue
		processArgument(dom, indentation + 1, pnode)
def processMethod(dom, indentation, pnode):
	print "%*s%s" % (indentation*2, "", pnode.getAttribute("name"))
def processNamespace(dom, indentation, pnode): # or class
	print "%*s%s" % (indentation*2, "", pnode.getAttribute("name"))
	for element in members(dom, pnode):
		if namespaceP(element):
			processNamespace(dom, indentation + 1, element)
		elif classP(element):
			processNamespace(dom, indentation + 1, element)
		elif structP(element):
			processNamespace(dom, indentation + 1, element)
		elif functionP(element):
			processFunction(dom, indentation + 1, element)
def parse(name):
	dom = minidom.parse(name)
	rootNamespace = [node for node in childElements(dom.documentElement) if namespaceP(node) and node.getAttribute("name") == "::"][0]
	processNamespace(dom, 0, rootNamespace)

if __name__ == "__main__":
	parse("test.xml")
