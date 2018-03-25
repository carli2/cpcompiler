#include "codetree.h"

namespace cpcompiler {
	std::map<const std::string, CommandDescriptor*> CommandDescriptor::commands;

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
