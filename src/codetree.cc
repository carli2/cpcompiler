#include "codetree.h"

namespace cpcompiler {
	std::map<const char*, CommandDescriptor*> CommandDescriptor::commands;

	namespace command {
		CodeNode *operator_add(CodeNode *context, CodeNode *node) {
			return NULL;
		}
	}

	void deployCommands() {
		CommandDescriptor("operator_add", &command::operator_add);
	}
}
