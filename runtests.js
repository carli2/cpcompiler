console.log('run this after node-gyp configure && node-gyp build');

var cpcompiler = require('./build/Release/cpcompiler');

console.log('using cpcompiler version ' + cpcompiler.getVersion());

var numbera = cpcompiler.node(12)
console.log(numbera.exec());

var numberb = cpcompiler.node(1)
console.log(numberb.exec());

var numberc = cpcompiler.node('operator_add', numbera, numberb);
console.log(numberc.exec());
