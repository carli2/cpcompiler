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
			["\\.", "return '.';"]
		]
	},

	"start": "script",

	"bnf": {
		"script": [["e", "return $1;"]],
		"e": [["NULL", "$$ = null;"]]
	}
};

const parser = new jison.Parser(grammar)
module.exports.parse = function (js) {
	return parser.parse(js);
};
