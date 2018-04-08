#include "../codetree.h"

namespace cpcompiler {
	/* anonymous namespace */
	namespace list {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
		}

		CodeNode *reflectExecute(CodeNode *context, CodeNode *node) {
			CodeNode *a = node->param1.node->exec(context);
			if (a->command == &CommandDescriptor::throw_) return a;
			CodeNode *b = node->param2.node->exec(context);
			if (b->command == &CommandDescriptor::throw_) return b;
			if (node->param1.node == a && node->param2.node == b) return node;

			CodeNode *result = CodeNode::allocate(node->command);
			result->param1.node = a;
			result->param2.node = b;
			return result;
		}
	}

	/* register commands */
	CommandDescriptor CommandDescriptor::emptylist("emptylist", &list::reflect, 0);
	CommandDescriptor CommandDescriptor::list("list", &list::reflectExecute, 2);
}

