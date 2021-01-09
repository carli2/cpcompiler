console.log('run this after node-gyp configure && node-gyp build');

const cpc = require('./build/Release/cpcompiler');

console.log('using cpcompiler version ' + cpc.getVersion());

var numbera = cpc.node(12)
var numberb = cpc.node(1)
var numberc = cpc.node('operator_add', numbera, numberb);
console.log(numbera.exec() + '+' + numberb.exec() + '=' + numberc.exec());

// call a print function
var print = cpc.node(function foo(value) {
	console.log('result is ' + value);
});
var code = cpc.node('call', print,
	cpc.node('list',
		numberc,
		cpc.node('emptylist')
	)
);
code.exec();

var returnNumber = cpc.node(function () {
	return 123;
});
var addSomething = cpc.node('operator_add',
	x=cpc.node('call', returnNumber, cpc.node('emptylist')),
	cpc.node(1)
);
console.log('returnNumber() = ' + x.exec());
console.log('returnNumber() + 1 = ' + addSomething.exec());

