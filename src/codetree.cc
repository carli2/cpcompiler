#include "codetree.h"

namespace cpcompiler {
	std::map<const std::string, CommandDescriptor*> CommandDescriptor::commands;

	CodeNode CodeNode::undefined(&CommandDescriptor::undefined);
	CodeNode CodeNode::null(&CommandDescriptor::null);

	CodeNode *CodeNode::number(double number) {
		CodeNode *result = allocate(&CommandDescriptor::number);
		result->param1.number = number;
		return result;
	}

	CodeNode *CodeNode::integer(std::size_t integer) {
		CodeNode *result = allocate(&CommandDescriptor::integer);
		result->param1.integer = integer;
		return result;
	}

	CodeNode *CodeNode::string(size_t length, const char *string) {
		CodeNode *result = allocate(&CommandDescriptor::string);
		result->param1.integer = length;
		result->param2.string = string;
		return result;
	}

	CodeNode *CodeNode::native(NativeFunction function, void *userdata) {
		CodeNode *result = allocate(&CommandDescriptor::native);
		result->param1.function = function;
		result->param2.userdata = userdata;
		return result;
	}

	CodeNode *CodeNode::allocate() {
		// TODO: Garbage Collect in separate thread
		allMemory[nextPtr].gcInfo = 1; // used
		return &allMemory[nextPtr++];
	}

	CodeNode CodeNode::allMemory[MAX_MEMORY];
	std::size_t CodeNode::nextPtr = 0;
}
