#pragma once

// token definition (lexer)
enum E_TokenId {
	TOKEN_TYPE_BOOL			  = 257,
	TOKEN_TYPE_BYTE,		   // 258
	TOKEN_IDENTIFIER_FNNAME,   // 259 - function name
	TOKEN_IDENTIFIER_FNPARAM,  // 260 - in function parmetres only
	TOKEN_IDENTIFIER,	       // 261 - in function body statement only,
	TOKEN_IDENTIFIER_LOCALVAR, // 262 - in function body statement only, after a type: local var déclaration
	TOKEN_IDENTIFIER_SETVAR,   // 263 - in function body statement only, var name for affectation only.
	TOKEN_NUMBER,			   // 264
	TOKEN_RETURN,			   // 265
};
