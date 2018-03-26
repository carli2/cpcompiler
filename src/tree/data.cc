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
	CommandDescriptor CommandDescriptor::true_("true", &data::reflect, 0);
	CommandDescriptor CommandDescriptor::false_("false", &data::reflect, 0);
	CommandDescriptor CommandDescriptor::number("number", &data::reflect);
	CommandDescriptor CommandDescriptor::integer("integer", &data::reflect);
	CommandDescriptor CommandDescriptor::string("string", &data::reflect);
}
