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
				NativeFunction fn = node->param1.node->param1.function;
				return fn(context, node->param2.node, node->param1.node->param2.userdata);
			}
			return node;
		}
	}

	/* register commands */
	CommandDescriptor CommandDescriptor::call("call", &call::call, 2);
	CommandDescriptor CommandDescriptor::native("native", &call::reflect, -1);
	CommandDescriptor CommandDescriptor::lambda("lambda", &call::reflect, 2);
	CommandDescriptor CommandDescriptor::argument("argument", &call::reflect, -1);
	CommandDescriptor CommandDescriptor::return_("return", &call::reflect, 1);

	CommandDescriptor CommandDescriptor::try_("try", &call::reflect, 2);
	CommandDescriptor CommandDescriptor::throw_("throw", &call::reflect, 1);
}

