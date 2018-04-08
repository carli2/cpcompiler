#include "../codetree.h"

namespace cpcompiler {
	// declare anonymous
	namespace objects {
		CodeNode *reflect(CodeNode *context, CodeNode *node) {
			return node;
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
				if (!val) {
					val = getValueFromObject(obj->param2.node, attr);
				}
				return val;
			} else if (obj->command == &CommandDescriptor::object) {
				// object: first try first object, then second
				CodeNode *val = getValueFromObject(obj->param1.node, attr);
				if (!val) {
					val = getValueFromObject(obj->param2.node, attr);
				}
				return val;
			} else if (obj->command == &CommandDescriptor::property) {
				if (obj->param1.node->equals(attr)) {
					// found property
					if (obj->param2.node->command == &CommandDescriptor::var) {
						// resolve var
						return obj->param2.node->param1.node;
					}
					return obj->param2.node;
				} else {
					// property not found
					return NULL;
				}
			} else if (obj->command == &CommandDescriptor::var) {
				// var
				return getValueFromObject(obj->param1.node, attr);
			} else if (obj->command == &CommandDescriptor::emptyobject) {
				// empty
				return NULL;
			} else if (obj->command == &CommandDescriptor::null) {
				// null
				// TODO: throw
				return NULL;

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
				return NULL;
			}
		}

		CodeNode *setObjectValue(CodeNode *obj, CodeNode *val, bool &success) {
			if (obj->command == &CommandDescriptor::prototype) {
				// prototype: only manipulate original object
				return setObjectValue(obj->param1.node, val, success);

			} else if (obj->command == &CommandDescriptor::emptyobject) {
				// empty: append property
				return obj;
				/*success = true;
				return val;*/

			} else if (obj->command == &CommandDescriptor::object) {
				// object: first try to replace; do not append
				CodeNode *left = setObjectValue(obj->param1.node, val, success);
				CodeNode *right = obj->param2.node;
				if (!success) right = setObjectValue(right, val, success);
				if (!success) {
					// was not able to replace: return without modification
					return obj;
				} else {
					// return modified object
					return CodeNode::allocate(&CommandDescriptor::object, { .node = left }, { .node = right });
				}

			} else if (obj->command == &CommandDescriptor::property) {
				// property: replace if is correct
				if (obj->param1.node->equals(val->param1.node)) {
					// property name equals: replace
					success = true;
					return val;
				} else {
					// otherwise: return unchanged
					return obj;
				}

			} else if (obj->command == &CommandDescriptor::var) {
				// set variable
				if (val->command == &CommandDescriptor::property) {
					// weave property into stored object
					obj->param1.node = setObjectValue(obj->param1.node, val, success);
					if (!success) {
						// not able to replace: append
						obj->param1.node = CodeNode::allocate(&CommandDescriptor::object, { .node = val }, { .node = obj->param1.node });
					}
					return obj->param1.node;
				} else {
					// set variable value directly
					obj->param1.node = val;
					return val;
				}
			}
			std::cout<<"set "; obj->print(std::cout, "");
			std::cout<<"to "; val->print(std::cout, "");
			return &CodeNode::undefined;
		}

		CodeNode *get(CodeNode *context, CodeNode *node) {
			// read property from object
			CodeNode *obj = node->param1.node->exec(context);
			CodeNode *attr = node->param2.node->exec(context);
			return getValueFromObject(obj, attr) ?: &CodeNode::undefined;
		}

		CodeNode *set(CodeNode *context, CodeNode *node) {
			// write property to object
			CodeNode *obj = node->param1.node->exec(context);
			CodeNode *val = node->param2.node->exec(context);
			bool success = false;
			CodeNode *result = setObjectValue(obj, val, success);
			if (!success) {
				// not able to set: append
				return CodeNode::allocate(&CommandDescriptor::object, { .node = val }, { .node = result });
			}
			return result;
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


