
#include "LangageGrammar.h"
#include "Compiler.h"


// init token definition (lexer)
enum E_TokenId {
	TOKEN_TYPE_BOOL   =  257,
	TOKEN_TYPE_BYTE,  // 258
	TOKEN_IDENTIFIER, // 259
	TOKEN_NUMBER,	  // 260
	TOKEN_RETURN,	  // 261
};
TokenDefinition token_definition[] =
{
	{ TOKEN_TYPE_BOOL,  "bool"},
	{ TOKEN_TYPE_BYTE,  "byte"},
	{ TOKEN_RETURN,		 "return"},
	{ TOKEN_IDENTIFIER, nullptr, "[a-zA-a_][a-zA-a0-9_]*"},
	{ TOKEN_NUMBER,		nullptr, "[0-9]*"},
};
// grammar elements
enum E_RuleId {
	RULE_FUNCTION			= 1000,
	RULE_FUNCTION_DEFINTION,// 1001 ex : bool f(byte a, byte b) 
	RULE_TYPE,				// 1002 ex : bool
	RULE_N_PARAMETERS_DECL,	// 1003 ex : (bool a, byte b)		
	RULE_1_PARAMETER_DECL,	// 1004 ex : bool b
	RULE_CODEBLOC,			// 1005 ex : { return a+b; }
	RULE_N_STATEMENTS,		// 1006 ex : a++;b++;return a+b;	
	RULE_1_STATEMENT,		// 1007 ex : a++
	RULE_OPERATION,			// 1008 ex : a&b
	RULE_INSTRUCITON_RETURN,// 1009 ex : return a+b
	RULE_EXPRESSION,		// 1010 ex : 123 our a or a+b
	RULE_LITTERAL,			// 1011 ex : 123
	RULE_VARIABLE,			// 1012 ex : a
	RULE_OPERATOR_AND,		// 1013 ex : a&b
	RULE_OPERATOR_OR,		// 1013 ex : a|b
	RULE_PROGRAM			 = 1999, //  the whole program
};
// get the token definition
std::pair<TokenDefinition*, int> LangageGrammar::get_token_definition(void) {
	int nb_token_rules = sizeof(token_definition) / sizeof(token_definition[0]);
	return { token_definition, nb_token_rules };
}


std::vector<RuleDefinition> LangageGrammar::get_grammar_definition(void) {
	RuleDefinition rules_definition[] =
	{
		{ RULE_PROGRAM, { RULE_FUNCTION }, 
			[this](TokenValue& result, std::vector<TokenValue> p) { 
				result.program_value = new_program(); 
				result.program_value->add_function(p[0].function_value);
			}
		},

		// ex : bool fct_name(byte a, byte b) { return a+b; }
		{ RULE_FUNCTION, { RULE_FUNCTION_DEFINTION, RULE_CODEBLOC },
			[this](TokenValue& result, std::vector<TokenValue> p) {
				result.function_value = new_function(p[0].function_definition_value, p[1].code_block_value ); }
		},
		// ex : bool fct_name(bool a, byte b)
		{ RULE_FUNCTION_DEFINTION, { RULE_TYPE, TOKEN_IDENTIFIER, '(', RULE_N_PARAMETERS_DECL ,')'},
			[this](TokenValue& result, std::vector<TokenValue> p) {
				result.function_definition_value = new_function_definition(*p[0].type_value, *p[1].string_value, p[3].function_all_paramters_value);  }
		},
		// ex: bool a, byte b
		{ RULE_N_PARAMETERS_DECL , { RULE_N_PARAMETERS_DECL, ',', RULE_1_PARAMETER_DECL } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {  result.function_all_paramters_value = p[0].function_all_paramters_value;  
																 p[0].function_all_paramters_value->add(*p[2].function_paramter_value);  }
		},
		// ex : bool a
		{ RULE_N_PARAMETERS_DECL , { RULE_1_PARAMETER_DECL },  
			[this](TokenValue& result, std::vector<TokenValue> p) { result.function_all_paramters_value = new_function_all_parameters(p[0].function_paramter_value);  }
		},
		// ex : bool a 
		{ RULE_1_PARAMETER_DECL , { RULE_TYPE, TOKEN_IDENTIFIER } ,
			[this](TokenValue& result, std::vector<TokenValue> p) { result.function_paramter_value = new_function_parameter(*p[0].type_value, *p[1].string_value);  }
		},
		// ex { a++; return a; }
		{ RULE_CODEBLOC , { '{', RULE_N_STATEMENTS, '}'} ,
			[this](TokenValue& result, std::vector<TokenValue> p) { result.code_block_value = p[1].code_block_value; }
		},
		// ex a++; return a;
		{ RULE_N_STATEMENTS , {RULE_N_STATEMENTS, RULE_1_STATEMENT } ,
			[this](TokenValue& result, std::vector<TokenValue> p) {
				result.code_block_value = p[0].code_block_value;
				result.code_block_value->add_statement((p[1].statement_value));
			}
		},
		{ RULE_N_STATEMENTS , {RULE_1_STATEMENT } ,
			[this](TokenValue& result, std::vector<TokenValue> p) {
				result.code_block_value = new_code_bloc(p[0].statement_value);
			}
		},
		// ex: return a;
		{ RULE_1_STATEMENT , {TOKEN_RETURN, RULE_EXPRESSION, ';' } ,
			[this](TokenValue& result, std::vector<TokenValue> p) { 
				result.statement_value = new_retun_statement(p[1].expresison_value );
			}
		},
		// all expressions : a,123,a&b,a-1+b
		{ RULE_EXPRESSION , {RULE_VARIABLE} ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expresison_value = p[0].expresison_value; }	},
		{ RULE_EXPRESSION , {RULE_LITTERAL } ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expresison_value = p[0].expresison_value; }	},
		{ RULE_EXPRESSION , {RULE_OPERATOR_AND } ,	[this](TokenValue& result, std::vector<TokenValue> p) { result.expresison_value = p[0].expresison_value; }	},
		{ RULE_EXPRESSION , {RULE_OPERATOR_OR }  ,	[this](TokenValue& result, std::vector<TokenValue> p) { result.expresison_value = p[0].expresison_value; }	},
		//a&b
		{ RULE_OPERATOR_AND , {RULE_EXPRESSION, '&', RULE_EXPRESSION } ,
			[this](TokenValue& result, std::vector<TokenValue> p) {
				result.expresison_value = new_binary_operation( BinaryOperation::Operator::op_and, p[0].expresison_value, p[2].expresison_value);
			}
		},
		//a|b
		{ RULE_OPERATOR_OR , {RULE_EXPRESSION, '|', RULE_EXPRESSION } ,
			[this](TokenValue& result, std::vector<TokenValue> p) {
				result.expresison_value = new_binary_operation( BinaryOperation::Operator::op_or, p[0].expresison_value, p[2].expresison_value);
			}
		},
		// ex: a
		{ RULE_VARIABLE , { TOKEN_IDENTIFIER } ,
			[this](TokenValue& result, std::vector<TokenValue> p) { result.expresison_value = new_variable( *p[0].string_value);  }
		},
		// ex: 123
		{ RULE_LITTERAL , { TOKEN_NUMBER } ,
			[this](TokenValue& result, std::vector<TokenValue> p) { result.expresison_value = new_literal(Type::Native::int8, *p[0].string_value);  }
		},
		// bool
		{ RULE_TYPE , { TOKEN_TYPE_BOOL } ,
			[this](TokenValue& result, std::vector<TokenValue> ) { result.type_value = new_Type(Type::Native::bit);  }
		},
		// byte
		{ RULE_TYPE , { TOKEN_TYPE_BYTE } ,
			[this](TokenValue& result, std::vector<TokenValue> ) { result.type_value = new_Type(Type::Native::int8);  }
		},
	};
	int nb_grammar_rules = sizeof(rules_definition) / sizeof(rules_definition[0]);
	return std::vector<RuleDefinition>(rules_definition, rules_definition+nb_grammar_rules);
};
