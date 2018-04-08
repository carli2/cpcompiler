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
			["if", "return 'IF';"],
			["while", "return 'WHILE';"],
			["break", "return 'BREAK';"],
			["else", "return 'ELSE';"],
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
		["left", ";"],
		["left", ","],
		["left", "||"],
		["left", "&&"],
		["left", "+", "-"],
		["left", "*", "/"],
		["left", "^"],
		["left", "."],
		["left", "UMINUS"],
		["left", "("]
	],

	"start": "script",

	"bnf": {
		"script": [["cmd", "return $1;"]],
		"e": [
			["NULL", "$$ = ['null'];"],
			["UNDEFINED", "$$ = ['undefined'];"],
			["TRUE", "$$ = ['true'];"],
			["FALSE", "$$ = ['false'];"],

			["e + e", "$$ = ['operator_add', $1, $3];"],

			["( e )", "$$ = $2;"],
			["IDENTIFIER", "$$ = ['get', ['scope'], ['string', $1]];"],
			["e . IDENTIFIER", "$$ = ['get', $1, ['string', $3]];"],
			["e list", "$$ = ['call', $1, $2];", {"prec": "CALL"}],
			["FUNCTION ( decllist ) { cmd }", "$$ = ['bind', ['command', $3.map(function (x, i) { return ['set', ['scope'], ['property', ['string', x], ['newvar', ['get', ['get', ['scope'], ['string', 'arguments']], [i]]]]]; }).reduce(function (a, x) { return ['command', a, x]; }), $6], ['newvar', ['prototype', ['emptyobject'], ['scope']]]];", {"prec": "CALL"}],
			["FUNCTION IDENTIFIER ( decllist ) { cmd }", "$$ = ['set', ['scope'], ['property', ['string', $2], ['bind', ['command', $3.map(function (x, i) { return ['set', ['scope'], ['property', ['string', x], ['newvar', ['get', ['get', ['scope'], ['string', 'arguments']], [i]]]]]; }).reduce(function (a, x) { return ['command', a, x]; }), $6], ['newvar', ['prototype', ['emptyobject'], ['scope']]]]]];", {"prec": "CALL"}],

			["NUMBER", "$$ = [Number(yytext)];"],
			["STRING", "$$ = ['string', yytext];"]
			// ["- e", "$$ = compiler.node('operator_neg', $2);", {"prec": "UMINUS"} ]
		],

		"cmd": [
			["e", "$$ = $1;"],
			["RETURN e", "$$ = ['return', $2];"],
			["BREAK", "$$ = ['break'];"],
			["IF ( e ) { cmd }", "$$ = ['if', $3, $6];"],
			["IF ( e ) { cmd } ELSE { cmd }", "$$ = ['if', $3, ['else', $6, $10]];"],
			["WHILE ( e ) { cmd }", "$$ = ['while', $3, $6];"],
			["cmd ;", "$$ = $1;"],
			["cmd ; cmd", "$$ = ['command', $1, $3];", {"prec": ";"}]
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
