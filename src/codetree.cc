#include "codetree.h"

namespace cpcompiler {
	std::map<const char*, CommandDescriptor*> CommandDescriptor::commands;

	namespace command {
		CodeNode *operator_add(CodeNode *context, CodeNode *node) {
			if (node->param1.node->command == &CommandDescriptor::number && node->param2.node->command == &CommandDescriptor::number) {
				CodeNode *result = CodeNode::allocate();
				result->command = &CommandDescriptor::number;
				result->param1.number = node->param1.node->param1.number + node->param2.node->param1.number;
				return result;
			}
			// TODO: string add, type conversion, error handling and much more
			return NULL;
		}
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
		}
	}

	CommandDescriptor CommandDescriptor::undefined("undefined", &command::reflect);
	CommandDescriptor CommandDescriptor::null("null", &command::reflect);
	CommandDescriptor CommandDescriptor::number("number", &command::reflect);
	CommandDescriptor CommandDescriptor::operator_add("operator_add", &command::operator_add);

	CodeNode CodeNode::undefined(&CommandDescriptor::undefined);
	CodeNode CodeNode::null(&CommandDescriptor::null);

	CodeNode *CodeNode::allocate() {
		// TODO: Garbage Collect in separate thread
		allMemory[nextPtr].gcInfo = 1; // used
		return &allMemory[nextPtr++];
	}

	CodeNode CodeNode::allMemory[MAX_MEMORY];
	std::size_t CodeNode::nextPtr = 0;
}
