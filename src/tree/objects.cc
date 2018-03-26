#include "../codetree.h"

namespace cpcompiler {
	// declare anonymous
	namespace objects {
		CodeNode *scope(CodeNode *context, CodeNode *node) {
			return context;
		}

		CodeNode *get(CodeNode *context, CodeNode *node) {
			// TODO: read property from object
			return &CodeNode::undefined;
		}
	}

	// register all operators
	CommandDescriptor CommandDescriptor::scope("scope", &objects::scope, 0);
	CommandDescriptor CommandDescriptor::get("get", &objects::get, 2);
	/* TODO:
	 * property (key, value) builds a key/value pair of which you can compose objects
	 * object (leftSubTree, rightSubTree) builds balanced tree of properties that form a object
	 * set (object, property) returns a updated object
	 */
}


