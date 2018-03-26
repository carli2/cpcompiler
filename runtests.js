console.log('run this after node-gyp configure && node-gyp build');

const cpcompiler = require('./build/Release/cpcompiler');

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

var returnNumber = cpcompiler.node(function () {
	return 123;
});
var addSomething = cpcompiler.node('operator_add',
	x=cpcompiler.node('call', returnNumber, cpcompiler.node('emptylist')),
	cpcompiler.node(1)
);
console.log('returnNumber() = ' + x.exec());
console.log('returnNumber() + 1 = ' + addSomething.exec());

