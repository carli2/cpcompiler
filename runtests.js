console.log('run this after node-gyp configure && node-gyp build');

var cpcompiler = require('./build/Release/cpcompiler');

console.log('using cpcompiler version ' + cpcompiler.getVersion());
