#include "../codetree.h"

namespace cpcompiler {
	// declare anonymous
	namespace operators {
		CodeNode *operator_add(CodeNode *context, CodeNode *node) {
			if (node->param1.node->command == &CommandDescriptor::number && node->param2.node->command == &CommandDescriptor::number) {
				CodeNode *result = CodeNode::allocate();
				result->command = &CommandDescriptor::number;
				result->param1.number = node->param1.node->param1.number + node->param2.node->param1.number;
				return result;
			}
			// TODO: string add, type conversion, error handling and much more
			return NULL;
		}
	}

	// register all operators
	CommandDescriptor CommandDescriptor::operator_add("operator_add", &operators::operator_add, 2);
}

