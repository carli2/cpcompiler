console.log('run this after node-gyp configure && node-gyp build');

var cpcompiler = require('./build/Release/cpcompiler');

console.log('using cpcompiler version ' + cpcompiler.getVersion());

var numbera = cpcompiler.node(12)
var numberb = cpcompiler.node(1)
var numberc = cpcompiler.node('operator_add', numbera, numberb);
console.log(numbera.exec() + '+' + numberb.exec() + '=' + numberc.exec());

// call a print function
var print = cpcompiler.node(function foo(value) {
	console.log('result is ' + value);
});
var code = cpcompiler.node('call', print,
	cpcompiler.node('list',
		numberc,
		cpcompiler.node('emptylist')
	)
);
code.exec();

