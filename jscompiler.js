const syntax = require('./jssyntax.js');
const cpcompiler = require('./build/Release/cpcompiler');

function compile(expr) {
	var ast = syntax.parse(expr);
	function pack(ast) {
		if (ast[0] != 'string' && ast[0] != 'integer' && ast[0] != 'argument') {
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

console.log('1 + 2 =', compileAndExecute('1 + 2'));
var fn = compile('function (x) { return x + 1; }');
console.log(fn.print());
console.log('(function (x) { return x + 1; })(5) =', fn.exec(5));

var codeTree = compile('console.log("Hello World")');
console.log(codeTree.print())
