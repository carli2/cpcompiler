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

		CodeNode *getValueFromObject(CodeNode *obj, CodeNode *attr) {
			if (obj->command == &CommandDescriptor::prototype) {
				// prototype: first try first object, then second
				CodeNode *val = getValueFromObject(obj->param1.node, attr);
				if (val->command == &CommandDescriptor::undefined) {
					val = getValueFromObject(obj->param2.node, attr);
				}
				return val;
			} else if (obj->command == &CommandDescriptor::object) {
				// object: first try first object, then second
				CodeNode *val = getValueFromObject(obj->param1.node, attr);
				if (val->command == &CommandDescriptor::undefined) {
					val = getValueFromObject(obj->param2.node, attr);
				}
				return val;
			} else if (obj->command == &CommandDescriptor::property) {
				if (obj->param1.node->equals(attr)) {
					// found property
					return obj->param2.node;
				} else {
					// property not found
					return &CodeNode::undefined;
				}
			} else if (obj->command == &CommandDescriptor::list && attr->command == &CommandDescriptor::number) {
				// get nth index of list (number)
				int idx = (int)attr->param1.number;
				while (idx) {
					obj = obj->param2.node; // linked list traversal
					if (obj->command != &CommandDescriptor::list) return &CodeNode::undefined; // end of list reached but index is higher
					idx--;
				}
				// found item
				return obj->param1.node;
			} else if (obj->command == &CommandDescriptor::list && attr->command == &CommandDescriptor::integer) {
				// get nth index of list (integer)
				int idx = attr->param1.integer;
				while (idx) {
					obj = obj->param2.node; // linked list traversal
					if (obj->command != &CommandDescriptor::list) return &CodeNode::undefined; // end of list reached but index is higher
					idx--;
				}
				// found item
				return obj->param1.node;
			} else {
				std::cout<<"get "; attr->print(std::cout, "");
				std::cout<<"from "; obj->print(std::cout, "");
				return &CodeNode::undefined;
			}
		}

		CodeNode *get(CodeNode *context, CodeNode *node) {
			// read property from object
			CodeNode *obj = node->param1.node->exec(context);
			CodeNode *attr = node->param2.node->exec(context);
			return getValueFromObject(obj, attr);
		}

		CodeNode *set(CodeNode *context, CodeNode *node) {
			// write property to object
			CodeNode *obj = node->param1.node->exec(context);
			CodeNode *val = node->param2.node->exec(context);
			std::cout<<"set "; obj->print(std::cout, "");
			std::cout<<"to "; val->print(std::cout, "");
			return &CodeNode::undefined;
		}
	}

	// register all operators
	CommandDescriptor CommandDescriptor::scope("scope", &objects::scope, 0);
	CommandDescriptor CommandDescriptor::get("get", &objects::get, 2);
	CommandDescriptor CommandDescriptor::set("set", &objects::set, 2);

	CommandDescriptor CommandDescriptor::property("property", &objects::reflectExecute, 2);
	CommandDescriptor CommandDescriptor::object("object", &objects::reflectExecute, 2);
	CommandDescriptor CommandDescriptor::emptyobject("emptyobject", &objects::reflect, 0);
	CommandDescriptor CommandDescriptor::prototype("prototype", &objects::reflectExecute, 2);
}


