#include "../codetree.h"

namespace cpcompiler {
	/* anonymous namespace */
	namespace call {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
		}

		CodeNode *call(CodeNode *context, CodeNode *node) {
			/* function, parameters */
			if (node->param1.node->command == &CommandDescriptor::lambda) {
				/* TODO: pull out lambda replaced value */
			}
			if (node->param1.node->command == &CommandDescriptor::native) {
				/* call native function */
				return node->param1.node->param1.function(context, node->param2.node, node->param1.node->param2.userdata);
			}
			return node;
		}
	}

	/* register commands */
	CommandDescriptor CommandDescriptor::call("call", &data::reflect, 2);
	CommandDescriptor CommandDescriptor::native("native", &data::reflect, -1);
	CommandDescriptor CommandDescriptor::lambda("lambda", &data::reflect, 2);
	CommandDescriptor CommandDescriptor::argument("argument", &data::reflect, 0);
}

