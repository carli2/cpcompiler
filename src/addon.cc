#include <node.h>
#include "codetree.h"
//#include <stdio.h>

namespace cpcompiler {
	using v8::FunctionCallbackInfo;
	using v8::Isolate;
	using v8::Local;
	using v8::Handle;
	using v8::Persistent;

	using v8::Object;
	using v8::ObjectTemplate;
	using v8::Function;
	using v8::String;
	using v8::Value;
	using v8::External;

	void Exec(const FunctionCallbackInfo<Value>& args) {
		if (args.This()->InternalFieldCount()) {
			// fetch CodeNode object
			CodeNode *node = (CodeNode *) External::Cast(*args.This()->GetInternalField(0))->Value();

			// execute
			node = node->command->executeFunction(&CodeNode::null, node); // TODO: retrieve context

			// output primitive value (TODO: put in function, make recursive)
			if (node && node->command == &CommandDescriptor::number) {
				args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), node->param1.number));
			}
		}
	}

	Handle<Object> wrapToObject(CodeNode *node, Isolate *isolate) {
		node->gcInfo = 2; // persistent
		auto wrapper = ObjectTemplate::New(isolate); // TODO: do not recreate it every time
		wrapper->SetInternalFieldCount(1);
		auto o = wrapper->NewInstance();
		o->SetInternalField(0, External::New(isolate, node));
		NODE_SET_METHOD(o, "exec", Exec); // TODO: put in prototype
		// TODO: make persistent and weak with destruction callback
		return o;
	}

	void Version(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "0.0.2"));
	}

	void Construct(const FunctionCallbackInfo<Value>& args) {
		CodeNode *n = &CodeNode::undefined;

		/* Number literal */
		if (args[0]->IsNumber()) {
			n = CodeNode::allocate();
			n->command = &CommandDescriptor::number;
			n->param1.number = args[0]->NumberValue();
		}

		/* node-based constructor */
		else if (args[0]->IsString()) {
			String::Utf8Value str(args[0]->ToString());
			auto iter = CommandDescriptor::commands.find(*str);
			if (iter != CommandDescriptor::commands.end()) {
				if (iter->second->nodeArguments != -1) {
					n = CodeNode::allocate();
					n->command = iter->second;
					if (iter->second->nodeArguments >= 1) {
						if (args[1]->IsObject() && Object::Cast(*args[1])->InternalFieldCount() == 1) {
							n->param1.node = (CodeNode *) External::Cast(*Object::Cast(*args[1])->GetInternalField(0))->Value();
						} else {
							n->param1.node = &CodeNode::undefined;
						}
					}
					if (iter->second->nodeArguments >= 2) {
						if (args[2]->IsObject() && Object::Cast(*args[2])->InternalFieldCount() == 1) {
							n->param2.node = (CodeNode *) External::Cast(*Object::Cast(*args[2])->GetInternalField(0))->Value();
						} else {
							n->param2.node = &CodeNode::undefined;
						}
					}
				}
			}
		}

		args.GetReturnValue().Set(wrapToObject(n, args.GetIsolate()));
	}

	void init(Local<Object> exports) {
	  NODE_SET_METHOD(exports, "getVersion", Version);
	  NODE_SET_METHOD(exports, "node", Construct);
	}

	NODE_MODULE(NODE_GYP_MODULE_NAME, init)
}
