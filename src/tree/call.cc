#include "../codetree.h"

namespace cpcompiler {
	/* anonymous namespace */
	namespace call {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
		}

		CodeNode *bind(CodeNode *context, CodeNode *node) {
			CodeNode *newctx = node->param2.node->exec(context);
			if (newctx->command == &CommandDescriptor::throw_) return newctx;
			return CodeNode::allocate(&CommandDescriptor::func, node->param1, { .node = newctx });
		}

		CodeNode *call(CodeNode *context, CodeNode *node) {
			/* function, parameters */
			CodeNode *func = node->param1.node->exec(context);
			if (func->command == &CommandDescriptor::throw_) return func;

			if (func->command == &CommandDescriptor::func) {
				/* function call */
				CodeNode *newctx = func->param2.node;
				/* TODO: extend context object with node->param2.node as argument list */
				return func->param1.node->exec(newctx);
			}
			if (func->command == &CommandDescriptor::native) {
				/* call native function */
				NativeFunction fn = func->param1.function;
				return fn(context, node->param2.node, func->param2.userdata);
			}
			return node;
		}
	}

	/* register commands */
	CommandDescriptor CommandDescriptor::call("call", &call::call, 2);
	CommandDescriptor CommandDescriptor::native("native", &call::reflect, -1);
	CommandDescriptor CommandDescriptor::func("func", &call::reflect, 2); // func is a data structure (cold)
	CommandDescriptor CommandDescriptor::bind("bind", &call::bind, 2); // bind is a piece of code (hot, cools down to func)
	CommandDescriptor CommandDescriptor::return_("return", &call::reflect, 1);

	CommandDescriptor CommandDescriptor::try_("try", &call::reflect, 2);
	CommandDescriptor CommandDescriptor::throw_("throw", &call::reflect, 1);
}

