#include <cstddef>
#include <map>

namespace cpcompiler {
	class CommandDescriptor;
	class CodeNode;

	const std::size_t MAX_MEMORY = 2 << 12; // 24

	/** for custom functions:
	 * @param context holds all variables, global object and so on
	 * @param arguments arguments passed from the caller
	 * @param userdata passed by function
	 * @return result as CodeNode
	 */
	typedef CodeNode *(*NativeFunction)(CodeNode *context, CodeNode *arguments, void *userdata);

	/* size: 64 bit */
	union arbitraryValue {
		CodeNode *node;
		const char *string;
		std::size_t integer;
		double number;
		NativeFunction function;
		void *userdata;
	};

	/** executing a node:
	 * @param context holds all variables, global object and so on
	 * @param node is the node to evaluate
	 * @return result as CodeNode
	 */
	typedef CodeNode *(*ExecuteFunction)(CodeNode *context, CodeNode *node);

	/* describes a node type: with all optimizations */
	class CommandDescriptor {
		public:
			const char *name; /* name of the node type */
			ExecuteFunction executeFunction; /* how to execute/optimze */
			int nodeArguments; /* how many node arguments are allowed: -1 = special (e.g. integer, number, string), 0 = leaf (null, undefined), 1 = unary, 2 = binary */
			
			static std::map<const std::string, CommandDescriptor*> commands; /* register of all commands */

			/* declaring new node types */
			CommandDescriptor(const char *name, ExecuteFunction executeFunction): name(name), executeFunction(executeFunction), nodeArguments(-1) { commands[name] = this; }
			CommandDescriptor(const char *name, ExecuteFunction executeFunction, int nodeArguments): name(name), executeFunction(executeFunction), nodeArguments(nodeArguments) { commands[name] = this; }

			/* data */
			static CommandDescriptor undefined;
			static CommandDescriptor null;
			static CommandDescriptor number;
			static CommandDescriptor string;
			static CommandDescriptor integer;

			/* arithmetic */
			static CommandDescriptor operator_add;

			/* call */
			static CommandDescriptor call;
			static CommandDescriptor native;
			static CommandDescriptor lambda;
			static CommandDescriptor argument;

			/* list */
			static CommandDescriptor emptylist;
			static CommandDescriptor list;

	};

	/* a node all code and data trees are constructed from */
	class CodeNode {
		public:
			CodeNode() {}
			CodeNode(CommandDescriptor *command): command(command) {}

			CommandDescriptor *command; /* command */
			arbitraryValue param1; /* 0-2 child nodes, not more. */
			arbitraryValue param2;
			std::size_t gcInfo = 0; // 0 = unused; gc info for garbage collector

			static CodeNode *allocate(); /* get free space in a few cycles thanks to constant node size */
			static inline CodeNode *allocate(CommandDescriptor *command) {
				CodeNode *result = allocate();
				result->command = command;
				return result;
			}
			static inline CodeNode *allocate(CommandDescriptor *command, const arbitraryValue &param1, const arbitraryValue &param2) {
				CodeNode *result = allocate();
				result->command = command;
				result->param1 = param1;
				result->param2 = param2;
				return result;
			}

			/* the heart: Execute */
			CodeNode *exec(CodeNode *context) {
				return this->command->executeFunction(context, this);
			}

			/* some default constructors */
			static CodeNode *number(double number);
			static CodeNode *integer(std::size_t integer);
			static CodeNode *string(const char *string);
			static CodeNode *native(NativeFunction function, void *userdata);

			/* some constants to save allocations */
			static CodeNode undefined;
			static CodeNode null;

		private:
			static CodeNode allMemory[MAX_MEMORY]; /* our heap; hopefully sparsely occupied by OS */
			static std::size_t nextPtr;
	};
}
