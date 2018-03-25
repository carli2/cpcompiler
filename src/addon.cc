#include <node.h>
#include "codetree.h"

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
			CodeNode *node = (CodeNode *) External::Cast(*args.This()->GetInternalField(0))->Value();
			// TODO: execute
			if (node && node->command == &CommandDescriptor::number) {
				args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), node->param1.number));
			}
		}
	}

	Handle<Object> wrapToObject(CodeNode *node, Isolate *isolate) {
		node->gcInfo = 2; // persistent
		auto wrapper = ObjectTemplate::New(isolate);
		wrapper->SetInternalFieldCount(1);
		auto o = wrapper->NewInstance();
		o->SetInternalField(0, External::New(isolate, node));
		NODE_SET_METHOD(o, "exec", Exec);
		return o;
		// External::Unwrap(val)
	}

	void Method(const FunctionCallbackInfo<Value>& args) {
	  Isolate* isolate = args.GetIsolate();
	  args.GetReturnValue().Set(String::NewFromUtf8(isolate, "0.0.1"));
	}

	void Construct(const FunctionCallbackInfo<Value>& args) {
		CodeNode *n = &CodeNode::undefined;

		/* Number literal */
		if (args[0]->IsNumber()) {
			n = CodeNode::allocate();
			n->command = &CommandDescriptor::number;
			n->param1.number = args[0]->NumberValue();
		}

		args.GetReturnValue().Set(wrapToObject(n, args.GetIsolate()));
	}

	void init(Local<Object> exports) {
	  NODE_SET_METHOD(exports, "getVersion", Method);
	  NODE_SET_METHOD(exports, "node", Construct);
	}

	NODE_MODULE(NODE_GYP_MODULE_NAME, init)
}
