#include "../codetree.h"

namespace cpcompiler {
	/* anonymous namespace */
	namespace controlflow {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
		}

		CodeNode *else_(CodeNode *context, CodeNode *node) {
			// execute true branch
			return node->param1.node->exec(context);
		}

		CodeNode *command(CodeNode *context, CodeNode *node) {
			CodeNode *a = node->param1.node->exec(context);
			if (a->command == &CommandDescriptor::throw_ || a->command == &CommandDescriptor::return_ || a->command == &CommandDescriptor::break_) {
				return a;
			}
			return node->param2.node->exec(context);
		}

		CodeNode *if_(CodeNode *context, CodeNode *node) {
			CodeNode *condition = node->param1.node->exec(context);
			if (condition->command == &CommandDescriptor::throw_) {
				return condition;
			}
			if (condition->isFalse()) {
				// false branch
				if (node->param2.node->command == &CommandDescriptor::else_) {
					// else: false branch
					return node->param2.node->param2.node->exec(context);
				}
				// no else: return undefined
				return &CodeNode::undefined;
			}
			return node->param2.node->exec(context);
		}

		CodeNode *while_(CodeNode *context, CodeNode *node) {
			do {
				CodeNode *condition = node->param1.node->exec(context);
				if (condition->command == &CommandDescriptor::throw_) {
					return condition;
				}
				if (condition->isFalse()) {
					// false branch
					return &CodeNode::undefined;
				}
				CodeNode *body = node->param2.node->exec(context);
				if (body->command == &CommandDescriptor::break_) {
					// break: return undefined
					return &CodeNode::undefined;
				}
				if (body->command == &CommandDescriptor::return_ || body->command == &CommandDescriptor::throw_) {
					return body;
				}
			} while (true);
		}

	}

	/* register commands */
	CommandDescriptor CommandDescriptor::command("command", &controlflow::command, 2);
	CommandDescriptor CommandDescriptor::if_("if", &controlflow::if_, 2);
	CommandDescriptor CommandDescriptor::else_("else", &controlflow::else_, 2);
	CommandDescriptor CommandDescriptor::while_("while", &controlflow::while_, 2);
	CommandDescriptor CommandDescriptor::break_("break", &controlflow::reflect, 0);
}


