#include "../codetree.h"

namespace cpcompiler {
	// declare anonymous
	namespace mutable_ {
		CodeNode *var(CodeNode *context, CodeNode *node) {
			// reflect content
			return node->param1.node;
		}

		CodeNode *setvar(CodeNode *context, CodeNode *node) {
			CodeNode *val = node->param2.node->exec(context);
			if (val->command == &CommandDescriptor::throw_) {
				return val;
			}
			// set mutable value
			node->param1.node->param1.node = val;
			return val;
		}

		CodeNode *newvar(CodeNode *context, CodeNode *node) {
			// evaluate new value
			CodeNode *val = node->param2.node->exec(context);
			if (val->command == &CommandDescriptor::throw_) {
				return val;
			}
			// allocate
			CodeNode *var = CodeNode::allocate(&CommandDescriptor::var);
			var->param1.node = val;
			return var;
		}
	}

	// register all operators
	CommandDescriptor CommandDescriptor::var("var", &mutable_::var, 1);
	CommandDescriptor CommandDescriptor::setvar("setvar", &mutable_::setvar, 2);
	CommandDescriptor CommandDescriptor::newvar("newvar", &mutable_::newvar, 1);
}


