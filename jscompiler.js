const syntax = require('./jssyntax.js');
const cpcompiler = require('./build/Release/cpcompiler');

function compile(expr) {
	var ast = syntax.parse(expr);
	function pack(ast) {
		if (ast[0] != 'string') {
			for (var i = 1; i < ast.length; i++) {
				ast[i] = pack(ast[i]);
			}
		}
		return cpcompiler.node.apply(cpcompiler.node, ast);
	}
	return pack(ast);
}

function compileAndExecute(expr, ...args) {
	return compile(expr).exec(...args);
}

console.log(compileAndExecute('1 + 2'));

var codeTree = compile('console.log("Hello World")');
console.log(codeTree.print())
