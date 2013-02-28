/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#include "Values/Error"
BEGIN_NAMESPACE_6D(Values)
NodeT parseError(NodeT aExpectedInput, NodeT aGotInput) {
	struct Error* result = NEW(Error);
	result->kind = symbolFromStr("ParseError");
	result->expectedInput = aExpectedInput;
	result->gotInput = aGotInput;
	result->context = nil;
	return refCXXInstance(result);
}
bool errorP(NodeT n) {
	return tagOfNode(n) == TAG_Error;
}
NodeT evalError(NodeT aExpectedInput, NodeT aGotInput, NodeT aContext) {
	struct Error* result = NEW(Error);
	result->kind = symbolFromStr("EvalError");
	result->expectedInput = aExpectedInput;
	result->gotInput = aGotInput;
	result->context = aContext;
	return refCXXInstance(result);
}
NodeT errorKind(NodeT node) {
	const struct Error* error = (const struct Error*) getCXXInstance(node);
	return error->kind;
}
NodeT errorExpectedInput(NodeT node) {
	const struct Error* error = (const struct Error*) getCXXInstance(node);
	return error->expectedInput;
}
NodeT errorGotInput(NodeT node) {
	const struct Error* error = (const struct Error*) getCXXInstance(node);
	return error->gotInput;
}
NodeT errorContext(NodeT node) {
	const struct Error* error = (const struct Error*) getCXXInstance(node);
	return error->context;
}

END_NAMESPACE_6D(Values)
