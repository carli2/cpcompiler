#include "../codetree.h"

namespace cpcompiler {
	// declare anonymous
	namespace objects {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return context;
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

		CodeNode *scope(CodeNode *context, CodeNode *node) {
			return context;
		}

		CodeNode *get(CodeNode *context, CodeNode *node) {
			// TODO: read property from object
			return &CodeNode::undefined;
		}

		CodeNode *set(CodeNode *context, CodeNode *node) {
			// TODO: write property to object
			return &CodeNode::undefined;
		}
	}

	// register all operators
	CommandDescriptor CommandDescriptor::scope("scope", &objects::scope, 0);
	CommandDescriptor CommandDescriptor::get("get", &objects::get, 2);
	CommandDescriptor CommandDescriptor::set("set", &objects::set, 2);

	CommandDescriptor CommandDescriptor::property("property", &objects::reflectExecute, 2);
	CommandDescriptor CommandDescriptor::object("object", &objects::reflectExecute, 2);
}


