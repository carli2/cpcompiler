#include "../codetree.h"

namespace cpcompiler {
	// declare anonymous
	namespace operators {
		CodeNode *operator_add(CodeNode *context, CodeNode *node) {
			CodeNode *left = node->param1.node->exec(context);
			if (left->command == &CommandDescriptor::throw_) {
				return left;
			}
			CodeNode *right = node->param2.node->exec(context);
			if (right->command == &CommandDescriptor::throw_) {
				return right;
			}

			if (left->command == &CommandDescriptor::number && right->command == &CommandDescriptor::number) {
				CodeNode *result = CodeNode::allocate();
				result->command = &CommandDescriptor::number;
				result->param1.number = left->param1.number + right->param1.number;
				return result;
			}
			// TODO: string add, type conversion, error handling and much more
			return &CodeNode::undefined;
		}
	}

	// register all operators
	CommandDescriptor CommandDescriptor::operator_add("operator_add", &operators::operator_add, 2);
}

