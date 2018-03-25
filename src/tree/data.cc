#include "../codetree.h"

namespace cpcompiler {
	/* anonymous namespace */
	namespace data {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
		}
	}

	/* register commands */
	CommandDescriptor CommandDescriptor::undefined("undefined", &data::reflect, 0);
	CommandDescriptor CommandDescriptor::null("null", &data::reflect, 0);
	CommandDescriptor CommandDescriptor::number("number", &data::reflect);
}
