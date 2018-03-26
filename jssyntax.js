const jison = require('jison');

var grammar = {
	"lex": {
		"rules": [
			["\\s+", "/* skip whitespace*/"],
			["true", "return 'TRUE';"],
			["false", "return 'FALSE';"],
			["null", "return 'NULL';"],
			["undefined", "return 'UNDEFINED';"],
			[",", "return ',';"],
			[";", "return ';';"],
			["\\+", "return '+';"],
			["-?[0-9]+(?:\\.[0-9]+)?", "return 'NUMBER';"],
			["\\.", "return '.';"]
		]
	},

	"operators": [
		["left", "+", "-"],
		["left", "*", "/"],
		["left", "^"],
		["left", "UMINUS"]
	],

	"start": "script",

	"bnf": {
		"script": [["e", "return $1;"]],
		"e": [
			["NULL", "$$ = ['null'];"],
			["e + e", "$$ = ['operator_add', $1, $3];"],
			["NUMBER", "$$ = [Number(yytext)];"]
			// ["- e", "$$ = compiler.node('operator_neg', $2);", {"prec": "UMINUS"} ]
		]
	}
};

const parser = new jison.Parser(grammar)
module.exports.parse = function (js) {
	return parser.parse(js);
};
