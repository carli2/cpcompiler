#include "codetree.h"
#include <node.h>
#include <stdio.h>
#include <sstream>
#include <cstring>

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

	CodeNode *nativeExecute(CodeNode *context, CodeNode *args, void *userdata);

	Local<Value> nodeToV8(Isolate *isolate, CodeNode *node) {
		if (node && node->command == &CommandDescriptor::number) {
			return v8::Number::New(isolate, node->param1.number);
		}
		return v8::Undefined(isolate);
	}

	CodeNode *v8ToCodeNode(Isolate *isolate, Local<Value> val) {
		CodeNode *n = &CodeNode::undefined;

		/* Number literal */
		if (val->IsNumber()) {
			n = CodeNode::number(val->NumberValue());
		}

		/* User function */
		else if (val->IsFunction()) {
			// TODO: add destructor callback
			auto p = new v8::NonCopyablePersistentTraits<Function>::NonCopyablePersistent(isolate, Handle<Function>::Cast(val));
			n = CodeNode::native(&nativeExecute, p);
		}

		return n;
	}

	void Print(const FunctionCallbackInfo<Value>& args) {
		if (args.This()->InternalFieldCount()) {
			// fetch CodeNode object
			CodeNode *node = (CodeNode *) External::Cast(*args.This()->GetInternalField(0))->Value();
			std::string prefix = "";
			if (args[0]->IsString()) {
				String::Utf8Value str(args[0]->ToString());
				prefix = *str;
			}
			std::stringstream ss;
			node->print(ss, prefix);
			args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), ss.str().data()));
		}
	}

	void Exec(const FunctionCallbackInfo<Value>& args) {
		if (args.This()->InternalFieldCount()) {
			// fetch CodeNode object
			CodeNode *node = (CodeNode *) External::Cast(*args.This()->GetInternalField(0))->Value();

			// execute
			node = node->exec(&CodeNode::null);

			// output result
			args.GetReturnValue().Set(nodeToV8(args.GetIsolate(), node));
		}
	}

	Handle<Object> wrapToObject(CodeNode *node, Isolate *isolate) {
		node->gcInfo = 2; // persistent
		auto wrapper = ObjectTemplate::New(isolate); // TODO: do not recreate it every time
		wrapper->SetInternalFieldCount(1);
		auto o = wrapper->NewInstance();
		o->SetInternalField(0, External::New(isolate, node));
		NODE_SET_METHOD(o, "exec", Exec); // TODO: put in prototype
		NODE_SET_METHOD(o, "print", Print); // TODO: put in prototype
		// TODO: make persistent and weak with destruction callback
		return o;
	}

	void Version(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "0.0.2"));
	}

	CodeNode *nativeExecute(CodeNode *context, CodeNode *args, void *userdata) {
		v8::Isolate*  isolate = v8::Isolate::GetCurrent();
		auto global = isolate->GetCurrentContext()->Global();

		auto fn = Local<Function>::New(isolate, *(const Persistent<Function>*) userdata);
		Local<Value> callargs[20];
		int argNr = 0;
		while (argNr < 20 && args->command == &CommandDescriptor::list) {
			CodeNode *node = args->param1.node; // fetch argument
			// put into v8 array
			callargs[argNr++] = nodeToV8(isolate, node);
			args = args->param2.node;
		}
		auto result = fn->Call(global, argNr, callargs);
		return v8ToCodeNode(isolate, result);
	}

	void Construct(const FunctionCallbackInfo<Value>& args) {
		CodeNode *n = &CodeNode::undefined;

		/* node-based constructor */
		if (args[0]->IsString()) {
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
				} else if (iter->second == &CommandDescriptor::string) {
					// string literal
					String::Utf8Value str(Local<String>::Cast(args[1]));
					char *mem = (char *) malloc(str.length() + 1);
					memcpy(mem, *str, str.length() + 1);
					n = CodeNode::string(str.length(), mem);
					// TODO: garbage collect later
				} else if (iter->second == &CommandDescriptor::integer) {
					// integer literal
					n = CodeNode::integer(args[1]->ToInteger()->Value());
				}
			} else {
				// not found
				printf("Warning: command not found: %s\n", *str);
				n = &CodeNode::undefined;
			}
		} else {
			n = v8ToCodeNode(args.GetIsolate(), args[0]);
		}

		args.GetReturnValue().Set(wrapToObject(n, args.GetIsolate()));
	}

	void init(Local<Object> exports) {
	  NODE_SET_METHOD(exports, "getVersion", Version);
	  NODE_SET_METHOD(exports, "node", Construct);
	}

	NODE_MODULE(NODE_GYP_MODULE_NAME, init)
}
