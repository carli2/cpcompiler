#include <cstddef>
#include <map>

namespace cpcompiler {
	class CommandDescriptor;
	class CodeNode;

	union arbitraryValue {
		CodeNode *node;
		const char *string;
		std::size_t integer;
		double number;
	};

	class CodeNode {
		public:
			CommandDescriptor *command;
			arbitraryValue param1;
			arbitraryValue param2;
			std::size_t gcInfo;
	};

	typedef CodeNode *(*ExecuteFunction)(CodeNode *context, CodeNode *node);

	class CommandDescriptor {
		public:
			const char *name;
			ExecuteFunction executeFunction;
			
			static std::map<const char*, CommandDescriptor*> commands;


			CommandDescriptor(const char *name, ExecuteFunction executeFunction): name(name), executeFunction(executeFunction) { commands[name] = this; }
	};

	void deployCommands();
}
