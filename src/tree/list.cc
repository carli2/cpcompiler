#include "../codetree.h"

namespace cpcompiler {
	/* anonymous namespace */
	namespace list {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
		}
	}

	/* register commands */
	CommandDescriptor CommandDescriptor::emptylist("emptylist", &list::reflect, 0);
	CommandDescriptor CommandDescriptor::list("list", &list::reflect, 2);
}

