const jison = require('jison');

var grammar = {
	"lex": {
		"rules": [
			["\\s+", "/* skip whitespace*/"],
			["true", "return 'TRUE';"],
			["false", "return 'FALSE';"],
			["null", "return 'NULL';"],
			["undefined", "return 'UNDEFINED';"],
			["function", "return 'FUNCTION';"],
			["return", "return 'RETURN';"],
			[",", "return ',';"],
			[";", "return ';';"],
			["{", "return '{';"],
			["}", "return '}';"],
			["\\(", "return '(';"],
			["\\)", "return ')';"],
			["\\+", "return '+';"],
			["-?[0-9]+(?:\\.[0-9]+)?", "return 'NUMBER';"],
			["[A-Za-z_\\$][A-Za-z_\\$0-9]*", "return 'IDENTIFIER';"],
			["\"(?:[^\\\"]|\\[^])*\"", "yytext = JSON.parse(yytext); return 'STRING';"],
			["'(?:[^\\\"]|\\[^])*'", "yytext = JSON.parse(yytext); return 'STRING';"],
			["\\.", "return '.';"]
		]
	},

	"operators": [
		["left", ","],
		["left", "+", "-"],
		["left", "*", "/"],
		["left", "^"],
		["left", "."],
		["left", "UMINUS"],
		["left", "("]
	],

	"start": "script",

	"bnf": {
		"script": [["e", "return $1;"]],
		"e": [
			["NULL", "$$ = ['null'];"],
			["UNDEFINED", "$$ = ['undefined'];"],
			["TRUE", "$$ = ['true'];"],
			["FALSE", "$$ = ['false'];"],

			["e + e", "$$ = ['operator_add', $1, $3];"],

			["IDENTIFIER", "$$ = ['get', ['scope'], ['string', $1]];"],
			["e . IDENTIFIER", "$$ = ['get', $1, ['string', $3]];"],
			["e list", "$$ = ['call', $1, $2];", {"prec": "CALL"}],
			["FUNCTION ( decllist ) { cmd }", "var args = $3, code = $6; $$ = ['lambda', (function applyArgs(node) {if (node.constructor !== Array) return node; /* TODO: nested lambda: add scope args */ var argidx = -1; if (node[0] == 'get' && node[1][0] == 'scope' && node[2][0] == 'string' && (argidx = args.indexOf(node[2][1])) != -1) { return ['argument', argidx]; } return node.map(applyArgs);})(code), ['null']];", {"prec": "CALL"}],

			["NUMBER", "$$ = [Number(yytext)];"],
			["STRING", "$$ = ['string', yytext];"]
			// ["- e", "$$ = compiler.node('operator_neg', $2);", {"prec": "UMINUS"} ]
		],

		"cmd": [
			["e ;", "$$ = $1;"],
			["RETURN e ;", "$$ = ['return', $2];"]
		],

		"list": [["( liststart", "$$ = $2;"]],
		"liststart": [[")", "$$ = ['emptylist'];"], ["e listtail", "$$ = ['list', $1, $2];"]],
		"listtail": [[")", "$$ = ['emptylist'];"], [", e listtail", "$$ = ['list', $2, $3];"]],

		"decllist": [["IDENTIFIER", "$$ = [$1];"], ["decllist , IDENTIFIER", "$$ = $1; $$.push($3);"]]
	}
};

const parser = new jison.Parser(grammar)
module.exports.parse = function (js) {
	return parser.parse(js);
};
