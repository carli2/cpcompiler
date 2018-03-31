#include "../codetree.h"

namespace cpcompiler {
	/* anonymous namespace */
	namespace controlflow {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
		}
	}

	/* register commands */
	CommandDescriptor CommandDescriptor::command("command", &controlflow::reflect, 2);
	CommandDescriptor CommandDescriptor::if_("if", &controlflow::reflect, 2);
	CommandDescriptor CommandDescriptor::else_("else", &controlflow::reflect, 2);
	CommandDescriptor CommandDescriptor::while_("while", &controlflow::reflect, 2);
}


