#include <cstddef>
#include <map>

namespace cpcompiler {
	class CommandDescriptor;
	class CodeNode;

	const std::size_t MAX_MEMORY = 2 << 24;

	union arbitraryValue {
		CodeNode *node;
		const char *string;
		std::size_t integer;
		double number;
	};

	class CodeNode {
		public:
			CodeNode() {}
			CodeNode(CommandDescriptor *command): command(command) {}

			CommandDescriptor *command;
			arbitraryValue param1;
			arbitraryValue param2;
			std::size_t gcInfo = 0; // unused

			static CodeNode *allocate();
			static inline CodeNode *allocate(CommandDescriptor *command, const arbitraryValue &param1, const arbitraryValue &param2) {
				CodeNode *result = allocate();
				result->command = command;
				result->param1 = param1;
				result->param2 = param2;
				return result;
			}

			static CodeNode undefined;
			static CodeNode null;

		private:
			static CodeNode allMemory[MAX_MEMORY];
			static std::size_t nextPtr;
	};

	typedef CodeNode *(*ExecuteFunction)(CodeNode *context, CodeNode *node);

	class CommandDescriptor {
		public:
			const char *name;
			ExecuteFunction executeFunction;
			
			static std::map<const char*, CommandDescriptor*> commands;

			CommandDescriptor(const char *name, ExecuteFunction executeFunction): name(name), executeFunction(executeFunction) { commands[name] = this; }

			/* some command descriptors forward */
			static CommandDescriptor undefined;
			static CommandDescriptor null;
			static CommandDescriptor number;
			static CommandDescriptor operator_add;

	};

	void deployCommands();
}
