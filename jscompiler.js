const syntax = require('./jssyntax.js');
const fs = require('fs');
function getLatestFile(...files) {
	var best = null, bestTime = 0;
	for (var i = 0; i < files.length; i++) {
		try {
			var ctime = fs.statSync(files[i]).ctime.getTime();
			if (ctime > bestTime) {
				bestTime = ctime;
				best = files[i];
			}
		} catch (e) {}
	}
	return best;
}
const cpc = require(getLatestFile('./build/Release/cpcompiler.node', './build/Debug/cpcompiler.node'));

function compile(expr) {
	var ast = syntax.parse(expr);
	function pack(ast) {
		if (ast[0] != 'string' && ast[0] != 'integer' && ast[0] != 'argument') {
			for (var i = 1; i < ast.length; i++) {
				ast[i] = pack(ast[i]);
			}
		}
		if (ast.constructor !== Array) {
			throw new Error('invalid ast: ' + JSON.stringify(ast));
		}
		return cpc.node.apply(cpc.node, ast);
	}
	return pack(ast);
}

function compileAndExecute(expr, ...args) {
	return compile(expr).exec(...args);
}

// context providing console.log
var context = cpc.node('property', cpc.node('string', 'console'), cpc.node('property', cpc.node('string', 'log'), cpc.node(function (x) { console.log(x); })));

console.log('1 + 2 =', compileAndExecute('1 + 2'));
console.log('');

var fn = compile('(function (x) { return x + 1; })(5)');
console.log(fn.print());
console.log('(function (x) { return x + 1; })(5) =', fn.exec());
console.log('');

var codeTree = compile('console.log("Hello World")');
console.log(codeTree.print());
codeTree.exec(context);
console.log('');

console.log(compile('while (a) { console.log("Hello World"); break; }').print());
