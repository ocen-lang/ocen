#include "ctype.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "unistd.h"
#include "errno.h"
#include "dirent.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "setjmp.h"
#include "dirent.h"
#include "libgen.h"

/* Embed: std/./prelude.h */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

char* format_string(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int size = vsnprintf(NULL, 0, format, args);
  va_end(args);
  va_start(args, format);
  char* s = (char*)calloc(1, size + 1);
  vsprintf(s, format, args);
  s[size] = '\0';
  va_end(args);
  return s;
}

void ae_assert(int cond, char *dbg_msg, char *msg) {
  if (!cond) {
    printf("--------------------------------------------------------------------------------\n");
    printf("%s\n", dbg_msg);
    if (msg) {
      printf("  Message: %s\n", msg);
    }
    printf("--------------------------------------------------------------------------------\n");
    fflush(stdout);
    __builtin_trap();
  }
}

/* Enums */
typedef enum compiler_ast_operators_Operator {
  compiler_ast_operators_Operator_Address,
  compiler_ast_operators_Operator_Dereference,
  compiler_ast_operators_Operator_Negate,
  compiler_ast_operators_Operator_Not,
  compiler_ast_operators_Operator_BitwiseNot,
  compiler_ast_operators_Operator_IsNotNull,
  compiler_ast_operators_Operator_PreIncrement,
  compiler_ast_operators_Operator_PreDecrement,
  compiler_ast_operators_Operator_PostIncrement,
  compiler_ast_operators_Operator_PostDecrement,
  compiler_ast_operators_Operator_And,
  compiler_ast_operators_Operator_Assignment,
  compiler_ast_operators_Operator_BitwiseAnd,
  compiler_ast_operators_Operator_BitwiseOr,
  compiler_ast_operators_Operator_BitwiseXor,
  compiler_ast_operators_Operator_Divide,
  compiler_ast_operators_Operator_DivideEquals,
  compiler_ast_operators_Operator_Equals,
  compiler_ast_operators_Operator_GreaterThan,
  compiler_ast_operators_Operator_GreaterThanEquals,
  compiler_ast_operators_Operator_In,
  compiler_ast_operators_Operator_Index,
  compiler_ast_operators_Operator_LeftShift,
  compiler_ast_operators_Operator_LessThan,
  compiler_ast_operators_Operator_LessThanEquals,
  compiler_ast_operators_Operator_Minus,
  compiler_ast_operators_Operator_MinusEquals,
  compiler_ast_operators_Operator_Modulus,
  compiler_ast_operators_Operator_Multiply,
  compiler_ast_operators_Operator_MultiplyEquals,
  compiler_ast_operators_Operator_NotEquals,
  compiler_ast_operators_Operator_Or,
  compiler_ast_operators_Operator_Plus,
  compiler_ast_operators_Operator_PlusEquals,
  compiler_ast_operators_Operator_RightShift,
  compiler_ast_operators_Operator_LeftShiftEquals,
  compiler_ast_operators_Operator_RightShiftEquals,
  compiler_ast_operators_Operator_IndexAssign,
  compiler_ast_operators_Operator_Error,
} compiler_ast_operators_Operator;

char *compiler_ast_operators_Operator_dbg(compiler_ast_operators_Operator this) {
  switch (this) {
    case compiler_ast_operators_Operator_Address: return "Address";
    case compiler_ast_operators_Operator_Dereference: return "Dereference";
    case compiler_ast_operators_Operator_Negate: return "Negate";
    case compiler_ast_operators_Operator_Not: return "Not";
    case compiler_ast_operators_Operator_BitwiseNot: return "BitwiseNot";
    case compiler_ast_operators_Operator_IsNotNull: return "IsNotNull";
    case compiler_ast_operators_Operator_PreIncrement: return "PreIncrement";
    case compiler_ast_operators_Operator_PreDecrement: return "PreDecrement";
    case compiler_ast_operators_Operator_PostIncrement: return "PostIncrement";
    case compiler_ast_operators_Operator_PostDecrement: return "PostDecrement";
    case compiler_ast_operators_Operator_And: return "And";
    case compiler_ast_operators_Operator_Assignment: return "Assignment";
    case compiler_ast_operators_Operator_BitwiseAnd: return "BitwiseAnd";
    case compiler_ast_operators_Operator_BitwiseOr: return "BitwiseOr";
    case compiler_ast_operators_Operator_BitwiseXor: return "BitwiseXor";
    case compiler_ast_operators_Operator_Divide: return "Divide";
    case compiler_ast_operators_Operator_DivideEquals: return "DivideEquals";
    case compiler_ast_operators_Operator_Equals: return "Equals";
    case compiler_ast_operators_Operator_GreaterThan: return "GreaterThan";
    case compiler_ast_operators_Operator_GreaterThanEquals: return "GreaterThanEquals";
    case compiler_ast_operators_Operator_In: return "In";
    case compiler_ast_operators_Operator_Index: return "Index";
    case compiler_ast_operators_Operator_LeftShift: return "LeftShift";
    case compiler_ast_operators_Operator_LessThan: return "LessThan";
    case compiler_ast_operators_Operator_LessThanEquals: return "LessThanEquals";
    case compiler_ast_operators_Operator_Minus: return "Minus";
    case compiler_ast_operators_Operator_MinusEquals: return "MinusEquals";
    case compiler_ast_operators_Operator_Modulus: return "Modulus";
    case compiler_ast_operators_Operator_Multiply: return "Multiply";
    case compiler_ast_operators_Operator_MultiplyEquals: return "MultiplyEquals";
    case compiler_ast_operators_Operator_NotEquals: return "NotEquals";
    case compiler_ast_operators_Operator_Or: return "Or";
    case compiler_ast_operators_Operator_Plus: return "Plus";
    case compiler_ast_operators_Operator_PlusEquals: return "PlusEquals";
    case compiler_ast_operators_Operator_RightShift: return "RightShift";
    case compiler_ast_operators_Operator_LeftShiftEquals: return "LeftShiftEquals";
    case compiler_ast_operators_Operator_RightShiftEquals: return "RightShiftEquals";
    case compiler_ast_operators_Operator_IndexAssign: return "IndexAssign";
    case compiler_ast_operators_Operator_Error: return "Error";
    default: return "<unknown>";
  }
}

typedef enum compiler_ast_nodes_ASTType {
  compiler_ast_nodes_ASTType_Assert,
  compiler_ast_nodes_ASTType_Block,
  compiler_ast_nodes_ASTType_BoolLiteral,
  compiler_ast_nodes_ASTType_Break,
  compiler_ast_nodes_ASTType_Call,
  compiler_ast_nodes_ASTType_Constant,
  compiler_ast_nodes_ASTType_Continue,
  compiler_ast_nodes_ASTType_Error,
  compiler_ast_nodes_ASTType_Identifier,
  compiler_ast_nodes_ASTType_If,
  compiler_ast_nodes_ASTType_Import,
  compiler_ast_nodes_ASTType_IntLiteral,
  compiler_ast_nodes_ASTType_Member,
  compiler_ast_nodes_ASTType_NSLookup,
  compiler_ast_nodes_ASTType_OverloadedOperator,
  compiler_ast_nodes_ASTType_Return,
  compiler_ast_nodes_ASTType_Yield,
  compiler_ast_nodes_ASTType_StringLiteral,
  compiler_ast_nodes_ASTType_SizeOf,
  compiler_ast_nodes_ASTType_VarDeclaration,
  compiler_ast_nodes_ASTType_While,
  compiler_ast_nodes_ASTType_For,
  compiler_ast_nodes_ASTType_CharLiteral,
  compiler_ast_nodes_ASTType_FloatLiteral,
  compiler_ast_nodes_ASTType_FormatStringLiteral,
  compiler_ast_nodes_ASTType_Cast,
  compiler_ast_nodes_ASTType_Null,
  compiler_ast_nodes_ASTType_MethodCall,
  compiler_ast_nodes_ASTType_Match,
  compiler_ast_nodes_ASTType_Defer,
  compiler_ast_nodes_ASTType_Specialization,
  compiler_ast_nodes_ASTType_ArrayLiteral,
  compiler_ast_nodes_ASTType_UnaryOp,
  compiler_ast_nodes_ASTType_BinaryOp,
} compiler_ast_nodes_ASTType;

char *compiler_ast_nodes_ASTType_dbg(compiler_ast_nodes_ASTType this) {
  switch (this) {
    case compiler_ast_nodes_ASTType_Assert: return "Assert";
    case compiler_ast_nodes_ASTType_Block: return "Block";
    case compiler_ast_nodes_ASTType_BoolLiteral: return "BoolLiteral";
    case compiler_ast_nodes_ASTType_Break: return "Break";
    case compiler_ast_nodes_ASTType_Call: return "Call";
    case compiler_ast_nodes_ASTType_Constant: return "Constant";
    case compiler_ast_nodes_ASTType_Continue: return "Continue";
    case compiler_ast_nodes_ASTType_Error: return "Error";
    case compiler_ast_nodes_ASTType_Identifier: return "Identifier";
    case compiler_ast_nodes_ASTType_If: return "If";
    case compiler_ast_nodes_ASTType_Import: return "Import";
    case compiler_ast_nodes_ASTType_IntLiteral: return "IntLiteral";
    case compiler_ast_nodes_ASTType_Member: return "Member";
    case compiler_ast_nodes_ASTType_NSLookup: return "NSLookup";
    case compiler_ast_nodes_ASTType_OverloadedOperator: return "OverloadedOperator";
    case compiler_ast_nodes_ASTType_Return: return "Return";
    case compiler_ast_nodes_ASTType_Yield: return "Yield";
    case compiler_ast_nodes_ASTType_StringLiteral: return "StringLiteral";
    case compiler_ast_nodes_ASTType_SizeOf: return "SizeOf";
    case compiler_ast_nodes_ASTType_VarDeclaration: return "VarDeclaration";
    case compiler_ast_nodes_ASTType_While: return "While";
    case compiler_ast_nodes_ASTType_For: return "For";
    case compiler_ast_nodes_ASTType_CharLiteral: return "CharLiteral";
    case compiler_ast_nodes_ASTType_FloatLiteral: return "FloatLiteral";
    case compiler_ast_nodes_ASTType_FormatStringLiteral: return "FormatStringLiteral";
    case compiler_ast_nodes_ASTType_Cast: return "Cast";
    case compiler_ast_nodes_ASTType_Null: return "Null";
    case compiler_ast_nodes_ASTType_MethodCall: return "MethodCall";
    case compiler_ast_nodes_ASTType_Match: return "Match";
    case compiler_ast_nodes_ASTType_Defer: return "Defer";
    case compiler_ast_nodes_ASTType_Specialization: return "Specialization";
    case compiler_ast_nodes_ASTType_ArrayLiteral: return "ArrayLiteral";
    case compiler_ast_nodes_ASTType_UnaryOp: return "UnaryOp";
    case compiler_ast_nodes_ASTType_BinaryOp: return "BinaryOp";
    default: return "<unknown>";
  }
}

typedef enum compiler_ast_nodes_ImportPartType {
  compiler_ast_nodes_ImportPartType_Single,
  compiler_ast_nodes_ImportPartType_Multiple,
  compiler_ast_nodes_ImportPartType_Wildcard,
} compiler_ast_nodes_ImportPartType;

char *compiler_ast_nodes_ImportPartType_dbg(compiler_ast_nodes_ImportPartType this) {
  switch (this) {
    case compiler_ast_nodes_ImportPartType_Single: return "Single";
    case compiler_ast_nodes_ImportPartType_Multiple: return "Multiple";
    case compiler_ast_nodes_ImportPartType_Wildcard: return "Wildcard";
    default: return "<unknown>";
  }
}

typedef enum compiler_ast_nodes_ImportType {
  compiler_ast_nodes_ImportType_GlobalNamespace,
  compiler_ast_nodes_ImportType_ProjectNamespace,
  compiler_ast_nodes_ImportType_ParentNamespace,
  compiler_ast_nodes_ImportType_CurrentScope,
} compiler_ast_nodes_ImportType;

char *compiler_ast_nodes_ImportType_dbg(compiler_ast_nodes_ImportType this) {
  switch (this) {
    case compiler_ast_nodes_ImportType_GlobalNamespace: return "GlobalNamespace";
    case compiler_ast_nodes_ImportType_ProjectNamespace: return "ProjectNamespace";
    case compiler_ast_nodes_ImportType_ParentNamespace: return "ParentNamespace";
    case compiler_ast_nodes_ImportType_CurrentScope: return "CurrentScope";
    default: return "<unknown>";
  }
}

typedef enum compiler_ast_scopes_SymbolType {
  compiler_ast_scopes_SymbolType_Function,
  compiler_ast_scopes_SymbolType_Structure,
  compiler_ast_scopes_SymbolType_Enum,
  compiler_ast_scopes_SymbolType_EnumVariant,
  compiler_ast_scopes_SymbolType_TypeDef,
  compiler_ast_scopes_SymbolType_Namespace,
  compiler_ast_scopes_SymbolType_Variable,
  compiler_ast_scopes_SymbolType_Constant,
} compiler_ast_scopes_SymbolType;

char *compiler_ast_scopes_SymbolType_dbg(compiler_ast_scopes_SymbolType this) {
  switch (this) {
    case compiler_ast_scopes_SymbolType_Function: return "Function";
    case compiler_ast_scopes_SymbolType_Structure: return "Structure";
    case compiler_ast_scopes_SymbolType_Enum: return "Enum";
    case compiler_ast_scopes_SymbolType_EnumVariant: return "EnumVariant";
    case compiler_ast_scopes_SymbolType_TypeDef: return "TypeDef";
    case compiler_ast_scopes_SymbolType_Namespace: return "Namespace";
    case compiler_ast_scopes_SymbolType_Variable: return "Variable";
    case compiler_ast_scopes_SymbolType_Constant: return "Constant";
    default: return "<unknown>";
  }
}

typedef enum compiler_ast_scopes_ReferenceType {
  compiler_ast_scopes_ReferenceType_Normal,
  compiler_ast_scopes_ReferenceType_OperatorOverload,
  compiler_ast_scopes_ReferenceType_DotShorthand,
} compiler_ast_scopes_ReferenceType;

char *compiler_ast_scopes_ReferenceType_dbg(compiler_ast_scopes_ReferenceType this) {
  switch (this) {
    case compiler_ast_scopes_ReferenceType_Normal: return "Normal";
    case compiler_ast_scopes_ReferenceType_OperatorOverload: return "OperatorOverload";
    case compiler_ast_scopes_ReferenceType_DotShorthand: return "DotShorthand";
    default: return "<unknown>";
  }
}

typedef enum compiler_lsp_CommandType {
  compiler_lsp_CommandType_Hover,
  compiler_lsp_CommandType_GoToDefinition,
  compiler_lsp_CommandType_GoToType,
  compiler_lsp_CommandType_None,
  compiler_lsp_CommandType_DocumentSymbols,
  compiler_lsp_CommandType_Completions,
  compiler_lsp_CommandType_References,
  compiler_lsp_CommandType_Renames,
  compiler_lsp_CommandType_SignatureHelp,
  compiler_lsp_CommandType_Validate,
} compiler_lsp_CommandType;

char *compiler_lsp_CommandType_dbg(compiler_lsp_CommandType this) {
  switch (this) {
    case compiler_lsp_CommandType_Hover: return "Hover";
    case compiler_lsp_CommandType_GoToDefinition: return "GoToDefinition";
    case compiler_lsp_CommandType_GoToType: return "GoToType";
    case compiler_lsp_CommandType_None: return "None";
    case compiler_lsp_CommandType_DocumentSymbols: return "DocumentSymbols";
    case compiler_lsp_CommandType_Completions: return "Completions";
    case compiler_lsp_CommandType_References: return "References";
    case compiler_lsp_CommandType_Renames: return "Renames";
    case compiler_lsp_CommandType_SignatureHelp: return "SignatureHelp";
    case compiler_lsp_CommandType_Validate: return "Validate";
    default: return "<unknown>";
  }
}

typedef enum compiler_attributes_AttributeType {
  compiler_attributes_AttributeType_Extern,
  compiler_attributes_AttributeType_Exits,
  compiler_attributes_AttributeType_VariadicFormat,
  compiler_attributes_AttributeType_Export,
  compiler_attributes_AttributeType_Formatting,
  compiler_attributes_AttributeType_Operator,
  compiler_attributes_AttributeType_Invalid,
} compiler_attributes_AttributeType;

char *compiler_attributes_AttributeType_dbg(compiler_attributes_AttributeType this) {
  switch (this) {
    case compiler_attributes_AttributeType_Extern: return "Extern";
    case compiler_attributes_AttributeType_Exits: return "Exits";
    case compiler_attributes_AttributeType_VariadicFormat: return "VariadicFormat";
    case compiler_attributes_AttributeType_Export: return "Export";
    case compiler_attributes_AttributeType_Formatting: return "Formatting";
    case compiler_attributes_AttributeType_Operator: return "Operator";
    case compiler_attributes_AttributeType_Invalid: return "Invalid";
    default: return "<unknown>";
  }
}

typedef enum compiler_tokens_TokenType {
  compiler_tokens_TokenType_AtSign,
  compiler_tokens_TokenType_Ampersand,
  compiler_tokens_TokenType_Backtick,
  compiler_tokens_TokenType_Caret,
  compiler_tokens_TokenType_CharLiteral,
  compiler_tokens_TokenType_CloseCurly,
  compiler_tokens_TokenType_CloseParen,
  compiler_tokens_TokenType_CloseSquare,
  compiler_tokens_TokenType_Colon,
  compiler_tokens_TokenType_ColonColon,
  compiler_tokens_TokenType_Comma,
  compiler_tokens_TokenType_Dot,
  compiler_tokens_TokenType_EOF,
  compiler_tokens_TokenType_Ellipsis,
  compiler_tokens_TokenType_Equals,
  compiler_tokens_TokenType_EqualEquals,
  compiler_tokens_TokenType_Exclamation,
  compiler_tokens_TokenType_FatArrow,
  compiler_tokens_TokenType_FloatLiteral,
  compiler_tokens_TokenType_FormatStringLiteral,
  compiler_tokens_TokenType_GreaterThan,
  compiler_tokens_TokenType_GreaterThanEquals,
  compiler_tokens_TokenType_Identifier,
  compiler_tokens_TokenType_IntLiteral,
  compiler_tokens_TokenType_LessThan,
  compiler_tokens_TokenType_LessThanEquals,
  compiler_tokens_TokenType_Line,
  compiler_tokens_TokenType_Minus,
  compiler_tokens_TokenType_MinusEquals,
  compiler_tokens_TokenType_MinusMinus,
  compiler_tokens_TokenType_NotEquals,
  compiler_tokens_TokenType_OpenCurly,
  compiler_tokens_TokenType_OpenParen,
  compiler_tokens_TokenType_OpenSquare,
  compiler_tokens_TokenType_Percent,
  compiler_tokens_TokenType_Plus,
  compiler_tokens_TokenType_PlusEquals,
  compiler_tokens_TokenType_PlusPlus,
  compiler_tokens_TokenType_Question,
  compiler_tokens_TokenType_Semicolon,
  compiler_tokens_TokenType_Slash,
  compiler_tokens_TokenType_SlashEquals,
  compiler_tokens_TokenType_Star,
  compiler_tokens_TokenType_StarEquals,
  compiler_tokens_TokenType_StringLiteral,
  compiler_tokens_TokenType_Tilde,
  compiler_tokens_TokenType_Newline,
  compiler_tokens_TokenType_BEGIN_KEYWORDS,
  compiler_tokens_TokenType_And,
  compiler_tokens_TokenType_As,
  compiler_tokens_TokenType_Assert,
  compiler_tokens_TokenType_Break,
  compiler_tokens_TokenType_Const,
  compiler_tokens_TokenType_Continue,
  compiler_tokens_TokenType_Def,
  compiler_tokens_TokenType_Defer,
  compiler_tokens_TokenType_Else,
  compiler_tokens_TokenType_Enum,
  compiler_tokens_TokenType_Extern,
  compiler_tokens_TokenType_False,
  compiler_tokens_TokenType_For,
  compiler_tokens_TokenType_Fn,
  compiler_tokens_TokenType_If,
  compiler_tokens_TokenType_Let,
  compiler_tokens_TokenType_Match,
  compiler_tokens_TokenType_Namespace,
  compiler_tokens_TokenType_Null,
  compiler_tokens_TokenType_Not,
  compiler_tokens_TokenType_Or,
  compiler_tokens_TokenType_Return,
  compiler_tokens_TokenType_SizeOf,
  compiler_tokens_TokenType_Struct,
  compiler_tokens_TokenType_True,
  compiler_tokens_TokenType_Then,
  compiler_tokens_TokenType_TypeDef,
  compiler_tokens_TokenType_Union,
  compiler_tokens_TokenType_Import,
  compiler_tokens_TokenType_Void,
  compiler_tokens_TokenType_Yield,
  compiler_tokens_TokenType_While,
} compiler_tokens_TokenType;

char *compiler_tokens_TokenType_dbg(compiler_tokens_TokenType this) {
  switch (this) {
    case compiler_tokens_TokenType_AtSign: return "AtSign";
    case compiler_tokens_TokenType_Ampersand: return "Ampersand";
    case compiler_tokens_TokenType_Backtick: return "Backtick";
    case compiler_tokens_TokenType_Caret: return "Caret";
    case compiler_tokens_TokenType_CharLiteral: return "CharLiteral";
    case compiler_tokens_TokenType_CloseCurly: return "CloseCurly";
    case compiler_tokens_TokenType_CloseParen: return "CloseParen";
    case compiler_tokens_TokenType_CloseSquare: return "CloseSquare";
    case compiler_tokens_TokenType_Colon: return "Colon";
    case compiler_tokens_TokenType_ColonColon: return "ColonColon";
    case compiler_tokens_TokenType_Comma: return "Comma";
    case compiler_tokens_TokenType_Dot: return "Dot";
    case compiler_tokens_TokenType_EOF: return "EOF";
    case compiler_tokens_TokenType_Ellipsis: return "Ellipsis";
    case compiler_tokens_TokenType_Equals: return "Equals";
    case compiler_tokens_TokenType_EqualEquals: return "EqualEquals";
    case compiler_tokens_TokenType_Exclamation: return "Exclamation";
    case compiler_tokens_TokenType_FatArrow: return "FatArrow";
    case compiler_tokens_TokenType_FloatLiteral: return "FloatLiteral";
    case compiler_tokens_TokenType_FormatStringLiteral: return "FormatStringLiteral";
    case compiler_tokens_TokenType_GreaterThan: return "GreaterThan";
    case compiler_tokens_TokenType_GreaterThanEquals: return "GreaterThanEquals";
    case compiler_tokens_TokenType_Identifier: return "Identifier";
    case compiler_tokens_TokenType_IntLiteral: return "IntLiteral";
    case compiler_tokens_TokenType_LessThan: return "LessThan";
    case compiler_tokens_TokenType_LessThanEquals: return "LessThanEquals";
    case compiler_tokens_TokenType_Line: return "Line";
    case compiler_tokens_TokenType_Minus: return "Minus";
    case compiler_tokens_TokenType_MinusEquals: return "MinusEquals";
    case compiler_tokens_TokenType_MinusMinus: return "MinusMinus";
    case compiler_tokens_TokenType_NotEquals: return "NotEquals";
    case compiler_tokens_TokenType_OpenCurly: return "OpenCurly";
    case compiler_tokens_TokenType_OpenParen: return "OpenParen";
    case compiler_tokens_TokenType_OpenSquare: return "OpenSquare";
    case compiler_tokens_TokenType_Percent: return "Percent";
    case compiler_tokens_TokenType_Plus: return "Plus";
    case compiler_tokens_TokenType_PlusEquals: return "PlusEquals";
    case compiler_tokens_TokenType_PlusPlus: return "PlusPlus";
    case compiler_tokens_TokenType_Question: return "Question";
    case compiler_tokens_TokenType_Semicolon: return "Semicolon";
    case compiler_tokens_TokenType_Slash: return "Slash";
    case compiler_tokens_TokenType_SlashEquals: return "SlashEquals";
    case compiler_tokens_TokenType_Star: return "Star";
    case compiler_tokens_TokenType_StarEquals: return "StarEquals";
    case compiler_tokens_TokenType_StringLiteral: return "StringLiteral";
    case compiler_tokens_TokenType_Tilde: return "Tilde";
    case compiler_tokens_TokenType_Newline: return "Newline";
    case compiler_tokens_TokenType_BEGIN_KEYWORDS: return "BEGIN_KEYWORDS";
    case compiler_tokens_TokenType_And: return "And";
    case compiler_tokens_TokenType_As: return "As";
    case compiler_tokens_TokenType_Assert: return "Assert";
    case compiler_tokens_TokenType_Break: return "Break";
    case compiler_tokens_TokenType_Const: return "Const";
    case compiler_tokens_TokenType_Continue: return "Continue";
    case compiler_tokens_TokenType_Def: return "Def";
    case compiler_tokens_TokenType_Defer: return "Defer";
    case compiler_tokens_TokenType_Else: return "Else";
    case compiler_tokens_TokenType_Enum: return "Enum";
    case compiler_tokens_TokenType_Extern: return "Extern";
    case compiler_tokens_TokenType_False: return "False";
    case compiler_tokens_TokenType_For: return "For";
    case compiler_tokens_TokenType_Fn: return "Fn";
    case compiler_tokens_TokenType_If: return "If";
    case compiler_tokens_TokenType_Let: return "Let";
    case compiler_tokens_TokenType_Match: return "Match";
    case compiler_tokens_TokenType_Namespace: return "Namespace";
    case compiler_tokens_TokenType_Null: return "Null";
    case compiler_tokens_TokenType_Not: return "Not";
    case compiler_tokens_TokenType_Or: return "Or";
    case compiler_tokens_TokenType_Return: return "Return";
    case compiler_tokens_TokenType_SizeOf: return "SizeOf";
    case compiler_tokens_TokenType_Struct: return "Struct";
    case compiler_tokens_TokenType_True: return "True";
    case compiler_tokens_TokenType_Then: return "Then";
    case compiler_tokens_TokenType_TypeDef: return "TypeDef";
    case compiler_tokens_TokenType_Union: return "Union";
    case compiler_tokens_TokenType_Import: return "Import";
    case compiler_tokens_TokenType_Void: return "Void";
    case compiler_tokens_TokenType_Yield: return "Yield";
    case compiler_tokens_TokenType_While: return "While";
    default: return "<unknown>";
  }
}

typedef enum compiler_errors_ErrorType {
  compiler_errors_ErrorType_Standard,
  compiler_errors_ErrorType_WithNote,
  compiler_errors_ErrorType_WithHint,
} compiler_errors_ErrorType;

char *compiler_errors_ErrorType_dbg(compiler_errors_ErrorType this) {
  switch (this) {
    case compiler_errors_ErrorType_Standard: return "Standard";
    case compiler_errors_ErrorType_WithNote: return "WithNote";
    case compiler_errors_ErrorType_WithHint: return "WithHint";
    default: return "<unknown>";
  }
}

typedef enum compiler_errors_MessageType {
  compiler_errors_MessageType_Error,
  compiler_errors_MessageType_Warning,
  compiler_errors_MessageType_Note,
} compiler_errors_MessageType;

char *compiler_errors_MessageType_dbg(compiler_errors_MessageType this) {
  switch (this) {
    case compiler_errors_MessageType_Error: return "Error";
    case compiler_errors_MessageType_Warning: return "Warning";
    case compiler_errors_MessageType_Note: return "Note";
    default: return "<unknown>";
  }
}

typedef enum compiler_types_BaseType {
  compiler_types_BaseType_Char,
  compiler_types_BaseType_Bool,
  compiler_types_BaseType_Void,
  compiler_types_BaseType_I8,
  compiler_types_BaseType_I16,
  compiler_types_BaseType_I32,
  compiler_types_BaseType_I64,
  compiler_types_BaseType_U8,
  compiler_types_BaseType_U16,
  compiler_types_BaseType_U32,
  compiler_types_BaseType_U64,
  compiler_types_BaseType_F32,
  compiler_types_BaseType_F64,
  compiler_types_BaseType_NUM_BASE_TYPES,
  compiler_types_BaseType_Pointer,
  compiler_types_BaseType_Function,
  compiler_types_BaseType_Structure,
  compiler_types_BaseType_Enum,
  compiler_types_BaseType_Unresolved,
  compiler_types_BaseType_Array,
  compiler_types_BaseType_Alias,
  compiler_types_BaseType_UnresolvedTemplate,
  compiler_types_BaseType_Error,
} compiler_types_BaseType;

char *compiler_types_BaseType_dbg(compiler_types_BaseType this) {
  switch (this) {
    case compiler_types_BaseType_Char: return "Char";
    case compiler_types_BaseType_Bool: return "Bool";
    case compiler_types_BaseType_Void: return "Void";
    case compiler_types_BaseType_I8: return "I8";
    case compiler_types_BaseType_I16: return "I16";
    case compiler_types_BaseType_I32: return "I32";
    case compiler_types_BaseType_I64: return "I64";
    case compiler_types_BaseType_U8: return "U8";
    case compiler_types_BaseType_U16: return "U16";
    case compiler_types_BaseType_U32: return "U32";
    case compiler_types_BaseType_U64: return "U64";
    case compiler_types_BaseType_F32: return "F32";
    case compiler_types_BaseType_F64: return "F64";
    case compiler_types_BaseType_NUM_BASE_TYPES: return "NUM_BASE_TYPES";
    case compiler_types_BaseType_Pointer: return "Pointer";
    case compiler_types_BaseType_Function: return "Function";
    case compiler_types_BaseType_Structure: return "Structure";
    case compiler_types_BaseType_Enum: return "Enum";
    case compiler_types_BaseType_Unresolved: return "Unresolved";
    case compiler_types_BaseType_Array: return "Array";
    case compiler_types_BaseType_Alias: return "Alias";
    case compiler_types_BaseType_UnresolvedTemplate: return "UnresolvedTemplate";
    case compiler_types_BaseType_Error: return "Error";
    default: return "<unknown>";
  }
}

typedef enum std_value_ValueType {
  std_value_ValueType_Null,
  std_value_ValueType_Bool,
  std_value_ValueType_Integer,
  std_value_ValueType_Float,
  std_value_ValueType_String,
  std_value_ValueType_List,
  std_value_ValueType_Dictionary,
} std_value_ValueType;

char *std_value_ValueType_dbg(std_value_ValueType this) {
  switch (this) {
    case std_value_ValueType_Null: return "Null";
    case std_value_ValueType_Bool: return "Bool";
    case std_value_ValueType_Integer: return "Integer";
    case std_value_ValueType_Float: return "Float";
    case std_value_ValueType_String: return "String";
    case std_value_ValueType_List: return "List";
    case std_value_ValueType_Dictionary: return "Dictionary";
    default: return "<unknown>";
  }
}

char *std_fs_SeekMode_dbg(i32 this) {
  switch (this) {
    case SEEK_SET: return "Set";
    case SEEK_CUR: return "Cur";
    case SEEK_END: return "End";
    default: return "<unknown>";
  }
}

/* Constants */
#define compiler_utils_MAX_N (128)
#define compiler_utils_MAX_M (128)
#define std_compact_map_INDEX_FREE (-1)
#define std_compact_map_INDEX_DELETED (-2)
/* Struct typedefs */
typedef struct compiler_docgen_DocGenerator compiler_docgen_DocGenerator;
typedef struct std_buffer_Buffer std_buffer_Buffer;
typedef struct compiler_passes_code_generator_CodeGenerator compiler_passes_code_generator_CodeGenerator;
typedef struct compiler_passes_reorder_structs_ReorderStructs compiler_passes_reorder_structs_ReorderStructs;
typedef struct compiler_passes_typechecker_TypeChecker compiler_passes_typechecker_TypeChecker;
typedef struct compiler_passes_register_types_RegisterTypes compiler_passes_register_types_RegisterTypes;
typedef struct compiler_passes_register_types_Finder compiler_passes_register_types_Finder;
typedef struct compiler_passes_mark_dead_code_MarkDeadCode compiler_passes_mark_dead_code_MarkDeadCode;
typedef struct compiler_passes_generic_pass_GenericPass compiler_passes_generic_pass_GenericPass;
typedef struct std_span_Location std_span_Location;
typedef struct std_span_Span std_span_Span;
typedef struct compiler_parser_Parser compiler_parser_Parser;
typedef struct compiler_lexer_Lexer compiler_lexer_Lexer;
typedef struct compiler_ast_program_Namespace compiler_ast_program_Namespace;
typedef struct compiler_ast_program_CachedTypes compiler_ast_program_CachedTypes;
typedef struct compiler_ast_program_Program compiler_ast_program_Program;
typedef struct compiler_ast_program_NSIterator compiler_ast_program_NSIterator;
typedef struct compiler_ast_operators_OperatorOverload compiler_ast_operators_OperatorOverload;
typedef struct compiler_ast_nodes_Variable compiler_ast_nodes_Variable;
typedef struct compiler_ast_nodes_VarDeclaration compiler_ast_nodes_VarDeclaration;
typedef struct compiler_ast_nodes_Structure compiler_ast_nodes_Structure;
typedef struct compiler_ast_nodes_Enum compiler_ast_nodes_Enum;
typedef struct compiler_ast_nodes_Function compiler_ast_nodes_Function;
typedef struct compiler_ast_nodes_Block compiler_ast_nodes_Block;
typedef struct compiler_ast_nodes_Identifier compiler_ast_nodes_Identifier;
typedef struct compiler_ast_nodes_Argument compiler_ast_nodes_Argument;
typedef struct compiler_ast_nodes_FuncCall compiler_ast_nodes_FuncCall;
typedef struct compiler_ast_nodes_ImportPartSingle compiler_ast_nodes_ImportPartSingle;
typedef union compiler_ast_nodes_ImportPartUnion compiler_ast_nodes_ImportPartUnion;
typedef struct compiler_ast_nodes_ImportPart compiler_ast_nodes_ImportPart;
typedef struct compiler_ast_nodes_Import compiler_ast_nodes_Import;
typedef struct compiler_ast_nodes_NumLiteral compiler_ast_nodes_NumLiteral;
typedef struct compiler_ast_nodes_Binary compiler_ast_nodes_Binary;
typedef struct compiler_ast_nodes_Unary compiler_ast_nodes_Unary;
typedef struct compiler_ast_nodes_NSLookup compiler_ast_nodes_NSLookup;
typedef struct compiler_ast_nodes_Member compiler_ast_nodes_Member;
typedef struct compiler_ast_nodes_Assertion compiler_ast_nodes_Assertion;
typedef struct compiler_ast_nodes_IfBranch compiler_ast_nodes_IfBranch;
typedef struct compiler_ast_nodes_IfStatement compiler_ast_nodes_IfStatement;
typedef struct compiler_ast_nodes_Loop compiler_ast_nodes_Loop;
typedef struct compiler_ast_nodes_Cast compiler_ast_nodes_Cast;
typedef struct compiler_ast_nodes_FormatString compiler_ast_nodes_FormatString;
typedef struct compiler_ast_nodes_MatchCase compiler_ast_nodes_MatchCase;
typedef struct compiler_ast_nodes_Match compiler_ast_nodes_Match;
typedef struct compiler_ast_nodes_Specialization compiler_ast_nodes_Specialization;
typedef struct compiler_ast_nodes_ArrayLiteral compiler_ast_nodes_ArrayLiteral;
typedef union compiler_ast_nodes_ASTUnion compiler_ast_nodes_ASTUnion;
typedef struct compiler_ast_nodes_AST compiler_ast_nodes_AST;
typedef struct compiler_ast_scopes_TemplateInstance compiler_ast_scopes_TemplateInstance;
typedef struct compiler_ast_scopes_Template compiler_ast_scopes_Template;
typedef struct compiler_ast_scopes_Reference compiler_ast_scopes_Reference;
typedef union compiler_ast_scopes_SymbolUnion compiler_ast_scopes_SymbolUnion;
typedef struct compiler_ast_scopes_Symbol compiler_ast_scopes_Symbol;
typedef struct compiler_ast_scopes_Scope compiler_ast_scopes_Scope;
typedef struct compiler_lsp_finder_Finder compiler_lsp_finder_Finder;
typedef struct compiler_attributes_Attribute compiler_attributes_Attribute;
typedef struct compiler_tokens_Token compiler_tokens_Token;
typedef struct compiler_errors_Error compiler_errors_Error;
typedef struct compiler_types_FunctionType compiler_types_FunctionType;
typedef struct compiler_types_ArrayType compiler_types_ArrayType;
typedef struct compiler_types_UnresolvedTemplate compiler_types_UnresolvedTemplate;
typedef struct compiler_types_TypeUnion compiler_types_TypeUnion;
typedef struct compiler_types_Type compiler_types_Type;
typedef struct std_CharIterator std_CharIterator;
typedef struct std_sv_SV std_sv_SV;
typedef struct std_sv_SVLineIterator std_sv_SVLineIterator;
typedef struct std_compact_map_Item__0 std_compact_map_Item__0;
typedef struct std_compact_map_Map__0 std_compact_map_Map__0;
typedef struct std_vector_Iterator__23 std_vector_Iterator__23;
typedef struct std_compact_map_Iterator__0 std_compact_map_Iterator__0;
typedef struct std_set_Set__0 std_set_Set__0;
typedef struct std_set_Set__1 std_set_Set__1;
typedef struct std_set_Set__2 std_set_Set__2;
typedef struct std_map_Iterator__9 std_map_Iterator__9;
typedef struct std_set_Iterator__2 std_set_Iterator__2;
typedef union std_value_ValueUnion std_value_ValueUnion;
typedef struct std_value_Value std_value_Value;
typedef struct std_map_Item__0 std_map_Item__0;
typedef struct std_map_Item__1 std_map_Item__1;
typedef struct std_map_Item__2 std_map_Item__2;
typedef struct std_map_Item__3 std_map_Item__3;
typedef struct std_map_Item__4 std_map_Item__4;
typedef struct std_map_Item__5 std_map_Item__5;
typedef struct std_map_Item__6 std_map_Item__6;
typedef struct std_map_Item__7 std_map_Item__7;
typedef struct std_map_Item__8 std_map_Item__8;
typedef struct std_map_Item__9 std_map_Item__9;
typedef struct std_map_Map__0 std_map_Map__0;
typedef struct std_map_Map__1 std_map_Map__1;
typedef struct std_map_Map__2 std_map_Map__2;
typedef struct std_map_Map__3 std_map_Map__3;
typedef struct std_map_Map__4 std_map_Map__4;
typedef struct std_map_Map__5 std_map_Map__5;
typedef struct std_map_Map__6 std_map_Map__6;
typedef struct std_map_Map__7 std_map_Map__7;
typedef struct std_map_Map__8 std_map_Map__8;
typedef struct std_map_Map__9 std_map_Map__9;
typedef struct std_map_Iterator__2 std_map_Iterator__2;
typedef struct std_map_Iterator__3 std_map_Iterator__3;
typedef struct std_map_Iterator__4 std_map_Iterator__4;
typedef struct std_map_Iterator__6 std_map_Iterator__6;
typedef struct std_map_Iterator__7 std_map_Iterator__7;
typedef struct std_map_ValueIterator__3 std_map_ValueIterator__3;
typedef struct std_map_ValueIterator__7 std_map_ValueIterator__7;
typedef struct std_vector_Vector__0 std_vector_Vector__0;
typedef struct std_vector_Vector__1 std_vector_Vector__1;
typedef struct std_vector_Vector__2 std_vector_Vector__2;
typedef struct std_vector_Vector__3 std_vector_Vector__3;
typedef struct std_vector_Vector__4 std_vector_Vector__4;
typedef struct std_vector_Vector__5 std_vector_Vector__5;
typedef struct std_vector_Vector__6 std_vector_Vector__6;
typedef struct std_vector_Vector__7 std_vector_Vector__7;
typedef struct std_vector_Vector__8 std_vector_Vector__8;
typedef struct std_vector_Vector__9 std_vector_Vector__9;
typedef struct std_vector_Vector__10 std_vector_Vector__10;
typedef struct std_vector_Vector__11 std_vector_Vector__11;
typedef struct std_vector_Vector__12 std_vector_Vector__12;
typedef struct std_vector_Vector__13 std_vector_Vector__13;
typedef struct std_vector_Vector__14 std_vector_Vector__14;
typedef struct std_vector_Vector__15 std_vector_Vector__15;
typedef struct std_vector_Vector__16 std_vector_Vector__16;
typedef struct std_vector_Vector__17 std_vector_Vector__17;
typedef struct std_vector_Vector__18 std_vector_Vector__18;
typedef struct std_vector_Vector__19 std_vector_Vector__19;
typedef struct std_vector_Vector__20 std_vector_Vector__20;
typedef struct std_vector_Vector__21 std_vector_Vector__21;
typedef struct std_vector_Vector__22 std_vector_Vector__22;
typedef struct std_vector_Vector__23 std_vector_Vector__23;
typedef struct std_vector_Vector__24 std_vector_Vector__24;
typedef struct std_vector_Iterator__0 std_vector_Iterator__0;
typedef struct std_vector_Iterator__1 std_vector_Iterator__1;
typedef struct std_vector_Iterator__2 std_vector_Iterator__2;
typedef struct std_vector_Iterator__3 std_vector_Iterator__3;
typedef struct std_vector_Iterator__4 std_vector_Iterator__4;
typedef struct std_vector_Iterator__5 std_vector_Iterator__5;
typedef struct std_vector_Iterator__6 std_vector_Iterator__6;
typedef struct std_vector_Iterator__10 std_vector_Iterator__10;
typedef struct std_vector_Iterator__11 std_vector_Iterator__11;
typedef struct std_vector_Iterator__12 std_vector_Iterator__12;
typedef struct std_vector_Iterator__13 std_vector_Iterator__13;
typedef struct std_vector_Iterator__14 std_vector_Iterator__14;
typedef struct std_vector_Iterator__15 std_vector_Iterator__15;
typedef struct std_vector_Iterator__16 std_vector_Iterator__16;
typedef struct std_vector_Iterator__17 std_vector_Iterator__17;
typedef struct std_vector_Iterator__18 std_vector_Iterator__18;
typedef struct std_vector_Iterator__19 std_vector_Iterator__19;
typedef struct std_vector_Iterator__20 std_vector_Iterator__20;
typedef struct std_vector_Iterator__21 std_vector_Iterator__21;

/* Struct definitions */
struct compiler_docgen_DocGenerator {
  compiler_ast_program_Program *program;
};

struct std_buffer_Buffer {
  u8 *data;
  u32 size;
  u32 capacity;
};

struct compiler_passes_code_generator_CodeGenerator {
  compiler_passes_generic_pass_GenericPass *o;
  std_buffer_Buffer out;
  u32 indent;
  std_vector_Vector__1 *yield_vars;
  u32 uid;
};

struct compiler_passes_reorder_structs_ReorderStructs {
  compiler_passes_generic_pass_GenericPass *o;
  std_vector_Vector__2 *all_structs;
  std_set_Set__0 *done;
};

struct compiler_passes_typechecker_TypeChecker {
  compiler_passes_generic_pass_GenericPass *o;
  std_vector_Vector__6 *unchecked_functions;
  bool in_template_instance;
};

struct compiler_passes_register_types_RegisterTypes {
  compiler_passes_generic_pass_GenericPass *o;
};

struct compiler_passes_register_types_Finder {
  compiler_passes_generic_pass_GenericPass *o;
  compiler_ast_scopes_Symbol *sym;
  compiler_types_Type *type;
};

struct compiler_passes_mark_dead_code_MarkDeadCode {
  compiler_passes_generic_pass_GenericPass *o;
  std_set_Set__1 *done;
};

struct compiler_passes_generic_pass_GenericPass {
  std_vector_Vector__7 *scope_stack;
  std_vector_Vector__8 *namespace_stack;
  compiler_ast_program_Program *program;
  compiler_types_Type *error_type;
};

struct std_span_Location {
  char *filename;
  u32 line;
  u32 col;
  u32 index;
};

struct std_span_Span {
  std_span_Location start;
  std_span_Location end;
};

struct compiler_parser_Parser {
  std_vector_Vector__9 *tokens;
  u32 curr;
  compiler_ast_nodes_Function *curr_func;
  compiler_ast_program_Program *program;
  compiler_ast_program_Namespace *ns;
  std_vector_Vector__10 *attrs;
  std_span_Span attrs_span;
  compiler_tokens_Token *attrs_start_tok;
};

struct compiler_lexer_Lexer {
  char *source;
  u32 source_len;
  u32 i;
  std_span_Location loc;
  bool seen_newline;
  std_vector_Vector__9 *tokens;
  std_vector_Vector__11 *errors;
  bool in_comment;
  std_buffer_Buffer comment;
  std_span_Location comment_start;
};

struct compiler_ast_program_Namespace {
  compiler_ast_program_Namespace *parent;
  std_vector_Vector__6 *functions;
  std_vector_Vector__2 *structs;
  std_vector_Vector__12 *enums;
  std_vector_Vector__13 *constants;
  std_vector_Vector__13 *variables;
  std_vector_Vector__13 *imports;
  std_map_Map__2 *typedefs;
  std_map_Map__3 *namespaces;
  std_map_Map__4 *exported_symbols;
  compiler_ast_scopes_Symbol *sym;
  compiler_ast_scopes_Scope *scope;
  char *path;
  std_span_Span span;
  bool is_a_file;
  bool always_add_to_scope;
  bool is_top_level;
  compiler_ast_program_Namespace *internal_project_root;
};

struct compiler_ast_program_CachedTypes {
  compiler_types_Type *sv_type;
  compiler_types_Type *buffer_type;
};

struct compiler_ast_program_Program {
  compiler_ast_program_Namespace *global;
  std_vector_Vector__2 *ordered_structs;
  std_vector_Vector__1 *c_includes;
  std_vector_Vector__1 *c_flags;
  std_map_Map__5 *operator_overloads;
  std_map_Map__6 *c_embeds;
  std_map_Map__6 *sources;
  std_vector_Vector__1 *library_paths;
  std_vector_Vector__11 *errors;
  u32 error_level;
  compiler_ast_program_CachedTypes cached_types;
  jmp_buf err_jmp_ctx;
  bool check_doc_links;
  bool gen_debug_info;
  bool keep_all_code;
};

struct compiler_ast_program_NSIterator {
  std_vector_Vector__8 *stack;
  compiler_ast_program_Namespace *curr;
};

struct compiler_ast_operators_OperatorOverload {
  compiler_ast_operators_Operator op;
  compiler_types_Type *type1;
  compiler_types_Type *type2;
  compiler_types_Type *type3;
};

struct compiler_ast_nodes_Variable {
  compiler_ast_scopes_Symbol *sym;
  compiler_types_Type *type;
  compiler_ast_nodes_AST *default_value;
  compiler_types_Type *parsed_type;
};

struct compiler_ast_nodes_VarDeclaration {
  compiler_ast_nodes_Variable *var;
  compiler_ast_nodes_AST *init;
};

struct compiler_ast_nodes_Structure {
  compiler_ast_scopes_Symbol *sym;
  std_vector_Vector__4 *fields;
  compiler_types_Type *type;
  bool is_union;
  std_span_Span span;
  bool is_dead;
  char *format_spec;
  char *format_args;
};

struct compiler_ast_nodes_Enum {
  compiler_ast_scopes_Symbol *sym;
  std_span_Span span;
  std_vector_Vector__4 *fields;
  compiler_types_Type *type;
  bool is_dead;
};

struct compiler_ast_nodes_Function {
  compiler_ast_scopes_Symbol *sym;
  std_vector_Vector__4 *params;
  compiler_types_Type *return_type;
  compiler_ast_nodes_AST *body;
  compiler_ast_scopes_Scope *scope;
  std_span_Span span;
  compiler_types_Type *parsed_return_type;
  compiler_ast_nodes_AST *name_ast;
  compiler_types_Type *type;
  bool checked;
  bool is_variadic;
  bool is_variadic_format;
  std_vector_Vector__14 *operator_overloads;
  bool exits;
  bool is_method;
  bool is_static;
  compiler_types_Type *parent_type;
  bool is_dead;
};

struct compiler_ast_nodes_Block {
  std_vector_Vector__13 *statements;
  compiler_ast_scopes_Scope *scope;
  compiler_ast_nodes_AST *final_stmt;
};

struct compiler_ast_nodes_Identifier {
  char *name;
  compiler_ast_nodes_Variable *var;
  bool is_function;
  compiler_ast_nodes_Function *func;
};

struct compiler_ast_nodes_Argument {
  compiler_ast_nodes_AST *expr;
  char *label;
  std_span_Span label_span;
};

struct compiler_ast_nodes_FuncCall {
  compiler_ast_nodes_AST *callee;
  std_vector_Vector__15 *args;
  compiler_ast_nodes_Function *func;
  std_span_Span open_paren_span;
  std_span_Span close_paren_span;
  bool is_constructor;
  bool is_function_pointer;
};

struct compiler_ast_nodes_ImportPartSingle {
  char *name;
  std_span_Span name_span;
  char *alias;
  std_span_Span alias_span;
};

union compiler_ast_nodes_ImportPartUnion {
  compiler_ast_nodes_ImportPartSingle single;
  std_vector_Vector__16 *paths;
};

struct compiler_ast_nodes_ImportPart {
  compiler_ast_nodes_ImportPartType type;
  compiler_ast_nodes_ImportPartUnion u;
  std_span_Span span;
  compiler_ast_scopes_Symbol *resolved_symbol;
};

struct compiler_ast_nodes_Import {
  std_vector_Vector__5 *parts;
  compiler_ast_nodes_ImportType type;
  u32 parent_count;
  bool export;
};

struct compiler_ast_nodes_NumLiteral {
  char *text;
  compiler_types_Type *suffix;
};

struct compiler_ast_nodes_Binary {
  compiler_ast_operators_Operator op;
  compiler_ast_nodes_AST *lhs;
  compiler_ast_nodes_AST *rhs;
  std_span_Span op_span;
};

struct compiler_ast_nodes_Unary {
  compiler_ast_operators_Operator op;
  compiler_ast_nodes_AST *expr;
  std_span_Span op_span;
};

struct compiler_ast_nodes_NSLookup {
  compiler_ast_nodes_AST *lhs;
  char *rhs_name;
  std_span_Span rhs_span;
};

struct compiler_ast_nodes_Member {
  compiler_ast_nodes_AST *lhs;
  char *rhs_name;
  std_span_Span rhs_span;
  bool dot_shorthand;
  bool is_pointer;
};

struct compiler_ast_nodes_Assertion {
  compiler_ast_nodes_AST *expr;
  compiler_ast_nodes_AST *msg;
};

struct compiler_ast_nodes_IfBranch {
  compiler_ast_nodes_AST *cond;
  compiler_ast_nodes_AST *body;
};

struct compiler_ast_nodes_IfStatement {
  std_vector_Vector__17 *branches;
  compiler_ast_nodes_AST *els;
  std_span_Span els_span;
};

struct compiler_ast_nodes_Loop {
  compiler_ast_nodes_AST *init;
  compiler_ast_nodes_AST *cond;
  compiler_ast_nodes_AST *step;
  compiler_ast_nodes_AST *body;
};

struct compiler_ast_nodes_Cast {
  compiler_ast_nodes_AST *lhs;
  compiler_types_Type *to;
};

struct compiler_ast_nodes_FormatString {
  std_vector_Vector__1 *parts;
  std_vector_Vector__1 *specs;
  std_vector_Vector__13 *exprs;
};

struct compiler_ast_nodes_MatchCase {
  compiler_ast_nodes_AST *cond;
  compiler_ast_nodes_AST *body;
  compiler_ast_nodes_Function *cmp_fn;
};

struct compiler_ast_nodes_Match {
  compiler_ast_nodes_AST *expr;
  std_vector_Vector__18 *cases;
  compiler_ast_nodes_AST *defolt;
  bool is_custom_match;
  std_span_Span defolt_span;
  std_span_Span match_span;
};

struct compiler_ast_nodes_Specialization {
  compiler_ast_nodes_AST *base;
  std_vector_Vector__0 *parsed_template_args;
  std_vector_Vector__0 *template_args;
};

struct compiler_ast_nodes_ArrayLiteral {
  std_vector_Vector__13 *elements;
};

union compiler_ast_nodes_ASTUnion {
  compiler_ast_nodes_Assertion assertion;
  compiler_ast_nodes_Binary binary;
  compiler_ast_nodes_Block block;
  bool bool_literal;
  compiler_ast_nodes_FuncCall call;
  compiler_ast_nodes_Cast cast;
  compiler_ast_nodes_Identifier ident;
  compiler_ast_nodes_IfStatement if_stmt;
  compiler_ast_nodes_Import import_path;
  compiler_ast_nodes_NSLookup lookup;
  compiler_ast_nodes_Loop loop;
  compiler_ast_nodes_Member member;
  compiler_ast_nodes_NumLiteral num_literal;
  char *string_literal;
  char *char_literal;
  compiler_ast_nodes_Unary unary;
  std_span_Span operator_span;
  compiler_ast_nodes_VarDeclaration var_decl;
  compiler_ast_nodes_FormatString fmt_str;
  compiler_types_Type *size_of_type;
  compiler_ast_nodes_Match match_stmt;
  compiler_ast_nodes_Specialization spec;
  compiler_ast_nodes_ArrayLiteral array_literal;
  compiler_ast_nodes_AST *child;
};

struct compiler_ast_nodes_AST {
  compiler_ast_nodes_ASTType type;
  std_span_Span span;
  compiler_ast_nodes_ASTUnion u;
  compiler_types_Type *etype;
  compiler_ast_scopes_Symbol *resolved_symbol;
  bool returns;
};

struct compiler_ast_scopes_TemplateInstance {
  std_vector_Vector__0 *args;
  compiler_ast_scopes_Symbol *parent;
  compiler_ast_scopes_Symbol *resolved;
};

struct compiler_ast_scopes_Template {
  std_vector_Vector__19 *params;
  std_vector_Vector__3 *instances;
};

struct compiler_ast_scopes_Reference {
  compiler_ast_scopes_ReferenceType type;
  std_span_Span span;
};

union compiler_ast_scopes_SymbolUnion {
  compiler_ast_nodes_Function *func;
  compiler_ast_nodes_Structure *struc;
  compiler_ast_nodes_Enum *enum_;
  compiler_ast_program_Namespace *ns;
  compiler_types_Type *type_def;
  compiler_ast_nodes_Variable *var;
};

struct compiler_ast_scopes_Symbol {
  char *name;
  char *display;
  char *full_name;
  std_span_Span span;
  compiler_ast_program_Namespace *ns;
  char *comment;
  std_span_Location comment_loc;
  std_vector_Vector__20 *references;
  compiler_ast_scopes_SymbolType type;
  compiler_ast_scopes_SymbolUnion u;
  bool is_extern;
  char *extern_name;
  compiler_ast_scopes_Template *template;
};

struct compiler_ast_scopes_Scope {
  std_map_Map__4 *items;
  std_vector_Vector__13 *defers;
  u32 loop_count;
  bool can_yield;
  compiler_ast_nodes_Function *cur_func;
  compiler_ast_scopes_Scope *parent;
};

struct compiler_lsp_finder_Finder {
  compiler_lsp_CommandType cmd;
  std_span_Location loc;
  compiler_ast_scopes_Symbol *found_sym;
  u32 active_param;
  compiler_ast_nodes_AST *call;
  bool found_in_ns_lookup;
};

struct compiler_attributes_Attribute {
  compiler_attributes_AttributeType type;
  std_vector_Vector__1 *args;
  std_span_Span span;
};

struct compiler_tokens_Token {
  compiler_tokens_TokenType type;
  std_span_Span span;
  char *text;
  compiler_tokens_Token *suffix;
  bool seen_newline;
  char *comment;
  std_span_Location comment_loc;
};

struct compiler_errors_Error {
  compiler_errors_ErrorType type;
  char *msg1;
  std_span_Span span1;
  char *msg2;
  std_span_Span span2;
};

struct compiler_types_FunctionType {
  compiler_ast_nodes_Function *orig;
  std_vector_Vector__4 *params;
  compiler_types_Type *return_type;
  bool is_variadic;
};

struct compiler_types_ArrayType {
  compiler_types_Type *elem_type;
  compiler_ast_nodes_AST *size_expr;
  bool size_known;
  u32 size;
};

struct compiler_types_UnresolvedTemplate {
  compiler_types_Type *base;
  std_vector_Vector__0 *args;
};

struct compiler_types_TypeUnion {
  compiler_types_Type *ptr;
  compiler_ast_nodes_Structure *struc;
  compiler_ast_nodes_Enum *enum_;
  compiler_ast_nodes_AST *unresolved;
  compiler_types_FunctionType func;
  compiler_types_ArrayType arr;
  compiler_types_UnresolvedTemplate unresolved_spec;
};

struct compiler_types_Type {
  char *name;
  compiler_types_BaseType base;
  std_span_Span span;
  compiler_types_TypeUnion u;
  std_map_Map__7 *methods;
  compiler_ast_scopes_Symbol *sym;
  compiler_ast_scopes_TemplateInstance *template_instance;
};

struct std_CharIterator {
  char *data;
  u32 len;
  u32 pos;
};

struct std_sv_SV {
  char *data;
  u32 len;
};

struct std_sv_SVLineIterator {
  std_sv_SV sv;
};

struct std_compact_map_Item__0 {
  u32 hash;
  char *key;
  std_value_Value *value;
};

struct std_compact_map_Map__0 {
  std_vector_Vector__23 *items;
  i32 *indices;
  u32 capacity;
};

struct std_vector_Iterator__23 {
  std_vector_Vector__23 *vec;
  u32 index;
};

struct std_compact_map_Iterator__0 {
  std_vector_Iterator__23 iter;
};

struct std_set_Set__0 {
  std_map_Map__0 *map;
  u32 size;
};

struct std_set_Set__1 {
  std_map_Map__1 *map;
  u32 size;
};

struct std_set_Set__2 {
  std_map_Map__9 *map;
  u32 size;
};

struct std_map_Iterator__9 {
  i32 idx;
  std_map_Item__9 *node;
  std_map_Map__9 *map;
};

struct std_set_Iterator__2 {
  std_map_Iterator__9 map_iter;
};

union std_value_ValueUnion {
  bool as_bool;
  i64 as_int;
  f64 as_float;
  std_buffer_Buffer as_str;
  std_vector_Vector__22 *as_list;
  std_compact_map_Map__0 *as_dict;
};

struct std_value_Value {
  std_value_ValueType type;
  std_value_ValueUnion u;
  std_span_Span span;
};

struct std_map_Item__0 {
  char *key;
  bool value;
  std_map_Item__0 *next;
};

struct std_map_Item__1 {
  u64 key;
  bool value;
  std_map_Item__1 *next;
};

struct std_map_Item__2 {
  char *key;
  compiler_types_Type *value;
  std_map_Item__2 *next;
};

struct std_map_Item__3 {
  char *key;
  compiler_ast_program_Namespace *value;
  std_map_Item__3 *next;
};

struct std_map_Item__4 {
  char *key;
  compiler_ast_scopes_Symbol *value;
  std_map_Item__4 *next;
};

struct std_map_Item__5 {
  compiler_ast_operators_OperatorOverload key;
  compiler_ast_nodes_Function *value;
  std_map_Item__5 *next;
};

struct std_map_Item__6 {
  char *key;
  char *value;
  std_map_Item__6 *next;
};

struct std_map_Item__7 {
  char *key;
  compiler_ast_nodes_Function *value;
  std_map_Item__7 *next;
};

struct std_map_Item__8 {
  char *key;
  compiler_ast_nodes_MatchCase *value;
  std_map_Item__8 *next;
};

struct std_map_Item__9 {
  std_span_Span key;
  bool value;
  std_map_Item__9 *next;
};

struct std_map_Map__0 {
  std_map_Item__0 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__1 {
  std_map_Item__1 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__2 {
  std_map_Item__2 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__3 {
  std_map_Item__3 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__4 {
  std_map_Item__4 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__5 {
  std_map_Item__5 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__6 {
  std_map_Item__6 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__7 {
  std_map_Item__7 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__8 {
  std_map_Item__8 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__9 {
  std_map_Item__9 **buckets;
  u32 size;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Iterator__2 {
  i32 idx;
  std_map_Item__2 *node;
  std_map_Map__2 *map;
};

struct std_map_Iterator__3 {
  i32 idx;
  std_map_Item__3 *node;
  std_map_Map__3 *map;
};

struct std_map_Iterator__4 {
  i32 idx;
  std_map_Item__4 *node;
  std_map_Map__4 *map;
};

struct std_map_Iterator__6 {
  i32 idx;
  std_map_Item__6 *node;
  std_map_Map__6 *map;
};

struct std_map_Iterator__7 {
  i32 idx;
  std_map_Item__7 *node;
  std_map_Map__7 *map;
};

struct std_map_ValueIterator__3 {
  std_map_Iterator__3 map_iter;
};

struct std_map_ValueIterator__7 {
  std_map_Iterator__7 map_iter;
};

struct std_vector_Vector__0 {
  compiler_types_Type **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__1 {
  char **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__2 {
  compiler_ast_nodes_Structure **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__3 {
  compiler_ast_scopes_TemplateInstance **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__4 {
  compiler_ast_nodes_Variable **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__5 {
  compiler_ast_nodes_ImportPart **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__6 {
  compiler_ast_nodes_Function **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__7 {
  compiler_ast_scopes_Scope **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__8 {
  compiler_ast_program_Namespace **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__9 {
  compiler_tokens_Token **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__10 {
  compiler_attributes_Attribute **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__11 {
  compiler_errors_Error **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__12 {
  compiler_ast_nodes_Enum **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__13 {
  compiler_ast_nodes_AST **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__14 {
  compiler_ast_operators_Operator *data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__15 {
  compiler_ast_nodes_Argument **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__16 {
  std_vector_Vector__5 **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__17 {
  compiler_ast_nodes_IfBranch *data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__18 {
  compiler_ast_nodes_MatchCase **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__19 {
  compiler_ast_scopes_Symbol **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__20 {
  compiler_ast_scopes_Reference *data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__21 {
  std_span_Span *data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__22 {
  std_value_Value **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__23 {
  std_compact_map_Item__0 *data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__24 {
  u32 *data;
  u32 size;
  u32 capacity;
};

struct std_vector_Iterator__0 {
  std_vector_Vector__0 *vec;
  u32 index;
};

struct std_vector_Iterator__1 {
  std_vector_Vector__1 *vec;
  u32 index;
};

struct std_vector_Iterator__2 {
  std_vector_Vector__2 *vec;
  u32 index;
};

struct std_vector_Iterator__3 {
  std_vector_Vector__3 *vec;
  u32 index;
};

struct std_vector_Iterator__4 {
  std_vector_Vector__4 *vec;
  u32 index;
};

struct std_vector_Iterator__5 {
  std_vector_Vector__5 *vec;
  u32 index;
};

struct std_vector_Iterator__6 {
  std_vector_Vector__6 *vec;
  u32 index;
};

struct std_vector_Iterator__10 {
  std_vector_Vector__10 *vec;
  u32 index;
};

struct std_vector_Iterator__11 {
  std_vector_Vector__11 *vec;
  u32 index;
};

struct std_vector_Iterator__12 {
  std_vector_Vector__12 *vec;
  u32 index;
};

struct std_vector_Iterator__13 {
  std_vector_Vector__13 *vec;
  u32 index;
};

struct std_vector_Iterator__14 {
  std_vector_Vector__14 *vec;
  u32 index;
};

struct std_vector_Iterator__15 {
  std_vector_Vector__15 *vec;
  u32 index;
};

struct std_vector_Iterator__16 {
  std_vector_Vector__16 *vec;
  u32 index;
};

struct std_vector_Iterator__17 {
  std_vector_Vector__17 *vec;
  u32 index;
};

struct std_vector_Iterator__18 {
  std_vector_Vector__18 *vec;
  u32 index;
};

struct std_vector_Iterator__19 {
  std_vector_Vector__19 *vec;
  u32 index;
};

struct std_vector_Iterator__20 {
  std_vector_Vector__20 *vec;
  u32 index;
};

struct std_vector_Iterator__21 {
  std_vector_Vector__21 *vec;
  u32 index;
};


/* function declarations */
std_value_Value *compiler_docgen_DocGenerator_gen_enum(compiler_docgen_DocGenerator *this, compiler_ast_nodes_Enum *enum_);
void compiler_docgen_DocGenerator_gen_location(compiler_docgen_DocGenerator *this, std_value_Value *obj, std_span_Span span);
char *compiler_docgen_DocGenerator_gen_templated_type(compiler_docgen_DocGenerator *this, compiler_types_Type *base, std_vector_Vector__0 *args);
char *compiler_docgen_DocGenerator_gen_typename_str(compiler_docgen_DocGenerator *this, compiler_types_Type *type);
std_value_Value *compiler_docgen_DocGenerator_gen_typename(compiler_docgen_DocGenerator *this, compiler_types_Type *type);
std_value_Value *compiler_docgen_DocGenerator_gen_methods(compiler_docgen_DocGenerator *this, compiler_types_Type *type);
std_value_Value *compiler_docgen_DocGenerator_gen_function(compiler_docgen_DocGenerator *this, compiler_ast_nodes_Function *func);
std_value_Value *compiler_docgen_DocGenerator_gen_struct(compiler_docgen_DocGenerator *this, compiler_ast_nodes_Structure *struc);
std_value_Value *compiler_docgen_DocGenerator_gen_ns(compiler_docgen_DocGenerator *this, compiler_ast_program_Namespace *ns);
std_value_Value *compiler_docgen_DocGenerator_gen_builtin(compiler_docgen_DocGenerator *this, compiler_types_Type *type);
std_value_Value *compiler_docgen_DocGenerator_gen_builtins(compiler_docgen_DocGenerator *this, compiler_ast_program_Program *program);
void compiler_docgen_generate_doc_json(compiler_ast_program_Program *program, char *json_path);
void compiler_passes_run_typecheck_passes(compiler_ast_program_Program *program);
char *compiler_passes_run_codegen_passes(compiler_ast_program_Program *program);
void compiler_passes_code_generator_CodeGenerator_gen_indent(compiler_passes_code_generator_CodeGenerator *this);
void str_replace(char **this, char *other);
compiler_errors_Error *compiler_passes_code_generator_CodeGenerator_error(compiler_passes_code_generator_CodeGenerator *this, compiler_errors_Error *err);
compiler_ast_scopes_Scope *compiler_passes_code_generator_CodeGenerator_scope(compiler_passes_code_generator_CodeGenerator *this);
void compiler_passes_code_generator_CodeGenerator_gen_debug_info(compiler_passes_code_generator_CodeGenerator *this, std_span_Span span, bool force);
char *compiler_passes_code_generator_CodeGenerator_get_op(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_internal_print(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_format_string_part(compiler_passes_code_generator_CodeGenerator *this, char *part);
void compiler_passes_code_generator_CodeGenerator_format_string_custom_specifier(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type, compiler_ast_nodes_AST *expr);
void compiler_passes_code_generator_CodeGenerator_format_string_custom_argument(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type, compiler_ast_nodes_AST *expr);
void compiler_passes_code_generator_CodeGenerator_gen_format_string_variadic(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, bool newline_after);
void compiler_passes_code_generator_CodeGenerator_gen_format_string(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_yield_expression(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *expr);
void compiler_passes_code_generator_CodeGenerator_gen_constant(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_constants(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns);
void compiler_passes_code_generator_CodeGenerator_gen_global_variables(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns);
void compiler_passes_code_generator_CodeGenerator_gen_control_body(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_AST *body);
void compiler_passes_code_generator_CodeGenerator_gen_in_yield_context(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_constructor(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_Structure *struc);
void compiler_passes_code_generator_CodeGenerator_gen_expression(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, bool is_top_level);
void compiler_passes_code_generator_CodeGenerator_gen_var_declaration(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_match_case_body(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_AST *body);
void compiler_passes_code_generator_CodeGenerator_gen_custom_match(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_match_bool(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_match(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_defers_upto(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_scopes_Scope *end_scope);
void compiler_passes_code_generator_CodeGenerator_gen_statement(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node);
void compiler_passes_code_generator_CodeGenerator_gen_block(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, bool with_braces);
char *compiler_passes_code_generator_CodeGenerator_helper_gen_type(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *top, compiler_types_Type *cur, char *acc, bool is_func_def);
char *compiler_passes_code_generator_CodeGenerator_get_type_name_string(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type, char *name, bool is_func_def);
void compiler_passes_code_generator_CodeGenerator_gen_type_and_name(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type, char *name);
void compiler_passes_code_generator_CodeGenerator_gen_type(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type);
void compiler_passes_code_generator_CodeGenerator_gen_function(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Function *func);
void compiler_passes_code_generator_CodeGenerator_gen_function_decl(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Function *func);
void compiler_passes_code_generator_CodeGenerator_gen_functions(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns);
void compiler_passes_code_generator_CodeGenerator_gen_function_decls(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns);
void compiler_passes_code_generator_CodeGenerator_gen_enum_types(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns);
void compiler_passes_code_generator_CodeGenerator_gen_enum_dbg_method(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Enum *enum_);
void compiler_passes_code_generator_CodeGenerator_gen_enum(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Enum *enum_);
void compiler_passes_code_generator_CodeGenerator_gen_struct(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Structure *struc);
void compiler_passes_code_generator_CodeGenerator_gen_structs_and_typedefs(compiler_passes_code_generator_CodeGenerator *this);
char *compiler_passes_code_generator_CodeGenerator_generate(compiler_passes_code_generator_CodeGenerator *this);
compiler_passes_code_generator_CodeGenerator compiler_passes_code_generator_CodeGenerator_make(compiler_ast_program_Program *program);
char *compiler_passes_code_generator_CodeGenerator_run(compiler_ast_program_Program *program);
compiler_passes_reorder_structs_ReorderStructs *compiler_passes_reorder_structs_ReorderStructs_new(compiler_ast_program_Program *program);
void compiler_passes_reorder_structs_ReorderStructs_free(compiler_passes_reorder_structs_ReorderStructs *this);
void compiler_passes_reorder_structs_ReorderStructs_collect_all_structs(compiler_passes_reorder_structs_ReorderStructs *this, compiler_ast_program_Namespace *ns);
void compiler_passes_reorder_structs_ReorderStructs_dfs(compiler_passes_reorder_structs_ReorderStructs *this, compiler_ast_nodes_Structure *struc);
void compiler_passes_reorder_structs_ReorderStructs_reorder_structs(compiler_passes_reorder_structs_ReorderStructs *this);
void compiler_passes_reorder_structs_ReorderStructs_run(compiler_ast_program_Program *program);
compiler_errors_Error *compiler_passes_typechecker_TypeChecker_error(compiler_passes_typechecker_TypeChecker *this, compiler_errors_Error *err);
compiler_ast_scopes_Scope *compiler_passes_typechecker_TypeChecker_scope(compiler_passes_typechecker_TypeChecker *this);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_get_base_type(compiler_passes_typechecker_TypeChecker *this, compiler_types_BaseType base, std_span_Span span);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_get_type_by_name(compiler_passes_typechecker_TypeChecker *this, char *name, std_span_Span span);
void compiler_passes_typechecker_TypeChecker_set_resolved_symbol(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_ast_scopes_Symbol *sym);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_resolve_type(compiler_passes_typechecker_TypeChecker *this, compiler_types_Type *old, bool allow_incomplete, bool error, bool resolve_templates);
void compiler_passes_typechecker_TypeChecker_try_infer_array_size(compiler_passes_typechecker_TypeChecker *this, compiler_types_ArrayType *arr_typ);
void compiler_passes_typechecker_TypeChecker_resolve_templated_struct_methods(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Structure *old, compiler_ast_nodes_Structure *cur);
void compiler_passes_typechecker_TypeChecker_resolve_templated_struct(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Structure *struc, compiler_ast_scopes_TemplateInstance *instance);
void compiler_passes_typechecker_TypeChecker_resolve_templated_function(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *func, compiler_ast_scopes_TemplateInstance *instance);
compiler_ast_scopes_Symbol *compiler_passes_typechecker_TypeChecker_find_template_instance(compiler_passes_typechecker_TypeChecker *this, std_vector_Vector__3 *template_instances, std_vector_Vector__0 *args);
compiler_ast_scopes_Symbol *compiler_passes_typechecker_TypeChecker_resolve_templated_symbol(compiler_passes_typechecker_TypeChecker *this, compiler_ast_scopes_Symbol *sym, compiler_ast_nodes_AST *node);
compiler_ast_scopes_Symbol *compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool error, compiler_types_Type *hint, bool resolve_templates);
void compiler_passes_typechecker_TypeChecker_check_block(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint);
void compiler_passes_typechecker_TypeChecker_check_method_call(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *method, compiler_ast_nodes_AST *node);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_internal_print(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_constructor(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node);
void compiler_passes_typechecker_TypeChecker_check_call_args(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, std_vector_Vector__4 *params, bool is_variadic);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_call(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_pointer_arith(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *_lhs, compiler_types_Type *_rhs);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_find_and_replace_overloaded_op(compiler_passes_typechecker_TypeChecker *this, compiler_ast_operators_Operator op, compiler_ast_nodes_AST *node, compiler_ast_nodes_AST *arg1, compiler_ast_nodes_AST *arg2, compiler_ast_nodes_AST *arg3);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_binary_op(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *lhs, compiler_types_Type *rhs);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_format_string(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_member(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_being_called);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_expression(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *hint);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_index(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *hint, bool is_being_assigned);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_assignment(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *lhs, compiler_types_Type *rhs);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_expression_helper(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *hint);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_call_dbg_on_enum_value(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST **node_ptr);
void compiler_passes_typechecker_TypeChecker_check_match_for_enum(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Enum *enum_, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint);
void compiler_passes_typechecker_TypeChecker_check_match_for_bool(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint);
compiler_ast_nodes_Function *compiler_passes_typechecker_TypeChecker_check_match_case_and_find_overload(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *expr, compiler_ast_nodes_MatchCase *mcase);
void compiler_passes_typechecker_TypeChecker_check_match(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint);
void compiler_passes_typechecker_TypeChecker_check_if(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint);
void compiler_passes_typechecker_TypeChecker_check_expression_statement(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_AST *body, bool is_expr, compiler_types_Type *hint);
void compiler_passes_typechecker_TypeChecker_check_while(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node);
void compiler_passes_typechecker_TypeChecker_check_for(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node);
void compiler_passes_typechecker_TypeChecker_check_statement(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node);
void compiler_passes_typechecker_TypeChecker_check_function(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *func);
void compiler_passes_typechecker_TypeChecker_handle_imports(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns, bool is_global);
void compiler_passes_typechecker_TypeChecker_check_globals(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_const);
void compiler_passes_typechecker_TypeChecker_check_namespace(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns);
void compiler_passes_typechecker_TypeChecker_resolve_doc_links(compiler_passes_typechecker_TypeChecker *this, compiler_ast_scopes_Symbol *sym);
compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_const_expression(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *hint);
void compiler_passes_typechecker_TypeChecker_handle_import_path_base(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_Import *imp, std_vector_Vector__5 *parts, compiler_ast_scopes_Symbol *base, char *alias, i32 start_idx);
void compiler_passes_typechecker_TypeChecker_handle_import_statement(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node);
void compiler_passes_typechecker_TypeChecker_pre_check_function(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns, compiler_ast_nodes_Function *func);
void compiler_passes_typechecker_TypeChecker_loosely_resolve_templated_struct(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Structure *struc);
void compiler_passes_typechecker_TypeChecker_resolve_struct(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Structure *struc);
void compiler_passes_typechecker_TypeChecker_check_operator_overload_function(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *func, compiler_ast_operators_Operator op);
void compiler_passes_typechecker_TypeChecker_check_function_declaration(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *func);
void compiler_passes_typechecker_TypeChecker_try_resolve_typedefs_in_namespace(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns, bool pre_import);
void compiler_passes_typechecker_TypeChecker_check_post_import(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns);
void compiler_passes_typechecker_TypeChecker_pre_check_globals(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_const);
void compiler_passes_typechecker_TypeChecker_check_pre_import(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns);
void compiler_passes_typechecker_TypeChecker_run(compiler_ast_program_Program *program);
void compiler_passes_register_types_RegisterTypes_register_struct(compiler_passes_register_types_RegisterTypes *this, compiler_ast_program_Namespace *ns, compiler_ast_nodes_Structure *struc);
void compiler_passes_register_types_RegisterTypes_register_enum(compiler_passes_register_types_RegisterTypes *this, compiler_ast_program_Namespace *ns, compiler_ast_nodes_Enum *enum_);
void compiler_passes_register_types_RegisterTypes_register_globals(compiler_passes_register_types_RegisterTypes *this, compiler_ast_nodes_AST *node);
void compiler_passes_register_types_RegisterTypes_add_dbg_method_for_enum(compiler_passes_register_types_RegisterTypes *this, compiler_ast_nodes_Enum *enum_);
void compiler_passes_register_types_RegisterTypes_register_namespace(compiler_passes_register_types_RegisterTypes *this, compiler_ast_program_Namespace *ns);
void compiler_passes_register_types_RegisterTypes_register_base_type(compiler_passes_register_types_RegisterTypes *this, compiler_types_BaseType base);
void compiler_passes_register_types_RegisterTypes_register_alias(compiler_passes_register_types_RegisterTypes *this, char *name, compiler_types_Type *orig);
void compiler_passes_register_types_RegisterTypes_register_builtin_types(compiler_passes_register_types_RegisterTypes *this);
compiler_passes_register_types_Finder compiler_passes_register_types_Finder_to(compiler_passes_register_types_Finder this, char *name);
void compiler_passes_register_types_RegisterTypes_register_cached_types(compiler_passes_register_types_RegisterTypes *this);
void compiler_passes_register_types_RegisterTypes_run(compiler_ast_program_Program *program);
compiler_passes_mark_dead_code_MarkDeadCode *compiler_passes_mark_dead_code_MarkDeadCode_new(compiler_ast_program_Program *program);
void compiler_passes_mark_dead_code_MarkDeadCode_free(compiler_passes_mark_dead_code_MarkDeadCode *this);
void compiler_passes_mark_dead_code_MarkDeadCode_mark_sym(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_scopes_Symbol *sym);
void compiler_passes_mark_dead_code_MarkDeadCode_mark_function(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_nodes_Function *f);
void compiler_passes_mark_dead_code_MarkDeadCode_mark_type(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_types_Type *typ);
void compiler_passes_mark_dead_code_MarkDeadCode_mark_struct(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_nodes_Structure *s);
void compiler_passes_mark_dead_code_MarkDeadCode_mark(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_nodes_AST *node);
compiler_ast_nodes_Function *compiler_passes_mark_dead_code_MarkDeadCode_find_main_function(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_program_Program *program);
void compiler_passes_mark_dead_code_MarkDeadCode_mark_sym_as_dead_by_default(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_scopes_Symbol *sym);
void compiler_passes_mark_dead_code_MarkDeadCode_run(compiler_ast_program_Program *program);
compiler_passes_generic_pass_GenericPass *compiler_passes_generic_pass_GenericPass_new(compiler_ast_program_Program *program);
void compiler_passes_generic_pass_GenericPass_push_scope(compiler_passes_generic_pass_GenericPass *this, compiler_ast_scopes_Scope *scope);
void compiler_passes_generic_pass_GenericPass_push_namespace(compiler_passes_generic_pass_GenericPass *this, compiler_ast_program_Namespace *ns);
compiler_ast_scopes_Scope *compiler_passes_generic_pass_GenericPass_pop_scope(compiler_passes_generic_pass_GenericPass *this);
compiler_ast_program_Namespace *compiler_passes_generic_pass_GenericPass_pop_namespace(compiler_passes_generic_pass_GenericPass *this);
compiler_ast_scopes_Scope *compiler_passes_generic_pass_GenericPass_scope(compiler_passes_generic_pass_GenericPass *this);
compiler_ast_program_Namespace *compiler_passes_generic_pass_GenericPass_ns(compiler_passes_generic_pass_GenericPass *this);
compiler_errors_Error *compiler_passes_generic_pass_GenericPass_error(compiler_passes_generic_pass_GenericPass *this, compiler_errors_Error *err);
void compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(compiler_passes_generic_pass_GenericPass *this, bool export, compiler_ast_scopes_Symbol *item, char *name);
void compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(compiler_passes_generic_pass_GenericPass *this, compiler_ast_scopes_Symbol *item, char *name);
compiler_ast_scopes_Symbol *compiler_passes_generic_pass_GenericPass_lookup_in_symbol(compiler_passes_generic_pass_GenericPass *this, compiler_ast_scopes_Symbol *sym, char *name, std_span_Span span, bool error);
void compiler_passes_generic_pass_GenericPass_import_all_from_namespace(compiler_passes_generic_pass_GenericPass *this, compiler_ast_program_Namespace *ns, bool export);
void compiler_passes_generic_pass_GenericPass_import_all_from_symbol(compiler_passes_generic_pass_GenericPass *this, compiler_ast_scopes_Symbol *sym, bool export);
compiler_parser_Parser compiler_parser_Parser_make(compiler_ast_program_Program *program, compiler_ast_program_Namespace *ns);
compiler_tokens_Token *compiler_parser_Parser_peek(compiler_parser_Parser *this, i32 off);
compiler_errors_Error *compiler_parser_Parser_error_msg(compiler_parser_Parser *this, char *msg);
compiler_errors_Error *compiler_parser_Parser_error(compiler_parser_Parser *this, compiler_errors_Error *err);
void compiler_parser_Parser_unhandled_type(compiler_parser_Parser *this, char *func);
compiler_tokens_Token *compiler_parser_Parser_token(compiler_parser_Parser *this);
bool compiler_parser_Parser_token_is(compiler_parser_Parser *this, compiler_tokens_TokenType type);
bool compiler_parser_Parser_token_is_identifier(compiler_parser_Parser *this, char *name);
bool compiler_parser_Parser_peek_token_is(compiler_parser_Parser *this, u32 off, compiler_tokens_TokenType type);
bool compiler_parser_Parser_consume_if(compiler_parser_Parser *this, compiler_tokens_TokenType type);
void compiler_parser_Parser_consume_newline_or(compiler_parser_Parser *this, compiler_tokens_TokenType type);
compiler_tokens_Token *compiler_parser_Parser_consume(compiler_parser_Parser *this, compiler_tokens_TokenType type);
bool compiler_parser_Parser_is_end_of_statement(compiler_parser_Parser *this);
void compiler_parser_Parser_consume_end_of_statement(compiler_parser_Parser *this);
void compiler_parser_Parser_clear_attributes(compiler_parser_Parser *this);
bool compiler_parser_Parser_is_compound_operator(compiler_parser_Parser *this, compiler_ast_operators_Operator op);
std_span_Span compiler_parser_Parser_consume_compound_operator(compiler_parser_Parser *this, compiler_ast_operators_Operator op);
compiler_types_Type *compiler_parser_Parser_parse_type(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_identifier(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_scoped_identifier(compiler_parser_Parser *this, bool consume_template);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_format_string(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_match(compiler_parser_Parser *this);
compiler_types_Type *compiler_parser_Parser_parse_literal_suffix_type(compiler_parser_Parser *this, compiler_tokens_Token *suffix);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_call(compiler_parser_Parser *this, compiler_ast_nodes_AST *callee);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_var_initializer(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_var_declaration(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_global_value(compiler_parser_Parser *this, bool is_const);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_atom(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_postfix(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_prefix(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_cast(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_term(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_additive(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_shift(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_bw_and(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_bw_xor(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_bw_or(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_relational(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_logical_not(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_logical_and(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_logical_or(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_expression(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_multi_if(compiler_parser_Parser *this, compiler_tokens_Token *start_tok);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_if(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_for_each(compiler_parser_Parser *this, std_span_Span start_span);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_for(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_statement(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_block(compiler_parser_Parser *this);
void compiler_parser_Parser_parse_template_params(compiler_parser_Parser *this, compiler_ast_scopes_Symbol *sym, std_span_Span *out_span);
void compiler_parser_Parser_add_doc_comment(compiler_parser_Parser *this, compiler_ast_scopes_Symbol *sym, compiler_tokens_Token *token);
compiler_ast_nodes_Function *compiler_parser_Parser_parse_function(compiler_parser_Parser *this);
void compiler_parser_Parser_parse_extern_into_symbol(compiler_parser_Parser *this, compiler_ast_scopes_Symbol *sym);
void compiler_parser_Parser_get_extern_from_attr(compiler_parser_Parser *this, compiler_ast_scopes_Symbol *sym, compiler_attributes_Attribute *attr);
std_vector_Vector__5 *compiler_parser_Parser_parse_import_path(compiler_parser_Parser *this);
compiler_ast_nodes_AST *compiler_parser_Parser_parse_import(compiler_parser_Parser *this);
bool compiler_parser_Parser_parse_struct_field(compiler_parser_Parser *this, compiler_ast_nodes_Structure *struc);
compiler_ast_nodes_Structure *compiler_parser_Parser_parse_struct(compiler_parser_Parser *this);
compiler_ast_nodes_Enum *compiler_parser_Parser_parse_enum(compiler_parser_Parser *this);
void compiler_parser_Parser_parse_attribute(compiler_parser_Parser *this);
void compiler_parser_Parser_parse_namespace_until(compiler_parser_Parser *this, compiler_tokens_TokenType end_type);
void compiler_parser_Parser_parse_compiler_option(compiler_parser_Parser *this);
void compiler_parser_Parser_try_load_mod_for_namespace(compiler_parser_Parser *this, compiler_ast_program_Namespace *ns);
compiler_ast_program_Namespace *compiler_parser_Parser_load_single_import_part(compiler_parser_Parser *this, compiler_ast_program_Namespace *base, char *name, std_span_Span span);
bool compiler_parser_Parser_load_import_path_from_base(compiler_parser_Parser *this, std_vector_Vector__5 *parts, compiler_ast_program_Namespace *base);
char *compiler_parser_Parser_find_external_library_path(compiler_parser_Parser *this, char *name);
compiler_ast_program_Namespace *compiler_parser_Parser_find_external_library(compiler_parser_Parser *this, char *name);
bool compiler_parser_Parser_load_import_path(compiler_parser_Parser *this, compiler_ast_nodes_AST *import_stmt);
void compiler_parser_Parser_load_file(compiler_parser_Parser *this, char *filename, char *contents);
void compiler_parser_Parser_couldnt_find_stdlib(compiler_parser_Parser *this);
void compiler_parser_Parser_find_and_import_stdlib(compiler_parser_Parser *this);
void compiler_parser_Parser_include_prelude_only(compiler_parser_Parser *this);
void compiler_parser_Parser_create_namespaces_for_initial_file(compiler_parser_Parser *this, char *filename, bool single_file);
void compiler_parser_Parser_parse_toplevel(compiler_ast_program_Program *program, char *filename, bool include_stdlib, char *file_contents, bool include_workspace_main);
compiler_lexer_Lexer compiler_lexer_Lexer_make(char *source, char *filename);
void compiler_lexer_Lexer_push(compiler_lexer_Lexer *this, compiler_tokens_Token *token);
void compiler_lexer_Lexer_push_type(compiler_lexer_Lexer *this, compiler_tokens_TokenType type, u32 len);
char compiler_lexer_Lexer_cur(compiler_lexer_Lexer *this);
void compiler_lexer_Lexer_inc(compiler_lexer_Lexer *this);
char compiler_lexer_Lexer_peek(compiler_lexer_Lexer *this, u32 offset);
void compiler_lexer_Lexer_lex_char_literal(compiler_lexer_Lexer *this);
void compiler_lexer_Lexer_lex_string_literal(compiler_lexer_Lexer *this, bool has_seen_f);
compiler_tokens_Token *compiler_lexer_Lexer_lex_int_literal_different_base(compiler_lexer_Lexer *this);
compiler_tokens_Token *compiler_lexer_Lexer_lex_numeric_literal_helper(compiler_lexer_Lexer *this);
void compiler_lexer_Lexer_lex_numeric_literal(compiler_lexer_Lexer *this);
void compiler_lexer_Lexer_lex_comment(compiler_lexer_Lexer *this);
std_vector_Vector__9 *compiler_lexer_Lexer_lex(compiler_lexer_Lexer *this);
compiler_ast_program_Namespace *compiler_ast_program_Namespace_new(compiler_ast_program_Namespace *parent, char *path);
compiler_ast_scopes_Symbol *compiler_ast_program_Namespace_find_importable_symbol(compiler_ast_program_Namespace *this, char *name);
compiler_ast_program_Namespace *compiler_ast_program_Namespace_get_project_root(compiler_ast_program_Namespace *this, std_span_Span span, compiler_ast_program_Program *program_for_errors);
compiler_ast_program_Program *compiler_ast_program_Program_new(void);
void compiler_ast_program_Program_exit_with_errors_if_any(compiler_ast_program_Program *this);
char *compiler_ast_program_Program_get_source_text(compiler_ast_program_Program *this, std_span_Span span);
compiler_errors_Error *compiler_ast_program_Program_error(compiler_ast_program_Program *this, compiler_errors_Error *err);
compiler_ast_nodes_Function *compiler_ast_program_Program_get_function_deep_copy(compiler_ast_program_Program *this, compiler_ast_nodes_Function *old, compiler_ast_program_Namespace *ns);
compiler_ast_nodes_Structure *compiler_ast_program_Program_get_struct_deep_copy(compiler_ast_program_Program *this, compiler_ast_nodes_Structure *old, compiler_ast_program_Namespace *ns);
compiler_types_Type *compiler_ast_program_Program_get_base_type(compiler_ast_program_Program *this, compiler_types_BaseType base, std_span_Span span);
compiler_types_Type *compiler_ast_program_Program_get_type_by_name(compiler_ast_program_Program *this, char *name, std_span_Span span);
compiler_ast_program_NSIterator compiler_ast_program_Program_iter_namespaces(compiler_ast_program_Program *this);
bool compiler_ast_program_NSIterator_has_value(compiler_ast_program_NSIterator *this);
void compiler_ast_program_NSIterator_next(compiler_ast_program_NSIterator *this);
compiler_ast_program_Namespace *compiler_ast_program_NSIterator_cur(compiler_ast_program_NSIterator *this);
compiler_ast_operators_Operator compiler_ast_operators_Operator_from_operator_overload(char *s, std_vector_Vector__4 *params);
compiler_ast_operators_Operator compiler_ast_operators_Operator_from_token(compiler_tokens_Token *tok);
u32 compiler_ast_operators_Operator_num_overload_params(compiler_ast_operators_Operator this);
bool compiler_ast_operators_Operator_needs_lhs_pointer_for_overload(compiler_ast_operators_Operator this);
u32 compiler_ast_operators_OperatorOverload_hash(compiler_ast_operators_OperatorOverload this);
bool compiler_ast_operators_OperatorOverload_eq(compiler_ast_operators_OperatorOverload this, compiler_ast_operators_OperatorOverload other);
compiler_ast_nodes_Variable *compiler_ast_nodes_Variable_new(compiler_types_Type *type);
compiler_ast_nodes_Structure *compiler_ast_nodes_Structure_new(void);
compiler_ast_nodes_Variable *compiler_ast_nodes_Structure_get_field(compiler_ast_nodes_Structure *this, char *name);
compiler_ast_nodes_Enum *compiler_ast_nodes_Enum_new(void);
compiler_ast_nodes_Variable *compiler_ast_nodes_Enum_get_field(compiler_ast_nodes_Enum *this, char *name);
compiler_ast_nodes_Function *compiler_ast_nodes_Function_new(void);
bool compiler_ast_nodes_Function_is_template_instance(compiler_ast_nodes_Function *this);
compiler_ast_nodes_Argument *compiler_ast_nodes_Argument_new(compiler_ast_nodes_AST *expr, compiler_tokens_Token *label_token);
compiler_ast_nodes_ImportPart *compiler_ast_nodes_ImportPart_new(compiler_ast_nodes_ImportPartType type, std_span_Span span);
compiler_ast_nodes_MatchCase *compiler_ast_nodes_MatchCase_new(compiler_ast_nodes_AST *cond, compiler_ast_nodes_AST *body);
compiler_ast_nodes_AST *compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType type, std_span_Span span);
compiler_ast_nodes_AST *compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator op, std_span_Span span, compiler_ast_nodes_AST *expr);
compiler_ast_nodes_AST *compiler_ast_nodes_AST_new_binop(compiler_ast_operators_Operator op, compiler_ast_nodes_AST *lhs, compiler_ast_nodes_AST *rhs, std_span_Span op_span);
compiler_ast_scopes_Symbol *compiler_ast_nodes_AST_symbol(compiler_ast_nodes_AST *this);
bool compiler_ast_nodes_AST_is_lvalue(compiler_ast_nodes_AST *this);
compiler_ast_scopes_TemplateInstance *compiler_ast_scopes_TemplateInstance_new(std_vector_Vector__0 *args, compiler_ast_scopes_Symbol *parent, compiler_ast_scopes_Symbol *resolved);
bool compiler_ast_scopes_TemplateInstance_matches(compiler_ast_scopes_TemplateInstance *this, std_vector_Vector__0 *other);
compiler_ast_scopes_Template *compiler_ast_scopes_Template_new(std_vector_Vector__19 *params);
char *compiler_ast_scopes_Symbol_out_name(compiler_ast_scopes_Symbol *this);
compiler_ast_scopes_Symbol *compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType type, compiler_ast_program_Namespace *ns, char *name, char *display, char *full_name, std_span_Span span);
char *compiler_ast_scopes_Symbol_join_display(char *a, char *b);
char *compiler_ast_scopes_Symbol_join_full_name(char *a, char *b);
bool compiler_ast_scopes_Symbol_is_templated(compiler_ast_scopes_Symbol *this);
compiler_ast_scopes_Symbol *compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType type, compiler_ast_program_Namespace *ns, compiler_ast_scopes_Symbol *parent, char *name, std_span_Span span);
void compiler_ast_scopes_Symbol_update_parent(compiler_ast_scopes_Symbol *this, compiler_ast_scopes_Symbol *parent);
compiler_ast_scopes_Symbol *compiler_ast_scopes_Symbol_from_local_variable(char *name, compiler_ast_nodes_Variable *var, std_span_Span span);
compiler_ast_scopes_Symbol *compiler_ast_scopes_Symbol_remove_alias(compiler_ast_scopes_Symbol *this);
void compiler_ast_scopes_Symbol_add_reference(compiler_ast_scopes_Symbol *this, compiler_ast_scopes_ReferenceType type, std_span_Span span);
compiler_ast_scopes_Scope *compiler_ast_scopes_Scope_new(compiler_ast_scopes_Scope *parent);
compiler_ast_scopes_Symbol *compiler_ast_scopes_Scope_lookup_recursive(compiler_ast_scopes_Scope *this, char *name);
compiler_ast_scopes_Symbol *compiler_ast_scopes_Scope_lookup_local(compiler_ast_scopes_Scope *this, char *name);
void compiler_ast_scopes_Scope_insert(compiler_ast_scopes_Scope *this, char *name, compiler_ast_scopes_Symbol *symbol);
void compiler_lsp_typecheck_and_log_errors(compiler_ast_program_Program *program, char *path);
void compiler_lsp_handle_validate(compiler_ast_program_Program *program, char *path);
void compiler_lsp_handle_location_command(compiler_ast_program_Program *program, compiler_lsp_CommandType type, std_span_Location loc);
void compiler_lsp_handle_document_symbols(compiler_ast_program_Program *program, char *path);
char *compiler_lsp_shift_args(i32 *argc, char ***argv);
void compiler_lsp_lsp_usage(i32 code, bool full);
void compiler_lsp_lsp_main(i32 argc, char **argv);
char *compiler_lsp_utils_gen_func_param_string(u32 i, compiler_ast_nodes_Variable *param, bool is_non_static_method);
char *compiler_lsp_utils_gen_type_string(compiler_types_Type *type, bool full);
char *compiler_lsp_utils_try_gen_expr_string(compiler_ast_nodes_AST *expr);
char *compiler_lsp_utils_gen_hover_string(compiler_ast_scopes_Symbol *sym);
compiler_types_Type *compiler_lsp_utils_get_symbol_typedef(compiler_ast_scopes_Symbol *sym);
std_value_Value *compiler_lsp_utils_gen_error_json(compiler_errors_Error *err);
std_value_Value *compiler_lsp_utils_gen_span_json(std_span_Span span);
std_value_Value *compiler_lsp_utils_gen_span_json_with_filename(std_span_Span span, std_span_Location search_loc);
std_value_Value *compiler_lsp_utils_gen_hover_string_with_docs(compiler_ast_scopes_Symbol *sym);
void compiler_lsp_utils_gen_type_methods_into(std_value_Value *obj, compiler_types_Type *type);
std_value_Value *compiler_lsp_utils_gen_enum_json(compiler_ast_nodes_Enum *enum_);
std_value_Value *compiler_lsp_utils_gen_struct_json(compiler_ast_nodes_Structure *struc);
std_value_Value *compiler_lsp_utils_gen_variable_json(compiler_ast_nodes_Variable *var);
std_value_Value *compiler_lsp_utils_gen_function_json(compiler_ast_nodes_Function *func);
std_value_Value *compiler_lsp_utils_gen_namespace_json(compiler_ast_program_Namespace *ns);
std_value_Value *compiler_lsp_utils_gen_completion_from_symbol(compiler_ast_scopes_Symbol *sym);
std_vector_Vector__21 *compiler_lsp_utils_get_unique_reference_spans(compiler_ast_scopes_Symbol *sym, bool for_rename);
std_value_Value *compiler_lsp_utils_gen_references_json(compiler_ast_scopes_Symbol *sym, std_span_Location loc);
std_value_Value *compiler_lsp_utils_gen_signature_help(compiler_ast_nodes_AST *node, u32 active_param);
std_value_Value *compiler_lsp_utils_gen_renames_json(compiler_ast_scopes_Symbol *sym, std_span_Location loc);
std_value_Value *compiler_lsp_utils_gen_completions_json(compiler_ast_scopes_Symbol *sym, bool ns_lookup);
compiler_lsp_finder_Finder compiler_lsp_finder_Finder_make(compiler_lsp_CommandType cmd, std_span_Location loc);
bool compiler_lsp_finder_Finder_find_in_identifier(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node);
bool compiler_lsp_finder_Finder_find_in_var(compiler_lsp_finder_Finder *this, compiler_ast_nodes_Variable *var);
bool compiler_lsp_finder_Finder_set_usage(compiler_lsp_finder_Finder *this, compiler_ast_scopes_Symbol *sym);
bool compiler_lsp_finder_Finder_find_in_literal(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node);
bool compiler_lsp_finder_Finder_find_signature_help(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node, std_vector_Vector__15 *args, u32 param_idx);
bool compiler_lsp_finder_Finder_find_in_call_args(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node, std_vector_Vector__15 *args);
bool compiler_lsp_finder_Finder_find_in_expression(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node);
bool compiler_lsp_finder_Finder_find_in_import_part(compiler_lsp_finder_Finder *this, compiler_ast_nodes_ImportPart *part);
bool compiler_lsp_finder_Finder_find_in_statement(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node);
bool compiler_lsp_finder_Finder_find_in_block(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node);
bool compiler_lsp_finder_Finder_find_in_type(compiler_lsp_finder_Finder *this, compiler_types_Type *type);
bool compiler_lsp_finder_Finder_find_in_function(compiler_lsp_finder_Finder *this, compiler_ast_nodes_Function *func);
bool compiler_lsp_finder_Finder_find_in_program(compiler_lsp_finder_Finder *this, compiler_ast_program_Namespace *ns);
bool compiler_lsp_finder_Finder_find(compiler_lsp_finder_Finder *this, compiler_ast_program_Program *program);
compiler_attributes_AttributeType compiler_attributes_AttributeType_from_str(char *s);
compiler_attributes_Attribute *compiler_attributes_Attribute_new(compiler_attributes_AttributeType type, std_span_Span span);
bool compiler_attributes_Attribute_validate(compiler_attributes_Attribute *this, compiler_parser_Parser *parser_for_errors);
compiler_tokens_Token *compiler_tokens_Token_new(compiler_tokens_TokenType type, std_span_Span span, char *text);
compiler_tokens_Token *compiler_tokens_Token_from_type(compiler_tokens_TokenType type, std_span_Span span);
compiler_tokens_Token *compiler_tokens_Token_from_ident(char *text, std_span_Span span);
bool compiler_tokens_Token_is_word(compiler_tokens_Token this);
bool compiler_tokens_Token_is_identifier(compiler_tokens_Token this, char *name);
compiler_tokens_TokenType compiler_tokens_TokenType_from_text(char *text);
char *compiler_tokens_TokenType_str(compiler_tokens_TokenType this);
char *compiler_errors_MessageType_to_color(compiler_errors_MessageType this);
char *compiler_errors_MessageType_str(compiler_errors_MessageType this);
void compiler_errors_display_line(void);
void compiler_errors_display_message(compiler_errors_MessageType type, std_span_Span span, char *msg);
void compiler_errors_display_message_span(compiler_errors_MessageType type, std_span_Span span, char *msg, bool line_after);
void compiler_errors_Error_display(compiler_errors_Error *this);
void compiler_errors_Error_panic(compiler_errors_Error *this) __attribute__((noreturn));
compiler_errors_Error *compiler_errors_Error_new(std_span_Span span, char *msg);
compiler_errors_Error *compiler_errors_Error_new_note(std_span_Span span, char *msg, char *note);
compiler_errors_Error *compiler_errors_Error_new_hint(std_span_Span span, char *msg, std_span_Span span2, char *hint);
void compiler_errors_display_error_messages(std_vector_Vector__11 *errors, u32 detail_level);
char *compiler_types_BaseType_str(compiler_types_BaseType this);
compiler_types_Type *compiler_types_Type_shallow_copy(compiler_types_Type *old);
compiler_types_Type *compiler_types_Type_new_resolved(compiler_types_BaseType base, std_span_Span span);
compiler_types_Type *compiler_types_Type_new_unresolved(char *name, std_span_Span span);
compiler_types_Type *compiler_types_Type_new_unresolved_base(compiler_types_BaseType base, std_span_Span span);
bool compiler_types_Type_is_integer(compiler_types_Type *this);
bool compiler_types_Type_is_float(compiler_types_Type *this);
bool compiler_types_Type_is_numeric(compiler_types_Type *this);
bool compiler_types_Type_is_numeric_or_char(compiler_types_Type *this);
bool compiler_types_Type_can_have_methods(compiler_types_Type *this);
bool compiler_types_Type_is_resolved(compiler_types_Type *this);
bool compiler_types_Type_eq(compiler_types_Type *this, compiler_types_Type *other, bool strict);
bool compiler_types_Type_is_str(compiler_types_Type *this);
compiler_types_Type *compiler_types_Type_unaliased(compiler_types_Type *this);
compiler_types_Type *compiler_types_Type_decay_array(compiler_types_Type *this);
char *compiler_types_Type_str(compiler_types_Type *this);
void usage(i32 code, bool full);
void save_and_compile_code(compiler_ast_program_Program *program, char *code);
void run_executable(i32 argc, char **argv);
void parse_args(i32 *argc, char ***argv, compiler_ast_program_Program *program);
i32 main(i32 argc, char **argv);
bool compiler_utils_directory_exists(char *path);
void std_panic(char *msg) __attribute__((noreturn));
u32 str_to_u32(char *this);
bool str_eq(char *this, char *other);
char *str_substring(char *this, u32 start, u32 len);
bool str_ends_with(char *this, char *suffix);
void str_strip_trailing_whitespace(char *this);
void str_replace_with(char **this, char *other);
void str_free(char **this);
std_CharIterator str_chars(char *this, u32 start);
bool std_CharIterator_has_value(std_CharIterator *this);
u32 std_CharIterator_next(std_CharIterator *this);
char std_CharIterator_cur(std_CharIterator *this);
bool char_is_digit(char this);
bool char_is_hex_digit(char this);
bool char_is_alpha(char this);
bool char_is_alnum(char this);
bool char_is_print(char this);
u32 u32_min(u32 this, u32 other);
u32 u32_max(u32 this, u32 other);
char *std_shift_args(i32 *argc, char ***argv, char *where);
bool std_sv_SV_is_empty(std_sv_SV *this);
std_sv_SV std_sv_SV_get(std_sv_SV this, u32 n);
char std_sv_SV_at(std_sv_SV this, u32 n);
std_sv_SV std_sv_SV_chop_by_delim(std_sv_SV *this, char delim);
std_sv_SV std_sv_SV_chop_line(std_sv_SV *this);
std_sv_SV std_sv_SV_slice(std_sv_SV this, u32 off, u32 end);
std_sv_SVLineIterator std_sv_SV_lines(std_sv_SV this);
bool std_sv_SVLineIterator_has_value(std_sv_SVLineIterator *this);
std_sv_SV std_sv_SVLineIterator_cur(std_sv_SVLineIterator *this);
void std_sv_SVLineIterator_next(std_sv_SVLineIterator *this);
compiler_passes_reorder_structs_ReorderStructs *std_mem_alloc__0(u32 count);
compiler_passes_mark_dead_code_MarkDeadCode *std_mem_alloc__1(u32 count);
compiler_passes_generic_pass_GenericPass *std_mem_alloc__2(u32 count);
compiler_ast_program_Namespace *std_mem_alloc__3(u32 count);
compiler_ast_program_Program *std_mem_alloc__4(u32 count);
compiler_ast_nodes_Variable *std_mem_alloc__5(u32 count);
compiler_ast_nodes_Structure *std_mem_alloc__6(u32 count);
compiler_ast_nodes_Enum *std_mem_alloc__7(u32 count);
compiler_ast_nodes_Function *std_mem_alloc__8(u32 count);
compiler_ast_nodes_Argument *std_mem_alloc__9(u32 count);
compiler_ast_nodes_ImportPart *std_mem_alloc__10(u32 count);
compiler_ast_nodes_MatchCase *std_mem_alloc__11(u32 count);
compiler_ast_nodes_AST *std_mem_alloc__12(u32 count);
compiler_ast_scopes_TemplateInstance *std_mem_alloc__13(u32 count);
compiler_ast_scopes_Template *std_mem_alloc__14(u32 count);
compiler_ast_scopes_Symbol *std_mem_alloc__15(u32 count);
compiler_ast_scopes_Scope *std_mem_alloc__16(u32 count);
compiler_attributes_Attribute *std_mem_alloc__17(u32 count);
compiler_tokens_Token *std_mem_alloc__18(u32 count);
compiler_errors_Error *std_mem_alloc__19(u32 count);
compiler_types_Type *std_mem_alloc__20(u32 count);
char *std_mem_alloc__21(u32 count);
i32 *std_mem_alloc__22(u32 count);
std_compact_map_Map__0 *std_mem_alloc__23(u32 count);
u8 *std_mem_alloc__24(u32 count);
std_set_Set__0 *std_mem_alloc__25(u32 count);
std_set_Set__1 *std_mem_alloc__26(u32 count);
std_set_Set__2 *std_mem_alloc__27(u32 count);
std_value_Value *std_mem_alloc__28(u32 count);
std_map_Item__0 *std_mem_alloc__29(u32 count);
std_map_Item__0 **std_mem_alloc__30(u32 count);
std_map_Map__0 *std_mem_alloc__31(u32 count);
std_map_Item__1 *std_mem_alloc__32(u32 count);
std_map_Item__1 **std_mem_alloc__33(u32 count);
std_map_Map__1 *std_mem_alloc__34(u32 count);
std_map_Item__2 *std_mem_alloc__35(u32 count);
std_map_Item__2 **std_mem_alloc__36(u32 count);
std_map_Map__2 *std_mem_alloc__37(u32 count);
std_map_Item__3 *std_mem_alloc__38(u32 count);
std_map_Item__3 **std_mem_alloc__39(u32 count);
std_map_Map__3 *std_mem_alloc__40(u32 count);
std_map_Item__4 *std_mem_alloc__41(u32 count);
std_map_Item__4 **std_mem_alloc__42(u32 count);
std_map_Map__4 *std_mem_alloc__43(u32 count);
std_map_Item__5 *std_mem_alloc__44(u32 count);
std_map_Item__5 **std_mem_alloc__45(u32 count);
std_map_Map__5 *std_mem_alloc__46(u32 count);
std_map_Item__6 *std_mem_alloc__47(u32 count);
std_map_Item__6 **std_mem_alloc__48(u32 count);
std_map_Map__6 *std_mem_alloc__49(u32 count);
std_map_Item__7 *std_mem_alloc__50(u32 count);
std_map_Item__7 **std_mem_alloc__51(u32 count);
std_map_Map__7 *std_mem_alloc__52(u32 count);
std_map_Item__8 *std_mem_alloc__53(u32 count);
std_map_Item__8 **std_mem_alloc__54(u32 count);
std_map_Map__8 *std_mem_alloc__55(u32 count);
std_map_Item__9 *std_mem_alloc__56(u32 count);
std_map_Item__9 **std_mem_alloc__57(u32 count);
std_map_Map__9 *std_mem_alloc__58(u32 count);
std_vector_Vector__0 *std_mem_alloc__59(u32 count);
compiler_types_Type **std_mem_alloc__60(u32 count);
std_vector_Vector__1 *std_mem_alloc__61(u32 count);
char **std_mem_alloc__62(u32 count);
std_vector_Vector__2 *std_mem_alloc__63(u32 count);
compiler_ast_nodes_Structure **std_mem_alloc__64(u32 count);
std_vector_Vector__3 *std_mem_alloc__65(u32 count);
compiler_ast_scopes_TemplateInstance **std_mem_alloc__66(u32 count);
std_vector_Vector__4 *std_mem_alloc__67(u32 count);
compiler_ast_nodes_Variable **std_mem_alloc__68(u32 count);
std_vector_Vector__5 *std_mem_alloc__69(u32 count);
compiler_ast_nodes_ImportPart **std_mem_alloc__70(u32 count);
std_vector_Vector__6 *std_mem_alloc__71(u32 count);
compiler_ast_nodes_Function **std_mem_alloc__72(u32 count);
std_vector_Vector__7 *std_mem_alloc__73(u32 count);
compiler_ast_scopes_Scope **std_mem_alloc__74(u32 count);
std_vector_Vector__8 *std_mem_alloc__75(u32 count);
compiler_ast_program_Namespace **std_mem_alloc__76(u32 count);
std_vector_Vector__9 *std_mem_alloc__77(u32 count);
compiler_tokens_Token **std_mem_alloc__78(u32 count);
std_vector_Vector__10 *std_mem_alloc__79(u32 count);
compiler_attributes_Attribute **std_mem_alloc__80(u32 count);
std_vector_Vector__11 *std_mem_alloc__81(u32 count);
compiler_errors_Error **std_mem_alloc__82(u32 count);
std_vector_Vector__12 *std_mem_alloc__83(u32 count);
compiler_ast_nodes_Enum **std_mem_alloc__84(u32 count);
std_vector_Vector__13 *std_mem_alloc__85(u32 count);
compiler_ast_nodes_AST **std_mem_alloc__86(u32 count);
std_vector_Vector__14 *std_mem_alloc__87(u32 count);
compiler_ast_operators_Operator *std_mem_alloc__88(u32 count);
std_vector_Vector__15 *std_mem_alloc__89(u32 count);
compiler_ast_nodes_Argument **std_mem_alloc__90(u32 count);
std_vector_Vector__16 *std_mem_alloc__91(u32 count);
std_vector_Vector__5 **std_mem_alloc__92(u32 count);
std_vector_Vector__17 *std_mem_alloc__93(u32 count);
compiler_ast_nodes_IfBranch *std_mem_alloc__94(u32 count);
std_vector_Vector__18 *std_mem_alloc__95(u32 count);
compiler_ast_nodes_MatchCase **std_mem_alloc__96(u32 count);
std_vector_Vector__19 *std_mem_alloc__97(u32 count);
compiler_ast_scopes_Symbol **std_mem_alloc__98(u32 count);
std_vector_Vector__20 *std_mem_alloc__99(u32 count);
compiler_ast_scopes_Reference *std_mem_alloc__100(u32 count);
std_vector_Vector__21 *std_mem_alloc__101(u32 count);
std_span_Span *std_mem_alloc__102(u32 count);
std_vector_Vector__22 *std_mem_alloc__103(u32 count);
std_value_Value **std_mem_alloc__104(u32 count);
std_vector_Vector__23 *std_mem_alloc__105(u32 count);
std_compact_map_Item__0 *std_mem_alloc__106(u32 count);
std_vector_Vector__24 *std_mem_alloc__107(u32 count);
u32 *std_mem_alloc__108(u32 count);
void std_mem_free(void *ptr);
u8 *std_mem_realloc__0(u8 *ptr, u32 old_count, u32 new_count);
compiler_types_Type **std_mem_realloc__1(compiler_types_Type **ptr, u32 old_count, u32 new_count);
char **std_mem_realloc__2(char **ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_Structure **std_mem_realloc__3(compiler_ast_nodes_Structure **ptr, u32 old_count, u32 new_count);
compiler_ast_scopes_TemplateInstance **std_mem_realloc__4(compiler_ast_scopes_TemplateInstance **ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_Variable **std_mem_realloc__5(compiler_ast_nodes_Variable **ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_ImportPart **std_mem_realloc__6(compiler_ast_nodes_ImportPart **ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_Function **std_mem_realloc__7(compiler_ast_nodes_Function **ptr, u32 old_count, u32 new_count);
compiler_ast_scopes_Scope **std_mem_realloc__8(compiler_ast_scopes_Scope **ptr, u32 old_count, u32 new_count);
compiler_ast_program_Namespace **std_mem_realloc__9(compiler_ast_program_Namespace **ptr, u32 old_count, u32 new_count);
compiler_tokens_Token **std_mem_realloc__10(compiler_tokens_Token **ptr, u32 old_count, u32 new_count);
compiler_attributes_Attribute **std_mem_realloc__11(compiler_attributes_Attribute **ptr, u32 old_count, u32 new_count);
compiler_errors_Error **std_mem_realloc__12(compiler_errors_Error **ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_Enum **std_mem_realloc__13(compiler_ast_nodes_Enum **ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_AST **std_mem_realloc__14(compiler_ast_nodes_AST **ptr, u32 old_count, u32 new_count);
compiler_ast_operators_Operator *std_mem_realloc__15(compiler_ast_operators_Operator *ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_Argument **std_mem_realloc__16(compiler_ast_nodes_Argument **ptr, u32 old_count, u32 new_count);
std_vector_Vector__5 **std_mem_realloc__17(std_vector_Vector__5 **ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_IfBranch *std_mem_realloc__18(compiler_ast_nodes_IfBranch *ptr, u32 old_count, u32 new_count);
compiler_ast_nodes_MatchCase **std_mem_realloc__19(compiler_ast_nodes_MatchCase **ptr, u32 old_count, u32 new_count);
compiler_ast_scopes_Symbol **std_mem_realloc__20(compiler_ast_scopes_Symbol **ptr, u32 old_count, u32 new_count);
compiler_ast_scopes_Reference *std_mem_realloc__21(compiler_ast_scopes_Reference *ptr, u32 old_count, u32 new_count);
std_span_Span *std_mem_realloc__22(std_span_Span *ptr, u32 old_count, u32 new_count);
std_value_Value **std_mem_realloc__23(std_value_Value **ptr, u32 old_count, u32 new_count);
std_compact_map_Item__0 *std_mem_realloc__24(std_compact_map_Item__0 *ptr, u32 old_count, u32 new_count);
u32 *std_mem_realloc__25(u32 *ptr, u32 old_count, u32 new_count);
void *std_mem_impl_my_calloc(void *state, u32 size);
void *std_mem_impl_my_realloc(void *state, void *ptr, u32 old_size, u32 size);
void std_mem_impl_my_free(void *state, void *ptr);
bool std_compact_map_Map__0_contains(std_compact_map_Map__0 *this, char *key);
std_compact_map_Iterator__0 std_compact_map_Map__0_iter(std_compact_map_Map__0 *this);
std_compact_map_Item__0 std_compact_map_Iterator__0_cur(std_compact_map_Iterator__0 *this);
void std_compact_map_Iterator__0_next(std_compact_map_Iterator__0 *this);
bool std_compact_map_Iterator__0_has_value(std_compact_map_Iterator__0 *this);
void std_compact_map_Map__0_insert(std_compact_map_Map__0 *this, char *key, std_value_Value *value);
u32 std_compact_map_Map__0_get_index(std_compact_map_Map__0 *this, char *key, u32 hash);
std_value_Value *std_compact_map_Map__0_get(std_compact_map_Map__0 *this, char *key, std_value_Value *defolt);
bool std_compact_map_Map__0_is_empty(std_compact_map_Map__0 *this);
void std_compact_map_Map__0_resize(std_compact_map_Map__0 *this, u32 new_capacity);
std_compact_map_Item__0 *std_compact_map_Map__0_get_item(std_compact_map_Map__0 *this, char *key);
std_compact_map_Map__0 *std_compact_map_Map__0_new(u32 capacity);
std_buffer_Buffer std_buffer_Buffer_make(u32 capacity);
std_buffer_Buffer std_buffer_Buffer_from_str(char *s);
void std_buffer_Buffer_resize_if_necessary(std_buffer_Buffer *this, u32 new_size);
void std_buffer_Buffer_write_str(std_buffer_Buffer *this, char *s);
void std_buffer_Buffer_write_str_f(std_buffer_Buffer *this, char *s);
void std_buffer_Buffer_write_char(std_buffer_Buffer *this, char c);
void std_buffer_Buffer_write_u8(std_buffer_Buffer *this, u8 value);
char *std_buffer_Buffer_new_str(std_buffer_Buffer this);
char *std_buffer_Buffer_str(std_buffer_Buffer this);
std_sv_SV std_buffer_Buffer_sv(std_buffer_Buffer this);
void std_buffer_Buffer_clear(std_buffer_Buffer *this);
void std_buffer_Buffer_free(std_buffer_Buffer *this);
void std_set_Set__0_add(std_set_Set__0 *this, char *key);
bool std_set_Set__0_contains(std_set_Set__0 *this, char *key);
void std_set_Set__0_free(std_set_Set__0 *this);
std_set_Set__0 *std_set_Set__0_new(void);
void std_set_Set__1_add(std_set_Set__1 *this, u64 key);
bool std_set_Set__1_contains(std_set_Set__1 *this, u64 key);
void std_set_Set__1_free(std_set_Set__1 *this);
std_set_Set__1 *std_set_Set__1_new(void);
void std_set_Set__2_add(std_set_Set__2 *this, std_span_Span key);
std_set_Iterator__2 std_set_Set__2_iter(std_set_Set__2 *this);
std_span_Span std_set_Iterator__2_cur(std_set_Iterator__2 *this);
void std_set_Iterator__2_next(std_set_Iterator__2 *this);
std_set_Iterator__2 std_set_Iterator__2_make(std_set_Set__2 *set);
bool std_set_Iterator__2_has_value(std_set_Iterator__2 *this);
void std_set_Set__2_free(std_set_Set__2 *this);
std_set_Set__2 *std_set_Set__2_new(void);
std_span_Location std_span_Location_default(void);
u32 std_span_Location_hash(std_span_Location this);
bool std_span_Location_eq(std_span_Location this, std_span_Location other);
char *std_span_Location_str(std_span_Location *this);
bool std_span_Location_is_valid(std_span_Location *this);
bool std_span_Location_is_before(std_span_Location *this, std_span_Location other);
std_span_Span std_span_Span_default(void);
u32 std_span_Span_hash(std_span_Span this);
bool std_span_Span_eq(std_span_Span this, std_span_Span other);
bool std_span_Span_is_valid(std_span_Span this);
std_span_Span std_span_Span_join(std_span_Span this, std_span_Span other);
bool std_span_Span_contains_loc(std_span_Span this, std_span_Location loc);
bool std_span_Span_starts_right_after(std_span_Span this, std_span_Span other);
bool u64_eq(u64 this, u64 other);
u32 std_traits_hash_hash_bytes(u8 *data, u32 len);
u32 std_traits_hash_pair_hash(u32 a, u32 b);
u32 str_hash(char *this);
u32 u32_hash(u32 this);
u32 u64_hash(u64 this);
std_value_Value *std_value_Value_new(std_value_ValueType type);
std_value_Value *std_value_Value_new_str(char *s);
std_value_Value *std_value_Value_new_bool(bool bul);
std_value_Value *std_value_Value_new_int(i64 num);
void std_value_Value_ensure(std_value_Value *this, std_value_ValueType type);
std_value_Value *std_value_Value_at_key(std_value_Value *this, char *key);
void std_value_Value_push(std_value_Value *this, std_value_Value *value);
std_value_Value *std_value_Value_get(std_value_Value *this, char *key, std_value_Value *defolt);
void std_value_Value_insert(std_value_Value *this, char *key, std_value_Value *value);
void std_value_Value_insert_str(std_value_Value *this, char *key, char *s);
void std_value_Value_insert_u32(std_value_Value *this, char *key, u32 i);
std_compact_map_Map__0 *std_value_Value_as_dict(std_value_Value *this);
char *std_value_Value_dbg(std_value_Value *this);
char *std_fs_realpath(char *path);
bool std_fs_file_exists(char *path);
void std_fs_write_file_bytes(char *path, void *data, u32 size);
void std_fs_write_file_str(char *path, char *data);
void std_fs_write_file(char *path, std_buffer_Buffer data);
std_buffer_Buffer std_fs_read_file(char *path);
void std_map_Item__0_free_list(std_map_Item__0 *this);
std_map_Item__0 *std_map_Item__0_new(char *key, bool value, std_map_Item__0 *next);
bool std_map_Map__0_contains(std_map_Map__0 *this, char *key);
void std_map_Map__0_free(std_map_Map__0 *this);
void std_map_Map__0_insert(std_map_Map__0 *this, char *key, bool value);
void std_map_Map__0_resize(std_map_Map__0 *this);
u32 std_map_Map__0_hash(std_map_Map__0 *this, char *key);
std_map_Item__0 *std_map_Map__0_get_item(std_map_Map__0 *this, char *key);
std_map_Map__0 *std_map_Map__0_new(u32 capacity);
void std_map_Item__1_free_list(std_map_Item__1 *this);
std_map_Item__1 *std_map_Item__1_new(u64 key, bool value, std_map_Item__1 *next);
bool std_map_Map__1_contains(std_map_Map__1 *this, u64 key);
void std_map_Map__1_free(std_map_Map__1 *this);
void std_map_Map__1_insert(std_map_Map__1 *this, u64 key, bool value);
void std_map_Map__1_resize(std_map_Map__1 *this);
u32 std_map_Map__1_hash(std_map_Map__1 *this, u64 key);
std_map_Item__1 *std_map_Map__1_get_item(std_map_Map__1 *this, u64 key);
std_map_Map__1 *std_map_Map__1_new(u32 capacity);
void std_map_Item__2_free_list(std_map_Item__2 *this);
std_map_Item__2 *std_map_Item__2_new(char *key, compiler_types_Type *value, std_map_Item__2 *next);
std_map_Item__2 *std_map_Iterator__2_cur(std_map_Iterator__2 *this);
void std_map_Iterator__2_next(std_map_Iterator__2 *this);
std_map_Iterator__2 std_map_Iterator__2_make(std_map_Map__2 *map);
bool std_map_Iterator__2_has_value(std_map_Iterator__2 *this);
std_map_Iterator__2 std_map_Map__2_iter(std_map_Map__2 *this);
void std_map_Map__2_insert(std_map_Map__2 *this, char *key, compiler_types_Type *value);
compiler_types_Type *std_map_Map__2_get(std_map_Map__2 *this, char *key, compiler_types_Type *defolt);
bool std_map_Map__2_is_empty(std_map_Map__2 *this);
void std_map_Map__2_resize(std_map_Map__2 *this);
u32 std_map_Map__2_hash(std_map_Map__2 *this, char *key);
std_map_Item__2 *std_map_Map__2_get_item(std_map_Map__2 *this, char *key);
std_map_Map__2 *std_map_Map__2_new(u32 capacity);
void std_map_Item__3_free_list(std_map_Item__3 *this);
std_map_Item__3 *std_map_Item__3_new(char *key, compiler_ast_program_Namespace *value, std_map_Item__3 *next);
bool std_map_Map__3_contains(std_map_Map__3 *this, char *key);
std_map_ValueIterator__3 std_map_Map__3_iter_values(std_map_Map__3 *this);
std_map_Item__3 *std_map_Iterator__3_cur(std_map_Iterator__3 *this);
void std_map_Iterator__3_next(std_map_Iterator__3 *this);
std_map_Iterator__3 std_map_Iterator__3_make(std_map_Map__3 *map);
bool std_map_Iterator__3_has_value(std_map_Iterator__3 *this);
compiler_ast_program_Namespace *std_map_ValueIterator__3_cur(std_map_ValueIterator__3 *this);
void std_map_ValueIterator__3_next(std_map_ValueIterator__3 *this);
bool std_map_ValueIterator__3_has_value(std_map_ValueIterator__3 *this);
std_map_Iterator__3 std_map_Map__3_iter(std_map_Map__3 *this);
compiler_ast_program_Namespace *std_map_Map__3_at(std_map_Map__3 *this, char *key);
void std_map_Map__3_insert(std_map_Map__3 *this, char *key, compiler_ast_program_Namespace *value);
compiler_ast_program_Namespace *std_map_Map__3_get(std_map_Map__3 *this, char *key, compiler_ast_program_Namespace *defolt);
bool std_map_Map__3_is_empty(std_map_Map__3 *this);
void std_map_Map__3_resize(std_map_Map__3 *this);
u32 std_map_Map__3_hash(std_map_Map__3 *this, char *key);
std_map_Item__3 *std_map_Map__3_get_item(std_map_Map__3 *this, char *key);
std_map_Map__3 *std_map_Map__3_new(u32 capacity);
void std_map_Item__4_free_list(std_map_Item__4 *this);
std_map_Item__4 *std_map_Item__4_new(char *key, compiler_ast_scopes_Symbol *value, std_map_Item__4 *next);
bool std_map_Map__4_contains(std_map_Map__4 *this, char *key);
std_map_Item__4 *std_map_Iterator__4_cur(std_map_Iterator__4 *this);
void std_map_Iterator__4_next(std_map_Iterator__4 *this);
std_map_Iterator__4 std_map_Iterator__4_make(std_map_Map__4 *map);
bool std_map_Iterator__4_has_value(std_map_Iterator__4 *this);
std_map_Iterator__4 std_map_Map__4_iter(std_map_Map__4 *this);
compiler_ast_scopes_Symbol *std_map_Map__4_at(std_map_Map__4 *this, char *key);
void std_map_Map__4_insert(std_map_Map__4 *this, char *key, compiler_ast_scopes_Symbol *value);
compiler_ast_scopes_Symbol *std_map_Map__4_get(std_map_Map__4 *this, char *key, compiler_ast_scopes_Symbol *defolt);
void std_map_Map__4_resize(std_map_Map__4 *this);
u32 std_map_Map__4_hash(std_map_Map__4 *this, char *key);
std_map_Item__4 *std_map_Map__4_get_item(std_map_Map__4 *this, char *key);
std_map_Map__4 *std_map_Map__4_new(u32 capacity);
void std_map_Item__5_free_list(std_map_Item__5 *this);
std_map_Item__5 *std_map_Item__5_new(compiler_ast_operators_OperatorOverload key, compiler_ast_nodes_Function *value, std_map_Item__5 *next);
void std_map_Map__5_insert(std_map_Map__5 *this, compiler_ast_operators_OperatorOverload key, compiler_ast_nodes_Function *value);
compiler_ast_nodes_Function *std_map_Map__5_get(std_map_Map__5 *this, compiler_ast_operators_OperatorOverload key, compiler_ast_nodes_Function *defolt);
void std_map_Map__5_resize(std_map_Map__5 *this);
u32 std_map_Map__5_hash(std_map_Map__5 *this, compiler_ast_operators_OperatorOverload key);
std_map_Item__5 *std_map_Map__5_get_item(std_map_Map__5 *this, compiler_ast_operators_OperatorOverload key);
std_map_Map__5 *std_map_Map__5_new(u32 capacity);
void std_map_Item__6_free_list(std_map_Item__6 *this);
std_map_Item__6 *std_map_Item__6_new(char *key, char *value, std_map_Item__6 *next);
bool std_map_Map__6_contains(std_map_Map__6 *this, char *key);
std_map_Item__6 *std_map_Iterator__6_cur(std_map_Iterator__6 *this);
void std_map_Iterator__6_next(std_map_Iterator__6 *this);
std_map_Iterator__6 std_map_Iterator__6_make(std_map_Map__6 *map);
bool std_map_Iterator__6_has_value(std_map_Iterator__6 *this);
std_map_Iterator__6 std_map_Map__6_iter(std_map_Map__6 *this);
void std_map_Map__6_insert(std_map_Map__6 *this, char *key, char *value);
char *std_map_Map__6_get(std_map_Map__6 *this, char *key, char *defolt);
void std_map_Map__6_resize(std_map_Map__6 *this);
u32 std_map_Map__6_hash(std_map_Map__6 *this, char *key);
std_map_Item__6 *std_map_Map__6_get_item(std_map_Map__6 *this, char *key);
std_map_Map__6 *std_map_Map__6_new(u32 capacity);
void std_map_Item__7_free_list(std_map_Item__7 *this);
std_map_Item__7 *std_map_Item__7_new(char *key, compiler_ast_nodes_Function *value, std_map_Item__7 *next);
std_map_ValueIterator__7 std_map_Map__7_iter_values(std_map_Map__7 *this);
std_map_Item__7 *std_map_Iterator__7_cur(std_map_Iterator__7 *this);
void std_map_Iterator__7_next(std_map_Iterator__7 *this);
std_map_Iterator__7 std_map_Iterator__7_make(std_map_Map__7 *map);
bool std_map_Iterator__7_has_value(std_map_Iterator__7 *this);
compiler_ast_nodes_Function *std_map_ValueIterator__7_cur(std_map_ValueIterator__7 *this);
void std_map_ValueIterator__7_next(std_map_ValueIterator__7 *this);
bool std_map_ValueIterator__7_has_value(std_map_ValueIterator__7 *this);
std_map_Iterator__7 std_map_Map__7_iter(std_map_Map__7 *this);
compiler_ast_nodes_Function *std_map_Map__7_at(std_map_Map__7 *this, char *key);
void std_map_Map__7_insert(std_map_Map__7 *this, char *key, compiler_ast_nodes_Function *value);
compiler_ast_nodes_Function *std_map_Map__7_get(std_map_Map__7 *this, char *key, compiler_ast_nodes_Function *defolt);
void std_map_Map__7_resize(std_map_Map__7 *this);
u32 std_map_Map__7_hash(std_map_Map__7 *this, char *key);
std_map_Item__7 *std_map_Map__7_get_item(std_map_Map__7 *this, char *key);
std_map_Map__7 *std_map_Map__7_new(u32 capacity);
void std_map_Item__8_free_list(std_map_Item__8 *this);
std_map_Item__8 *std_map_Item__8_new(char *key, compiler_ast_nodes_MatchCase *value, std_map_Item__8 *next);
bool std_map_Map__8_contains(std_map_Map__8 *this, char *key);
void std_map_Map__8_free(std_map_Map__8 *this);
void std_map_Map__8_insert(std_map_Map__8 *this, char *key, compiler_ast_nodes_MatchCase *value);
compiler_ast_nodes_MatchCase *std_map_Map__8_get(std_map_Map__8 *this, char *key, compiler_ast_nodes_MatchCase *defolt);
void std_map_Map__8_resize(std_map_Map__8 *this);
u32 std_map_Map__8_hash(std_map_Map__8 *this, char *key);
std_map_Item__8 *std_map_Map__8_get_item(std_map_Map__8 *this, char *key);
std_map_Map__8 *std_map_Map__8_new(u32 capacity);
void std_map_Item__9_free_list(std_map_Item__9 *this);
std_map_Item__9 *std_map_Item__9_new(std_span_Span key, bool value, std_map_Item__9 *next);
std_map_Item__9 *std_map_Iterator__9_cur(std_map_Iterator__9 *this);
void std_map_Iterator__9_next(std_map_Iterator__9 *this);
std_map_Iterator__9 std_map_Iterator__9_make(std_map_Map__9 *map);
bool std_map_Iterator__9_has_value(std_map_Iterator__9 *this);
std_map_Iterator__9 std_map_Map__9_iter(std_map_Map__9 *this);
void std_map_Map__9_free(std_map_Map__9 *this);
void std_map_Map__9_insert(std_map_Map__9 *this, std_span_Span key, bool value);
void std_map_Map__9_resize(std_map_Map__9 *this);
u32 std_map_Map__9_hash(std_map_Map__9 *this, std_span_Span key);
std_map_Item__9 *std_map_Map__9_get_item(std_map_Map__9 *this, std_span_Span key);
std_map_Map__9 *std_map_Map__9_new(u32 capacity);
std_vector_Iterator__0 std_vector_Vector__0_iter(std_vector_Vector__0 *this);
compiler_types_Type *std_vector_Iterator__0_cur(std_vector_Iterator__0 *this);
void std_vector_Iterator__0_next(std_vector_Iterator__0 *this);
std_vector_Iterator__0 std_vector_Iterator__0_make(std_vector_Vector__0 *vec);
bool std_vector_Iterator__0_has_value(std_vector_Iterator__0 *this);
compiler_types_Type *std_vector_Vector__0_at(std_vector_Vector__0 *this, u32 i);
void std_vector_Vector__0_resize(std_vector_Vector__0 *this, u32 new_capacity);
std_vector_Vector__0 *std_vector_Vector__0_new(u32 capacity);
void std_vector_Vector__0_push(std_vector_Vector__0 *this, compiler_types_Type *value);
char *std_vector_Vector__1_pop(std_vector_Vector__1 *this);
char *std_vector_Vector__1_back(std_vector_Vector__1 *this, u32 i);
std_vector_Iterator__1 std_vector_Vector__1_iter(std_vector_Vector__1 *this);
char *std_vector_Iterator__1_cur(std_vector_Iterator__1 *this);
void std_vector_Iterator__1_next(std_vector_Iterator__1 *this);
std_vector_Iterator__1 std_vector_Iterator__1_make(std_vector_Vector__1 *vec);
bool std_vector_Iterator__1_has_value(std_vector_Iterator__1 *this);
char *std_vector_Vector__1_at(std_vector_Vector__1 *this, u32 i);
void std_vector_Vector__1_free(std_vector_Vector__1 *this);
void std_vector_Vector__1_resize(std_vector_Vector__1 *this, u32 new_capacity);
std_vector_Vector__1 *std_vector_Vector__1_new(u32 capacity);
void std_vector_Vector__1_push(std_vector_Vector__1 *this, char *value);
std_vector_Iterator__2 std_vector_Vector__2_iter(std_vector_Vector__2 *this);
compiler_ast_nodes_Structure *std_vector_Iterator__2_cur(std_vector_Iterator__2 *this);
void std_vector_Iterator__2_next(std_vector_Iterator__2 *this);
std_vector_Iterator__2 std_vector_Iterator__2_make(std_vector_Vector__2 *vec);
bool std_vector_Iterator__2_has_value(std_vector_Iterator__2 *this);
void std_vector_Vector__2_free(std_vector_Vector__2 *this);
bool std_vector_Vector__2_is_empty(std_vector_Vector__2 *this);
void std_vector_Vector__2_resize(std_vector_Vector__2 *this, u32 new_capacity);
std_vector_Vector__2 *std_vector_Vector__2_new(u32 capacity);
void std_vector_Vector__2_push(std_vector_Vector__2 *this, compiler_ast_nodes_Structure *value);
std_vector_Iterator__3 std_vector_Vector__3_iter(std_vector_Vector__3 *this);
compiler_ast_scopes_TemplateInstance *std_vector_Iterator__3_cur(std_vector_Iterator__3 *this);
void std_vector_Iterator__3_next(std_vector_Iterator__3 *this);
std_vector_Iterator__3 std_vector_Iterator__3_make(std_vector_Vector__3 *vec);
bool std_vector_Iterator__3_has_value(std_vector_Iterator__3 *this);
void std_vector_Vector__3_resize(std_vector_Vector__3 *this, u32 new_capacity);
std_vector_Vector__3 *std_vector_Vector__3_new(u32 capacity);
void std_vector_Vector__3_push(std_vector_Vector__3 *this, compiler_ast_scopes_TemplateInstance *value);
compiler_ast_nodes_Variable *std_vector_Vector__4_back(std_vector_Vector__4 *this, u32 i);
std_vector_Iterator__4 std_vector_Vector__4_iter(std_vector_Vector__4 *this);
compiler_ast_nodes_Variable *std_vector_Iterator__4_cur(std_vector_Iterator__4 *this);
void std_vector_Iterator__4_next(std_vector_Iterator__4 *this);
std_vector_Iterator__4 std_vector_Iterator__4_make(std_vector_Vector__4 *vec);
bool std_vector_Iterator__4_has_value(std_vector_Iterator__4 *this);
compiler_ast_nodes_Variable *std_vector_Vector__4_at(std_vector_Vector__4 *this, u32 i);
void std_vector_Vector__4_free(std_vector_Vector__4 *this);
bool std_vector_Vector__4_is_empty(std_vector_Vector__4 *this);
void std_vector_Vector__4_resize(std_vector_Vector__4 *this, u32 new_capacity);
std_vector_Vector__4 *std_vector_Vector__4_new(u32 capacity);
void std_vector_Vector__4_push(std_vector_Vector__4 *this, compiler_ast_nodes_Variable *value);
std_vector_Iterator__5 std_vector_Vector__5_iter(std_vector_Vector__5 *this);
compiler_ast_nodes_ImportPart *std_vector_Iterator__5_cur(std_vector_Iterator__5 *this);
void std_vector_Iterator__5_next(std_vector_Iterator__5 *this);
std_vector_Iterator__5 std_vector_Iterator__5_make(std_vector_Vector__5 *vec);
bool std_vector_Iterator__5_has_value(std_vector_Iterator__5 *this);
compiler_ast_nodes_ImportPart *std_vector_Vector__5_at(std_vector_Vector__5 *this, u32 i);
void std_vector_Vector__5_resize(std_vector_Vector__5 *this, u32 new_capacity);
std_vector_Vector__5 *std_vector_Vector__5_new(u32 capacity);
void std_vector_Vector__5_push(std_vector_Vector__5 *this, compiler_ast_nodes_ImportPart *value);
compiler_ast_nodes_Function *std_vector_Vector__6_pop(std_vector_Vector__6 *this);
std_vector_Iterator__6 std_vector_Vector__6_iter(std_vector_Vector__6 *this);
compiler_ast_nodes_Function *std_vector_Iterator__6_cur(std_vector_Iterator__6 *this);
void std_vector_Iterator__6_next(std_vector_Iterator__6 *this);
std_vector_Iterator__6 std_vector_Iterator__6_make(std_vector_Vector__6 *vec);
bool std_vector_Iterator__6_has_value(std_vector_Iterator__6 *this);
bool std_vector_Vector__6_is_empty(std_vector_Vector__6 *this);
void std_vector_Vector__6_resize(std_vector_Vector__6 *this, u32 new_capacity);
std_vector_Vector__6 *std_vector_Vector__6_new(u32 capacity);
void std_vector_Vector__6_push(std_vector_Vector__6 *this, compiler_ast_nodes_Function *value);
compiler_ast_scopes_Scope *std_vector_Vector__7_pop(std_vector_Vector__7 *this);
compiler_ast_scopes_Scope *std_vector_Vector__7_at(std_vector_Vector__7 *this, u32 i);
void std_vector_Vector__7_resize(std_vector_Vector__7 *this, u32 new_capacity);
std_vector_Vector__7 *std_vector_Vector__7_new(u32 capacity);
void std_vector_Vector__7_push(std_vector_Vector__7 *this, compiler_ast_scopes_Scope *value);
compiler_ast_program_Namespace *std_vector_Vector__8_pop(std_vector_Vector__8 *this);
compiler_ast_program_Namespace *std_vector_Vector__8_at(std_vector_Vector__8 *this, u32 i);
void std_vector_Vector__8_free(std_vector_Vector__8 *this);
bool std_vector_Vector__8_is_empty(std_vector_Vector__8 *this);
void std_vector_Vector__8_resize(std_vector_Vector__8 *this, u32 new_capacity);
std_vector_Vector__8 *std_vector_Vector__8_new(u32 capacity);
void std_vector_Vector__8_push(std_vector_Vector__8 *this, compiler_ast_program_Namespace *value);
compiler_tokens_Token *std_vector_Vector__9_at(std_vector_Vector__9 *this, u32 i);
void std_vector_Vector__9_free(std_vector_Vector__9 *this);
compiler_tokens_Token *std_vector_Vector__9_unchecked_at(std_vector_Vector__9 *this, u32 i);
void std_vector_Vector__9_resize(std_vector_Vector__9 *this, u32 new_capacity);
std_vector_Vector__9 *std_vector_Vector__9_new(u32 capacity);
void std_vector_Vector__9_push(std_vector_Vector__9 *this, compiler_tokens_Token *value);
std_vector_Iterator__10 std_vector_Vector__10_iter(std_vector_Vector__10 *this);
compiler_attributes_Attribute *std_vector_Iterator__10_cur(std_vector_Iterator__10 *this);
void std_vector_Iterator__10_next(std_vector_Iterator__10 *this);
std_vector_Iterator__10 std_vector_Iterator__10_make(std_vector_Vector__10 *vec);
bool std_vector_Iterator__10_has_value(std_vector_Iterator__10 *this);
void std_vector_Vector__10_resize(std_vector_Vector__10 *this, u32 new_capacity);
std_vector_Vector__10 *std_vector_Vector__10_new(u32 capacity);
void std_vector_Vector__10_push(std_vector_Vector__10 *this, compiler_attributes_Attribute *value);
void std_vector_Vector__10_clear(std_vector_Vector__10 *this);
std_vector_Iterator__11 std_vector_Vector__11_iter(std_vector_Vector__11 *this);
compiler_errors_Error *std_vector_Iterator__11_cur(std_vector_Iterator__11 *this);
void std_vector_Iterator__11_next(std_vector_Iterator__11 *this);
std_vector_Iterator__11 std_vector_Iterator__11_make(std_vector_Vector__11 *vec);
bool std_vector_Iterator__11_has_value(std_vector_Iterator__11 *this);
compiler_errors_Error *std_vector_Vector__11_at(std_vector_Vector__11 *this, u32 i);
void std_vector_Vector__11_free(std_vector_Vector__11 *this);
bool std_vector_Vector__11_is_empty(std_vector_Vector__11 *this);
void std_vector_Vector__11_resize(std_vector_Vector__11 *this, u32 new_capacity);
std_vector_Vector__11 *std_vector_Vector__11_new(u32 capacity);
void std_vector_Vector__11_push(std_vector_Vector__11 *this, compiler_errors_Error *value);
std_vector_Iterator__12 std_vector_Vector__12_iter(std_vector_Vector__12 *this);
compiler_ast_nodes_Enum *std_vector_Iterator__12_cur(std_vector_Iterator__12 *this);
void std_vector_Iterator__12_next(std_vector_Iterator__12 *this);
std_vector_Iterator__12 std_vector_Iterator__12_make(std_vector_Vector__12 *vec);
bool std_vector_Iterator__12_has_value(std_vector_Iterator__12 *this);
bool std_vector_Vector__12_is_empty(std_vector_Vector__12 *this);
void std_vector_Vector__12_resize(std_vector_Vector__12 *this, u32 new_capacity);
std_vector_Vector__12 *std_vector_Vector__12_new(u32 capacity);
void std_vector_Vector__12_push(std_vector_Vector__12 *this, compiler_ast_nodes_Enum *value);
std_vector_Iterator__13 std_vector_Vector__13_iter(std_vector_Vector__13 *this);
compiler_ast_nodes_AST *std_vector_Iterator__13_cur(std_vector_Iterator__13 *this);
void std_vector_Iterator__13_next(std_vector_Iterator__13 *this);
std_vector_Iterator__13 std_vector_Iterator__13_make(std_vector_Vector__13 *vec);
bool std_vector_Iterator__13_has_value(std_vector_Iterator__13 *this);
compiler_ast_nodes_AST *std_vector_Vector__13_at(std_vector_Vector__13 *this, u32 i);
void std_vector_Vector__13_free(std_vector_Vector__13 *this);
bool std_vector_Vector__13_is_empty(std_vector_Vector__13 *this);
void std_vector_Vector__13_resize(std_vector_Vector__13 *this, u32 new_capacity);
std_vector_Vector__13 *std_vector_Vector__13_new(u32 capacity);
void std_vector_Vector__13_push(std_vector_Vector__13 *this, compiler_ast_nodes_AST *value);
std_vector_Iterator__14 std_vector_Vector__14_iter(std_vector_Vector__14 *this);
compiler_ast_operators_Operator std_vector_Iterator__14_cur(std_vector_Iterator__14 *this);
void std_vector_Iterator__14_next(std_vector_Iterator__14 *this);
std_vector_Iterator__14 std_vector_Iterator__14_make(std_vector_Vector__14 *vec);
bool std_vector_Iterator__14_has_value(std_vector_Iterator__14 *this);
void std_vector_Vector__14_resize(std_vector_Vector__14 *this, u32 new_capacity);
std_vector_Vector__14 *std_vector_Vector__14_new(u32 capacity);
void std_vector_Vector__14_push(std_vector_Vector__14 *this, compiler_ast_operators_Operator value);
void std_vector_Vector__15_push_front(std_vector_Vector__15 *this, compiler_ast_nodes_Argument *value);
std_vector_Iterator__15 std_vector_Vector__15_iter(std_vector_Vector__15 *this);
compiler_ast_nodes_Argument *std_vector_Iterator__15_cur(std_vector_Iterator__15 *this);
void std_vector_Iterator__15_next(std_vector_Iterator__15 *this);
std_vector_Iterator__15 std_vector_Iterator__15_make(std_vector_Vector__15 *vec);
bool std_vector_Iterator__15_has_value(std_vector_Iterator__15 *this);
compiler_ast_nodes_Argument *std_vector_Vector__15_at(std_vector_Vector__15 *this, u32 i);
void std_vector_Vector__15_resize(std_vector_Vector__15 *this, u32 new_capacity);
std_vector_Vector__15 *std_vector_Vector__15_new(u32 capacity);
void std_vector_Vector__15_push(std_vector_Vector__15 *this, compiler_ast_nodes_Argument *value);
std_vector_Iterator__16 std_vector_Vector__16_iter(std_vector_Vector__16 *this);
std_vector_Vector__5 *std_vector_Iterator__16_cur(std_vector_Iterator__16 *this);
void std_vector_Iterator__16_next(std_vector_Iterator__16 *this);
std_vector_Iterator__16 std_vector_Iterator__16_make(std_vector_Vector__16 *vec);
bool std_vector_Iterator__16_has_value(std_vector_Iterator__16 *this);
std_vector_Vector__5 *std_vector_Vector__16_at(std_vector_Vector__16 *this, u32 i);
void std_vector_Vector__16_resize(std_vector_Vector__16 *this, u32 new_capacity);
std_vector_Vector__16 *std_vector_Vector__16_new(u32 capacity);
void std_vector_Vector__16_push(std_vector_Vector__16 *this, std_vector_Vector__5 *value);
std_vector_Iterator__17 std_vector_Vector__17_iter(std_vector_Vector__17 *this);
compiler_ast_nodes_IfBranch std_vector_Iterator__17_cur(std_vector_Iterator__17 *this);
void std_vector_Iterator__17_next(std_vector_Iterator__17 *this);
std_vector_Iterator__17 std_vector_Iterator__17_make(std_vector_Vector__17 *vec);
bool std_vector_Iterator__17_has_value(std_vector_Iterator__17 *this);
compiler_ast_nodes_IfBranch std_vector_Vector__17_at(std_vector_Vector__17 *this, u32 i);
void std_vector_Vector__17_resize(std_vector_Vector__17 *this, u32 new_capacity);
std_vector_Vector__17 *std_vector_Vector__17_new(u32 capacity);
void std_vector_Vector__17_push(std_vector_Vector__17 *this, compiler_ast_nodes_IfBranch value);
std_vector_Iterator__18 std_vector_Vector__18_iter(std_vector_Vector__18 *this);
compiler_ast_nodes_MatchCase *std_vector_Iterator__18_cur(std_vector_Iterator__18 *this);
void std_vector_Iterator__18_next(std_vector_Iterator__18 *this);
std_vector_Iterator__18 std_vector_Iterator__18_make(std_vector_Vector__18 *vec);
bool std_vector_Iterator__18_has_value(std_vector_Iterator__18 *this);
compiler_ast_nodes_MatchCase *std_vector_Vector__18_at(std_vector_Vector__18 *this, u32 i);
void std_vector_Vector__18_resize(std_vector_Vector__18 *this, u32 new_capacity);
std_vector_Vector__18 *std_vector_Vector__18_new(u32 capacity);
void std_vector_Vector__18_push(std_vector_Vector__18 *this, compiler_ast_nodes_MatchCase *value);
std_vector_Iterator__19 std_vector_Vector__19_iter(std_vector_Vector__19 *this);
compiler_ast_scopes_Symbol *std_vector_Iterator__19_cur(std_vector_Iterator__19 *this);
void std_vector_Iterator__19_next(std_vector_Iterator__19 *this);
std_vector_Iterator__19 std_vector_Iterator__19_make(std_vector_Vector__19 *vec);
bool std_vector_Iterator__19_has_value(std_vector_Iterator__19 *this);
compiler_ast_scopes_Symbol *std_vector_Vector__19_at(std_vector_Vector__19 *this, u32 i);
void std_vector_Vector__19_resize(std_vector_Vector__19 *this, u32 new_capacity);
std_vector_Vector__19 *std_vector_Vector__19_new(u32 capacity);
void std_vector_Vector__19_push(std_vector_Vector__19 *this, compiler_ast_scopes_Symbol *value);
std_vector_Iterator__20 std_vector_Vector__20_iter(std_vector_Vector__20 *this);
compiler_ast_scopes_Reference std_vector_Iterator__20_cur(std_vector_Iterator__20 *this);
void std_vector_Iterator__20_next(std_vector_Iterator__20 *this);
std_vector_Iterator__20 std_vector_Iterator__20_make(std_vector_Vector__20 *vec);
bool std_vector_Iterator__20_has_value(std_vector_Iterator__20 *this);
void std_vector_Vector__20_resize(std_vector_Vector__20 *this, u32 new_capacity);
std_vector_Vector__20 *std_vector_Vector__20_new(u32 capacity);
void std_vector_Vector__20_push(std_vector_Vector__20 *this, compiler_ast_scopes_Reference value);
std_vector_Iterator__21 std_vector_Vector__21_iter(std_vector_Vector__21 *this);
std_span_Span std_vector_Iterator__21_cur(std_vector_Iterator__21 *this);
void std_vector_Iterator__21_next(std_vector_Iterator__21 *this);
std_vector_Iterator__21 std_vector_Iterator__21_make(std_vector_Vector__21 *vec);
bool std_vector_Iterator__21_has_value(std_vector_Iterator__21 *this);
void std_vector_Vector__21_free(std_vector_Vector__21 *this);
void std_vector_Vector__21_resize(std_vector_Vector__21 *this, u32 new_capacity);
std_vector_Vector__21 *std_vector_Vector__21_new(u32 capacity);
void std_vector_Vector__21_push(std_vector_Vector__21 *this, std_span_Span value);
std_value_Value *std_vector_Vector__22_at(std_vector_Vector__22 *this, u32 i);
void std_vector_Vector__22_resize(std_vector_Vector__22 *this, u32 new_capacity);
std_vector_Vector__22 *std_vector_Vector__22_new(u32 capacity);
void std_vector_Vector__22_push(std_vector_Vector__22 *this, std_value_Value *value);
std_vector_Iterator__23 std_vector_Vector__23_iter(std_vector_Vector__23 *this);
std_compact_map_Item__0 std_vector_Iterator__23_cur(std_vector_Iterator__23 *this);
void std_vector_Iterator__23_next(std_vector_Iterator__23 *this);
std_vector_Iterator__23 std_vector_Iterator__23_make(std_vector_Vector__23 *vec);
bool std_vector_Iterator__23_has_value(std_vector_Iterator__23 *this);
std_compact_map_Item__0 std_vector_Vector__23_at(std_vector_Vector__23 *this, u32 i);
void std_vector_Vector__23_resize(std_vector_Vector__23 *this, u32 new_capacity);
std_vector_Vector__23 *std_vector_Vector__23_new(u32 capacity);
void std_vector_Vector__23_push(std_vector_Vector__23 *this, std_compact_map_Item__0 value);
u32 std_vector_Vector__24_at(std_vector_Vector__24 *this, u32 i);
void std_vector_Vector__24_free(std_vector_Vector__24 *this);
void std_vector_Vector__24_resize(std_vector_Vector__24 *this, u32 new_capacity);
std_vector_Vector__24 *std_vector_Vector__24_new(u32 capacity);
void std_vector_Vector__24_push(std_vector_Vector__24 *this, u32 value);
void std_json_serialize_into(std_value_Value *val, std_buffer_Buffer *sb);
std_buffer_Buffer std_json_serialize(std_value_Value *val);
void std_json_write_to_file(std_value_Value *val, char *filename);
bool compiler_lsp_utils_verbose = false;
char *exec_path = "./out";
char *c_path = NULL;
char *filename = NULL;
bool compile_c = true;
bool silent = false;
bool debug = false;
u32 error_level = 2;
char *docs_path = NULL;
bool include_stdlib = true;
std_vector_Vector__1 *extra_c_flags;
bool run_after_compile = false;
void *std_mem_state_allocator = NULL;
void *(*std_mem_state_alloc_fn)(void *, u32) = std_mem_impl_my_calloc;
void *(*std_mem_state_realloc_fn)(void *, void *, u32, u32) = std_mem_impl_my_realloc;
void (*std_mem_state_free_fn)(void *, void *) = std_mem_impl_my_free;
/* function implementations */
std_value_Value *compiler_docgen_DocGenerator_gen_enum(compiler_docgen_DocGenerator *this, compiler_ast_nodes_Enum *enum_) {
  std_value_Value *enum_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(enum_doc, "id", format_string("%x", enum_->type));
  std_value_Value_insert_str(enum_doc, "name", enum_->sym->name);
  if (((bool)enum_->sym->comment)) {
    std_value_Value_insert_str(enum_doc, "description", enum_->sym->comment);
  }
  std_value_Value_insert_str(enum_doc, "kind", "enum");
  if (enum_->sym->is_extern) {
    std_value_Value_insert_str(enum_doc, "extern", compiler_ast_scopes_Symbol_out_name(enum_->sym));
  } else {
    compiler_docgen_DocGenerator_gen_location(this, enum_doc, enum_->sym->span);
  }
  std_value_Value *fields_doc = std_value_Value_new(std_value_ValueType_List);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      std_value_Value *field_doc = std_value_Value_new(std_value_ValueType_Dictionary);
      std_value_Value_insert_str(field_doc, "name", field->sym->name);
      if (((bool)field->sym->comment)) {
        std_value_Value_insert_str(field_doc, "description", field->sym->comment);
      }
      if (field->sym->is_extern) {
        std_value_Value_insert_str(field_doc, "extern", compiler_ast_scopes_Symbol_out_name(field->sym));
      }
      std_value_Value_push(fields_doc, field_doc);
    }
  }
  std_value_Value_insert(enum_doc, "fields", fields_doc);
  std_value_Value *methods_doc = compiler_docgen_DocGenerator_gen_methods(this, enum_->type);
  std_value_Value_insert(enum_doc, "methods", methods_doc);
  return enum_doc;
}

void compiler_docgen_DocGenerator_gen_location(compiler_docgen_DocGenerator *this, std_value_Value *obj, std_span_Span span) {
  std_span_Location start = span.start;
  if (std_span_Location_is_valid(&start)) {
    char *src_str = format_string("%s#L%u", start.filename, start.line);
    std_value_Value_insert_str(obj, "source", src_str);
  }
}

char *compiler_docgen_DocGenerator_gen_templated_type(compiler_docgen_DocGenerator *this, compiler_types_Type *base, std_vector_Vector__0 *args) {
  char *base_name = compiler_docgen_DocGenerator_gen_typename_str(this, base);
  std_buffer_Buffer buf = std_buffer_Buffer_make(16);
  std_buffer_Buffer_write_str(&buf, base_name);
  std_buffer_Buffer_write_str(&buf, "<");
  bool first = true;
  for (std_vector_Iterator__0 __iter = std_vector_Vector__0_iter(args); std_vector_Iterator__0_has_value(&__iter); std_vector_Iterator__0_next(&__iter)) {
    compiler_types_Type *arg_type = std_vector_Iterator__0_cur(&__iter);
    {
      if (!first) {
      std_buffer_Buffer_write_str(&buf, ", ");
      }
      first=false;
      char *param_name = compiler_docgen_DocGenerator_gen_typename_str(this, arg_type);
      std_buffer_Buffer_write_str(&buf, param_name);
    }
  }
  std_buffer_Buffer_write_str(&buf, ">");
  return std_buffer_Buffer_str(buf);
}

char *compiler_docgen_DocGenerator_gen_typename_str(compiler_docgen_DocGenerator *this, compiler_types_Type *type) {
  ae_assert(((bool)type), "/Users/mustafa/ocen-lang/ocen/compiler/docgen.oc:78:12: Assertion failed: `type?`", "gen_typename_str called with null type");
  switch (type->base) {
    case compiler_types_BaseType_Char:
    case compiler_types_BaseType_Bool:
    case compiler_types_BaseType_Void:
    case compiler_types_BaseType_I8:
    case compiler_types_BaseType_I16:
    case compiler_types_BaseType_I32:
    case compiler_types_BaseType_I64:
    case compiler_types_BaseType_U8:
    case compiler_types_BaseType_U16:
    case compiler_types_BaseType_U32:
    case compiler_types_BaseType_U64:
    case compiler_types_BaseType_F32:
    case compiler_types_BaseType_F64:
    case compiler_types_BaseType_Alias:
    case compiler_types_BaseType_Enum: {
      return format_string("{{%x}}", type);
    } break;
    case compiler_types_BaseType_Structure: {
      if (!((bool)type->template_instance)) {
        return format_string("{{%x}}", type);
      } else {
        compiler_ast_nodes_Structure *struc = type->u.struc;
        compiler_ast_scopes_TemplateInstance *instance = type->template_instance;
        compiler_ast_scopes_Symbol *parent = instance->parent;
        ae_assert(parent->type==compiler_ast_scopes_SymbolType_Structure, "/Users/mustafa/ocen-lang/ocen/compiler/docgen.oc:93:24: Assertion failed: `parent.type == Structure`", "Template instance parent is not a structure");
        compiler_ast_nodes_Structure *parent_struc = parent->u.struc;
        return compiler_docgen_DocGenerator_gen_templated_type(this, parent_struc->type, instance->args);
      }
    } break;
    case compiler_types_BaseType_UnresolvedTemplate: {
      compiler_types_UnresolvedTemplate uspec = type->u.unresolved_spec;
      return compiler_docgen_DocGenerator_gen_templated_type(this, uspec.base, uspec.args);
    } break;
    case compiler_types_BaseType_Pointer: {
      char *sub = compiler_docgen_DocGenerator_gen_typename_str(this, type->u.ptr);
      str_replace_with(&sub, format_string("&%s", sub));
      return sub;
    } break;
    case compiler_types_BaseType_Array: {
      char *sub = compiler_docgen_DocGenerator_gen_typename_str(this, type->u.arr.elem_type);
      str_replace_with(&sub, format_string("[%s]", sub));
      return sub;
    } break;
    case compiler_types_BaseType_Function: {
      std_buffer_Buffer buf = std_buffer_Buffer_make(16);
      std_buffer_Buffer_write_str(&buf, "fn(");
      bool first = true;
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(type->u.func.params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        compiler_ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
        {
          if (!first) {
          std_buffer_Buffer_write_str(&buf, ", ");
          }
          first=false;
          char *param_name = compiler_docgen_DocGenerator_gen_typename_str(this, param->type);
          std_buffer_Buffer_write_str_f(&buf, param_name);
        }
      }
      std_buffer_Buffer_write_str(&buf, "): ");
      char *return_name = compiler_docgen_DocGenerator_gen_typename_str(this, type->u.func.return_type);
      std_buffer_Buffer_write_str_f(&buf, return_name);
      return std_buffer_Buffer_str(buf);
    } break;
    case compiler_types_BaseType_Unresolved: {
      compiler_ast_nodes_AST *node = type->u.unresolved;
      switch (node->type) {
        case compiler_ast_nodes_ASTType_Identifier: {
          return strdup(node->u.ident.name);
        } break;
        default: {
          compiler_ast_program_Program_error(this->program, compiler_errors_Error_new(node->span, format_string("Unhandled node in gen_typename_str: %s", compiler_ast_nodes_ASTType_dbg(node->type))));
          return "<unknown>";
        } break;
      }
    } break;
    default: {
      compiler_ast_program_Program_error(this->program, compiler_errors_Error_new(type->span, format_string("Unhandled type in gen_typename_str: %s", compiler_types_BaseType_dbg(type->base))));
      return "<unknown>";
    } break;
  }
}

std_value_Value *compiler_docgen_DocGenerator_gen_typename(compiler_docgen_DocGenerator *this, compiler_types_Type *type) {
  ae_assert(((bool)type), "/Users/mustafa/ocen-lang/ocen/compiler/docgen.oc:153:12: Assertion failed: `type?`", "gen_typename called with null type");
  return std_value_Value_new_str(compiler_docgen_DocGenerator_gen_typename_str(this, type));
}

std_value_Value *compiler_docgen_DocGenerator_gen_methods(compiler_docgen_DocGenerator *this, compiler_types_Type *type) {
  ae_assert(compiler_types_Type_can_have_methods(type), "/Users/mustafa/ocen-lang/ocen/compiler/docgen.oc:158:12: Assertion failed: `type.can_have_methods()`", "gen_methods called with type that can't have methods");
  std_value_Value *methods_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  for (std_map_Iterator__7 __iter = std_map_Map__7_iter(type->methods); std_map_Iterator__7_has_value(&__iter); std_map_Iterator__7_next(&__iter)) {
    std_map_Item__7 *it = std_map_Iterator__7_cur(&__iter);
    {
      compiler_ast_nodes_Function *method = it->value;
      std_value_Value_insert(methods_doc, method->sym->name, compiler_docgen_DocGenerator_gen_function(this, method));
    }
  }
  return methods_doc;
}

std_value_Value *compiler_docgen_DocGenerator_gen_function(compiler_docgen_DocGenerator *this, compiler_ast_nodes_Function *func) {
  std_value_Value *func_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(func_doc, "id", format_string("%x", func));
  std_value_Value_insert_str(func_doc, "name", format_string("%s", func->sym->name));
  if (((bool)func->sym->comment)) {
    std_value_Value_insert_str(func_doc, "description", func->sym->comment);
  }
  if (func->is_method) {
    std_value_Value_insert_str(func_doc, "kind", "method");
    std_value_Value_insert(func_doc, "parent", compiler_docgen_DocGenerator_gen_typename(this, func->parent_type));
  } else {
    std_value_Value_insert_str(func_doc, "kind", "function");
  }
  if (func->sym->is_extern) {
    std_value_Value_insert_str(func_doc, "extern", compiler_ast_scopes_Symbol_out_name(func->sym));
  } else {
    compiler_docgen_DocGenerator_gen_location(this, func_doc, func->span);
  }
  std_value_Value *return_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  compiler_types_Type *ret_type = func->return_type;
  std_value_Value_insert(return_doc, "type", compiler_docgen_DocGenerator_gen_typename(this, ret_type));
  std_value_Value *params_doc = std_value_Value_new(std_value_ValueType_List);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(func->params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
    {
      std_value_Value *param_doc = std_value_Value_new(std_value_ValueType_Dictionary);
      std_value_Value_insert_str(param_doc, "name", format_string("%s", param->sym->name));
      if (((bool)param->sym->comment)) {
        std_value_Value_insert_str(param_doc, "description", param->sym->comment);
      }
      std_value_Value_insert(param_doc, "type", compiler_docgen_DocGenerator_gen_typename(this, param->type));
      compiler_ast_nodes_AST *default_value = param->default_value;
      if (((bool)default_value)) {
        std_value_Value *value_str = std_value_Value_new_str(compiler_ast_program_Program_get_source_text(this->program, default_value->span));
        std_value_Value_insert(param_doc, "default_value", value_str);
      }
      std_value_Value_push(params_doc, param_doc);
    }
  }
  std_value_Value_insert(func_doc, "params", params_doc);
  std_value_Value_insert(func_doc, "return", return_doc);
  return func_doc;
}

std_value_Value *compiler_docgen_DocGenerator_gen_struct(compiler_docgen_DocGenerator *this, compiler_ast_nodes_Structure *struc) {
  std_value_Value *struc_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(struc_doc, "id", format_string("%x", struc->type));
  std_value_Value_insert_str(struc_doc, "name", format_string("%s", struc->sym->name));
  if (((bool)struc->sym->comment)) {
    std_value_Value_insert_str(struc_doc, "description", struc->sym->comment);
  }
  if (struc->is_union) {
    std_value_Value_insert_str(struc_doc, "kind", "union");
  } else {
    std_value_Value_insert_str(struc_doc, "kind", "struct");
  }
  std_value_Value_insert(struc_doc, "is_templated", std_value_Value_new_bool(compiler_ast_scopes_Symbol_is_templated(struc->sym)));
  if (compiler_ast_scopes_Symbol_is_templated(struc->sym)) {
    std_value_Value *params_doc = std_value_Value_new(std_value_ValueType_List);
    for (std_vector_Iterator__19 __iter = std_vector_Vector__19_iter(struc->sym->template->params); std_vector_Iterator__19_has_value(&__iter); std_vector_Iterator__19_next(&__iter)) {
      compiler_ast_scopes_Symbol *sym = std_vector_Iterator__19_cur(&__iter);
      {
        std_value_Value_push(params_doc, std_value_Value_new_str(sym->name));
      }
    }
    std_value_Value_insert(struc_doc, "template_params", params_doc);
  }
  if (struc->sym->is_extern) {
    std_value_Value_insert_str(struc_doc, "extern", compiler_ast_scopes_Symbol_out_name(struc->sym));
  } else {
    compiler_docgen_DocGenerator_gen_location(this, struc_doc, struc->span);
  }
  std_value_Value *fields_doc = std_value_Value_new(std_value_ValueType_List);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      std_value_Value *field_doc = std_value_Value_new(std_value_ValueType_Dictionary);
      std_value_Value_insert_str(field_doc, "name", field->sym->name);
      std_value_Value_insert(field_doc, "type", compiler_docgen_DocGenerator_gen_typename(this, field->type));
      if (((bool)field->sym->comment)) {
        std_value_Value_insert_str(field_doc, "description", field->sym->comment);
      }
      if (field->sym->is_extern) {
        std_value_Value_insert_str(field_doc, "extern", compiler_ast_scopes_Symbol_out_name(field->sym));
      }
      std_value_Value_push(fields_doc, field_doc);
    }
  }
  std_value_Value_insert(struc_doc, "fields", fields_doc);
  std_value_Value *methods_doc = compiler_docgen_DocGenerator_gen_methods(this, struc->type);
  std_value_Value_insert(struc_doc, "methods", methods_doc);
  return struc_doc;
}

std_value_Value *compiler_docgen_DocGenerator_gen_ns(compiler_docgen_DocGenerator *this, compiler_ast_program_Namespace *ns) {
  std_value_Value *ns_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(ns_doc, "id", format_string("%x", ns));
  if (((bool)ns->sym->comment)) {
    std_value_Value_insert_str(ns_doc, "description", ns->sym->comment);
  }
  std_value_Value_insert_str(ns_doc, "name", ns->sym->name);
  std_value_Value_insert_str(ns_doc, "kind", "namespace");
  if (ns->is_a_file || ns->is_top_level) {
    compiler_docgen_DocGenerator_gen_location(this, ns_doc, ns->span);
  }
  if (!std_vector_Vector__12_is_empty(ns->enums)) {
    std_value_Value *enums_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
      compiler_ast_nodes_Enum *enum_ = std_vector_Iterator__12_cur(&__iter);
      {
        std_value_Value_insert(enums_doc, enum_->sym->name, compiler_docgen_DocGenerator_gen_enum(this, enum_));
      }
    }
    std_value_Value_insert(ns_doc, "enums", enums_doc);
  }
  if (!std_vector_Vector__2_is_empty(ns->structs)) {
    std_value_Value *structs_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    std_value_Value *unions_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
      compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
      {
        std_value_Value *struct_doc = compiler_docgen_DocGenerator_gen_struct(this, struc);
        if (!struc->is_union) {
          std_value_Value_insert(structs_doc, struc->sym->name, struct_doc);
        } else {
          std_value_Value_insert(unions_doc, struc->sym->name, struct_doc);
        }
      }
    }
    if (!std_compact_map_Map__0_is_empty(std_value_Value_as_dict(structs_doc))) {
      std_value_Value_insert(ns_doc, "structs", structs_doc);
    }
    if (!std_compact_map_Map__0_is_empty(std_value_Value_as_dict(unions_doc))) {
      std_value_Value_insert(ns_doc, "unions", unions_doc);
    }
  }
  if (!std_vector_Vector__13_is_empty(ns->variables)) {
    std_value_Value *vars_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
      compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
      {
        compiler_ast_nodes_Variable *var = node->u.var_decl.var;
        std_value_Value *var_doc = std_value_Value_new(std_value_ValueType_Dictionary);
        std_value_Value_insert_str(var_doc, "id", format_string("%x", var));
        if (((bool)var->sym->comment)) {
          std_value_Value_insert_str(var_doc, "description", var->sym->comment);
        }
        std_value_Value_insert_str(var_doc, "name", var->sym->name);
        std_value_Value_insert_str(var_doc, "kind", "variable");
        std_value_Value_insert(var_doc, "type", compiler_docgen_DocGenerator_gen_typename(this, var->type));
        compiler_docgen_DocGenerator_gen_location(this, var_doc, var->sym->span);
        if (var->sym->is_extern) {
          std_value_Value_insert_str(var_doc, "extern", compiler_ast_scopes_Symbol_out_name(var->sym));
        }
        std_value_Value_insert(vars_doc, var->sym->name, var_doc);
      }
    }
    std_value_Value_insert(ns_doc, "variables", vars_doc);
  }
  if (!std_vector_Vector__13_is_empty(ns->constants)) {
    std_value_Value *consts_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
      compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
      {
        compiler_ast_nodes_Variable *var = node->u.var_decl.var;
        std_value_Value *const_doc = std_value_Value_new(std_value_ValueType_Dictionary);
        std_value_Value_insert_str(const_doc, "id", format_string("%x", var));
        if (((bool)var->sym->comment)) {
          std_value_Value_insert_str(const_doc, "description", var->sym->comment);
        }
        std_value_Value_insert_str(const_doc, "name", var->sym->name);
        std_value_Value_insert_str(const_doc, "kind", "constant");
        std_value_Value_insert(const_doc, "type", compiler_docgen_DocGenerator_gen_typename(this, var->type));
        compiler_docgen_DocGenerator_gen_location(this, const_doc, var->sym->span);
        if (var->sym->is_extern) {
          std_value_Value_insert_str(const_doc, "extern", compiler_ast_scopes_Symbol_out_name(var->sym));
        }
        std_value_Value_insert(consts_doc, var->sym->name, const_doc);
      }
    }
    std_value_Value_insert(ns_doc, "constants", consts_doc);
  }
  if (!std_vector_Vector__6_is_empty(ns->functions)) {
    std_value_Value *funcs_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
      compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
      {
        if (func->is_method) {
          continue;
        }
        std_value_Value *func_doc = compiler_docgen_DocGenerator_gen_function(this, func);
        std_value_Value_insert(funcs_doc, func->sym->name, func_doc);
      }
    }
    std_value_Value_insert(ns_doc, "functions", funcs_doc);
  }
  if (!std_map_Map__2_is_empty(ns->typedefs)) {
    std_value_Value *typedefs_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_map_Iterator__2 __iter = std_map_Map__2_iter(ns->typedefs); std_map_Iterator__2_has_value(&__iter); std_map_Iterator__2_next(&__iter)) {
      std_map_Item__2 *it = std_map_Iterator__2_cur(&__iter);
      {
        std_value_Value *typedef_doc = std_value_Value_new(std_value_ValueType_Dictionary);
        std_value_Value_insert_str(typedef_doc, "kind", "typedef");
        std_value_Value_insert_str(typedef_doc, "name", strdup(it->key));
        std_value_Value_insert(typedef_doc, "type", compiler_docgen_DocGenerator_gen_typename(this, it->value));
        std_value_Value_insert(typedefs_doc, it->key, typedef_doc);
      }
    }
    std_value_Value_insert(ns_doc, "typedefs", typedefs_doc);
  }
  if (!std_map_Map__3_is_empty(ns->namespaces)) {
    std_value_Value *namespaces_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_map_Iterator__3 __iter = std_map_Map__3_iter(ns->namespaces); std_map_Iterator__3_has_value(&__iter); std_map_Iterator__3_next(&__iter)) {
      std_map_Item__3 *it = std_map_Iterator__3_cur(&__iter);
      {
        std_value_Value *ns_doc = compiler_docgen_DocGenerator_gen_ns(this, it->value);
        std_value_Value_insert(namespaces_doc, it->key, ns_doc);
      }
    }
    std_value_Value_insert(ns_doc, "namespaces", namespaces_doc);
  }
  return ns_doc;
}

std_value_Value *compiler_docgen_DocGenerator_gen_builtin(compiler_docgen_DocGenerator *this, compiler_types_Type *type) {
  std_value_Value *type_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(type_doc, "id", format_string("%x", type));
  std_value_Value_insert_str(type_doc, "name", format_string("%s", type->sym->name));
  std_value_Value_insert_str(type_doc, "description", format_string("Built-in type %s", type->sym->name));
  std_value_Value_insert_str(type_doc, "kind", "builtin");
  std_value_Value *methods_doc = compiler_docgen_DocGenerator_gen_methods(this, type);
  std_value_Value_insert(type_doc, "methods", methods_doc);
  return type_doc;
}

std_value_Value *compiler_docgen_DocGenerator_gen_builtins(compiler_docgen_DocGenerator *this, compiler_ast_program_Program *program) {
  std_value_Value *builtins_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  for (u32 i = 0; i < ((u32)compiler_types_BaseType_NUM_BASE_TYPES); i+=1) {
    compiler_types_Type *type = compiler_ast_program_Program_get_base_type(program, ((compiler_types_BaseType)i), std_span_Span_default());
    std_value_Value_insert(builtins_doc, type->name, compiler_docgen_DocGenerator_gen_builtin(this, type));
  }
  compiler_types_Type *str_type = compiler_ast_program_Program_get_type_by_name(program, "str", std_span_Span_default());
  std_value_Value_insert(builtins_doc, "str", compiler_docgen_DocGenerator_gen_builtin(this, str_type));
  compiler_types_Type *untyped_ptr_type = compiler_ast_program_Program_get_type_by_name(program, "untyped_ptr", std_span_Span_default());
  std_value_Value_insert(builtins_doc, "untyped_ptr", compiler_docgen_DocGenerator_gen_builtin(this, untyped_ptr_type));
  return builtins_doc;
}

void compiler_docgen_generate_doc_json(compiler_ast_program_Program *program, char *json_path) {
  compiler_docgen_DocGenerator docs_generator = (compiler_docgen_DocGenerator){.program=program};
  std_value_Value *docs = compiler_docgen_DocGenerator_gen_ns(&docs_generator, program->global);
  std_value_Value *builtins = compiler_docgen_DocGenerator_gen_builtins(&docs_generator, program);
  std_value_Value_insert(docs, "builtins", builtins);
  std_value_Value *container = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(container, "ocen", docs);
  std_json_write_to_file(container, json_path);
}

void compiler_passes_run_typecheck_passes(compiler_ast_program_Program *program) {
  compiler_passes_register_types_RegisterTypes_run(program);
  compiler_passes_typechecker_TypeChecker_run(program);
}

char *compiler_passes_run_codegen_passes(compiler_ast_program_Program *program) {
  compiler_passes_reorder_structs_ReorderStructs_run(program);
  compiler_passes_mark_dead_code_MarkDeadCode_run(program);
  char *code = compiler_passes_code_generator_CodeGenerator_run(program);
  return code;
}

void compiler_passes_code_generator_CodeGenerator_gen_indent(compiler_passes_code_generator_CodeGenerator *this) {
  for (u32 i = 0; i < this->indent; i+=1) {
    std_buffer_Buffer_write_str(&this->out, "  ");
  }
}

void str_replace(char **this, char *other) {
  char *s = (*this);
  std_mem_free(s);
  (*this)=other;
}

compiler_errors_Error *compiler_passes_code_generator_CodeGenerator_error(compiler_passes_code_generator_CodeGenerator *this, compiler_errors_Error *err) {
  return compiler_passes_generic_pass_GenericPass_error(this->o, err);
}

compiler_ast_scopes_Scope *compiler_passes_code_generator_CodeGenerator_scope(compiler_passes_code_generator_CodeGenerator *this) {
  return compiler_passes_generic_pass_GenericPass_scope(this->o);
}

void compiler_passes_code_generator_CodeGenerator_gen_debug_info(compiler_passes_code_generator_CodeGenerator *this, std_span_Span span, bool force) {
  if (!this->o->program->gen_debug_info && !force) {
  return;
  }
  std_span_Location loc = span.start;
  std_buffer_Buffer_write_str_f(&this->out, format_string("\n#line %u \"%s\"\n", loc.line, loc.filename));
}

char *compiler_passes_code_generator_CodeGenerator_get_op(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  return ({ char *__yield_0;
    switch (node->type) {
      case compiler_ast_nodes_ASTType_BinaryOp: {
        __yield_0 = ({ char *__yield_1;
          switch (node->u.binary.op) {
            case compiler_ast_operators_Operator_And: {
              __yield_1 = "&&";
            } break;
            case compiler_ast_operators_Operator_Assignment:
            case compiler_ast_operators_Operator_IndexAssign: {
              __yield_1 = "=";
            } break;
            case compiler_ast_operators_Operator_BitwiseAnd: {
              __yield_1 = "&";
            } break;
            case compiler_ast_operators_Operator_BitwiseOr: {
              __yield_1 = "|";
            } break;
            case compiler_ast_operators_Operator_BitwiseXor: {
              __yield_1 = "^";
            } break;
            case compiler_ast_operators_Operator_Divide: {
              __yield_1 = "/";
            } break;
            case compiler_ast_operators_Operator_Equals: {
              __yield_1 = "==";
            } break;
            case compiler_ast_operators_Operator_GreaterThan: {
              __yield_1 = ">";
            } break;
            case compiler_ast_operators_Operator_GreaterThanEquals: {
              __yield_1 = ">=";
            } break;
            case compiler_ast_operators_Operator_LeftShift: {
              __yield_1 = "<<";
            } break;
            case compiler_ast_operators_Operator_LessThan: {
              __yield_1 = "<";
            } break;
            case compiler_ast_operators_Operator_LessThanEquals: {
              __yield_1 = "<=";
            } break;
            case compiler_ast_operators_Operator_Minus: {
              __yield_1 = "-";
            } break;
            case compiler_ast_operators_Operator_Modulus: {
              __yield_1 = "%";
            } break;
            case compiler_ast_operators_Operator_Multiply: {
              __yield_1 = "*";
            } break;
            case compiler_ast_operators_Operator_NotEquals: {
              __yield_1 = "!=";
            } break;
            case compiler_ast_operators_Operator_Or: {
              __yield_1 = "||";
            } break;
            case compiler_ast_operators_Operator_Plus: {
              __yield_1 = "+";
            } break;
            case compiler_ast_operators_Operator_PlusEquals: {
              __yield_1 = "+=";
            } break;
            case compiler_ast_operators_Operator_MinusEquals: {
              __yield_1 = "-=";
            } break;
            case compiler_ast_operators_Operator_MultiplyEquals: {
              __yield_1 = "*=";
            } break;
            case compiler_ast_operators_Operator_DivideEquals: {
              __yield_1 = "/=";
            } break;
            case compiler_ast_operators_Operator_RightShift: {
              __yield_1 = ">>";
            } break;
            case compiler_ast_operators_Operator_LeftShiftEquals: {
              __yield_1 = "<<=";
            } break;
            case compiler_ast_operators_Operator_RightShiftEquals: {
              __yield_1 = ">>=";
            } break;
            default: std_panic(format_string("Unknown binary op type in get_op: %s", compiler_ast_operators_Operator_dbg(node->u.binary.op))); break;
          }
;__yield_1; });
      } break;
      case compiler_ast_nodes_ASTType_UnaryOp: {
        __yield_0 = ({ char *__yield_1;
          switch (node->u.unary.op) {
            case compiler_ast_operators_Operator_PreDecrement: {
              __yield_1 = "--";
            } break;
            case compiler_ast_operators_Operator_PreIncrement: {
              __yield_1 = "++";
            } break;
            case compiler_ast_operators_Operator_PostDecrement: {
              __yield_1 = "--";
            } break;
            case compiler_ast_operators_Operator_PostIncrement: {
              __yield_1 = "++";
            } break;
            default: std_panic(format_string("Unknown unary op type in get_op: %s", compiler_ast_operators_Operator_dbg(node->u.unary.op))); break;
          }
;__yield_1; });
      } break;
      default: std_panic(format_string("Unknown op type in get_op: %s", compiler_ast_nodes_ASTType_dbg(node->type))); break;
    }
;__yield_0; });
}

void compiler_passes_code_generator_CodeGenerator_gen_internal_print(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_AST *callee = node->u.call.callee;
  bool newline_after = str_eq(callee->u.ident.name, "println");
  std_buffer_Buffer_write_str(&this->out, "printf(");
  std_vector_Vector__15 *args = node->u.call.args;
  compiler_ast_nodes_Argument *first = std_vector_Vector__15_at(args, 0);
  if (first->expr->type==compiler_ast_nodes_ASTType_FormatStringLiteral) {
    compiler_passes_code_generator_CodeGenerator_gen_format_string_variadic(this, first->expr, newline_after);
    std_buffer_Buffer_write_str(&this->out, ")");
    return;
  }
  for (u32 i = 0; i < args->size; i+=1) {
    if (i > 0) {
    std_buffer_Buffer_write_str(&this->out, ", ");
    }
    compiler_ast_nodes_Argument *arg = std_vector_Vector__15_at(args, i);
    compiler_passes_code_generator_CodeGenerator_gen_expression(this, arg->expr, false);
    if (i==0 && newline_after) {
    std_buffer_Buffer_write_str(&this->out, "\"\\n\"");
    }
  }
  std_buffer_Buffer_write_str(&this->out, ")");
}

void compiler_passes_code_generator_CodeGenerator_gen_format_string_part(compiler_passes_code_generator_CodeGenerator *this, char *part) {
  u32 len = strlen(part);
  for (u32 i = 0; i < len; i+=1) {
    if (part[i]=='\\') {
      i+=1;
      switch (part[i]) {
        case '`':
        case '{':
        case '}': {
        } break;
        default: {
          std_buffer_Buffer_write_char(&this->out, '\\');
        } break;
      }
    } else if (part[i]=='"') {
      std_buffer_Buffer_write_char(&this->out, '\\');
    } else if (part[i]=='%') {
      std_buffer_Buffer_write_char(&this->out, '%');
    }
    std_buffer_Buffer_write_char(&this->out, part[i]);
  }
}

void compiler_passes_code_generator_CodeGenerator_format_string_custom_specifier(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type, compiler_ast_nodes_AST *expr) {
  if (type->base==compiler_types_BaseType_Structure) {
    compiler_ast_nodes_Structure *struc = type->u.struc;
    if (((bool)struc) && ((bool)struc->format_spec)) {
      std_buffer_Buffer_write_str(&this->out, struc->format_spec);
      return;
    }
  }
  compiler_passes_code_generator_CodeGenerator_error(this, compiler_errors_Error_new(expr->span, format_string("Invalid type in CodeGenerator::format_string_custom_specifier: '%s'", compiler_types_Type_str(type))));
  std_buffer_Buffer_write_str(&this->out, "%s");
}

void compiler_passes_code_generator_CodeGenerator_format_string_custom_argument(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type, compiler_ast_nodes_AST *expr) {
  if (type->base==compiler_types_BaseType_Structure) {
    compiler_ast_nodes_Structure *struc = type->u.struc;
    if (((bool)struc) && ((bool)struc->format_args)) {
      for (std_CharIterator __iter = str_chars(struc->format_args, 0); std_CharIterator_has_value(&__iter); std_CharIterator_next(&__iter)) {
        char c = std_CharIterator_cur(&__iter);
        {
          if (c=='$') {
            std_buffer_Buffer_write_str(&this->out, "(");
            compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
            std_buffer_Buffer_write_str(&this->out, ")");
          } else {
            std_buffer_Buffer_write_char(&this->out, c);
          }
        }
      }
      return;
    }
  }
  compiler_passes_code_generator_CodeGenerator_error(this, compiler_errors_Error_new(expr->span, format_string("Invalid type in CodeGenerator::format_string_custom_argument: '%s'", compiler_types_Type_str(type))));
  compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
}

void compiler_passes_code_generator_CodeGenerator_gen_format_string_variadic(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, bool newline_after) {
  std_vector_Vector__1 *parts = node->u.fmt_str.parts;
  std_vector_Vector__13 *exprs = node->u.fmt_str.exprs;
  std_vector_Vector__1 *specs = node->u.fmt_str.specs;
  std_buffer_Buffer_write_char(&this->out, '"');
  for (u32 i = 0; i < exprs->size; i+=1) {
    char *part = std_vector_Vector__1_at(parts, i);
    compiler_ast_nodes_AST *expr = std_vector_Vector__13_at(exprs, i);
    compiler_passes_code_generator_CodeGenerator_gen_format_string_part(this, part);
    char *spec = std_vector_Vector__1_at(specs, i);
    if (((bool)spec)) {
      std_buffer_Buffer_write_str(&this->out, "%");
      std_buffer_Buffer_write_str(&this->out, spec);
      continue;
    }
    compiler_types_Type *expr_type = compiler_types_Type_unaliased(expr->etype);
    switch (expr_type->base) {
      case compiler_types_BaseType_I8:
      case compiler_types_BaseType_I16:
      case compiler_types_BaseType_I32: {
        std_buffer_Buffer_write_str(&this->out, "%d");
      } break;
      case compiler_types_BaseType_U8:
      case compiler_types_BaseType_U16:
      case compiler_types_BaseType_U32: {
        std_buffer_Buffer_write_str(&this->out, "%u");
      } break;
      case compiler_types_BaseType_I64: {
        std_buffer_Buffer_write_str(&this->out, "%\" PRId64 \"");
      } break;
      case compiler_types_BaseType_U64: {
        std_buffer_Buffer_write_str(&this->out, "%\" PRIu64 \"");
      } break;
      case compiler_types_BaseType_Bool: {
        std_buffer_Buffer_write_str(&this->out, "%s");
      } break;
      case compiler_types_BaseType_F32:
      case compiler_types_BaseType_F64: {
        std_buffer_Buffer_write_str(&this->out, "%f");
      } break;
      case compiler_types_BaseType_Char: {
        std_buffer_Buffer_write_str(&this->out, "%c");
      } break;
      case compiler_types_BaseType_Pointer: {
        switch (expr_type->u.ptr->base) {
          case compiler_types_BaseType_Char: {
            std_buffer_Buffer_write_str(&this->out, "%s");
          } break;
          default: {
            std_buffer_Buffer_write_str(&this->out, "%p");
          } break;
        }
      } break;
      default: {
        compiler_passes_code_generator_CodeGenerator_format_string_custom_specifier(this, expr_type, expr);
      } break;
    }
  }
  char *part = std_vector_Vector__1_back(parts, 0);
  compiler_passes_code_generator_CodeGenerator_gen_format_string_part(this, part);
  if (newline_after) {
  std_buffer_Buffer_write_str(&this->out, "\\n");
  }
  std_buffer_Buffer_write_char(&this->out, '"');
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(exprs); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *expr = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_types_Type *expr_type = compiler_types_Type_unaliased(expr->etype);
      std_buffer_Buffer_write_str(&this->out, ", ");
      switch (expr_type->base) {
        case compiler_types_BaseType_I8:
        case compiler_types_BaseType_I16:
        case compiler_types_BaseType_I32:
        case compiler_types_BaseType_U8:
        case compiler_types_BaseType_U16:
        case compiler_types_BaseType_U32:
        case compiler_types_BaseType_I64:
        case compiler_types_BaseType_U64:
        case compiler_types_BaseType_F32:
        case compiler_types_BaseType_F64:
        case compiler_types_BaseType_Char:
        case compiler_types_BaseType_Pointer: {
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
        } break;
        case compiler_types_BaseType_Bool: {
          std_buffer_Buffer_write_str(&this->out, "((");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
          std_buffer_Buffer_write_str(&this->out, ") ? \"true\" : \"false\")");
        } break;
        default: {
          compiler_passes_code_generator_CodeGenerator_format_string_custom_argument(this, expr_type, expr);
        } break;
      }
    }
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_format_string(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  std_buffer_Buffer_write_str(&this->out, "format_string(");
  compiler_passes_code_generator_CodeGenerator_gen_format_string_variadic(this, node, false);
  std_buffer_Buffer_write_str(&this->out, ")");
}

void compiler_passes_code_generator_CodeGenerator_gen_yield_expression(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *expr) {
  char *yield_var = std_vector_Vector__1_back(this->yield_vars, 0);
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, yield_var);
  std_buffer_Buffer_write_str(&this->out, " = ");
  compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
  std_buffer_Buffer_write_str(&this->out, ";\n");
}

void compiler_passes_code_generator_CodeGenerator_gen_constant(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_Variable *const_ = node->u.var_decl.var;
  if (!const_->sym->is_extern) {
    compiler_passes_code_generator_CodeGenerator_gen_indent(this);
    std_buffer_Buffer_write_str(&this->out, "#define ");
    std_buffer_Buffer_write_str(&this->out, compiler_ast_scopes_Symbol_out_name(const_->sym));
    std_buffer_Buffer_write_str(&this->out, " (");
    compiler_passes_code_generator_CodeGenerator_gen_expression(this, node->u.var_decl.init, false);
    std_buffer_Buffer_write_str(&this->out, ")\n");
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_constants(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns) {
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *const_ = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_constant(this, const_);
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_constants(this, child);
    }
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_global_variables(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns) {
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_ast_nodes_Variable *var = node->u.var_decl.var;
      if (!var->sym->is_extern) {
        compiler_passes_code_generator_CodeGenerator_gen_var_declaration(this, node);
        std_buffer_Buffer_write_str(&this->out, ";\n");
      }
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_global_variables(this, child);
    }
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_control_body(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_AST *body) {
  if (body->type==compiler_ast_nodes_ASTType_Block) {
    compiler_passes_code_generator_CodeGenerator_gen_block(this, body, true);
  } else {
    bool is_elif = body->type==compiler_ast_nodes_ASTType_If;
    if (!is_elif) {
    std_buffer_Buffer_write_str(&this->out, "{\n");
    }
    if (((bool)node->etype) && (body->type != compiler_ast_nodes_ASTType_Yield)) {
      compiler_passes_code_generator_CodeGenerator_gen_yield_expression(this, body);
    } else {
      compiler_passes_code_generator_CodeGenerator_gen_statement(this, body);
    }
    compiler_passes_code_generator_CodeGenerator_gen_indent(this);
    if (!is_elif) {
    std_buffer_Buffer_write_str(&this->out, "}");
    }
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_in_yield_context(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  char *yield_var = format_string("__yield_%u", this->yield_vars->size);
  std_vector_Vector__1_push(this->yield_vars, yield_var);
  compiler_types_Type *ret_type = node->etype;
  std_buffer_Buffer_write_str(&this->out, "({ ");
  compiler_passes_code_generator_CodeGenerator_gen_type_and_name(this, ret_type, yield_var);
  std_buffer_Buffer_write_str(&this->out, ";\n");
  if (node->type==compiler_ast_nodes_ASTType_Block) {
    compiler_passes_code_generator_CodeGenerator_gen_block(this, node, false);
  } else {
    this->indent+=1;
    compiler_passes_code_generator_CodeGenerator_gen_statement(this, node);
    this->indent-=1;
  }
  std_buffer_Buffer_write_str(&this->out, ";");
  std_buffer_Buffer_write_str(&this->out, yield_var);
  std_buffer_Buffer_write_str(&this->out, "; })");
  std_vector_Vector__1_pop(this->yield_vars);
}

void compiler_passes_code_generator_CodeGenerator_gen_constructor(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_Structure *struc) {
  std_buffer_Buffer_write_str_f(&this->out, format_string("(%s){", compiler_ast_scopes_Symbol_out_name(struc->sym)));
  std_vector_Vector__4 *fields = struc->fields;
  std_vector_Vector__15 *args = node->u.call.args;
  for (u32 i = 0; i < args->size; i+=1) {
    if (i != 0) {
    std_buffer_Buffer_write_str(&this->out, ", ");
    }
    compiler_ast_nodes_Argument *arg = std_vector_Vector__15_at(args, i);
    compiler_ast_nodes_Variable *field = std_vector_Vector__4_at(fields, i);
    std_buffer_Buffer_write_str_f(&this->out, format_string(".%s=", compiler_ast_scopes_Symbol_out_name(field->sym)));
    compiler_passes_code_generator_CodeGenerator_gen_expression(this, arg->expr, false);
  }
  std_buffer_Buffer_write_str(&this->out, "}");
}

void compiler_passes_code_generator_CodeGenerator_gen_expression(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, bool is_top_level) {
  switch (node->type) {
    case compiler_ast_nodes_ASTType_IntLiteral: {
      compiler_ast_nodes_NumLiteral *num_lit = &node->u.num_literal;
      if ((node->etype->base != compiler_types_BaseType_I32) && (node->etype->base != compiler_types_BaseType_U32)) {
        std_buffer_Buffer_write_str(&this->out, "((");
        compiler_passes_code_generator_CodeGenerator_gen_type(this, node->etype);
        std_buffer_Buffer_write_str(&this->out, ")");
        std_buffer_Buffer_write_str(&this->out, num_lit->text);
        std_buffer_Buffer_write_str(&this->out, ")");
      } else {
        std_buffer_Buffer_write_str(&this->out, num_lit->text);
      }
    } break;
    case compiler_ast_nodes_ASTType_FloatLiteral: {
      compiler_ast_nodes_NumLiteral *num_lit = &node->u.num_literal;
      std_buffer_Buffer_write_str(&this->out, num_lit->text);
      if (node->etype->base==compiler_types_BaseType_F32) {
        std_buffer_Buffer_write_str(&this->out, "f");
      }
    } break;
    case compiler_ast_nodes_ASTType_ArrayLiteral: {
      std_vector_Vector__13 *elements = node->u.array_literal.elements;
      std_buffer_Buffer_write_str(&this->out, "{");
      for (u32 i = 0; i < elements->size; i+=1) {
        if (i != 0) {
        std_buffer_Buffer_write_str(&this->out, ", ");
        }
        compiler_ast_nodes_AST *expr = std_vector_Vector__13_at(elements, i);
        compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
      }
      std_buffer_Buffer_write_str(&this->out, "}");
    } break;
    case compiler_ast_nodes_ASTType_FormatStringLiteral: {
      compiler_passes_code_generator_CodeGenerator_gen_format_string(this, node);
    } break;
    case compiler_ast_nodes_ASTType_StringLiteral: {
      char *str_lit = node->u.string_literal;
      std_buffer_Buffer_write_str(&this->out, "\"");
      std_buffer_Buffer_write_str(&this->out, str_lit);
      std_buffer_Buffer_write_str(&this->out, "\"");
    } break;
    case compiler_ast_nodes_ASTType_CharLiteral: {
      char *char_lit = node->u.char_literal;
      std_buffer_Buffer_write_str(&this->out, "'");
      std_buffer_Buffer_write_str(&this->out, char_lit);
      std_buffer_Buffer_write_str(&this->out, "'");
    } break;
    case compiler_ast_nodes_ASTType_If: {
      std_vector_Vector__17 *branches = node->u.if_stmt.branches;
      compiler_ast_nodes_AST *els = node->u.if_stmt.els;
      if (((branches->size==1 && (std_vector_Vector__17_at(branches, 0).body->type != compiler_ast_nodes_ASTType_Block)) && ((bool)els)) && (els->type != compiler_ast_nodes_ASTType_Block)) {
        compiler_ast_nodes_IfBranch branch = std_vector_Vector__17_at(branches, 0);
        std_buffer_Buffer_write_str(&this->out, "(");
        compiler_passes_code_generator_CodeGenerator_gen_expression(this, branch.cond, false);
        std_buffer_Buffer_write_str(&this->out, " ? ");
        compiler_passes_code_generator_CodeGenerator_gen_expression(this, branch.body, false);
        std_buffer_Buffer_write_str(&this->out, " : ");
        compiler_passes_code_generator_CodeGenerator_gen_expression(this, els, false);
        std_buffer_Buffer_write_str(&this->out, ")");
      } else {
        compiler_passes_code_generator_CodeGenerator_gen_in_yield_context(this, node);
      }
    } break;
    case compiler_ast_nodes_ASTType_Match: {
      compiler_passes_code_generator_CodeGenerator_gen_in_yield_context(this, node);
    } break;
    case compiler_ast_nodes_ASTType_Block: {
      compiler_passes_code_generator_CodeGenerator_gen_in_yield_context(this, node);
    } break;
    case compiler_ast_nodes_ASTType_Member: {
      compiler_ast_scopes_Symbol *sym = node->resolved_symbol;
      if (((bool)sym) && sym->type==compiler_ast_scopes_SymbolType_Function) {
        std_buffer_Buffer_write_str(&this->out, compiler_ast_scopes_Symbol_out_name(sym));
        return;
      }
      compiler_ast_nodes_AST *lhs = node->u.member.lhs;
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, lhs, false);
      if (node->u.member.is_pointer) {
        std_buffer_Buffer_write_str(&this->out, "->");
      } else {
        std_buffer_Buffer_write_str(&this->out, ".");
      }
      std_buffer_Buffer_write_str(&this->out, node->u.member.rhs_name);
    } break;
    case compiler_ast_nodes_ASTType_Identifier:
    case compiler_ast_nodes_ASTType_NSLookup:
    case compiler_ast_nodes_ASTType_Specialization:
    case compiler_ast_nodes_ASTType_OverloadedOperator: {
      compiler_ast_scopes_Symbol *sym = node->resolved_symbol;
      if (!((bool)sym)) {
        compiler_passes_code_generator_CodeGenerator_error(this, compiler_errors_Error_new(node->span, "Symbol not found in CodeGenerator::gen_expression"));
        return;
      }
      switch (sym->type) {
        case compiler_ast_scopes_SymbolType_Function:
        case compiler_ast_scopes_SymbolType_Variable:
        case compiler_ast_scopes_SymbolType_Constant:
        case compiler_ast_scopes_SymbolType_EnumVariant: {
          std_buffer_Buffer_write_str(&this->out, compiler_ast_scopes_Symbol_out_name(sym));
        } break;
        default: std_panic(format_string("Unhandled symbol type: %s", compiler_ast_scopes_SymbolType_dbg(sym->type))); break;
      }
    } break;
    case compiler_ast_nodes_ASTType_Call: {
      compiler_ast_nodes_AST *callee = node->u.call.callee;
      if (callee->type==compiler_ast_nodes_ASTType_Identifier && (str_eq(callee->u.ident.name, "print") || str_eq(callee->u.ident.name, "println"))) {
        compiler_passes_code_generator_CodeGenerator_gen_internal_print(this, node);
        return;
      }
      compiler_ast_scopes_Symbol *sym = compiler_ast_nodes_AST_symbol(callee);
      if ((((bool)sym) && sym->type==compiler_ast_scopes_SymbolType_Structure) && node->u.call.is_constructor) {
        compiler_passes_code_generator_CodeGenerator_gen_constructor(this, node, sym->u.struc);
        return;
      }
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, callee, false);
      bool is_variadic_format = (((((bool)sym) && sym->type==compiler_ast_scopes_SymbolType_Function) && ((bool)sym->u.func)) && sym->u.func->is_variadic_format);
      std_buffer_Buffer_write_str(&this->out, "(");
      std_vector_Vector__15 *args = node->u.call.args;
      for (u32 i = 0; i < args->size; i+=1) {
        if (i != 0) {
        std_buffer_Buffer_write_str(&this->out, ", ");
        }
        compiler_ast_nodes_Argument *arg = std_vector_Vector__15_at(args, i);
        if ((i==(args->size - 1) && is_variadic_format) && arg->expr->type==compiler_ast_nodes_ASTType_FormatStringLiteral) {
          compiler_passes_code_generator_CodeGenerator_gen_format_string_variadic(this, arg->expr, false);
        } else {
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, arg->expr, false);
        }
      }
      std_buffer_Buffer_write_str(&this->out, ")");
    } break;
    case compiler_ast_nodes_ASTType_BoolLiteral: {
      bool bool_lit = node->u.bool_literal;
      std_buffer_Buffer_write_str(&this->out, (bool_lit ? "true" : "false"));
    } break;
    case compiler_ast_nodes_ASTType_UnaryOp: {
      switch (node->u.unary.op) {
        case compiler_ast_operators_Operator_Address: {
          compiler_ast_nodes_AST *expr = node->u.unary.expr;
          std_buffer_Buffer_write_str(&this->out, "&");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
        } break;
        case compiler_ast_operators_Operator_Dereference: {
          compiler_ast_nodes_AST *expr = node->u.unary.expr;
          std_buffer_Buffer_write_str(&this->out, "(*");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
          std_buffer_Buffer_write_str(&this->out, ")");
        } break;
        case compiler_ast_operators_Operator_Negate: {
          compiler_ast_nodes_AST *expr = node->u.unary.expr;
          std_buffer_Buffer_write_str(&this->out, "-");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
        } break;
        case compiler_ast_operators_Operator_BitwiseNot: {
          compiler_ast_nodes_AST *expr = node->u.unary.expr;
          std_buffer_Buffer_write_str(&this->out, "~");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
        } break;
        case compiler_ast_operators_Operator_Not: {
          compiler_ast_nodes_AST *expr = node->u.unary.expr;
          std_buffer_Buffer_write_str(&this->out, "!");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
        } break;
        case compiler_ast_operators_Operator_IsNotNull: {
          compiler_ast_nodes_AST *expr = node->u.unary.expr;
          std_buffer_Buffer_write_str(&this->out, "((bool)");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
          std_buffer_Buffer_write_str(&this->out, ")");
        } break;
        case compiler_ast_operators_Operator_PreIncrement:
        case compiler_ast_operators_Operator_PreDecrement: {
          std_buffer_Buffer_write_str(&this->out, compiler_passes_code_generator_CodeGenerator_get_op(this, node));
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, node->u.unary.expr, false);
        } break;
        case compiler_ast_operators_Operator_PostIncrement:
        case compiler_ast_operators_Operator_PostDecrement: {
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, node->u.unary.expr, false);
          std_buffer_Buffer_write_str(&this->out, compiler_passes_code_generator_CodeGenerator_get_op(this, node));
        } break;
        default: {
          compiler_passes_code_generator_CodeGenerator_error(this, compiler_errors_Error_new(node->span, format_string("Unhandled unary op type in CodeGenerator: %s", compiler_ast_operators_Operator_dbg(node->u.unary.op))));
        } break;
      }
    } break;
    case compiler_ast_nodes_ASTType_Cast: {
      compiler_ast_nodes_AST *expr = node->u.cast.lhs;
      compiler_types_Type *type = node->etype;
      std_buffer_Buffer_write_str(&this->out, "((");
      std_buffer_Buffer_write_str(&this->out, compiler_passes_code_generator_CodeGenerator_get_type_name_string(this, type, "", false));
      std_buffer_Buffer_write_str(&this->out, ")");
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, false);
      std_buffer_Buffer_write_str(&this->out, ")");
    } break;
    case compiler_ast_nodes_ASTType_SizeOf: {
      std_buffer_Buffer_write_str(&this->out, "((u32)sizeof(");
      compiler_passes_code_generator_CodeGenerator_gen_type(this, node->u.size_of_type);
      std_buffer_Buffer_write_str(&this->out, "))");
    } break;
    case compiler_ast_nodes_ASTType_Null: {
      std_buffer_Buffer_write_str(&this->out, "NULL");
    } break;
    case compiler_ast_nodes_ASTType_BinaryOp: {
      switch (node->u.binary.op) {
        case compiler_ast_operators_Operator_Index: {
          compiler_ast_nodes_AST *lhs = node->u.binary.lhs;
          compiler_ast_nodes_AST *rhs = node->u.binary.rhs;
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, lhs, false);
          std_buffer_Buffer_write_str(&this->out, "[");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, rhs, false);
          std_buffer_Buffer_write_str(&this->out, "]");
        } break;
        case compiler_ast_operators_Operator_And:
        case compiler_ast_operators_Operator_BitwiseAnd:
        case compiler_ast_operators_Operator_BitwiseOr:
        case compiler_ast_operators_Operator_BitwiseXor:
        case compiler_ast_operators_Operator_Divide:
        case compiler_ast_operators_Operator_GreaterThan:
        case compiler_ast_operators_Operator_GreaterThanEquals:
        case compiler_ast_operators_Operator_LeftShift:
        case compiler_ast_operators_Operator_LeftShiftEquals:
        case compiler_ast_operators_Operator_RightShiftEquals:
        case compiler_ast_operators_Operator_LessThan:
        case compiler_ast_operators_Operator_LessThanEquals:
        case compiler_ast_operators_Operator_Minus:
        case compiler_ast_operators_Operator_Modulus:
        case compiler_ast_operators_Operator_Multiply:
        case compiler_ast_operators_Operator_NotEquals:
        case compiler_ast_operators_Operator_Or:
        case compiler_ast_operators_Operator_Plus:
        case compiler_ast_operators_Operator_RightShift: {
          compiler_ast_nodes_AST *lhs = node->u.binary.lhs;
          compiler_ast_nodes_AST *rhs = node->u.binary.rhs;
          if (!is_top_level) {
          std_buffer_Buffer_write_str(&this->out, "(");
          }
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, lhs, false);
          std_buffer_Buffer_write_str(&this->out, " ");
          std_buffer_Buffer_write_str(&this->out, compiler_passes_code_generator_CodeGenerator_get_op(this, node));
          std_buffer_Buffer_write_str(&this->out, " ");
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, rhs, false);
          if (!is_top_level) {
          std_buffer_Buffer_write_str(&this->out, ")");
          }
        } break;
        case compiler_ast_operators_Operator_Equals:
        case compiler_ast_operators_Operator_Assignment:
        case compiler_ast_operators_Operator_IndexAssign:
        case compiler_ast_operators_Operator_PlusEquals:
        case compiler_ast_operators_Operator_MinusEquals:
        case compiler_ast_operators_Operator_DivideEquals:
        case compiler_ast_operators_Operator_MultiplyEquals: {
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, node->u.binary.lhs, false);
          std_buffer_Buffer_write_str(&this->out, compiler_passes_code_generator_CodeGenerator_get_op(this, node));
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, node->u.binary.rhs, false);
        } break;
        default: {
          compiler_passes_code_generator_CodeGenerator_error(this, compiler_errors_Error_new(node->span, format_string("Unhandled binary op type in CodeGenerator: %s", compiler_ast_operators_Operator_dbg(node->u.binary.op))));
        } break;
      }
    } break;
    default: {
      compiler_passes_code_generator_CodeGenerator_error(this, compiler_errors_Error_new(node->span, format_string("Unhandled expression type in CodeGenerator: %s", compiler_ast_nodes_ASTType_dbg(node->type))));
    } break;
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_var_declaration(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_Variable *var = node->u.var_decl.var;
  compiler_passes_code_generator_CodeGenerator_gen_type_and_name(this, var->type, compiler_ast_scopes_Symbol_out_name(var->sym));
  if (((bool)node->u.var_decl.init)) {
    std_buffer_Buffer_write_str(&this->out, " = ");
    compiler_passes_code_generator_CodeGenerator_gen_expression(this, node->u.var_decl.init, false);
  } else {
    switch (var->type->base) {
      case compiler_types_BaseType_Array:
      case compiler_types_BaseType_Structure: {
        std_buffer_Buffer_write_str(&this->out, " = {0}");
      } break;
      default: {
      } break;
    }
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_match_case_body(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_AST *body) {
  if (body->type==compiler_ast_nodes_ASTType_Block) {
    std_buffer_Buffer_write_str(&this->out, " ");
    compiler_passes_code_generator_CodeGenerator_gen_block(this, body, true);
  } else if (body->type==compiler_ast_nodes_ASTType_Call && body->returns) {
    std_buffer_Buffer_write_str(&this->out, " ");
    compiler_passes_code_generator_CodeGenerator_gen_expression(this, body, false);
    std_buffer_Buffer_write_str(&this->out, ";");
  } else if (((bool)node->etype) && (body->type != compiler_ast_nodes_ASTType_Yield)) {
    std_buffer_Buffer_write_str(&this->out, " {\n");
    this->indent+=1;
    compiler_passes_code_generator_CodeGenerator_gen_yield_expression(this, body);
    this->indent-=1;
    compiler_passes_code_generator_CodeGenerator_gen_indent(this);
    std_buffer_Buffer_write_str(&this->out, "}");
  } else {
    std_buffer_Buffer_write_str(&this->out, " {\n");
    this->indent+=1;
    compiler_passes_code_generator_CodeGenerator_gen_statement(this, body);
    this->indent-=1;
    compiler_passes_code_generator_CodeGenerator_gen_indent(this);
    std_buffer_Buffer_write_str(&this->out, "}");
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_custom_match(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_Match stmt = node->u.match_stmt;
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, "{\n");
  char *match_var = format_string("__match_var_%u", this->uid++);
  this->indent+=1;
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  compiler_passes_code_generator_CodeGenerator_gen_type_and_name(this, stmt.expr->etype, match_var);
  std_buffer_Buffer_write_str(&this->out, " = ");
  compiler_passes_code_generator_CodeGenerator_gen_expression(this, stmt.expr, false);
  std_buffer_Buffer_write_str(&this->out, ";\n");
  std_vector_Vector__18 *cases = stmt.cases;
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, "if (");
  for (u32 i = 0; i < cases->size; i+=1) {
    compiler_ast_nodes_MatchCase *_case = std_vector_Vector__18_at(cases, i);
    if (((bool)_case->cmp_fn)) {
      std_buffer_Buffer_write_str(&this->out, compiler_ast_scopes_Symbol_out_name(_case->cmp_fn->sym));
      std_buffer_Buffer_write_str(&this->out, "(");
      std_buffer_Buffer_write_str(&this->out, match_var);
      std_buffer_Buffer_write_str(&this->out, ", ");
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, _case->cond, false);
      std_buffer_Buffer_write_str(&this->out, ")");
    } else {
      std_buffer_Buffer_write_str(&this->out, format_string("(%s == ", match_var));
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, _case->cond, false);
      std_buffer_Buffer_write_str(&this->out, ")");
    }
    if (((bool)_case->body)) {
      std_buffer_Buffer_write_str(&this->out, ")");
      compiler_passes_code_generator_CodeGenerator_gen_match_case_body(this, node, _case->body);
      std_buffer_Buffer_write_str(&this->out, " else ");
      if (i != (cases->size - 1)) {
        std_buffer_Buffer_write_str(&this->out, "if (");
      }
    } else {
      std_buffer_Buffer_write_str(&this->out, " || ");
    }
  }
  if (((bool)stmt.defolt)) {
    compiler_passes_code_generator_CodeGenerator_gen_match_case_body(this, node, stmt.defolt);
  }
  std_buffer_Buffer_write_str(&this->out, "\n");
  this->indent-=1;
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, "}\n");
}

void compiler_passes_code_generator_CodeGenerator_gen_match_bool(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_Match stmt = node->u.match_stmt;
  compiler_ast_nodes_MatchCase *true_case = std_vector_Vector__18_at(stmt.cases, 0);
  compiler_ast_nodes_MatchCase *false_case = std_vector_Vector__18_at(stmt.cases, 1);
  if (!true_case->cond->u.bool_literal) {
    compiler_ast_nodes_MatchCase *tmp = true_case;
    true_case=false_case;
    false_case=tmp;
  }
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, "if (");
  compiler_passes_code_generator_CodeGenerator_gen_expression(this, stmt.expr, false);
  std_buffer_Buffer_write_str(&this->out, ") ");
  compiler_passes_code_generator_CodeGenerator_gen_control_body(this, node, true_case->body);
  std_buffer_Buffer_write_str(&this->out, " else ");
  compiler_passes_code_generator_CodeGenerator_gen_control_body(this, node, false_case->body);
}

void compiler_passes_code_generator_CodeGenerator_gen_match(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_Match stmt = node->u.match_stmt;
  if (stmt.is_custom_match) {
    compiler_passes_code_generator_CodeGenerator_gen_custom_match(this, node);
    return;
  }
  if (stmt.expr->etype->base==compiler_types_BaseType_Bool) {
    compiler_passes_code_generator_CodeGenerator_gen_match_bool(this, node);
    return;
  }
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, "switch (");
  compiler_passes_code_generator_CodeGenerator_gen_expression(this, stmt.expr, false);
  std_buffer_Buffer_write_str(&this->out, ") {\n");
  std_vector_Vector__18 *cases = stmt.cases;
  this->indent+=1;
  for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(cases); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
    compiler_ast_nodes_MatchCase *_case = std_vector_Iterator__18_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_write_str(&this->out, "case ");
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, _case->cond, false);
      std_buffer_Buffer_write_str(&this->out, ":");
      if (((bool)_case->body)) {
        compiler_passes_code_generator_CodeGenerator_gen_match_case_body(this, node, _case->body);
        std_buffer_Buffer_write_str(&this->out, " break;\n");
      } else {
        std_buffer_Buffer_write_str(&this->out, "\n");
      }
    }
  }
  if (((bool)stmt.defolt)) {
    compiler_passes_code_generator_CodeGenerator_gen_indent(this);
    std_buffer_Buffer_write_str(&this->out, "default:");
    compiler_passes_code_generator_CodeGenerator_gen_match_case_body(this, node, stmt.defolt);
    std_buffer_Buffer_write_str(&this->out, " break;\n");
  }
  this->indent-=1;
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, "}\n");
}

void compiler_passes_code_generator_CodeGenerator_gen_defers_upto(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_scopes_Scope *end_scope) {
  bool first = true;
  for (compiler_ast_scopes_Scope *cur = compiler_passes_code_generator_CodeGenerator_scope(this); ((bool)cur); cur=cur->parent) {
    for (u32 i = 0; i < cur->defers->size; i+=1) {
      if (first) {
        first=false;
        compiler_passes_code_generator_CodeGenerator_gen_indent(this);
        std_buffer_Buffer_write_str(&this->out, "/* defers */\n");
      }
      u32 idx = ((cur->defers->size - i) - 1);
      compiler_ast_nodes_AST *stmt = std_vector_Vector__13_at(cur->defers, idx);
      compiler_passes_code_generator_CodeGenerator_gen_statement(this, stmt);
    }
    if (cur==end_scope) {
    break;
    }
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_statement(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node) {
  compiler_passes_code_generator_CodeGenerator_gen_debug_info(this, node->span, false);
  switch (node->type) {
    case compiler_ast_nodes_ASTType_Return: {
      compiler_ast_scopes_Scope *upto = compiler_passes_code_generator_CodeGenerator_scope(this);
      for (compiler_ast_scopes_Scope *cur = compiler_passes_code_generator_CodeGenerator_scope(this); ((bool)cur) && ((bool)cur->cur_func); cur=cur->parent) {
        upto=cur;
      }
      compiler_passes_code_generator_CodeGenerator_gen_defers_upto(this, upto);
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      if (((bool)node->u.child)) {
        std_buffer_Buffer_write_str(&this->out, "return ");
        compiler_passes_code_generator_CodeGenerator_gen_expression(this, node->u.child, true);
        std_buffer_Buffer_write_str(&this->out, ";\n");
      } else {
        std_buffer_Buffer_write_str(&this->out, "return;\n");
      }
    } break;
    case compiler_ast_nodes_ASTType_Yield: {
      compiler_passes_code_generator_CodeGenerator_gen_yield_expression(this, node->u.child);
    } break;
    case compiler_ast_nodes_ASTType_Import: {
    } break;
    case compiler_ast_nodes_ASTType_Break:
    case compiler_ast_nodes_ASTType_Continue: {
      u32 loop_count = compiler_passes_code_generator_CodeGenerator_scope(this)->loop_count;
      compiler_ast_scopes_Scope *upto = compiler_passes_code_generator_CodeGenerator_scope(this);
      for (compiler_ast_scopes_Scope *cur = compiler_passes_code_generator_CodeGenerator_scope(this); ((bool)cur) && cur->loop_count==loop_count; cur=cur->parent) {
        upto=cur;
      }
      compiler_passes_code_generator_CodeGenerator_gen_defers_upto(this, upto);
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      if (node->type==compiler_ast_nodes_ASTType_Break) {
        std_buffer_Buffer_write_str(&this->out, "break;\n");
      } else {
        std_buffer_Buffer_write_str(&this->out, "continue;\n");
      }
    } break;
    case compiler_ast_nodes_ASTType_VarDeclaration: {
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      compiler_passes_code_generator_CodeGenerator_gen_var_declaration(this, node);
      std_buffer_Buffer_write_str(&this->out, ";\n");
    } break;
    case compiler_ast_nodes_ASTType_Block: {
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      compiler_passes_code_generator_CodeGenerator_gen_block(this, node, true);
      std_buffer_Buffer_write_str(&this->out, "\n");
    } break;
    case compiler_ast_nodes_ASTType_Defer: {
      std_vector_Vector__13_push(compiler_passes_code_generator_CodeGenerator_scope(this)->defers, node->u.child);
    } break;
    case compiler_ast_nodes_ASTType_If: {
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      std_vector_Vector__17 *branches = node->u.if_stmt.branches;
      for (u32 i = 0; i < branches->size; i+=1) {
        compiler_ast_nodes_IfBranch branch = std_vector_Vector__17_at(branches, i);
        if (i > 0) {
        std_buffer_Buffer_write_str(&this->out, " else ");
        }
        std_buffer_Buffer_write_str(&this->out, "if (");
        compiler_passes_code_generator_CodeGenerator_gen_expression(this, branch.cond, true);
        std_buffer_Buffer_write_str(&this->out, ") ");
        compiler_passes_code_generator_CodeGenerator_gen_control_body(this, node, branch.body);
      }
      compiler_ast_nodes_AST *else_body = node->u.if_stmt.els;
      if (((bool)else_body)) {
        std_buffer_Buffer_write_str(&this->out, " else ");
        compiler_passes_code_generator_CodeGenerator_gen_control_body(this, node, else_body);
        std_buffer_Buffer_write_str(&this->out, "\n");
      } else {
        std_buffer_Buffer_write_str(&this->out, "\n");
      }
    } break;
    case compiler_ast_nodes_ASTType_Match: {
      compiler_passes_code_generator_CodeGenerator_gen_match(this, node);
    } break;
    case compiler_ast_nodes_ASTType_While: {
      compiler_ast_nodes_AST *cond = node->u.loop.cond;
      compiler_ast_nodes_AST *body = node->u.loop.body;
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_write_str(&this->out, "while (");
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, cond, true);
      std_buffer_Buffer_write_str(&this->out, ") ");
      compiler_passes_code_generator_CodeGenerator_gen_block(this, body, true);
      std_buffer_Buffer_write_str(&this->out, "\n");
    } break;
    case compiler_ast_nodes_ASTType_For: {
      compiler_ast_nodes_AST *init = node->u.loop.init;
      compiler_ast_nodes_AST *cond = node->u.loop.cond;
      compiler_ast_nodes_AST *step = node->u.loop.step;
      compiler_ast_nodes_AST *body = node->u.loop.body;
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_write_str(&this->out, "for (");
      if (((bool)init)) {
        if (init->type==compiler_ast_nodes_ASTType_VarDeclaration) {
          compiler_passes_code_generator_CodeGenerator_gen_var_declaration(this, init);
        } else {
          compiler_passes_code_generator_CodeGenerator_gen_expression(this, init, true);
        }
      }
      std_buffer_Buffer_write_str(&this->out, "; ");
      if (((bool)cond)) {
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, cond, true);
      }
      std_buffer_Buffer_write_str(&this->out, "; ");
      if (((bool)step)) {
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, step, true);
      }
      std_buffer_Buffer_write_str(&this->out, ") ");
      compiler_passes_code_generator_CodeGenerator_gen_block(this, body, true);
      std_buffer_Buffer_write_str(&this->out, "\n");
    } break;
    case compiler_ast_nodes_ASTType_Assert: {
      compiler_ast_nodes_AST *expr = node->u.assertion.expr;
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_write_str(&this->out, "ae_assert(");
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, expr, true);
      std_buffer_Buffer_write_str(&this->out, ", ");
      {
        std_buffer_Buffer_write_str(&this->out, "\"");
        std_buffer_Buffer_write_str_f(&this->out, std_span_Location_str(&expr->span.start));
        char *expr_str = compiler_ast_program_Program_get_source_text(this->o->program, expr->span);
        std_buffer_Buffer_write_str(&this->out, ": Assertion failed: `");
        u32 len = strlen(expr_str);
        for (u32 i = 0; i < len; i+=1) {
          switch (expr_str[i]) {
            case '"': {
              std_buffer_Buffer_write_str(&this->out, "\\\"");
            } break;
            default: {
              std_buffer_Buffer_write_char(&this->out, expr_str[i]);
            } break;
          }
        }
        std_buffer_Buffer_write_str(&this->out, "`\"");
      }
      std_buffer_Buffer_write_str(&this->out, ", ");
      if (((bool)node->u.assertion.msg)) {
        compiler_passes_code_generator_CodeGenerator_gen_expression(this, node->u.assertion.msg, false);
      } else {
        std_buffer_Buffer_write_str(&this->out, "NULL");
      }
      std_buffer_Buffer_write_str(&this->out, ");");
      if (expr->type==compiler_ast_nodes_ASTType_BoolLiteral && expr->u.bool_literal==false) {
        std_buffer_Buffer_write_str(&this->out, " exit(1);");
      }
      std_buffer_Buffer_write_str(&this->out, "\n");
    } break;
    default: {
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      compiler_passes_code_generator_CodeGenerator_gen_expression(this, node, true);
      std_buffer_Buffer_write_str(&this->out, ";\n");
    } break;
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_block(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_AST *node, bool with_braces) {
  if (with_braces) {
  std_buffer_Buffer_write_str(&this->out, "{\n");
  }
  compiler_ast_scopes_Scope *scope = node->u.block.scope;
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, node->u.block.scope);
  std_vector_Vector__13 *statements = node->u.block.statements;
  this->indent+=1;
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(statements); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *statement = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_statement(this, statement);
    }
  }
  if (!node->returns) {
    compiler_passes_code_generator_CodeGenerator_gen_defers_upto(this, scope);
  }
  this->indent-=1;
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  if (with_braces) {
  std_buffer_Buffer_write_str(&this->out, "}");
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

char *compiler_passes_code_generator_CodeGenerator_helper_gen_type(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *top, compiler_types_Type *cur, char *acc, bool is_func_def) {
  switch (cur->base) {
    case compiler_types_BaseType_Void:
    case compiler_types_BaseType_Bool:
    case compiler_types_BaseType_Char:
    case compiler_types_BaseType_I8:
    case compiler_types_BaseType_I16:
    case compiler_types_BaseType_I32:
    case compiler_types_BaseType_I64:
    case compiler_types_BaseType_U8:
    case compiler_types_BaseType_U16:
    case compiler_types_BaseType_U32:
    case compiler_types_BaseType_U64:
    case compiler_types_BaseType_F32:
    case compiler_types_BaseType_F64: {
      str_replace(&acc, format_string("%s %s", compiler_types_BaseType_str(cur->base), acc));
    } break;
    case compiler_types_BaseType_Structure: {
      str_replace(&acc, format_string("%s %s", compiler_ast_scopes_Symbol_out_name(cur->u.struc->sym), acc));
    } break;
    case compiler_types_BaseType_Enum: {
      str_replace(&acc, format_string("%s %s", compiler_ast_scopes_Symbol_out_name(cur->u.enum_->sym), acc));
    } break;
    case compiler_types_BaseType_Alias: {
      acc=compiler_passes_code_generator_CodeGenerator_helper_gen_type(this, top, cur->u.ptr, acc, false);
    } break;
    case compiler_types_BaseType_Function: {
      std_buffer_Buffer args_str = std_buffer_Buffer_make(16);
      std_vector_Vector__4 *params = cur->u.func.params;
      if (params->size==0) {
      std_buffer_Buffer_write_str(&args_str, "void");
      }
      for (u32 i = 0; i < params->size; i+=1) {
        if (i != 0) {
        std_buffer_Buffer_write_str(&args_str, ", ");
        }
        compiler_ast_nodes_Variable *var = std_vector_Vector__4_at(params, i);
        char *arg_str = compiler_passes_code_generator_CodeGenerator_get_type_name_string(this, var->type, compiler_ast_scopes_Symbol_out_name(var->sym), false);
        std_buffer_Buffer_write_str_f(&args_str, arg_str);
      }
      if (cur->u.func.is_variadic) {
      std_buffer_Buffer_write_str(&args_str, ", ...");
      }
      if (is_func_def && cur==top) {
        str_replace(&acc, format_string("%s(%s)", acc, std_buffer_Buffer_str(args_str)));
      } else {
        str_replace(&acc, format_string("(*%s)(%s)", acc, std_buffer_Buffer_str(args_str)));
      }
      std_mem_free(args_str.data);
      acc=compiler_passes_code_generator_CodeGenerator_helper_gen_type(this, top, cur->u.func.return_type, acc, false);
    } break;
    case compiler_types_BaseType_Pointer: {
      bool needs_parens = (((bool)cur->u.ptr) && cur->u.ptr->base==compiler_types_BaseType_Array);
      if (cur->u.ptr->base==compiler_types_BaseType_Function) {
      } else if (needs_parens) {
        str_replace(&acc, format_string("(*%s)", acc));
      } else {
        str_replace(&acc, format_string("*%s", acc));
      }
      acc=compiler_passes_code_generator_CodeGenerator_helper_gen_type(this, top, cur->u.ptr, acc, false);
    } break;
    case compiler_types_BaseType_Array: {
      std_buffer_Buffer prev_buffer = this->out;
      this->out=std_buffer_Buffer_make(16);
      compiler_types_ArrayType *arr_typ = &cur->u.arr;
      if (((bool)arr_typ->size_expr)) {
        compiler_passes_code_generator_CodeGenerator_gen_expression(this, arr_typ->size_expr, false);
      } else if (arr_typ->size_known) {
        std_buffer_Buffer_write_str_f(&this->out, format_string("%u", arr_typ->size));
      } else {
        compiler_passes_code_generator_CodeGenerator_error(this, compiler_errors_Error_new(cur->span, "Array size not known at compile time"));
      }
      str_replace(&acc, format_string("%s[%s]", acc, std_buffer_Buffer_str(this->out)));
      std_mem_free(this->out.data);
      this->out=prev_buffer;
      acc=compiler_passes_code_generator_CodeGenerator_helper_gen_type(this, top, cur->u.arr.elem_type, acc, false);
    } break;
    default: {
      compiler_passes_code_generator_CodeGenerator_error(this, compiler_errors_Error_new(cur->span, format_string("Unhandled type found in CodeGenerator::helper_gen_type: %s: %s", compiler_types_BaseType_dbg(cur->base), compiler_types_Type_str(cur))));
    } break;
  }
  return acc;
}

char *compiler_passes_code_generator_CodeGenerator_get_type_name_string(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type, char *name, bool is_func_def) {
  ae_assert(type != NULL, "/Users/mustafa/ocen-lang/ocen/compiler/passes/code_generator.oc:1040:12: Assertion failed: `type != null`", NULL);
  char *final = compiler_passes_code_generator_CodeGenerator_helper_gen_type(this, type, type, strdup(name), is_func_def);
  str_strip_trailing_whitespace(final);
  return final;
}

void compiler_passes_code_generator_CodeGenerator_gen_type_and_name(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type, char *name) {
  std_buffer_Buffer_write_str_f(&this->out, compiler_passes_code_generator_CodeGenerator_get_type_name_string(this, type, name, false));
}

void compiler_passes_code_generator_CodeGenerator_gen_type(compiler_passes_code_generator_CodeGenerator *this, compiler_types_Type *type) {
  compiler_passes_code_generator_CodeGenerator_gen_type_and_name(this, type, "");
}

void compiler_passes_code_generator_CodeGenerator_gen_function(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Function *func) {
  if (func->is_method && func->parent_type->base==compiler_types_BaseType_Structure) {
    compiler_ast_nodes_Structure *struc = func->parent_type->u.struc;
    if (compiler_ast_scopes_Symbol_is_templated(struc->sym)) {
    return;
    }
  }
  if (compiler_ast_scopes_Symbol_is_templated(func->sym)) {
  return;
  }
  if (func->is_dead) {
  return;
  }
  compiler_passes_code_generator_CodeGenerator_gen_debug_info(this, func->sym->span, false);
  compiler_passes_code_generator_CodeGenerator_gen_function_decl(this, func);
  std_buffer_Buffer_write_str(&this->out, " ");
  compiler_passes_code_generator_CodeGenerator_gen_block(this, func->body, true);
  std_buffer_Buffer_write_str(&this->out, "\n\n");
}

void compiler_passes_code_generator_CodeGenerator_gen_function_decl(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Function *func) {
  char *funfull_name = compiler_ast_scopes_Symbol_out_name(func->sym);
  char *s = compiler_passes_code_generator_CodeGenerator_get_type_name_string(this, func->type, funfull_name, true);
  std_buffer_Buffer_write_str_f(&this->out, s);
}

void compiler_passes_code_generator_CodeGenerator_gen_functions(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns) {
  std_vector_Vector__6 *functions = ns->functions;
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      if (func->sym->is_extern) {
      continue;
      }
      if (compiler_ast_scopes_Symbol_is_templated(func->sym)) {
        for (std_vector_Iterator__3 __iter = std_vector_Vector__3_iter(func->sym->template->instances); std_vector_Iterator__3_has_value(&__iter); std_vector_Iterator__3_next(&__iter)) {
          compiler_ast_scopes_TemplateInstance *instance = std_vector_Iterator__3_cur(&__iter);
          {
            compiler_ast_scopes_Symbol *sym = instance->resolved;
            ae_assert(sym->type==compiler_ast_scopes_SymbolType_Function, "/Users/mustafa/ocen-lang/ocen/compiler/passes/code_generator.oc:1085:24: Assertion failed: `sym.type == Function`", NULL);
            compiler_ast_nodes_Function *func = sym->u.func;
            compiler_passes_code_generator_CodeGenerator_gen_function(this, func);
          }
        }
      } else {
        compiler_passes_code_generator_CodeGenerator_gen_function(this, func);
      }
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_functions(this, child);
    }
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

void compiler_passes_code_generator_CodeGenerator_gen_function_decls(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns) {
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      if (func->sym->is_extern) {
      continue;
      }
      if (func->is_method && func->parent_type->base==compiler_types_BaseType_Structure) {
        compiler_ast_nodes_Structure *struc = func->parent_type->u.struc;
        if (compiler_ast_scopes_Symbol_is_templated(struc->sym)) {
        continue;
        }
      }
      if (compiler_ast_scopes_Symbol_is_templated(func->sym)) {
        for (std_vector_Iterator__3 __iter = std_vector_Vector__3_iter(func->sym->template->instances); std_vector_Iterator__3_has_value(&__iter); std_vector_Iterator__3_next(&__iter)) {
          compiler_ast_scopes_TemplateInstance *instance = std_vector_Iterator__3_cur(&__iter);
          {
            compiler_ast_scopes_Symbol *sym = instance->resolved;
            ae_assert(sym->type==compiler_ast_scopes_SymbolType_Function, "/Users/mustafa/ocen-lang/ocen/compiler/passes/code_generator.oc:1112:24: Assertion failed: `sym.type == Function`", NULL);
            compiler_ast_nodes_Function *func = sym->u.func;
            if (func->is_dead) {
            continue;
            }
            compiler_passes_code_generator_CodeGenerator_gen_function_decl(this, func);
            if (func->exits) {
            std_buffer_Buffer_write_str(&this->out, " __attribute__((noreturn))");
            }
            std_buffer_Buffer_write_str(&this->out, ";\n");
          }
        }
        continue;
      }
      if (func->is_dead) {
      continue;
      }
      compiler_passes_code_generator_CodeGenerator_gen_function_decl(this, func);
      if (func->exits) {
      std_buffer_Buffer_write_str(&this->out, " __attribute__((noreturn))");
      }
      std_buffer_Buffer_write_str(&this->out, ";\n");
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_function_decls(this, child);
    }
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_enum_types(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_program_Namespace *ns) {
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    compiler_ast_nodes_Enum *enum_ = std_vector_Iterator__12_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_enum(this, enum_);
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_enum_types(this, child);
    }
  }
}

void compiler_passes_code_generator_CodeGenerator_gen_enum_dbg_method(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Enum *enum_) {
  compiler_ast_nodes_Function *dbg = std_map_Map__7_at(enum_->type->methods, "dbg");
  compiler_passes_code_generator_CodeGenerator_gen_function_decl(this, dbg);
  std_buffer_Buffer_write_str(&this->out, " {\n");
  this->indent+=1;
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, "switch (this) {\n");
  this->indent+=1;
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      compiler_passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_write_str(&this->out, "case ");
      std_buffer_Buffer_write_str(&this->out, compiler_ast_scopes_Symbol_out_name(field->sym));
      std_buffer_Buffer_write_str_f(&this->out, format_string(": return \"%s\";\n", field->sym->name));
    }
  }
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str_f(&this->out, format_string("default: return \"<unknown>\";\n"));
  this->indent-=1;
  compiler_passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_write_str(&this->out, "}\n");
  this->indent-=1;
  std_buffer_Buffer_write_str(&this->out, "}\n\n");
}

void compiler_passes_code_generator_CodeGenerator_gen_enum(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Enum *enum_) {
  if (enum_->is_dead) {
  return;
  }
  char *enum_name = compiler_ast_scopes_Symbol_out_name(enum_->sym);
  if (!enum_->sym->is_extern) {
    std_buffer_Buffer_write_str_f(&this->out, format_string("typedef enum %s {\n", enum_name));
    for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
      compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
      {
        std_buffer_Buffer_write_str(&this->out, "  ");
        std_buffer_Buffer_write_str(&this->out, compiler_ast_scopes_Symbol_out_name(field->sym));
        std_buffer_Buffer_write_str(&this->out, ",\n");
      }
    }
    std_buffer_Buffer_write_str_f(&this->out, format_string("} %s;\n\n", enum_name));
  }
  compiler_passes_code_generator_CodeGenerator_gen_enum_dbg_method(this, enum_);
}

void compiler_passes_code_generator_CodeGenerator_gen_struct(compiler_passes_code_generator_CodeGenerator *this, compiler_ast_nodes_Structure *struc) {
  if (struc->is_dead) {
  return;
  }
  char *strufull_name = compiler_ast_scopes_Symbol_out_name(struc->sym);
  if (struc->is_union) {
    std_buffer_Buffer_write_str_f(&this->out, format_string("union %s {\n", strufull_name));
  } else {
    std_buffer_Buffer_write_str_f(&this->out, format_string("struct %s {\n", strufull_name));
  }
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      std_buffer_Buffer_write_str(&this->out, "  ");
      compiler_passes_code_generator_CodeGenerator_gen_type_and_name(this, field->type, compiler_ast_scopes_Symbol_out_name(field->sym));
      std_buffer_Buffer_write_str(&this->out, ";\n");
    }
  }
  std_buffer_Buffer_write_str(&this->out, "};\n\n");
}

void compiler_passes_code_generator_CodeGenerator_gen_structs_and_typedefs(compiler_passes_code_generator_CodeGenerator *this) {
  std_vector_Vector__2 *structs = this->o->program->ordered_structs;
  if (structs->size==0) {
  return;
  }
  std_buffer_Buffer_write_str(&this->out, "/* Struct typedefs */\n");
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      if (struc->sym->is_extern) {
      continue;
      }
      if (struc->is_dead) {
      continue;
      }
      char *strufull_name = compiler_ast_scopes_Symbol_out_name(struc->sym);
      if (struc->is_union) {
        std_buffer_Buffer_write_str_f(&this->out, format_string("typedef union %s %s;\n", strufull_name, strufull_name));
      } else {
        std_buffer_Buffer_write_str_f(&this->out, format_string("typedef struct %s %s;\n", strufull_name, strufull_name));
      }
    }
  }
  std_buffer_Buffer_write_str(&this->out, "\n/* Struct definitions */\n");
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      if (struc->sym->is_extern) {
      continue;
      }
      compiler_passes_code_generator_CodeGenerator_gen_struct(this, struc);
    }
  }
  std_buffer_Buffer_write_str(&this->out, "\n");
}

char *compiler_passes_code_generator_CodeGenerator_generate(compiler_passes_code_generator_CodeGenerator *this) {
  for (std_vector_Iterator__1 __iter = std_vector_Vector__1_iter(this->o->program->c_includes); std_vector_Iterator__1_has_value(&__iter); std_vector_Iterator__1_next(&__iter)) {
    char *include = std_vector_Iterator__1_cur(&__iter);
    {
      std_buffer_Buffer_write_str_f(&this->out, format_string("#include \"%s\"\n", include));
    }
  }
  std_buffer_Buffer_write_str(&this->out, "\n");
  for (std_map_Iterator__6 __iter = std_map_Map__6_iter(this->o->program->c_embeds); std_map_Iterator__6_has_value(&__iter); std_map_Iterator__6_next(&__iter)) {
    std_map_Item__6 *it = std_map_Iterator__6_cur(&__iter);
    {
      std_buffer_Buffer_write_str_f(&this->out, format_string("/* Embed: %s */\n", it->key));
      std_buffer_Buffer_write_str(&this->out, it->value);
      std_buffer_Buffer_write_str(&this->out, "\n\n");
    }
  }
  std_buffer_Buffer_write_str(&this->out, "/* Enums */\n");
  compiler_passes_code_generator_CodeGenerator_gen_enum_types(this, this->o->program->global);
  std_buffer_Buffer_write_str(&this->out, "/* Constants */\n");
  compiler_passes_code_generator_CodeGenerator_gen_constants(this, this->o->program->global);
  compiler_passes_code_generator_CodeGenerator_gen_structs_and_typedefs(this);
  std_buffer_Buffer_write_str(&this->out, "/* function declarations */\n");
  compiler_passes_code_generator_CodeGenerator_gen_function_decls(this, this->o->program->global);
  compiler_passes_code_generator_CodeGenerator_gen_global_variables(this, this->o->program->global);
  std_buffer_Buffer_write_str(&this->out, "/* function implementations */\n");
  compiler_passes_code_generator_CodeGenerator_gen_functions(this, this->o->program->global);
  return std_buffer_Buffer_str(this->out);
}

compiler_passes_code_generator_CodeGenerator compiler_passes_code_generator_CodeGenerator_make(compiler_ast_program_Program *program) {
  return (compiler_passes_code_generator_CodeGenerator){.o=compiler_passes_generic_pass_GenericPass_new(program), .out=std_buffer_Buffer_make(16), .indent=0, .yield_vars=std_vector_Vector__1_new(16), .uid=0};
}

char *compiler_passes_code_generator_CodeGenerator_run(compiler_ast_program_Program *program) {
  compiler_passes_code_generator_CodeGenerator pass = compiler_passes_code_generator_CodeGenerator_make(program);
  return compiler_passes_code_generator_CodeGenerator_generate(&pass);
}

compiler_passes_reorder_structs_ReorderStructs *compiler_passes_reorder_structs_ReorderStructs_new(compiler_ast_program_Program *program) {
  compiler_passes_reorder_structs_ReorderStructs *pass = std_mem_alloc__0(1);
  (*pass)=(compiler_passes_reorder_structs_ReorderStructs){.o=compiler_passes_generic_pass_GenericPass_new(program), .all_structs=std_vector_Vector__2_new(16), .done=std_set_Set__0_new()};
  return pass;
}

void compiler_passes_reorder_structs_ReorderStructs_free(compiler_passes_reorder_structs_ReorderStructs *this) {
  std_vector_Vector__2_free(this->all_structs);
  std_set_Set__0_free(this->done);
  std_mem_free(this->o);
  std_mem_free(this);
}

void compiler_passes_reorder_structs_ReorderStructs_collect_all_structs(compiler_passes_reorder_structs_ReorderStructs *this, compiler_ast_program_Namespace *ns) {
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      if (compiler_ast_scopes_Symbol_is_templated(struc->sym)) {
        for (std_vector_Iterator__3 __iter = std_vector_Vector__3_iter(struc->sym->template->instances); std_vector_Iterator__3_has_value(&__iter); std_vector_Iterator__3_next(&__iter)) {
          compiler_ast_scopes_TemplateInstance *instance = std_vector_Iterator__3_cur(&__iter);
          {
            compiler_ast_scopes_Symbol *sym = instance->resolved;
            ae_assert(sym->type==compiler_ast_scopes_SymbolType_Structure, "/Users/mustafa/ocen-lang/ocen/compiler/passes/reorder_structs.oc:43:24: Assertion failed: `sym.type == Structure`", NULL);
            std_vector_Vector__2_push(this->all_structs, sym->u.struc);
          }
        }
      } else {
        std_vector_Vector__2_push(this->all_structs, struc);
      }
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_reorder_structs_ReorderStructs_collect_all_structs(this, child);
    }
  }
}

void compiler_passes_reorder_structs_ReorderStructs_dfs(compiler_passes_reorder_structs_ReorderStructs *this, compiler_ast_nodes_Structure *struc) {
  if (std_set_Set__0_contains(this->done, struc->sym->full_name)) {
  return;
  }
  std_set_Set__0_add(this->done, struc->sym->full_name);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      if (((bool)field->type) && field->type->base==compiler_types_BaseType_Structure) {
        compiler_ast_nodes_Structure *other = field->type->u.struc;
        compiler_passes_reorder_structs_ReorderStructs_dfs(this, other);
      }
    }
  }
  std_vector_Vector__2_push(this->o->program->ordered_structs, struc);
}

void compiler_passes_reorder_structs_ReorderStructs_reorder_structs(compiler_passes_reorder_structs_ReorderStructs *this) {
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(this->all_structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      compiler_passes_reorder_structs_ReorderStructs_dfs(this, struc);
    }
  }
}

void compiler_passes_reorder_structs_ReorderStructs_run(compiler_ast_program_Program *program) {
  compiler_passes_reorder_structs_ReorderStructs *pass = compiler_passes_reorder_structs_ReorderStructs_new(program);
  compiler_passes_reorder_structs_ReorderStructs_collect_all_structs(pass, program->global);
  compiler_passes_reorder_structs_ReorderStructs_reorder_structs(pass);
  compiler_passes_reorder_structs_ReorderStructs_free(pass);
}

compiler_errors_Error *compiler_passes_typechecker_TypeChecker_error(compiler_passes_typechecker_TypeChecker *this, compiler_errors_Error *err) {
  return compiler_passes_generic_pass_GenericPass_error(this->o, err);
}

compiler_ast_scopes_Scope *compiler_passes_typechecker_TypeChecker_scope(compiler_passes_typechecker_TypeChecker *this) {
  return compiler_passes_generic_pass_GenericPass_scope(this->o);
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_get_base_type(compiler_passes_typechecker_TypeChecker *this, compiler_types_BaseType base, std_span_Span span) {
  return compiler_ast_program_Program_get_base_type(this->o->program, base, span);
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_get_type_by_name(compiler_passes_typechecker_TypeChecker *this, char *name, std_span_Span span) {
  return compiler_ast_program_Program_get_type_by_name(this->o->program, name, span);
}

void compiler_passes_typechecker_TypeChecker_set_resolved_symbol(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_ast_scopes_Symbol *sym) {
  node->resolved_symbol=sym;
  if (node->type==compiler_ast_nodes_ASTType_Import) {
  return;
  }
  compiler_ast_scopes_ReferenceType ref_type = compiler_ast_scopes_ReferenceType_Normal;
  std_span_Span span = ({ std_span_Span __yield_0;
    switch (node->type) {
      case compiler_ast_nodes_ASTType_Identifier: {
        __yield_0 = node->span;
      } break;
      case compiler_ast_nodes_ASTType_Import:
      case compiler_ast_nodes_ASTType_Specialization: {
        return;
      } break;
      case compiler_ast_nodes_ASTType_OverloadedOperator: {
        ref_type=compiler_ast_scopes_ReferenceType_OperatorOverload;
        __yield_0 = node->u.operator_span;
      } break;
      case compiler_ast_nodes_ASTType_NSLookup: {
        __yield_0 = node->u.lookup.rhs_span;
      } break;
      case compiler_ast_nodes_ASTType_Member: {
        if (node->u.member.dot_shorthand) {
          ref_type=compiler_ast_scopes_ReferenceType_DotShorthand;
        }
        __yield_0 = node->u.member.rhs_span;
      } break;
      default: {
        ae_assert(false, "/Users/mustafa/ocen-lang/ocen/compiler/passes/typechecker.oc:64:20: Assertion failed: `false`", format_string("Unhandled node type in set_resolved_symbol: %s", compiler_ast_nodes_ASTType_dbg(node->type))); exit(1);
      } break;
    }
;__yield_0; });
  if (((bool)sym) && !this->in_template_instance) {
    compiler_ast_scopes_Symbol_add_reference(sym, ref_type, span);
  }
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_resolve_type(compiler_passes_typechecker_TypeChecker *this, compiler_types_Type *old, bool allow_incomplete, bool error, bool resolve_templates) {
  bool p_a = allow_incomplete;
  bool p_e = error;
  bool p_r = resolve_templates;
  compiler_types_Type *resolved = old;
  switch (old->base) {
    case compiler_types_BaseType_Pointer: {
      compiler_types_Type *ptr = compiler_passes_typechecker_TypeChecker_resolve_type(this, old->u.ptr, p_a, p_e, p_r);
      if (!((bool)ptr)) {
      return NULL;
      }
      switch (ptr->base) {
        case compiler_types_BaseType_Char: {
          resolved=compiler_ast_program_Program_get_type_by_name(this->o->program, "str", old->span);
        } break;
        case compiler_types_BaseType_Void: {
          resolved=compiler_ast_program_Program_get_type_by_name(this->o->program, "untyped_ptr", old->span);
        } break;
        default: {
          resolved=compiler_types_Type_shallow_copy(old);
          resolved->u.ptr=ptr;
        } break;
      }
    } break;
    case compiler_types_BaseType_Alias: {
      if (!((bool)old->u.ptr)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(old->span, "This type does not point to anything", "Cannot use a typedef for a type when defining any of it's members.\nUse fully qualified type for all field / method definitions."));
        return NULL;
      }
      return compiler_passes_typechecker_TypeChecker_resolve_type(this, old->u.ptr, p_a, p_e, p_r);
    } break;
    case compiler_types_BaseType_Function: {
      std_vector_Vector__4 *checked_params = std_vector_Vector__4_new(16);
      compiler_types_FunctionType func = old->u.func;
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(func.params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        compiler_ast_nodes_Variable *var = std_vector_Iterator__4_cur(&__iter);
        {
          var->type=compiler_passes_typechecker_TypeChecker_resolve_type(this, var->type, p_a, p_e, p_r);
          if (!((bool)var->type)) {
          return NULL;
          }
          std_vector_Vector__4_push(checked_params, var);
        }
      }
      compiler_types_Type *return_type = compiler_passes_typechecker_TypeChecker_resolve_type(this, func.return_type, p_a, p_e, p_r);
      if (!((bool)return_type)) {
      return NULL;
      }
      resolved=compiler_types_Type_shallow_copy(old);
      resolved->u.func=(compiler_types_FunctionType){.orig=func.orig, .params=checked_params, .return_type=return_type, .is_variadic=func.is_variadic};
    } break;
    case compiler_types_BaseType_Unresolved: {
      compiler_ast_nodes_AST *node = old->u.unresolved;
      compiler_ast_scopes_Symbol *res = compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node, p_e, NULL, p_r);
      if (((bool)res)) {
        switch (res->type) {
          case compiler_ast_scopes_SymbolType_TypeDef: {
            resolved=res->u.type_def;
          } break;
          case compiler_ast_scopes_SymbolType_Structure: {
            compiler_ast_nodes_Structure *struc = res->u.struc;
            if (compiler_ast_scopes_Symbol_is_templated(res) && !allow_incomplete) {
              if (error) {
                compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(old->span, format_string("Cannot use templated struct %s as a type", struc->sym->name)));
              }
              return resolved;
            }
            resolved=res->u.struc->type;
            if (node->type==compiler_ast_nodes_ASTType_Specialization && compiler_ast_scopes_Symbol_is_templated(res)) {
              ae_assert(!resolve_templates, "/Users/mustafa/ocen-lang/ocen/compiler/passes/typechecker.oc:144:36: Assertion failed: `not resolve_templates`", "Should have been errored in resolve_scoped_identifier");
              compiler_types_Type *type = compiler_types_Type_new_resolved(compiler_types_BaseType_UnresolvedTemplate, node->span);
              type->u.unresolved_spec=(compiler_types_UnresolvedTemplate){.base=resolved, .args=node->u.spec.template_args};
              resolved=type;
            }
          } break;
          case compiler_ast_scopes_SymbolType_Function: {
            compiler_ast_nodes_Function *func = res->u.func;
            if (compiler_ast_scopes_Symbol_is_templated(res) && !allow_incomplete) {
              if (error) {
                compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(old->span, format_string("Cannot use templated function %s as a type", func->sym->name)));
              }
              return resolved;
            }
            resolved=res->u.func->type;
            if (node->type==compiler_ast_nodes_ASTType_Specialization && compiler_ast_scopes_Symbol_is_templated(res)) {
              ae_assert(!resolve_templates, "/Users/mustafa/ocen-lang/ocen/compiler/passes/typechecker.oc:160:36: Assertion failed: `not resolve_templates`", "Should have been errored in resolve_scoped_identifier");
              compiler_types_Type *type = compiler_types_Type_new_resolved(compiler_types_BaseType_UnresolvedTemplate, node->span);
              type->u.unresolved_spec=(compiler_types_UnresolvedTemplate){.base=resolved, .args=node->u.spec.template_args};
              resolved=type;
            }
          } break;
          case compiler_ast_scopes_SymbolType_Enum: {
            resolved=res->u.enum_->type;
          } break;
          default: {
            if (error) {
              compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(res->span, format_string("Cannot use %s as a type", res->display)));
              resolved=NULL;
            }
          } break;
        }
      } else {
        if (error) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(old->u.unresolved->span, "Could not find type with this name"));
          resolved=NULL;
        }
      }
    } break;
    case compiler_types_BaseType_Array: {
      compiler_types_ArrayType arr = old->u.arr;
      compiler_types_Type *elem_type = compiler_passes_typechecker_TypeChecker_resolve_type(this, arr.elem_type, p_a, p_e, p_r);
      compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_check_expression(this, arr.size_expr, NULL);
      resolved=compiler_types_Type_shallow_copy(old);
      resolved->u.arr.elem_type=elem_type;
      compiler_passes_typechecker_TypeChecker_try_infer_array_size(this, &resolved->u.arr);
    } break;
    case compiler_types_BaseType_Structure:
    case compiler_types_BaseType_Char:
    case compiler_types_BaseType_Bool:
    case compiler_types_BaseType_Void:
    case compiler_types_BaseType_I8:
    case compiler_types_BaseType_I16:
    case compiler_types_BaseType_I32:
    case compiler_types_BaseType_I64:
    case compiler_types_BaseType_U8:
    case compiler_types_BaseType_U16:
    case compiler_types_BaseType_U32:
    case compiler_types_BaseType_U64:
    case compiler_types_BaseType_F32:
    case compiler_types_BaseType_F64:
    case compiler_types_BaseType_Enum: {
    } break;
    default: {
      if (error) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(old->span, format_string("Unhandled type in resolve %s", compiler_types_BaseType_str(old->base))));
        resolved=NULL;
      }
    } break;
  }
  return resolved;
}

void compiler_passes_typechecker_TypeChecker_try_infer_array_size(compiler_passes_typechecker_TypeChecker *this, compiler_types_ArrayType *arr_typ) {
  compiler_ast_nodes_AST *size_expr = arr_typ->size_expr;
  if (size_expr->type==compiler_ast_nodes_ASTType_IntLiteral) {
    if (((bool)size_expr->u.num_literal.suffix)) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(size_expr->span, "Array size literal cannot have a suffix"));
    }
    arr_typ->size_known=true;
    arr_typ->size=str_to_u32(size_expr->u.num_literal.text);
  }
}

void compiler_passes_typechecker_TypeChecker_resolve_templated_struct_methods(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Structure *old, compiler_ast_nodes_Structure *cur) {
  compiler_types_Type *old_type = old->type;
  compiler_types_Type *cur_type = cur->type;
  std_map_Map__7 *old_methods = old_type->methods;
  std_map_Map__7 *cur_methods = cur_type->methods;
  compiler_ast_program_Namespace *parent_ns = old->sym->ns;
  for (std_map_Iterator__7 __iter = std_map_Map__7_iter(old_methods); std_map_Iterator__7_has_value(&__iter); std_map_Iterator__7_next(&__iter)) {
    std_map_Item__7 *iter = std_map_Iterator__7_cur(&__iter);
    {
      char *name = iter->key;
      compiler_ast_nodes_Function *method = iter->value;
      compiler_ast_nodes_Function *new_method = compiler_ast_program_Program_get_function_deep_copy(this->o->program, method, parent_ns);
      new_method->parent_type=cur_type;
      std_map_Map__7_insert(cur_methods, name, new_method);
      compiler_ast_scopes_Symbol_update_parent(new_method->sym, cur_type->sym);
      std_vector_Vector__6_push(parent_ns->functions, new_method);
      if (!method->is_static) {
        compiler_ast_nodes_Variable *this_param = std_vector_Vector__4_at(new_method->params, 0);
        if (this_param->type->base==compiler_types_BaseType_Pointer) {
          this_param->type->u.ptr=cur_type;
        } else {
          this_param->type=cur_type;
        }
      }
      new_method->scope=compiler_passes_typechecker_TypeChecker_scope(this);
      compiler_passes_typechecker_TypeChecker_check_function_declaration(this, new_method);
      std_vector_Vector__6_push(this->unchecked_functions, new_method);
    }
  }
}

void compiler_passes_typechecker_TypeChecker_resolve_templated_struct(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Structure *struc, compiler_ast_scopes_TemplateInstance *instance) {
  compiler_ast_scopes_Symbol *sym = instance->resolved;
  compiler_ast_nodes_Structure *resolved_struc = compiler_ast_program_Program_get_struct_deep_copy(this->o->program, struc, struc->sym->ns);
  resolved_struc->sym->template=NULL;
  resolved_struc->sym=sym;
  sym->u.struc=resolved_struc;
  resolved_struc->format_spec=struc->format_spec;
  resolved_struc->format_args=struc->format_args;
  compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Structure, sym->span);
  typ->u.struc=resolved_struc;
  resolved_struc->type=typ;
  typ->sym=sym;
  typ->template_instance=instance;
  compiler_passes_typechecker_TypeChecker_resolve_struct(this, resolved_struc);
  compiler_passes_typechecker_TypeChecker_resolve_templated_struct_methods(this, struc, resolved_struc);
}

void compiler_passes_typechecker_TypeChecker_resolve_templated_function(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *func, compiler_ast_scopes_TemplateInstance *instance) {
  compiler_ast_scopes_Symbol *sym = instance->resolved;
  compiler_ast_nodes_Function *resolved_func = compiler_ast_program_Program_get_function_deep_copy(this->o->program, func, func->sym->ns);
  resolved_func->sym->template=NULL;
  resolved_func->sym=sym;
  if (((bool)func->parent_type)) {
    compiler_ast_scopes_Symbol_update_parent(sym, func->parent_type->sym);
  }
  sym->u.func=resolved_func;
  resolved_func->scope=compiler_passes_typechecker_TypeChecker_scope(this);
  compiler_passes_typechecker_TypeChecker_check_function_declaration(this, resolved_func);
  resolved_func->type->template_instance=instance;
  std_vector_Vector__6_push(this->unchecked_functions, resolved_func);
}

compiler_ast_scopes_Symbol *compiler_passes_typechecker_TypeChecker_find_template_instance(compiler_passes_typechecker_TypeChecker *this, std_vector_Vector__3 *template_instances, std_vector_Vector__0 *args) {
  for (std_vector_Iterator__3 __iter = std_vector_Vector__3_iter(template_instances); std_vector_Iterator__3_has_value(&__iter); std_vector_Iterator__3_next(&__iter)) {
    compiler_ast_scopes_TemplateInstance *instance = std_vector_Iterator__3_cur(&__iter);
    {
      if (compiler_ast_scopes_TemplateInstance_matches(instance, args)) {
      return instance->resolved;
      }
    }
  }
  return NULL;
}

compiler_ast_scopes_Symbol *compiler_passes_typechecker_TypeChecker_resolve_templated_symbol(compiler_passes_typechecker_TypeChecker *this, compiler_ast_scopes_Symbol *sym, compiler_ast_nodes_AST *node) {
  {
    compiler_ast_scopes_Symbol *found = compiler_passes_typechecker_TypeChecker_find_template_instance(this, sym->template->instances, node->u.spec.template_args);
    if (((bool)found)) {
    return found;
    }
  }
  compiler_ast_program_Namespace *parent_ns = sym->ns;
  std_vector_Vector__0 *template_args = node->u.spec.template_args;
  std_vector_Vector__19 *template_params = sym->template->params;
  if (template_params->size != template_args->size) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Invalid number of template arguments for %s", sym->name)));
    return NULL;
  }
  std_buffer_Buffer new_display_name = std_buffer_Buffer_make(16);
  std_buffer_Buffer_write_str(&new_display_name, sym->name);
  std_buffer_Buffer_write_str(&new_display_name, "<");
  compiler_ast_scopes_Scope *scope = compiler_ast_scopes_Scope_new(sym->ns->scope);
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, scope);
  bool was_in_template_instance = this->in_template_instance;
  this->in_template_instance=true;
  for (u32 i = 0; i < template_params->size; i+=1) {
    compiler_ast_scopes_Symbol *sym = std_vector_Vector__19_at(template_params, i);
    compiler_types_Type *arg = std_vector_Vector__0_at(template_args, i);
    if (i > 0) {
    std_buffer_Buffer_write_str(&new_display_name, ", ");
    }
    std_buffer_Buffer_write_str(&new_display_name, compiler_types_Type_str(arg));
    compiler_ast_scopes_Symbol *cur_sym = compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType_TypeDef, NULL, sym->name, sym->name, sym->name, sym->span);
    cur_sym->u.type_def=arg;
    compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, cur_sym, NULL);
  }
  std_buffer_Buffer_write_str(&new_display_name, ">");
  char *new_out_name = format_string("%s__%u", sym->name, sym->template->instances->size);
  compiler_ast_scopes_Symbol *new_sym = compiler_ast_scopes_Symbol_new_with_parent(sym->type, parent_ns, parent_ns->sym, new_out_name, sym->span);
  new_sym->display=std_buffer_Buffer_str(new_display_name);
  compiler_ast_scopes_TemplateInstance *instance = compiler_ast_scopes_TemplateInstance_new(template_args, sym, new_sym);
  std_vector_Vector__3_push(sym->template->instances, instance);
  switch (sym->type) {
    case compiler_ast_scopes_SymbolType_Structure: {
      compiler_passes_typechecker_TypeChecker_resolve_templated_struct(this, sym->u.struc, instance);
    } break;
    case compiler_ast_scopes_SymbolType_Function: {
      compiler_passes_typechecker_TypeChecker_resolve_templated_function(this, sym->u.func, instance);
    } break;
    default: {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot specialize non-templated symbol %s", sym->name)));
      /* defers */
      this->in_template_instance=was_in_template_instance;
      return NULL;
    } break;
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
  /* defers */
  this->in_template_instance=was_in_template_instance;
  return new_sym;
}

compiler_ast_scopes_Symbol *compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool error, compiler_types_Type *hint, bool resolve_templates) {
  switch (node->type) {
    case compiler_ast_nodes_ASTType_NSLookup: {
      std_span_Span lhs_span = node->u.lookup.lhs->span;
      compiler_ast_scopes_Symbol *lhs = compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node->u.lookup.lhs, error, NULL, resolve_templates);
      if (!((bool)lhs)) {
      return NULL;
      }
      char *name = node->u.lookup.rhs_name;
      if (!((bool)name)) {
      return NULL;
      }
      compiler_ast_scopes_Symbol *res = compiler_passes_generic_pass_GenericPass_lookup_in_symbol(this->o, lhs, name, node->span, error);
      node->resolved_symbol=res;
      return res;
    } break;
    case compiler_ast_nodes_ASTType_Identifier: {
      char *name = node->u.ident.name;
      compiler_ast_scopes_Symbol *res = ((compiler_ast_scopes_Symbol *)NULL);
      if (((bool)hint) && hint->base==compiler_types_BaseType_Enum) {
        compiler_ast_nodes_Enum *enum_ = hint->u.enum_;
        compiler_ast_nodes_Variable *field = ((compiler_ast_nodes_Variable *)compiler_ast_nodes_Enum_get_field(enum_, name));
        if (((bool)field)) {
          res=field->sym;
        }
      }
      if (!((bool)res)) {
        res=compiler_ast_scopes_Scope_lookup_recursive(compiler_passes_typechecker_TypeChecker_scope(this), name);
      }
      compiler_passes_typechecker_TypeChecker_set_resolved_symbol(this, node, res);
      if (error && !((bool)res)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Couldn't find this identifier"));
      }
      return res;
    } break;
    case compiler_ast_nodes_ASTType_Specialization: {
      compiler_ast_scopes_Symbol *base = compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node->u.spec.base, error, hint, resolve_templates);
      if (!((bool)base)) {
      return NULL;
      }
      base=compiler_ast_scopes_Symbol_remove_alias(base);
      if (!compiler_ast_scopes_Symbol_is_templated(base)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot specialize non-templated symbol %s", base->name)));
        return NULL;
      }
      std_vector_Vector__0 *args = node->u.spec.parsed_template_args;
      std_vector_Vector__0 *resolved_args = std_vector_Vector__0_new(args->size);
      bool failed = false;
      for (u32 i = 0; i < args->size; i+=1) {
        compiler_types_Type *resolved_arg = compiler_passes_typechecker_TypeChecker_resolve_type(this, std_vector_Vector__0_at(args, i), true, error, resolve_templates);
        if (!((bool)resolved_arg)) {
          failed=true;
          continue;
        }
        std_vector_Vector__0_push(resolved_args, resolved_arg);
      }
      if (failed) {
      return NULL;
      }
      node->u.spec.template_args=resolved_args;
      if (!resolve_templates) {
        compiler_passes_typechecker_TypeChecker_set_resolved_symbol(this, node, base);
        return base;
      }
      return compiler_passes_typechecker_TypeChecker_resolve_templated_symbol(this, base, node);
    } break;
    default: {
      if (error) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Don't know how to resolve node type %s", compiler_ast_nodes_ASTType_dbg(node->type))));
      }
      return NULL;
    } break;
  }
}

void compiler_passes_typechecker_TypeChecker_check_block(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint) {
  compiler_ast_nodes_Block *block = &node->u.block;
  compiler_ast_scopes_Scope *scope = compiler_ast_scopes_Scope_new(compiler_passes_typechecker_TypeChecker_scope(this));
  scope->can_yield=(scope->can_yield || is_expr);
  block->scope=scope;
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, block->scope);
  std_vector_Vector__13 *stmts = block->statements;
  std_span_Span yield_span = std_span_Span_default();
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(stmts); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *stmt = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_statement(this, stmt);
      if (stmt->returns) {
      node->returns=true;
      }
      if (stmt->type != compiler_ast_nodes_ASTType_Yield) {
      continue;
      }
      if (((bool)node->etype)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_hint(stmt->span, "Cannot yield multiple times in a block", yield_span, "Previously yielded here"));
      }
      node->etype=stmt->etype;
      yield_span=stmt->span;
      node->u.block.final_stmt=stmt;
    }
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

void compiler_passes_typechecker_TypeChecker_check_method_call(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *method, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_AST *callee = node->u.call.callee;
  if ((callee->type != compiler_ast_nodes_ASTType_Member) && (callee->type != compiler_ast_nodes_ASTType_NSLookup)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(callee->span, "Method call is not to a member, internal compiler error"));
    return;
  }
  if (callee->type != compiler_ast_nodes_ASTType_Member) {
  return;
  }
  if (method->params->size==0) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(callee->span, "Instance method should have `this` argument, internal error"));
    return;
  }
  compiler_types_Type *method_param = std_vector_Vector__4_at(method->params, 0)->type;
  if (!((bool)method_param)) {
  return;
  }
  compiler_ast_nodes_Member member = callee->u.member;
  compiler_ast_nodes_AST *first_arg = member.lhs;
  if (member.is_pointer && (method_param->base != compiler_types_BaseType_Pointer)) {
    first_arg=compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_Dereference, first_arg->span, first_arg);
  } else if (!member.is_pointer && method_param->base==compiler_types_BaseType_Pointer) {
    first_arg=compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_Address, first_arg->span, first_arg);
  }
  std_vector_Vector__15_push_front(node->u.call.args, compiler_ast_nodes_Argument_new(first_arg, NULL));
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_internal_print(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node) {
  std_vector_Vector__15 *args = node->u.call.args;
  if (args->size < 1) {
    compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new(node->span, "Function requires at least one argument"));
  }
  compiler_ast_nodes_Argument *first = std_vector_Vector__15_at(args, 0);
  compiler_types_Type *first_type = compiler_passes_typechecker_TypeChecker_check_expression(this, first->expr, NULL);
  if (((bool)first_type) && !compiler_types_Type_is_str(first_type)) {
    compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new(first->expr->span, format_string("First argument must be a string literal, got %s", compiler_types_Type_str(first_type))));
  }
  for (std_vector_Iterator__15 __iter = std_vector_Vector__15_iter(args); std_vector_Iterator__15_has_value(&__iter); std_vector_Iterator__15_next(&__iter)) {
    compiler_ast_nodes_Argument *arg = std_vector_Iterator__15_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_expression(this, arg->expr, NULL);
      compiler_passes_typechecker_TypeChecker_call_dbg_on_enum_value(this, &arg->expr);
    }
  }
  return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Void, node->span);
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_constructor(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node) {
  node->u.call.is_constructor=true;
  compiler_ast_nodes_AST *callee = node->u.call.callee;
  compiler_ast_scopes_Symbol *type_sym = compiler_ast_scopes_Symbol_remove_alias(callee->resolved_symbol);
  ae_assert(type_sym->type==compiler_ast_scopes_SymbolType_Structure, "/Users/mustafa/ocen-lang/ocen/compiler/passes/typechecker.oc:542:12: Assertion failed: `type_sym.type == Structure`", format_string("Got non-struct type in check_constructor: %s", compiler_ast_scopes_SymbolType_dbg(type_sym->type)));
  compiler_ast_nodes_Structure *struc = type_sym->u.struc;
  std_vector_Vector__4 *params = struc->fields;
  compiler_passes_typechecker_TypeChecker_check_call_args(this, node, params, false);
  return struc->type;
}

void compiler_passes_typechecker_TypeChecker_check_call_args(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, std_vector_Vector__4 *params, bool is_variadic) {
  std_vector_Vector__15 *args = node->u.call.args;
  if ((params->size < args->size) && !is_variadic) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Too many arguments, expected %u but got %u", params->size, args->size)));
  }
  for (u32 i = 0; i < params->size; i+=1) {
    compiler_ast_nodes_Variable *param = std_vector_Vector__4_at(params, i);
    if (i < args->size) {
      compiler_ast_nodes_Argument *arg = std_vector_Vector__15_at(args, i);
      if (((bool)arg->label) && !str_eq(arg->label, param->sym->name)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(arg->label_span, format_string("Argument label '%s' does not match parameter name '%s'", arg->label, param->sym->name)));
      }
      compiler_types_Type *arg_type = compiler_passes_typechecker_TypeChecker_check_expression(this, arg->expr, param->type);
      if (!((bool)arg_type) || !((bool)param->type)) {
      continue;
      }
      if (!compiler_types_Type_eq(arg_type, param->type, false)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(arg->expr->span, format_string("Argument %s has type %s but expected %s", param->sym->name, compiler_types_Type_str(arg_type), compiler_types_Type_str(param->type))));
      }
    } else if (((bool)param->default_value)) {
      compiler_ast_nodes_Argument *new_arg = compiler_ast_nodes_Argument_new(param->default_value, NULL);
      std_vector_Vector__15_push(args, new_arg);
    } else {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Missing required argument %s", param->sym->name)));
      return;
    }
  }
  if (is_variadic) {
    if (args->size < params->size) {
      ae_assert(this->o->program->errors->size > 1, "/Users/mustafa/ocen-lang/ocen/compiler/passes/typechecker.oc:593:20: Assertion failed: `.o.program.errors.size > 1`", "Should have errored already");
      return;
    }
    for (u32 i = params->size; i < args->size; i+=1) {
      compiler_ast_nodes_Argument *arg = std_vector_Vector__15_at(args, i);
      compiler_types_Type *arg_type = compiler_passes_typechecker_TypeChecker_check_expression(this, arg->expr, NULL);
      if (!((bool)arg_type)) {
      continue;
      }
    }
  }
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_call(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_AST *callee = node->u.call.callee;
  std_vector_Vector__15 *args = node->u.call.args;
  if (callee->type==compiler_ast_nodes_ASTType_Identifier) {
    callee->u.ident.is_function=false;
    char *name = callee->u.ident.name;
    if (str_eq(name, "print") || str_eq(name, "println")) {
      return compiler_passes_typechecker_TypeChecker_check_internal_print(this, node);
    }
  }
  compiler_types_Type *res = ({ compiler_types_Type *__yield_0;
    switch (callee->type) {
      case compiler_ast_nodes_ASTType_Member: {
        __yield_0 = compiler_passes_typechecker_TypeChecker_check_member(this, callee, true);
      } break;
      case compiler_ast_nodes_ASTType_Identifier:
      case compiler_ast_nodes_ASTType_NSLookup:
      case compiler_ast_nodes_ASTType_Specialization: {
        compiler_ast_scopes_Symbol *sym = compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(this, callee, true, NULL, true);
        __yield_0 = ({ compiler_types_Type *__yield_1;
          if (((bool)sym)) {
            sym=compiler_ast_scopes_Symbol_remove_alias(sym);
            compiler_passes_typechecker_TypeChecker_set_resolved_symbol(this, callee, sym);
            __yield_1 = ({ compiler_types_Type *__yield_2;
              switch (sym->type) {
                case compiler_ast_scopes_SymbolType_Structure:
                case compiler_ast_scopes_SymbolType_TypeDef: {
                  return compiler_passes_typechecker_TypeChecker_check_constructor(this, node);
                } break;
                default: {
                  __yield_2 = compiler_passes_typechecker_TypeChecker_check_expression(this, callee, NULL);
                } break;
              }
;__yield_2; });
          } else {
          __yield_1 = this->o->error_type;
          };__yield_1; });
      } break;
      default: {
        __yield_0 = compiler_passes_typechecker_TypeChecker_check_expression(this, callee, NULL);
      } break;
    }
;__yield_0; });
  if (((bool)res)) {
    if (res->base==compiler_types_BaseType_Pointer && res->u.ptr->base==compiler_types_BaseType_Function) {
      res=res->u.ptr;
    } else if (res->base != compiler_types_BaseType_Function) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(callee->span, format_string("Cannot call a non-function type: %s", compiler_types_Type_str(res))));
      res=this->o->error_type;
    }
  } else {
    res=this->o->error_type;
  }
  if (res->base==compiler_types_BaseType_Error) {
    for (std_vector_Iterator__15 __iter = std_vector_Vector__15_iter(args); std_vector_Iterator__15_has_value(&__iter); std_vector_Iterator__15_next(&__iter)) {
      compiler_ast_nodes_Argument *arg = std_vector_Iterator__15_cur(&__iter);
      {
        compiler_passes_typechecker_TypeChecker_check_expression(this, arg->expr, NULL);
      }
    }
    return NULL;
  }
  compiler_types_FunctionType func = res->u.func;
  if (((bool)func.orig) && func.orig->exits) {
  node->returns=true;
  }
  std_vector_Vector__4 *params = func.params;
  if ((((bool)func.orig) && func.orig->is_method) && !func.orig->is_static) {
    compiler_passes_typechecker_TypeChecker_check_method_call(this, func.orig, node);
  }
  compiler_passes_typechecker_TypeChecker_check_call_args(this, node, params, func.is_variadic);
  bool is_variadic_format = (((bool)func.orig) && func.orig->is_variadic_format);
  if (is_variadic_format && (args->size >= params->size)) {
    std_vector_Vector__15 *args = node->u.call.args;
    compiler_ast_nodes_Variable *param = std_vector_Vector__4_back(params, 0);
    compiler_ast_nodes_AST *arg = std_vector_Vector__15_at(args, (params->size - 1))->expr;
    if (!compiler_types_Type_is_str(param->type)) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(func.orig->sym->span, "Variadic-format function must have last positional argument of type 'str'"));
    }
    switch (arg->type) {
      case compiler_ast_nodes_ASTType_StringLiteral:
      case compiler_ast_nodes_ASTType_FormatStringLiteral: {
      } break;
      default: {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(arg->span, "Expected a string literal for variadic-format function"));
      } break;
    }
  }
  if (((bool)func.orig)) {
    node->u.call.is_function_pointer=false;
    node->u.call.func=func.orig;
  }
  return func.return_type;
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_pointer_arith(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *_lhs, compiler_types_Type *_rhs) {
  compiler_ast_operators_Operator op = node->u.binary.op;
  if (op==compiler_ast_operators_Operator_Plus || op==compiler_ast_operators_Operator_Minus) {
    if (_lhs->base==compiler_types_BaseType_Pointer && compiler_types_Type_is_integer(_rhs)) {
      return _lhs;
    }
    if (compiler_types_Type_is_integer(_lhs) && _rhs->base==compiler_types_BaseType_Pointer) {
      return _rhs;
    }
    if (compiler_types_Type_eq(_lhs, _rhs, false) && _lhs->base==compiler_types_BaseType_Pointer) {
      if (op==compiler_ast_operators_Operator_Minus) {
        return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_I64, node->span);
      }
    }
  }
  compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Invalid pointer arithmetic"));
  return NULL;
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_find_and_replace_overloaded_op(compiler_passes_typechecker_TypeChecker *this, compiler_ast_operators_Operator op, compiler_ast_nodes_AST *node, compiler_ast_nodes_AST *arg1, compiler_ast_nodes_AST *arg2, compiler_ast_nodes_AST *arg3) {
  if (compiler_ast_operators_Operator_needs_lhs_pointer_for_overload(op)) {
    if (compiler_ast_nodes_AST_is_lvalue(arg1) && (arg1->etype->base != compiler_types_BaseType_Pointer)) {
      arg1=compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_Address, arg1->span, arg1);
      if (!((bool)compiler_passes_typechecker_TypeChecker_check_expression(this, arg1, NULL))) {
      return NULL;
      }
    }
  }
  compiler_ast_operators_OperatorOverload overload = {0};
  overload.op=op;
  if (((bool)arg1)) {
  overload.type1=arg1->etype;
  }
  if (((bool)arg2)) {
  overload.type2=arg2->etype;
  }
  if (((bool)arg3)) {
  overload.type3=arg3->etype;
  }
  compiler_ast_nodes_Function *func = std_map_Map__5_get(this->o->program->operator_overloads, overload, NULL);
  if (!((bool)func)) {
  return NULL;
  }
  compiler_ast_nodes_AST *callee = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_OverloadedOperator, node->u.binary.op_span);
  callee->u.operator_span=({ std_span_Span __yield_0;
    switch (node->type) {
      case compiler_ast_nodes_ASTType_BinaryOp: {
        __yield_0 = node->u.binary.op_span;
      } break;
      case compiler_ast_nodes_ASTType_UnaryOp: {
        __yield_0 = node->u.unary.op_span;
      } break;
      default: {
        __yield_0 = node->span;
      } break;
    }
;__yield_0; });
  compiler_passes_typechecker_TypeChecker_set_resolved_symbol(this, callee, func->sym);
  std_vector_Vector__15 *args = std_vector_Vector__15_new(16);
  if (((bool)arg1)) {
  std_vector_Vector__15_push(args, compiler_ast_nodes_Argument_new(arg1, NULL));
  }
  if (((bool)arg2)) {
  std_vector_Vector__15_push(args, compiler_ast_nodes_Argument_new(arg2, NULL));
  }
  if (((bool)arg3)) {
  std_vector_Vector__15_push(args, compiler_ast_nodes_Argument_new(arg3, NULL));
  }
  node->type=compiler_ast_nodes_ASTType_Call;
  node->u.call.callee=callee;
  node->u.call.args=args;
  node->etype=func->return_type;
  return func->return_type;
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_binary_op(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *lhs, compiler_types_Type *rhs) {
  compiler_ast_operators_Operator op = node->u.binary.op;
  compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_find_and_replace_overloaded_op(this, op, node, node->u.binary.lhs, node->u.binary.rhs, NULL);
  if (((bool)res)) {
  return res;
  }
  switch (op) {
    case compiler_ast_operators_Operator_Plus:
    case compiler_ast_operators_Operator_Minus:
    case compiler_ast_operators_Operator_Multiply:
    case compiler_ast_operators_Operator_Divide: {
      if (lhs->base==compiler_types_BaseType_Pointer || rhs->base==compiler_types_BaseType_Pointer) {
        return compiler_passes_typechecker_TypeChecker_check_pointer_arith(this, node, lhs, rhs);
      } else if (!compiler_types_Type_is_numeric(lhs) || !compiler_types_Type_is_numeric(rhs)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Operator `%s` does not support `%s` and `%s`", compiler_ast_operators_Operator_dbg(op), compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      } else if (!compiler_types_Type_eq(lhs, rhs, false)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      } else {
        return lhs;
      }
    } break;
    case compiler_ast_operators_Operator_PlusEquals:
    case compiler_ast_operators_Operator_MinusEquals:
    case compiler_ast_operators_Operator_DivideEquals:
    case compiler_ast_operators_Operator_MultiplyEquals: {
      compiler_ast_nodes_AST *lhs_node = node->u.binary.lhs;
      if (!compiler_ast_nodes_AST_is_lvalue(lhs_node)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Left hand side of assignment must be assignable"));
        return NULL;
      }
      if (!compiler_types_Type_is_numeric(lhs) || !compiler_types_Type_is_numeric(rhs)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Operator `%s` does not support `%s` and `%s`", compiler_ast_operators_Operator_dbg(op), compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      }
      if (!compiler_types_Type_eq(lhs, rhs, false)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      }
      return lhs;
    } break;
    case compiler_ast_operators_Operator_LessThan:
    case compiler_ast_operators_Operator_LessThanEquals:
    case compiler_ast_operators_Operator_GreaterThan:
    case compiler_ast_operators_Operator_GreaterThanEquals: {
      if (!compiler_types_Type_is_numeric_or_char(lhs) || !compiler_types_Type_is_numeric_or_char(rhs)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Operator `%s` does not support `%s` and `%s`", compiler_ast_operators_Operator_dbg(op), compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      }
      if (!compiler_types_Type_eq(lhs, rhs, false)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
      }
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
    } break;
    case compiler_ast_operators_Operator_Equals: {
      if (!compiler_types_Type_eq(lhs, rhs, false) || lhs->base==compiler_types_BaseType_Structure) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Operator `%s` does not support `%s` and `%s`", compiler_ast_operators_Operator_dbg(op), compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      }
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
    } break;
    case compiler_ast_operators_Operator_NotEquals: {
      if (!compiler_types_Type_eq(lhs, rhs, false) || lhs->base==compiler_types_BaseType_Structure) {
        compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_find_and_replace_overloaded_op(this, compiler_ast_operators_Operator_Equals, node, node->u.binary.lhs, node->u.binary.rhs, NULL);
        if (((bool)res)) {
          compiler_ast_nodes_AST *copy = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_UnaryOp, node->span);
          (*copy)=(*node);
          (*node)=(*compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_Not, node->span, copy));
          compiler_passes_typechecker_TypeChecker_check_expression(this, node, NULL);
        } else {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Operator `%s` does not support `%s` and `%s`", compiler_ast_operators_Operator_dbg(op), compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
          return NULL;
        }
      }
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
    } break;
    case compiler_ast_operators_Operator_And:
    case compiler_ast_operators_Operator_Or: {
      if (!compiler_types_Type_eq(lhs, rhs, false) || (lhs->base != compiler_types_BaseType_Bool)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Operator `%s` does not support `%s` and `%s`", compiler_ast_operators_Operator_dbg(op), compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      }
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
    } break;
    case compiler_ast_operators_Operator_BitwiseXor: {
      if (!compiler_types_Type_eq(lhs, rhs, false)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Operator `%s` does not support `%s` and `%s`", compiler_ast_operators_Operator_dbg(op), compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      }
      if ((lhs->base != compiler_types_BaseType_Bool) && !compiler_types_Type_is_integer(lhs)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Operator requires integer types"));
      }
      return lhs;
    } break;
    case compiler_ast_operators_Operator_Modulus:
    case compiler_ast_operators_Operator_BitwiseOr:
    case compiler_ast_operators_Operator_BitwiseAnd:
    case compiler_ast_operators_Operator_LeftShift:
    case compiler_ast_operators_Operator_RightShift:
    case compiler_ast_operators_Operator_LeftShiftEquals:
    case compiler_ast_operators_Operator_RightShiftEquals: {
      if (!compiler_types_Type_is_integer(lhs) || !compiler_types_Type_is_integer(rhs)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Operator `%s` does not support `%s` and `%s`", compiler_ast_operators_Operator_dbg(op), compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
        return NULL;
      }
      if (!compiler_types_Type_eq(lhs, rhs, false)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
      }
      compiler_ast_nodes_AST *lhs_node = node->u.binary.lhs;
      switch (op) {
        case compiler_ast_operators_Operator_LeftShiftEquals:
        case compiler_ast_operators_Operator_RightShiftEquals: {
          if (!compiler_ast_nodes_AST_is_lvalue(lhs_node)) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(lhs_node->span, format_string("Must be an l-value")));
            return NULL;
          }
        } break;
        default: {
        } break;
      }
      return lhs;
    } break;
    default: std_panic(format_string("Internal error: unhandled op in check_binary_op: %s", compiler_ast_nodes_ASTType_dbg(node->type))); break;
  }
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_format_string(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node) {
  std_vector_Vector__1 *parts = node->u.fmt_str.parts;
  std_vector_Vector__13 *exprs = node->u.fmt_str.exprs;
  if (parts->size != (exprs->size + 1)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Number of format string parts does not match number of expressions"));
  }
  for (u32 i = 0; i < exprs->size; i+=1) {
    compiler_ast_nodes_AST *expr = std_vector_Vector__13_at(exprs, i);
    compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, expr, NULL);
    if (!((bool)typ)) {
    continue;
    }
    typ=compiler_passes_typechecker_TypeChecker_call_dbg_on_enum_value(this, &expr);
    exprs->data[i]=expr;
    if (!((bool)typ)) {
    continue;
    }
    typ=compiler_types_Type_unaliased(typ);
    switch (typ->base) {
      case compiler_types_BaseType_Bool:
      case compiler_types_BaseType_Char:
      case compiler_types_BaseType_I8:
      case compiler_types_BaseType_I16:
      case compiler_types_BaseType_I32:
      case compiler_types_BaseType_I64:
      case compiler_types_BaseType_U8:
      case compiler_types_BaseType_U16:
      case compiler_types_BaseType_U32:
      case compiler_types_BaseType_U64:
      case compiler_types_BaseType_F32:
      case compiler_types_BaseType_F64:
      case compiler_types_BaseType_Pointer: {
      } break;
      default: {
        bool can_format = false;
        if (typ->base==compiler_types_BaseType_Structure) {
          compiler_ast_nodes_Structure *struc = typ->u.struc;
          if (((bool)struc) && ((bool)struc->format_spec)) {
            can_format=true;
          }
        }
        if (can_format) {
          switch (expr->type) {
            case compiler_ast_nodes_ASTType_Identifier:
            case compiler_ast_nodes_ASTType_Member:
            case compiler_ast_nodes_ASTType_NSLookup: {
              continue;
            } break;
            default: {
              compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(expr->span, format_string("Can only format %s in simple expressions", typ->sym->display), "Try moving the expression into a variable and formatting that instead"));
            } break;
          }
        } else {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(expr->span, format_string("Type '%s' cannot be formatted automatically", compiler_types_Type_str(typ))));
        }
      } break;
    }
  }
  return compiler_passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span);
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_member(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_being_called) {
  compiler_types_Type *lhs = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.member.lhs, NULL);
  if (!((bool)lhs)) {
  return NULL;
  }
  bool is_pointer = false;
  if (lhs->base==compiler_types_BaseType_Pointer) {
    is_pointer=true;
    if (!((bool)lhs->u.ptr)) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(lhs->span, "Got unresolved type"));
      return NULL;
    }
    lhs=lhs->u.ptr;
  }
  if (lhs->base==compiler_types_BaseType_UnresolvedTemplate) {
    lhs=lhs->u.unresolved_spec.base;
  }
  char *rhs_name = node->u.member.rhs_name;
  if (!((bool)rhs_name)) {
  return NULL;
  }
  if (lhs->base==compiler_types_BaseType_Structure) {
    compiler_ast_nodes_Structure *struc = lhs->u.struc;
    compiler_ast_nodes_Variable *field = compiler_ast_nodes_Structure_get_field(struc, rhs_name);
    if (((bool)field)) {
      node->u.member.is_pointer=is_pointer;
      compiler_passes_typechecker_TypeChecker_set_resolved_symbol(this, node, field->sym);
      return field->type;
    }
  }
  if (compiler_types_Type_can_have_methods(lhs)) {
    compiler_ast_nodes_Function *method = std_map_Map__7_get(lhs->methods, rhs_name, NULL);
    if (((bool)method)) {
      if (!is_being_called) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Cannot access method without calling it"));
        return NULL;
      }
      if (method->is_static) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Cannot call static method as instance method"));
        return NULL;
      }
      node->u.member.is_pointer=is_pointer;
      compiler_passes_typechecker_TypeChecker_set_resolved_symbol(this, node, method->sym);
      return method->type;
    }
  }
  compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Type %s has no member named '%s'", compiler_types_Type_str(lhs), rhs_name)));
  return NULL;
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_expression(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *hint) {
  if (((bool)node->etype)) {
  return node->etype;
  }
  compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression_helper(this, node, hint);
  if (((bool)typ) && (node->type != compiler_ast_nodes_ASTType_ArrayLiteral)) {
  typ=compiler_types_Type_decay_array(typ);
  }
  node->etype=typ;
  return typ;
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_index(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *hint, bool is_being_assigned) {
  compiler_types_Type *lhs = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.binary.lhs, NULL);
  compiler_types_Type *rhs = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.binary.rhs, NULL);
  if (!((bool)lhs) || !((bool)rhs)) {
  return NULL;
  }
  if (!is_being_assigned) {
    compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_find_and_replace_overloaded_op(this, compiler_ast_operators_Operator_Index, node, node->u.binary.lhs, node->u.binary.rhs, NULL);
    if (((bool)res)) {
    return res;
    }
  }
  if (!compiler_types_Type_is_integer(rhs)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Index must be an integer, got %s", compiler_types_Type_str(rhs))));
    return NULL;
  }
  lhs=compiler_types_Type_unaliased(lhs);
  switch (lhs->base) {
    case compiler_types_BaseType_Array: {
      return lhs->u.arr.elem_type;
    } break;
    case compiler_types_BaseType_Pointer: {
      return lhs->u.ptr;
    } break;
    default: {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot index type %s", compiler_types_Type_str(lhs))));
      return NULL;
    } break;
  }
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_assignment(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *lhs, compiler_types_Type *rhs) {
  if (!compiler_ast_nodes_AST_is_lvalue(node->u.binary.lhs)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->u.binary.lhs->span, "Must be an l-value"));
  }
  if (!compiler_types_Type_eq(lhs, rhs, false)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->u.binary.rhs->span, format_string("Variable type does not match assignment type, Expected type '%s', got '%s'", compiler_types_Type_str(lhs), compiler_types_Type_str(rhs))));
  }
  return lhs;
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_expression_helper(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *hint) {
  switch (node->type) {
    case compiler_ast_nodes_ASTType_IntLiteral: {
      if (((bool)node->u.num_literal.suffix)) {
        return compiler_passes_typechecker_TypeChecker_resolve_type(this, node->u.num_literal.suffix, false, true, true);
      }
      if (((bool)hint) && compiler_types_Type_is_integer(hint)) {
      return hint;
      }
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_U32, node->span);
    } break;
    case compiler_ast_nodes_ASTType_FloatLiteral: {
      if (((bool)node->u.num_literal.suffix)) {
        return compiler_passes_typechecker_TypeChecker_resolve_type(this, node->u.num_literal.suffix, false, true, true);
      }
      if (((bool)hint) && compiler_types_Type_is_float(hint)) {
      return hint;
      }
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_F32, node->span);
    } break;
    case compiler_ast_nodes_ASTType_StringLiteral: {
      return compiler_passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span);
    } break;
    case compiler_ast_nodes_ASTType_Null: {
      if (((bool)hint)) {
        if (hint->base==compiler_types_BaseType_Pointer) {
        return hint;
        }
        if (hint->base==compiler_types_BaseType_Function) {
        return hint;
        }
      }
      return compiler_passes_typechecker_TypeChecker_get_type_by_name(this, "untyped_ptr", node->span);
    } break;
    case compiler_ast_nodes_ASTType_Cast: {
      compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.cast.lhs, NULL);
      if (!((bool)typ)) {
      return NULL;
      }
      compiler_types_Type *target = compiler_passes_typechecker_TypeChecker_resolve_type(this, node->u.cast.to, false, true, true);
      if (!((bool)target)) {
      return NULL;
      }
      node->u.cast.to=target;
      return target;
    } break;
    case compiler_ast_nodes_ASTType_FormatStringLiteral: {
      return compiler_passes_typechecker_TypeChecker_check_format_string(this, node);
    } break;
    case compiler_ast_nodes_ASTType_CharLiteral: {
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Char, node->span);
    } break;
    case compiler_ast_nodes_ASTType_BoolLiteral: {
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
    } break;
    case compiler_ast_nodes_ASTType_UnaryOp: {
      switch (node->u.unary.op) {
        case compiler_ast_operators_Operator_PreIncrement:
        case compiler_ast_operators_Operator_PostIncrement:
        case compiler_ast_operators_Operator_PreDecrement:
        case compiler_ast_operators_Operator_PostDecrement: {
          compiler_types_Type *lhs = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.unary.expr, NULL);
          if (!((bool)lhs)) {
          return NULL;
          }
          if (!compiler_types_Type_is_integer(lhs)) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot increment or decrement non-integer type: %s", compiler_types_Type_str(lhs))));
            return NULL;
          }
          if (!compiler_ast_nodes_AST_is_lvalue(node->u.unary.expr)) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Can't perform %s on a non-lvalue", compiler_ast_operators_Operator_dbg(node->u.unary.op))));
          }
          return lhs;
        } break;
        case compiler_ast_operators_Operator_Negate: {
          if (!(((bool)hint) && compiler_types_Type_is_numeric(hint))) {
            hint=compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_I32, node->span);
          }
          compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.unary.expr, hint);
          if (!((bool)typ)) {
          return NULL;
          }
          if (!compiler_types_Type_is_numeric(typ)) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot negate non-numeric type: %s", compiler_types_Type_str(typ))));
            return NULL;
          }
          return typ;
        } break;
        case compiler_ast_operators_Operator_BitwiseNot: {
          compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.unary.expr, hint);
          if (!((bool)typ)) {
          return NULL;
          }
          if (!compiler_types_Type_is_integer(typ)) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot do bitwise-not on non-integer type: %s", compiler_types_Type_str(typ))));
            return NULL;
          }
          return typ;
        } break;
        case compiler_ast_operators_Operator_IsNotNull: {
          compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.unary.expr, NULL);
          if (!((bool)typ)) {
          return NULL;
          }
          typ=compiler_types_Type_unaliased(typ);
          if (typ->base != compiler_types_BaseType_Pointer) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Can only use ? on pointer types, got %s", compiler_types_Type_str(typ))));
          }
          return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
        } break;
        case compiler_ast_operators_Operator_Not: {
          compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.unary.expr, compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span));
          if (!((bool)typ)) {
          return NULL;
          }
          if (typ->base != compiler_types_BaseType_Bool) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot negate non-boolean type: %s", compiler_types_Type_str(typ))));
            return NULL;
          }
          return typ;
        } break;
        case compiler_ast_operators_Operator_Address: {
          compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.unary.expr, NULL);
          if (!((bool)typ)) {
          return NULL;
          }
          switch (typ->base) {
            case compiler_types_BaseType_Char: {
              return compiler_passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span);
            } break;
            case compiler_types_BaseType_Void: {
              return compiler_passes_typechecker_TypeChecker_get_type_by_name(this, "untyped_ptr", node->span);
            } break;
            default: {
              compiler_types_Type *ptr = compiler_types_Type_new_resolved(compiler_types_BaseType_Pointer, node->span);
              ptr->u.ptr=typ;
              return ptr;
            } break;
          }
        } break;
        case compiler_ast_operators_Operator_Dereference: {
          compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.unary.expr, NULL);
          if (!((bool)typ)) {
          return NULL;
          }
          if (typ->base != compiler_types_BaseType_Pointer) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot dereference non-pointer type: %s", compiler_types_Type_str(typ))));
            return NULL;
          }
          return typ->u.ptr;
        } break;
        default: {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Unknown unary operator in check_expression: %s", compiler_ast_operators_Operator_dbg(node->u.unary.op))));
          return NULL;
        } break;
      }
    } break;
    case compiler_ast_nodes_ASTType_Member: {
      return compiler_passes_typechecker_TypeChecker_check_member(this, node, false);
    } break;
    case compiler_ast_nodes_ASTType_SizeOf: {
      compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_resolve_type(this, node->u.size_of_type, false, true, true);
      if (!((bool)typ)) {
      return NULL;
      }
      node->u.size_of_type=typ;
      return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_U32, node->span);
    } break;
    case compiler_ast_nodes_ASTType_If: {
      compiler_passes_typechecker_TypeChecker_check_if(this, node, true, hint);
      return node->etype;
    } break;
    case compiler_ast_nodes_ASTType_Block: {
      compiler_passes_typechecker_TypeChecker_check_block(this, node, true, hint);
      return node->etype;
    } break;
    case compiler_ast_nodes_ASTType_Match: {
      compiler_passes_typechecker_TypeChecker_check_match(this, node, true, hint);
      return node->etype;
    } break;
    case compiler_ast_nodes_ASTType_Identifier:
    case compiler_ast_nodes_ASTType_NSLookup:
    case compiler_ast_nodes_ASTType_Specialization: {
      compiler_ast_scopes_Symbol *item = compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node, true, hint, true);
      if (!((bool)item)) {
      return NULL;
      }
      item=compiler_ast_scopes_Symbol_remove_alias(item);
      switch (item->type) {
        case compiler_ast_scopes_SymbolType_Function: {
          return item->u.func->type;
        } break;
        case compiler_ast_scopes_SymbolType_Variable:
        case compiler_ast_scopes_SymbolType_Constant:
        case compiler_ast_scopes_SymbolType_EnumVariant: {
          return item->u.var->type;
        } break;
        case compiler_ast_scopes_SymbolType_TypeDef: {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot use type `%s` as an expression", item->name)));
          return NULL;
        } break;
        case compiler_ast_scopes_SymbolType_Structure:
        case compiler_ast_scopes_SymbolType_Enum:
        case compiler_ast_scopes_SymbolType_Namespace: {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot use %s `%s` as an expression", compiler_ast_scopes_SymbolType_dbg(item->type), item->name)));
          return NULL;
        } break;
      }
    } break;
    case compiler_ast_nodes_ASTType_Call: {
      return compiler_passes_typechecker_TypeChecker_check_call(this, node);
    } break;
    case compiler_ast_nodes_ASTType_BinaryOp: {
      compiler_ast_nodes_AST *lhs_node = node->u.binary.lhs;
      compiler_ast_nodes_AST *rhs_node = node->u.binary.rhs;
      compiler_ast_operators_Operator op = node->u.binary.op;
      switch (op) {
        case compiler_ast_operators_Operator_Plus:
        case compiler_ast_operators_Operator_Minus:
        case compiler_ast_operators_Operator_Multiply:
        case compiler_ast_operators_Operator_Divide:
        case compiler_ast_operators_Operator_LessThan:
        case compiler_ast_operators_Operator_LessThanEquals:
        case compiler_ast_operators_Operator_GreaterThan:
        case compiler_ast_operators_Operator_GreaterThanEquals:
        case compiler_ast_operators_Operator_Equals:
        case compiler_ast_operators_Operator_NotEquals:
        case compiler_ast_operators_Operator_PlusEquals:
        case compiler_ast_operators_Operator_MinusEquals:
        case compiler_ast_operators_Operator_MultiplyEquals:
        case compiler_ast_operators_Operator_DivideEquals:
        case compiler_ast_operators_Operator_And:
        case compiler_ast_operators_Operator_Or:
        case compiler_ast_operators_Operator_Modulus:
        case compiler_ast_operators_Operator_BitwiseOr:
        case compiler_ast_operators_Operator_BitwiseAnd:
        case compiler_ast_operators_Operator_BitwiseXor:
        case compiler_ast_operators_Operator_LeftShiftEquals:
        case compiler_ast_operators_Operator_RightShiftEquals:
        case compiler_ast_operators_Operator_LeftShift:
        case compiler_ast_operators_Operator_RightShift: {
          compiler_types_Type *lhs = compiler_passes_typechecker_TypeChecker_check_expression(this, lhs_node, NULL);
          compiler_types_Type *rhs = compiler_passes_typechecker_TypeChecker_check_expression(this, rhs_node, lhs);
          if (!((bool)lhs) || !((bool)rhs)) {
          return NULL;
          }
          return compiler_passes_typechecker_TypeChecker_check_binary_op(this, node, compiler_types_Type_unaliased(lhs), compiler_types_Type_unaliased(rhs));
        } break;
        case compiler_ast_operators_Operator_In: {
          compiler_types_Type *lhs = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.binary.lhs, NULL);
          compiler_types_Type *rhs = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.binary.rhs, NULL);
          if (!((bool)lhs) || !((bool)rhs)) {
          return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
          }
          compiler_passes_typechecker_TypeChecker_find_and_replace_overloaded_op(this, compiler_ast_operators_Operator_In, node, node->u.binary.rhs, node->u.binary.lhs, NULL);
          return compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
        } break;
        case compiler_ast_operators_Operator_Index: {
          return compiler_passes_typechecker_TypeChecker_check_index(this, node, hint, false);
        } break;
        case compiler_ast_operators_Operator_Assignment: {
          compiler_types_Type *lhs = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.binary.lhs, NULL);
          compiler_types_Type *rhs = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.binary.rhs, lhs);
          if (!((bool)lhs) || !((bool)rhs)) {
          return NULL;
          }
          return compiler_passes_typechecker_TypeChecker_check_assignment(this, node, lhs, rhs);
        } break;
        case compiler_ast_operators_Operator_IndexAssign: {
          compiler_ast_nodes_AST *index = node->u.binary.lhs;
          compiler_ast_nodes_AST *arg1 = index->u.binary.lhs;
          compiler_ast_nodes_AST *arg2 = index->u.binary.rhs;
          compiler_ast_nodes_AST *arg3 = node->u.binary.rhs;
          if (!((bool)compiler_passes_typechecker_TypeChecker_check_expression(this, arg1, NULL))) {
          return NULL;
          }
          if (!((bool)compiler_passes_typechecker_TypeChecker_check_expression(this, arg2, NULL))) {
          return NULL;
          }
          compiler_types_Type *arg3_hint = NULL;
          compiler_types_Type *arg1_typ = compiler_types_Type_unaliased(arg1->etype);
          if (arg1_typ->base==compiler_types_BaseType_Pointer) {
            arg3_hint=arg1_typ->u.ptr;
          }
          if (!((bool)compiler_passes_typechecker_TypeChecker_check_expression(this, arg3, arg3_hint))) {
          return NULL;
          }
          compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_find_and_replace_overloaded_op(this, compiler_ast_operators_Operator_IndexAssign, node, arg1, arg2, arg3);
          if (((bool)res)) {
          return res;
          }
          compiler_types_Type *lhs = compiler_passes_typechecker_TypeChecker_check_index(this, index, NULL, true);
          compiler_types_Type *rhs = compiler_passes_typechecker_TypeChecker_check_expression(this, arg3, lhs);
          if (!((bool)lhs) || !((bool)rhs)) {
          return NULL;
          }
          return compiler_passes_typechecker_TypeChecker_check_assignment(this, node, lhs, rhs);
        } break;
        default: std_panic(format_string("Internal error: unhandled op in check_expression binary_op: %s", compiler_ast_nodes_ASTType_dbg(node->type))); break;
      }
    } break;
    case compiler_ast_nodes_ASTType_ArrayLiteral: {
      compiler_types_Type *hint_elem_type = ((compiler_types_Type *)NULL);
      if (((bool)hint) && hint->base==compiler_types_BaseType_Array) {
        hint_elem_type=hint->u.arr.elem_type;
      }
      compiler_types_Type *elem_type = ((compiler_types_Type *)NULL);
      std_span_Span first_span = std_span_Span_default();
      for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(node->u.array_literal.elements); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
        compiler_ast_nodes_AST *elem = std_vector_Iterator__13_cur(&__iter);
        {
          compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_expression(this, elem, hint_elem_type);
          if (!((bool)typ)) {
          continue;
          }
          if (!((bool)elem_type)) {
            elem_type=typ;
            hint_elem_type=elem_type;
            first_span=elem->span;
          } else if (!compiler_types_Type_eq(elem_type, typ, false)) {
            compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_hint(elem->span, format_string("Expected type %s, but got %s", compiler_types_Type_str(elem_type), compiler_types_Type_str(typ)), first_span, format_string("First element was of type %s", compiler_types_Type_str(elem_type))));
            return NULL;
          }
        }
      }
      if (!((bool)elem_type)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Array literal must have at least one element"));
        return NULL;
      }
      compiler_types_Type *arr = compiler_types_Type_new_resolved(compiler_types_BaseType_Array, node->span);
      arr->u.arr.elem_type=elem_type;
      arr->u.arr.size_known=true;
      arr->u.arr.size=node->u.array_literal.elements->size;
      return arr;
    } break;
    default: {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Invalid expression in TypeChecker::check_expression: %s", compiler_ast_nodes_ASTType_dbg(node->type))));
      return NULL;
    } break;
  }
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_call_dbg_on_enum_value(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST **node_ptr) {
  compiler_ast_nodes_AST *node = (*node_ptr);
  if (!((bool)node->etype)) {
  return NULL;
  }
  if (node->etype->base != compiler_types_BaseType_Enum) {
  return node->etype;
  }
  compiler_ast_nodes_AST *member = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Member, node->span);
  member->u.member.lhs=node;
  member->u.member.rhs_name="dbg";
  member->u.member.rhs_span=node->span;
  compiler_ast_nodes_AST *call = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Call, node->span);
  call->u.call.callee=member;
  call->u.call.args=std_vector_Vector__15_new(16);
  (*node_ptr)=call;
  return compiler_passes_typechecker_TypeChecker_check_expression(this, call, NULL);
}

void compiler_passes_typechecker_TypeChecker_check_match_for_enum(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Enum *enum_, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint) {
  std_map_Map__8 *mapping = std_map_Map__8_new(32);
  std_vector_Vector__18 *cases = node->u.match_stmt.cases;
  node->returns=(cases->size > 0);
  for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(cases); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
    compiler_ast_nodes_MatchCase *_case = std_vector_Iterator__18_cur(&__iter);
    {
      compiler_ast_nodes_AST *cond = _case->cond;
      char *name;
      compiler_types_Type *cond_type = compiler_passes_typechecker_TypeChecker_check_expression(this, cond, enum_->type);
      if (!((bool)cond_type)) {
      continue;
      }
      if (!compiler_types_Type_eq(cond_type, enum_->type, false)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_hint(cond->span, "Condition does not match expression type", node->u.match_stmt.expr->span, format_string("Match expression is of type '%s'", compiler_types_Type_str(enum_->type))));
      }
      if (cond_type->base != compiler_types_BaseType_Enum) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(cond->span, "Expected an enum value"));
        continue;
      }
      name=cond->resolved_symbol->name;
      compiler_ast_nodes_MatchCase *prev = std_map_Map__8_get(mapping, name, NULL);
      if (((bool)prev)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_hint(cond->span, "Duplicate condition name in match", prev->cond->span, "This condition was previously used here"));
      }
      std_map_Map__8_insert(mapping, name, _case);
      if (((bool)_case->body)) {
        compiler_passes_typechecker_TypeChecker_check_expression_statement(this, node, _case->body, is_expr, hint);
      }
    }
  }
  compiler_ast_nodes_AST *defolt = node->u.match_stmt.defolt;
  if (mapping->size != enum_->fields->size) {
    std_buffer_Buffer buf = std_buffer_Buffer_make(16);
    std_buffer_Buffer_write_str(&buf, "Remaining fields: ");
    bool first = true;
    for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
      compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
      {
        if (!std_map_Map__8_contains(mapping, field->sym->name)) {
          if (!first) {
          std_buffer_Buffer_write_str(&buf, " | ");
          }
          std_buffer_Buffer_write_str(&buf, field->sym->name);
          first=false;
        }
      }
    }
    if (!((bool)defolt)) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(node->u.match_stmt.expr->span, format_string("Match does not cover all cases (Only %u of %u)", mapping->size, enum_->fields->size), std_buffer_Buffer_str(buf)));
    } else {
      compiler_passes_typechecker_TypeChecker_check_expression_statement(this, node, defolt, is_expr, hint);
    }
  } else {
    if (((bool)defolt)) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->u.match_stmt.defolt_span, "`else` case is not needed for this match"));
    }
  }
  if (is_expr && !((bool)node->etype)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->u.match_stmt.match_span, "Expression-match must yield a value"));
  }
  /* defers */
  std_map_Map__8_free(mapping);
}

void compiler_passes_typechecker_TypeChecker_check_match_for_bool(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint) {
  compiler_ast_nodes_Match *match_stmt = &node->u.match_stmt;
  if ((match_stmt->cases->size != 2) || ((bool)match_stmt->defolt)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(match_stmt->match_span, "Match for bool must have exactly `true` and `false` cases"));
  }
  bool seen_true = false;
  bool seen_false = false;
  node->returns=true;
  for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(match_stmt->cases); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
    compiler_ast_nodes_MatchCase *_case = std_vector_Iterator__18_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_expression(this, _case->cond, NULL);
      if (_case->cond->type != compiler_ast_nodes_ASTType_BoolLiteral) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(_case->cond->span, "Expected either `true` or `false`"));
      } else {
        if (_case->cond->u.bool_literal) {
          seen_true=true;
        } else {
          seen_false=true;
        }
      }
      if (((bool)_case->body)) {
        compiler_passes_typechecker_TypeChecker_check_expression_statement(this, node, _case->body, is_expr, hint);
      } else {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(_case->cond->span, "Case must have a body"));
      }
    }
  }
  if (!seen_true) {
  compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(match_stmt->match_span, "Missing `true` case"));
  }
  if (!seen_false) {
  compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(match_stmt->match_span, "Missing `false` case"));
  }
  if (is_expr && !((bool)node->etype)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(match_stmt->match_span, "Expression-match must yield a value"));
  }
}

compiler_ast_nodes_Function *compiler_passes_typechecker_TypeChecker_check_match_case_and_find_overload(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *expr, compiler_ast_nodes_MatchCase *mcase) {
  compiler_types_Type *lhs = expr->etype;
  compiler_types_Type *rhs = mcase->cond->etype;
  if (compiler_types_Type_eq(lhs, rhs, false)) {
    if (compiler_types_Type_is_numeric_or_char(lhs)) {
    return NULL;
    }
    if (lhs->base==compiler_types_BaseType_Bool) {
    return NULL;
    }
  }
  compiler_ast_operators_OperatorOverload overload = {0};
  overload.op=compiler_ast_operators_Operator_Equals;
  overload.type1=lhs;
  overload.type2=rhs;
  compiler_ast_nodes_Function *func = std_map_Map__5_get(this->o->program->operator_overloads, overload, NULL);
  if (!((bool)func)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_hint(mcase->cond->span, format_string("Cannot match %s with this case: %s", compiler_types_Type_str(lhs), compiler_types_Type_str(rhs)), expr->span, format_string("Match expression is of type %s", compiler_types_Type_str(lhs))));
    return NULL;
  }
  compiler_types_Type *ret = compiler_types_Type_unaliased(func->return_type);
  if (ret->base != compiler_types_BaseType_Bool) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_hint(mcase->cond->span, format_string("Overload %s must return a boolean", func->sym->display), func->sym->span, format_string("Defined here, return type is %s", compiler_types_Type_str(ret))));
    return NULL;
  }
  mcase->cmp_fn=func;
  return func;
}

void compiler_passes_typechecker_TypeChecker_check_match(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint) {
  compiler_ast_nodes_Match *match_stmt = &node->u.match_stmt;
  compiler_ast_nodes_AST *expr = match_stmt->expr;
  compiler_types_Type *expr_type = compiler_passes_typechecker_TypeChecker_check_expression(this, expr, NULL);
  if (!((bool)expr_type)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Match statement must have a valid expression"));
    return;
  }
  switch (expr_type->base) {
    case compiler_types_BaseType_Enum: {
      compiler_passes_typechecker_TypeChecker_check_match_for_enum(this, expr_type->u.enum_, node, is_expr, hint);
      return;
    } break;
    case compiler_types_BaseType_Bool: {
      compiler_passes_typechecker_TypeChecker_check_match_for_bool(this, node, is_expr, hint);
      return;
    } break;
    default: {
    } break;
  }
  std_vector_Vector__18 *cases = match_stmt->cases;
  node->returns=(cases->size > 0);
  for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(cases); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
    compiler_ast_nodes_MatchCase *_case = std_vector_Iterator__18_cur(&__iter);
    {
      compiler_types_Type *cond_type = compiler_passes_typechecker_TypeChecker_check_expression(this, _case->cond, expr_type);
      if (!((bool)cond_type)) {
      continue;
      }
      compiler_ast_nodes_Function *cmp_fn = compiler_passes_typechecker_TypeChecker_check_match_case_and_find_overload(this, expr, _case);
      if (((bool)cmp_fn)) {
        match_stmt->is_custom_match=true;
      } else {
        bool is_constant = (((bool)_case->cond->resolved_symbol) && _case->cond->resolved_symbol->type==compiler_ast_scopes_SymbolType_Constant);
        if (((_case->cond->type != compiler_ast_nodes_ASTType_IntLiteral) && (_case->cond->type != compiler_ast_nodes_ASTType_CharLiteral)) && !is_constant) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(_case->cond->span, "Match condition must use only literals"));
        }
      }
      if (((bool)_case->body)) {
        compiler_passes_typechecker_TypeChecker_check_expression_statement(this, node, _case->body, is_expr, hint);
      }
    }
  }
  compiler_ast_nodes_AST *defolt = node->u.match_stmt.defolt;
  if (!((bool)defolt)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->u.match_stmt.match_span, "`else` case is missing"));
  } else {
    compiler_passes_typechecker_TypeChecker_check_expression_statement(this, node, defolt, is_expr, hint);
  }
  if ((is_expr && !((bool)node->etype)) && !node->returns) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Expression-match must yield a value"));
  }
}

void compiler_passes_typechecker_TypeChecker_check_if(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_expr, compiler_types_Type *hint) {
  node->returns=true;
  for (std_vector_Iterator__17 __iter = std_vector_Vector__17_iter(node->u.if_stmt.branches); std_vector_Iterator__17_has_value(&__iter); std_vector_Iterator__17_next(&__iter)) {
    compiler_ast_nodes_IfBranch branch = std_vector_Iterator__17_cur(&__iter);
    {
      compiler_types_Type *cond_type = compiler_passes_typechecker_TypeChecker_check_expression(this, branch.cond, compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span));
      if (((bool)cond_type) && (cond_type->base != compiler_types_BaseType_Bool)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(branch.cond->span, "Condition must be a boolean", format_string("Got type '%s'", compiler_types_Type_str(cond_type))));
      }
      compiler_passes_typechecker_TypeChecker_check_expression_statement(this, node, branch.body, is_expr, hint);
    }
  }
  if (((bool)node->u.if_stmt.els)) {
    compiler_ast_nodes_AST *else_stmt = node->u.if_stmt.els;
    compiler_passes_typechecker_TypeChecker_check_expression_statement(this, node, else_stmt, is_expr, hint);
  } else if (is_expr) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "If expressions must have an else branch"));
  } else {
    node->returns=false;
  }
}

void compiler_passes_typechecker_TypeChecker_check_expression_statement(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_AST *body, bool is_expr, compiler_types_Type *hint) {
  switch (body->type) {
    case compiler_ast_nodes_ASTType_Match: {
      compiler_passes_typechecker_TypeChecker_check_match(this, body, is_expr, hint);
    } break;
    case compiler_ast_nodes_ASTType_If: {
      compiler_passes_typechecker_TypeChecker_check_if(this, body, is_expr, hint);
    } break;
    case compiler_ast_nodes_ASTType_Block: {
      compiler_passes_typechecker_TypeChecker_check_block(this, body, is_expr, hint);
    } break;
    default: {
      if (is_expr) {
        compiler_passes_typechecker_TypeChecker_check_expression(this, body, hint);
      } else {
        compiler_passes_typechecker_TypeChecker_check_statement(this, body);
      }
    } break;
  }
  node->returns=(node->returns && body->returns);
  if (!is_expr) {
  return;
  }
  compiler_types_Type *ret = body->etype;
  if (body->returns) {
  } else if (!((bool)ret)) {
    std_span_Span start_span = (std_span_Span){.start=body->span.start, .end=body->span.start};
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(start_span, format_string("Must yield a value in this branch, body type is %s", compiler_ast_nodes_ASTType_dbg(body->type))));
  } else if (!((bool)node->etype)) {
    node->etype=ret;
  } else if (!compiler_types_Type_eq(node->etype, ret, false)) {
    compiler_ast_nodes_AST *yield_stmt = (node->type==compiler_ast_nodes_ASTType_Block ? node->u.block.final_stmt : node);
    if (!((bool)yield_stmt)) {
    yield_stmt=node;
    }
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(yield_stmt->span, "Yield type of branch doesn't match previous branches", format_string("Expected type '%s', got '%s'", compiler_types_Type_str(node->etype), compiler_types_Type_str(ret))));
  }
}

void compiler_passes_typechecker_TypeChecker_check_while(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node) {
  compiler_ast_scopes_Scope *scope = compiler_ast_scopes_Scope_new(compiler_passes_typechecker_TypeChecker_scope(this));
  scope->loop_count+=1;
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, scope);
  compiler_ast_nodes_AST *cond = node->u.loop.cond;
  compiler_ast_nodes_AST *body = node->u.loop.body;
  compiler_types_Type *cond_type = compiler_passes_typechecker_TypeChecker_check_expression(this, cond, compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span));
  if (((bool)cond_type) && (cond_type->base != compiler_types_BaseType_Bool)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(cond->span, "Condition must be a boolean", format_string("Got type '%s'", compiler_types_Type_str(cond_type))));
  }
  compiler_passes_typechecker_TypeChecker_check_statement(this, body);
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

void compiler_passes_typechecker_TypeChecker_check_for(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node) {
  compiler_ast_scopes_Scope *scope = compiler_ast_scopes_Scope_new(compiler_passes_typechecker_TypeChecker_scope(this));
  scope->loop_count+=1;
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, scope);
  compiler_ast_nodes_AST *init = node->u.loop.init;
  compiler_ast_nodes_AST *cond = node->u.loop.cond;
  compiler_ast_nodes_AST *step = node->u.loop.step;
  compiler_ast_nodes_AST *body = node->u.loop.body;
  if (((bool)init)) {
  compiler_passes_typechecker_TypeChecker_check_statement(this, init);
  }
  if (((bool)cond)) {
    compiler_types_Type *cond_type = compiler_passes_typechecker_TypeChecker_check_expression(this, cond, compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span));
    if (((bool)cond_type) && (cond_type->base != compiler_types_BaseType_Bool)) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(cond->span, "Condition must be a boolean", format_string("Got type '%s'", compiler_types_Type_str(cond_type))));
    }
  }
  if (((bool)step)) {
  compiler_passes_typechecker_TypeChecker_check_expression(this, step, NULL);
  }
  if (((bool)body)) {
  compiler_passes_typechecker_TypeChecker_check_statement(this, body);
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

void compiler_passes_typechecker_TypeChecker_check_statement(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node) {
  switch (node->type) {
    case compiler_ast_nodes_ASTType_Return: {
      compiler_ast_nodes_Function *cur_func = compiler_passes_typechecker_TypeChecker_scope(this)->cur_func;
      if (!((bool)cur_func)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Cannot return from outside a function"));
        return;
      }
      compiler_types_Type *expected = cur_func->return_type;
      compiler_types_Type *res = NULL;
      if (((bool)node->u.child)) {
      res=compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.child, expected);
      }
      if (expected->base==compiler_types_BaseType_Void) {
        if (((bool)node->u.child)) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Cannot return a value from a void function"));
        }
      } else if (((bool)node->u.child)) {
        if (((bool)res) && !compiler_types_Type_eq(res, expected, false)) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Return type %s does not match function return type %s", compiler_types_Type_str(res), compiler_types_Type_str(expected))));
        }
      } else {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Expected a return value for non-void function"));
      }
      node->returns=true;
    } break;
    case compiler_ast_nodes_ASTType_Assert: {
      compiler_ast_nodes_AST *expr = node->u.assertion.expr;
      compiler_types_Type *expr_typ = compiler_passes_typechecker_TypeChecker_check_expression(this, expr, compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span));
      if (((bool)expr_typ) && (expr_typ->base != compiler_types_BaseType_Bool)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Can only assert boolean types, got %s", compiler_types_Type_str(expr_typ))));
      }
      if (((bool)node->u.assertion.msg)) {
        compiler_types_Type *msg_typ = compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.assertion.msg, NULL);
        if (((bool)msg_typ) && (msg_typ != compiler_passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span))) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Can only assert strings, got %s", compiler_types_Type_str(msg_typ))));
        }
      }
      if (expr->type==compiler_ast_nodes_ASTType_BoolLiteral && expr->u.bool_literal==false) {
        node->returns=true;
      }
    } break;
    case compiler_ast_nodes_ASTType_Defer: {
      compiler_passes_typechecker_TypeChecker_check_statement(this, node->u.child);
    } break;
    case compiler_ast_nodes_ASTType_Yield: {
      if (!compiler_passes_typechecker_TypeChecker_scope(this)->can_yield) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Cannot yield here"));
      }
      node->etype=compiler_passes_typechecker_TypeChecker_check_expression(this, node->u.child, NULL);
    } break;
    case compiler_ast_nodes_ASTType_Import: {
      compiler_passes_typechecker_TypeChecker_handle_import_statement(this, node);
    } break;
    case compiler_ast_nodes_ASTType_Break:
    case compiler_ast_nodes_ASTType_Continue: {
      if (compiler_passes_typechecker_TypeChecker_scope(this)->loop_count==0) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("%s statement outside of loop", compiler_ast_nodes_ASTType_dbg(node->type))));
      }
    } break;
    case compiler_ast_nodes_ASTType_If: {
      compiler_passes_typechecker_TypeChecker_check_if(this, node, false, NULL);
    } break;
    case compiler_ast_nodes_ASTType_While: {
      compiler_passes_typechecker_TypeChecker_check_while(this, node);
    } break;
    case compiler_ast_nodes_ASTType_For: {
      compiler_passes_typechecker_TypeChecker_check_for(this, node);
    } break;
    case compiler_ast_nodes_ASTType_Block: {
      compiler_passes_typechecker_TypeChecker_check_block(this, node, false, NULL);
    } break;
    case compiler_ast_nodes_ASTType_Match: {
      compiler_passes_typechecker_TypeChecker_check_match(this, node, false, NULL);
    } break;
    case compiler_ast_nodes_ASTType_VarDeclaration: {
      compiler_ast_nodes_Variable *var = node->u.var_decl.var;
      compiler_ast_scopes_Symbol *res = compiler_ast_scopes_Scope_lookup_local(compiler_passes_typechecker_TypeChecker_scope(this), var->sym->name);
      if (((bool)res)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Variable %s already exists in this scope", var->sym->name)));
        return;
      }
      compiler_ast_scopes_Symbol *sym = var->sym;
      compiler_ast_scopes_Scope_insert(compiler_passes_typechecker_TypeChecker_scope(this), var->sym->name, sym);
      bool is_inferred = var->type==NULL;
      if (is_inferred) {
        var->type=compiler_types_Type_new_unresolved("<inferred>", node->span);
      } else {
        var->type=compiler_passes_typechecker_TypeChecker_resolve_type(this, var->type, false, true, true);
        if (!((bool)var->type)) {
        return;
        }
      }
      compiler_ast_nodes_AST *init = node->u.var_decl.init;
      if (((bool)init)) {
        compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_check_expression(this, init, var->type);
        if (!((bool)res)) {
        return;
        }
        if (is_inferred) {
          var->type=res;
        } else if (!compiler_types_Type_eq(res, var->type, false)) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(init->span, format_string("Variable %s has type %s but initializer has type %s", var->sym->name, compiler_types_Type_str(var->type), compiler_types_Type_str(res))));
        }
      } else if (is_inferred) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Variable %s has no type and no initializer", var->sym->name)));
      }
    } break;
    default: {
      compiler_passes_typechecker_TypeChecker_check_expression(this, node, NULL);
    } break;
  }
}

void compiler_passes_typechecker_TypeChecker_check_function(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *func) {
  compiler_passes_typechecker_TypeChecker_resolve_doc_links(this, func->sym);
  bool is_templated = false;
  if (func->is_method && func->parent_type->base==compiler_types_BaseType_Structure) {
    compiler_ast_nodes_Structure *struc = func->parent_type->u.struc;
    if (compiler_ast_scopes_Symbol_is_templated(struc->sym)) {
    is_templated=true;
    }
  }
  if (compiler_ast_scopes_Symbol_is_templated(func->sym)) {
  is_templated=true;
  }
  if (func->checked) {
  return;
  }
  func->checked=true;
  compiler_ast_scopes_Scope *new_scope = compiler_ast_scopes_Scope_new(func->scope);
  std_vector_Vector__4 *params = func->params;
  bool was_in_template_instance = this->in_template_instance;
  this->in_template_instance=(this->in_template_instance || compiler_ast_nodes_Function_is_template_instance(func));
  u32 error_count_before = this->o->program->errors->size;
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
    {
      if (!((bool)param->type)) {
      continue;
      }
      compiler_ast_nodes_AST *default_expr = param->default_value;
      if (((bool)default_expr)) {
        compiler_types_Type *default_type = compiler_passes_typechecker_TypeChecker_check_expression(this, default_expr, param->type);
        if (((bool)default_type) && !compiler_types_Type_eq(default_type, param->type, false)) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(default_expr->span, format_string("Default argument has type %s but expected %s", compiler_types_Type_str(default_type), compiler_types_Type_str(param->type))));
        }
      }
      std_map_Map__4_insert(new_scope->items, param->sym->name, param->sym);
    }
  }
  new_scope->cur_func=func;
  if (func->sym->is_extern) {
  /* defers */
  this->in_template_instance=was_in_template_instance;
  return;
  }
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, new_scope);
  compiler_passes_typechecker_TypeChecker_check_statement(this, func->body);
  if ((!func->body->returns && (func->return_type->base != compiler_types_BaseType_Void)) && !str_eq(func->sym->full_name, "main")) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(func->sym->span, "Function does not always return"));
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
  if (is_templated) {
    if (this->o->program->errors->size > error_count_before) {
      this->o->program->errors->size=error_count_before;
    }
  }
  /* defers */
  this->in_template_instance=was_in_template_instance;
}

void compiler_passes_typechecker_TypeChecker_handle_imports(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns, bool is_global) {
  compiler_passes_generic_pass_GenericPass_push_namespace(this->o, ns);
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_handle_imports(this, child, false);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->imports); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *import_ = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_handle_import_statement(this, import_);
    }
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
  compiler_passes_generic_pass_GenericPass_pop_namespace(this->o);
}

void compiler_passes_typechecker_TypeChecker_check_globals(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_const) {
  compiler_ast_nodes_Variable *var = node->u.var_decl.var;
  compiler_passes_typechecker_TypeChecker_resolve_doc_links(this, var->sym);
  compiler_ast_nodes_AST *init = node->u.var_decl.init;
  if (is_const) {
    if (((bool)init)) {
      compiler_passes_typechecker_TypeChecker_check_const_expression(this, init, NULL);
    } else if (!node->u.var_decl.var->sym->is_extern) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Constant must have an initializer"));
    }
  } else if (((bool)init)) {
    compiler_types_Type *init_type = compiler_passes_typechecker_TypeChecker_check_expression(this, init, var->type);
    if (!((bool)init_type) || !((bool)var->type)) {
    return;
    }
    if (!compiler_types_Type_eq(init_type, var->type, false)) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(init->span, format_string("Variable %s has type %s but got %s", var->sym->name, compiler_types_Type_str(var->type), compiler_types_Type_str(init_type))));
    }
  }
}

void compiler_passes_typechecker_TypeChecker_check_namespace(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns) {
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  compiler_passes_generic_pass_GenericPass_push_namespace(this->o, ns);
  compiler_passes_typechecker_TypeChecker_resolve_doc_links(this, ns->sym);
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_function(this, func);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_globals(this, node, true);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_globals(this, node, false);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    compiler_ast_nodes_Enum *enum_ = std_vector_Iterator__12_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_resolve_doc_links(this, enum_->sym);
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
        {
          compiler_passes_typechecker_TypeChecker_resolve_doc_links(this, field->sym);
        }
      }
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_namespace(this, child);
    }
  }
  compiler_passes_generic_pass_GenericPass_pop_namespace(this->o);
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

void compiler_passes_typechecker_TypeChecker_resolve_doc_links(compiler_passes_typechecker_TypeChecker *this, compiler_ast_scopes_Symbol *sym) {
  if (!((bool)sym->comment)) {
  return;
  }
  std_buffer_Buffer buffer = std_buffer_Buffer_make(16);
  u32 prev = 0;
  char *doc = sym->comment;
  u32 doc_len = strlen(doc);
  for (u32 i = 0; i < (doc_len - 1); i++) {
    if (doc[i]=='\\') {
      i++;
    } else if (doc[i]==doc[(i + 1)] && doc[(i + 1)]=='{') {
      std_buffer_Buffer_write_str_f(&buffer, str_substring(doc, prev, (i - prev)));
      prev=(i + 2);
      i++;
    } else if (doc[i]==doc[(i + 1)] && doc[(i + 1)]=='}') {
      char *part = str_substring(doc, prev, (i - prev));
      if (!this->o->program->check_doc_links) {
        std_buffer_Buffer_write_char(&buffer, '`');
        std_buffer_Buffer_write_str_f(&buffer, part);
        std_buffer_Buffer_write_char(&buffer, '`');
        prev=(i + 2);
        i++;
        continue;
      }
      compiler_lexer_Lexer lexer = compiler_lexer_Lexer_make(part, sym->span.start.filename);
      std_span_Span span = (std_span_Span){.start=sym->comment_loc, .end=sym->comment_loc};
      std_vector_Vector__9 *tokens = compiler_lexer_Lexer_lex(&lexer);
      if (lexer.errors->size > 0) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(span, format_string("Invalid link: '%s' in this doc", part)));
        return;
      }
      compiler_parser_Parser parser = compiler_parser_Parser_make(this->o->program, compiler_passes_generic_pass_GenericPass_ns(this->o));
      parser.tokens=tokens;
      parser.curr=0;
      compiler_ast_nodes_AST *ident = compiler_parser_Parser_parse_scoped_identifier(&parser, true);
      if (!((bool)ident)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(span, format_string("Invalid link: '%s' in this doc", part)));
        return;
      }
      compiler_ast_scopes_Symbol *sym = compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(this, ident, false, NULL, true);
      if (!((bool)sym)) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(span, format_string("Couldn't find symbol '%s' in this doc link", part)));
        return;
      }
      char *linked_part = ({ char *__yield_0;
        switch (sym->type) {
          case compiler_ast_scopes_SymbolType_Structure: {
            compiler_types_Type *typ = sym->u.struc->type;
            if (((bool)typ->template_instance)) {
              compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_note(span, format_string("Cannot link directly to specialezed type '%s'", part), "Try doing `{{A}}<{{B}}>` instead of `{{A<B>}}`"));
              return;
            }
            __yield_0 = format_string("%x", sym->u.struc->type);
          } break;
          case compiler_ast_scopes_SymbolType_Function: {
            __yield_0 = format_string("%x", sym->u.func);
          } break;
          case compiler_ast_scopes_SymbolType_Enum: {
            __yield_0 = format_string("%x", sym->u.enum_->type);
          } break;
          case compiler_ast_scopes_SymbolType_TypeDef: {
            __yield_0 = format_string("%x", sym->u.type_def);
          } break;
          case compiler_ast_scopes_SymbolType_Variable:
          case compiler_ast_scopes_SymbolType_EnumVariant: {
            __yield_0 = format_string("%x", sym->u.var);
          } break;
          case compiler_ast_scopes_SymbolType_Constant: {
            __yield_0 = format_string("%x", sym->u.var);
          } break;
          case compiler_ast_scopes_SymbolType_Namespace: {
            __yield_0 = format_string("%x", sym->u.ns);
          } break;
        }
;__yield_0; });
      std_buffer_Buffer_write_str(&buffer, "{{");
      std_buffer_Buffer_write_str_f(&buffer, linked_part);
      std_buffer_Buffer_write_str(&buffer, "}}");
      std_mem_free(part);
      prev=(i + 2);
      i++;
    }
  }
  std_buffer_Buffer_write_str_f(&buffer, str_substring(doc, prev, doc_len));
  sym->comment=std_buffer_Buffer_str(buffer);
}

compiler_types_Type *compiler_passes_typechecker_TypeChecker_check_const_expression(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_types_Type *hint) {
  compiler_types_Type *typ = ({ compiler_types_Type *__yield_0;
    switch (node->type) {
      case compiler_ast_nodes_ASTType_Identifier:
      case compiler_ast_nodes_ASTType_NSLookup: {
        compiler_ast_scopes_Symbol *sym = compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node, true, hint, true);
        if (!((bool)sym)) {
        return NULL;
        }
        if (sym->type != compiler_ast_scopes_SymbolType_Constant) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_hint(node->span, "Cannot use a non-constant value in a constant expression", sym->span, "Value was defined here"));
          return NULL;
        }
        __yield_0 = sym->u.var->type;
      } break;
      case compiler_ast_nodes_ASTType_IntLiteral: {
        __yield_0 = compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_I32, node->span);
      } break;
      case compiler_ast_nodes_ASTType_FloatLiteral: {
        __yield_0 = compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_F32, node->span);
      } break;
      case compiler_ast_nodes_ASTType_BoolLiteral: {
        __yield_0 = compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Bool, node->span);
      } break;
      case compiler_ast_nodes_ASTType_CharLiteral: {
        __yield_0 = compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Char, node->span);
      } break;
      case compiler_ast_nodes_ASTType_StringLiteral: {
        __yield_0 = compiler_passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span);
      } break;
      case compiler_ast_nodes_ASTType_BinaryOp: {
        compiler_types_Type *lhs = compiler_passes_typechecker_TypeChecker_check_const_expression(this, node->u.binary.lhs, NULL);
        compiler_types_Type *rhs = compiler_passes_typechecker_TypeChecker_check_const_expression(this, node->u.binary.rhs, NULL);
        if (!((bool)lhs) || !((bool)rhs)) {
        return NULL;
        }
        if (lhs->base==compiler_types_BaseType_Pointer || rhs->base==compiler_types_BaseType_Pointer) {
          compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Cannot do pointer arithmetic in constant expressions"));
        }
        __yield_0 = compiler_passes_typechecker_TypeChecker_check_binary_op(this, node, compiler_types_Type_unaliased(lhs), compiler_types_Type_unaliased(rhs));
      } break;
      case compiler_ast_nodes_ASTType_UnaryOp: {
        __yield_0 = ({ compiler_types_Type *__yield_1;
          switch (node->u.unary.op) {
            case compiler_ast_operators_Operator_BitwiseNot:
            case compiler_ast_operators_Operator_Negate: {
              if (!((bool)hint) || !compiler_types_Type_is_numeric(hint)) {
                hint=compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_I32, node->span);
              }
              compiler_types_Type *typ = compiler_passes_typechecker_TypeChecker_check_const_expression(this, node->u.unary.expr, hint);
              if (!((bool)typ)) {
              return NULL;
              }
              if (!compiler_types_Type_is_numeric(typ)) {
                compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Cannot negate non-numeric type: %s", compiler_types_Type_str(typ))));
                return NULL;
              }
              __yield_1 = typ;
            } break;
            default: {
              compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("Unsupported operator in constant expression: %s", compiler_ast_operators_Operator_dbg(node->u.unary.op))));
              return NULL;
            } break;
          }
;__yield_1; });
      } break;
      default: {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Unsupported operator in constant expression"));
        return NULL;
      } break;
    }
;__yield_0; });
  node->etype=typ;
  return typ;
}

void compiler_passes_typechecker_TypeChecker_handle_import_path_base(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, compiler_ast_nodes_Import *imp, std_vector_Vector__5 *parts, compiler_ast_scopes_Symbol *base, char *alias, i32 start_idx) {
  for (u32 i = ((u32)start_idx); i < parts->size; i+=1) {
    compiler_ast_nodes_ImportPart *part = std_vector_Vector__5_at(parts, i);
    switch (part->type) {
      case compiler_ast_nodes_ImportPartType_Wildcard: {
        compiler_passes_generic_pass_GenericPass_import_all_from_symbol(this->o, base, imp->export);
        return;
      } break;
      case compiler_ast_nodes_ImportPartType_Multiple: {
        std_vector_Vector__16 *paths = part->u.paths;
        for (u32 j = 0; j < paths->size; j+=1) {
          std_vector_Vector__5 *path = std_vector_Vector__16_at(paths, j);
          compiler_passes_typechecker_TypeChecker_handle_import_path_base(this, node, imp, path, base, alias, 0);
        }
        return;
      } break;
      case compiler_ast_nodes_ImportPartType_Single: {
      } break;
    }
    char *name = part->u.single.name;
    if (str_eq(name, "this")) {
      if ((i + 1) != parts->size) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(part->span, "`this` can only be used as the last part of an import path"));
        return;
      }
      if (((bool)part->u.single.alias)) {
      alias=part->u.single.alias;
      }
      break;
    }
    compiler_ast_scopes_Symbol *new_base = compiler_passes_generic_pass_GenericPass_lookup_in_symbol(this->o, base, name, part->u.single.alias_span, false);
    if (!((bool)new_base)) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(part->span, format_string("Invalid import, namespace %s does not exist", name)));
      return;
    }
    base=new_base;
    alias=part->u.single.alias;
    if (!((bool)alias)) {
    alias=name;
    }
    part->resolved_symbol=base;
    compiler_ast_scopes_Symbol_add_reference(base, compiler_ast_scopes_ReferenceType_Normal, part->u.single.name_span);
  }
  compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, base, alias);
  if (imp->export) {
    std_map_Map__4_insert(compiler_passes_generic_pass_GenericPass_ns(this->o)->exported_symbols, alias, base);
  }
}

void compiler_passes_typechecker_TypeChecker_handle_import_statement(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_Import path = node->u.import_path;
  compiler_ast_nodes_ImportPart *part = std_vector_Vector__5_at(path.parts, 0);
  if (part->type != compiler_ast_nodes_ImportPartType_Single) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(part->span, "Invalid import, first part must be a single identifier"));
    return;
  }
  char *name = part->u.single.name;
  char *alias = part->u.single.alias;
  if (!((bool)alias)) {
  alias=name;
  }
  compiler_ast_scopes_Symbol *base = ({ compiler_ast_scopes_Symbol *__yield_0;
    switch (path.type) {
      case compiler_ast_nodes_ImportType_GlobalNamespace: {
        __yield_0 = compiler_ast_program_Namespace_find_importable_symbol(this->o->program->global, name);
      } break;
      case compiler_ast_nodes_ImportType_ProjectNamespace: {
        compiler_ast_program_Namespace *project_root = compiler_ast_program_Namespace_get_project_root(compiler_passes_generic_pass_GenericPass_ns(this->o), node->span, this->o->program);
        if (!((bool)project_root)) {
        return;
        }
        __yield_0 = compiler_ast_program_Namespace_find_importable_symbol(project_root, name);
      } break;
      case compiler_ast_nodes_ImportType_ParentNamespace: {
        compiler_ast_program_Namespace *cur = compiler_passes_generic_pass_GenericPass_ns(this->o);
        for (u32 i = 0; i < path.parent_count; i+=1) {
          if (!((bool)cur->parent)) {
            compiler_errors_Error_panic(compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, "Cannot import from parent of root namespace")));
            return;
          }
          cur=cur->parent;
        }
        __yield_0 = compiler_ast_program_Namespace_find_importable_symbol(cur, name);
      } break;
      case compiler_ast_nodes_ImportType_CurrentScope: {
        __yield_0 = compiler_ast_scopes_Scope_lookup_recursive(compiler_passes_generic_pass_GenericPass_scope(this->o), name);
      } break;
    }
;__yield_0; });
  if (!((bool)base)) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(part->span, format_string("Couldn't import %s", name)));
    return;
  }
  compiler_passes_typechecker_TypeChecker_set_resolved_symbol(this, node, base);
  compiler_passes_typechecker_TypeChecker_handle_import_path_base(this, node, &path, path.parts, base, alias, 1);
}

void compiler_passes_typechecker_TypeChecker_pre_check_function(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns, compiler_ast_nodes_Function *func) {
  if (func->is_method) {
    std_span_Span parent_span = func->parent_type->span;
    compiler_types_Type *parent_type = compiler_passes_typechecker_TypeChecker_resolve_type(this, func->parent_type, true, true, true);
    if (!((bool)parent_type)) {
      compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new(parent_span, "Could not find this type"));
      return;
    }
    if (!compiler_types_Type_can_have_methods(parent_type)) {
      compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new(parent_span, "This type cannot have methods"));
      return;
    }
    switch (parent_type->base) {
      case compiler_types_BaseType_Structure: {
        compiler_ast_nodes_Structure *struc = parent_type->u.struc;
        compiler_ast_nodes_Variable *name = compiler_ast_nodes_Structure_get_field(struc, func->sym->name);
        if (((bool)name)) {
          compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new_hint(func->sym->span, "Field with this name already exists", name->sym->span, "Previous definition here"));
        }
      } break;
      case compiler_types_BaseType_Enum: {
        compiler_ast_nodes_Enum *enum_ = parent_type->u.enum_;
        compiler_ast_nodes_Variable *name = compiler_ast_nodes_Enum_get_field(enum_, func->sym->name);
        if (((bool)name)) {
          compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new_hint(func->sym->span, "Enum value with this name already exists", name->sym->span, "Previous definition here"));
        }
      } break;
      default: {
      } break;
    }
    compiler_ast_nodes_Function *res = std_map_Map__7_get(parent_type->methods, func->sym->name, NULL);
    if (((bool)res)) {
      compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new_hint(func->sym->span, "Method with this name already exists", res->sym->span, "Previous definition here"));
      return;
    }
    compiler_ast_scopes_Symbol_update_parent(func->sym, parent_type->sym);
    std_map_Map__7_insert(parent_type->methods, func->sym->name, func);
    func->parent_type=parent_type;
  } else {
    compiler_ast_scopes_Symbol *item = func->sym;
    compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, item, NULL);
  }
  func->scope=compiler_passes_typechecker_TypeChecker_scope(this);
}

void compiler_passes_typechecker_TypeChecker_loosely_resolve_templated_struct(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Structure *struc) {
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_resolve_type(this, field->type, true, false, false);
      field->type=res;
    }
  }
}

void compiler_passes_typechecker_TypeChecker_resolve_struct(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Structure *struc) {
  std_vector_Vector__4 *fields = struc->fields;
  compiler_passes_typechecker_TypeChecker_resolve_doc_links(this, struc->sym);
  if (compiler_ast_scopes_Symbol_is_templated(struc->sym)) {
    compiler_passes_typechecker_TypeChecker_loosely_resolve_templated_struct(this, struc);
    return;
  }
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_resolve_type(this, field->type, false, true, true);
      compiler_passes_typechecker_TypeChecker_resolve_doc_links(this, field->sym);
      if (!((bool)res)) {
        compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new(field->sym->span, "Couldn't resolve type"));
      } else {
        field->type=res;
      }
    }
  }
}

void compiler_passes_typechecker_TypeChecker_check_operator_overload_function(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *func, compiler_ast_operators_Operator op) {
  u32 num_params_needed = compiler_ast_operators_Operator_num_overload_params(op);
  if (num_params_needed != func->params->size) {
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(func->sym->span, format_string("Operator overload for %s must have %u parameters", compiler_ast_operators_Operator_dbg(op), num_params_needed)));
    return;
  }
  if (compiler_ast_operators_Operator_needs_lhs_pointer_for_overload(op)) {
    compiler_ast_nodes_Variable *lhs = std_vector_Vector__4_at(func->params, 0);
    compiler_types_Type *lhs_type = compiler_types_Type_unaliased(lhs->type);
    if (lhs_type->base != compiler_types_BaseType_Pointer) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(lhs->sym->span, format_string("First parameter of %s operator must be a pointer-type", compiler_ast_operators_Operator_dbg(op))));
      return;
    }
  }
  switch (op) {
    case compiler_ast_operators_Operator_Equals:
    case compiler_ast_operators_Operator_NotEquals:
    case compiler_ast_operators_Operator_IsNotNull: {
      if (func->return_type->base != compiler_types_BaseType_Bool) {
        compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(func->sym->span, "`==` operator must return a boolean"));
        return;
      }
    } break;
    default: {
    } break;
  }
  compiler_ast_operators_OperatorOverload overload = {0};
  overload.op=op;
  if (num_params_needed > 0) {
  overload.type1=std_vector_Vector__4_at(func->params, 0)->type;
  }
  if (num_params_needed > 1) {
  overload.type2=std_vector_Vector__4_at(func->params, 1)->type;
  }
  if (num_params_needed > 2) {
  overload.type3=std_vector_Vector__4_at(func->params, 2)->type;
  }
  std_map_Item__5 *it = std_map_Map__5_get_item(this->o->program->operator_overloads, overload);
  if (((bool)it)) {
    if (it->value != func) {
      compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new_hint(func->sym->span, format_string("Operator overload for %s already exists (%s)", compiler_ast_operators_Operator_dbg(op), func->sym->display), it->value->sym->span, format_string("Previous definition here (%s)", it->value->sym->display)));
    }
    return;
  }
  std_map_Map__5_insert(this->o->program->operator_overloads, overload, func);
}

void compiler_passes_typechecker_TypeChecker_check_function_declaration(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_Function *func) {
  bool allow_incomplete = false;
  bool error = true;
  bool resolve_templates = true;
  if (func->is_method && func->parent_type->base==compiler_types_BaseType_Structure) {
    compiler_ast_nodes_Structure *struc = func->parent_type->u.struc;
    if (compiler_ast_scopes_Symbol_is_templated(struc->sym)) {
      allow_incomplete=true;
      error=false;
      resolve_templates=false;
    }
  }
  if (compiler_ast_scopes_Symbol_is_templated(func->sym)) {
    allow_incomplete=true;
    error=false;
    resolve_templates=false;
  }
  func->return_type=compiler_passes_typechecker_TypeChecker_resolve_type(this, func->return_type, allow_incomplete, error, resolve_templates);
  if (!((bool)func->return_type)) {
    func->return_type=compiler_passes_typechecker_TypeChecker_get_base_type(this, compiler_types_BaseType_Void, func->sym->span);
  }
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(func->params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
    {
      param->type=compiler_passes_typechecker_TypeChecker_resolve_type(this, param->type, allow_incomplete, error, resolve_templates);
    }
  }
  if (((bool)func->operator_overloads)) {
    for (std_vector_Iterator__14 __iter = std_vector_Vector__14_iter(func->operator_overloads); std_vector_Iterator__14_has_value(&__iter); std_vector_Iterator__14_next(&__iter)) {
      compiler_ast_operators_Operator op = std_vector_Iterator__14_cur(&__iter);
      {
        compiler_passes_typechecker_TypeChecker_check_operator_overload_function(this, func, op);
      }
    }
  }
  if (((bool)func->name_ast) && func->name_ast->type==compiler_ast_nodes_ASTType_NSLookup) {
    compiler_passes_typechecker_TypeChecker_resolve_scoped_identifier(this, func->name_ast->u.lookup.lhs, false, NULL, false);
  }
  compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Function, func->sym->span);
  typ->u.func=(compiler_types_FunctionType){.orig=func, .params=func->params, .return_type=func->return_type, .is_variadic=func->is_variadic};
  func->type=typ;
}

void compiler_passes_typechecker_TypeChecker_try_resolve_typedefs_in_namespace(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns, bool pre_import) {
  for (std_map_Iterator__2 __iter = std_map_Map__2_iter(ns->typedefs); std_map_Iterator__2_has_value(&__iter); std_map_Iterator__2_next(&__iter)) {
    std_map_Item__2 *it = std_map_Iterator__2_cur(&__iter);
    {
      if (compiler_types_Type_is_resolved(it->value)) {
      continue;
      }
      compiler_ast_scopes_Symbol *sym = compiler_ast_scopes_Scope_lookup_recursive(compiler_passes_generic_pass_GenericPass_scope(this->o), it->key);
      ae_assert(((bool)sym), "/Users/mustafa/ocen-lang/ocen/compiler/passes/typechecker.oc:2395:16: Assertion failed: `sym?`", "Should have added the symbol into scope already");
      ae_assert(sym->type==compiler_ast_scopes_SymbolType_TypeDef, "/Users/mustafa/ocen-lang/ocen/compiler/passes/typechecker.oc:2399:16: Assertion failed: `sym.type == TypeDef`", NULL);
      compiler_types_Type *res = compiler_passes_typechecker_TypeChecker_resolve_type(this, it->value, false, !pre_import, true);
      if (!((bool)res)) {
      continue;
      }
      sym->u.type_def->u.ptr=res;
      sym->u.type_def=res;
      it->value=res;
    }
  }
}

void compiler_passes_typechecker_TypeChecker_check_post_import(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns) {
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  compiler_passes_generic_pass_GenericPass_push_namespace(this->o, ns);
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_function_declaration(this, func);
    }
  }
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_resolve_struct(this, struc);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_pre_check_globals(this, node, true);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_pre_check_globals(this, node, false);
    }
  }
  compiler_passes_typechecker_TypeChecker_try_resolve_typedefs_in_namespace(this, ns, false);
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_post_import(this, child);
    }
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
  compiler_passes_generic_pass_GenericPass_pop_namespace(this->o);
}

void compiler_passes_typechecker_TypeChecker_pre_check_globals(compiler_passes_typechecker_TypeChecker *this, compiler_ast_nodes_AST *node, bool is_const) {
  compiler_ast_nodes_Variable *var = node->u.var_decl.var;
  if (!((bool)var->type)) {
    char *c = (is_const ? "Constant" : "Global variable");
    compiler_passes_typechecker_TypeChecker_error(this, compiler_errors_Error_new(node->span, format_string("%s must have a type", c)));
    return;
  }
  compiler_types_Type *type = compiler_passes_typechecker_TypeChecker_resolve_type(this, var->type, false, true, true);
  if (!((bool)type)) {
  return;
  }
  var->type=type;
}

void compiler_passes_typechecker_TypeChecker_check_pre_import(compiler_passes_typechecker_TypeChecker *this, compiler_ast_program_Namespace *ns) {
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_pre_check_function(this, ns, func);
    }
  }
  compiler_passes_typechecker_TypeChecker_try_resolve_typedefs_in_namespace(this, ns, true);
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      compiler_passes_typechecker_TypeChecker_check_pre_import(this, child);
    }
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

void compiler_passes_typechecker_TypeChecker_run(compiler_ast_program_Program *program) {
  compiler_passes_typechecker_TypeChecker pass = (compiler_passes_typechecker_TypeChecker){.o=compiler_passes_generic_pass_GenericPass_new(program), .unchecked_functions=std_vector_Vector__6_new(16), .in_template_instance=false};
  compiler_passes_typechecker_TypeChecker_check_pre_import(&pass, program->global);
  compiler_passes_typechecker_TypeChecker_handle_imports(&pass, program->global, true);
  compiler_passes_typechecker_TypeChecker_check_post_import(&pass, program->global);
  compiler_passes_typechecker_TypeChecker_check_namespace(&pass, program->global);
  while (pass.unchecked_functions->size > 0) {
    compiler_ast_nodes_Function *func = ((compiler_ast_nodes_Function *)std_vector_Vector__6_pop(pass.unchecked_functions));
    compiler_passes_typechecker_TypeChecker_check_function(&pass, func);
  }
}

void compiler_passes_register_types_RegisterTypes_register_struct(compiler_passes_register_types_RegisterTypes *this, compiler_ast_program_Namespace *ns, compiler_ast_nodes_Structure *struc) {
  compiler_ast_scopes_Symbol *item = struc->sym;
  compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, item, NULL);
  compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Structure, struc->sym->span);
  typ->u.struc=struc;
  struc->type=typ;
  typ->sym=struc->sym;
}

void compiler_passes_register_types_RegisterTypes_register_enum(compiler_passes_register_types_RegisterTypes *this, compiler_ast_program_Namespace *ns, compiler_ast_nodes_Enum *enum_) {
  compiler_ast_scopes_Symbol *item = enum_->sym;
  compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, item, NULL);
  compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Enum, enum_->sym->span);
  typ->u.enum_=enum_;
  enum_->type=typ;
  typ->sym=enum_->sym;
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      field->type=typ;
    }
  }
}

void compiler_passes_register_types_RegisterTypes_register_globals(compiler_passes_register_types_RegisterTypes *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_Variable *var = node->u.var_decl.var;
  compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, var->sym, NULL);
}

void compiler_passes_register_types_RegisterTypes_add_dbg_method_for_enum(compiler_passes_register_types_RegisterTypes *this, compiler_ast_nodes_Enum *enum_) {
  std_span_Span span = enum_->sym->span;
  compiler_ast_nodes_Function *func = compiler_ast_nodes_Function_new();
  func->span=std_span_Span_default();
  func->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Function, enum_->sym->ns, enum_->sym, "dbg", span);
  func->sym->u.func=func;
  func->return_type=compiler_ast_program_Program_get_type_by_name(this->o->program, "str", span);
  func->is_method=true;
  func->parent_type=enum_->type;
  compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(enum_->type);
  var->sym=compiler_ast_scopes_Symbol_from_local_variable("this", var, span);
  std_vector_Vector__4_push(func->params, var);
  compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Function, span);
  typ->u.func=(compiler_types_FunctionType){.orig=func, .params=func->params, .return_type=func->return_type, .is_variadic=func->is_variadic};
  func->type=typ;
  std_map_Map__7_insert(enum_->type->methods, "dbg", func);
}

void compiler_passes_register_types_RegisterTypes_register_namespace(compiler_passes_register_types_RegisterTypes *this, compiler_ast_program_Namespace *ns) {
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      compiler_passes_register_types_RegisterTypes_register_struct(this, ns, struc);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    compiler_ast_nodes_Enum *enum_ = std_vector_Iterator__12_cur(&__iter);
    {
      compiler_passes_register_types_RegisterTypes_register_enum(this, ns, enum_);
      compiler_passes_register_types_RegisterTypes_add_dbg_method_for_enum(this, enum_);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_register_types_RegisterTypes_register_globals(this, node);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_passes_register_types_RegisterTypes_register_globals(this, node);
    }
  }
  for (std_map_Iterator__2 __iter = std_map_Map__2_iter(ns->typedefs); std_map_Iterator__2_has_value(&__iter); std_map_Iterator__2_next(&__iter)) {
    std_map_Item__2 *it = std_map_Iterator__2_cur(&__iter);
    {
      compiler_passes_register_types_RegisterTypes_register_alias(this, it->key, it->value);
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      if (child->always_add_to_scope) {
        compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, child->sym, NULL);
      }
      compiler_passes_register_types_RegisterTypes_register_namespace(this, child);
    }
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

void compiler_passes_register_types_RegisterTypes_register_base_type(compiler_passes_register_types_RegisterTypes *this, compiler_types_BaseType base) {
  char *name = compiler_types_BaseType_str(base);
  compiler_ast_scopes_Symbol *sym = compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType_TypeDef, NULL, name, name, name, std_span_Span_default());
  compiler_types_Type *typ = compiler_types_Type_new_resolved(base, std_span_Span_default());
  typ->sym=sym;
  sym->u.type_def=typ;
  std_map_Map__4_insert(this->o->program->global->scope->items, name, sym);
}

void compiler_passes_register_types_RegisterTypes_register_alias(compiler_passes_register_types_RegisterTypes *this, char *name, compiler_types_Type *orig) {
  compiler_ast_scopes_Symbol *sym = compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType_TypeDef, NULL, name, name, name, std_span_Span_default());
  compiler_types_Type *alias = compiler_types_Type_new_resolved(compiler_types_BaseType_Alias, std_span_Span_default());
  alias->name=name;
  alias->u.ptr=orig;
  alias->sym=sym;
  sym->u.type_def=alias;
  compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, sym, name);
}

void compiler_passes_register_types_RegisterTypes_register_builtin_types(compiler_passes_register_types_RegisterTypes *this) {
  for (u32 i = 0; i < ((u32)compiler_types_BaseType_NUM_BASE_TYPES); i+=1) {
    compiler_passes_register_types_RegisterTypes_register_base_type(this, ((compiler_types_BaseType)i));
  }
  compiler_passes_generic_pass_GenericPass_push_scope(this->o, this->o->program->global->scope);
  {
    compiler_types_Type *base = compiler_ast_program_Program_get_base_type(this->o->program, compiler_types_BaseType_Void, std_span_Span_default());
    compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Pointer, std_span_Span_default());
    typ->u.ptr=base;
    compiler_passes_register_types_RegisterTypes_register_alias(this, "untyped_ptr", typ);
  }
  {
    compiler_types_Type *base = compiler_ast_program_Program_get_base_type(this->o->program, compiler_types_BaseType_Char, std_span_Span_default());
    compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Pointer, std_span_Span_default());
    typ->u.ptr=base;
    compiler_passes_register_types_RegisterTypes_register_alias(this, "str", typ);
  }
  compiler_passes_generic_pass_GenericPass_pop_scope(this->o);
}

compiler_passes_register_types_Finder compiler_passes_register_types_Finder_to(compiler_passes_register_types_Finder this, char *name) {
  if (!((bool)this.sym)) {
  return this;
  }
  this.sym=compiler_passes_generic_pass_GenericPass_lookup_in_symbol(this.o, this.sym, name, std_span_Span_default(), false);
  if (((bool)this.sym)) {
    switch (this.sym->type) {
      case compiler_ast_scopes_SymbolType_TypeDef: {
        this.type=this.sym->u.type_def;
      } break;
      case compiler_ast_scopes_SymbolType_Structure: {
        this.type=this.sym->u.struc->type;
      } break;
      case compiler_ast_scopes_SymbolType_Enum: {
        this.type=this.sym->u.enum_->type;
      } break;
      default: {
        this.type=NULL;
      } break;
    }
  }
  return this;
}

void compiler_passes_register_types_RegisterTypes_register_cached_types(compiler_passes_register_types_RegisterTypes *this) {
  compiler_passes_register_types_Finder finder = (compiler_passes_register_types_Finder){.o=this->o, .sym=this->o->program->global->sym, .type=NULL};
  compiler_types_Type *sv_type = compiler_passes_register_types_Finder_to(compiler_passes_register_types_Finder_to(compiler_passes_register_types_Finder_to(finder, "std"), "sv"), "SV").type;
  this->o->program->cached_types.sv_type=sv_type;
  compiler_types_Type *buffer_type = compiler_passes_register_types_Finder_to(compiler_passes_register_types_Finder_to(compiler_passes_register_types_Finder_to(finder, "std"), "buffer"), "Buffer").type;
  this->o->program->cached_types.buffer_type=buffer_type;
}

void compiler_passes_register_types_RegisterTypes_run(compiler_ast_program_Program *program) {
  compiler_passes_register_types_RegisterTypes pass = (compiler_passes_register_types_RegisterTypes){.o=compiler_passes_generic_pass_GenericPass_new(program)};
  compiler_passes_register_types_RegisterTypes_register_builtin_types(&pass);
  compiler_passes_register_types_RegisterTypes_register_namespace(&pass, program->global);
  compiler_passes_register_types_RegisterTypes_register_cached_types(&pass);
}

compiler_passes_mark_dead_code_MarkDeadCode *compiler_passes_mark_dead_code_MarkDeadCode_new(compiler_ast_program_Program *program) {
  compiler_passes_mark_dead_code_MarkDeadCode *pass = std_mem_alloc__1(1);
  (*pass)=(compiler_passes_mark_dead_code_MarkDeadCode){.o=compiler_passes_generic_pass_GenericPass_new(program), .done=std_set_Set__1_new()};
  return pass;
}

void compiler_passes_mark_dead_code_MarkDeadCode_free(compiler_passes_mark_dead_code_MarkDeadCode *this) {
  std_set_Set__1_free(this->done);
  std_mem_free(this->o);
  std_mem_free(this);
}

void compiler_passes_mark_dead_code_MarkDeadCode_mark_sym(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_scopes_Symbol *sym) {
  if (!((bool)sym)) {
  return;
  }
  switch (sym->type) {
    case compiler_ast_scopes_SymbolType_Function: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_function(this, sym->u.func);
    } break;
    case compiler_ast_scopes_SymbolType_Structure: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_struct(this, sym->u.struc);
    } break;
    case compiler_ast_scopes_SymbolType_Enum: {
      sym->u.enum_->is_dead=false;
    } break;
    case compiler_ast_scopes_SymbolType_Constant:
    case compiler_ast_scopes_SymbolType_Variable: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, sym->u.var->type);
    } break;
    default: {
    } break;
  }
}

void compiler_passes_mark_dead_code_MarkDeadCode_mark_function(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_nodes_Function *f) {
  if (!((bool)f) || std_set_Set__1_contains(this->done, ((u64)f))) {
  return;
  }
  std_set_Set__1_add(this->done, ((u64)f));
  f->is_dead=false;
  compiler_passes_mark_dead_code_MarkDeadCode_mark(this, f->body);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(f->params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
    {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, param->type);
    }
  }
}

void compiler_passes_mark_dead_code_MarkDeadCode_mark_type(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_types_Type *typ) {
  if (!((bool)typ)) {
  return;
  }
  switch (typ->base) {
    case compiler_types_BaseType_Pointer:
    case compiler_types_BaseType_Alias: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, typ->u.ptr);
    } break;
    case compiler_types_BaseType_Array: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, typ->u.arr.elem_type);
    } break;
    case compiler_types_BaseType_Function: {
      compiler_types_FunctionType ft = typ->u.func;
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, ft.return_type);
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(ft.params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        compiler_ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, param->type);
        }
      }
    } break;
    default: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_sym(this, typ->sym);
      if (((bool)typ->template_instance)) {
        for (std_vector_Iterator__0 __iter = std_vector_Vector__0_iter(typ->template_instance->args); std_vector_Iterator__0_has_value(&__iter); std_vector_Iterator__0_next(&__iter)) {
          compiler_types_Type *arg = std_vector_Iterator__0_cur(&__iter);
          {
            compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, arg);
          }
        }
      }
    } break;
  }
}

void compiler_passes_mark_dead_code_MarkDeadCode_mark_struct(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_nodes_Structure *s) {
  if (!((bool)s) || std_set_Set__1_contains(this->done, ((u64)s))) {
  return;
  }
  std_set_Set__1_add(this->done, ((u64)s));
  s->is_dead=false;
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(s->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, field->type);
    }
  }
}

void compiler_passes_mark_dead_code_MarkDeadCode_mark(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_nodes_AST *node) {
  if (!((bool)node)) {
  return;
  }
  compiler_passes_mark_dead_code_MarkDeadCode_mark_sym(this, node->resolved_symbol);
  switch (node->type) {
    case compiler_ast_nodes_ASTType_Import:
    case compiler_ast_nodes_ASTType_Break:
    case compiler_ast_nodes_ASTType_Continue:
    case compiler_ast_nodes_ASTType_IntLiteral:
    case compiler_ast_nodes_ASTType_FloatLiteral:
    case compiler_ast_nodes_ASTType_BoolLiteral:
    case compiler_ast_nodes_ASTType_StringLiteral:
    case compiler_ast_nodes_ASTType_CharLiteral:
    case compiler_ast_nodes_ASTType_Null: {
    } break;
    case compiler_ast_nodes_ASTType_Return:
    case compiler_ast_nodes_ASTType_Yield:
    case compiler_ast_nodes_ASTType_Defer: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.child);
    } break;
    case compiler_ast_nodes_ASTType_UnaryOp: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.unary.expr);
    } break;
    case compiler_ast_nodes_ASTType_SizeOf: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, node->u.size_of_type);
    } break;
    case compiler_ast_nodes_ASTType_VarDeclaration: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.var_decl.init);
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, node->u.var_decl.var->type);
    } break;
    case compiler_ast_nodes_ASTType_Block: {
      for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(node->u.block.statements); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
        compiler_ast_nodes_AST *statement = std_vector_Iterator__13_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark(this, statement);
        }
      }
    } break;
    case compiler_ast_nodes_ASTType_If: {
      for (std_vector_Iterator__17 __iter = std_vector_Vector__17_iter(node->u.if_stmt.branches); std_vector_Iterator__17_has_value(&__iter); std_vector_Iterator__17_next(&__iter)) {
        compiler_ast_nodes_IfBranch branch = std_vector_Iterator__17_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark(this, branch.cond);
          compiler_passes_mark_dead_code_MarkDeadCode_mark(this, branch.body);
        }
      }
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.if_stmt.els);
    } break;
    case compiler_ast_nodes_ASTType_Match: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.match_stmt.expr);
      for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(node->u.match_stmt.cases); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
        compiler_ast_nodes_MatchCase *c = std_vector_Iterator__18_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark(this, c->cond);
          compiler_passes_mark_dead_code_MarkDeadCode_mark(this, c->body);
          if (((bool)c->cmp_fn)) {
            compiler_passes_mark_dead_code_MarkDeadCode_mark_function(this, c->cmp_fn);
          }
        }
      }
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.match_stmt.defolt);
    } break;
    case compiler_ast_nodes_ASTType_While:
    case compiler_ast_nodes_ASTType_For: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.loop.init);
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.loop.cond);
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.loop.step);
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.loop.body);
    } break;
    case compiler_ast_nodes_ASTType_Assert: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.assertion.expr);
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.assertion.msg);
    } break;
    case compiler_ast_nodes_ASTType_FormatStringLiteral: {
      for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(node->u.fmt_str.exprs); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
        compiler_ast_nodes_AST *expr = std_vector_Iterator__13_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark(this, expr);
        }
      }
    } break;
    case compiler_ast_nodes_ASTType_Member: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.member.lhs);
    } break;
    case compiler_ast_nodes_ASTType_Identifier:
    case compiler_ast_nodes_ASTType_NSLookup:
    case compiler_ast_nodes_ASTType_Specialization:
    case compiler_ast_nodes_ASTType_OverloadedOperator: {
    } break;
    case compiler_ast_nodes_ASTType_Call: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.call.callee);
      for (std_vector_Iterator__15 __iter = std_vector_Vector__15_iter(node->u.call.args); std_vector_Iterator__15_has_value(&__iter); std_vector_Iterator__15_next(&__iter)) {
        compiler_ast_nodes_Argument *arg = std_vector_Iterator__15_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark(this, arg->expr);
        }
      }
    } break;
    case compiler_ast_nodes_ASTType_BinaryOp: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.binary.lhs);
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.binary.rhs);
    } break;
    case compiler_ast_nodes_ASTType_Cast: {
      compiler_passes_mark_dead_code_MarkDeadCode_mark(this, node->u.cast.lhs);
      compiler_passes_mark_dead_code_MarkDeadCode_mark_type(this, node->u.cast.to);
    } break;
    case compiler_ast_nodes_ASTType_ArrayLiteral: {
      for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(node->u.array_literal.elements); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
        compiler_ast_nodes_AST *expr = std_vector_Iterator__13_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark(this, expr);
        }
      }
    } break;
    default: {
      compiler_passes_generic_pass_GenericPass_error(this->o, compiler_errors_Error_new(node->span, format_string("Unhandled expression type in MarkDeadCode: %s", compiler_ast_nodes_ASTType_dbg(node->type))));
    } break;
  }
}

compiler_ast_nodes_Function *compiler_passes_mark_dead_code_MarkDeadCode_find_main_function(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_program_Program *program) {
  for (compiler_ast_program_NSIterator __iter = compiler_ast_program_Program_iter_namespaces(program); compiler_ast_program_NSIterator_has_value(&__iter); compiler_ast_program_NSIterator_next(&__iter)) {
    compiler_ast_program_Namespace *ns = compiler_ast_program_NSIterator_cur(&__iter);
    {
      for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
        compiler_ast_nodes_Function *f = std_vector_Iterator__6_cur(&__iter);
        {
          if (str_eq(f->sym->full_name, "main")) {
            return f;
          }
        }
      }
    }
  }
  return NULL;
}

void compiler_passes_mark_dead_code_MarkDeadCode_mark_sym_as_dead_by_default(compiler_passes_mark_dead_code_MarkDeadCode *this, compiler_ast_scopes_Symbol *sym) {
  if (!((bool)sym)) {
  return;
  }
  switch (sym->type) {
    case compiler_ast_scopes_SymbolType_Function: {
      sym->u.func->is_dead=true;
    } break;
    case compiler_ast_scopes_SymbolType_Structure: {
      sym->u.struc->is_dead=true;
    } break;
    case compiler_ast_scopes_SymbolType_Enum: {
      sym->u.enum_->is_dead=true;
    } break;
    default: {
    } break;
  }
  if (((bool)sym->template)) {
    for (std_vector_Iterator__3 __iter = std_vector_Vector__3_iter(sym->template->instances); std_vector_Iterator__3_has_value(&__iter); std_vector_Iterator__3_next(&__iter)) {
      compiler_ast_scopes_TemplateInstance *instance = std_vector_Iterator__3_cur(&__iter);
      {
        compiler_passes_mark_dead_code_MarkDeadCode_mark_sym_as_dead_by_default(this, instance->resolved);
      }
    }
  }
}

void compiler_passes_mark_dead_code_MarkDeadCode_run(compiler_ast_program_Program *program) {
  compiler_passes_mark_dead_code_MarkDeadCode *pass = compiler_passes_mark_dead_code_MarkDeadCode_new(program);
  compiler_ast_nodes_Function *main = compiler_passes_mark_dead_code_MarkDeadCode_find_main_function(pass, program);
  if (!((bool)main)) {
    printf("[+] No main function found, not marking any functions as dead.""\n");
    /* defers */
    compiler_passes_mark_dead_code_MarkDeadCode_free(pass);
    return;
  }
  if (program->keep_all_code) {
    /* defers */
    compiler_passes_mark_dead_code_MarkDeadCode_free(pass);
    return;
  }
  for (compiler_ast_program_NSIterator __iter = compiler_ast_program_Program_iter_namespaces(program); compiler_ast_program_NSIterator_has_value(&__iter); compiler_ast_program_NSIterator_next(&__iter)) {
    compiler_ast_program_Namespace *ns = compiler_ast_program_NSIterator_cur(&__iter);
    {
      for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
        compiler_ast_nodes_Function *f = std_vector_Iterator__6_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark_sym_as_dead_by_default(pass, f->sym);
        }
      }
      for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
        compiler_ast_nodes_Structure *s = std_vector_Iterator__2_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark_sym_as_dead_by_default(pass, s->sym);
        }
      }
      for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
        compiler_ast_nodes_Enum *e = std_vector_Iterator__12_cur(&__iter);
        {
          compiler_passes_mark_dead_code_MarkDeadCode_mark_sym_as_dead_by_default(pass, e->sym);
        }
      }
    }
  }
  for (compiler_ast_program_NSIterator __iter = compiler_ast_program_Program_iter_namespaces(program); compiler_ast_program_NSIterator_has_value(&__iter); compiler_ast_program_NSIterator_next(&__iter)) {
    compiler_ast_program_Namespace *ns = compiler_ast_program_NSIterator_cur(&__iter);
    {
      for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
        compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
        {
          compiler_ast_nodes_Variable *var = node->u.var_decl.var;
          compiler_passes_mark_dead_code_MarkDeadCode_mark_type(pass, var->type);
          if (((bool)node->u.var_decl.init)) {
            compiler_passes_mark_dead_code_MarkDeadCode_mark(pass, node->u.var_decl.init);
          }
        }
      }
      for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
        compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
        {
          compiler_ast_nodes_Variable *var = node->u.var_decl.var;
          compiler_passes_mark_dead_code_MarkDeadCode_mark_type(pass, var->type);
          if (((bool)node->u.var_decl.init)) {
            compiler_passes_mark_dead_code_MarkDeadCode_mark(pass, node->u.var_decl.init);
          }
        }
      }
    }
  }
  compiler_passes_mark_dead_code_MarkDeadCode_mark_function(pass, main);
  /* defers */
  compiler_passes_mark_dead_code_MarkDeadCode_free(pass);
}

compiler_passes_generic_pass_GenericPass *compiler_passes_generic_pass_GenericPass_new(compiler_ast_program_Program *program) {
  compiler_passes_generic_pass_GenericPass *pass = std_mem_alloc__2(1);
  pass->scope_stack=std_vector_Vector__7_new(16);
  pass->namespace_stack=std_vector_Vector__8_new(16);
  pass->program=program;
  pass->error_type=compiler_types_Type_new_resolved(compiler_types_BaseType_Error, std_span_Span_default());
  return pass;
}

void compiler_passes_generic_pass_GenericPass_push_scope(compiler_passes_generic_pass_GenericPass *this, compiler_ast_scopes_Scope *scope) {
  std_vector_Vector__7_push(this->scope_stack, scope);
}

void compiler_passes_generic_pass_GenericPass_push_namespace(compiler_passes_generic_pass_GenericPass *this, compiler_ast_program_Namespace *ns) {
  std_vector_Vector__8_push(this->namespace_stack, ns);
}

compiler_ast_scopes_Scope *compiler_passes_generic_pass_GenericPass_pop_scope(compiler_passes_generic_pass_GenericPass *this) {
  if (this->scope_stack->size==0) {
  return NULL;
  }
  return ((compiler_ast_scopes_Scope *)std_vector_Vector__7_pop(this->scope_stack));
}

compiler_ast_program_Namespace *compiler_passes_generic_pass_GenericPass_pop_namespace(compiler_passes_generic_pass_GenericPass *this) {
  if (this->namespace_stack->size==0) {
  return NULL;
  }
  return ((compiler_ast_program_Namespace *)std_vector_Vector__8_pop(this->namespace_stack));
}

compiler_ast_scopes_Scope *compiler_passes_generic_pass_GenericPass_scope(compiler_passes_generic_pass_GenericPass *this) {
  if (this->scope_stack->size==0) {
  return NULL;
  }
  return std_vector_Vector__7_at(this->scope_stack, (this->scope_stack->size - 1));
}

compiler_ast_program_Namespace *compiler_passes_generic_pass_GenericPass_ns(compiler_passes_generic_pass_GenericPass *this) {
  if (this->namespace_stack->size==0) {
  return NULL;
  }
  return std_vector_Vector__8_at(this->namespace_stack, (this->namespace_stack->size - 1));
}

compiler_errors_Error *compiler_passes_generic_pass_GenericPass_error(compiler_passes_generic_pass_GenericPass *this, compiler_errors_Error *err) {
  std_vector_Vector__11_push(this->program->errors, err);
  return err;
}

void compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(compiler_passes_generic_pass_GenericPass *this, bool export, compiler_ast_scopes_Symbol *item, char *name) {
  if (!((bool)name)) {
  name=item->name;
  }
  compiler_ast_scopes_Symbol *prev = compiler_ast_scopes_Scope_lookup_local(compiler_passes_generic_pass_GenericPass_scope(this), name);
  if (((bool)prev)) {
    compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new_hint(item->span, format_string("Name %s already exists in scope", name), prev->span, format_string("Previous use of %s", name)));
    return;
  }
  compiler_ast_scopes_Scope_insert(compiler_passes_generic_pass_GenericPass_scope(this), name, item);
  if (!export) {
  return;
  }
  std_map_Map__4 *exported = compiler_passes_generic_pass_GenericPass_ns(this)->exported_symbols;
  std_map_Item__4 *it = std_map_Map__4_get_item(exported, name);
  if (((bool)it)) {
    compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new_hint(item->span, format_string("Name %s already exported from namespace", name), it->value->span, format_string("Previous export of %s", name)));
    return;
  }
  std_map_Map__4_insert(compiler_passes_generic_pass_GenericPass_ns(this)->exported_symbols, name, item);
}

void compiler_passes_generic_pass_GenericPass_insert_into_scope_checked(compiler_passes_generic_pass_GenericPass *this, compiler_ast_scopes_Symbol *item, char *name) {
  compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, false, item, name);
}

compiler_ast_scopes_Symbol *compiler_passes_generic_pass_GenericPass_lookup_in_symbol(compiler_passes_generic_pass_GenericPass *this, compiler_ast_scopes_Symbol *sym, char *name, std_span_Span span, bool error) {
  return ({ compiler_ast_scopes_Symbol *__yield_0;
    switch (sym->type) {
      case compiler_ast_scopes_SymbolType_Namespace: {
        compiler_ast_program_Namespace *ns = sym->u.ns;
        compiler_ast_scopes_Symbol *res = compiler_ast_program_Namespace_find_importable_symbol(ns, name);
        if (error && !((bool)res)) {
          compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new_hint(span, format_string("Could not find symbol %s in namespace %s", name, sym->display), ns->span, format_string("Namespace was defined here")));
        }
        __yield_0 = res;
      } break;
      case compiler_ast_scopes_SymbolType_Structure: {
        compiler_ast_nodes_Structure *struc = sym->u.struc;
        if (compiler_ast_scopes_Symbol_is_templated(sym)) {
          compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new_hint(span, format_string("Need to specify template specialization for %s", sym->display), sym->span, format_string("Template was defined here")));
        }
        compiler_ast_nodes_Function *method = std_map_Map__7_get(struc->type->methods, name, NULL);
        if (error && !((bool)method)) {
          compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new(span, format_string("Could not find method %s in structure %s", name, sym->display)));
        }
        if (!((bool)method)) {
        return NULL;
        }
        __yield_0 = method->sym;
      } break;
      case compiler_ast_scopes_SymbolType_TypeDef: {
        compiler_types_Type *type_def = sym->u.type_def;
        compiler_ast_nodes_Function *method = std_map_Map__7_get(type_def->methods, name, NULL);
        if (error && !((bool)method)) {
          compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new(span, format_string("Could not find method %s in type %s", name, sym->display)));
        }
        if (!((bool)method)) {
        return NULL;
        }
        __yield_0 = method->sym;
      } break;
      case compiler_ast_scopes_SymbolType_Enum: {
        compiler_ast_nodes_Enum *enum_ = sym->u.enum_;
        compiler_ast_nodes_Variable *field = compiler_ast_nodes_Enum_get_field(enum_, name);
        if (((bool)field)) {
        return field->sym;
        }
        compiler_ast_nodes_Function *method = std_map_Map__7_get(enum_->type->methods, name, NULL);
        if (((bool)method)) {
        return method->sym;
        }
        if (error) {
          compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new_hint(span, format_string("Could not find value/method %s in enum %s", name, sym->display), sym->span, format_string("Enum was defined here")));
        }
        return NULL;
      } break;
      default: {
        compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new_hint(span, format_string("Can't lookup a name inside a %s", compiler_ast_scopes_SymbolType_dbg(sym->type)), sym->span, format_string("%s was defined here", compiler_ast_scopes_SymbolType_dbg(sym->type))));
        return NULL;
      } break;
    }
;__yield_0; });
}

void compiler_passes_generic_pass_GenericPass_import_all_from_namespace(compiler_passes_generic_pass_GenericPass *this, compiler_ast_program_Namespace *ns, bool export) {
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      if (!func->is_method) {
        compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, func->sym, NULL);
      }
    }
  }
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, struc->sym, NULL);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    compiler_ast_nodes_Enum *enum_ = std_vector_Iterator__12_cur(&__iter);
    {
      compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, enum_->sym, NULL);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_ast_nodes_Variable *var = node->u.var_decl.var;
      compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, var->sym, NULL);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_ast_nodes_Variable *var = node->u.var_decl.var;
      compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, var->sym, NULL);
    }
  }
  for (std_map_Iterator__2 __iter = std_map_Map__2_iter(ns->typedefs); std_map_Iterator__2_has_value(&__iter); std_map_Iterator__2_next(&__iter)) {
    std_map_Item__2 *it = std_map_Iterator__2_cur(&__iter);
    {
      compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, it->value->sym, it->key);
    }
  }
  for (std_map_Iterator__4 __iter = std_map_Map__4_iter(ns->exported_symbols); std_map_Iterator__4_has_value(&__iter); std_map_Iterator__4_next(&__iter)) {
    std_map_Item__4 *it = std_map_Iterator__4_cur(&__iter);
    {
      compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, it->value, it->key);
    }
  }
}

void compiler_passes_generic_pass_GenericPass_import_all_from_symbol(compiler_passes_generic_pass_GenericPass *this, compiler_ast_scopes_Symbol *sym, bool export) {
  switch (sym->type) {
    case compiler_ast_scopes_SymbolType_Namespace: {
      compiler_passes_generic_pass_GenericPass_import_all_from_namespace(this, sym->u.ns, export);
    } break;
    case compiler_ast_scopes_SymbolType_TypeDef: {
      for (std_map_ValueIterator__7 __iter = std_map_Map__7_iter_values(sym->u.type_def->methods); std_map_ValueIterator__7_has_value(&__iter); std_map_ValueIterator__7_next(&__iter)) {
        compiler_ast_nodes_Function *method = std_map_ValueIterator__7_cur(&__iter);
        {
          compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, method->sym, NULL);
        }
      }
    } break;
    case compiler_ast_scopes_SymbolType_Structure: {
      for (std_map_ValueIterator__7 __iter = std_map_Map__7_iter_values(sym->u.struc->type->methods); std_map_ValueIterator__7_has_value(&__iter); std_map_ValueIterator__7_next(&__iter)) {
        compiler_ast_nodes_Function *method = std_map_ValueIterator__7_cur(&__iter);
        {
          compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, method->sym, NULL);
        }
      }
    } break;
    case compiler_ast_scopes_SymbolType_Enum: {
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(sym->u.enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
        {
          compiler_passes_generic_pass_GenericPass_insert_into_scope_checked_and_export(this, export, field->sym, NULL);
        }
      }
    } break;
    default: {
      compiler_passes_generic_pass_GenericPass_error(this, compiler_errors_Error_new(sym->span, format_string("Can't imdo wildcard import from a %s", compiler_ast_scopes_SymbolType_dbg(sym->type))));
    } break;
  }
}

compiler_parser_Parser compiler_parser_Parser_make(compiler_ast_program_Program *program, compiler_ast_program_Namespace *ns) {
  return (compiler_parser_Parser){.tokens=NULL, .curr=0, .curr_func=NULL, .program=program, .ns=ns, .attrs=std_vector_Vector__10_new(16), .attrs_span=std_span_Span_default(), .attrs_start_tok=NULL};
}

compiler_tokens_Token *compiler_parser_Parser_peek(compiler_parser_Parser *this, i32 off) {
  i32 idx = (((i32)this->curr) + off);
  ae_assert((0 <= idx) && (idx < ((i32)this->tokens->size)), "/Users/mustafa/ocen-lang/ocen/compiler/parser.oc:53:12: Assertion failed: `0i32 <= idx < (.tokens.size as i32`", NULL);
  return std_vector_Vector__9_at(this->tokens, ((u32)idx));
}

compiler_errors_Error *compiler_parser_Parser_error_msg(compiler_parser_Parser *this, char *msg) {
  compiler_errors_Error *err = compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, msg);
  std_vector_Vector__11_push(this->program->errors, err);
  return err;
}

compiler_errors_Error *compiler_parser_Parser_error(compiler_parser_Parser *this, compiler_errors_Error *err) {
  std_vector_Vector__11_push(this->program->errors, err);
  return err;
}

void compiler_parser_Parser_unhandled_type(compiler_parser_Parser *this, char *func) {
  compiler_parser_Parser_error_msg(this, format_string("Unexpected token in %s: %s", func, compiler_tokens_TokenType_str(compiler_parser_Parser_token(this)->type)));
}

compiler_tokens_Token *compiler_parser_Parser_token(compiler_parser_Parser *this) {
  if (this->curr >= this->tokens->size) {
    this->curr=(this->tokens->size - 1);
    compiler_parser_Parser_error_msg(this, "Unexpected end of file");
    longjmp(this->program->err_jmp_ctx, 1);
  }
  return std_vector_Vector__9_unchecked_at(this->tokens, ((u32)this->curr));
}

bool compiler_parser_Parser_token_is(compiler_parser_Parser *this, compiler_tokens_TokenType type) {
  if (type==compiler_tokens_TokenType_Newline) {
    return compiler_parser_Parser_token(this)->seen_newline;
  }
  return compiler_parser_Parser_token(this)->type==type;
}

bool compiler_parser_Parser_token_is_identifier(compiler_parser_Parser *this, char *name) {
  return compiler_tokens_Token_is_identifier((*compiler_parser_Parser_token(this)), name);
}

bool compiler_parser_Parser_peek_token_is(compiler_parser_Parser *this, u32 off, compiler_tokens_TokenType type) {
  if ((this->curr + off) >= this->tokens->size) {
  return false;
  }
  compiler_tokens_Token *tok = std_vector_Vector__9_at(this->tokens, (this->curr + off));
  return tok->type==type;
}

bool compiler_parser_Parser_consume_if(compiler_parser_Parser *this, compiler_tokens_TokenType type) {
  if (compiler_parser_Parser_token_is(this, type)) {
    if (type != compiler_tokens_TokenType_Newline) {
      if (this->curr < (this->tokens->size - 1)) {
      this->curr+=1;
      }
    }
    return true;
  }
  return false;
}

void compiler_parser_Parser_consume_newline_or(compiler_parser_Parser *this, compiler_tokens_TokenType type) {
  if (compiler_parser_Parser_token_is(this, type)) {
    if (this->curr < (this->tokens->size - 1)) {
    this->curr+=1;
    }
  } else if (!compiler_parser_Parser_token(this)->seen_newline) {
    compiler_parser_Parser_error_msg(this, format_string("Expected %s or newline", compiler_tokens_TokenType_str(type)));
    longjmp(this->program->err_jmp_ctx, 1);
  }
}

compiler_tokens_Token *compiler_parser_Parser_consume(compiler_parser_Parser *this, compiler_tokens_TokenType type) {
  compiler_tokens_Token *tok = compiler_parser_Parser_token(this);
  if (!compiler_parser_Parser_consume_if(this, type)) {
    compiler_parser_Parser_error_msg(this, format_string("Expected TokenType::%s", compiler_tokens_TokenType_str(type)));
    longjmp(this->program->err_jmp_ctx, 1);
  }
  return tok;
}

bool compiler_parser_Parser_is_end_of_statement(compiler_parser_Parser *this) {
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
  return true;
  }
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Semicolon)) {
  return true;
  }
  if (compiler_parser_Parser_token(this)->seen_newline) {
  return true;
  }
  return false;
}

void compiler_parser_Parser_consume_end_of_statement(compiler_parser_Parser *this) {
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
  return;
  }
  compiler_parser_Parser_consume_newline_or(this, compiler_tokens_TokenType_Semicolon);
}

void compiler_parser_Parser_clear_attributes(compiler_parser_Parser *this) {
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(this->attrs); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    compiler_attributes_Attribute *attr = std_vector_Iterator__10_cur(&__iter);
    {
      std_mem_free(attr);
    }
  }
  std_vector_Vector__10_clear(this->attrs);
  this->attrs_start_tok=NULL;
}

bool compiler_parser_Parser_is_compound_operator(compiler_parser_Parser *this, compiler_ast_operators_Operator op) {
  return ({ bool __yield_0;
    switch (op) {
      case compiler_ast_operators_Operator_LeftShift: {
        __yield_0 = (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_LessThan) && compiler_parser_Parser_peek_token_is(this, 1, compiler_tokens_TokenType_LessThan));
      } break;
      case compiler_ast_operators_Operator_RightShift: {
        __yield_0 = (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_GreaterThan) && compiler_parser_Parser_peek_token_is(this, 1, compiler_tokens_TokenType_GreaterThan));
      } break;
      case compiler_ast_operators_Operator_LeftShiftEquals: {
        __yield_0 = (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_LessThan) && compiler_parser_Parser_peek_token_is(this, 1, compiler_tokens_TokenType_LessThanEquals));
      } break;
      case compiler_ast_operators_Operator_RightShiftEquals: {
        __yield_0 = (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_GreaterThan) && compiler_parser_Parser_peek_token_is(this, 1, compiler_tokens_TokenType_GreaterThanEquals));
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

std_span_Span compiler_parser_Parser_consume_compound_operator(compiler_parser_Parser *this, compiler_ast_operators_Operator op) {
  std_span_Span span = compiler_parser_Parser_token(this)->span;
  switch (op) {
    case compiler_ast_operators_Operator_LeftShift: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_LessThan);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_LessThan);
    } break;
    case compiler_ast_operators_Operator_RightShift: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_GreaterThan);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_GreaterThan);
    } break;
    case compiler_ast_operators_Operator_LeftShiftEquals: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_LessThan);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_LessThanEquals);
    } break;
    case compiler_ast_operators_Operator_RightShiftEquals: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_GreaterThan);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_GreaterThanEquals);
    } break;
    default: {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(span, format_string("Unknown operator %s in Parser::consume_compound_operator", compiler_ast_operators_Operator_dbg(op))));
    } break;
  }
  compiler_tokens_Token *prev_token = std_vector_Vector__9_at(this->tokens, (this->curr - 1));
  return std_span_Span_join(span, prev_token->span);
}

compiler_types_Type *compiler_parser_Parser_parse_type(compiler_parser_Parser *this) {
  return ({ compiler_types_Type *__yield_0;
    switch (compiler_parser_Parser_token(this)->type) {
      case compiler_tokens_TokenType_Identifier: {
        compiler_ast_nodes_AST *ident = compiler_parser_Parser_parse_scoped_identifier(this, true);
        char *name = (ident->type==compiler_ast_nodes_ASTType_Identifier ? ident->u.ident.name : "<unresolved>");
        compiler_types_Type *typ = compiler_types_Type_new_unresolved(name, ident->span);
        typ->u.unresolved=ident;
        __yield_0 = typ;
      } break;
      case compiler_tokens_TokenType_Ampersand: {
        compiler_tokens_Token *amp = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Ampersand);
        compiler_types_Type *base = compiler_parser_Parser_parse_type(this);
        compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Pointer, std_span_Span_join(amp->span, base->span));
        typ->u.ptr=base;
        __yield_0 = typ;
      } break;
      case compiler_tokens_TokenType_Fn: {
        std_span_Span start_span = compiler_parser_Parser_token(this)->span;
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Fn);
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenParen);
        std_vector_Vector__4 *params = std_vector_Vector__4_new(16);
        bool is_variadic = false;
        while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseParen)) {
          if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Ellipsis)) {
            compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Ellipsis);
            is_variadic=true;
            break;
          }
          compiler_types_Type *param_type = compiler_parser_Parser_parse_type(this);
          compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(param_type);
          var->sym=compiler_ast_scopes_Symbol_from_local_variable("", var, param_type->span);
          std_vector_Vector__4_push(params, var);
          if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseParen)) {
            compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Comma);
          }
        }
        compiler_tokens_Token *close = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseParen);
        compiler_types_Type *return_type;
        if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Colon)) {
          return_type=compiler_parser_Parser_parse_type(this);
        } else {
          return_type=compiler_types_Type_new_unresolved_base(compiler_types_BaseType_Void, start_span);
        }
        compiler_types_Type *type = compiler_types_Type_new_resolved(compiler_types_BaseType_Function, std_span_Span_join(start_span, close->span));
        type->u.func=(compiler_types_FunctionType){.orig=NULL, .params=params, .return_type=return_type, .is_variadic=is_variadic};
        __yield_0 = type;
      } break;
      case compiler_tokens_TokenType_OpenSquare: {
        std_span_Span start_span = compiler_parser_Parser_token(this)->span;
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenSquare);
        compiler_types_Type *elem_type = compiler_parser_Parser_parse_type(this);
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Semicolon);
        compiler_ast_nodes_AST *size_expr = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_CloseSquare);
        compiler_tokens_Token *close = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseSquare);
        compiler_types_Type *typ = compiler_types_Type_new_resolved(compiler_types_BaseType_Array, std_span_Span_join(start_span, close->span));
        typ->u.arr=(compiler_types_ArrayType){.elem_type=elem_type, .size_expr=size_expr, .size_known=false, .size=0};
        __yield_0 = typ;
      } break;
      default: {
        compiler_parser_Parser_unhandled_type(this, "parse_type");
        __yield_0 = compiler_types_Type_new_unresolved_base(compiler_types_BaseType_Error, compiler_parser_Parser_token(this)->span);
      } break;
    }
;__yield_0; });
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_identifier(compiler_parser_Parser *this) {
  compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Identifier, tok->span);
  node->u.ident.name=tok->text;
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_scoped_identifier(compiler_parser_Parser *this, bool consume_template) {
  compiler_ast_nodes_AST *node = compiler_parser_Parser_parse_identifier(this);
  while (true) {
    switch (compiler_parser_Parser_token(this)->type) {
      case compiler_tokens_TokenType_ColonColon: {
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_ColonColon);
        compiler_ast_nodes_AST *lookup = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_NSLookup, node->span);
        lookup->u.lookup.lhs=node;
        node=lookup;
        if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier)) {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected identifier after `::`"));
          node->span.end=compiler_parser_Parser_token(this)->span.start;
        } else {
          compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
          node->span=std_span_Span_join(node->span, name->span);
          node->u.lookup.rhs_name=name->text;
          node->u.lookup.rhs_span=name->span;
        }
      } break;
      case compiler_tokens_TokenType_LessThan: {
        if (!consume_template) {
        return node;
        }
        compiler_tokens_Token *prev_token = std_vector_Vector__9_at(this->tokens, (((u32)this->curr) - 1));
        if (!std_span_Span_starts_right_after(compiler_parser_Parser_token(this)->span, prev_token->span)) {
          return node;
        }
        compiler_tokens_Token *next_next_token = std_vector_Vector__9_at(this->tokens, (((u32)this->curr) + 2));
        if (next_next_token->type==compiler_tokens_TokenType_Dot) {
          return node;
        }
        compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_LessThan);
        std_vector_Vector__0 *args = std_vector_Vector__0_new(16);
        while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_GreaterThan)) {
          std_vector_Vector__0_push(args, compiler_parser_Parser_parse_type(this));
          if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_GreaterThan)) {
            if (!compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Comma)) {
              compiler_parser_Parser_error(this, compiler_errors_Error_new_note(compiler_parser_Parser_token(this)->span, "Parsing template specialization: expected `,` or `>`", "If you're comparing values, put a space before the `<` earlier"));
              return compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Error, node->span);
            }
          }
        }
        compiler_tokens_Token *end = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_GreaterThan);
        compiler_ast_nodes_AST *spec = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Specialization, std_span_Span_join(node->span, end->span));
        spec->u.spec=(compiler_ast_nodes_Specialization){.base=node, .parsed_template_args=args, .template_args=args};
        node=spec;
      } break;
      default: {
        return node;
      } break;
    }
  }
  return NULL;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_format_string(compiler_parser_Parser *this) {
  compiler_tokens_Token *fstr = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_FormatStringLiteral);
  u32 fstr_len = strlen(fstr->text);
  std_vector_Vector__1 *expr_parts = std_vector_Vector__1_new(16);
  std_vector_Vector__24 *expr_start = std_vector_Vector__24_new(16);
  std_vector_Vector__1 *format_parts = std_vector_Vector__1_new(16);
  std_vector_Vector__1 *specifiers = std_vector_Vector__1_new(16);
  u32 specifier_loc = 0;
  bool specifier_found = false;
  u32 count = 0;
  u32 cur_start = 0;
  for (u32 i = 0; i < fstr_len; i+=1) {
    switch (fstr->text[i]) {
      case '\\': {
        i+=1;
      } break;
      case '{': {
        if (count==0) {
          char *part = str_substring(fstr->text, cur_start, (i - cur_start));
          std_vector_Vector__1_push(format_parts, part);
          cur_start=(i + 1);
        }
        count+=1;
      } break;
      case '}': {
        count-=1;
        if (count==0) {
          if (specifier_loc > 0) {
            u32 len = (specifier_loc - cur_start);
            char *part = str_substring(fstr->text, cur_start, len);
            std_vector_Vector__1_push(expr_parts, part);
            std_vector_Vector__24_push(expr_start, cur_start);
            specifier_loc+=1;
            while ((specifier_loc < i) && fstr->text[specifier_loc]==' ') {
              specifier_loc+=1;
            }
            if (specifier_loc==i) {
              std_span_Location loc = fstr->span.start;
              loc.col+=(specifier_loc + 1);
              std_span_Span span = (std_span_Span){.start=loc, .end=loc};
              compiler_parser_Parser_error(this, compiler_errors_Error_new(span, "Expected format specifier"));
              return NULL;
            }
            char *spec = str_substring(fstr->text, specifier_loc, (i - specifier_loc));
            std_vector_Vector__1_push(specifiers, spec);
          } else {
            char *part = str_substring(fstr->text, cur_start, (i - cur_start));
            std_vector_Vector__1_push(expr_parts, part);
            std_vector_Vector__24_push(expr_start, cur_start);
            std_vector_Vector__1_push(specifiers, NULL);
          }
          cur_start=(i + 1);
          specifier_loc=0;
          specifier_found=false;
        } else if (count < 0) {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(fstr->span, "Unmatched '}' in format string"));
          return NULL;
        }
      } break;
      case ':': {
        if ((count==1 && (fstr->text[(i - 1)] != ':')) && (fstr->text[(i + 1)] != ':')) {
          specifier_loc=i;
          specifier_found=true;
        }
      } break;
      default: {
      } break;
    }
  }
  if (count != 0) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(fstr->span, "Unmatched '{' in format string"));
    return NULL;
  }
  char *part = str_substring(fstr->text, cur_start, (fstr_len - cur_start));
  std_vector_Vector__1_push(format_parts, part);
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_FormatStringLiteral, fstr->span);
  node->u.fmt_str.parts=format_parts;
  std_span_Location fstr_start = fstr->span.start;
  std_vector_Vector__13 *expr_nodes = std_vector_Vector__13_new(16);
  for (u32 i = 0; i < expr_parts->size; i+=1) {
    char *part = std_vector_Vector__1_at(expr_parts, i);
    u32 start = std_vector_Vector__24_at(expr_start, i);
    compiler_lexer_Lexer lexer = compiler_lexer_Lexer_make(part, fstr_start.filename);
    lexer.loc=fstr_start;
    lexer.loc.col+=(start + 1);
    std_vector_Vector__9 *tokens = compiler_lexer_Lexer_lex(&lexer);
    for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(lexer.errors); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
      compiler_errors_Error *error = std_vector_Iterator__11_cur(&__iter);
      {
        compiler_parser_Parser_error(this, error);
      }
    }
    std_vector_Vector__11_free(lexer.errors);
    compiler_parser_Parser sub_parser = compiler_parser_Parser_make(this->program, this->ns);
    sub_parser.tokens=tokens;
    sub_parser.curr=0;
    sub_parser.curr_func=this->curr_func;
    compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_expression(&sub_parser, compiler_tokens_TokenType_CloseCurly);
    if (!compiler_parser_Parser_token_is(&sub_parser, compiler_tokens_TokenType_EOF)) {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(expr->span, "Invalid expression in format string"));
    }
    std_vector_Vector__13_push(expr_nodes, expr);
  }
  node->u.fmt_str.exprs=expr_nodes;
  node->u.fmt_str.specs=specifiers;
  std_vector_Vector__1_free(expr_parts);
  std_vector_Vector__24_free(expr_start);
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_match(compiler_parser_Parser *this) {
  compiler_tokens_Token *op = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Match);
  compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_OpenCurly);
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Match, std_span_Span_join(op->span, expr->span));
  node->u.match_stmt.expr=expr;
  node->u.match_stmt.match_span=op->span;
  std_vector_Vector__18 *cases = std_vector_Vector__18_new(16);
  compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenCurly);
  while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Else)) {
      node->u.match_stmt.defolt_span=compiler_parser_Parser_token(this)->span;
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Else);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_FatArrow);
      node->u.match_stmt.defolt=compiler_parser_Parser_parse_statement(this);
    } else {
      compiler_ast_nodes_AST *cond = compiler_parser_Parser_parse_atom(this, compiler_tokens_TokenType_Line);
      compiler_ast_nodes_AST *body = ((compiler_ast_nodes_AST *)NULL);
      if (!compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Line)) {
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_FatArrow);
        body=compiler_parser_Parser_parse_statement(this);
        if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
          compiler_parser_Parser_consume_newline_or(this, compiler_tokens_TokenType_Comma);
        }
      }
      compiler_ast_nodes_MatchCase *_case = compiler_ast_nodes_MatchCase_new(cond, body);
      std_vector_Vector__18_push(cases, _case);
    }
    compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Comma);
  }
  node->span=std_span_Span_join(op->span, compiler_parser_Parser_token(this)->span);
  compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseCurly);
  node->u.match_stmt.cases=cases;
  return node;
}

compiler_types_Type *compiler_parser_Parser_parse_literal_suffix_type(compiler_parser_Parser *this, compiler_tokens_Token *suffix) {
  if (!((bool)suffix)) {
  return NULL;
  }
  compiler_ast_nodes_AST *ident = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Identifier, suffix->span);
  ident->u.ident.name=suffix->text;
  compiler_types_Type *typ = compiler_types_Type_new_unresolved(suffix->text, suffix->span);
  typ->u.unresolved=ident;
  return typ;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_call(compiler_parser_Parser *this, compiler_ast_nodes_AST *callee) {
  compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenParen);
  std_vector_Vector__15 *args = std_vector_Vector__15_new(16);
  while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseParen)) {
    compiler_tokens_Token *label_tok = NULL;
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier) && compiler_parser_Parser_peek_token_is(this, 1, compiler_tokens_TokenType_Colon)) {
      label_tok=compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Colon);
    }
    compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Comma);
    std_vector_Vector__15_push(args, compiler_ast_nodes_Argument_new(expr, label_tok));
    if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseParen)) {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Comma);
    }
  }
  compiler_tokens_Token *end = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseParen);
  compiler_ast_nodes_ASTType call_type = compiler_ast_nodes_ASTType_Call;
  compiler_ast_nodes_AST *call = compiler_ast_nodes_AST_new(call_type, std_span_Span_join(callee->span, end->span));
  call->u.call.callee=callee;
  call->u.call.args=args;
  call->u.call.open_paren_span=start->span;
  call->u.call.close_paren_span=end->span;
  return call;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_var_initializer(compiler_parser_Parser *this) {
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_OpenSquare)) {
    compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenSquare);
    std_vector_Vector__13 *elements = std_vector_Vector__13_new(16);
    while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseSquare)) {
      std_vector_Vector__13_push(elements, compiler_parser_Parser_parse_var_initializer(this));
      if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseSquare)) {
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Comma);
      }
    }
    compiler_tokens_Token *close = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseSquare);
    compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_ArrayLiteral, std_span_Span_join(start->span, close->span));
    node->u.array_literal.elements=elements;
    return node;
  }
  return compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_var_declaration(compiler_parser_Parser *this) {
  compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Let);
  compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
  std_span_Span end_span = name->span;
  compiler_types_Type *type = ((compiler_types_Type *)NULL);
  if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Colon)) {
    type=compiler_parser_Parser_parse_type(this);
    end_span=type->span;
  }
  compiler_ast_nodes_AST *init = ((compiler_ast_nodes_AST *)NULL);
  if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Equals)) {
    init=compiler_parser_Parser_parse_var_initializer(this);
    end_span=init->span;
  }
  compiler_parser_Parser_consume_end_of_statement(this);
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_VarDeclaration, std_span_Span_join(start->span, end_span));
  compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(type);
  var->sym=compiler_ast_scopes_Symbol_from_local_variable(name->text, var, name->span);
  node->u.var_decl.var=var;
  node->u.var_decl.init=init;
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_global_value(compiler_parser_Parser *this, bool is_const) {
  compiler_tokens_Token *start_token = ({ compiler_tokens_Token *__yield_0;
    if (is_const) {
      __yield_0 = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Const);
    } else {
      __yield_0 = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Let);
    }
;__yield_0; });
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_VarDeclaration, compiler_parser_Parser_token(this)->span);
  compiler_tokens_Token *name = ({ compiler_tokens_Token *__yield_0;
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier)) {
      __yield_0 = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
    } else {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected identifier"));
      return node;
    }
;__yield_0; });
  compiler_types_Type *type = ((compiler_types_Type *)NULL);
  if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Colon)) {
    type=compiler_parser_Parser_parse_type(this);
  }
  compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(type);
  var->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Variable, this->ns, this->ns->sym, name->text, name->span);
  var->sym->u.var=var;
  if (is_const) {
    var->sym->type=compiler_ast_scopes_SymbolType_Constant;
  }
  compiler_parser_Parser_parse_extern_into_symbol(this, var->sym);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(this->attrs); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    compiler_attributes_Attribute *attr = std_vector_Iterator__10_cur(&__iter);
    {
      switch (attr->type) {
        case compiler_attributes_AttributeType_Extern: {
          compiler_parser_Parser_get_extern_from_attr(this, var->sym, attr);
        } break;
        default: {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(attr->span, "Unexpected attribute for variable"));
        } break;
      }
    }
  }
  node->u.var_decl.var=var;
  if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Equals)) {
    if (is_const) {
      node->u.var_decl.init=compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
    } else {
      node->u.var_decl.init=compiler_parser_Parser_parse_var_initializer(this);
    }
  }
  compiler_parser_Parser_consume_newline_or(this, compiler_tokens_TokenType_Semicolon);
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_atom(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *node = ((compiler_ast_nodes_AST *)NULL);
  switch (compiler_parser_Parser_token(this)->type) {
    case compiler_tokens_TokenType_If: {
      node=compiler_parser_Parser_parse_if(this);
    } break;
    case compiler_tokens_TokenType_Match: {
      node=compiler_parser_Parser_parse_match(this);
    } break;
    case compiler_tokens_TokenType_OpenCurly: {
      node=compiler_parser_Parser_parse_block(this);
    } break;
    case compiler_tokens_TokenType_FormatStringLiteral: {
      node=compiler_parser_Parser_parse_format_string(this);
    } break;
    case compiler_tokens_TokenType_Null: {
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Null);
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Null, tok->span);
    } break;
    case compiler_tokens_TokenType_IntLiteral: {
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_IntLiteral, compiler_parser_Parser_token(this)->span);
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_IntLiteral);
      node->u.num_literal=(compiler_ast_nodes_NumLiteral){.text=tok->text, .suffix=compiler_parser_Parser_parse_literal_suffix_type(this, tok->suffix)};
    } break;
    case compiler_tokens_TokenType_FloatLiteral: {
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_FloatLiteral, compiler_parser_Parser_token(this)->span);
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_FloatLiteral);
      node->u.num_literal=(compiler_ast_nodes_NumLiteral){.text=tok->text, .suffix=compiler_parser_Parser_parse_literal_suffix_type(this, tok->suffix)};
    } break;
    case compiler_tokens_TokenType_StringLiteral: {
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_StringLiteral, compiler_parser_Parser_token(this)->span);
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_StringLiteral);
      node->u.string_literal=tok->text;
    } break;
    case compiler_tokens_TokenType_CharLiteral: {
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_CharLiteral, compiler_parser_Parser_token(this)->span);
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CharLiteral);
      node->u.char_literal=tok->text;
    } break;
    case compiler_tokens_TokenType_Identifier: {
      node=compiler_parser_Parser_parse_scoped_identifier(this, true);
    } break;
    case compiler_tokens_TokenType_True:
    case compiler_tokens_TokenType_False: {
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_BoolLiteral, tok->span);
      node->u.bool_literal=tok->type==compiler_tokens_TokenType_True;
    } break;
    case compiler_tokens_TokenType_OpenParen: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenParen);
      node=compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_CloseParen);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseParen);
    } break;
    case compiler_tokens_TokenType_Dot: {
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Dot);
      if ((!((bool)this->curr_func) || !this->curr_func->is_method) || this->curr_func->is_static) {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(tok->span, "Cannot use dot shorthand outside of a method"));
        return compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Error, tok->span);
      }
      compiler_ast_nodes_AST *lhs = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Identifier, tok->span);
      lhs->u.ident.name="this";
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Member, tok->span);
      node->u.member.lhs=lhs;
      node->u.member.dot_shorthand=true;
      if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier)) {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected identifier after `.`"));
        node->span.end=compiler_parser_Parser_token(this)->span.start;
      } else {
        compiler_tokens_Token *ident = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
        node->span=std_span_Span_join(tok->span, ident->span);
        node->u.member.rhs_name=ident->text;
        node->u.member.rhs_span=ident->span;
      }
    } break;
    default: {
      compiler_parser_Parser_unhandled_type(this, "parse_expression");
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Error, compiler_parser_Parser_token(this)->span);
      this->curr+=1;
    } break;
  }
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_postfix(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *node = compiler_parser_Parser_parse_atom(this, end_type);
  bool running = true;
  while (running) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    switch (compiler_parser_Parser_token(this)->type) {
      case compiler_tokens_TokenType_OpenParen: {
        node=compiler_parser_Parser_parse_call(this, node);
      } break;
      case compiler_tokens_TokenType_Dot: {
        if (compiler_parser_Parser_token_is(this, end_type)) {
        break;
        }
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Dot);
        compiler_ast_nodes_AST *member = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Member, node->span);
        member->u.member.lhs=node;
        node=member;
        if (compiler_parser_Parser_token_is(this, end_type) || !compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier)) {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected identifier after `.`"));
          node->span.end=compiler_parser_Parser_token(this)->span.start;
        } else {
          compiler_tokens_Token *ident = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
          node->span=std_span_Span_join(node->span, ident->span);
          node->u.member.rhs_name=ident->text;
          node->u.member.rhs_span=ident->span;
        }
      } break;
      case compiler_tokens_TokenType_Question: {
        compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Question);
        node=compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_IsNotNull, std_span_Span_join(node->span, tok->span), node);
      } break;
      case compiler_tokens_TokenType_OpenSquare: {
        compiler_tokens_Token *open = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenSquare);
        compiler_ast_nodes_AST *index = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_CloseSquare);
        compiler_tokens_Token *close = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseSquare);
        compiler_ast_nodes_AST *op = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_BinaryOp, std_span_Span_join(node->span, close->span));
        op->u.binary.op=compiler_ast_operators_Operator_Index;
        op->u.binary.lhs=node;
        op->u.binary.rhs=index;
        op->u.binary.op_span=open->span;
        node=op;
      } break;
      case compiler_tokens_TokenType_MinusMinus:
      case compiler_tokens_TokenType_PlusPlus: {
        std_span_Span span = std_span_Span_join(node->span, compiler_parser_Parser_token(this)->span);
        compiler_ast_operators_Operator op = ({ compiler_ast_operators_Operator __yield_0;
          if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_MinusMinus)) {
            compiler_parser_Parser_consume(this, compiler_tokens_TokenType_MinusMinus);
            __yield_0 = compiler_ast_operators_Operator_PostDecrement;
          } else {
            compiler_parser_Parser_consume(this, compiler_tokens_TokenType_PlusPlus);
            __yield_0 = compiler_ast_operators_Operator_PostIncrement;
          }
;__yield_0; });
        node=compiler_ast_nodes_AST_new_unop(op, span, node);
      } break;
      default: {
        running=false;
      } break;
    }
  }
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_prefix(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  switch (compiler_parser_Parser_token(this)->type) {
    case compiler_tokens_TokenType_Ampersand: {
      compiler_tokens_Token *amp = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Ampersand);
      compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_prefix(this, end_type);
      compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_Address, std_span_Span_join(amp->span, expr->span), expr);
      return node;
    } break;
    case compiler_tokens_TokenType_MinusMinus:
    case compiler_tokens_TokenType_PlusPlus: {
      std_span_Span start_span = compiler_parser_Parser_token(this)->span;
      compiler_ast_operators_Operator op = ({ compiler_ast_operators_Operator __yield_0;
        if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_MinusMinus)) {
          compiler_parser_Parser_consume(this, compiler_tokens_TokenType_MinusMinus);
          __yield_0 = compiler_ast_operators_Operator_PreDecrement;
        } else {
          compiler_parser_Parser_consume(this, compiler_tokens_TokenType_PlusPlus);
          __yield_0 = compiler_ast_operators_Operator_PreIncrement;
        }
;__yield_0; });
      compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_prefix(this, end_type);
      return compiler_ast_nodes_AST_new_unop(op, std_span_Span_join(start_span, expr->span), expr);
    } break;
    case compiler_tokens_TokenType_SizeOf: {
      compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_SizeOf);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenParen);
      compiler_types_Type *type = compiler_parser_Parser_parse_type(this);
      compiler_tokens_Token *close = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseParen);
      compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_SizeOf, std_span_Span_join(start->span, close->span));
      node->u.size_of_type=type;
      return node;
    } break;
    case compiler_tokens_TokenType_Star: {
      compiler_tokens_Token *star = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Star);
      compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_prefix(this, end_type);
      compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_Dereference, std_span_Span_join(star->span, expr->span), expr);
      return node;
    } break;
    case compiler_tokens_TokenType_Minus: {
      compiler_tokens_Token *minus = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Minus);
      compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_prefix(this, end_type);
      compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_Negate, std_span_Span_join(minus->span, expr->span), expr);
      return node;
    } break;
    case compiler_tokens_TokenType_Tilde: {
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Tilde);
      compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_prefix(this, end_type);
      compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_BitwiseNot, std_span_Span_join(tok->span, expr->span), expr);
      return node;
    } break;
    default: {
      return compiler_parser_Parser_parse_postfix(this, end_type);
    } break;
  }
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_cast(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_prefix(this, end_type);
  while (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_As)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_As);
    compiler_types_Type *type_node = compiler_parser_Parser_parse_type(this);
    compiler_ast_nodes_AST *op = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Cast, std_span_Span_join(lhs->span, type_node->span));
    op->u.cast.lhs=lhs;
    op->u.cast.to=type_node;
    lhs=op;
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_term(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_cast(this, end_type);
  while ((compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Star) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Slash)) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Percent)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    compiler_tokens_Token *op_tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
    compiler_ast_operators_Operator op = compiler_ast_operators_Operator_from_token(op_tok);
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_cast(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_tok->span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_additive(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_term(this, end_type);
  while (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Plus) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Minus)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    compiler_tokens_Token *op_tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
    compiler_ast_operators_Operator op = compiler_ast_operators_Operator_from_token(op_tok);
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_term(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_tok->span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_shift(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_additive(this, end_type);
  while (compiler_parser_Parser_is_compound_operator(this, compiler_ast_operators_Operator_LeftShift) || compiler_parser_Parser_is_compound_operator(this, compiler_ast_operators_Operator_RightShift)) {
    compiler_ast_operators_Operator op = ({ compiler_ast_operators_Operator __yield_0;
      if (compiler_parser_Parser_token(this)->type==compiler_tokens_TokenType_LessThan) {
        __yield_0 = compiler_ast_operators_Operator_LeftShift;
      } else {
        __yield_0 = compiler_ast_operators_Operator_RightShift;
      }
;__yield_0; });
    std_span_Span op_span = compiler_parser_Parser_consume_compound_operator(this, op);
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_additive(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_bw_and(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_shift(this, end_type);
  while (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Ampersand)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    compiler_tokens_Token *op_tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
    compiler_ast_operators_Operator op = compiler_ast_operators_Operator_from_token(op_tok);
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_shift(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_tok->span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_bw_xor(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_bw_and(this, end_type);
  while (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Caret)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    compiler_tokens_Token *op_tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
    compiler_ast_operators_Operator op = compiler_ast_operators_Operator_from_token(op_tok);
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_bw_and(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_tok->span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_bw_or(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_bw_xor(this, end_type);
  while (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Line)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    compiler_tokens_Token *op_tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
    compiler_ast_operators_Operator op = compiler_ast_operators_Operator_from_token(op_tok);
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_bw_xor(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_tok->span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_relational(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  std_vector_Vector__13 *operands = std_vector_Vector__13_new(16);
  std_vector_Vector__9 *operators = std_vector_Vector__9_new(16);
  std_vector_Vector__13_push(operands, compiler_parser_Parser_parse_bw_or(this, end_type));
  while ((((((compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_LessThan) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_GreaterThan)) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_LessThanEquals)) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_GreaterThanEquals)) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_EqualEquals)) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_NotEquals)) || compiler_parser_Parser_token_is_identifier(this, "in")) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    bool done = ({ bool __yield_0;
      switch (compiler_parser_Parser_token(this)->type) {
        case compiler_tokens_TokenType_LessThan: {
          __yield_0 = compiler_parser_Parser_is_compound_operator(this, compiler_ast_operators_Operator_LeftShiftEquals);
        } break;
        case compiler_tokens_TokenType_GreaterThan: {
          __yield_0 = compiler_parser_Parser_is_compound_operator(this, compiler_ast_operators_Operator_RightShiftEquals);
        } break;
        default: {
          __yield_0 = false;
        } break;
      }
;__yield_0; });
    if (done) {
    break;
    }
    compiler_tokens_Token *token = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
    std_vector_Vector__9_push(operators, token);
    compiler_ast_nodes_AST *term = compiler_parser_Parser_parse_bw_or(this, end_type);
    std_vector_Vector__13_push(operands, term);
  }
  if (operators->size==0) {
  return std_vector_Vector__13_at(operands, 0);
  }
  compiler_ast_nodes_AST *root = ((compiler_ast_nodes_AST *)NULL);
  for (u32 i = 0; i < operators->size; i+=1) {
    compiler_tokens_Token *tok = std_vector_Vector__9_at(operators, i);
    compiler_ast_nodes_AST *lhs = std_vector_Vector__13_at(operands, i);
    compiler_ast_nodes_AST *rhs = std_vector_Vector__13_at(operands, (i + 1));
    compiler_ast_nodes_AST *op = compiler_ast_nodes_AST_new_binop(compiler_ast_operators_Operator_from_token(tok), lhs, rhs, tok->span);
    if (((bool)root)) {
      root=compiler_ast_nodes_AST_new_binop(compiler_ast_operators_Operator_And, root, op, tok->span);
    } else {
      root=op;
    }
  }
  std_vector_Vector__13_free(operands);
  std_vector_Vector__9_free(operators);
  return root;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_logical_not(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Not)) {
    compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Not);
    compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_logical_not(this, end_type);
    return compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator_Not, std_span_Span_join(tok->span, expr->span), expr);
  }
  return compiler_parser_Parser_parse_relational(this, end_type);
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_logical_and(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_logical_not(this, end_type);
  while (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_And)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    compiler_tokens_Token *op_tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
    compiler_ast_operators_Operator op = compiler_ast_operators_Operator_from_token(op_tok);
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_logical_not(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_tok->span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_logical_or(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_logical_and(this, end_type);
  while (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Or)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    compiler_tokens_Token *op_tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
    compiler_ast_operators_Operator op = compiler_ast_operators_Operator_from_token(op_tok);
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_logical_and(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_tok->span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_expression(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_ast_nodes_AST *lhs = compiler_parser_Parser_parse_logical_or(this, end_type);
  while ((((((compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Equals) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_PlusEquals)) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_MinusEquals)) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_StarEquals)) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_SlashEquals)) || compiler_parser_Parser_is_compound_operator(this, compiler_ast_operators_Operator_LeftShiftEquals)) || compiler_parser_Parser_is_compound_operator(this, compiler_ast_operators_Operator_RightShiftEquals)) {
    if (compiler_parser_Parser_token_is(this, end_type)) {
    break;
    }
    std_span_Span op_span = {0};
    compiler_ast_operators_Operator op = ({ compiler_ast_operators_Operator __yield_0;
      switch (compiler_parser_Parser_token(this)->type) {
        case compiler_tokens_TokenType_LessThan: {
          op_span=compiler_parser_Parser_consume_compound_operator(this, compiler_ast_operators_Operator_LeftShiftEquals);
          __yield_0 = compiler_ast_operators_Operator_LeftShiftEquals;
        } break;
        case compiler_tokens_TokenType_GreaterThan: {
          op_span=compiler_parser_Parser_consume_compound_operator(this, compiler_ast_operators_Operator_RightShiftEquals);
          __yield_0 = compiler_ast_operators_Operator_RightShiftEquals;
        } break;
        default: {
          compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
          op_span=tok->span;
          __yield_0 = compiler_ast_operators_Operator_from_token(tok);
        } break;
      }
;__yield_0; });
    if ((op==compiler_ast_operators_Operator_Assignment && lhs->type==compiler_ast_nodes_ASTType_BinaryOp) && lhs->u.binary.op==compiler_ast_operators_Operator_Index) {
      op=compiler_ast_operators_Operator_IndexAssign;
    }
    compiler_ast_nodes_AST *rhs = compiler_parser_Parser_parse_expression(this, end_type);
    lhs=compiler_ast_nodes_AST_new_binop(op, lhs, rhs, op_span);
  }
  return lhs;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_multi_if(compiler_parser_Parser *this, compiler_tokens_Token *start_tok) {
  std_span_Span start_span = start_tok->span;
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_If, start_span);
  compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenCurly);
  std_vector_Vector__17 *branches = std_vector_Vector__17_new(16);
  node->u.if_stmt.branches=branches;
  while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Else)) {
      node->u.if_stmt.els_span=compiler_parser_Parser_token(this)->span;
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Else);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_FatArrow);
      node->u.if_stmt.els=compiler_parser_Parser_parse_statement(this);
    } else {
      compiler_ast_nodes_AST *cond = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_FatArrow);
      if (!compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_FatArrow)) {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected `=>` after condition"));
      }
      compiler_ast_nodes_AST *body = compiler_parser_Parser_parse_statement(this);
      std_vector_Vector__17_push(branches, (compiler_ast_nodes_IfBranch){.cond=cond, .body=body});
    }
    compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Comma);
  }
  std_span_Span end_span = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseCurly)->span;
  node->span=std_span_Span_join(start_span, end_span);
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_if(compiler_parser_Parser *this) {
  compiler_tokens_Token *start_tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_If);
  std_span_Span start_span = start_tok->span;
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_OpenCurly)) {
  return compiler_parser_Parser_parse_multi_if(this, start_tok);
  }
  std_vector_Vector__17 *cases = std_vector_Vector__17_new(16);
  std_span_Span end_span = start_span;
  while (true) {
    compiler_ast_nodes_AST *cond = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
    compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Then);
    compiler_ast_nodes_AST *body = compiler_parser_Parser_parse_statement(this);
    std_vector_Vector__17_push(cases, (compiler_ast_nodes_IfBranch){.cond=cond, .body=body});
    end_span=body->span;
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Else) && compiler_parser_Parser_peek_token_is(this, 1, compiler_tokens_TokenType_If)) {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Else);
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_If);
    } else {
      break;
    }
  }
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_If, std_span_Span_join(start_span, end_span));
  node->u.if_stmt.branches=cases;
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Else)) {
    compiler_tokens_Token *els_tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Else);
    node->u.if_stmt.els=compiler_parser_Parser_parse_statement(this);
    node->u.if_stmt.els_span=els_tok->span;
  }
  node->span=std_span_Span_join(start_span, end_span);
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_for_each(compiler_parser_Parser *this, std_span_Span start_span) {
  compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
  if (!(compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Colon) || compiler_parser_Parser_token_is_identifier(this, "in"))) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected `:` of `in` after for-each loop variable"));
    return compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Error, start_span);
  }
  compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
  compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
  char *iter_var_name = "__iter";
  compiler_ast_nodes_AST *init = ({ compiler_ast_nodes_AST *__yield_0;
    compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_VarDeclaration, start_span);
    compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(NULL);
    var->sym=compiler_ast_scopes_Symbol_from_local_variable(iter_var_name, var, start_span);
    node->u.var_decl.var=var;
    node->u.var_decl.init=expr;
    __yield_0 = node;
  ;__yield_0; });
  compiler_ast_nodes_AST *cond = ({ compiler_ast_nodes_AST *__yield_0;
    compiler_ast_nodes_AST *iter_name = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Identifier, start_span);
    iter_name->u.ident.name=iter_var_name;
    compiler_ast_nodes_AST *member = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Member, start_span);
    member->u.member.lhs=iter_name;
    member->u.member.rhs_name="has_value";
    member->u.member.rhs_span=expr->span;
    compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Call, start_span);
    node->u.call.callee=member;
    node->u.call.args=std_vector_Vector__15_new(16);
    node->u.call.open_paren_span=name->span;
    node->u.call.close_paren_span=name->span;
    __yield_0 = node;
  ;__yield_0; });
  compiler_ast_nodes_AST *step = ({ compiler_ast_nodes_AST *__yield_0;
    compiler_ast_nodes_AST *iter_name = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Identifier, start_span);
    iter_name->u.ident.name=iter_var_name;
    compiler_ast_nodes_AST *member = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Member, start_span);
    member->u.member.lhs=iter_name;
    member->u.member.rhs_name="next";
    member->u.member.rhs_span=expr->span;
    compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Call, start_span);
    node->u.call.callee=member;
    node->u.call.args=std_vector_Vector__15_new(16);
    node->u.call.open_paren_span=name->span;
    node->u.call.close_paren_span=name->span;
    __yield_0 = node;
  ;__yield_0; });
  compiler_ast_nodes_AST *loop_var_decl = ({ compiler_ast_nodes_AST *__yield_0;
    compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(NULL);
    var->sym=compiler_ast_scopes_Symbol_from_local_variable(name->text, var, name->span);
    compiler_ast_nodes_AST *iter_name = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Identifier, start_span);
    iter_name->u.ident.name=iter_var_name;
    compiler_ast_nodes_AST *member = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Member, start_span);
    member->u.member.lhs=iter_name;
    member->u.member.rhs_name="cur";
    member->u.member.rhs_span=expr->span;
    compiler_ast_nodes_AST *call = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Call, start_span);
    call->u.call.callee=member;
    call->u.call.args=std_vector_Vector__15_new(16);
    call->u.call.open_paren_span=name->span;
    call->u.call.close_paren_span=name->span;
    compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_VarDeclaration, start_span);
    node->u.var_decl.var=var;
    node->u.var_decl.init=call;
    __yield_0 = node;
  ;__yield_0; });
  compiler_ast_nodes_AST *inner_body = compiler_parser_Parser_parse_block(this);
  std_vector_Vector__13 *statements = std_vector_Vector__13_new(16);
  std_vector_Vector__13_push(statements, loop_var_decl);
  std_vector_Vector__13_push(statements, inner_body);
  compiler_ast_nodes_AST *body = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Block, inner_body->span);
  body->u.block.statements=statements;
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_For, std_span_Span_join(start_span, body->span));
  node->u.loop.init=init;
  node->u.loop.cond=cond;
  node->u.loop.step=step;
  node->u.loop.body=body;
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_for(compiler_parser_Parser *this) {
  compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_For);
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier) && (compiler_parser_Parser_peek(this, 1)->type==compiler_tokens_TokenType_Colon || compiler_tokens_Token_is_identifier((*compiler_parser_Parser_peek(this, 1)), "in"))) {
    return compiler_parser_Parser_parse_for_each(this, tok->span);
  }
  compiler_ast_nodes_AST *init = ((compiler_ast_nodes_AST *)NULL);
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Let)) {
    init=compiler_parser_Parser_parse_statement(this);
  } else {
    init=compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Semicolon);
    compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Semicolon);
  }
  compiler_ast_nodes_AST *cond = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Semicolon);
  compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Semicolon);
  compiler_ast_nodes_AST *step = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_OpenCurly);
  compiler_ast_nodes_AST *body = compiler_parser_Parser_parse_block(this);
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_For, std_span_Span_join(tok->span, body->span));
  node->u.loop.init=init;
  node->u.loop.cond=cond;
  node->u.loop.step=step;
  node->u.loop.body=body;
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_statement(compiler_parser_Parser *this) {
  compiler_ast_nodes_AST *node = ((compiler_ast_nodes_AST *)NULL);
  std_span_Span start_span = compiler_parser_Parser_token(this)->span;
  switch (compiler_parser_Parser_token(this)->type) {
    case compiler_tokens_TokenType_OpenCurly: {
      node=compiler_parser_Parser_parse_block(this);
    } break;
    case compiler_tokens_TokenType_Return: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Return);
      compiler_ast_nodes_AST *expr = ((compiler_ast_nodes_AST *)NULL);
      if (!compiler_parser_Parser_is_end_of_statement(this)) {
        expr=compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
      }
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Return, std_span_Span_join(start_span, compiler_parser_Parser_token(this)->span));
      node->u.child=expr;
      compiler_parser_Parser_consume_end_of_statement(this);
    } break;
    case compiler_tokens_TokenType_Yield: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Yield);
      compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Yield, std_span_Span_join(start_span, compiler_parser_Parser_token(this)->span));
      node->u.child=expr;
      compiler_parser_Parser_consume_end_of_statement(this);
    } break;
    case compiler_tokens_TokenType_Break: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Break);
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Break, std_span_Span_join(start_span, compiler_parser_Parser_token(this)->span));
      compiler_parser_Parser_consume_end_of_statement(this);
    } break;
    case compiler_tokens_TokenType_Continue: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Continue);
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Continue, std_span_Span_join(start_span, compiler_parser_Parser_token(this)->span));
      compiler_parser_Parser_consume_end_of_statement(this);
    } break;
    case compiler_tokens_TokenType_While: {
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_While);
      compiler_ast_nodes_AST *cond = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_OpenCurly);
      compiler_ast_nodes_AST *body = compiler_parser_Parser_parse_block(this);
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_While, std_span_Span_join(tok->span, body->span));
      node->u.loop.cond=cond;
      node->u.loop.body=body;
    } break;
    case compiler_tokens_TokenType_Assert: {
      compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Assert);
      compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
      compiler_ast_nodes_AST *msg = ((compiler_ast_nodes_AST *)NULL);
      std_span_Span end_span = expr->span;
      if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Comma)) {
        msg=compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
        end_span=msg->span;
      }
      compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Assert, std_span_Span_join(start->span, end_span));
      node->u.assertion.expr=expr;
      node->u.assertion.msg=msg;
      return node;
    } break;
    case compiler_tokens_TokenType_Defer: {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Defer);
      compiler_ast_nodes_AST *expr = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
      node=compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Defer, std_span_Span_join(start_span, compiler_parser_Parser_token(this)->span));
      node->u.child=expr;
      compiler_parser_Parser_consume_end_of_statement(this);
    } break;
    case compiler_tokens_TokenType_Import: {
      node=compiler_parser_Parser_parse_import(this);
      compiler_parser_Parser_consume_end_of_statement(this);
    } break;
    case compiler_tokens_TokenType_For: {
      node=compiler_parser_Parser_parse_for(this);
    } break;
    case compiler_tokens_TokenType_Let: {
      node=compiler_parser_Parser_parse_var_declaration(this);
    } break;
    default: {
      node=compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
      compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Semicolon);
    } break;
  }
  return node;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_block(compiler_parser_Parser *this) {
  if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_OpenCurly)) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected '{'"));
    return compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Error, compiler_parser_Parser_token(this)->span);
  }
  compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenCurly);
  std_vector_Vector__13 *statements = std_vector_Vector__13_new(16);
  while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
    compiler_ast_nodes_AST *statement = compiler_parser_Parser_parse_statement(this);
    if (((bool)statement)) {
    std_vector_Vector__13_push(statements, statement);
    }
  }
  if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected '}'"));
    return compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Error, compiler_parser_Parser_token(this)->span);
  }
  compiler_tokens_Token *end = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseCurly);
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Block, std_span_Span_join(start->span, end->span));
  node->u.block.statements=statements;
  return node;
}

void compiler_parser_Parser_parse_template_params(compiler_parser_Parser *this, compiler_ast_scopes_Symbol *sym, std_span_Span *out_span) {
  std_span_Span start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_LessThan)->span;
  std_vector_Vector__19 *params = std_vector_Vector__19_new(16);
  while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_GreaterThan)) {
    compiler_tokens_Token *type = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
    char *name = type->text;
    compiler_ast_scopes_Symbol *sym = compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType_TypeDef, this->ns, name, name, name, type->span);
    sym->u.type_def=compiler_types_Type_new_unresolved(name, type->span);
    sym->u.type_def->sym=sym;
    std_vector_Vector__19_push(params, sym);
    if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_GreaterThan)) {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Comma);
    }
  }
  std_span_Span end = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_GreaterThan)->span;
  if (((bool)out_span)) {
    (*out_span)=std_span_Span_join(start, end);
  }
  sym->template=compiler_ast_scopes_Template_new(params);
}

void compiler_parser_Parser_add_doc_comment(compiler_parser_Parser *this, compiler_ast_scopes_Symbol *sym, compiler_tokens_Token *token) {
  if (((bool)token->comment)) {
    sym->comment=token->comment;
    sym->comment_loc=token->comment_loc;
  }
  if ((this->attrs->size > 0) && ((bool)this->attrs_start_tok->comment)) {
    if (!((bool)sym->comment)) {
      sym->comment=this->attrs_start_tok->comment;
      sym->comment_loc=this->attrs_start_tok->comment_loc;
    }
  }
}

compiler_ast_nodes_Function *compiler_parser_Parser_parse_function(compiler_parser_Parser *this) {
  compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Def);
  compiler_types_Type *parent_type = ((compiler_types_Type *)NULL);
  bool is_method = false;
  bool is_static = true;
  compiler_ast_nodes_AST *ident = compiler_parser_Parser_parse_scoped_identifier(this, false);
  if (!((bool)ident)) {
  return NULL;
  }
  std_span_Span name_span = ident->span;
  compiler_ast_nodes_Function *func = compiler_ast_nodes_Function_new();
  func->name_ast=ident;
  char *name = ({ char *__yield_0;
    switch (ident->type) {
      case compiler_ast_nodes_ASTType_Identifier: {
        __yield_0 = ident->u.ident.name;
      } break;
      case compiler_ast_nodes_ASTType_NSLookup: {
        parent_type=compiler_types_Type_new_unresolved("<unresolved>", ident->span);
        parent_type->u.unresolved=ident->u.lookup.lhs;
        is_method=true;
        name_span=ident->u.lookup.rhs_span;
        __yield_0 = ident->u.lookup.rhs_name;
      } break;
      default: {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(ident->span, "Expected identifier"));
        __yield_0 = "<error>";
      } break;
    }
;__yield_0; });
  func->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Function, this->ns, this->ns->sym, name, name_span);
  func->sym->u.func=func;
  compiler_parser_Parser_add_doc_comment(this, func->sym, start);
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_LessThan)) {
    compiler_parser_Parser_parse_template_params(this, func->sym, NULL);
  }
  compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenParen);
  bool seen_default = false;
  while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseParen)) {
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Ellipsis)) {
      if (seen_default) {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Cannot have variadic parameters and default parameters"));
      }
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Ellipsis);
      func->is_variadic=true;
      break;
    }
    bool found_amp = compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Ampersand);
    compiler_tokens_Token *var_name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
    compiler_types_Type *type = ((compiler_types_Type *)NULL);
    if (std_vector_Vector__4_is_empty(func->params) && is_method) {
      if (str_eq(var_name->text, "this")) {
        type=parent_type;
        if (found_amp) {
          type=compiler_types_Type_new_resolved(compiler_types_BaseType_Pointer, parent_type->span);
          type->u.ptr=parent_type;
        }
        is_static=false;
      } else if (found_amp) {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(var_name->span, "Expected 'this' over here"));
      }
    }
    if (!((bool)type)) {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Colon);
      type=compiler_parser_Parser_parse_type(this);
    }
    compiler_ast_nodes_AST *default_value = ((compiler_ast_nodes_AST *)NULL);
    if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Equals)) {
      default_value=compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Comma);
      seen_default=true;
    } else if (seen_default) {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(var_name->span, "Cannot have non-default parameters after default parameters"));
    }
    compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(type);
    var->sym=compiler_ast_scopes_Symbol_from_local_variable(var_name->text, var, var_name->span);
    var->default_value=default_value;
    std_vector_Vector__4_push(func->params, var);
    compiler_parser_Parser_add_doc_comment(this, var->sym, var_name);
    if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseParen)) {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Comma);
    }
  }
  std_span_Span end_span = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseParen)->span;
  bool returns = false;
  if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Colon)) {
    func->return_type=compiler_parser_Parser_parse_type(this);
    end_span=func->return_type->span;
    returns=true;
  } else if (str_eq(func->sym->full_name, "main")) {
    func->return_type=compiler_types_Type_new_unresolved_base(compiler_types_BaseType_I32, name_span);
    returns=true;
  } else {
    func->return_type=compiler_types_Type_new_unresolved_base(compiler_types_BaseType_Void, name_span);
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier) && str_eq(compiler_parser_Parser_token(this)->text, "exits")) {
      end_span=compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier)->span;
      func->exits=true;
    }
  }
  func->is_method=is_method;
  func->is_static=is_static;
  func->parent_type=parent_type;
  func->parsed_return_type=func->return_type;
  compiler_parser_Parser_parse_extern_into_symbol(this, func->sym);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(this->attrs); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    compiler_attributes_Attribute *attr = std_vector_Iterator__10_cur(&__iter);
    {
      switch (attr->type) {
        case compiler_attributes_AttributeType_Exits: {
          func->exits=true;
        } break;
        case compiler_attributes_AttributeType_Extern: {
          compiler_parser_Parser_get_extern_from_attr(this, func->sym, attr);
        } break;
        case compiler_attributes_AttributeType_VariadicFormat: {
          if (!func->is_variadic) {
            compiler_parser_Parser_error(this, compiler_errors_Error_new(attr->span, "Variadic format attribute can only be used on variadic functions"));
          }
          func->is_variadic_format=true;
        } break;
        case compiler_attributes_AttributeType_Operator: {
          compiler_ast_operators_Operator op = compiler_ast_operators_Operator_from_operator_overload(std_vector_Vector__1_at(attr->args, 0), func->params);
          if (op==compiler_ast_operators_Operator_Error) {
            compiler_parser_Parser_error(this, compiler_errors_Error_new(attr->span, "Invalid operator"));
            continue;
          }
          if (!((bool)func->operator_overloads)) {
            func->operator_overloads=std_vector_Vector__14_new(16);
          }
          std_vector_Vector__14_push(func->operator_overloads, op);
        } break;
        default: {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(attr->span, format_string("Invalid attribute for function: %s", compiler_attributes_AttributeType_dbg(attr->type))));
        } break;
      }
    }
  }
  if (func->sym->is_extern) {
    func->span=std_span_Span_join(start->span, end_span);
    return func;
  }
  this->curr_func=func;
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_FatArrow)) {
    compiler_tokens_Token *arrow = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_FatArrow);
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_OpenCurly)) {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected an expression for an arrow function"));
      return NULL;
    }
    compiler_ast_nodes_AST *stmt = compiler_parser_Parser_parse_expression(this, compiler_tokens_TokenType_Newline);
    if (returns) {
      compiler_ast_nodes_AST *ret = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Return, stmt->span);
      ret->u.child=stmt;
      stmt=ret;
    }
    compiler_ast_nodes_AST *body = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Block, stmt->span);
    std_vector_Vector__13 *statements = std_vector_Vector__13_new(16);
    std_vector_Vector__13_push(statements, stmt);
    body->u.block.statements=statements;
    func->body=body;
  } else {
    func->body=compiler_parser_Parser_parse_block(this);
  }
  this->curr_func=NULL;
  func->span=std_span_Span_join(start->span, func->body->span);
  return func;
}

void compiler_parser_Parser_parse_extern_into_symbol(compiler_parser_Parser *this, compiler_ast_scopes_Symbol *sym) {
  if (!compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Extern)) {
  return;
  }
  sym->is_extern=true;
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_OpenParen)) {
    compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenParen);
    compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_StringLiteral);
    compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseParen);
    sym->extern_name=name->text;
  } else {
    sym->extern_name=sym->name;
  }
}

void compiler_parser_Parser_get_extern_from_attr(compiler_parser_Parser *this, compiler_ast_scopes_Symbol *sym, compiler_attributes_Attribute *attr) {
  ae_assert(attr->type==compiler_attributes_AttributeType_Extern, "/Users/mustafa/ocen-lang/ocen/compiler/parser.oc:1587:12: Assertion failed: `attr.type == Extern`", NULL);
  sym->is_extern=true;
  if (attr->args->size > 0) {
    sym->extern_name=std_vector_Vector__1_at(attr->args, 0);
  } else {
    sym->extern_name=sym->name;
  }
}

std_vector_Vector__5 *compiler_parser_Parser_parse_import_path(compiler_parser_Parser *this) {
  std_vector_Vector__5 *parts = std_vector_Vector__5_new(16);
  while (true) {
    bool done = false;
    if (compiler_tokens_Token_is_word((*compiler_parser_Parser_token(this)))) {
      compiler_tokens_Token *word = compiler_parser_Parser_token(this);
      this->curr+=1;
      compiler_ast_nodes_ImportPart *part = compiler_ast_nodes_ImportPart_new(compiler_ast_nodes_ImportPartType_Single, word->span);
      part->u.single.name=word->text;
      part->u.single.name_span=word->span;
      if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_As)) {
        compiler_tokens_Token *alias = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
        part->u.single.alias=alias->text;
        part->u.single.alias_span=alias->span;
        done=true;
      }
      std_vector_Vector__5_push(parts, part);
    } else if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Star)) {
      compiler_tokens_Token *tok = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Star);
      compiler_ast_nodes_ImportPart *part = compiler_ast_nodes_ImportPart_new(compiler_ast_nodes_ImportPartType_Wildcard, tok->span);
      std_vector_Vector__5_push(parts, part);
      done=true;
    } else if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_OpenCurly)) {
      compiler_tokens_Token *open = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenCurly);
      std_vector_Vector__16 *sub_paths = std_vector_Vector__16_new(16);
      while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
        std_vector_Vector__5 *sub_path = compiler_parser_Parser_parse_import_path(this);
        if (!((bool)sub_path)) {
        return NULL;
        }
        std_vector_Vector__16_push(sub_paths, sub_path);
        if (!compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Comma)) {
        break;
        }
      }
      compiler_tokens_Token *close = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseCurly);
      compiler_ast_nodes_ImportPart *part = compiler_ast_nodes_ImportPart_new(compiler_ast_nodes_ImportPartType_Multiple, std_span_Span_join(open->span, close->span));
      part->u.paths=sub_paths;
      std_vector_Vector__5_push(parts, part);
      done=true;
    } else {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected identifier"));
      return NULL;
    }
    if (done) {
    break;
    }
    if (!compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_ColonColon)) {
    break;
    }
  }
  return parts;
}

compiler_ast_nodes_AST *compiler_parser_Parser_parse_import(compiler_parser_Parser *this) {
  std_span_Span span = compiler_parser_Parser_token(this)->span;
  compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Import);
  u32 parent_count = 0;
  compiler_ast_nodes_ImportType type = ({ compiler_ast_nodes_ImportType __yield_0;
    switch (compiler_parser_Parser_token(this)->type) {
      case compiler_tokens_TokenType_AtSign: {
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_AtSign);
        __yield_0 = compiler_ast_nodes_ImportType_ProjectNamespace;
      } break;
      case compiler_tokens_TokenType_ColonColon: {
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_ColonColon);
        __yield_0 = compiler_ast_nodes_ImportType_CurrentScope;
      } break;
      case compiler_tokens_TokenType_Dot:
      case compiler_tokens_TokenType_Ellipsis: {
        bool done = false;
        while (!done) {
          switch (compiler_parser_Parser_token(this)->type) {
            case compiler_tokens_TokenType_Dot: {
              compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Dot);
              parent_count+=1;
            } break;
            case compiler_tokens_TokenType_Ellipsis: {
              compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Ellipsis);
              parent_count+=3;
            } break;
            default: {
              done=true;
            } break;
          }
        }
        __yield_0 = compiler_ast_nodes_ImportType_ParentNamespace;
      } break;
      default: {
        __yield_0 = compiler_ast_nodes_ImportType_GlobalNamespace;
      } break;
    }
;__yield_0; });
  if (this->ns->is_top_level) {
    parent_count-=1;
  }
  std_vector_Vector__5 *parts = compiler_parser_Parser_parse_import_path(this);
  if (!((bool)parts)) {
  return NULL;
  }
  if (parts->size==0) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(span, "Invalid import statement"));
    return NULL;
  }
  compiler_ast_nodes_AST *node = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Import, span);
  node->u.import_path=(compiler_ast_nodes_Import){.parts=parts, .type=type, .parent_count=parent_count, .export=false};
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(this->attrs); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    compiler_attributes_Attribute *attr = std_vector_Iterator__10_cur(&__iter);
    {
      switch (attr->type) {
        case compiler_attributes_AttributeType_Export: {
          node->u.import_path.export=true;
        } break;
        default: {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(attr->span, "Invalid attribute for import"));
        } break;
      }
    }
  }
  if (!compiler_parser_Parser_load_import_path(this, node)) {
  return NULL;
  }
  return node;
}

bool compiler_parser_Parser_parse_struct_field(compiler_parser_Parser *this, compiler_ast_nodes_Structure *struc) {
  std_vector_Vector__4 *fields = std_vector_Vector__4_new(4);
  while (true) {
    if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier)) {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected identifier for field name"));
      return false;
    }
    compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
    compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(NULL);
    var->sym=compiler_ast_scopes_Symbol_from_local_variable(name->text, var, name->span);
    compiler_parser_Parser_add_doc_comment(this, var->sym, name);
    std_vector_Vector__4_push(fields, var);
    if (!compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Comma)) {
    break;
    }
  }
  if (!compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Colon)) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected ':' after struct field names for type"));
    return false;
  }
  compiler_types_Type *type = compiler_parser_Parser_parse_type(this);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *var = std_vector_Iterator__4_cur(&__iter);
    {
      var->type=type;
      var->parsed_type=type;
      std_vector_Vector__4_push(struc->fields, var);
    }
  }
  std_vector_Vector__4_free(fields);
  return true;
}

compiler_ast_nodes_Structure *compiler_parser_Parser_parse_struct(compiler_parser_Parser *this) {
  compiler_tokens_Token *start = compiler_parser_Parser_token(this);
  bool is_union = ({ bool __yield_0;
    if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Union)) {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Union);
      __yield_0 = true;
    } else {
      compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Struct);
      __yield_0 = false;
    }
;__yield_0; });
  compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
  compiler_ast_nodes_Structure *struc = compiler_ast_nodes_Structure_new();
  struc->is_union=is_union;
  struc->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Structure, this->ns, this->ns->sym, name->text, name->span);
  struc->sym->u.struc=struc;
  compiler_parser_Parser_add_doc_comment(this, struc->sym, start);
  if (compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_LessThan)) {
    compiler_parser_Parser_parse_template_params(this, struc->sym, NULL);
  }
  compiler_parser_Parser_parse_extern_into_symbol(this, struc->sym);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(this->attrs); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    compiler_attributes_Attribute *attr = std_vector_Iterator__10_cur(&__iter);
    {
      switch (attr->type) {
        case compiler_attributes_AttributeType_Extern: {
          compiler_parser_Parser_get_extern_from_attr(this, struc->sym, attr);
        } break;
        case compiler_attributes_AttributeType_Formatting: {
          struc->format_spec=std_vector_Vector__1_at(attr->args, 0);
          struc->format_args=std_vector_Vector__1_at(attr->args, 1);
        } break;
        default: {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(attr->span, "Invalid attribute for struct"));
        } break;
      }
    }
  }
  if (!struc->sym->is_extern || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_OpenCurly)) {
    compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenCurly);
    while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
      if (!compiler_parser_Parser_parse_struct_field(this, struc)) {
      break;
      }
      if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
        compiler_parser_Parser_consume_newline_or(this, compiler_tokens_TokenType_Comma);
      }
    }
    compiler_tokens_Token *end = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseCurly);
    struc->span=std_span_Span_join(start->span, end->span);
  }
  return struc;
}

compiler_ast_nodes_Enum *compiler_parser_Parser_parse_enum(compiler_parser_Parser *this) {
  compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Enum);
  compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
  compiler_ast_nodes_Enum *enum_def = compiler_ast_nodes_Enum_new();
  enum_def->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Enum, this->ns, this->ns->sym, name->text, start->span);
  enum_def->sym->u.enum_=enum_def;
  compiler_parser_Parser_add_doc_comment(this, enum_def->sym, start);
  compiler_parser_Parser_parse_extern_into_symbol(this, enum_def->sym);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(this->attrs); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    compiler_attributes_Attribute *attr = std_vector_Iterator__10_cur(&__iter);
    {
      switch (attr->type) {
        case compiler_attributes_AttributeType_Extern: {
          compiler_parser_Parser_get_extern_from_attr(this, enum_def->sym, attr);
        } break;
        default: {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(attr->span, "Invalid attribute for enum"));
        } break;
      }
    }
  }
  compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenCurly);
  while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
    compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
    compiler_ast_nodes_Variable *var = compiler_ast_nodes_Variable_new(NULL);
    var->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_EnumVariant, this->ns, enum_def->sym, name->text, name->span);
    var->sym->u.var=var;
    compiler_parser_Parser_add_doc_comment(this, var->sym, name);
    if (compiler_parser_Parser_consume_if(this, compiler_tokens_TokenType_Equals)) {
      compiler_parser_Parser_parse_extern_into_symbol(this, var->sym);
    }
    std_vector_Vector__4_push(enum_def->fields, var);
    if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseCurly)) {
      compiler_parser_Parser_consume_newline_or(this, compiler_tokens_TokenType_Comma);
    }
  }
  compiler_tokens_Token *end = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseCurly);
  enum_def->span=std_span_Span_join(start->span, end->span);
  return enum_def;
}

void compiler_parser_Parser_parse_attribute(compiler_parser_Parser *this) {
  compiler_tokens_Token *start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenSquare);
  if (this->attrs->size==0) {
    this->attrs_span=start->span;
    this->attrs_start_tok=start;
  }
  if (!(compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Identifier) || compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_Extern))) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Expected attribute name here"));
    return;
  }
  compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_parser_Parser_token(this)->type);
  compiler_attributes_AttributeType attr_type = compiler_attributes_AttributeType_from_str(name->text);
  if (attr_type==compiler_attributes_AttributeType_Invalid) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(name->span, "Unknown attribute type"));
    return;
  }
  compiler_attributes_Attribute *attr = compiler_attributes_Attribute_new(attr_type, name->span);
  while (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_CloseSquare)) {
    if (!compiler_parser_Parser_token_is(this, compiler_tokens_TokenType_StringLiteral)) {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Only string literals supported in attribute arguments"));
      this->curr+=1;
      continue;
    }
    compiler_tokens_Token *arg = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_StringLiteral);
    std_vector_Vector__1_push(attr->args, arg->text);
  }
  compiler_tokens_Token *close = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseSquare);
  this->attrs_span=std_span_Span_join(this->attrs_span, close->span);
  if (!compiler_attributes_Attribute_validate(attr, this)) {
  return;
  }
  std_vector_Vector__10_push(this->attrs, attr);
}

void compiler_parser_Parser_parse_namespace_until(compiler_parser_Parser *this, compiler_tokens_TokenType end_type) {
  compiler_parser_Parser_add_doc_comment(this, this->ns->sym, compiler_parser_Parser_token(this));
  while (!compiler_parser_Parser_token_is(this, end_type)) {
    bool still_parsing_attributes = false;
    switch (compiler_parser_Parser_token(this)->type) {
      case compiler_tokens_TokenType_OpenSquare: {
        compiler_parser_Parser_parse_attribute(this);
        still_parsing_attributes=true;
      } break;
      case compiler_tokens_TokenType_Def: {
        compiler_ast_nodes_Function *func = compiler_parser_Parser_parse_function(this);
        if (((bool)func)) {
        std_vector_Vector__6_push(this->ns->functions, func);
        }
      } break;
      case compiler_tokens_TokenType_Import: {
        compiler_ast_nodes_AST *import_ = compiler_parser_Parser_parse_import(this);
        if (((bool)import_)) {
        std_vector_Vector__13_push(this->ns->imports, import_);
        }
      } break;
      case compiler_tokens_TokenType_Namespace: {
        if (this->attrs->size > 0) {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Attributes are not allowed on namespaces"));
        }
        std_span_Span start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Namespace)->span;
        compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
        compiler_ast_program_Namespace *old_ns = this->ns;
        compiler_ast_program_Namespace *new_ns = compiler_ast_program_Namespace_new(this->ns, this->ns->path);
        new_ns->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Namespace, old_ns, old_ns->sym, name->text, name->span);
        new_ns->sym->u.ns=new_ns;
        new_ns->always_add_to_scope=true;
        std_map_Map__3_insert(old_ns->namespaces, name->text, new_ns);
        this->ns=new_ns;
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_OpenCurly);
        compiler_parser_Parser_parse_namespace_until(this, compiler_tokens_TokenType_CloseCurly);
        std_span_Span end = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_CloseCurly)->span;
        new_ns->span=std_span_Span_join(start, end);
        this->ns=old_ns;
      } break;
      case compiler_tokens_TokenType_Struct:
      case compiler_tokens_TokenType_Union: {
        compiler_ast_nodes_Structure *struc = compiler_parser_Parser_parse_struct(this);
        if (((bool)struc)) {
        std_vector_Vector__2_push(this->ns->structs, struc);
        }
      } break;
      case compiler_tokens_TokenType_TypeDef: {
        if (this->attrs->size > 0) {
          compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Attributes are not allowed on typedefs"));
        }
        std_span_Span start = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_TypeDef)->span;
        compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
        compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Equals);
        compiler_types_Type *type = compiler_parser_Parser_parse_type(this);
        compiler_parser_Parser_consume_end_of_statement(this);
        if (((bool)type)) {
          std_map_Map__2_insert(this->ns->typedefs, name->text, type);
        }
      } break;
      case compiler_tokens_TokenType_Enum: {
        compiler_ast_nodes_Enum *enum_value = compiler_parser_Parser_parse_enum(this);
        if (((bool)enum_value)) {
        std_vector_Vector__12_push(this->ns->enums, enum_value);
        }
      } break;
      case compiler_tokens_TokenType_Let: {
        compiler_ast_nodes_AST *var = compiler_parser_Parser_parse_global_value(this, false);
        if (((bool)var)) {
        std_vector_Vector__13_push(this->ns->variables, var);
        }
      } break;
      case compiler_tokens_TokenType_Const: {
        compiler_ast_nodes_AST *con = compiler_parser_Parser_parse_global_value(this, true);
        if (((bool)con)) {
        std_vector_Vector__13_push(this->ns->constants, con);
        }
      } break;
      case compiler_tokens_TokenType_AtSign: {
        compiler_parser_Parser_parse_compiler_option(this);
      } break;
      default: {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, format_string("Unexpected token in Parser: %s", compiler_tokens_TokenType_dbg(compiler_parser_Parser_token(this)->type))));
        this->curr+=1;
      } break;
    }
    if (!still_parsing_attributes) {
      compiler_parser_Parser_clear_attributes(this);
    }
  }
}

void compiler_parser_Parser_parse_compiler_option(compiler_parser_Parser *this) {
  if (this->attrs->size > 0) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler_parser_Parser_token(this)->span, "Attributes are not allowed on compiler options"));
  }
  compiler_parser_Parser_consume(this, compiler_tokens_TokenType_AtSign);
  compiler_tokens_Token *compiler = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
  if (!str_eq(compiler->text, "compiler")) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(compiler->span, "Expected 'compiler'"));
  }
  compiler_tokens_Token *name = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_Identifier);
  {
    char *__match_var_0 = name->text;
    if (str_eq(__match_var_0, "c_include")) {
      compiler_tokens_Token *filename = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_StringLiteral);
      std_vector_Vector__1_push(this->program->c_includes, filename->text);
    } else if (str_eq(__match_var_0, "c_flag")) {
      compiler_tokens_Token *flag = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_StringLiteral);
      std_vector_Vector__1_push(this->program->c_flags, flag->text);
    } else if (str_eq(__match_var_0, "c_embed")) {
      compiler_tokens_Token *path = compiler_parser_Parser_consume(this, compiler_tokens_TokenType_StringLiteral);
      char *cur_dir = ({ char *__yield_0;
        if (this->ns->is_top_level) {
          __yield_0 = this->ns->path;
        } else {
          __yield_0 = this->ns->parent->path;
        }
;__yield_0; });
      char *full_path = format_string("%s/%s", cur_dir, path->text);
      if (!std_fs_file_exists(full_path)) {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(path->span, format_string("File '%s' does not exist", full_path)));
        return;
      }
      std_buffer_Buffer contents = std_fs_read_file(full_path);
      std_map_Map__6_insert(this->program->c_embeds, full_path, std_buffer_Buffer_str(contents));
    } else  {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(name->span, "Unknown compiler option"));
    }
  }
}

void compiler_parser_Parser_try_load_mod_for_namespace(compiler_parser_Parser *this, compiler_ast_program_Namespace *ns) {
  char *mod_path = format_string("%s/mod.oc", ns->path);
  if (std_fs_file_exists(mod_path) && !ns->is_top_level) {
    ns->is_top_level=true;
    compiler_parser_Parser parser = compiler_parser_Parser_make(this->program, ns);
    compiler_parser_Parser_load_file(&parser, mod_path, NULL);
  } else {
    std_mem_free(mod_path);
  }
}

compiler_ast_program_Namespace *compiler_parser_Parser_load_single_import_part(compiler_parser_Parser *this, compiler_ast_program_Namespace *base, char *name, std_span_Span span) {
  compiler_parser_Parser_try_load_mod_for_namespace(this, base);
  compiler_ast_scopes_Symbol *sym = compiler_ast_program_Namespace_find_importable_symbol(base, name);
  if (str_eq(name, "this")) {
  return base;
  }
  if (((bool)sym) && (sym->type != compiler_ast_scopes_SymbolType_Namespace)) {
  return base;
  }
  compiler_ast_program_Namespace *next = (((bool)sym) ? sym->u.ns : NULL);
  char *part_path = format_string("%s/%s", base->path, name);
  if (!((bool)next)) {
    bool dir_exists = compiler_utils_directory_exists(part_path);
    char *path = format_string("%s/%s.oc", base->path, name);
    bool file_exists = std_fs_file_exists(path);
    if (!dir_exists && !file_exists) {
      compiler_parser_Parser_error(this, compiler_errors_Error_new(span, format_string("Could not find import path %s(.oc)", part_path)));
      return NULL;
    }
    next=compiler_ast_program_Namespace_new(base, part_path);
    next->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Namespace, base, base->sym, name, span);
    next->sym->u.ns=next;
    std_map_Map__3_insert(base->namespaces, name, next);
    if (file_exists) {
      compiler_parser_Parser parser = compiler_parser_Parser_make(this->program, next);
      compiler_parser_Parser_load_file(&parser, strdup(path), NULL);
    } else {
      compiler_parser_Parser_try_load_mod_for_namespace(this, next);
    }
    std_mem_free(path);
  }
  return next;
}

bool compiler_parser_Parser_load_import_path_from_base(compiler_parser_Parser *this, std_vector_Vector__5 *parts, compiler_ast_program_Namespace *base) {
  for (u32 i = 0; (i < parts->size) && (!base->is_a_file || base->is_top_level); i+=1) {
    compiler_ast_nodes_ImportPart *part = std_vector_Vector__5_at(parts, i);
    switch (part->type) {
      case compiler_ast_nodes_ImportPartType_Wildcard: {
        compiler_parser_Parser_error(this, compiler_errors_Error_new(part->span, format_string("Wildcard import is not allowed from non-module")));
        return false;
      } break;
      case compiler_ast_nodes_ImportPartType_Multiple: {
        std_vector_Vector__16 *paths = part->u.paths;
        bool success = true;
        for (std_vector_Iterator__16 __iter = std_vector_Vector__16_iter(paths); std_vector_Iterator__16_has_value(&__iter); std_vector_Iterator__16_next(&__iter)) {
          std_vector_Vector__5 *path = std_vector_Iterator__16_cur(&__iter);
          {
            success=(compiler_parser_Parser_load_import_path_from_base(this, path, base) && success);
          }
        }
        return success;
      } break;
      case compiler_ast_nodes_ImportPartType_Single: {
        base=compiler_parser_Parser_load_single_import_part(this, base, part->u.single.name, part->span);
        if (!((bool)base)) {
        return false;
        }
      } break;
    }
  }
  return true;
}

char *compiler_parser_Parser_find_external_library_path(compiler_parser_Parser *this, char *name) {
  for (std_vector_Iterator__1 __iter = std_vector_Vector__1_iter(this->program->library_paths); std_vector_Iterator__1_has_value(&__iter); std_vector_Iterator__1_next(&__iter)) {
    char *lib_path = std_vector_Iterator__1_cur(&__iter);
    {
      char *path = ((strlen(lib_path) > 0) ? format_string("%s/%s", lib_path, name) : strdup(name));
      if (compiler_utils_directory_exists(path)) {
        return path;
      }
      std_mem_free(path);
    }
  }
  return NULL;
}

compiler_ast_program_Namespace *compiler_parser_Parser_find_external_library(compiler_parser_Parser *this, char *name) {
  char *path = compiler_parser_Parser_find_external_library_path(this, name);
  if (!((bool)path)) {
  return NULL;
  }
  compiler_ast_program_Namespace *ns = compiler_ast_program_Namespace_new(this->program->global, path);
  ns->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Namespace, this->program->global, this->program->global->sym, name, std_span_Span_default());
  ns->sym->u.ns=ns;
  ns->always_add_to_scope=true;
  ns->internal_project_root=ns;
  compiler_parser_Parser_try_load_mod_for_namespace(this, ns);
  return ns;
}

bool compiler_parser_Parser_load_import_path(compiler_parser_Parser *this, compiler_ast_nodes_AST *import_stmt) {
  compiler_ast_nodes_Import *path = &import_stmt->u.import_path;
  compiler_ast_program_Namespace *base = ({ compiler_ast_program_Namespace *__yield_0;
    switch (path->type) {
      case compiler_ast_nodes_ImportType_GlobalNamespace: {
        std_vector_Vector__5 *parts = path->parts;
        ae_assert(parts->size > 0, "/Users/mustafa/ocen-lang/ocen/compiler/parser.oc:2142:20: Assertion failed: `parts.size > 0`", "Expected at least one part in import path");
        ae_assert(std_vector_Vector__5_at(parts, 0)->type==compiler_ast_nodes_ImportPartType_Single, "/Users/mustafa/ocen-lang/ocen/compiler/parser.oc:2143:20: Assertion failed: `parts.at(0).type == Single`", "Expected first part to be a single import");
        compiler_ast_nodes_ImportPartSingle first_part = std_vector_Vector__5_at(parts, 0)->u.single;
        char *lib_name = first_part.name;
        if (!std_map_Map__3_contains(this->program->global->namespaces, lib_name)) {
          compiler_ast_program_Namespace *lib = compiler_parser_Parser_find_external_library(this, lib_name);
          if (!((bool)lib)) {
            compiler_parser_Parser_error(this, compiler_errors_Error_new(import_stmt->span, format_string("Could not find library '%s'", lib_name)));
            return false;
          }
          std_map_Map__3_insert(this->program->global->namespaces, lib_name, lib);
        }
        __yield_0 = this->program->global;
      } break;
      case compiler_ast_nodes_ImportType_ProjectNamespace: {
        __yield_0 = compiler_ast_program_Namespace_get_project_root(this->ns, import_stmt->span, this->program);
      } break;
      case compiler_ast_nodes_ImportType_ParentNamespace: {
        compiler_ast_program_Namespace *cur = this->ns;
        for (u32 i = 0; i < path->parent_count; i+=1) {
          if (!((bool)cur->parent)) {
            compiler_ast_nodes_ImportPart *first_part = std_vector_Vector__5_at(path->parts, 0);
            compiler_parser_Parser_error(this, compiler_errors_Error_new(first_part->span, "Cannot import from parent of root namespace"));
            longjmp(this->program->err_jmp_ctx, 1);
          }
          cur=cur->parent;
        }
        __yield_0 = cur;
      } break;
      case compiler_ast_nodes_ImportType_CurrentScope: {
        return true;
      } break;
    }
;__yield_0; });
  if (!((bool)base)) {
  return false;
  }
  compiler_parser_Parser_load_import_path_from_base(this, path->parts, base);
  return true;
}

void compiler_parser_Parser_load_file(compiler_parser_Parser *this, char *filename, char *contents) {
  if (std_map_Map__6_contains(this->program->sources, filename)) {
  return;
  }
  std_span_Location loc = (std_span_Location){.filename=strdup(filename), .line=0, .col=0, .index=0};
  std_span_Span span = (std_span_Span){.start=loc, .end=loc};
  this->ns->span=span;
  this->ns->sym->span=span;
  if (!((bool)contents)) {
    contents=std_buffer_Buffer_str(std_fs_read_file(filename));
  }
  std_map_Map__6_insert(this->program->sources, filename, contents);
  compiler_lexer_Lexer lexer = compiler_lexer_Lexer_make(contents, filename);
  this->tokens=compiler_lexer_Lexer_lex(&lexer);
  this->curr=0;
  this->ns->is_a_file=true;
  std_span_Span start = compiler_parser_Parser_token(this)->span;
  compiler_parser_Parser_parse_namespace_until(this, compiler_tokens_TokenType_EOF);
  std_span_Span end = compiler_parser_Parser_token(this)->span;
  this->ns->span=std_span_Span_join(start, end);
}

void compiler_parser_Parser_couldnt_find_stdlib(compiler_parser_Parser *this) {
  printf("--------------------------------------------------------------------------------""\n");
  printf("    Could not find standard library. Set OCEN_ROOT environment variable.""\n");
  printf("      Alternatively, compile from the root of `ocen` repository.""\n");
  printf("--------------------------------------------------------------------------------""\n");
  longjmp(this->program->err_jmp_ctx, 1);
}

void compiler_parser_Parser_find_and_import_stdlib(compiler_parser_Parser *this) {
  compiler_ast_program_Namespace *std_ns = compiler_parser_Parser_find_external_library(this, "std");
  std_map_Map__3_insert(this->program->global->namespaces, "std", std_ns);
}

void compiler_parser_Parser_include_prelude_only(compiler_parser_Parser *this) {
  char *std_path = compiler_parser_Parser_find_external_library_path(this, "std");
  if (!((bool)std_path)) {
    compiler_parser_Parser_couldnt_find_stdlib(this);
  }
  char *prelude_path = format_string("%s/prelude.h", std_path);
  if (!std_fs_file_exists(prelude_path)) {
    compiler_parser_Parser_couldnt_find_stdlib(this);
  }
  std_buffer_Buffer content = std_fs_read_file(prelude_path);
  std_map_Map__6_insert(this->program->c_embeds, prelude_path, std_buffer_Buffer_str(content));
}

void compiler_parser_Parser_create_namespaces_for_initial_file(compiler_parser_Parser *this, char *filename, bool single_file) {
  compiler_ast_program_Namespace *std_lib_ns = compiler_parser_Parser_find_external_library(this, "std");
  if (!((bool)std_lib_ns)) {
  compiler_parser_Parser_couldnt_find_stdlib(this);
  }
  char *std_lib_ns_path = std_fs_realpath(std_lib_ns->path);
  char *cur = std_fs_realpath(filename);
  if (!((bool)cur)) {
    compiler_parser_Parser_error(this, compiler_errors_Error_new(std_span_Span_default(), format_string("Could not find file: %s", filename)));
    longjmp(this->program->err_jmp_ctx, 1);
  }
  std_vector_Vector__1 *namespace_paths = std_vector_Vector__1_new(16);
  bool found_root = false;
  while (true) {
    char *base = strdup(dirname(strdup(cur)));
    std_vector_Vector__1_push(namespace_paths, base);
    if (str_eq(base, "/")) {
    break;
    }
    if (single_file) {
    break;
    }
    char *potential_main_path = format_string("%s/main.oc", base);
    bool main_exists = std_fs_file_exists(potential_main_path);
    str_free(&potential_main_path);
    if (str_eq(base, std_lib_ns_path) || main_exists) {
      found_root=true;
      break;
    }
    cur=base;
  }
  i32 start = (found_root ? ((i32)namespace_paths->size) : 0);
  compiler_ast_program_Namespace *cur_ns = this->program->global;
  for (i32 i = (start - 1); i >= 0; i-=1) {
    char *path = namespace_paths->data[i];
    char *t1 = strdup(path);
    char *base = strdup(basename(t1));
    str_free(&t1);
    if (std_map_Map__3_contains(cur_ns->namespaces, base)) {
      cur_ns=std_map_Map__3_at(cur_ns->namespaces, base);
      continue;
    }
    compiler_ast_program_Namespace *new_ns = compiler_ast_program_Namespace_new(cur_ns, path);
    std_span_Location loc = (std_span_Location){.filename=path, .line=0, .col=0, .index=0};
    new_ns->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Namespace, cur_ns, cur_ns->sym, base, (std_span_Span){.start=loc, .end=loc});
    new_ns->sym->u.ns=new_ns;
    if (i==(start - 1)) {
      new_ns->internal_project_root=new_ns;
    }
    std_map_Map__3_insert(cur_ns->namespaces, base, new_ns);
    cur_ns=new_ns;
  }
  char *file_base = strdup(basename(filename));
  if (str_ends_with(file_base, ".oc")) {
    file_base[(strlen(file_base) - 3)]='\0';
  }
  if (str_eq(file_base, "mod")) {
    cur_ns->is_top_level=true;
    this->ns=cur_ns;
    return;
  }
  if (std_map_Map__3_contains(cur_ns->namespaces, file_base)) {
    this->ns=std_map_Map__3_at(cur_ns->namespaces, file_base);
    return;
  }
  compiler_ast_program_Namespace *child_ns = compiler_ast_program_Namespace_new(cur_ns, filename);
  child_ns->sym=compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType_Namespace, cur_ns, file_base, "", "", std_span_Span_default());
  child_ns->sym->u.ns=child_ns;
  std_map_Map__3_insert(cur_ns->namespaces, file_base, child_ns);
  child_ns->sym->full_name="";
  this->ns=child_ns;
}

void compiler_parser_Parser_parse_toplevel(compiler_ast_program_Program *program, char *filename, bool include_stdlib, char *file_contents, bool include_workspace_main) {
  if (setjmp(program->err_jmp_ctx) > 0) {
  return;
  }
  compiler_parser_Parser parser = compiler_parser_Parser_make(program, program->global);
  if (include_stdlib) {
    compiler_parser_Parser_find_and_import_stdlib(&parser);
  } else {
    compiler_parser_Parser_include_prelude_only(&parser);
  }
  compiler_parser_Parser_create_namespaces_for_initial_file(&parser, filename, false);
  compiler_parser_Parser_load_file(&parser, filename, file_contents);
  compiler_ast_program_Namespace *file_ns = parser.ns;
  if (include_workspace_main && ((bool)file_ns->internal_project_root)) {
    char *potential_main = format_string("%s/main.oc", file_ns->internal_project_root->path);
    if (std_fs_file_exists(potential_main)) {
      compiler_ast_program_Namespace *main_ns = std_map_Map__3_get(file_ns->internal_project_root->namespaces, "main", NULL);
      if (main_ns==file_ns) {
      return;
      }
      if (!((bool)main_ns)) {
        compiler_ast_program_Namespace *root = file_ns->internal_project_root;
        main_ns=compiler_ast_program_Namespace_new(root, "main");
        main_ns->sym=compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType_Namespace, root, root->sym, "main", std_span_Span_default());
        main_ns->sym->u.ns=main_ns;
        std_map_Map__3_insert(root->namespaces, "main", main_ns);
      }
      parser.ns=main_ns;
      compiler_parser_Parser_load_file(&parser, potential_main, NULL);
    } else {
      str_free(&potential_main);
    }
  }
}

compiler_lexer_Lexer compiler_lexer_Lexer_make(char *source, char *filename) {
  std_span_Location start_loc = (std_span_Location){.filename=filename, .line=1, .col=1, .index=0};
  return (compiler_lexer_Lexer){.source=source, .source_len=strlen(source), .i=0, .loc=start_loc, .seen_newline=false, .tokens=std_vector_Vector__9_new(16), .errors=std_vector_Vector__11_new(16), .in_comment=false, .comment=std_buffer_Buffer_make(16), .comment_start=start_loc};
}

void compiler_lexer_Lexer_push(compiler_lexer_Lexer *this, compiler_tokens_Token *token) {
  token->seen_newline=this->seen_newline;
  if (this->comment.size > 0) {
    token->comment=std_buffer_Buffer_new_str(this->comment);
    token->comment_loc=this->comment_start;
  }
  std_buffer_Buffer_clear(&this->comment);
  std_vector_Vector__9_push(this->tokens, token);
  this->seen_newline=false;
  this->in_comment=false;
}

void compiler_lexer_Lexer_push_type(compiler_lexer_Lexer *this, compiler_tokens_TokenType type, u32 len) {
  std_span_Location start_loc = this->loc;
  for (u32 i = 0; i < len; i+=1) {
    compiler_lexer_Lexer_inc(this);
  }
  compiler_lexer_Lexer_push(this, compiler_tokens_Token_from_type(type, (std_span_Span){.start=start_loc, .end=this->loc}));
}

char compiler_lexer_Lexer_cur(compiler_lexer_Lexer *this) {
  return this->source[this->i];
}

void compiler_lexer_Lexer_inc(compiler_lexer_Lexer *this) {
  switch (compiler_lexer_Lexer_cur(this)) {
    case '\n': {
      this->loc.line+=1;
      this->loc.col=1;
      this->seen_newline=true;
    } break;
    default: {
      this->loc.col+=1;
    } break;
  }
  this->i+=1;
  this->loc.index+=1;
}

char compiler_lexer_Lexer_peek(compiler_lexer_Lexer *this, u32 offset) {
  if (compiler_lexer_Lexer_cur(this)=='\0') {
    return compiler_lexer_Lexer_cur(this);
  }
  return this->source[(this->i + offset)];
}

void compiler_lexer_Lexer_lex_char_literal(compiler_lexer_Lexer *this) {
  std_span_Location start_loc = this->loc;
  u32 start = (this->i + 1);
  compiler_lexer_Lexer_inc(this);
  if (compiler_lexer_Lexer_cur(this)=='\\') {
    compiler_lexer_Lexer_inc(this);
  }
  compiler_lexer_Lexer_inc(this);
  if (compiler_lexer_Lexer_cur(this) != '\'') {
    std_vector_Vector__11_push(this->errors, compiler_errors_Error_new((std_span_Span){.start=this->loc, .end=this->loc}, "Expected ' after character literal"));
  }
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  compiler_lexer_Lexer_inc(this);
  compiler_lexer_Lexer_push(this, compiler_tokens_Token_new(compiler_tokens_TokenType_CharLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text));
}

void compiler_lexer_Lexer_lex_string_literal(compiler_lexer_Lexer *this, bool has_seen_f) {
  std_span_Location start_loc = this->loc;
  char end_char = compiler_lexer_Lexer_cur(this);
  u32 start = (this->i + 1);
  compiler_lexer_Lexer_inc(this);
  while ((this->i < this->source_len) && (compiler_lexer_Lexer_cur(this) != end_char)) {
    if (compiler_lexer_Lexer_cur(this)=='\\') {
      compiler_lexer_Lexer_inc(this);
    }
    compiler_lexer_Lexer_inc(this);
  }
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  compiler_lexer_Lexer_inc(this);
  if (this->i >= this->source_len) {
    std_vector_Vector__11_push(this->errors, compiler_errors_Error_new((std_span_Span){.start=this->loc, .end=this->loc}, "Unterminated string literal"));
  }
  if ((end_char=='`' || has_seen_f)) {
  compiler_lexer_Lexer_push(this, compiler_tokens_Token_new(compiler_tokens_TokenType_FormatStringLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text));
  } else {
  compiler_lexer_Lexer_push(this, compiler_tokens_Token_new(compiler_tokens_TokenType_StringLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text));
  }}

compiler_tokens_Token *compiler_lexer_Lexer_lex_int_literal_different_base(compiler_lexer_Lexer *this) {
  std_span_Location start_loc = this->loc;
  u32 start = this->i;
  compiler_lexer_Lexer_inc(this);
  switch (compiler_lexer_Lexer_cur(this)) {
    case 'x': {
      compiler_lexer_Lexer_inc(this);
      while ((this->i < this->source_len) && char_is_hex_digit(compiler_lexer_Lexer_cur(this))) {
        compiler_lexer_Lexer_inc(this);
      }
    } break;
    case 'b': {
      compiler_lexer_Lexer_inc(this);
      while (((this->i < this->source_len) && compiler_lexer_Lexer_cur(this)=='0') || compiler_lexer_Lexer_cur(this)=='1') {
        compiler_lexer_Lexer_inc(this);
      }
    } break;
    default: {
      ae_assert(false, "/Users/mustafa/ocen-lang/ocen/compiler/lexer.oc:150:24: Assertion failed: `false`", "Invalid base for int literal"); exit(1);
    } break;
  }
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  return compiler_tokens_Token_new(compiler_tokens_TokenType_IntLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text);
}

compiler_tokens_Token *compiler_lexer_Lexer_lex_numeric_literal_helper(compiler_lexer_Lexer *this) {
  std_span_Location start_loc = this->loc;
  if (compiler_lexer_Lexer_cur(this)=='0') {
    switch (compiler_lexer_Lexer_peek(this, 1)) {
      case 'x':
      case 'b': {
        return compiler_lexer_Lexer_lex_int_literal_different_base(this);
      } break;
      default: {
      } break;
    }
  }
  u32 start = this->i;
  compiler_tokens_TokenType token_type;
  while (char_is_digit(compiler_lexer_Lexer_cur(this))) {
    compiler_lexer_Lexer_inc(this);
  }
  if (compiler_lexer_Lexer_cur(this)=='.') {
    compiler_lexer_Lexer_inc(this);
    while (char_is_digit(compiler_lexer_Lexer_cur(this))) {
      compiler_lexer_Lexer_inc(this);
    }
    token_type=compiler_tokens_TokenType_FloatLiteral;
  } else {
    token_type=compiler_tokens_TokenType_IntLiteral;
  }
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  return compiler_tokens_Token_new(token_type, (std_span_Span){.start=start_loc, .end=this->loc}, text);
}

void compiler_lexer_Lexer_lex_numeric_literal(compiler_lexer_Lexer *this) {
  compiler_tokens_Token *token = compiler_lexer_Lexer_lex_numeric_literal_helper(this);
  if ((compiler_lexer_Lexer_cur(this)=='u' || compiler_lexer_Lexer_cur(this)=='i') || compiler_lexer_Lexer_cur(this)=='f') {
    std_span_Location start_loc = this->loc;
    u32 start = this->i;
    compiler_lexer_Lexer_inc(this);
    while ((this->i < this->source_len) && char_is_digit(compiler_lexer_Lexer_cur(this))) {
      compiler_lexer_Lexer_inc(this);
    }
    u32 len = (this->i - start);
    char *suffix = str_substring(this->source, start, len);
    token->suffix=compiler_tokens_Token_from_ident(suffix, (std_span_Span){.start=start_loc, .end=this->loc});
  }
  compiler_lexer_Lexer_push(this, token);
}

void compiler_lexer_Lexer_lex_comment(compiler_lexer_Lexer *this) {
  while (compiler_lexer_Lexer_cur(this)=='/') {
    compiler_lexer_Lexer_inc(this);
  }
  bool save_comment = false;
  if ((compiler_lexer_Lexer_cur(this)=='*' || compiler_lexer_Lexer_cur(this)=='.') || compiler_lexer_Lexer_cur(this)=='!') {
    compiler_lexer_Lexer_inc(this);
    save_comment=true;
    if (this->comment.size==0) {
      this->comment_start=this->loc;
    }
  }
  if (compiler_lexer_Lexer_cur(this)==' ' || compiler_lexer_Lexer_cur(this)=='\t') {
    compiler_lexer_Lexer_inc(this);
  }
  while ((this->i < this->source_len) && (compiler_lexer_Lexer_cur(this) != '\n')) {
    if (save_comment) {
    std_buffer_Buffer_write_char(&this->comment, compiler_lexer_Lexer_cur(this));
    }
    compiler_lexer_Lexer_inc(this);
  }
  if (save_comment) {
  std_buffer_Buffer_write_char(&this->comment, '\n');
  }
}

std_vector_Vector__9 *compiler_lexer_Lexer_lex(compiler_lexer_Lexer *this) {
  while (this->i < this->source_len) {
    char c = compiler_lexer_Lexer_cur(this);
    switch (c) {
      case ' ':
      case '\t':
      case '\v':
      case '\r':
      case '\b':
      case '\n': {
        compiler_lexer_Lexer_inc(this);
      } break;
      case ';': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Semicolon, 1);
      } break;
      case ',': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Comma, 1);
      } break;
      case '(': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_OpenParen, 1);
      } break;
      case ')': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_CloseParen, 1);
      } break;
      case '[': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_OpenSquare, 1);
      } break;
      case ']': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_CloseSquare, 1);
      } break;
      case '{': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_OpenCurly, 1);
      } break;
      case '}': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_CloseCurly, 1);
      } break;
      case '@': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_AtSign, 1);
      } break;
      case '%': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Percent, 1);
      } break;
      case '^': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Caret, 1);
      } break;
      case '&': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Ampersand, 1);
      } break;
      case '|': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Line, 1);
      } break;
      case '?': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Question, 1);
      } break;
      case '~': {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Tilde, 1);
      } break;
      case '.': {
        if ((compiler_lexer_Lexer_peek(this, 1)=='.' && compiler_lexer_Lexer_peek(this, 2)=='.')) {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Ellipsis, 3);
        } else {
        compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Dot, 1);
        }      } break;
      case '!': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case '=': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_NotEquals, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Exclamation, 1);
          } break;
        }
      } break;
      case ':': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case ':': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_ColonColon, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Colon, 1);
          } break;
        }
      } break;
      case '=': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case '=': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_EqualEquals, 2);
          } break;
          case '>': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_FatArrow, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Equals, 1);
          } break;
        }
      } break;
      case '*': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case '=': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_StarEquals, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Star, 1);
          } break;
        }
      } break;
      case '+': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case '=': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_PlusEquals, 2);
          } break;
          case '+': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_PlusPlus, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Plus, 1);
          } break;
        }
      } break;
      case '-': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case '=': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_MinusEquals, 2);
          } break;
          case '-': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_MinusMinus, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Minus, 1);
          } break;
        }
      } break;
      case '<': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case '=': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_LessThanEquals, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_LessThan, 1);
          } break;
        }
      } break;
      case '>': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case '=': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_GreaterThanEquals, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_GreaterThan, 1);
          } break;
        }
      } break;
      case '/': {
        switch (compiler_lexer_Lexer_peek(this, 1)) {
          case '/': {
            compiler_lexer_Lexer_lex_comment(this);
          } break;
          case '=': {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_SlashEquals, 2);
          } break;
          default: {
            compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_Slash, 1);
          } break;
        }
      } break;
      case '\'': {
        compiler_lexer_Lexer_lex_char_literal(this);
      } break;
      case '"':
      case '`': {
        compiler_lexer_Lexer_lex_string_literal(this, false);
      } break;
      default: {
        std_span_Location start_loc = this->loc;
        if (c=='f' && compiler_lexer_Lexer_peek(this, 1)=='"') {
          compiler_lexer_Lexer_inc(this);
          compiler_lexer_Lexer_lex_string_literal(this, true);
        } else if (char_is_digit(c)) {
        compiler_lexer_Lexer_lex_numeric_literal(this);
        } else if (char_is_alpha(c) || c=='_') {
          u32 start = this->i;
          while (char_is_alnum(compiler_lexer_Lexer_cur(this)) || compiler_lexer_Lexer_cur(this)=='_') {
            compiler_lexer_Lexer_inc(this);
          }
          u32 len = (this->i - start);
          char *text = str_substring(this->source, start, len);
          compiler_lexer_Lexer_push(this, compiler_tokens_Token_from_ident(text, (std_span_Span){.start=start_loc, .end=this->loc}));
        } else {
          std_vector_Vector__11_push(this->errors, compiler_errors_Error_new((std_span_Span){.start=this->loc, .end=this->loc}, format_string("Unrecognized char in lexer: '%c'", c)));
          compiler_lexer_Lexer_inc(this);
        }
      } break;
    }
  }
  this->seen_newline=true;
  compiler_lexer_Lexer_push_type(this, compiler_tokens_TokenType_EOF, 0);
  return this->tokens;
}

compiler_ast_program_Namespace *compiler_ast_program_Namespace_new(compiler_ast_program_Namespace *parent, char *path) {
  compiler_ast_program_Namespace *ns = std_mem_alloc__3(1);
  ns->parent=parent;
  compiler_ast_scopes_Scope *parent_scope = (((bool)parent) ? parent->scope : NULL);
  compiler_ast_program_Namespace *parent_root = (((bool)parent) ? parent->internal_project_root : NULL);
  ns->scope=compiler_ast_scopes_Scope_new(parent_scope);
  ns->functions=std_vector_Vector__6_new(16);
  ns->structs=std_vector_Vector__2_new(16);
  ns->enums=std_vector_Vector__12_new(16);
  ns->constants=std_vector_Vector__13_new(16);
  ns->variables=std_vector_Vector__13_new(16);
  ns->typedefs=std_map_Map__2_new(32);
  ns->namespaces=std_map_Map__3_new(32);
  ns->imports=std_vector_Vector__13_new(16);
  ns->exported_symbols=std_map_Map__4_new(32);
  ns->path=path;
  ns->is_a_file=false;
  ns->internal_project_root=parent_root;
  return ns;
}

compiler_ast_scopes_Symbol *compiler_ast_program_Namespace_find_importable_symbol(compiler_ast_program_Namespace *this, char *name) {
  compiler_ast_program_Namespace *item = std_map_Map__3_get(this->namespaces, name, NULL);
  if (((bool)item)) {
  return item->sym;
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(this->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_ast_nodes_Variable *var = node->u.var_decl.var;
      if (str_eq(var->sym->name, name)) {
      return var->sym;
      }
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(this->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *node = std_vector_Iterator__13_cur(&__iter);
    {
      compiler_ast_nodes_Variable *var = node->u.var_decl.var;
      if (str_eq(var->sym->name, name)) {
      return var->sym;
      }
    }
  }
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(this->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      if (func->is_method) {
      continue;
      }
      if (str_eq(func->sym->name, name)) {
      return func->sym;
      }
    }
  }
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(this->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      if (str_eq(struc->sym->name, name)) {
      return struc->sym;
      }
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(this->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    compiler_ast_nodes_Enum *enum_ = std_vector_Iterator__12_cur(&__iter);
    {
      if (str_eq(enum_->sym->name, name)) {
      return enum_->sym;
      }
    }
  }
  if (std_map_Map__4_contains(this->exported_symbols, name)) {
    return std_map_Map__4_at(this->exported_symbols, name);
  }
  compiler_types_Type *td = std_map_Map__2_get(this->typedefs, name, NULL);
  if (((bool)td)) {
  return td->sym;
  }
  return NULL;
}

compiler_ast_program_Namespace *compiler_ast_program_Namespace_get_project_root(compiler_ast_program_Namespace *this, std_span_Span span, compiler_ast_program_Program *program_for_errors) {
  if (!((bool)this->internal_project_root)) {
    compiler_ast_program_Program_error(program_for_errors, compiler_errors_Error_new_note(span, "No project root found, can't use a Project Namespace import", "A project root is a top-level directory containing a `main.oc` file."));
  }
  return this->internal_project_root;
}

compiler_ast_program_Program *compiler_ast_program_Program_new(void) {
  compiler_ast_program_Program *prog = std_mem_alloc__4(1);
  prog->global=compiler_ast_program_Namespace_new(NULL, "");
  prog->ordered_structs=std_vector_Vector__2_new(16);
  prog->global->sym=compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType_Namespace, prog->global, "", "", "", std_span_Span_default());
  prog->global->sym->u.ns=prog->global;
  prog->error_level=1;
  prog->errors=std_vector_Vector__11_new(16);
  prog->c_includes=std_vector_Vector__1_new(16);
  prog->c_flags=std_vector_Vector__1_new(16);
  prog->c_embeds=std_map_Map__6_new(32);
  prog->sources=std_map_Map__6_new(32);
  prog->library_paths=std_vector_Vector__1_new(16);
  prog->operator_overloads=std_map_Map__5_new(32);
  return prog;
}

void compiler_ast_program_Program_exit_with_errors_if_any(compiler_ast_program_Program *this) {
  if (!std_vector_Vector__11_is_empty(this->errors)) {
    compiler_errors_display_error_messages(this->errors, this->error_level);
    exit(1);
  }
}

char *compiler_ast_program_Program_get_source_text(compiler_ast_program_Program *this, std_span_Span span) {
  std_span_Location start = span.start;
  std_span_Location end = span.end;
  char *contents = ((char *)std_map_Map__6_get(this->sources, start.filename, NULL));
  if (!((bool)contents)) {
  return NULL;
  }
  u32 len = (end.index - start.index);
  return str_substring(contents, start.index, len);
}

compiler_errors_Error *compiler_ast_program_Program_error(compiler_ast_program_Program *this, compiler_errors_Error *err) {
  std_vector_Vector__11_push(this->errors, err);
  return err;
}

compiler_ast_nodes_Function *compiler_ast_program_Program_get_function_deep_copy(compiler_ast_program_Program *this, compiler_ast_nodes_Function *old, compiler_ast_program_Namespace *ns) {
  char *func_text = compiler_ast_program_Program_get_source_text(this, old->span);
  if (!((bool)func_text)) {
  return NULL;
  }
  compiler_lexer_Lexer lexer = compiler_lexer_Lexer_make(func_text, old->span.start.filename);
  lexer.loc=old->span.start;
  std_vector_Vector__9 *tokens = compiler_lexer_Lexer_lex(&lexer);
  compiler_parser_Parser parser = compiler_parser_Parser_make(this, ns);
  parser.tokens=tokens;
  parser.curr=0;
  compiler_ast_nodes_Function *func = compiler_parser_Parser_parse_function(&parser);
  func->operator_overloads=old->operator_overloads;
  return func;
}

compiler_ast_nodes_Structure *compiler_ast_program_Program_get_struct_deep_copy(compiler_ast_program_Program *this, compiler_ast_nodes_Structure *old, compiler_ast_program_Namespace *ns) {
  char *text = compiler_ast_program_Program_get_source_text(this, old->span);
  if (!((bool)text)) {
  return NULL;
  }
  compiler_lexer_Lexer lexer = compiler_lexer_Lexer_make(text, old->span.start.filename);
  lexer.loc=old->span.start;
  std_vector_Vector__9 *tokens = compiler_lexer_Lexer_lex(&lexer);
  compiler_parser_Parser parser = compiler_parser_Parser_make(this, ns);
  parser.tokens=tokens;
  parser.curr=0;
  compiler_ast_nodes_Structure *struc = compiler_parser_Parser_parse_struct(&parser);
  return struc;
}

compiler_types_Type *compiler_ast_program_Program_get_base_type(compiler_ast_program_Program *this, compiler_types_BaseType base, std_span_Span span) {
  compiler_ast_scopes_Symbol *sym = compiler_ast_scopes_Scope_lookup_local(this->global->scope, compiler_types_BaseType_str(base));
  if (((bool)sym) && sym->type==compiler_ast_scopes_SymbolType_TypeDef) {
    return sym->u.type_def;
  } else {
    compiler_ast_program_Program_error(this, compiler_errors_Error_new(span, format_string("Internal compiler error, couldn't find base type %s", compiler_types_BaseType_dbg(base))));
    return NULL;
  }
}

compiler_types_Type *compiler_ast_program_Program_get_type_by_name(compiler_ast_program_Program *this, char *name, std_span_Span span) {
  compiler_ast_scopes_Symbol *sym = compiler_ast_scopes_Scope_lookup_local(this->global->scope, name);
  if (((bool)sym) && sym->type==compiler_ast_scopes_SymbolType_TypeDef) {
    return sym->u.type_def;
  } else {
    compiler_ast_program_Program_error(this, compiler_errors_Error_new(span, format_string("Internal compiler error, couldn't find type %s", name)));
    return NULL;
  }
}

compiler_ast_program_NSIterator compiler_ast_program_Program_iter_namespaces(compiler_ast_program_Program *this) {
  return (compiler_ast_program_NSIterator){.stack=std_vector_Vector__8_new(16), .curr=this->global};
}

bool compiler_ast_program_NSIterator_has_value(compiler_ast_program_NSIterator *this) {
  return ((bool)this->curr);
}

void compiler_ast_program_NSIterator_next(compiler_ast_program_NSIterator *this) {
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(this->curr->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *ns = std_map_ValueIterator__3_cur(&__iter);
    {
      std_vector_Vector__8_push(this->stack, ns);
    }
  }
  if (std_vector_Vector__8_is_empty(this->stack)) {
    this->curr=NULL;
    std_vector_Vector__8_free(this->stack);
  } else {
    this->curr=std_vector_Vector__8_pop(this->stack);
  }
}

compiler_ast_program_Namespace *compiler_ast_program_NSIterator_cur(compiler_ast_program_NSIterator *this) {
  return this->curr;
}

compiler_ast_operators_Operator compiler_ast_operators_Operator_from_operator_overload(char *s, std_vector_Vector__4 *params) {
  return ({ compiler_ast_operators_Operator __yield_0;
    {
      char *__match_var_1 = s;
      if (str_eq(__match_var_1, "+")) {
        __yield_0 = compiler_ast_operators_Operator_Plus;
      } else if (str_eq(__match_var_1, "-")) {
        __yield_0 = ({ compiler_ast_operators_Operator __yield_1;
          switch (params->size) {
            case 1: {
              __yield_1 = compiler_ast_operators_Operator_Negate;
            } break;
            default: {
              __yield_1 = compiler_ast_operators_Operator_Minus;
            } break;
          }
;__yield_1; });
      } else if (str_eq(__match_var_1, "*")) {
        __yield_0 = compiler_ast_operators_Operator_Multiply;
      } else if (str_eq(__match_var_1, "/")) {
        __yield_0 = compiler_ast_operators_Operator_Divide;
      } else if (str_eq(__match_var_1, "==")) {
        __yield_0 = compiler_ast_operators_Operator_Equals;
      } else if (str_eq(__match_var_1, "!=")) {
        __yield_0 = compiler_ast_operators_Operator_NotEquals;
      } else if (str_eq(__match_var_1, "[]")) {
        __yield_0 = compiler_ast_operators_Operator_Index;
      } else if (str_eq(__match_var_1, "<<")) {
        __yield_0 = compiler_ast_operators_Operator_LeftShift;
      } else if (str_eq(__match_var_1, ">>")) {
        __yield_0 = compiler_ast_operators_Operator_RightShift;
      } else if (str_eq(__match_var_1, "&")) {
        __yield_0 = ({ compiler_ast_operators_Operator __yield_1;
          switch (params->size) {
            case 1: {
              __yield_1 = compiler_ast_operators_Operator_Address;
            } break;
            default: {
              __yield_1 = compiler_ast_operators_Operator_BitwiseAnd;
            } break;
          }
;__yield_1; });
      } else if (str_eq(__match_var_1, "|")) {
        __yield_0 = compiler_ast_operators_Operator_BitwiseOr;
      } else if (str_eq(__match_var_1, "+=")) {
        __yield_0 = compiler_ast_operators_Operator_PlusEquals;
      } else if (str_eq(__match_var_1, "-=")) {
        __yield_0 = compiler_ast_operators_Operator_MinusEquals;
      } else if (str_eq(__match_var_1, "*=")) {
        __yield_0 = compiler_ast_operators_Operator_MultiplyEquals;
      } else if (str_eq(__match_var_1, "/=")) {
        __yield_0 = compiler_ast_operators_Operator_DivideEquals;
      } else if (str_eq(__match_var_1, "[]=")) {
        __yield_0 = compiler_ast_operators_Operator_IndexAssign;
      } else if (str_eq(__match_var_1, "<<=")) {
        __yield_0 = compiler_ast_operators_Operator_LeftShiftEquals;
      } else if (str_eq(__match_var_1, ">>=")) {
        __yield_0 = compiler_ast_operators_Operator_RightShiftEquals;
      } else if (str_eq(__match_var_1, "%")) {
        __yield_0 = compiler_ast_operators_Operator_Modulus;
      } else if (str_eq(__match_var_1, "in")) {
        __yield_0 = compiler_ast_operators_Operator_In;
      } else if (str_eq(__match_var_1, "not")) {
        __yield_0 = compiler_ast_operators_Operator_Not;
      } else if (str_eq(__match_var_1, "?")) {
        __yield_0 = compiler_ast_operators_Operator_IsNotNull;
      } else  {
        __yield_0 = compiler_ast_operators_Operator_Error;
      }
    }
;__yield_0; });
}

compiler_ast_operators_Operator compiler_ast_operators_Operator_from_token(compiler_tokens_Token *tok) {
  return ({ compiler_ast_operators_Operator __yield_0;
    switch (tok->type) {
      case compiler_tokens_TokenType_Ampersand: {
        __yield_0 = compiler_ast_operators_Operator_BitwiseAnd;
      } break;
      case compiler_tokens_TokenType_And: {
        __yield_0 = compiler_ast_operators_Operator_And;
      } break;
      case compiler_tokens_TokenType_Caret: {
        __yield_0 = compiler_ast_operators_Operator_BitwiseXor;
      } break;
      case compiler_tokens_TokenType_EqualEquals: {
        __yield_0 = compiler_ast_operators_Operator_Equals;
      } break;
      case compiler_tokens_TokenType_Equals: {
        __yield_0 = compiler_ast_operators_Operator_Assignment;
      } break;
      case compiler_tokens_TokenType_GreaterThan: {
        __yield_0 = compiler_ast_operators_Operator_GreaterThan;
      } break;
      case compiler_tokens_TokenType_GreaterThanEquals: {
        __yield_0 = compiler_ast_operators_Operator_GreaterThanEquals;
      } break;
      case compiler_tokens_TokenType_LessThan: {
        __yield_0 = compiler_ast_operators_Operator_LessThan;
      } break;
      case compiler_tokens_TokenType_LessThanEquals: {
        __yield_0 = compiler_ast_operators_Operator_LessThanEquals;
      } break;
      case compiler_tokens_TokenType_Line: {
        __yield_0 = compiler_ast_operators_Operator_BitwiseOr;
      } break;
      case compiler_tokens_TokenType_Minus: {
        __yield_0 = compiler_ast_operators_Operator_Minus;
      } break;
      case compiler_tokens_TokenType_MinusEquals: {
        __yield_0 = compiler_ast_operators_Operator_MinusEquals;
      } break;
      case compiler_tokens_TokenType_NotEquals: {
        __yield_0 = compiler_ast_operators_Operator_NotEquals;
      } break;
      case compiler_tokens_TokenType_Or: {
        __yield_0 = compiler_ast_operators_Operator_Or;
      } break;
      case compiler_tokens_TokenType_Percent: {
        __yield_0 = compiler_ast_operators_Operator_Modulus;
      } break;
      case compiler_tokens_TokenType_Plus: {
        __yield_0 = compiler_ast_operators_Operator_Plus;
      } break;
      case compiler_tokens_TokenType_PlusEquals: {
        __yield_0 = compiler_ast_operators_Operator_PlusEquals;
      } break;
      case compiler_tokens_TokenType_Slash: {
        __yield_0 = compiler_ast_operators_Operator_Divide;
      } break;
      case compiler_tokens_TokenType_SlashEquals: {
        __yield_0 = compiler_ast_operators_Operator_DivideEquals;
      } break;
      case compiler_tokens_TokenType_Star: {
        __yield_0 = compiler_ast_operators_Operator_Multiply;
      } break;
      case compiler_tokens_TokenType_StarEquals: {
        __yield_0 = compiler_ast_operators_Operator_MultiplyEquals;
      } break;
      case compiler_tokens_TokenType_Identifier: {
        __yield_0 = ({ compiler_ast_operators_Operator __yield_1;
          {
            char *__match_var_2 = tok->text;
            if (str_eq(__match_var_2, "in")) {
              __yield_1 = compiler_ast_operators_Operator_In;
            } else  std_panic(format_string("Unhandled identifier in Operator::from_token: %s", tok->text));
          }
;__yield_1; });
      } break;
      default: std_panic(format_string("Unhandled token type in Operator::from_token: %s", compiler_tokens_TokenType_str(tok->type))); break;
    }
;__yield_0; });
}

u32 compiler_ast_operators_Operator_num_overload_params(compiler_ast_operators_Operator this) {
  return ({ u32 __yield_0;
    switch (this) {
      case compiler_ast_operators_Operator_Address:
      case compiler_ast_operators_Operator_Dereference:
      case compiler_ast_operators_Operator_Negate:
      case compiler_ast_operators_Operator_Not:
      case compiler_ast_operators_Operator_BitwiseNot:
      case compiler_ast_operators_Operator_IsNotNull:
      case compiler_ast_operators_Operator_PreIncrement:
      case compiler_ast_operators_Operator_PreDecrement:
      case compiler_ast_operators_Operator_PostIncrement:
      case compiler_ast_operators_Operator_PostDecrement: {
        __yield_0 = 1;
      } break;
      case compiler_ast_operators_Operator_And:
      case compiler_ast_operators_Operator_Assignment:
      case compiler_ast_operators_Operator_BitwiseAnd:
      case compiler_ast_operators_Operator_BitwiseOr:
      case compiler_ast_operators_Operator_LeftShiftEquals:
      case compiler_ast_operators_Operator_Divide:
      case compiler_ast_operators_Operator_RightShiftEquals:
      case compiler_ast_operators_Operator_BitwiseXor:
      case compiler_ast_operators_Operator_DivideEquals:
      case compiler_ast_operators_Operator_Equals:
      case compiler_ast_operators_Operator_GreaterThan:
      case compiler_ast_operators_Operator_GreaterThanEquals:
      case compiler_ast_operators_Operator_Index:
      case compiler_ast_operators_Operator_LeftShift:
      case compiler_ast_operators_Operator_LessThan:
      case compiler_ast_operators_Operator_LessThanEquals:
      case compiler_ast_operators_Operator_Minus:
      case compiler_ast_operators_Operator_MinusEquals:
      case compiler_ast_operators_Operator_Modulus:
      case compiler_ast_operators_Operator_Multiply:
      case compiler_ast_operators_Operator_MultiplyEquals:
      case compiler_ast_operators_Operator_Or:
      case compiler_ast_operators_Operator_Plus:
      case compiler_ast_operators_Operator_NotEquals:
      case compiler_ast_operators_Operator_PlusEquals:
      case compiler_ast_operators_Operator_RightShift:
      case compiler_ast_operators_Operator_In: {
        __yield_0 = 2;
      } break;
      case compiler_ast_operators_Operator_IndexAssign: {
        __yield_0 = 3;
      } break;
      case compiler_ast_operators_Operator_Error: {
        __yield_0 = 0;
      } break;
    }
;__yield_0; });
}

bool compiler_ast_operators_Operator_needs_lhs_pointer_for_overload(compiler_ast_operators_Operator this) {
  return ({ bool __yield_0;
    switch (this) {
      case compiler_ast_operators_Operator_MultiplyEquals: {
        __yield_0 = true;
      } break;
      case compiler_ast_operators_Operator_DivideEquals: {
        __yield_0 = true;
      } break;
      case compiler_ast_operators_Operator_PlusEquals: {
        __yield_0 = true;
      } break;
      case compiler_ast_operators_Operator_MinusEquals: {
        __yield_0 = true;
      } break;
      case compiler_ast_operators_Operator_IndexAssign: {
        __yield_0 = true;
      } break;
      case compiler_ast_operators_Operator_LeftShiftEquals: {
        __yield_0 = true;
      } break;
      case compiler_ast_operators_Operator_RightShiftEquals: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

u32 compiler_ast_operators_OperatorOverload_hash(compiler_ast_operators_OperatorOverload this) {
  u32 hash = u32_hash(((u32)this.op));
  if (((bool)this.type1)) {
    std_traits_hash_pair_hash(hash, ((u32)this.type1->base));
  }
  if (((bool)this.type2)) {
    std_traits_hash_pair_hash(hash, ((u32)this.type2->base));
  }
  if (((bool)this.type3)) {
    std_traits_hash_pair_hash(hash, ((u32)this.type3->base));
  }
  return hash;
}

bool compiler_ast_operators_OperatorOverload_eq(compiler_ast_operators_OperatorOverload this, compiler_ast_operators_OperatorOverload other) {
  return ((this.op==other.op && compiler_types_Type_eq(this.type1, other.type1, true)) && compiler_types_Type_eq(this.type2, other.type2, true)) && compiler_types_Type_eq(this.type3, other.type3, true);
}

compiler_ast_nodes_Variable *compiler_ast_nodes_Variable_new(compiler_types_Type *type) {
  compiler_ast_nodes_Variable *var = std_mem_alloc__5(1);
  var->type=type;
  var->parsed_type=type;
  return var;
}

compiler_ast_nodes_Structure *compiler_ast_nodes_Structure_new(void) {
  compiler_ast_nodes_Structure *struc = std_mem_alloc__6(1);
  struc->fields=std_vector_Vector__4_new(16);
  return struc;
}

compiler_ast_nodes_Variable *compiler_ast_nodes_Structure_get_field(compiler_ast_nodes_Structure *this, char *name) {
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(this->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      if (str_eq(field->sym->name, name)) {
        return field;
      }
    }
  }
  return NULL;
}

compiler_ast_nodes_Enum *compiler_ast_nodes_Enum_new(void) {
  compiler_ast_nodes_Enum *enum_ = std_mem_alloc__7(1);
  enum_->fields=std_vector_Vector__4_new(16);
  return enum_;
}

compiler_ast_nodes_Variable *compiler_ast_nodes_Enum_get_field(compiler_ast_nodes_Enum *this, char *name) {
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(this->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      if (str_eq(field->sym->name, name)) {
      return field;
      }
    }
  }
  return NULL;
}

compiler_ast_nodes_Function *compiler_ast_nodes_Function_new(void) {
  compiler_ast_nodes_Function *func = std_mem_alloc__8(1);
  func->params=std_vector_Vector__4_new(16);
  return func;
}

bool compiler_ast_nodes_Function_is_template_instance(compiler_ast_nodes_Function *this) {
  if (((bool)this->type) && ((bool)this->type->template_instance)) {
  return true;
  }
  if (((bool)this->parent_type) && ((bool)this->parent_type->template_instance)) {
  return true;
  }
  return false;
}

compiler_ast_nodes_Argument *compiler_ast_nodes_Argument_new(compiler_ast_nodes_AST *expr, compiler_tokens_Token *label_token) {
  compiler_ast_nodes_Argument *arg = std_mem_alloc__9(1);
  arg->expr=expr;
  if (((bool)label_token)) {
  arg->label=label_token->text;
  }
  if (((bool)label_token)) {
  arg->label_span=label_token->span;
  }
  return arg;
}

compiler_ast_nodes_ImportPart *compiler_ast_nodes_ImportPart_new(compiler_ast_nodes_ImportPartType type, std_span_Span span) {
  compiler_ast_nodes_ImportPart *part = std_mem_alloc__10(1);
  part->type=type;
  part->span=span;
  return part;
}

compiler_ast_nodes_MatchCase *compiler_ast_nodes_MatchCase_new(compiler_ast_nodes_AST *cond, compiler_ast_nodes_AST *body) {
  compiler_ast_nodes_MatchCase *_case = std_mem_alloc__11(1);
  _case->cond=cond;
  _case->body=body;
  return _case;
}

compiler_ast_nodes_AST *compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType type, std_span_Span span) {
  compiler_ast_nodes_AST *ast = std_mem_alloc__12(1);
  ast->type=type;
  ast->span=span;
  return ast;
}

compiler_ast_nodes_AST *compiler_ast_nodes_AST_new_unop(compiler_ast_operators_Operator op, std_span_Span span, compiler_ast_nodes_AST *expr) {
  compiler_ast_nodes_AST *ast = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_UnaryOp, span);
  ast->u.unary.op=op;
  ast->u.unary.expr=expr;
  return ast;
}

compiler_ast_nodes_AST *compiler_ast_nodes_AST_new_binop(compiler_ast_operators_Operator op, compiler_ast_nodes_AST *lhs, compiler_ast_nodes_AST *rhs, std_span_Span op_span) {
  std_span_Span span = std_span_Span_join(lhs->span, rhs->span);
  compiler_ast_nodes_AST *ast = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_BinaryOp, span);
  ast->u.binary.op=op;
  ast->u.binary.lhs=lhs;
  ast->u.binary.rhs=rhs;
  ast->u.binary.op_span=op_span;
  return ast;
}

compiler_ast_scopes_Symbol *compiler_ast_nodes_AST_symbol(compiler_ast_nodes_AST *this) {
  return this->resolved_symbol;
}

bool compiler_ast_nodes_AST_is_lvalue(compiler_ast_nodes_AST *this) {
  return ({ bool __yield_0;
    switch (this->type) {
      case compiler_ast_nodes_ASTType_Identifier: {
        __yield_0 = !this->u.ident.is_function;
      } break;
      case compiler_ast_nodes_ASTType_Member: {
        __yield_0 = true;
      } break;
      case compiler_ast_nodes_ASTType_UnaryOp: {
        __yield_0 = this->u.unary.op==compiler_ast_operators_Operator_Dereference;
      } break;
      case compiler_ast_nodes_ASTType_BinaryOp: {
        __yield_0 = this->u.binary.op==compiler_ast_operators_Operator_Index;
      } break;
      case compiler_ast_nodes_ASTType_NSLookup: {
        compiler_ast_scopes_Symbol *sym = this->resolved_symbol;
        return ((bool)sym) && sym->type==compiler_ast_scopes_SymbolType_Variable;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

compiler_ast_scopes_TemplateInstance *compiler_ast_scopes_TemplateInstance_new(std_vector_Vector__0 *args, compiler_ast_scopes_Symbol *parent, compiler_ast_scopes_Symbol *resolved) {
  compiler_ast_scopes_TemplateInstance *instance = std_mem_alloc__13(1);
  instance->args=args;
  instance->parent=parent;
  instance->resolved=resolved;
  return instance;
}

bool compiler_ast_scopes_TemplateInstance_matches(compiler_ast_scopes_TemplateInstance *this, std_vector_Vector__0 *other) {
  ae_assert(other->size==this->args->size, "/Users/mustafa/ocen-lang/ocen/compiler/ast/scopes.oc:38:12: Assertion failed: `other.size == .args.size`", NULL);
  for (u32 i = 0; i < other->size; i++) {
    compiler_types_Type *a = std_vector_Vector__0_at(this->args, i);
    compiler_types_Type *b = std_vector_Vector__0_at(other, i);
    if (!compiler_types_Type_eq(a, b, true)) {
    return false;
    }
  }
  return true;
}

compiler_ast_scopes_Template *compiler_ast_scopes_Template_new(std_vector_Vector__19 *params) {
  compiler_ast_scopes_Template *templ = std_mem_alloc__14(1);
  templ->params=params;
  templ->instances=std_vector_Vector__3_new(16);
  return templ;
}

char *compiler_ast_scopes_Symbol_out_name(compiler_ast_scopes_Symbol *this) {
  if (this->is_extern) {
  return this->extern_name;
  }
  return this->full_name;
}

compiler_ast_scopes_Symbol *compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType type, compiler_ast_program_Namespace *ns, char *name, char *display, char *full_name, std_span_Span span) {
  compiler_ast_scopes_Symbol *item = std_mem_alloc__15(1);
  item->name=name;
  item->display=display;
  item->references=std_vector_Vector__20_new(4);
  u32 full_name_len = strlen(full_name);
  for (u32 i = 0; i < full_name_len; i++) {
    char c = full_name[i];
    if ((i==0 && char_is_digit(c)) || (!char_is_alnum(c) && (c != '_'))) {
      full_name[i]='_';
    }
  }
  item->full_name=full_name;
  item->span=span;
  item->type=type;
  item->ns=ns;
  return item;
}

char *compiler_ast_scopes_Symbol_join_display(char *a, char *b) {
  return (strlen(a)==0 ? b : format_string("%s::%s", a, b));
}

char *compiler_ast_scopes_Symbol_join_full_name(char *a, char *b) {
  return (strlen(a)==0 ? b : format_string("%s_%s", a, b));
}

bool compiler_ast_scopes_Symbol_is_templated(compiler_ast_scopes_Symbol *this) {
  return ((bool)this->template);
}

compiler_ast_scopes_Symbol *compiler_ast_scopes_Symbol_new_with_parent(compiler_ast_scopes_SymbolType type, compiler_ast_program_Namespace *ns, compiler_ast_scopes_Symbol *parent, char *name, std_span_Span span) {
  char *display = compiler_ast_scopes_Symbol_join_display(parent->display, name);
  char *full_name = compiler_ast_scopes_Symbol_join_full_name(parent->full_name, name);
  return compiler_ast_scopes_Symbol_new(type, ns, name, display, full_name, span);
}

void compiler_ast_scopes_Symbol_update_parent(compiler_ast_scopes_Symbol *this, compiler_ast_scopes_Symbol *parent) {
  this->display=compiler_ast_scopes_Symbol_join_display(parent->display, this->name);
  if (!this->is_extern) {
    this->full_name=compiler_ast_scopes_Symbol_join_full_name(parent->full_name, this->name);
  }
}

compiler_ast_scopes_Symbol *compiler_ast_scopes_Symbol_from_local_variable(char *name, compiler_ast_nodes_Variable *var, std_span_Span span) {
  compiler_ast_scopes_Symbol *item = compiler_ast_scopes_Symbol_new(compiler_ast_scopes_SymbolType_Variable, NULL, name, name, name, span);
  item->u.var=var;
  return item;
}

compiler_ast_scopes_Symbol *compiler_ast_scopes_Symbol_remove_alias(compiler_ast_scopes_Symbol *this) {
  return ({ compiler_ast_scopes_Symbol *__yield_0;
    switch (this->type) {
      case compiler_ast_scopes_SymbolType_TypeDef: {
        if (this->u.type_def->sym != this) {
          return compiler_ast_scopes_Symbol_remove_alias(this->u.type_def->sym);
        }
        return this;
      } break;
      default: {
        __yield_0 = this;
      } break;
    }
;__yield_0; });
}

void compiler_ast_scopes_Symbol_add_reference(compiler_ast_scopes_Symbol *this, compiler_ast_scopes_ReferenceType type, std_span_Span span) {
  if (!((bool)this)) {
  return;
  }
  std_vector_Vector__20_push(this->references, (compiler_ast_scopes_Reference){.type=type, .span=span});
  if (this->type==compiler_ast_scopes_SymbolType_TypeDef && (this->u.type_def->sym != this)) {
    compiler_ast_scopes_Symbol_add_reference(this->u.type_def->sym, type, span);
    return;
  }
  compiler_types_Type *parent_type = ({ compiler_types_Type *__yield_0;
    switch (this->type) {
      case compiler_ast_scopes_SymbolType_Structure: {
        __yield_0 = this->u.struc->type;
      } break;
      case compiler_ast_scopes_SymbolType_Function: {
        __yield_0 = this->u.func->type;
      } break;
      default: {
        __yield_0 = NULL;
      } break;
    }
;__yield_0; });
  if (!((bool)parent_type)) {
  return;
  }
  if (!((bool)parent_type->template_instance)) {
  return;
  }
  compiler_ast_scopes_Symbol *child_sym = parent_type->template_instance->parent;
  compiler_ast_scopes_Symbol_add_reference(child_sym, type, span);
}

compiler_ast_scopes_Scope *compiler_ast_scopes_Scope_new(compiler_ast_scopes_Scope *parent) {
  compiler_ast_scopes_Scope *scope = std_mem_alloc__16(1);
  scope->items=std_map_Map__4_new(32);
  scope->defers=std_vector_Vector__13_new(16);
  if (((bool)parent)) {
    scope->loop_count=parent->loop_count;
    scope->can_yield=parent->can_yield;
    scope->cur_func=parent->cur_func;
    scope->parent=parent;
  }
  return scope;
}

compiler_ast_scopes_Symbol *compiler_ast_scopes_Scope_lookup_recursive(compiler_ast_scopes_Scope *this, char *name) {
  compiler_ast_scopes_Symbol *item = std_map_Map__4_get(this->items, name, NULL);
  if (((bool)item)) {
  return item;
  }
  if (((bool)this->parent)) {
  return compiler_ast_scopes_Scope_lookup_recursive(this->parent, name);
  }
  return NULL;
}

compiler_ast_scopes_Symbol *compiler_ast_scopes_Scope_lookup_local(compiler_ast_scopes_Scope *this, char *name) {
  return std_map_Map__4_get(this->items, name, NULL);
}

void compiler_ast_scopes_Scope_insert(compiler_ast_scopes_Scope *this, char *name, compiler_ast_scopes_Symbol *symbol) {
  std_map_Map__4_insert(this->items, name, symbol);
}

void compiler_lsp_typecheck_and_log_errors(compiler_ast_program_Program *program, char *path) {
  compiler_passes_run_typecheck_passes(program);
  if (program->errors->size==0) {
    if (compiler_lsp_utils_verbose) {
    printf("type-checked program successfully""\n");
    }
    return;
  }
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(program->errors); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    compiler_errors_Error *err = std_vector_Iterator__11_cur(&__iter);
    {
      if (!((bool)err->span1.start.filename)) {
      continue;
      }
      if (str_eq(err->span1.start.filename, path)) {
        std_span_Location start = err->span1.start;
        std_span_Location end = err->span1.end;
        if (compiler_lsp_utils_verbose) {
        printf("ERROR: %s:%d:%d - %d:%d :: %s""\n", start.filename, start.line, start.col, end.line, end.col, err->msg1);
        }
      }
    }
  }
}

void compiler_lsp_handle_validate(compiler_ast_program_Program *program, char *path) {
  compiler_lsp_typecheck_and_log_errors(program, path);
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(program->errors); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    compiler_errors_Error *err = std_vector_Iterator__11_cur(&__iter);
    {
      if (!((bool)err->span1.start.filename)) {
      continue;
      }
      if (!str_eq(err->span1.start.filename, path)) {
      continue;
      }
      std_value_Value *err_value = compiler_lsp_utils_gen_error_json(err);
      printf("%s\n", std_value_Value_dbg(err_value));
    }
  }
}

void compiler_lsp_handle_location_command(compiler_ast_program_Program *program, compiler_lsp_CommandType type, std_span_Location loc) {
  compiler_lsp_typecheck_and_log_errors(program, loc.filename);
  if (compiler_lsp_utils_verbose) {
  printf("Looking for location: %s:%u:%u\n", (loc).filename, (loc).line, (loc).col);
  }
  compiler_lsp_finder_Finder finder = compiler_lsp_finder_Finder_make(type, loc);
  if (!compiler_lsp_finder_Finder_find(&finder, program)) {
    if (compiler_lsp_utils_verbose) {
    printf("No result found""\n");
    }
    return;
  }
  std_value_Value *resp = ({ std_value_Value *__yield_0;
    switch (type) {
      case compiler_lsp_CommandType_Hover: {
        __yield_0 = compiler_lsp_utils_gen_hover_string_with_docs(finder.found_sym);
      } break;
      case compiler_lsp_CommandType_GoToDefinition: {
        compiler_ast_scopes_Symbol *usage = finder.found_sym;
        std_span_Span jump_span = ({ std_span_Span __yield_1;
          switch (usage->type) {
            case compiler_ast_scopes_SymbolType_Namespace: {
              __yield_1 = usage->u.ns->span;
            } break;
            default: {
              __yield_1 = usage->span;
            } break;
          }
;__yield_1; });
        __yield_0 = compiler_lsp_utils_gen_span_json_with_filename(jump_span, loc);
      } break;
      case compiler_lsp_CommandType_GoToType: {
        compiler_types_Type *typ = compiler_lsp_utils_get_symbol_typedef(finder.found_sym);
        if ((((bool)typ) && !compiler_types_Type_can_have_methods(typ)) && typ->base==compiler_types_BaseType_Pointer) {
          typ=typ->u.ptr;
        }
        if (!((bool)typ)) {
        return;
        }
        __yield_0 = compiler_lsp_utils_gen_span_json_with_filename(typ->span, loc);
      } break;
      case compiler_lsp_CommandType_Completions: {
        __yield_0 = compiler_lsp_utils_gen_completions_json(finder.found_sym, finder.found_in_ns_lookup);
      } break;
      case compiler_lsp_CommandType_References: {
        __yield_0 = compiler_lsp_utils_gen_references_json(finder.found_sym, loc);
      } break;
      case compiler_lsp_CommandType_Renames: {
        __yield_0 = compiler_lsp_utils_gen_renames_json(finder.found_sym, loc);
      } break;
      case compiler_lsp_CommandType_SignatureHelp: {
        __yield_0 = compiler_lsp_utils_gen_signature_help(finder.call, finder.active_param);
      } break;
      default: std_panic("Unhandled command type"); break;
    }
;__yield_0; });
  if (!((bool)resp)) {
  return;
  }
  std_buffer_Buffer resp_text = std_json_serialize(resp);
  printf("%.*s\n", (resp_text).size, (resp_text).data);
}

void compiler_lsp_handle_document_symbols(compiler_ast_program_Program *program, char *path) {
  compiler_lsp_typecheck_and_log_errors(program, path);
  compiler_ast_program_Namespace *doc_ns = NULL;
  for (compiler_ast_program_NSIterator __iter = compiler_ast_program_Program_iter_namespaces(program); compiler_ast_program_NSIterator_has_value(&__iter); compiler_ast_program_NSIterator_next(&__iter)) {
    compiler_ast_program_Namespace *ns = compiler_ast_program_NSIterator_cur(&__iter);
    {
      char *ns_filename = ns->span.start.filename;
      if (((bool)ns_filename) && str_eq(ns_filename, path)) {
        doc_ns=ns;
        break;
      }
    }
  }
  if (!((bool)doc_ns)) {
    if (compiler_lsp_utils_verbose) {
    printf("No namespace found for path: %s\n", path);
    }
    printf("[]""\n");
    return;
  }
  if (compiler_lsp_utils_verbose) {
  printf("Got ns: %s\n", doc_ns->sym->name);
  }
  std_value_Value *resp = compiler_lsp_utils_gen_namespace_json(doc_ns);
  if (!((bool)resp)) {
  return;
  }
  std_value_Value *children = std_value_Value_get(resp, "children", NULL);
  std_buffer_Buffer resp_text = std_json_serialize(children);
  printf("%.*s\n", (resp_text).size, (resp_text).data);
}

char *compiler_lsp_shift_args(i32 *argc, char ***argv) {
  if ((*argc)==0) {
    printf("Expected argument but got none\n");
    exit(1);
  }
  char *arg = (*argv)[0];
  (*argv)=((*argv) + 1);
  (*argc)=((*argc) - 1);
  return arg;
}

void compiler_lsp_lsp_usage(i32 code, bool full) {
  printf("Symbol:""\n");
  printf("   ./ocen [--help] [compile-options] <file>""\n");
  printf("   ./ocen lsp [--help] [lsp-options] <file>""\n");
  if (!full) {
  exit(code);
  }
  printf("--------------------------------------------------------""\n");
  printf("LSP Options:""\n");
  printf("    --show-path path   Display this file path instead of actual one""\n");
  printf("    -h <line> <col>    Type hints for the given location""\n");
  printf("    -d <line> <col>    Find definition for the given location""\n");
  printf("    -t <line> <col>    Find type for the given location""\n");
  printf("    -c <line> <col>    Completions for the given location""\n");
  printf("    -r <line> <col>    References for the given location""\n");
  printf("    -s <line> <col>    Signature Help for the given location""\n");
  printf("    -v                 Verbose output""\n");
  printf("    --doc-symbols      List all symbols in the file""\n");
  printf("    --validate         List all errors in the file""\n");
  exit(code);
}

void compiler_lsp_lsp_main(i32 argc, char **argv) {
  compiler_lsp_shift_args(&argc, &argv);
  char *show_path = NULL;
  char *file_path = NULL;
  u32 line = 0;
  u32 col = 0;
  compiler_lsp_CommandType cmd_type = compiler_lsp_CommandType_None;
  while (argc > 0) {
    char *arg = compiler_lsp_shift_args(&argc, &argv);
    {
      char *__match_var_3 = arg;
      if (str_eq(__match_var_3, "--help")) {
        compiler_lsp_lsp_usage(0, true);
      } else if (str_eq(__match_var_3, "-h") || str_eq(__match_var_3, "-d") || str_eq(__match_var_3, "-t") || str_eq(__match_var_3, "-c") || str_eq(__match_var_3, "-r") || str_eq(__match_var_3, "-s") || str_eq(__match_var_3, "--refs") || str_eq(__match_var_3, "--renames")) {
        cmd_type=({ compiler_lsp_CommandType __yield_0;
          {
            char *__match_var_4 = arg;
            if (str_eq(__match_var_4, "-h")) {
              __yield_0 = compiler_lsp_CommandType_Hover;
            } else if (str_eq(__match_var_4, "-d")) {
              __yield_0 = compiler_lsp_CommandType_GoToDefinition;
            } else if (str_eq(__match_var_4, "-t")) {
              __yield_0 = compiler_lsp_CommandType_GoToType;
            } else if (str_eq(__match_var_4, "-c")) {
              __yield_0 = compiler_lsp_CommandType_Completions;
            } else if (str_eq(__match_var_4, "-s")) {
              __yield_0 = compiler_lsp_CommandType_SignatureHelp;
            } else if (str_eq(__match_var_4, "-r") || str_eq(__match_var_4, "--refs")) {
              __yield_0 = compiler_lsp_CommandType_References;
            } else if (str_eq(__match_var_4, "--renames")) {
              __yield_0 = compiler_lsp_CommandType_Renames;
            } else  std_panic("Invalid command type");
          }
;__yield_0; });
        line=str_to_u32(compiler_lsp_shift_args(&argc, &argv));
        col=str_to_u32(compiler_lsp_shift_args(&argc, &argv));
      } else if (str_eq(__match_var_3, "--validate")) {
        cmd_type=compiler_lsp_CommandType_Validate;
      } else if (str_eq(__match_var_3, "--doc-symbols")) {
        cmd_type=compiler_lsp_CommandType_DocumentSymbols;
      } else if (str_eq(__match_var_3, "-v")) {
        compiler_lsp_utils_verbose=true;
      } else if (str_eq(__match_var_3, "--show-path")) {
        show_path=compiler_lsp_shift_args(&argc, &argv);
      } else  {
        if (((bool)file_path)) {
          printf("Too many arguments, got: %s\n", arg);
          compiler_lsp_lsp_usage(1, false);
        }
        file_path=arg;
      }
    }
  }
  if (!((bool)file_path)) {
    printf("No file path provided""\n");
    compiler_lsp_lsp_usage(1, false);
  }
  file_path=std_fs_realpath(file_path);
  if (cmd_type==compiler_lsp_CommandType_None) {
    printf("No command type provided""\n");
    compiler_lsp_lsp_usage(1, false);
  }
  if (!((bool)show_path)) {
    show_path=file_path;
  } else {
    show_path=std_fs_realpath(show_path);
  }
  compiler_ast_program_Program *program = compiler_ast_program_Program_new();
  char *ocen_root = getenv("OCEN_ROOT");
  if (!((bool)ocen_root) || strlen(ocen_root)==0) {
    ocen_root=".";
  }
  std_vector_Vector__1_push(program->library_paths, ocen_root);
  char *contents = std_buffer_Buffer_str(std_fs_read_file(file_path));
  compiler_parser_Parser_parse_toplevel(program, show_path, true, contents, true);
  switch (cmd_type) {
    case compiler_lsp_CommandType_DocumentSymbols: {
      compiler_lsp_handle_document_symbols(program, show_path);
    } break;
    case compiler_lsp_CommandType_Validate: {
      compiler_lsp_handle_validate(program, show_path);
    } break;
    default: {
      std_span_Location loc = (std_span_Location){.filename=show_path, .line=line, .col=col, .index=0};
      compiler_lsp_handle_location_command(program, cmd_type, loc);
    } break;
  }
}

char *compiler_lsp_utils_gen_func_param_string(u32 i, compiler_ast_nodes_Variable *param, bool is_non_static_method) {
  std_buffer_Buffer sb = std_buffer_Buffer_make(16);
  if (i==0 && is_non_static_method) {
    if (param->type->base==compiler_types_BaseType_Pointer) {
      std_buffer_Buffer_write_str(&sb, "&");
    }
    std_buffer_Buffer_write_str(&sb, "this");
  } else {
    if (!str_eq(param->sym->name, "")) {
      std_buffer_Buffer_write_str(&sb, param->sym->name);
      std_buffer_Buffer_write_str(&sb, ": ");
    }
    std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_type_string(param->type, false));
  }
  if (((bool)param->default_value)) {
    char *expr_str = compiler_lsp_utils_try_gen_expr_string(param->default_value);
    if (((bool)expr_str)) {
      std_buffer_Buffer_write_str(&sb, " = ");
      std_buffer_Buffer_write_str(&sb, expr_str);
    }
  }
  return std_buffer_Buffer_str(sb);
}

char *compiler_lsp_utils_gen_type_string(compiler_types_Type *type, bool full) {
  return ({ char *__yield_0;
    switch (type->base) {
      case compiler_types_BaseType_Char:
      case compiler_types_BaseType_Bool:
      case compiler_types_BaseType_Void:
      case compiler_types_BaseType_I8:
      case compiler_types_BaseType_I16:
      case compiler_types_BaseType_I32:
      case compiler_types_BaseType_I64:
      case compiler_types_BaseType_U8:
      case compiler_types_BaseType_U16:
      case compiler_types_BaseType_U32:
      case compiler_types_BaseType_U64:
      case compiler_types_BaseType_F32:
      case compiler_types_BaseType_F64: {
        __yield_0 = compiler_types_BaseType_str(type->base);
      } break;
      case compiler_types_BaseType_Pointer: {
        __yield_0 = ({ char *__yield_1;
          switch (type->u.ptr->base) {
            case compiler_types_BaseType_Void: {
              __yield_1 = "untyped_ptr";
            } break;
            case compiler_types_BaseType_Char: {
              __yield_1 = "str";
            } break;
            default: {
              __yield_1 = format_string("&%s", compiler_lsp_utils_gen_type_string(type->u.ptr, full));
            } break;
          }
;__yield_1; });
      } break;
      case compiler_types_BaseType_Array: {
        __yield_0 = format_string("&%s", compiler_lsp_utils_gen_type_string(type->u.arr.elem_type, full));
      } break;
      case compiler_types_BaseType_Structure:
      case compiler_types_BaseType_Enum:
      case compiler_types_BaseType_Alias: {
        if (full) {
        return type->sym->display;
        }
        if (!((bool)type->template_instance)) {
        return type->sym->name;
        }
        compiler_ast_scopes_TemplateInstance *instance = type->template_instance;
        std_buffer_Buffer sb = std_buffer_Buffer_make(16);
        std_buffer_Buffer_write_str(&sb, instance->parent->name);
        std_buffer_Buffer_write_str(&sb, "<");
        for (u32 i = 0; i < instance->args->size; i+=1) {
          if (i > 0) {
            std_buffer_Buffer_write_str(&sb, ", ");
          }
          std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_type_string(std_vector_Vector__0_at(instance->args, i), false));
        }
        std_buffer_Buffer_write_str(&sb, ">");
        __yield_0 = std_buffer_Buffer_str(sb);
      } break;
      case compiler_types_BaseType_Function: {
        std_buffer_Buffer sb = std_buffer_Buffer_make(16);
        compiler_types_FunctionType func_type = type->u.func;
        bool is_non_static_method = false;
        if (((bool)func_type.orig)) {
          if (func_type.orig->is_method) {
            is_non_static_method=!func_type.orig->is_static;
          }
          std_buffer_Buffer_write_str(&sb, "def ");
          std_buffer_Buffer_write_str(&sb, func_type.orig->sym->display);
        } else {
          std_buffer_Buffer_write_str(&sb, "fn");
        }
        std_buffer_Buffer_write_str(&sb, "(");
        for (u32 i = 0; i < func_type.params->size; i+=1) {
          compiler_ast_nodes_Variable *param = std_vector_Vector__4_at(func_type.params, i);
          if (i > 0) {
            std_buffer_Buffer_write_str(&sb, ", ");
          }
          std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_func_param_string(i, param, is_non_static_method));
        }
        if (func_type.is_variadic) {
          if (func_type.params->size > 0) {
            std_buffer_Buffer_write_str(&sb, ", ");
          }
          std_buffer_Buffer_write_str(&sb, "...");
        }
        std_buffer_Buffer_write_str(&sb, ")");
        if (func_type.return_type->base != compiler_types_BaseType_Void) {
          std_buffer_Buffer_write_str(&sb, ": ");
          std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_type_string(func_type.return_type, false));
        }
        __yield_0 = std_buffer_Buffer_str(sb);
      } break;
      case compiler_types_BaseType_Unresolved: {
        compiler_ast_nodes_AST *unres = type->u.unresolved;
        __yield_0 = ({ char *__yield_1;
          switch (unres->type) {
            case compiler_ast_nodes_ASTType_Identifier: {
              __yield_1 = unres->u.ident.name;
            } break;
            default: {
              __yield_1 = "<unresolved>";
            } break;
          }
;__yield_1; });
      } break;
      case compiler_types_BaseType_UnresolvedTemplate: {
        compiler_types_UnresolvedTemplate unres = type->u.unresolved_spec;
        std_buffer_Buffer sb = std_buffer_Buffer_make(16);
        std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_type_string(unres.base, full));
        std_buffer_Buffer_write_str(&sb, "<");
        for (u32 i = 0; i < unres.args->size; i+=1) {
          if (i > 0) {
            std_buffer_Buffer_write_str(&sb, ", ");
          }
          std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_type_string(std_vector_Vector__0_at(unres.args, i), false));
        }
        std_buffer_Buffer_write_str(&sb, ">");
        __yield_0 = std_buffer_Buffer_str(sb);
      } break;
      case compiler_types_BaseType_Error:
      case compiler_types_BaseType_NUM_BASE_TYPES: {
        __yield_0 = "ERROR";
      } break;
    }
;__yield_0; });
}

char *compiler_lsp_utils_try_gen_expr_string(compiler_ast_nodes_AST *expr) {
  return ({ char *__yield_0;
    switch (expr->type) {
      case compiler_ast_nodes_ASTType_BoolLiteral: {
        __yield_0 = (expr->u.bool_literal ? "true" : "false");
      } break;
      case compiler_ast_nodes_ASTType_IntLiteral:
      case compiler_ast_nodes_ASTType_FloatLiteral: {
        compiler_ast_nodes_NumLiteral *literal = &expr->u.num_literal;
        __yield_0 = ({ char *__yield_1;
          if (((bool)literal->suffix)) {
            __yield_1 = format_string("%s%s", literal->text, compiler_types_Type_str(literal->suffix));
          } else {
            __yield_1 = literal->text;
          }
;__yield_1; });
      } break;
      case compiler_ast_nodes_ASTType_StringLiteral:
      case compiler_ast_nodes_ASTType_CharLiteral: {
        __yield_0 = expr->u.string_literal;
      } break;
      case compiler_ast_nodes_ASTType_Null: {
        __yield_0 = "null";
      } break;
      case compiler_ast_nodes_ASTType_Identifier: {
        __yield_0 = expr->u.ident.name;
      } break;
      case compiler_ast_nodes_ASTType_NSLookup: {
        char *lhs_str = compiler_lsp_utils_try_gen_expr_string(expr->u.lookup.lhs);
        if (!((bool)lhs_str)) {
        return NULL;
        }
        __yield_0 = format_string("%s::%s", lhs_str, expr->u.lookup.rhs_name);
      } break;
      default: {
        __yield_0 = NULL;
      } break;
    }
;__yield_0; });
}

char *compiler_lsp_utils_gen_hover_string(compiler_ast_scopes_Symbol *sym) {
  return ({ char *__yield_0;
    switch (sym->type) {
      case compiler_ast_scopes_SymbolType_TypeDef: {
        __yield_0 = compiler_lsp_utils_gen_type_string(sym->u.type_def, true);
      } break;
      case compiler_ast_scopes_SymbolType_Function: {
        __yield_0 = compiler_lsp_utils_gen_type_string(sym->u.func->type, true);
      } break;
      case compiler_ast_scopes_SymbolType_Variable: {
        std_buffer_Buffer sb = std_buffer_Buffer_make(16);
        std_buffer_Buffer_write_str(&sb, sym->display);
        std_buffer_Buffer_write_str(&sb, ": ");
        std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_type_string(sym->u.var->type, true));
        __yield_0 = std_buffer_Buffer_str(sb);
      } break;
      case compiler_ast_scopes_SymbolType_Constant: {
        std_buffer_Buffer sb = std_buffer_Buffer_make(16);
        std_buffer_Buffer_write_str(&sb, "const ");
        std_buffer_Buffer_write_str(&sb, sym->display);
        std_buffer_Buffer_write_str(&sb, ": ");
        std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_type_string(sym->u.var->type, true));
        __yield_0 = std_buffer_Buffer_str(sb);
      } break;
      case compiler_ast_scopes_SymbolType_Enum:
      case compiler_ast_scopes_SymbolType_EnumVariant: {
        __yield_0 = format_string("enum %s", sym->display);
      } break;
      case compiler_ast_scopes_SymbolType_Structure: {
        __yield_0 = format_string("struct %s", sym->display);
      } break;
      case compiler_ast_scopes_SymbolType_Namespace: {
        __yield_0 = format_string("namespace %s", sym->display);
      } break;
    }
;__yield_0; });
}

compiler_types_Type *compiler_lsp_utils_get_symbol_typedef(compiler_ast_scopes_Symbol *sym) {
  return ({ compiler_types_Type *__yield_0;
    switch (sym->type) {
      case compiler_ast_scopes_SymbolType_TypeDef: {
        __yield_0 = sym->u.type_def;
      } break;
      case compiler_ast_scopes_SymbolType_Function: {
        __yield_0 = sym->u.func->type;
      } break;
      case compiler_ast_scopes_SymbolType_Variable: {
        __yield_0 = sym->u.var->type;
      } break;
      case compiler_ast_scopes_SymbolType_EnumVariant: {
        __yield_0 = sym->u.var->type;
      } break;
      case compiler_ast_scopes_SymbolType_Constant: {
        __yield_0 = sym->u.var->type;
      } break;
      case compiler_ast_scopes_SymbolType_Enum: {
        __yield_0 = sym->u.enum_->type;
      } break;
      case compiler_ast_scopes_SymbolType_Structure: {
        __yield_0 = sym->u.struc->type;
      } break;
      case compiler_ast_scopes_SymbolType_Namespace: {
        __yield_0 = NULL;
      } break;
    }
;__yield_0; });
}

std_value_Value *compiler_lsp_utils_gen_error_json(compiler_errors_Error *err) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(obj, "severity", "Error");
  std_value_Value_insert(obj, "span", compiler_lsp_utils_gen_span_json(err->span1));
  std_value_Value_insert_str(obj, "message", err->msg1);
  switch (err->type) {
    case compiler_errors_ErrorType_WithHint:
    case compiler_errors_ErrorType_WithNote: {
      std_value_Value *extra_info = std_value_Value_new(std_value_ValueType_Dictionary);
      std_span_Span extra_span = ({ std_span_Span __yield_0;
        switch (err->type) {
          case compiler_errors_ErrorType_WithHint: {
            __yield_0 = (std_span_Span_is_valid(err->span2) ? err->span2 : err->span1);
          } break;
          default: {
            __yield_0 = err->span1;
          } break;
        }
;__yield_0; });
      std_value_Value_insert(extra_info, "span", compiler_lsp_utils_gen_span_json(extra_span));
      std_value_Value_insert_str(extra_info, "message", err->msg2);
      std_value_Value_insert(obj, "extra_info", extra_info);
    } break;
    case compiler_errors_ErrorType_Standard: {
    } break;
  }
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_span_json(std_span_Span span) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(obj, "start_line", std_value_Value_new_int(((i64)span.start.line)));
  std_value_Value_insert(obj, "start_col", std_value_Value_new_int(((i64)span.start.col)));
  std_value_Value_insert(obj, "end_line", std_value_Value_new_int(((i64)span.end.line)));
  std_value_Value_insert(obj, "end_col", std_value_Value_new_int(((i64)span.end.col)));
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_span_json_with_filename(std_span_Span span, std_span_Location search_loc) {
  std_value_Value *obj = compiler_lsp_utils_gen_span_json(span);
  if (!str_eq(span.start.filename, search_loc.filename)) {
    std_value_Value_insert_str(obj, "file", span.start.filename);
  }
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_hover_string_with_docs(compiler_ast_scopes_Symbol *sym) {
  std_buffer_Buffer sb = std_buffer_Buffer_make(16);
  std_buffer_Buffer_write_str(&sb, compiler_lsp_utils_gen_hover_string(sym));
  if (((bool)sym->comment)) {
    std_buffer_Buffer_write_str(&sb, "\n");
    std_buffer_Buffer_write_str(&sb, sym->comment);
  }
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(obj, "hover", std_buffer_Buffer_str(sb));
  return obj;
}

void compiler_lsp_utils_gen_type_methods_into(std_value_Value *obj, compiler_types_Type *type) {
  if (!((bool)type) || !((bool)type->methods)) {
  return;
  }
  for (std_map_Iterator__7 __iter = std_map_Map__7_iter(type->methods); std_map_Iterator__7_has_value(&__iter); std_map_Iterator__7_next(&__iter)) {
    std_map_Item__7 *it = std_map_Iterator__7_cur(&__iter);
    {
      compiler_ast_nodes_Function *func = it->value;
      std_value_Value *func_doc = std_value_Value_new(std_value_ValueType_Dictionary);
      std_value_Value_insert_str(func_doc, "name", it->key);
      std_value_Value_insert_str(func_doc, "detail", compiler_lsp_utils_gen_hover_string(func->sym));
      std_value_Value_insert_str(func_doc, "kind", "method");
      std_value_Value_insert(func_doc, "range", compiler_lsp_utils_gen_span_json(func->sym->span));
      std_value_Value_insert(func_doc, "selection_range", compiler_lsp_utils_gen_span_json(func->sym->span));
      std_value_Value_insert(func_doc, "children", std_value_Value_new(std_value_ValueType_List));
      std_value_Value_push(obj, func_doc);
    }
  }
}

std_value_Value *compiler_lsp_utils_gen_enum_json(compiler_ast_nodes_Enum *enum_) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(obj, "name", enum_->sym->name);
  std_value_Value_insert_str(obj, "detail", compiler_lsp_utils_gen_hover_string(enum_->sym));
  std_value_Value_insert_str(obj, "kind", "enum");
  std_value_Value_insert(obj, "range", compiler_lsp_utils_gen_span_json(enum_->sym->span));
  std_value_Value_insert(obj, "selection_range", compiler_lsp_utils_gen_span_json(enum_->sym->span));
  std_value_Value *children = std_value_Value_new(std_value_ValueType_List);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *member = std_vector_Iterator__4_cur(&__iter);
    {
      std_value_Value *member_obj = std_value_Value_new(std_value_ValueType_Dictionary);
      std_value_Value_insert_str(member_obj, "name", member->sym->name);
      std_value_Value_insert_str(member_obj, "kind", "enum-member");
      std_value_Value_insert(member_obj, "range", compiler_lsp_utils_gen_span_json(member->sym->span));
      std_value_Value_insert(member_obj, "selection_range", compiler_lsp_utils_gen_span_json(member->sym->span));
      std_value_Value_insert(member_obj, "children", std_value_Value_new(std_value_ValueType_List));
      std_value_Value_push(children, member_obj);
    }
  }
  compiler_lsp_utils_gen_type_methods_into(children, enum_->type);
  std_value_Value_insert(obj, "children", children);
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_struct_json(compiler_ast_nodes_Structure *struc) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(obj, "name", struc->sym->name);
  std_value_Value_insert_str(obj, "detail", compiler_lsp_utils_gen_hover_string(struc->sym));
  std_value_Value_insert_str(obj, "kind", "struct");
  std_value_Value_insert(obj, "range", compiler_lsp_utils_gen_span_json(struc->sym->span));
  std_value_Value_insert(obj, "selection_range", compiler_lsp_utils_gen_span_json(struc->sym->span));
  std_value_Value *children = std_value_Value_new(std_value_ValueType_List);
  compiler_lsp_utils_gen_type_methods_into(children, struc->type);
  std_value_Value_insert(obj, "children", children);
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_variable_json(compiler_ast_nodes_Variable *var) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(obj, "name", var->sym->name);
  std_value_Value_insert_str(obj, "detail", compiler_lsp_utils_gen_hover_string(var->sym));
  std_value_Value_insert_str(obj, "kind", "variable");
  std_value_Value_insert(obj, "range", compiler_lsp_utils_gen_span_json(var->sym->span));
  std_value_Value_insert(obj, "selection_range", compiler_lsp_utils_gen_span_json(var->sym->span));
  std_value_Value_insert(obj, "children", std_value_Value_new(std_value_ValueType_List));
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_function_json(compiler_ast_nodes_Function *func) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(obj, "name", func->sym->name);
  std_value_Value_insert_str(obj, "detail", compiler_lsp_utils_gen_hover_string(func->sym));
  std_value_Value_insert_str(obj, "kind", "function");
  std_value_Value_insert(obj, "range", compiler_lsp_utils_gen_span_json(func->sym->span));
  std_value_Value_insert(obj, "selection_range", compiler_lsp_utils_gen_span_json(func->sym->span));
  std_value_Value_insert(obj, "children", std_value_Value_new(std_value_ValueType_List));
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_namespace_json(compiler_ast_program_Namespace *ns) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(obj, "name", ns->sym->name);
  std_value_Value_insert_str(obj, "detail", compiler_lsp_utils_gen_hover_string(ns->sym));
  std_value_Value_insert_str(obj, "kind", "namespace");
  std_value_Value_insert(obj, "range", compiler_lsp_utils_gen_span_json(ns->sym->span));
  std_value_Value_insert(obj, "selection_range", compiler_lsp_utils_gen_span_json(ns->span));
  std_value_Value *children = std_value_Value_new(std_value_ValueType_List);
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    compiler_ast_nodes_Enum *enum_ = std_vector_Iterator__12_cur(&__iter);
    {
      std_value_Value_push(children, compiler_lsp_utils_gen_enum_json(enum_));
    }
  }
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      std_value_Value_push(children, compiler_lsp_utils_gen_struct_json(struc));
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *var = std_vector_Iterator__13_cur(&__iter);
    {
      std_value_Value_push(children, compiler_lsp_utils_gen_variable_json(var->u.var_decl.var));
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *var = std_vector_Iterator__13_cur(&__iter);
    {
      std_value_Value_push(children, compiler_lsp_utils_gen_variable_json(var->u.var_decl.var));
    }
  }
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      if (func->is_method) {
      continue;
      }
      std_value_Value_push(children, compiler_lsp_utils_gen_function_json(func));
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *it = std_map_ValueIterator__3_cur(&__iter);
    {
      std_value_Value_push(children, compiler_lsp_utils_gen_namespace_json(it));
    }
  }
  std_value_Value_insert(obj, "children", children);
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_completion_from_symbol(compiler_ast_scopes_Symbol *sym) {
  std_value_Value *val = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert_str(val, "label", sym->name);
  std_value_Value_insert_str(val, "detail", compiler_lsp_utils_gen_hover_string(sym));
  switch (sym->type) {
    case compiler_ast_scopes_SymbolType_Function: {
      std_value_Value_insert_str(val, "insertText", sym->name);
      std_value_Value_insert_str(val, "kind", "function");
      std_value_Value_insert_str(val, "labelDetails", compiler_lsp_utils_gen_hover_string(sym));
      if (((bool)sym->comment)) {
        std_value_Value_insert_str(val, "documentation", sym->comment);
      }
    } break;
    case compiler_ast_scopes_SymbolType_Variable: {
      std_value_Value_insert_str(val, "insertText", sym->name);
      std_value_Value_insert_str(val, "kind", "field");
      std_value_Value_insert_str(val, "labelDetails", compiler_lsp_utils_gen_hover_string(sym));
      if (((bool)sym->comment)) {
        std_value_Value_insert_str(val, "documentation", sym->comment);
      }
    } break;
    default: {
      if (compiler_lsp_utils_verbose) {
        printf("gen_completion_from_symbol: unhandled symbol type: %s\n", compiler_ast_scopes_SymbolType_dbg(sym->type));
      }
    } break;
  }
  return val;
}

std_vector_Vector__21 *compiler_lsp_utils_get_unique_reference_spans(compiler_ast_scopes_Symbol *sym, bool for_rename) {
  std_set_Set__2 *set = std_set_Set__2_new();
  for (std_vector_Iterator__20 __iter = std_vector_Vector__20_iter(sym->references); std_vector_Iterator__20_has_value(&__iter); std_vector_Iterator__20_next(&__iter)) {
    compiler_ast_scopes_Reference ref = std_vector_Iterator__20_cur(&__iter);
    {
      if (for_rename && (ref.type != compiler_ast_scopes_ReferenceType_Normal)) {
      continue;
      }
      std_set_Set__2_add(set, ref.span);
    }
  }
  std_set_Set__2_add(set, sym->span);
  std_vector_Vector__21 *vec = std_vector_Vector__21_new(16);
  for (std_set_Iterator__2 __iter = std_set_Set__2_iter(set); std_set_Iterator__2_has_value(&__iter); std_set_Iterator__2_next(&__iter)) {
    std_span_Span span = std_set_Iterator__2_cur(&__iter);
    {
      std_vector_Vector__21_push(vec, span);
    }
  }
  std_set_Set__2_free(set);
  return vec;
}

std_value_Value *compiler_lsp_utils_gen_references_json(compiler_ast_scopes_Symbol *sym, std_span_Location loc) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_List);
  std_vector_Vector__21 *spans = compiler_lsp_utils_get_unique_reference_spans(sym, false);
  for (std_vector_Iterator__21 __iter = std_vector_Vector__21_iter(spans); std_vector_Iterator__21_has_value(&__iter); std_vector_Iterator__21_next(&__iter)) {
    std_span_Span ref = std_vector_Iterator__21_cur(&__iter);
    {
      std_value_Value_push(obj, compiler_lsp_utils_gen_span_json_with_filename(ref, loc));
    }
  }
  std_vector_Vector__21_free(spans);
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_signature_help(compiler_ast_nodes_AST *node, u32 active_param) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  compiler_ast_scopes_Symbol *callee_sym = node->u.call.callee->resolved_symbol;
  if (!((bool)callee_sym) || (callee_sym->type != compiler_ast_scopes_SymbolType_Function)) {
  return obj;
  }
  compiler_ast_nodes_Function *func = callee_sym->u.func;
  std_value_Value_insert_str(obj, "label", compiler_lsp_utils_gen_type_string(func->type, true));
  std_vector_Vector__4 *params = func->params;
  std_value_Value *params_obj = std_value_Value_new(std_value_ValueType_List);
  bool is_non_static_method = (func->is_method && !func->is_static);
  for (u32 i = 0; i < params->size; i+=1) {
    compiler_ast_nodes_Variable *param = std_vector_Vector__4_at(params, i);
    std_value_Value *param_obj = std_value_Value_new(std_value_ValueType_Dictionary);
    std_value_Value_insert_str(param_obj, "label", compiler_lsp_utils_gen_func_param_string(i, param, is_non_static_method));
    std_value_Value_push(params_obj, param_obj);
  }
  std_value_Value_insert(obj, "parameters", params_obj);
  std_value_Value *sig_help = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(sig_help, "signatures", std_value_Value_new(std_value_ValueType_List));
  std_value_Value_push(std_value_Value_at_key(sig_help, "signatures"), obj);
  std_value_Value_insert_u32(sig_help, "activeSignature", 0);
  std_value_Value_insert_u32(sig_help, "activeParameter", active_param);
  return sig_help;
}

std_value_Value *compiler_lsp_utils_gen_renames_json(compiler_ast_scopes_Symbol *sym, std_span_Location loc) {
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_List);
  std_vector_Vector__21 *spans = compiler_lsp_utils_get_unique_reference_spans(sym, true);
  for (std_vector_Iterator__21 __iter = std_vector_Vector__21_iter(spans); std_vector_Iterator__21_has_value(&__iter); std_vector_Iterator__21_next(&__iter)) {
    std_span_Span ref = std_vector_Iterator__21_cur(&__iter);
    {
      u32 size = (ref.end.index - ref.start.index);
      if (str_eq(sym->name, "this") && size==1) {
        ref=(std_span_Span){.start=ref.start, .end=ref.start};
      }
      std_value_Value_push(obj, compiler_lsp_utils_gen_span_json_with_filename(ref, loc));
    }
  }
  return obj;
}

std_value_Value *compiler_lsp_utils_gen_completions_json(compiler_ast_scopes_Symbol *sym, bool ns_lookup) {
  std_value_Value *completions = std_value_Value_new(std_value_ValueType_List);
  switch (sym->type) {
    case compiler_ast_scopes_SymbolType_Structure: {
      if (!ns_lookup) {
        for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(sym->u.struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
          compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
          {
            std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(field->sym));
          }
        }
      }
      for (std_map_ValueIterator__7 __iter = std_map_Map__7_iter_values(sym->u.struc->type->methods); std_map_ValueIterator__7_has_value(&__iter); std_map_ValueIterator__7_next(&__iter)) {
        compiler_ast_nodes_Function *mth = std_map_ValueIterator__7_cur(&__iter);
        {
          std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(mth->sym));
        }
      }
    } break;
    case compiler_ast_scopes_SymbolType_TypeDef: {
      for (std_map_ValueIterator__7 __iter = std_map_Map__7_iter_values(sym->u.type_def->methods); std_map_ValueIterator__7_has_value(&__iter); std_map_ValueIterator__7_next(&__iter)) {
        compiler_ast_nodes_Function *mth = std_map_ValueIterator__7_cur(&__iter);
        {
          std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(mth->sym));
        }
      }
    } break;
    case compiler_ast_scopes_SymbolType_Enum: {
      if (ns_lookup) {
        for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(sym->u.enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
          compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
          {
            std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(field->sym));
          }
        }
      }
      for (std_map_ValueIterator__7 __iter = std_map_Map__7_iter_values(sym->u.enum_->type->methods); std_map_ValueIterator__7_has_value(&__iter); std_map_ValueIterator__7_next(&__iter)) {
        compiler_ast_nodes_Function *mth = std_map_ValueIterator__7_cur(&__iter);
        {
          std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(mth->sym));
        }
      }
    } break;
    case compiler_ast_scopes_SymbolType_Variable: {
      compiler_types_Type *typ = compiler_lsp_utils_get_symbol_typedef(sym);
      if ((((bool)typ) && !compiler_types_Type_can_have_methods(typ)) && typ->base==compiler_types_BaseType_Pointer) {
        typ=typ->u.ptr;
      }
      if (((bool)typ) && ((bool)typ->sym)) {
        return compiler_lsp_utils_gen_completions_json(typ->sym, false);
      }
    } break;
    case compiler_ast_scopes_SymbolType_Namespace: {
      for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(sym->u.ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
        compiler_ast_program_Namespace *it = std_map_ValueIterator__3_cur(&__iter);
        {
          std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(it->sym));
        }
      }
      for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(sym->u.ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
        compiler_ast_nodes_Enum *it = std_vector_Iterator__12_cur(&__iter);
        {
          std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(it->sym));
        }
      }
      for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(sym->u.ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
        compiler_ast_nodes_Structure *it = std_vector_Iterator__2_cur(&__iter);
        {
          std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(it->sym));
        }
      }
      for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(sym->u.ns->variables); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
        compiler_ast_nodes_AST *it = std_vector_Iterator__13_cur(&__iter);
        {
          if (((bool)it->resolved_symbol)) {
            std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(it->resolved_symbol));
          }
        }
      }
      for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(sym->u.ns->constants); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
        compiler_ast_nodes_AST *it = std_vector_Iterator__13_cur(&__iter);
        {
          if (((bool)it->resolved_symbol)) {
            std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(it->resolved_symbol));
          }
        }
      }
      for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(sym->u.ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
        compiler_ast_nodes_Function *it = std_vector_Iterator__6_cur(&__iter);
        {
          if (!it->is_method) {
            std_value_Value_push(completions, compiler_lsp_utils_gen_completion_from_symbol(it->sym));
          }
        }
      }
    } break;
    default: {
    } break;
  }
  std_value_Value *obj = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(obj, "completions", completions);
  return obj;
}

compiler_lsp_finder_Finder compiler_lsp_finder_Finder_make(compiler_lsp_CommandType cmd, std_span_Location loc) {
  compiler_lsp_finder_Finder finder = {0};
  finder.cmd=cmd;
  finder.loc=loc;
  return finder;
}

bool compiler_lsp_finder_Finder_find_in_identifier(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node) {
  compiler_ast_nodes_Identifier *ident = &node->u.ident;
  if (std_span_Span_contains_loc(node->span, this->loc)) {
    return compiler_lsp_finder_Finder_set_usage(this, node->resolved_symbol);
  }
  return false;
}

bool compiler_lsp_finder_Finder_find_in_var(compiler_lsp_finder_Finder *this, compiler_ast_nodes_Variable *var) {
  if (std_span_Span_contains_loc(var->sym->span, this->loc)) {
    return compiler_lsp_finder_Finder_set_usage(this, var->sym);
  }
  if (((bool)var->parsed_type) && compiler_lsp_finder_Finder_find_in_type(this, var->parsed_type)) {
    return true;
  }
  return false;
}

bool compiler_lsp_finder_Finder_set_usage(compiler_lsp_finder_Finder *this, compiler_ast_scopes_Symbol *sym) {
  this->found_sym=sym;
  return ((bool)sym);
}

bool compiler_lsp_finder_Finder_find_in_literal(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node) {
  if (std_span_Span_contains_loc(node->span, this->loc)) {
    if (((bool)node->etype)) {
      return compiler_lsp_finder_Finder_set_usage(this, node->etype->sym);
    }
  }
  return false;
}

bool compiler_lsp_finder_Finder_find_signature_help(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node, std_vector_Vector__15 *args, u32 param_idx) {
  if (this->cmd != compiler_lsp_CommandType_SignatureHelp) {
  return false;
  }
  if (!std_span_Span_contains_loc(node->span, this->loc)) {
  return false;
  }
  compiler_ast_scopes_Symbol *func = node->u.call.callee->resolved_symbol;
  if (!((bool)func) || (func->type != compiler_ast_scopes_SymbolType_Function)) {
  return false;
  }
  compiler_ast_scopes_Symbol *func_sym = func->u.func->sym;
  std_vector_Vector__4 *params = func->u.func->params;
  if (param_idx > params->size) {
  return false;
  }
  if (param_idx==0) {
    std_span_Span open_paren_span = node->u.call.open_paren_span;
    open_paren_span.end.col+=1;
    if (std_span_Span_contains_loc(open_paren_span, this->loc)) {
      this->active_param=0;
      this->call=node;
      return true;
    }
  } else {
    compiler_ast_nodes_Argument *prev_arg = std_vector_Vector__15_at(args, (param_idx - 1));
    std_span_Span arg_span = (((bool)prev_arg->label) ? std_span_Span_join(prev_arg->label_span, prev_arg->expr->span) : prev_arg->expr->span);
    if (std_span_Span_contains_loc(arg_span, this->loc)) {
      this->active_param=(param_idx - 1);
      this->call=node;
      return true;
    }
    std_span_Span close_paren_span = node->u.call.close_paren_span;
    std_span_Span mid_span = (std_span_Span){.start=arg_span.end, .end=close_paren_span.start};
    if (std_span_Span_contains_loc(mid_span, this->loc)) {
      this->active_param=param_idx;
      this->call=node;
      return true;
    }
  }
  return false;
}

bool compiler_lsp_finder_Finder_find_in_call_args(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node, std_vector_Vector__15 *args) {
  if (compiler_lsp_finder_Finder_find_signature_help(this, node, args, args->size)) {
  return true;
  }
  for (u32 i = 0; i < args->size; i+=1) {
    compiler_ast_nodes_Argument *arg = std_vector_Vector__15_at(args, i);
    if (std_span_Span_contains_loc(arg->label_span, this->loc)) {
      if (compiler_lsp_utils_verbose) {
      printf("TODO: set usage for Finder::find_in_call_args label span""\n");
      }
      return true;
    }
    if (compiler_lsp_finder_Finder_find_in_expression(this, arg->expr)) {
    return true;
    }
  }
  return false;
}

bool compiler_lsp_finder_Finder_find_in_expression(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node) {
  switch (node->type) {
    case compiler_ast_nodes_ASTType_IntLiteral:
    case compiler_ast_nodes_ASTType_FloatLiteral:
    case compiler_ast_nodes_ASTType_BoolLiteral:
    case compiler_ast_nodes_ASTType_StringLiteral:
    case compiler_ast_nodes_ASTType_CharLiteral:
    case compiler_ast_nodes_ASTType_Null: {
      return compiler_lsp_finder_Finder_find_in_literal(this, node);
    } break;
    case compiler_ast_nodes_ASTType_FormatStringLiteral: {
      compiler_ast_nodes_FormatString *fmt = &node->u.fmt_str;
      for (u32 i = 0; i < fmt->exprs->size; i+=1) {
        if (compiler_lsp_finder_Finder_find_in_expression(this, std_vector_Vector__13_at(fmt->exprs, i))) {
        return true;
        }
      }
      return compiler_lsp_finder_Finder_find_in_literal(this, node);
    } break;
    case compiler_ast_nodes_ASTType_Identifier: {
      return compiler_lsp_finder_Finder_find_in_identifier(this, node);
    } break;
    case compiler_ast_nodes_ASTType_Member: {
      if (compiler_lsp_finder_Finder_find_in_expression(this, node->u.member.lhs)) {
      return true;
      }
      if (std_span_Span_contains_loc(node->u.member.rhs_span, this->loc)) {
        return compiler_lsp_finder_Finder_set_usage(this, node->resolved_symbol);
      }
      char *rhs = node->u.member.rhs_name;
      if (!((bool)rhs) && std_span_Span_contains_loc(node->span, this->loc)) {
        return compiler_lsp_finder_Finder_set_usage(this, node->u.member.lhs->resolved_symbol);
      }
    } break;
    case compiler_ast_nodes_ASTType_NSLookup: {
      if (compiler_lsp_finder_Finder_find_in_expression(this, node->u.lookup.lhs)) {
      return true;
      }
      char *rhs = node->u.member.rhs_name;
      if (!((bool)rhs) && std_span_Span_contains_loc(node->span, this->loc)) {
        bool res = compiler_lsp_finder_Finder_set_usage(this, node->u.member.lhs->resolved_symbol);
        if (res) {
        this->found_in_ns_lookup=true;
        }
        return res;
      }
      if (std_span_Span_contains_loc(node->u.member.rhs_span, this->loc)) {
        return compiler_lsp_finder_Finder_set_usage(this, node->resolved_symbol);
      }
    } break;
    case compiler_ast_nodes_ASTType_BinaryOp: {
      compiler_ast_nodes_AST *lhs = node->u.binary.lhs;
      compiler_ast_nodes_AST *rhs = node->u.binary.rhs;
      return compiler_lsp_finder_Finder_find_in_expression(this, lhs) || compiler_lsp_finder_Finder_find_in_expression(this, rhs);
    } break;
    case compiler_ast_nodes_ASTType_UnaryOp: {
      return compiler_lsp_finder_Finder_find_in_expression(this, node->u.unary.expr);
    } break;
    case compiler_ast_nodes_ASTType_Defer:
    case compiler_ast_nodes_ASTType_Yield: {
      return compiler_lsp_finder_Finder_find_in_expression(this, node->u.child);
    } break;
    case compiler_ast_nodes_ASTType_Call: {
      compiler_ast_nodes_FuncCall *call = &node->u.call;
      if (compiler_lsp_finder_Finder_find_in_expression(this, call->callee)) {
      return true;
      }
      if (compiler_lsp_finder_Finder_find_in_call_args(this, node, call->args)) {
      return true;
      }
    } break;
    case compiler_ast_nodes_ASTType_Cast: {
      if (compiler_lsp_finder_Finder_find_in_expression(this, node->u.cast.lhs)) {
      return true;
      }
      return compiler_lsp_finder_Finder_find_in_type(this, node->u.cast.to);
    } break;
    case compiler_ast_nodes_ASTType_SizeOf: {
      return compiler_lsp_finder_Finder_find_in_type(this, node->u.size_of_type);
    } break;
    case compiler_ast_nodes_ASTType_If: {
      compiler_ast_nodes_IfStatement *stmt = &node->u.if_stmt;
      for (std_vector_Iterator__17 __iter = std_vector_Vector__17_iter(stmt->branches); std_vector_Iterator__17_has_value(&__iter); std_vector_Iterator__17_next(&__iter)) {
        compiler_ast_nodes_IfBranch branch = std_vector_Iterator__17_cur(&__iter);
        {
          if (compiler_lsp_finder_Finder_find_in_expression(this, branch.cond)) {
          return true;
          }
          if (compiler_lsp_finder_Finder_find_in_statement(this, branch.body)) {
          return true;
          }
        }
      }
      if (((bool)stmt->els) && compiler_lsp_finder_Finder_find_in_statement(this, stmt->els)) {
      return true;
      }
    } break;
    case compiler_ast_nodes_ASTType_Match: {
      compiler_ast_nodes_Match *stmt = &node->u.match_stmt;
      if (compiler_lsp_finder_Finder_find_in_expression(this, stmt->expr)) {
      return true;
      }
      for (u32 i = 0; i < stmt->cases->size; i+=1) {
        compiler_ast_nodes_MatchCase *case_ = ((compiler_ast_nodes_MatchCase *)std_vector_Vector__18_at(stmt->cases, i));
        if (compiler_lsp_finder_Finder_find_in_expression(this, case_->cond)) {
        return true;
        }
        if (((bool)case_->body) && compiler_lsp_finder_Finder_find_in_statement(this, case_->body)) {
        return true;
        }
      }
      if (((bool)stmt->defolt) && compiler_lsp_finder_Finder_find_in_statement(this, stmt->defolt)) {
      return true;
      }
    } break;
    case compiler_ast_nodes_ASTType_Break:
    case compiler_ast_nodes_ASTType_Continue: {
    } break;
    case compiler_ast_nodes_ASTType_Assert: {
      return compiler_lsp_finder_Finder_find_in_expression(this, node->u.assertion.expr);
    } break;
    case compiler_ast_nodes_ASTType_Specialization: {
      compiler_ast_nodes_Specialization *spec = &node->u.spec;
      if (compiler_lsp_finder_Finder_find_in_expression(this, spec->base)) {
      return true;
      }
      for (std_vector_Iterator__0 __iter = std_vector_Vector__0_iter(spec->parsed_template_args); std_vector_Iterator__0_has_value(&__iter); std_vector_Iterator__0_next(&__iter)) {
        compiler_types_Type *ty = std_vector_Iterator__0_cur(&__iter);
        {
          if (compiler_lsp_finder_Finder_find_in_type(this, ty)) {
          return true;
          }
        }
      }
    } break;
    case compiler_ast_nodes_ASTType_Block: {
      return compiler_lsp_finder_Finder_find_in_block(this, node);
    } break;
    case compiler_ast_nodes_ASTType_OverloadedOperator: {
      std_span_Span op_span = node->u.operator_span;
      if (std_span_Span_contains_loc(op_span, this->loc)) {
        return compiler_lsp_finder_Finder_set_usage(this, node->resolved_symbol);
      }
    } break;
    default: {
      if (compiler_lsp_utils_verbose) {
      printf("Unhandled node type in Finder::find_in_expression: %s""\n", compiler_ast_nodes_ASTType_dbg(node->type));
      }
    } break;
  }
  return false;
}

bool compiler_lsp_finder_Finder_find_in_import_part(compiler_lsp_finder_Finder *this, compiler_ast_nodes_ImportPart *part) {
  return ({ bool __yield_0;
    switch (part->type) {
      case compiler_ast_nodes_ImportPartType_Single: {
        if (std_span_Span_contains_loc(part->span, this->loc)) {
          return compiler_lsp_finder_Finder_set_usage(this, part->resolved_symbol);
        }
        return false;
      } break;
      case compiler_ast_nodes_ImportPartType_Multiple: {
        for (std_vector_Iterator__16 __iter = std_vector_Vector__16_iter(part->u.paths); std_vector_Iterator__16_has_value(&__iter); std_vector_Iterator__16_next(&__iter)) {
          std_vector_Vector__5 *subpath = std_vector_Iterator__16_cur(&__iter);
          {
            for (std_vector_Iterator__5 __iter = std_vector_Vector__5_iter(subpath); std_vector_Iterator__5_has_value(&__iter); std_vector_Iterator__5_next(&__iter)) {
              compiler_ast_nodes_ImportPart *subpart = std_vector_Iterator__5_cur(&__iter);
              {
                if (compiler_lsp_finder_Finder_find_in_import_part(this, subpart)) {
                return true;
                }
              }
            }
          }
        }
        return false;
      } break;
      case compiler_ast_nodes_ImportPartType_Wildcard: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

bool compiler_lsp_finder_Finder_find_in_statement(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node) {
  switch (node->type) {
    case compiler_ast_nodes_ASTType_While:
    case compiler_ast_nodes_ASTType_For: {
      compiler_ast_nodes_Loop *loop = &node->u.loop;
      if (((bool)loop->init) && compiler_lsp_finder_Finder_find_in_statement(this, loop->init)) {
      return true;
      }
      if (((bool)loop->cond) && compiler_lsp_finder_Finder_find_in_expression(this, loop->cond)) {
      return true;
      }
      if (((bool)loop->step) && compiler_lsp_finder_Finder_find_in_expression(this, loop->step)) {
      return true;
      }
      if (((bool)loop->body) && compiler_lsp_finder_Finder_find_in_statement(this, loop->body)) {
      return true;
      }
    } break;
    case compiler_ast_nodes_ASTType_VarDeclaration: {
      compiler_ast_nodes_VarDeclaration *decl = &node->u.var_decl;
      if (((bool)decl->var) && compiler_lsp_finder_Finder_find_in_var(this, decl->var)) {
      return true;
      }
      if (((bool)decl->init) && compiler_lsp_finder_Finder_find_in_expression(this, decl->init)) {
      return true;
      }
    } break;
    case compiler_ast_nodes_ASTType_Block: {
      return compiler_lsp_finder_Finder_find_in_block(this, node);
    } break;
    case compiler_ast_nodes_ASTType_Return: {
      return ((bool)node->u.child) && compiler_lsp_finder_Finder_find_in_expression(this, node->u.child);
    } break;
    case compiler_ast_nodes_ASTType_Import: {
      compiler_ast_nodes_Import path = node->u.import_path;
      for (std_vector_Iterator__5 __iter = std_vector_Vector__5_iter(path.parts); std_vector_Iterator__5_has_value(&__iter); std_vector_Iterator__5_next(&__iter)) {
        compiler_ast_nodes_ImportPart *part = std_vector_Iterator__5_cur(&__iter);
        {
          if (compiler_lsp_finder_Finder_find_in_import_part(this, part)) {
          return true;
          }
        }
      }
    } break;
    default: {
      return compiler_lsp_finder_Finder_find_in_expression(this, node);
    } break;
  }
  return false;
}

bool compiler_lsp_finder_Finder_find_in_block(compiler_lsp_finder_Finder *this, compiler_ast_nodes_AST *node) {
  std_vector_Vector__13 *stmts = node->u.block.statements;
  for (u32 i = 0; i < stmts->size; i+=1) {
    if (compiler_lsp_finder_Finder_find_in_statement(this, std_vector_Vector__13_at(stmts, i))) {
    return true;
    }
  }
  return false;
}

bool compiler_lsp_finder_Finder_find_in_type(compiler_lsp_finder_Finder *this, compiler_types_Type *type) {
  switch (type->base) {
    case compiler_types_BaseType_Pointer: {
      return compiler_lsp_finder_Finder_find_in_type(this, type->u.ptr);
    } break;
    case compiler_types_BaseType_Array: {
      if (((bool)type->u.arr.size_expr) && compiler_lsp_finder_Finder_find_in_expression(this, type->u.arr.size_expr)) {
      return true;
      }
      return compiler_lsp_finder_Finder_find_in_type(this, type->u.arr.elem_type);
    } break;
    case compiler_types_BaseType_Unresolved: {
      return compiler_lsp_finder_Finder_find_in_expression(this, type->u.unresolved);
    } break;
    case compiler_types_BaseType_UnresolvedTemplate: {
      compiler_types_UnresolvedTemplate spec = type->u.unresolved_spec;
      if (compiler_lsp_finder_Finder_find_in_type(this, spec.base)) {
      return true;
      }
      for (std_vector_Iterator__0 __iter = std_vector_Vector__0_iter(spec.args); std_vector_Iterator__0_has_value(&__iter); std_vector_Iterator__0_next(&__iter)) {
        compiler_types_Type *ty = std_vector_Iterator__0_cur(&__iter);
        {
          if (compiler_lsp_finder_Finder_find_in_type(this, ty)) {
          return true;
          }
        }
      }
    } break;
    default: {
      if (std_span_Span_contains_loc(type->span, this->loc)) {
        return compiler_lsp_finder_Finder_set_usage(this, type->sym);
      }
    } break;
  }
  return false;
}

bool compiler_lsp_finder_Finder_find_in_function(compiler_lsp_finder_Finder *this, compiler_ast_nodes_Function *func) {
  if (compiler_ast_nodes_Function_is_template_instance(func)) {
  return false;
  }
  if (std_span_Span_contains_loc(func->sym->span, this->loc)) {
    return compiler_lsp_finder_Finder_set_usage(this, func->sym);
  }
  if (compiler_lsp_finder_Finder_find_in_expression(this, func->name_ast)) {
  return true;
  }
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(func->params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    compiler_ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
    {
      if (compiler_lsp_finder_Finder_find_in_var(this, param)) {
      return true;
      }
    }
  }
  compiler_types_Type *ret_type = func->parsed_return_type;
  if (((bool)ret_type) && compiler_lsp_finder_Finder_find_in_type(this, ret_type)) {
  return true;
  }
  return ((bool)func->body) && compiler_lsp_finder_Finder_find_in_statement(this, func->body);
}

bool compiler_lsp_finder_Finder_find_in_program(compiler_lsp_finder_Finder *this, compiler_ast_program_Namespace *ns) {
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(ns->structs); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    compiler_ast_nodes_Structure *struc = std_vector_Iterator__2_cur(&__iter);
    {
      if (((bool)struc->type) && ((bool)struc->type->template_instance)) {
      continue;
      }
      if (std_span_Span_contains_loc(struc->sym->span, this->loc)) {
      return compiler_lsp_finder_Finder_set_usage(this, struc->sym);
      }
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
        {
          if (compiler_lsp_finder_Finder_find_in_var(this, field)) {
          return true;
          }
        }
      }
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->enums); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    compiler_ast_nodes_Enum *enm = std_vector_Iterator__12_cur(&__iter);
    {
      if (std_span_Span_contains_loc(enm->sym->span, this->loc)) {
      return compiler_lsp_finder_Finder_set_usage(this, enm->sym);
      }
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enm->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        compiler_ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
        {
          if (compiler_lsp_finder_Finder_find_in_var(this, field)) {
          return true;
          }
        }
      }
    }
  }
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(ns->functions); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    compiler_ast_nodes_Function *func = std_vector_Iterator__6_cur(&__iter);
    {
      if (std_span_Span_contains_loc(func->sym->span, this->loc)) {
      return compiler_lsp_finder_Finder_set_usage(this, func->sym);
      }
      if (compiler_lsp_finder_Finder_find_in_function(this, func)) {
      return true;
      }
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->imports); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    compiler_ast_nodes_AST *import_ = std_vector_Iterator__13_cur(&__iter);
    {
      if (compiler_lsp_finder_Finder_find_in_statement(this, import_)) {
      return true;
      }
    }
  }
  for (std_map_ValueIterator__3 __iter = std_map_Map__3_iter_values(ns->namespaces); std_map_ValueIterator__3_has_value(&__iter); std_map_ValueIterator__3_next(&__iter)) {
    compiler_ast_program_Namespace *child = std_map_ValueIterator__3_cur(&__iter);
    {
      if (std_span_Span_contains_loc(child->sym->span, this->loc)) {
      return compiler_lsp_finder_Finder_set_usage(this, child->sym);
      }
      if (compiler_lsp_finder_Finder_find_in_program(this, child)) {
      return true;
      }
    }
  }
  return false;
}

bool compiler_lsp_finder_Finder_find(compiler_lsp_finder_Finder *this, compiler_ast_program_Program *program) {
  return compiler_lsp_finder_Finder_find_in_program(this, program->global);
}

compiler_attributes_AttributeType compiler_attributes_AttributeType_from_str(char *s) {
  return ({ compiler_attributes_AttributeType __yield_0;
    {
      char *__match_var_5 = s;
      if (str_eq(__match_var_5, "extern")) {
        __yield_0 = compiler_attributes_AttributeType_Extern;
      } else if (str_eq(__match_var_5, "exits")) {
        __yield_0 = compiler_attributes_AttributeType_Exits;
      } else if (str_eq(__match_var_5, "variadic_format")) {
        __yield_0 = compiler_attributes_AttributeType_VariadicFormat;
      } else if (str_eq(__match_var_5, "export")) {
        __yield_0 = compiler_attributes_AttributeType_Export;
      } else if (str_eq(__match_var_5, "formatting")) {
        __yield_0 = compiler_attributes_AttributeType_Formatting;
      } else if (str_eq(__match_var_5, "operator")) {
        __yield_0 = compiler_attributes_AttributeType_Operator;
      } else  {
        __yield_0 = compiler_attributes_AttributeType_Invalid;
      }
    }
;__yield_0; });
}

compiler_attributes_Attribute *compiler_attributes_Attribute_new(compiler_attributes_AttributeType type, std_span_Span span) {
  compiler_attributes_Attribute *attr = std_mem_alloc__17(1);
  attr->type=type;
  attr->args=std_vector_Vector__1_new(2);
  attr->span=span;
  return attr;
}

bool compiler_attributes_Attribute_validate(compiler_attributes_Attribute *this, compiler_parser_Parser *parser_for_errors) {
  switch (this->type) {
    case compiler_attributes_AttributeType_Extern: {
      if (this->args->size > 1) {
        compiler_parser_Parser_error(parser_for_errors, compiler_errors_Error_new(this->span, "Extern attribute takes at most one argument"));
        return false;
      }
    } break;
    case compiler_attributes_AttributeType_Formatting: {
      if ((this->args->size < 1) || (this->args->size > 2)) {
        compiler_parser_Parser_error(parser_for_errors, compiler_errors_Error_new_note(this->span, "Incorrect number of arguments for formatting attribute", "Only one or two arguments are allowed"));
        return false;
      }
      if (this->args->size==2) {
        bool found_dollar = false;
        for (std_CharIterator __iter = str_chars(std_vector_Vector__1_at(this->args, 1), 0); std_CharIterator_has_value(&__iter); std_CharIterator_next(&__iter)) {
          char c = std_CharIterator_cur(&__iter);
          {
            if (c=='$') {
            found_dollar=true;
            }
          }
        }
        if (!found_dollar) {
          compiler_parser_Parser_error(parser_for_errors, compiler_errors_Error_new(this->span, "Second argument of formatting attribute must contain '$'"));
          return false;
        }
      } else {
        std_vector_Vector__1_push(this->args, "$");
      }
    } break;
    case compiler_attributes_AttributeType_Exits:
    case compiler_attributes_AttributeType_VariadicFormat:
    case compiler_attributes_AttributeType_Export: {
      if (this->args->size > 0) {
        compiler_parser_Parser_error(parser_for_errors, compiler_errors_Error_new(this->span, "Exits attribute takes no arguments"));
        return false;
      }
    } break;
    case compiler_attributes_AttributeType_Invalid: {
      compiler_parser_Parser_error(parser_for_errors, compiler_errors_Error_new(this->span, "Invalid attribute"));
      return false;
    } break;
    case compiler_attributes_AttributeType_Operator: {
      if (this->args->size != 1) {
        compiler_parser_Parser_error(parser_for_errors, compiler_errors_Error_new(this->span, "Operator attribute takes exactly one argument"));
        return false;
      }
    } break;
  }
  return true;
}

compiler_tokens_Token *compiler_tokens_Token_new(compiler_tokens_TokenType type, std_span_Span span, char *text) {
  compiler_tokens_Token *tok = std_mem_alloc__18(1);
  (*tok)=(compiler_tokens_Token){.type=type, .span=span, .text=text, .suffix=NULL, .seen_newline=false, .comment=NULL, .comment_loc=std_span_Location_default()};
  return tok;
}

compiler_tokens_Token *compiler_tokens_Token_from_type(compiler_tokens_TokenType type, std_span_Span span) {
  return compiler_tokens_Token_new(type, span, "");
}

compiler_tokens_Token *compiler_tokens_Token_from_ident(char *text, std_span_Span span) {
  compiler_tokens_TokenType type = compiler_tokens_TokenType_from_text(text);
  return compiler_tokens_Token_new(type, span, text);
}

bool compiler_tokens_Token_is_word(compiler_tokens_Token this) {
  return ({ bool __yield_0;
    switch (this.type) {
      case compiler_tokens_TokenType_Identifier: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = (((u64)this.type) > ((u64)compiler_tokens_TokenType_BEGIN_KEYWORDS));
      } break;
    }
;__yield_0; });
}

bool compiler_tokens_Token_is_identifier(compiler_tokens_Token this, char *name) {
  return ({ bool __yield_0;
    switch (this.type) {
      case compiler_tokens_TokenType_Identifier: {
        __yield_0 = str_eq(name, this.text);
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

compiler_tokens_TokenType compiler_tokens_TokenType_from_text(char *text) {
  return ({ compiler_tokens_TokenType __yield_0;
    {
      char *__match_var_6 = text;
      if (str_eq(__match_var_6, "and")) {
        __yield_0 = compiler_tokens_TokenType_And;
      } else if (str_eq(__match_var_6, "as")) {
        __yield_0 = compiler_tokens_TokenType_As;
      } else if (str_eq(__match_var_6, "assert")) {
        __yield_0 = compiler_tokens_TokenType_Assert;
      } else if (str_eq(__match_var_6, "break")) {
        __yield_0 = compiler_tokens_TokenType_Break;
      } else if (str_eq(__match_var_6, "const")) {
        __yield_0 = compiler_tokens_TokenType_Const;
      } else if (str_eq(__match_var_6, "continue")) {
        __yield_0 = compiler_tokens_TokenType_Continue;
      } else if (str_eq(__match_var_6, "def")) {
        __yield_0 = compiler_tokens_TokenType_Def;
      } else if (str_eq(__match_var_6, "defer")) {
        __yield_0 = compiler_tokens_TokenType_Defer;
      } else if (str_eq(__match_var_6, "else")) {
        __yield_0 = compiler_tokens_TokenType_Else;
      } else if (str_eq(__match_var_6, "enum")) {
        __yield_0 = compiler_tokens_TokenType_Enum;
      } else if (str_eq(__match_var_6, "extern")) {
        __yield_0 = compiler_tokens_TokenType_Extern;
      } else if (str_eq(__match_var_6, "false")) {
        __yield_0 = compiler_tokens_TokenType_False;
      } else if (str_eq(__match_var_6, "for")) {
        __yield_0 = compiler_tokens_TokenType_For;
      } else if (str_eq(__match_var_6, "fn")) {
        __yield_0 = compiler_tokens_TokenType_Fn;
      } else if (str_eq(__match_var_6, "if")) {
        __yield_0 = compiler_tokens_TokenType_If;
      } else if (str_eq(__match_var_6, "let")) {
        __yield_0 = compiler_tokens_TokenType_Let;
      } else if (str_eq(__match_var_6, "match")) {
        __yield_0 = compiler_tokens_TokenType_Match;
      } else if (str_eq(__match_var_6, "namespace")) {
        __yield_0 = compiler_tokens_TokenType_Namespace;
      } else if (str_eq(__match_var_6, "not")) {
        __yield_0 = compiler_tokens_TokenType_Not;
      } else if (str_eq(__match_var_6, "null")) {
        __yield_0 = compiler_tokens_TokenType_Null;
      } else if (str_eq(__match_var_6, "or")) {
        __yield_0 = compiler_tokens_TokenType_Or;
      } else if (str_eq(__match_var_6, "return")) {
        __yield_0 = compiler_tokens_TokenType_Return;
      } else if (str_eq(__match_var_6, "sizeof")) {
        __yield_0 = compiler_tokens_TokenType_SizeOf;
      } else if (str_eq(__match_var_6, "struct")) {
        __yield_0 = compiler_tokens_TokenType_Struct;
      } else if (str_eq(__match_var_6, "true")) {
        __yield_0 = compiler_tokens_TokenType_True;
      } else if (str_eq(__match_var_6, "then")) {
        __yield_0 = compiler_tokens_TokenType_Then;
      } else if (str_eq(__match_var_6, "typedef")) {
        __yield_0 = compiler_tokens_TokenType_TypeDef;
      } else if (str_eq(__match_var_6, "union")) {
        __yield_0 = compiler_tokens_TokenType_Union;
      } else if (str_eq(__match_var_6, "import")) {
        __yield_0 = compiler_tokens_TokenType_Import;
      } else if (str_eq(__match_var_6, "void")) {
        __yield_0 = compiler_tokens_TokenType_Void;
      } else if (str_eq(__match_var_6, "yield")) {
        __yield_0 = compiler_tokens_TokenType_Yield;
      } else if (str_eq(__match_var_6, "while")) {
        __yield_0 = compiler_tokens_TokenType_While;
      } else  {
        __yield_0 = compiler_tokens_TokenType_Identifier;
      }
    }
;__yield_0; });
}

char *compiler_tokens_TokenType_str(compiler_tokens_TokenType this) {
  return ({ char *__yield_0;
    switch (this) {
      case compiler_tokens_TokenType_And: {
        __yield_0 = "and";
      } break;
      case compiler_tokens_TokenType_As: {
        __yield_0 = "as";
      } break;
      case compiler_tokens_TokenType_Assert: {
        __yield_0 = "assert";
      } break;
      case compiler_tokens_TokenType_Break: {
        __yield_0 = "break";
      } break;
      case compiler_tokens_TokenType_Const: {
        __yield_0 = "const";
      } break;
      case compiler_tokens_TokenType_Continue: {
        __yield_0 = "continue";
      } break;
      case compiler_tokens_TokenType_Def: {
        __yield_0 = "def";
      } break;
      case compiler_tokens_TokenType_Defer: {
        __yield_0 = "defer";
      } break;
      case compiler_tokens_TokenType_Else: {
        __yield_0 = "else";
      } break;
      case compiler_tokens_TokenType_Enum: {
        __yield_0 = "enum";
      } break;
      case compiler_tokens_TokenType_Extern: {
        __yield_0 = "extern";
      } break;
      case compiler_tokens_TokenType_False: {
        __yield_0 = "false";
      } break;
      case compiler_tokens_TokenType_For: {
        __yield_0 = "for";
      } break;
      case compiler_tokens_TokenType_Fn: {
        __yield_0 = "fn";
      } break;
      case compiler_tokens_TokenType_If: {
        __yield_0 = "if";
      } break;
      case compiler_tokens_TokenType_Let: {
        __yield_0 = "let";
      } break;
      case compiler_tokens_TokenType_Match: {
        __yield_0 = "match";
      } break;
      case compiler_tokens_TokenType_Namespace: {
        __yield_0 = "namespace";
      } break;
      case compiler_tokens_TokenType_Not: {
        __yield_0 = "not";
      } break;
      case compiler_tokens_TokenType_Null: {
        __yield_0 = "null";
      } break;
      case compiler_tokens_TokenType_Or: {
        __yield_0 = "or";
      } break;
      case compiler_tokens_TokenType_Return: {
        __yield_0 = "return";
      } break;
      case compiler_tokens_TokenType_SizeOf: {
        __yield_0 = "sizeof";
      } break;
      case compiler_tokens_TokenType_Struct: {
        __yield_0 = "struct";
      } break;
      case compiler_tokens_TokenType_True: {
        __yield_0 = "true";
      } break;
      case compiler_tokens_TokenType_Then: {
        __yield_0 = "then";
      } break;
      case compiler_tokens_TokenType_TypeDef: {
        __yield_0 = "typedef";
      } break;
      case compiler_tokens_TokenType_Union: {
        __yield_0 = "union";
      } break;
      case compiler_tokens_TokenType_Import: {
        __yield_0 = "import";
      } break;
      case compiler_tokens_TokenType_Void: {
        __yield_0 = "void";
      } break;
      case compiler_tokens_TokenType_Yield: {
        __yield_0 = "yield";
      } break;
      case compiler_tokens_TokenType_While: {
        __yield_0 = "while";
      } break;
      default: {
        __yield_0 = compiler_tokens_TokenType_dbg(this);
      } break;
    }
;__yield_0; });
}

char *compiler_errors_MessageType_to_color(compiler_errors_MessageType this) {
  return ({ char *__yield_0;
    switch (this) {
      case compiler_errors_MessageType_Error: {
        __yield_0 = "\x1b[31m";
      } break;
      case compiler_errors_MessageType_Warning: {
        __yield_0 = "\x1b[33m";
      } break;
      case compiler_errors_MessageType_Note: {
        __yield_0 = "\x1b[32m";
      } break;
    }
;__yield_0; });
}

char *compiler_errors_MessageType_str(compiler_errors_MessageType this) {
  return ({ char *__yield_0;
    switch (this) {
      case compiler_errors_MessageType_Error: {
        __yield_0 = "Error";
      } break;
      case compiler_errors_MessageType_Warning: {
        __yield_0 = "Warning";
      } break;
      case compiler_errors_MessageType_Note: {
        __yield_0 = "Note";
      } break;
    }
;__yield_0; });
}

void compiler_errors_display_line(void) {
  printf("--------------------------------------------------------------------------------""\n");
}

void compiler_errors_display_message(compiler_errors_MessageType type, std_span_Span span, char *msg) {
  compiler_errors_display_line();
  char *filename = span.start.filename;
  if (str_eq(filename, "<default>") || span.start.line==0) {
    printf("%s: %s""\n", compiler_errors_MessageType_str(type), msg);
  } else {
    printf("%s: %s: %s""\n", std_span_Location_str(&span.start), compiler_errors_MessageType_str(type), msg);
  }
  compiler_errors_display_line();
}

void compiler_errors_display_message_span(compiler_errors_MessageType type, std_span_Span span, char *msg, bool line_after) {
  char *color = compiler_errors_MessageType_to_color(type);
  char *reset = "\x1b[0m";
  compiler_errors_display_message(type, span, msg);
  char *filename = span.start.filename;
  if (!std_fs_file_exists(filename)) {
  return;
  }
  std_buffer_Buffer contents = std_fs_read_file(filename);
  u32 around_offset = 1;
  u32 min_line = u32_max((span.start.line - around_offset), 1);
  u32 max_line = (span.end.line + around_offset);
  u32 line_no = 1;
  for (std_sv_SVLineIterator __iter = std_sv_SV_lines(std_buffer_Buffer_sv(contents)); std_sv_SVLineIterator_has_value(&__iter); std_sv_SVLineIterator_next(&__iter)) {
    std_sv_SV line = std_sv_SVLineIterator_cur(&__iter);
    {
      if (line_no > max_line) {
      break;
      }
      if (line_no >= min_line) {
        printf("%4d | ", line_no);
        if (line_no==span.start.line) {
          u32 start_col = (span.start.col - 1);
          u32 end_col = (span.end.col - 1);
          if (span.end.line != span.start.line) {
            end_col=line.len;
          }
          for (u32 i = 0; i < start_col; i+=1) {
            printf("%c", std_sv_SV_at(line, i));
          }
          printf("%s", color);
          for (u32 i = start_col; i < end_col; i+=1) {
            printf("%c", std_sv_SV_at(line, i));
          }
          std_sv_SV remaining_line = std_sv_SV_slice(line, end_col, 0);
          printf("%s%.*s\n", reset, (remaining_line).len, (remaining_line).data);
          printf("%*s%s^ %s%s""\n", (start_col + 7), "", color, msg, reset);
        } else {
          printf("%.*s\n", (line).len, (line).data);
        }
      }
      line_no+=1;
    }
  }
  if (line_after) {
    compiler_errors_display_line();
  }
  /* defers */
  std_buffer_Buffer_free(&contents);
}

void compiler_errors_Error_display(compiler_errors_Error *this) {
  switch (this->type) {
    case compiler_errors_ErrorType_Standard: {
      compiler_errors_display_message_span(compiler_errors_MessageType_Error, this->span1, this->msg1, true);
    } break;
    case compiler_errors_ErrorType_WithNote: {
      compiler_errors_display_message_span(compiler_errors_MessageType_Error, this->span1, this->msg1, false);
      compiler_errors_display_message(compiler_errors_MessageType_Note, this->span1, this->msg2);
    } break;
    case compiler_errors_ErrorType_WithHint: {
      compiler_errors_display_message_span(compiler_errors_MessageType_Error, this->span1, this->msg1, false);
      compiler_errors_display_message_span(compiler_errors_MessageType_Note, this->span2, this->msg2, true);
    } break;
  }
}

void compiler_errors_Error_panic(compiler_errors_Error *this) {
  compiler_errors_Error_display(this);
  exit(1);
}

compiler_errors_Error *compiler_errors_Error_new(std_span_Span span, char *msg) {
  compiler_errors_Error *err = std_mem_alloc__19(1);
  err->type=compiler_errors_ErrorType_Standard;
  err->msg1=msg;
  err->span1=span;
  return err;
}

compiler_errors_Error *compiler_errors_Error_new_note(std_span_Span span, char *msg, char *note) {
  compiler_errors_Error *err = std_mem_alloc__19(1);
  err->type=compiler_errors_ErrorType_WithNote;
  err->msg1=msg;
  err->span1=span;
  err->msg2=note;
  return err;
}

compiler_errors_Error *compiler_errors_Error_new_hint(std_span_Span span, char *msg, std_span_Span span2, char *hint) {
  compiler_errors_Error *err = std_mem_alloc__19(1);
  err->type=compiler_errors_ErrorType_WithHint;
  err->msg1=msg;
  err->span1=span;
  err->msg2=hint;
  err->span2=span2;
  return err;
}

void compiler_errors_display_error_messages(std_vector_Vector__11 *errors, u32 detail_level) {
  char *num_errors_env = getenv("OCEN_NUM_ERRORS");
  u32 max_num_errors = (((bool)num_errors_env) ? str_to_u32(num_errors_env) : 10);
  u32 num_errors = u32_min(errors->size, max_num_errors);
  for (u32 i = 0; i < num_errors; i+=1) {
    compiler_errors_Error *err = std_vector_Vector__11_at(errors, ((num_errors - i) - 1));
    switch (detail_level) {
      case 0: {
        printf("%s: %s""\n", std_span_Location_str(&err->span1.start), err->msg1);
      } break;
      case 1: {
        compiler_errors_display_message_span(compiler_errors_MessageType_Error, err->span1, err->msg1, true);
      } break;
      case 2: {
        compiler_errors_Error_display(err);
      } break;
      default: std_panic("invalid detail level"); break;
    }
  }
}

char *compiler_types_BaseType_str(compiler_types_BaseType this) {
  return ({ char *__yield_0;
    switch (this) {
      case compiler_types_BaseType_Char: {
        __yield_0 = "char";
      } break;
      case compiler_types_BaseType_Bool: {
        __yield_0 = "bool";
      } break;
      case compiler_types_BaseType_Void: {
        __yield_0 = "void";
      } break;
      case compiler_types_BaseType_I8: {
        __yield_0 = "i8";
      } break;
      case compiler_types_BaseType_I16: {
        __yield_0 = "i16";
      } break;
      case compiler_types_BaseType_I32: {
        __yield_0 = "i32";
      } break;
      case compiler_types_BaseType_I64: {
        __yield_0 = "i64";
      } break;
      case compiler_types_BaseType_U8: {
        __yield_0 = "u8";
      } break;
      case compiler_types_BaseType_U16: {
        __yield_0 = "u16";
      } break;
      case compiler_types_BaseType_U32: {
        __yield_0 = "u32";
      } break;
      case compiler_types_BaseType_U64: {
        __yield_0 = "u64";
      } break;
      case compiler_types_BaseType_F32: {
        __yield_0 = "f32";
      } break;
      case compiler_types_BaseType_F64: {
        __yield_0 = "f64";
      } break;
      default: {
        __yield_0 = compiler_types_BaseType_dbg(this);
      } break;
    }
;__yield_0; });
}

compiler_types_Type *compiler_types_Type_shallow_copy(compiler_types_Type *old) {
  compiler_types_Type *new = std_mem_alloc__20(1);
  (*new)=(*old);
  return new;
}

compiler_types_Type *compiler_types_Type_new_resolved(compiler_types_BaseType base, std_span_Span span) {
  compiler_types_Type *type = std_mem_alloc__20(1);
  type->base=base;
  type->span=span;
  type->name=compiler_types_BaseType_str(base);
  type->methods=std_map_Map__7_new(32);
  return type;
}

compiler_types_Type *compiler_types_Type_new_unresolved(char *name, std_span_Span span) {
  compiler_types_Type *type = compiler_types_Type_new_resolved(compiler_types_BaseType_Unresolved, span);
  type->name=name;
  return type;
}

compiler_types_Type *compiler_types_Type_new_unresolved_base(compiler_types_BaseType base, std_span_Span span) {
  compiler_ast_nodes_AST *ident = compiler_ast_nodes_AST_new(compiler_ast_nodes_ASTType_Identifier, span);
  ident->u.ident.name=compiler_types_BaseType_str(base);
  compiler_types_Type *type = compiler_types_Type_new_resolved(compiler_types_BaseType_Unresolved, span);
  type->u.unresolved=ident;
  return type;
}

bool compiler_types_Type_is_integer(compiler_types_Type *this) {
  return ({ bool __yield_0;
    switch (this->base) {
      case compiler_types_BaseType_I8:
      case compiler_types_BaseType_I16:
      case compiler_types_BaseType_I32:
      case compiler_types_BaseType_I64:
      case compiler_types_BaseType_U8:
      case compiler_types_BaseType_U16:
      case compiler_types_BaseType_U32:
      case compiler_types_BaseType_U64: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

bool compiler_types_Type_is_float(compiler_types_Type *this) {
  return this->base==compiler_types_BaseType_F32 || this->base==compiler_types_BaseType_F64;
}

bool compiler_types_Type_is_numeric(compiler_types_Type *this) {
  return ({ bool __yield_0;
    switch (this->base) {
      case compiler_types_BaseType_I8:
      case compiler_types_BaseType_I16:
      case compiler_types_BaseType_I32:
      case compiler_types_BaseType_I64:
      case compiler_types_BaseType_U8:
      case compiler_types_BaseType_U16:
      case compiler_types_BaseType_U32:
      case compiler_types_BaseType_U64:
      case compiler_types_BaseType_F32:
      case compiler_types_BaseType_F64: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

bool compiler_types_Type_is_numeric_or_char(compiler_types_Type *this) {
  return compiler_types_Type_is_numeric(this) || this->base==compiler_types_BaseType_Char;
}

bool compiler_types_Type_can_have_methods(compiler_types_Type *this) {
  return ({ bool __yield_0;
    switch (this->base) {
      case compiler_types_BaseType_Char:
      case compiler_types_BaseType_Bool:
      case compiler_types_BaseType_Void:
      case compiler_types_BaseType_I8:
      case compiler_types_BaseType_I16:
      case compiler_types_BaseType_I32:
      case compiler_types_BaseType_I64:
      case compiler_types_BaseType_U8:
      case compiler_types_BaseType_U16:
      case compiler_types_BaseType_U32:
      case compiler_types_BaseType_U64:
      case compiler_types_BaseType_F32:
      case compiler_types_BaseType_F64:
      case compiler_types_BaseType_Structure:
      case compiler_types_BaseType_Enum:
      case compiler_types_BaseType_Alias: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

bool compiler_types_Type_is_resolved(compiler_types_Type *this) {
  return this->base != compiler_types_BaseType_Unresolved;
}

bool compiler_types_Type_eq(compiler_types_Type *this, compiler_types_Type *other, bool strict) {
  if (this==NULL && other==NULL) {
  return true;
  }
  if (this==NULL || other==NULL) {
  return false;
  }
  if (this->base==compiler_types_BaseType_Alias) {
  return compiler_types_Type_eq(this->u.ptr, other, strict);
  }
  if (other->base==compiler_types_BaseType_Alias) {
  return compiler_types_Type_eq(this, other->u.ptr, strict);
  }
  if (this->base != other->base) {
  return false;
  }
  switch (this->base) {
    case compiler_types_BaseType_Error:
    case compiler_types_BaseType_Unresolved:
    case compiler_types_BaseType_UnresolvedTemplate: {
      return false;
    } break;
    case compiler_types_BaseType_Function: {
      compiler_types_FunctionType af = this->u.func;
      compiler_types_FunctionType bf = other->u.func;
      if (!compiler_types_Type_eq(af.return_type, bf.return_type, true)) {
      return false;
      }
      if (af.params->size != bf.params->size) {
      return false;
      }
      for (u32 i = 0; i < af.params->size; i+=1) {
        compiler_ast_nodes_Variable *a = std_vector_Vector__4_at(af.params, i);
        compiler_ast_nodes_Variable *b = std_vector_Vector__4_at(bf.params, i);
        if (!compiler_types_Type_eq(a->type, b->type, true)) {
        return false;
        }
      }
      return true;
    } break;
    case compiler_types_BaseType_Pointer: {
      if (!strict) {
        if (this->u.ptr->base==compiler_types_BaseType_Void || other->u.ptr->base==compiler_types_BaseType_Void) {
          return true;
        }
      }
      return compiler_types_Type_eq(this->u.ptr, other->u.ptr, true);
    } break;
    case compiler_types_BaseType_Structure: {
      return this->u.struc==other->u.struc;
    } break;
    case compiler_types_BaseType_Enum: {
      return this->u.enum_==other->u.enum_;
    } break;
    case compiler_types_BaseType_Array: {
      if (!compiler_types_Type_eq(this->u.arr.elem_type, other->u.arr.elem_type, true)) {
      return false;
      }
      if (this->u.arr.size_known && other->u.arr.size_known) {
        return this->u.arr.size==other->u.arr.size;
      }
      return true;
    } break;
    default: {
      if (((u32)this->base) < ((u32)compiler_types_BaseType_NUM_BASE_TYPES)) {
        return true;
      }
      ae_assert(false, "/Users/mustafa/ocen-lang/ocen/compiler/types.oc:215:20: Assertion failed: `false`", format_string("Unhandled case in Type::eq(), base = %s", compiler_types_BaseType_dbg(this->base))); exit(1);
    } break;
  }
}

bool compiler_types_Type_is_str(compiler_types_Type *this) {
  return this->base==compiler_types_BaseType_Alias && str_eq(this->sym->full_name, "str");
}

compiler_types_Type *compiler_types_Type_unaliased(compiler_types_Type *this) {
  if (this->base != compiler_types_BaseType_Alias) {
  return this;
  }
  return compiler_types_Type_unaliased(this->u.ptr);
}

compiler_types_Type *compiler_types_Type_decay_array(compiler_types_Type *this) {
  if (this->base != compiler_types_BaseType_Array) {
  return this;
  }
  compiler_types_Type *res = compiler_types_Type_new_resolved(compiler_types_BaseType_Pointer, this->span);
  res->u.ptr=this->u.arr.elem_type;
  return res;
}

char *compiler_types_Type_str(compiler_types_Type *this) {
  return ({ char *__yield_0;
    switch (this->base) {
      case compiler_types_BaseType_Pointer: {
        __yield_0 = format_string("&%s", compiler_types_Type_str(this->u.ptr));
      } break;
      case compiler_types_BaseType_Function: {
        std_buffer_Buffer buf = std_buffer_Buffer_make(16);
        std_buffer_Buffer_write_str(&buf, "fn(");
        for (u32 i = 0; i < this->u.func.params->size; i+=1) {
          compiler_ast_nodes_Variable *param = std_vector_Vector__4_at(this->u.func.params, i);
          std_buffer_Buffer_write_str(&buf, compiler_types_Type_str(param->type));
          if (i < (this->u.func.params->size - 1)) {
            std_buffer_Buffer_write_str(&buf, ", ");
          }
        }
        std_buffer_Buffer_write_str(&buf, "): ");
        std_buffer_Buffer_write_str(&buf, compiler_types_Type_str(this->u.func.return_type));
        return std_buffer_Buffer_str(buf);
      } break;
      case compiler_types_BaseType_Array: {
        std_buffer_Buffer buf = std_buffer_Buffer_make(16);
        std_buffer_Buffer_write_str(&buf, compiler_types_Type_str(this->u.arr.elem_type));
        std_buffer_Buffer_write_str(&buf, "[");
        if (this->u.arr.size_known) {
          std_buffer_Buffer_write_str(&buf, format_string("%u", this->u.arr.size));
        } else {
          std_buffer_Buffer_write_str(&buf, format_string("?%p", this));
        }
        std_buffer_Buffer_write_str(&buf, "]");
        return std_buffer_Buffer_str(buf);
      } break;
      case compiler_types_BaseType_Structure: {
        __yield_0 = this->u.struc->sym->display;
      } break;
      case compiler_types_BaseType_Enum: {
        __yield_0 = this->u.enum_->sym->display;
      } break;
      case compiler_types_BaseType_Alias: {
        __yield_0 = this->name;
      } break;
      default: {
        __yield_0 = compiler_types_BaseType_str(this->base);
      } break;
    }
;__yield_0; });
}

void usage(i32 code, bool full) {
  printf("Usage:""\n");
  printf("   ./ocen [--help] [compile-options] <file>""\n");
  printf("   ./ocen lsp [--help] [lsp-options] <file>""\n");
  if (!full) {
  exit(code);
  }
  printf("--------------------------------------------------------""\n");
  printf("Compile Options:""\n");
  printf("    -o path        Output executable (default: ./out)""\n");
  printf("    -c path        Output C code (default: {out}.c)""\n");
  printf("    --no-stdlid    Don't include the standard library""\n");
  printf("    -e0            Minimal one-line errors""\n");
  printf("    -e1            Error messages with source code (default)""\n");
  printf("    -e2            Error messages with source / hints""\n");
  printf("    -s             Silent mode (no debug output)""\n");
  printf("    -n             Don't compile C code (default: false)""\n");
  printf("    --no-dce       Don't perform dead code elimination""\n");
  printf("    -d             Emit debug information (default: false)""\n");
  printf("    -l path        Directory to search for libraries (can be used multiple times)""\n");
  printf("    --docs path    Output documentation JSON (default: none)""\n");
  printf("    --cflags flags Additional C flags (can be used multiple times)""\n");
  printf("    -h             Display this information""\n");
  printf("    -r <args>      Run executable with arguments (can only be at the end)""\n");
  exit(code);
}

void save_and_compile_code(compiler_ast_program_Program *program, char *code) {
  if (!((bool)c_path)) {
    c_path=format_string("%s.c", exec_path);
  }
  std_fs_write_file_str(c_path, code);
  if (!compile_c) {
  return;
  }
  std_buffer_Buffer cmd = std_buffer_Buffer_make(16);
  char *c_compiler = getenv("CC");
  if (!((bool)c_compiler)) {
  c_compiler="gcc";
  }
  std_buffer_Buffer_write_str_f(&cmd, format_string("%s -o %s %s", c_compiler, exec_path, c_path));
  for (std_vector_Iterator__1 __iter = std_vector_Vector__1_iter(program->c_flags); std_vector_Iterator__1_has_value(&__iter); std_vector_Iterator__1_next(&__iter)) {
    char *flag = std_vector_Iterator__1_cur(&__iter);
    {
      std_buffer_Buffer_write_str(&cmd, " ");
      std_buffer_Buffer_write_str(&cmd, flag);
    }
  }
  for (std_vector_Iterator__1 __iter = std_vector_Vector__1_iter(extra_c_flags); std_vector_Iterator__1_has_value(&__iter); std_vector_Iterator__1_next(&__iter)) {
    char *flag = std_vector_Iterator__1_cur(&__iter);
    {
      std_buffer_Buffer_write_str(&cmd, " ");
      std_buffer_Buffer_write_str(&cmd, flag);
    }
  }
  if (debug) {
  std_buffer_Buffer_write_str(&cmd, " -ggdb3");
  }
  if (!silent) {
    printf("[+] %.*s\n", (cmd).size, (cmd).data);
  }
  i32 exit_code = system(std_buffer_Buffer_str(cmd));
  if (exit_code != 0) {
    printf("[-] Compilation failed""\n");
    exit(1);
  }
}

void run_executable(i32 argc, char **argv) {
  std_buffer_Buffer cmd = std_buffer_Buffer_make(16);
  std_buffer_Buffer_write_str(&cmd, exec_path);
  for (i32 i = 0; i < argc; i++) {
    std_buffer_Buffer_write_str(&cmd, " ");
    std_buffer_Buffer_write_str(&cmd, argv[i]);
  }
  if (!silent) {
    printf("[+] %.*s\n", (cmd).size, (cmd).data);
  }
  i32 exit_code = system(std_buffer_Buffer_str(cmd));
  exit(exit_code);
}

void parse_args(i32 *argc, char ***argv, compiler_ast_program_Program *program) {
  extra_c_flags=std_vector_Vector__1_new(16);
  while ((*argc) > 0) {
    char *arg = std_shift_args(argc, argv, "here");
    {
      char *__match_var_7 = arg;
      if (str_eq(__match_var_7, "--help")) {
        usage(0, true);
      } else if (str_eq(__match_var_7, "-s")) {
        silent=true;
      } else if (str_eq(__match_var_7, "-d")) {
        debug=true;
      } else if (str_eq(__match_var_7, "-n")) {
        compile_c=false;
        program->keep_all_code=true;
      } else if (str_eq(__match_var_7, "--no-dce")) {
        program->keep_all_code=true;
      } else if (str_eq(__match_var_7, "-o")) {
        exec_path=std_shift_args(argc, argv, "here");
      } else if (str_eq(__match_var_7, "-c")) {
        c_path=std_shift_args(argc, argv, "here");
      } else if (str_eq(__match_var_7, "-l")) {
        std_vector_Vector__1_push(program->library_paths, std_shift_args(argc, argv, "here"));
      } else if (str_eq(__match_var_7, "-e0")) {
        error_level=0;
      } else if (str_eq(__match_var_7, "-e1")) {
        error_level=1;
      } else if (str_eq(__match_var_7, "-e2")) {
        error_level=2;
      } else if (str_eq(__match_var_7, "--docs")) {
        docs_path=std_shift_args(argc, argv, "here");
        program->check_doc_links=true;
      } else if (str_eq(__match_var_7, "--no-stdlib")) {
        include_stdlib=false;
      } else if (str_eq(__match_var_7, "--cflags") || str_eq(__match_var_7, "-cf")) {
        std_vector_Vector__1_push(extra_c_flags, std_shift_args(argc, argv, "here"));
      } else if (str_eq(__match_var_7, "-r") || str_eq(__match_var_7, "--run")) {
        run_after_compile=true;
        break;
      } else  {
        if (arg[0]=='-') {
          printf("Unknown option: %s""\n", arg);
          usage(1, true);
        } else if (!((bool)filename)) {
          filename=arg;
        } else {
          printf("Unknown option/argument: '%s'""\n", arg);
          usage(1, true);
        }
      }
    }
  }
  if (!((bool)filename)) {
    printf("No file specified""\n");
    usage(1, false);
  }
  if (run_after_compile && !compile_c) {
    printf("Cannot run without compiling""\n");
    usage(1, false);
  }
}

i32 main(i32 argc, char **argv) {
  std_shift_args(&argc, &argv, "here");
  if ((argc > 1) && str_eq(argv[0], "lsp")) {
    compiler_lsp_lsp_main(argc, argv);
    exit(0);
  }
  compiler_ast_program_Program *program = compiler_ast_program_Program_new();
  std_vector_Vector__1_push(program->library_paths, "");
  char *ocen_root = getenv("OCEN_ROOT");
  if (((bool)ocen_root) && (strlen(ocen_root) > 0)) {
    std_vector_Vector__1_push(program->library_paths, ocen_root);
  }
  parse_args(&argc, &argv, program);
  program->error_level=error_level;
  program->gen_debug_info=debug;
  compiler_parser_Parser_parse_toplevel(program, filename, include_stdlib, NULL, true);
  compiler_ast_program_Program_exit_with_errors_if_any(program);
  compiler_passes_run_typecheck_passes(program);
  compiler_ast_program_Program_exit_with_errors_if_any(program);
  if (((bool)docs_path)) {
    compiler_docgen_generate_doc_json(program, docs_path);
  } else {
    char *code = compiler_passes_run_codegen_passes(program);
    compiler_ast_program_Program_exit_with_errors_if_any(program);
    save_and_compile_code(program, code);
    if (run_after_compile) {
    run_executable(argc, argv);
    }
  }
}

bool compiler_utils_directory_exists(char *path) {
  DIR *dir = opendir(path);
  if (dir==NULL) {
  return false;
  }
  closedir(dir);
  return true;
}

void std_panic(char *msg) {
  printf("%s""\n", msg);
  exit(1);
}

u32 str_to_u32(char *this) {
  return ((u32)atoi(this));
}

bool str_eq(char *this, char *other) {
  void *a = ((void *)this);
  void *b = ((void *)other);
  if (a==b) {
  return true;
  }
  if (!(((bool)a) && ((bool)b))) {
  return false;
  }
  return strcmp(this, other)==0;
}

char *str_substring(char *this, u32 start, u32 len) {
  char *new_str = std_mem_alloc__21(((u32)(len + 1)));
  memcpy(new_str, &this[start], len);
  return new_str;
}

bool str_ends_with(char *this, char *suffix) {
  u32 suffix_len = strlen(suffix);
  u32 len = strlen(this);
  if (suffix_len > len) {
    return false;
  }
  for (u32 i = 0; i < suffix_len; i+=1) {
    if (this[((len - suffix_len) + i)] != suffix[i]) {
      return false;
    }
  }
  return true;
}

void str_strip_trailing_whitespace(char *this) {
  for (u32 i = (strlen(this) - 1); i >= 0; i-=1) {
    if (this[i] != ' ') {
    break;
    }
    this[i]='\0';
  }
}

void str_replace_with(char **this, char *other) {
  std_mem_free((*this));
  (*this)=other;
}

void str_free(char **this) {
  std_mem_free((*this));
  (*this)=NULL;
}

std_CharIterator str_chars(char *this, u32 start) {
  return (std_CharIterator){.data=this, .len=strlen(this), .pos=start};
}

bool std_CharIterator_has_value(std_CharIterator *this) {
  return this->pos < this->len;
}

u32 std_CharIterator_next(std_CharIterator *this) {
  return this->pos++;
}

char std_CharIterator_cur(std_CharIterator *this) {
  return this->data[this->pos];
}

bool char_is_digit(char this) {
  return isdigit(this);
}

bool char_is_hex_digit(char this) {
  return isxdigit(this);
}

bool char_is_alpha(char this) {
  return isalpha(this);
}

bool char_is_alnum(char this) {
  return isalnum(this);
}

bool char_is_print(char this) {
  return isprint(this);
}

u32 u32_min(u32 this, u32 other) {
  return ((this < other) ? this : other);
}

u32 u32_max(u32 this, u32 other) {
  return ((this > other) ? this : other);
}

char *std_shift_args(i32 *argc, char ***argv, char *where) {
  if ((*argc)==0) {
    printf("Expected argument but got none\n");
    exit(1);
  }
  char *arg = (*argv)[0];
  (*argv)=((*argv) + 1);
  (*argc)=((*argc) - 1);
  return arg;
}

bool std_sv_SV_is_empty(std_sv_SV *this) {
  return this->len==0;
}

std_sv_SV std_sv_SV_get(std_sv_SV this, u32 n) {
  return (std_sv_SV){.data=this.data, .len=n};
}

char std_sv_SV_at(std_sv_SV this, u32 n) {
  ae_assert(n < this.len, "std/sv.oc:29:12: Assertion failed: `n < .len`", "Index out of bounds in SV::at");
  return this.data[n];
}

std_sv_SV std_sv_SV_chop_by_delim(std_sv_SV *this, char delim) {
  u32 i = 0;
  while ((i < this->len) && (this->data[i] != delim)) {
    i+=1;
  }
  std_sv_SV res = std_sv_SV_get((*this), i);
  if (i==this->len) {
    this->data=(this->data + i);
    this->len-=i;
  } else {
    this->data=((this->data + i) + 1);
    this->len-=(i + 1);
  }
  return res;
}

std_sv_SV std_sv_SV_chop_line(std_sv_SV *this) {
  return std_sv_SV_chop_by_delim(this, '\n');
}

std_sv_SV std_sv_SV_slice(std_sv_SV this, u32 off, u32 end) {
  if (end==0) {
  end=this.len;
  }
  if (end > this.len) {
  end=this.len;
  }
  if (off >= end) {
  off=end;
  }
  return (std_sv_SV){.data=(this.data + off), .len=(end - off)};
}

std_sv_SVLineIterator std_sv_SV_lines(std_sv_SV this) {
  return (std_sv_SVLineIterator){.sv=this};
}

bool std_sv_SVLineIterator_has_value(std_sv_SVLineIterator *this) {
  return !std_sv_SV_is_empty(&this->sv);
}

std_sv_SV std_sv_SVLineIterator_cur(std_sv_SVLineIterator *this) {
  return std_sv_SV_chop_line(&this->sv);
}

void std_sv_SVLineIterator_next(std_sv_SVLineIterator *this) {
}

compiler_passes_reorder_structs_ReorderStructs *std_mem_alloc__0(u32 count) {
  return ((compiler_passes_reorder_structs_ReorderStructs *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_passes_reorder_structs_ReorderStructs)))));
}

compiler_passes_mark_dead_code_MarkDeadCode *std_mem_alloc__1(u32 count) {
  return ((compiler_passes_mark_dead_code_MarkDeadCode *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_passes_mark_dead_code_MarkDeadCode)))));
}

compiler_passes_generic_pass_GenericPass *std_mem_alloc__2(u32 count) {
  return ((compiler_passes_generic_pass_GenericPass *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_passes_generic_pass_GenericPass)))));
}

compiler_ast_program_Namespace *std_mem_alloc__3(u32 count) {
  return ((compiler_ast_program_Namespace *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_program_Namespace)))));
}

compiler_ast_program_Program *std_mem_alloc__4(u32 count) {
  return ((compiler_ast_program_Program *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_program_Program)))));
}

compiler_ast_nodes_Variable *std_mem_alloc__5(u32 count) {
  return ((compiler_ast_nodes_Variable *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Variable)))));
}

compiler_ast_nodes_Structure *std_mem_alloc__6(u32 count) {
  return ((compiler_ast_nodes_Structure *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Structure)))));
}

compiler_ast_nodes_Enum *std_mem_alloc__7(u32 count) {
  return ((compiler_ast_nodes_Enum *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Enum)))));
}

compiler_ast_nodes_Function *std_mem_alloc__8(u32 count) {
  return ((compiler_ast_nodes_Function *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Function)))));
}

compiler_ast_nodes_Argument *std_mem_alloc__9(u32 count) {
  return ((compiler_ast_nodes_Argument *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Argument)))));
}

compiler_ast_nodes_ImportPart *std_mem_alloc__10(u32 count) {
  return ((compiler_ast_nodes_ImportPart *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_ImportPart)))));
}

compiler_ast_nodes_MatchCase *std_mem_alloc__11(u32 count) {
  return ((compiler_ast_nodes_MatchCase *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_MatchCase)))));
}

compiler_ast_nodes_AST *std_mem_alloc__12(u32 count) {
  return ((compiler_ast_nodes_AST *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_AST)))));
}

compiler_ast_scopes_TemplateInstance *std_mem_alloc__13(u32 count) {
  return ((compiler_ast_scopes_TemplateInstance *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_scopes_TemplateInstance)))));
}

compiler_ast_scopes_Template *std_mem_alloc__14(u32 count) {
  return ((compiler_ast_scopes_Template *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_scopes_Template)))));
}

compiler_ast_scopes_Symbol *std_mem_alloc__15(u32 count) {
  return ((compiler_ast_scopes_Symbol *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_scopes_Symbol)))));
}

compiler_ast_scopes_Scope *std_mem_alloc__16(u32 count) {
  return ((compiler_ast_scopes_Scope *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_scopes_Scope)))));
}

compiler_attributes_Attribute *std_mem_alloc__17(u32 count) {
  return ((compiler_attributes_Attribute *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_attributes_Attribute)))));
}

compiler_tokens_Token *std_mem_alloc__18(u32 count) {
  return ((compiler_tokens_Token *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_tokens_Token)))));
}

compiler_errors_Error *std_mem_alloc__19(u32 count) {
  return ((compiler_errors_Error *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_errors_Error)))));
}

compiler_types_Type *std_mem_alloc__20(u32 count) {
  return ((compiler_types_Type *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_types_Type)))));
}

char *std_mem_alloc__21(u32 count) {
  return ((char *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(char)))));
}

i32 *std_mem_alloc__22(u32 count) {
  return ((i32 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(i32)))));
}

std_compact_map_Map__0 *std_mem_alloc__23(u32 count) {
  return ((std_compact_map_Map__0 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_compact_map_Map__0)))));
}

u8 *std_mem_alloc__24(u32 count) {
  return ((u8 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(u8)))));
}

std_set_Set__0 *std_mem_alloc__25(u32 count) {
  return ((std_set_Set__0 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_set_Set__0)))));
}

std_set_Set__1 *std_mem_alloc__26(u32 count) {
  return ((std_set_Set__1 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_set_Set__1)))));
}

std_set_Set__2 *std_mem_alloc__27(u32 count) {
  return ((std_set_Set__2 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_set_Set__2)))));
}

std_value_Value *std_mem_alloc__28(u32 count) {
  return ((std_value_Value *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_value_Value)))));
}

std_map_Item__0 *std_mem_alloc__29(u32 count) {
  return ((std_map_Item__0 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__0)))));
}

std_map_Item__0 **std_mem_alloc__30(u32 count) {
  return ((std_map_Item__0 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__0 *)))));
}

std_map_Map__0 *std_mem_alloc__31(u32 count) {
  return ((std_map_Map__0 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__0)))));
}

std_map_Item__1 *std_mem_alloc__32(u32 count) {
  return ((std_map_Item__1 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__1)))));
}

std_map_Item__1 **std_mem_alloc__33(u32 count) {
  return ((std_map_Item__1 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__1 *)))));
}

std_map_Map__1 *std_mem_alloc__34(u32 count) {
  return ((std_map_Map__1 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__1)))));
}

std_map_Item__2 *std_mem_alloc__35(u32 count) {
  return ((std_map_Item__2 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__2)))));
}

std_map_Item__2 **std_mem_alloc__36(u32 count) {
  return ((std_map_Item__2 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__2 *)))));
}

std_map_Map__2 *std_mem_alloc__37(u32 count) {
  return ((std_map_Map__2 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__2)))));
}

std_map_Item__3 *std_mem_alloc__38(u32 count) {
  return ((std_map_Item__3 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__3)))));
}

std_map_Item__3 **std_mem_alloc__39(u32 count) {
  return ((std_map_Item__3 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__3 *)))));
}

std_map_Map__3 *std_mem_alloc__40(u32 count) {
  return ((std_map_Map__3 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__3)))));
}

std_map_Item__4 *std_mem_alloc__41(u32 count) {
  return ((std_map_Item__4 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__4)))));
}

std_map_Item__4 **std_mem_alloc__42(u32 count) {
  return ((std_map_Item__4 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__4 *)))));
}

std_map_Map__4 *std_mem_alloc__43(u32 count) {
  return ((std_map_Map__4 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__4)))));
}

std_map_Item__5 *std_mem_alloc__44(u32 count) {
  return ((std_map_Item__5 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__5)))));
}

std_map_Item__5 **std_mem_alloc__45(u32 count) {
  return ((std_map_Item__5 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__5 *)))));
}

std_map_Map__5 *std_mem_alloc__46(u32 count) {
  return ((std_map_Map__5 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__5)))));
}

std_map_Item__6 *std_mem_alloc__47(u32 count) {
  return ((std_map_Item__6 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__6)))));
}

std_map_Item__6 **std_mem_alloc__48(u32 count) {
  return ((std_map_Item__6 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__6 *)))));
}

std_map_Map__6 *std_mem_alloc__49(u32 count) {
  return ((std_map_Map__6 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__6)))));
}

std_map_Item__7 *std_mem_alloc__50(u32 count) {
  return ((std_map_Item__7 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__7)))));
}

std_map_Item__7 **std_mem_alloc__51(u32 count) {
  return ((std_map_Item__7 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__7 *)))));
}

std_map_Map__7 *std_mem_alloc__52(u32 count) {
  return ((std_map_Map__7 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__7)))));
}

std_map_Item__8 *std_mem_alloc__53(u32 count) {
  return ((std_map_Item__8 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__8)))));
}

std_map_Item__8 **std_mem_alloc__54(u32 count) {
  return ((std_map_Item__8 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__8 *)))));
}

std_map_Map__8 *std_mem_alloc__55(u32 count) {
  return ((std_map_Map__8 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__8)))));
}

std_map_Item__9 *std_mem_alloc__56(u32 count) {
  return ((std_map_Item__9 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__9)))));
}

std_map_Item__9 **std_mem_alloc__57(u32 count) {
  return ((std_map_Item__9 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Item__9 *)))));
}

std_map_Map__9 *std_mem_alloc__58(u32 count) {
  return ((std_map_Map__9 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_map_Map__9)))));
}

std_vector_Vector__0 *std_mem_alloc__59(u32 count) {
  return ((std_vector_Vector__0 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__0)))));
}

compiler_types_Type **std_mem_alloc__60(u32 count) {
  return ((compiler_types_Type **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_types_Type *)))));
}

std_vector_Vector__1 *std_mem_alloc__61(u32 count) {
  return ((std_vector_Vector__1 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__1)))));
}

char **std_mem_alloc__62(u32 count) {
  return ((char **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(char *)))));
}

std_vector_Vector__2 *std_mem_alloc__63(u32 count) {
  return ((std_vector_Vector__2 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__2)))));
}

compiler_ast_nodes_Structure **std_mem_alloc__64(u32 count) {
  return ((compiler_ast_nodes_Structure **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Structure *)))));
}

std_vector_Vector__3 *std_mem_alloc__65(u32 count) {
  return ((std_vector_Vector__3 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__3)))));
}

compiler_ast_scopes_TemplateInstance **std_mem_alloc__66(u32 count) {
  return ((compiler_ast_scopes_TemplateInstance **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_scopes_TemplateInstance *)))));
}

std_vector_Vector__4 *std_mem_alloc__67(u32 count) {
  return ((std_vector_Vector__4 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__4)))));
}

compiler_ast_nodes_Variable **std_mem_alloc__68(u32 count) {
  return ((compiler_ast_nodes_Variable **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Variable *)))));
}

std_vector_Vector__5 *std_mem_alloc__69(u32 count) {
  return ((std_vector_Vector__5 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__5)))));
}

compiler_ast_nodes_ImportPart **std_mem_alloc__70(u32 count) {
  return ((compiler_ast_nodes_ImportPart **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_ImportPart *)))));
}

std_vector_Vector__6 *std_mem_alloc__71(u32 count) {
  return ((std_vector_Vector__6 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__6)))));
}

compiler_ast_nodes_Function **std_mem_alloc__72(u32 count) {
  return ((compiler_ast_nodes_Function **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Function *)))));
}

std_vector_Vector__7 *std_mem_alloc__73(u32 count) {
  return ((std_vector_Vector__7 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__7)))));
}

compiler_ast_scopes_Scope **std_mem_alloc__74(u32 count) {
  return ((compiler_ast_scopes_Scope **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_scopes_Scope *)))));
}

std_vector_Vector__8 *std_mem_alloc__75(u32 count) {
  return ((std_vector_Vector__8 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__8)))));
}

compiler_ast_program_Namespace **std_mem_alloc__76(u32 count) {
  return ((compiler_ast_program_Namespace **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_program_Namespace *)))));
}

std_vector_Vector__9 *std_mem_alloc__77(u32 count) {
  return ((std_vector_Vector__9 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__9)))));
}

compiler_tokens_Token **std_mem_alloc__78(u32 count) {
  return ((compiler_tokens_Token **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_tokens_Token *)))));
}

std_vector_Vector__10 *std_mem_alloc__79(u32 count) {
  return ((std_vector_Vector__10 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__10)))));
}

compiler_attributes_Attribute **std_mem_alloc__80(u32 count) {
  return ((compiler_attributes_Attribute **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_attributes_Attribute *)))));
}

std_vector_Vector__11 *std_mem_alloc__81(u32 count) {
  return ((std_vector_Vector__11 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__11)))));
}

compiler_errors_Error **std_mem_alloc__82(u32 count) {
  return ((compiler_errors_Error **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_errors_Error *)))));
}

std_vector_Vector__12 *std_mem_alloc__83(u32 count) {
  return ((std_vector_Vector__12 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__12)))));
}

compiler_ast_nodes_Enum **std_mem_alloc__84(u32 count) {
  return ((compiler_ast_nodes_Enum **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Enum *)))));
}

std_vector_Vector__13 *std_mem_alloc__85(u32 count) {
  return ((std_vector_Vector__13 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__13)))));
}

compiler_ast_nodes_AST **std_mem_alloc__86(u32 count) {
  return ((compiler_ast_nodes_AST **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_AST *)))));
}

std_vector_Vector__14 *std_mem_alloc__87(u32 count) {
  return ((std_vector_Vector__14 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__14)))));
}

compiler_ast_operators_Operator *std_mem_alloc__88(u32 count) {
  return ((compiler_ast_operators_Operator *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_operators_Operator)))));
}

std_vector_Vector__15 *std_mem_alloc__89(u32 count) {
  return ((std_vector_Vector__15 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__15)))));
}

compiler_ast_nodes_Argument **std_mem_alloc__90(u32 count) {
  return ((compiler_ast_nodes_Argument **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_Argument *)))));
}

std_vector_Vector__16 *std_mem_alloc__91(u32 count) {
  return ((std_vector_Vector__16 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__16)))));
}

std_vector_Vector__5 **std_mem_alloc__92(u32 count) {
  return ((std_vector_Vector__5 **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__5 *)))));
}

std_vector_Vector__17 *std_mem_alloc__93(u32 count) {
  return ((std_vector_Vector__17 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__17)))));
}

compiler_ast_nodes_IfBranch *std_mem_alloc__94(u32 count) {
  return ((compiler_ast_nodes_IfBranch *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_IfBranch)))));
}

std_vector_Vector__18 *std_mem_alloc__95(u32 count) {
  return ((std_vector_Vector__18 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__18)))));
}

compiler_ast_nodes_MatchCase **std_mem_alloc__96(u32 count) {
  return ((compiler_ast_nodes_MatchCase **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_nodes_MatchCase *)))));
}

std_vector_Vector__19 *std_mem_alloc__97(u32 count) {
  return ((std_vector_Vector__19 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__19)))));
}

compiler_ast_scopes_Symbol **std_mem_alloc__98(u32 count) {
  return ((compiler_ast_scopes_Symbol **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_scopes_Symbol *)))));
}

std_vector_Vector__20 *std_mem_alloc__99(u32 count) {
  return ((std_vector_Vector__20 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__20)))));
}

compiler_ast_scopes_Reference *std_mem_alloc__100(u32 count) {
  return ((compiler_ast_scopes_Reference *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(compiler_ast_scopes_Reference)))));
}

std_vector_Vector__21 *std_mem_alloc__101(u32 count) {
  return ((std_vector_Vector__21 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__21)))));
}

std_span_Span *std_mem_alloc__102(u32 count) {
  return ((std_span_Span *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_span_Span)))));
}

std_vector_Vector__22 *std_mem_alloc__103(u32 count) {
  return ((std_vector_Vector__22 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__22)))));
}

std_value_Value **std_mem_alloc__104(u32 count) {
  return ((std_value_Value **)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_value_Value *)))));
}

std_vector_Vector__23 *std_mem_alloc__105(u32 count) {
  return ((std_vector_Vector__23 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__23)))));
}

std_compact_map_Item__0 *std_mem_alloc__106(u32 count) {
  return ((std_compact_map_Item__0 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_compact_map_Item__0)))));
}

std_vector_Vector__24 *std_mem_alloc__107(u32 count) {
  return ((std_vector_Vector__24 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(std_vector_Vector__24)))));
}

u32 *std_mem_alloc__108(u32 count) {
  return ((u32 *)std_mem_state_alloc_fn(std_mem_state_allocator, (count * ((u32)sizeof(u32)))));
}

void std_mem_free(void *ptr) {
  if (std_mem_state_free_fn != NULL) {
    std_mem_state_free_fn(std_mem_state_allocator, ptr);
    return;
  }
}

u8 *std_mem_realloc__0(u8 *ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(u8)));
  u32 new_size = (new_count * ((u32)sizeof(u8)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  u8 *new_ptr = std_mem_alloc__24(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_types_Type **std_mem_realloc__1(compiler_types_Type **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_types_Type *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_types_Type *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_types_Type **new_ptr = std_mem_alloc__60(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

char **std_mem_realloc__2(char **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(char *)));
  u32 new_size = (new_count * ((u32)sizeof(char *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  char **new_ptr = std_mem_alloc__62(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_Structure **std_mem_realloc__3(compiler_ast_nodes_Structure **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_Structure *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_Structure *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_Structure **new_ptr = std_mem_alloc__64(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_scopes_TemplateInstance **std_mem_realloc__4(compiler_ast_scopes_TemplateInstance **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_scopes_TemplateInstance *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_scopes_TemplateInstance *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_scopes_TemplateInstance **new_ptr = std_mem_alloc__66(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_Variable **std_mem_realloc__5(compiler_ast_nodes_Variable **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_Variable *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_Variable *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_Variable **new_ptr = std_mem_alloc__68(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_ImportPart **std_mem_realloc__6(compiler_ast_nodes_ImportPart **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_ImportPart *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_ImportPart *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_ImportPart **new_ptr = std_mem_alloc__70(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_Function **std_mem_realloc__7(compiler_ast_nodes_Function **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_Function *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_Function *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_Function **new_ptr = std_mem_alloc__72(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_scopes_Scope **std_mem_realloc__8(compiler_ast_scopes_Scope **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_scopes_Scope *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_scopes_Scope *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_scopes_Scope **new_ptr = std_mem_alloc__74(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_program_Namespace **std_mem_realloc__9(compiler_ast_program_Namespace **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_program_Namespace *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_program_Namespace *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_program_Namespace **new_ptr = std_mem_alloc__76(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_tokens_Token **std_mem_realloc__10(compiler_tokens_Token **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_tokens_Token *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_tokens_Token *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_tokens_Token **new_ptr = std_mem_alloc__78(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_attributes_Attribute **std_mem_realloc__11(compiler_attributes_Attribute **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_attributes_Attribute *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_attributes_Attribute *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_attributes_Attribute **new_ptr = std_mem_alloc__80(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_errors_Error **std_mem_realloc__12(compiler_errors_Error **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_errors_Error *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_errors_Error *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_errors_Error **new_ptr = std_mem_alloc__82(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_Enum **std_mem_realloc__13(compiler_ast_nodes_Enum **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_Enum *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_Enum *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_Enum **new_ptr = std_mem_alloc__84(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_AST **std_mem_realloc__14(compiler_ast_nodes_AST **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_AST *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_AST *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_AST **new_ptr = std_mem_alloc__86(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_operators_Operator *std_mem_realloc__15(compiler_ast_operators_Operator *ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_operators_Operator)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_operators_Operator)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_operators_Operator *new_ptr = std_mem_alloc__88(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_Argument **std_mem_realloc__16(compiler_ast_nodes_Argument **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_Argument *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_Argument *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_Argument **new_ptr = std_mem_alloc__90(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

std_vector_Vector__5 **std_mem_realloc__17(std_vector_Vector__5 **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(std_vector_Vector__5 *)));
  u32 new_size = (new_count * ((u32)sizeof(std_vector_Vector__5 *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  std_vector_Vector__5 **new_ptr = std_mem_alloc__92(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_IfBranch *std_mem_realloc__18(compiler_ast_nodes_IfBranch *ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_IfBranch)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_IfBranch)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_IfBranch *new_ptr = std_mem_alloc__94(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_nodes_MatchCase **std_mem_realloc__19(compiler_ast_nodes_MatchCase **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_nodes_MatchCase *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_nodes_MatchCase *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_nodes_MatchCase **new_ptr = std_mem_alloc__96(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_scopes_Symbol **std_mem_realloc__20(compiler_ast_scopes_Symbol **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_scopes_Symbol *)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_scopes_Symbol *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_scopes_Symbol **new_ptr = std_mem_alloc__98(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

compiler_ast_scopes_Reference *std_mem_realloc__21(compiler_ast_scopes_Reference *ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(compiler_ast_scopes_Reference)));
  u32 new_size = (new_count * ((u32)sizeof(compiler_ast_scopes_Reference)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  compiler_ast_scopes_Reference *new_ptr = std_mem_alloc__100(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

std_span_Span *std_mem_realloc__22(std_span_Span *ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(std_span_Span)));
  u32 new_size = (new_count * ((u32)sizeof(std_span_Span)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  std_span_Span *new_ptr = std_mem_alloc__102(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

std_value_Value **std_mem_realloc__23(std_value_Value **ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(std_value_Value *)));
  u32 new_size = (new_count * ((u32)sizeof(std_value_Value *)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  std_value_Value **new_ptr = std_mem_alloc__104(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

std_compact_map_Item__0 *std_mem_realloc__24(std_compact_map_Item__0 *ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(std_compact_map_Item__0)));
  u32 new_size = (new_count * ((u32)sizeof(std_compact_map_Item__0)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  std_compact_map_Item__0 *new_ptr = std_mem_alloc__106(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

u32 *std_mem_realloc__25(u32 *ptr, u32 old_count, u32 new_count) {
  u32 old_size = (old_count * ((u32)sizeof(u32)));
  u32 new_size = (new_count * ((u32)sizeof(u32)));
  if (std_mem_state_realloc_fn != NULL) {
    return std_mem_state_realloc_fn(std_mem_state_allocator, ptr, old_size, new_size);
  }
  ae_assert(new_size >= old_size, "std/mem.oc:60:12: Assertion failed: `new_size >= old_size`", "Cannot shrink memory in default allocator");
  u32 *new_ptr = std_mem_alloc__108(new_count);
  memcpy(new_ptr, ptr, old_size);
  std_mem_free(ptr);
  return new_ptr;
}

void *std_mem_impl_my_calloc(void *state, u32 size) {
  return calloc(size, 1);
}

void *std_mem_impl_my_realloc(void *state, void *ptr, u32 old_size, u32 size) {
  return realloc(ptr, size);
}

void std_mem_impl_my_free(void *state, void *ptr) {
  free(ptr);
}

bool std_compact_map_Map__0_contains(std_compact_map_Map__0 *this, char *key) {
  u32 hash = str_hash(key);
  u32 index = std_compact_map_Map__0_get_index(this, key, hash);
  return this->indices[index] >= 0;
}

std_compact_map_Iterator__0 std_compact_map_Map__0_iter(std_compact_map_Map__0 *this) {
  return (std_compact_map_Iterator__0){.iter=std_vector_Vector__23_iter(this->items)};
}

std_compact_map_Item__0 std_compact_map_Iterator__0_cur(std_compact_map_Iterator__0 *this) {
  return std_vector_Iterator__23_cur(&this->iter);
}

void std_compact_map_Iterator__0_next(std_compact_map_Iterator__0 *this) {
  std_vector_Iterator__23_next(&this->iter);
}

bool std_compact_map_Iterator__0_has_value(std_compact_map_Iterator__0 *this) {
  return std_vector_Iterator__23_has_value(&this->iter);
}

void std_compact_map_Map__0_insert(std_compact_map_Map__0 *this, char *key, std_value_Value *value) {
  u32 hash = str_hash(key);
  u32 index = std_compact_map_Map__0_get_index(this, key, hash);
  if (this->indices[index] < 0) {
    this->indices[index]=((i32)this->items->size);
    std_vector_Vector__23_push(this->items, (std_compact_map_Item__0){.hash=hash, .key=key, .value=value});
    if (((u32)this->items->size) >= this->capacity) {
      std_compact_map_Map__0_resize(this, (this->capacity * 2));
    }
  } else {
    u32 item_index = ((u32)this->indices[index]);
    this->items->data[item_index].value=value;
  }
}

u32 std_compact_map_Map__0_get_index(std_compact_map_Map__0 *this, char *key, u32 hash) {
  u32 perturb = hash;
  u32 j = (hash % this->capacity);
  u32 i = j;
  i32 first_deleted = -1;
  while (this->indices[i] != std_compact_map_INDEX_FREE) {
    if (this->indices[i]==std_compact_map_INDEX_DELETED) {
      if (first_deleted < 0) {
        first_deleted=((i32)i);
      }
    } else {
      std_compact_map_Item__0 item = this->items->data[this->indices[i]];
      if (item.hash==hash && str_eq(item.key, key)) {
        return i;
      }
    }
    j=(((5 * j) + perturb) + 1);
    i=(j % this->capacity);
    perturb=(perturb >> 5);
  }
  if (first_deleted < 0) {
    return i;
  }
  return ((u32)first_deleted);
}

std_value_Value *std_compact_map_Map__0_get(std_compact_map_Map__0 *this, char *key, std_value_Value *defolt) {
  std_compact_map_Item__0 *item = std_compact_map_Map__0_get_item(this, key);
  if (!((bool)item)) {
  return defolt;
  }
  return item->value;
}

bool std_compact_map_Map__0_is_empty(std_compact_map_Map__0 *this) {
  return this->items->size==0;
}

void std_compact_map_Map__0_resize(std_compact_map_Map__0 *this, u32 new_capacity) {
  std_mem_free(this->indices);
  this->indices=std_mem_alloc__22(new_capacity);
  this->capacity=new_capacity;
  for (u32 i = 0; i < new_capacity; i++) {
    this->indices[i]=std_compact_map_INDEX_FREE;
  }
  for (u32 i = 0; i < this->items->size; i++) {
    std_compact_map_Item__0 item = std_vector_Vector__23_at(this->items, i);
    u32 index = std_compact_map_Map__0_get_index(this, item.key, item.hash);
    if (this->indices[index]==std_compact_map_INDEX_FREE) {
      this->indices[index]=((i32)i);
    }
  }
}

std_compact_map_Item__0 *std_compact_map_Map__0_get_item(std_compact_map_Map__0 *this, char *key) {
  u32 hash = str_hash(key);
  u32 index = std_compact_map_Map__0_get_index(this, key, hash);
  if (this->indices[index] < 0) {
    return NULL;
  }
  u32 idx = ((u32)this->indices[index]);
  return &this->items->data[idx];
}

std_compact_map_Map__0 *std_compact_map_Map__0_new(u32 capacity) {
  std_vector_Vector__23 *items = std_vector_Vector__23_new(capacity);
  i32 *indices = std_mem_alloc__22(capacity);
  for (u32 i = 0; i < capacity; i++) {
    indices[i]=std_compact_map_INDEX_FREE;
  }
  std_compact_map_Map__0 *map = std_mem_alloc__23(1);
  map->items=items;
  map->indices=indices;
  map->capacity=capacity;
  return map;
}

std_buffer_Buffer std_buffer_Buffer_make(u32 capacity) {
  return (std_buffer_Buffer){.data=std_mem_alloc__24(capacity), .size=0, .capacity=capacity};
}

std_buffer_Buffer std_buffer_Buffer_from_str(char *s) {
  return (std_buffer_Buffer){.data=((u8 *)s), .size=((u32)strlen(s)), .capacity=((u32)strlen(s))};
}

void std_buffer_Buffer_resize_if_necessary(std_buffer_Buffer *this, u32 new_size) {
  u32 new_size_nt = (new_size + 1);
  if (new_size_nt >= this->capacity) {
    u32 new_capacity = u32_max(this->capacity, new_size_nt);
    this->data=std_mem_realloc__0(this->data, this->capacity, new_capacity);
    memset((this->data + this->capacity), ((u8)0), (new_capacity - this->capacity));
    this->capacity=((u32)new_capacity);
    ae_assert(((bool)this->data), "std/buffer.oc:66:16: Assertion failed: `.data?`", "Out of memory!");
  }
}

void std_buffer_Buffer_write_str(std_buffer_Buffer *this, char *s) {
  u32 len = ((u32)strlen(s));
  std_buffer_Buffer_resize_if_necessary(this, (this->size + len));
  memcpy((this->data + this->size), s, len);
  this->size+=len;
}

void std_buffer_Buffer_write_str_f(std_buffer_Buffer *this, char *s) {
  std_buffer_Buffer_write_str(this, s);
  std_mem_free(s);
}

void std_buffer_Buffer_write_char(std_buffer_Buffer *this, char c) {
  std_buffer_Buffer_write_u8(this, ((u8)c));
}

void std_buffer_Buffer_write_u8(std_buffer_Buffer *this, u8 value) {
  std_buffer_Buffer_resize_if_necessary(this, (this->size + 1));
  this->data[this->size]=value;
  this->size+=1;
}

char *std_buffer_Buffer_new_str(std_buffer_Buffer this) {
  return strdup(((char *)this.data));
}

char *std_buffer_Buffer_str(std_buffer_Buffer this) {
  return ((char *)this.data);
}

std_sv_SV std_buffer_Buffer_sv(std_buffer_Buffer this) {
  return (std_sv_SV){.data=((char *)this.data), .len=this.size};
}

void std_buffer_Buffer_clear(std_buffer_Buffer *this) {
  this->size=0;
  memset(this->data, ((u8)0), this->capacity);
}

void std_buffer_Buffer_free(std_buffer_Buffer *this) {
  std_mem_free(this->data);
}

void std_set_Set__0_add(std_set_Set__0 *this, char *key) {
  std_map_Map__0_insert(this->map, key, true);
  this->size=this->map->size;
}

bool std_set_Set__0_contains(std_set_Set__0 *this, char *key) {
  return std_map_Map__0_contains(this->map, key);
}

void std_set_Set__0_free(std_set_Set__0 *this) {
  std_map_Map__0_free(this->map);
  std_mem_free(this);
}

std_set_Set__0 *std_set_Set__0_new(void) {
  std_set_Set__0 *set = std_mem_alloc__25(1);
  set->map=std_map_Map__0_new(32);
  return set;
}

void std_set_Set__1_add(std_set_Set__1 *this, u64 key) {
  std_map_Map__1_insert(this->map, key, true);
  this->size=this->map->size;
}

bool std_set_Set__1_contains(std_set_Set__1 *this, u64 key) {
  return std_map_Map__1_contains(this->map, key);
}

void std_set_Set__1_free(std_set_Set__1 *this) {
  std_map_Map__1_free(this->map);
  std_mem_free(this);
}

std_set_Set__1 *std_set_Set__1_new(void) {
  std_set_Set__1 *set = std_mem_alloc__26(1);
  set->map=std_map_Map__1_new(32);
  return set;
}

void std_set_Set__2_add(std_set_Set__2 *this, std_span_Span key) {
  std_map_Map__9_insert(this->map, key, true);
  this->size=this->map->size;
}

std_set_Iterator__2 std_set_Set__2_iter(std_set_Set__2 *this) {
  return std_set_Iterator__2_make(this);
}

std_span_Span std_set_Iterator__2_cur(std_set_Iterator__2 *this) {
  return std_map_Iterator__9_cur(&this->map_iter)->key;
}

void std_set_Iterator__2_next(std_set_Iterator__2 *this) {
  std_map_Iterator__9_next(&this->map_iter);
}

std_set_Iterator__2 std_set_Iterator__2_make(std_set_Set__2 *set) {
  return (std_set_Iterator__2){.map_iter=std_map_Map__9_iter(set->map)};
}

bool std_set_Iterator__2_has_value(std_set_Iterator__2 *this) {
  return std_map_Iterator__9_has_value(&this->map_iter);
}

void std_set_Set__2_free(std_set_Set__2 *this) {
  std_map_Map__9_free(this->map);
  std_mem_free(this);
}

std_set_Set__2 *std_set_Set__2_new(void) {
  std_set_Set__2 *set = std_mem_alloc__27(1);
  set->map=std_map_Map__9_new(32);
  return set;
}

std_span_Location std_span_Location_default(void) {
  std_span_Location loc = {0};
  loc.filename="<default>";
  loc.line=0;
  loc.col=0;
  loc.index=0;
  return loc;
}

u32 std_span_Location_hash(std_span_Location this) {
  return std_traits_hash_pair_hash(str_hash(this.filename), u32_hash(this.index));
}

bool std_span_Location_eq(std_span_Location this, std_span_Location other) {
  return str_eq(this.filename, other.filename) && this.index==other.index;
}

char *std_span_Location_str(std_span_Location *this) {
  return format_string("%s:%u:%u", this->filename, this->line, this->col);
}

bool std_span_Location_is_valid(std_span_Location *this) {
  return ((this->line > 0) && (this->col > 0)) && (this->index >= 0);
}

bool std_span_Location_is_before(std_span_Location *this, std_span_Location other) {
  if (this->line > other.line) {
  return false;
  }
  if (this->line < other.line) {
  return true;
  }
  return this->col <= other.col;
}

std_span_Span std_span_Span_default(void) {
  std_span_Span span = {0};
  span.start=(std_span_Location){.filename="<default>", .line=0, .col=0, .index=0};
  span.end=(std_span_Location){.filename="<default>", .line=0, .col=0, .index=0};
  return span;
}

u32 std_span_Span_hash(std_span_Span this) {
  return std_traits_hash_pair_hash(std_span_Location_hash(this.start), u32_hash(this.end.index));
}

bool std_span_Span_eq(std_span_Span this, std_span_Span other) {
  return std_span_Location_eq(this.start, other.start) && std_span_Location_eq(this.end, other.end);
}

bool std_span_Span_is_valid(std_span_Span this) {
  return std_span_Location_is_valid(&this.start) && std_span_Location_is_valid(&this.end);
}

std_span_Span std_span_Span_join(std_span_Span this, std_span_Span other) {
  std_span_Span span = {0};
  span.start=this.start;
  span.end=other.end;
  return span;
}

bool std_span_Span_contains_loc(std_span_Span this, std_span_Location loc) {
  if (!std_span_Span_is_valid(this)) {
  return false;
  }
  if (!str_eq(this.start.filename, loc.filename)) {
  return false;
  }
  return std_span_Location_is_before(&this.start, loc) && std_span_Location_is_before(&loc, this.end);
}

bool std_span_Span_starts_right_after(std_span_Span this, std_span_Span other) {
  if (!std_span_Span_is_valid(this)) {
  return false;
  }
  if (!str_eq(this.start.filename, other.start.filename)) {
  return false;
  }
  return this.start.index==other.end.index;
}

bool u64_eq(u64 this, u64 other) {
  return this==other;
}

u32 std_traits_hash_hash_bytes(u8 *data, u32 len) {
  u32 hash = 5381;
  for (u32 i = 0; i < len; i+=1) {
    hash=((hash * 33) ^ ((u32)data[i]));
  }
  return hash;
}

u32 std_traits_hash_pair_hash(u32 a, u32 b) {
  return (a * 33) ^ b;
}

u32 str_hash(char *this) {
  return std_traits_hash_hash_bytes(((u8 *)this), strlen(this));
}

u32 u32_hash(u32 this) {
  return ((u32)this) * 7817;
}

u32 u64_hash(u64 this) {
  return std_traits_hash_pair_hash(u32_hash(((u32)this)), u32_hash(((u32)(this >> ((u64)32)))));
}

std_value_Value *std_value_Value_new(std_value_ValueType type) {
  std_value_Value *val = std_mem_alloc__28(1);
  val->type=type;
  switch (type) {
    case std_value_ValueType_Dictionary: {
      val->u.as_dict=std_compact_map_Map__0_new(16);
    } break;
    case std_value_ValueType_List: {
      val->u.as_list=std_vector_Vector__22_new(16);
    } break;
    default: {
    } break;
  }
  val->span=std_span_Span_default();
  return val;
}

std_value_Value *std_value_Value_new_str(char *s) {
  std_value_Value *val = std_value_Value_new(std_value_ValueType_String);
  val->u.as_str=std_buffer_Buffer_from_str(s);
  return val;
}

std_value_Value *std_value_Value_new_bool(bool bul) {
  std_value_Value *val = std_value_Value_new(std_value_ValueType_Bool);
  val->u.as_bool=bul;
  return val;
}

std_value_Value *std_value_Value_new_int(i64 num) {
  std_value_Value *val = std_value_Value_new(std_value_ValueType_Integer);
  val->u.as_int=num;
  return val;
}

void std_value_Value_ensure(std_value_Value *this, std_value_ValueType type) {
  if (this->type != type) {
    printf("%s:%u:%u: Value type mismatch, expected %s but got %s\n", (this->span.start).filename, (this->span.start).line, (this->span.start).col, std_value_ValueType_dbg(this->type), std_value_ValueType_dbg(type));
    exit(1);
  }
}

std_value_Value *std_value_Value_at_key(std_value_Value *this, char *key) {
  std_value_Value_ensure(this, std_value_ValueType_Dictionary);
  if (!std_compact_map_Map__0_contains(this->u.as_dict, key)) {
    printf("%s:%u:%u: Key not found: %s\n", (this->span.start).filename, (this->span.start).line, (this->span.start).col, key);
    exit(1);
  }
  return std_compact_map_Map__0_get(this->u.as_dict, key, NULL);
}

void std_value_Value_push(std_value_Value *this, std_value_Value *value) {
  std_value_Value_ensure(this, std_value_ValueType_List);
  std_vector_Vector__22_push(this->u.as_list, value);
}

std_value_Value *std_value_Value_get(std_value_Value *this, char *key, std_value_Value *defolt) {
  std_value_Value_ensure(this, std_value_ValueType_Dictionary);
  return std_compact_map_Map__0_get(this->u.as_dict, key, defolt);
}

void std_value_Value_insert(std_value_Value *this, char *key, std_value_Value *value) {
  std_value_Value_ensure(this, std_value_ValueType_Dictionary);
  std_compact_map_Map__0_insert(this->u.as_dict, key, value);
}

void std_value_Value_insert_str(std_value_Value *this, char *key, char *s) {
  std_value_Value_insert(this, key, std_value_Value_new_str(s));
}

void std_value_Value_insert_u32(std_value_Value *this, char *key, u32 i) {
  std_value_Value_insert(this, key, std_value_Value_new_int(((i64)i)));
}

std_compact_map_Map__0 *std_value_Value_as_dict(std_value_Value *this) {
  std_value_Value_ensure(this, std_value_ValueType_Dictionary);
  return this->u.as_dict;
}

char *std_value_Value_dbg(std_value_Value *this) {
  std_buffer_Buffer buf = std_json_serialize(this);
  return std_buffer_Buffer_str(buf);
}

char *std_fs_realpath(char *path) {
  return realpath(path, NULL);
}

bool std_fs_file_exists(char *path) {
  FILE *file = fopen(path, "r");
  if (!((bool)file)) {
  return false;
  }
  fclose(file);
  return true;
}

void std_fs_write_file_bytes(char *path, void *data, u32 size) {
  FILE *file = fopen(path, "wb");
  if (!((bool)file)) {
  std_panic(format_string("[-] Failed to open file: %s", path));
  }
  i32 written = fwrite(data, 1, size, file);
  if (((u32)written) != size) {
    std_panic(format_string("[-] Failed to write to file: %s", path));
  }
  fclose(file);
}

void std_fs_write_file_str(char *path, char *data) {
  std_fs_write_file_bytes(path, data, strlen(data));
}

void std_fs_write_file(char *path, std_buffer_Buffer data) {
  std_fs_write_file_bytes(path, data.data, data.size);
}

std_buffer_Buffer std_fs_read_file(char *path) {
  FILE *file = fopen(path, "r");
  if (!((bool)file)) {
  std_panic(format_string("[-] Failed to open file: %s", path));
  }
  fseek(file, ((i64)0), SEEK_END);
  u32 size = ((u32)ftell(file));
  fseek(file, ((i64)0), SEEK_SET);
  std_buffer_Buffer data = std_buffer_Buffer_make((size + 1));
  i32 read = fread(data.data, 1, size, file);
  if (((u32)read) != size) {
    std_panic(format_string("[-] Failed to read from file: %s", path));
  }
  fclose(file);
  data.size=((u32)size);
  data.data[size]=((u8)0);
  return data;
}

void std_map_Item__0_free_list(std_map_Item__0 *this) {
  std_map_Item__0 *cur = this;
  while (((bool)cur)) {
    std_map_Item__0 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__0 *std_map_Item__0_new(char *key, bool value, std_map_Item__0 *next) {
  std_map_Item__0 *node = std_mem_alloc__29(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

bool std_map_Map__0_contains(std_map_Map__0 *this, char *key) {
  return ((bool)std_map_Map__0_get_item(this, key));
}

void std_map_Map__0_free(std_map_Map__0 *this) {
  for (u32 i = 0; i < this->num_buckets; i+=1) {
    std_map_Item__0_free_list(this->buckets[i]);
  }
  std_mem_free(this->buckets);
  std_mem_free(this);
}

void std_map_Map__0_insert(std_map_Map__0 *this, char *key, bool value) {
  std_map_Item__0 *node = std_map_Map__0_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__0_hash(this, key);
    std_map_Item__0 *new_node = std_map_Item__0_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__0_resize(this);
    }
  }
}

void std_map_Map__0_resize(std_map_Map__0 *this) {
  std_map_Item__0 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__30(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__0 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__0_hash(this, node->key);
      std_map_Item__0 *new_node = std_map_Item__0_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__0_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__0_hash(std_map_Map__0 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__0 *std_map_Map__0_get_item(std_map_Map__0 *this, char *key) {
  u32 hash = std_map_Map__0_hash(this, key);
  std_map_Item__0 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__0 *std_map_Map__0_new(u32 capacity) {
  std_map_Map__0 *map = std_mem_alloc__31(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__30(map->num_buckets);
  return map;
}

void std_map_Item__1_free_list(std_map_Item__1 *this) {
  std_map_Item__1 *cur = this;
  while (((bool)cur)) {
    std_map_Item__1 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__1 *std_map_Item__1_new(u64 key, bool value, std_map_Item__1 *next) {
  std_map_Item__1 *node = std_mem_alloc__32(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

bool std_map_Map__1_contains(std_map_Map__1 *this, u64 key) {
  return ((bool)std_map_Map__1_get_item(this, key));
}

void std_map_Map__1_free(std_map_Map__1 *this) {
  for (u32 i = 0; i < this->num_buckets; i+=1) {
    std_map_Item__1_free_list(this->buckets[i]);
  }
  std_mem_free(this->buckets);
  std_mem_free(this);
}

void std_map_Map__1_insert(std_map_Map__1 *this, u64 key, bool value) {
  std_map_Item__1 *node = std_map_Map__1_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__1_hash(this, key);
    std_map_Item__1 *new_node = std_map_Item__1_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__1_resize(this);
    }
  }
}

void std_map_Map__1_resize(std_map_Map__1 *this) {
  std_map_Item__1 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__33(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__1 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__1_hash(this, node->key);
      std_map_Item__1 *new_node = std_map_Item__1_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__1_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__1_hash(std_map_Map__1 *this, u64 key) {
  u32 hash = u64_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__1 *std_map_Map__1_get_item(std_map_Map__1 *this, u64 key) {
  u32 hash = std_map_Map__1_hash(this, key);
  std_map_Item__1 *node = this->buckets[hash];
  while (((bool)node)) {
    if (u64_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__1 *std_map_Map__1_new(u32 capacity) {
  std_map_Map__1 *map = std_mem_alloc__34(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__33(map->num_buckets);
  return map;
}

void std_map_Item__2_free_list(std_map_Item__2 *this) {
  std_map_Item__2 *cur = this;
  while (((bool)cur)) {
    std_map_Item__2 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__2 *std_map_Item__2_new(char *key, compiler_types_Type *value, std_map_Item__2 *next) {
  std_map_Item__2 *node = std_mem_alloc__35(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

std_map_Item__2 *std_map_Iterator__2_cur(std_map_Iterator__2 *this) {
  return this->node;
}

void std_map_Iterator__2_next(std_map_Iterator__2 *this) {
  while (this->idx < ((i32)this->map->num_buckets)) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) {
      return;
      }
    }
    this->idx+=1;
    this->node=({ std_map_Item__2 *__yield_0;
      if (this->idx < ((i32)this->map->num_buckets)) {
        __yield_0 = this->map->buckets[this->idx];
      } else {
        __yield_0 = NULL;
      }
;__yield_0; });
    if (((bool)this->node)) {
    return;
    }
  }
}

std_map_Iterator__2 std_map_Iterator__2_make(std_map_Map__2 *map) {
  std_map_Iterator__2 it = (std_map_Iterator__2){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__2_next(&it);
  return it;
}

bool std_map_Iterator__2_has_value(std_map_Iterator__2 *this) {
  return ((bool)this->node);
}

std_map_Iterator__2 std_map_Map__2_iter(std_map_Map__2 *this) {
  return std_map_Iterator__2_make(this);
}

void std_map_Map__2_insert(std_map_Map__2 *this, char *key, compiler_types_Type *value) {
  std_map_Item__2 *node = std_map_Map__2_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__2_hash(this, key);
    std_map_Item__2 *new_node = std_map_Item__2_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__2_resize(this);
    }
  }
}

compiler_types_Type *std_map_Map__2_get(std_map_Map__2 *this, char *key, compiler_types_Type *defolt) {
  std_map_Item__2 *node = std_map_Map__2_get_item(this, key);
  if (!((bool)node)) {
  return defolt;
  }
  return node->value;
}

bool std_map_Map__2_is_empty(std_map_Map__2 *this) {
  return this->size==0;
}

void std_map_Map__2_resize(std_map_Map__2 *this) {
  std_map_Item__2 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__36(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__2 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__2_hash(this, node->key);
      std_map_Item__2 *new_node = std_map_Item__2_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__2_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__2_hash(std_map_Map__2 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__2 *std_map_Map__2_get_item(std_map_Map__2 *this, char *key) {
  u32 hash = std_map_Map__2_hash(this, key);
  std_map_Item__2 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__2 *std_map_Map__2_new(u32 capacity) {
  std_map_Map__2 *map = std_mem_alloc__37(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__36(map->num_buckets);
  return map;
}

void std_map_Item__3_free_list(std_map_Item__3 *this) {
  std_map_Item__3 *cur = this;
  while (((bool)cur)) {
    std_map_Item__3 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__3 *std_map_Item__3_new(char *key, compiler_ast_program_Namespace *value, std_map_Item__3 *next) {
  std_map_Item__3 *node = std_mem_alloc__38(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

bool std_map_Map__3_contains(std_map_Map__3 *this, char *key) {
  return ((bool)std_map_Map__3_get_item(this, key));
}

std_map_ValueIterator__3 std_map_Map__3_iter_values(std_map_Map__3 *this) {
  return (std_map_ValueIterator__3){.map_iter=std_map_Map__3_iter(this)};
}

std_map_Item__3 *std_map_Iterator__3_cur(std_map_Iterator__3 *this) {
  return this->node;
}

void std_map_Iterator__3_next(std_map_Iterator__3 *this) {
  while (this->idx < ((i32)this->map->num_buckets)) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) {
      return;
      }
    }
    this->idx+=1;
    this->node=({ std_map_Item__3 *__yield_0;
      if (this->idx < ((i32)this->map->num_buckets)) {
        __yield_0 = this->map->buckets[this->idx];
      } else {
        __yield_0 = NULL;
      }
;__yield_0; });
    if (((bool)this->node)) {
    return;
    }
  }
}

std_map_Iterator__3 std_map_Iterator__3_make(std_map_Map__3 *map) {
  std_map_Iterator__3 it = (std_map_Iterator__3){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__3_next(&it);
  return it;
}

bool std_map_Iterator__3_has_value(std_map_Iterator__3 *this) {
  return ((bool)this->node);
}

compiler_ast_program_Namespace *std_map_ValueIterator__3_cur(std_map_ValueIterator__3 *this) {
  return std_map_Iterator__3_cur(&this->map_iter)->value;
}

void std_map_ValueIterator__3_next(std_map_ValueIterator__3 *this) {
  std_map_Iterator__3_next(&this->map_iter);
}

bool std_map_ValueIterator__3_has_value(std_map_ValueIterator__3 *this) {
  return std_map_Iterator__3_has_value(&this->map_iter);
}

std_map_Iterator__3 std_map_Map__3_iter(std_map_Map__3 *this) {
  return std_map_Iterator__3_make(this);
}

compiler_ast_program_Namespace *std_map_Map__3_at(std_map_Map__3 *this, char *key) {
  std_map_Item__3 *node = std_map_Map__3_get_item(this, key);
  ae_assert(((bool)node), "std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__3_insert(std_map_Map__3 *this, char *key, compiler_ast_program_Namespace *value) {
  std_map_Item__3 *node = std_map_Map__3_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__3_hash(this, key);
    std_map_Item__3 *new_node = std_map_Item__3_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__3_resize(this);
    }
  }
}

compiler_ast_program_Namespace *std_map_Map__3_get(std_map_Map__3 *this, char *key, compiler_ast_program_Namespace *defolt) {
  std_map_Item__3 *node = std_map_Map__3_get_item(this, key);
  if (!((bool)node)) {
  return defolt;
  }
  return node->value;
}

bool std_map_Map__3_is_empty(std_map_Map__3 *this) {
  return this->size==0;
}

void std_map_Map__3_resize(std_map_Map__3 *this) {
  std_map_Item__3 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__39(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__3 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__3_hash(this, node->key);
      std_map_Item__3 *new_node = std_map_Item__3_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__3_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__3_hash(std_map_Map__3 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__3 *std_map_Map__3_get_item(std_map_Map__3 *this, char *key) {
  u32 hash = std_map_Map__3_hash(this, key);
  std_map_Item__3 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__3 *std_map_Map__3_new(u32 capacity) {
  std_map_Map__3 *map = std_mem_alloc__40(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__39(map->num_buckets);
  return map;
}

void std_map_Item__4_free_list(std_map_Item__4 *this) {
  std_map_Item__4 *cur = this;
  while (((bool)cur)) {
    std_map_Item__4 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__4 *std_map_Item__4_new(char *key, compiler_ast_scopes_Symbol *value, std_map_Item__4 *next) {
  std_map_Item__4 *node = std_mem_alloc__41(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

bool std_map_Map__4_contains(std_map_Map__4 *this, char *key) {
  return ((bool)std_map_Map__4_get_item(this, key));
}

std_map_Item__4 *std_map_Iterator__4_cur(std_map_Iterator__4 *this) {
  return this->node;
}

void std_map_Iterator__4_next(std_map_Iterator__4 *this) {
  while (this->idx < ((i32)this->map->num_buckets)) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) {
      return;
      }
    }
    this->idx+=1;
    this->node=({ std_map_Item__4 *__yield_0;
      if (this->idx < ((i32)this->map->num_buckets)) {
        __yield_0 = this->map->buckets[this->idx];
      } else {
        __yield_0 = NULL;
      }
;__yield_0; });
    if (((bool)this->node)) {
    return;
    }
  }
}

std_map_Iterator__4 std_map_Iterator__4_make(std_map_Map__4 *map) {
  std_map_Iterator__4 it = (std_map_Iterator__4){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__4_next(&it);
  return it;
}

bool std_map_Iterator__4_has_value(std_map_Iterator__4 *this) {
  return ((bool)this->node);
}

std_map_Iterator__4 std_map_Map__4_iter(std_map_Map__4 *this) {
  return std_map_Iterator__4_make(this);
}

compiler_ast_scopes_Symbol *std_map_Map__4_at(std_map_Map__4 *this, char *key) {
  std_map_Item__4 *node = std_map_Map__4_get_item(this, key);
  ae_assert(((bool)node), "std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__4_insert(std_map_Map__4 *this, char *key, compiler_ast_scopes_Symbol *value) {
  std_map_Item__4 *node = std_map_Map__4_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__4_hash(this, key);
    std_map_Item__4 *new_node = std_map_Item__4_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__4_resize(this);
    }
  }
}

compiler_ast_scopes_Symbol *std_map_Map__4_get(std_map_Map__4 *this, char *key, compiler_ast_scopes_Symbol *defolt) {
  std_map_Item__4 *node = std_map_Map__4_get_item(this, key);
  if (!((bool)node)) {
  return defolt;
  }
  return node->value;
}

void std_map_Map__4_resize(std_map_Map__4 *this) {
  std_map_Item__4 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__42(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__4 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__4_hash(this, node->key);
      std_map_Item__4 *new_node = std_map_Item__4_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__4_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__4_hash(std_map_Map__4 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__4 *std_map_Map__4_get_item(std_map_Map__4 *this, char *key) {
  u32 hash = std_map_Map__4_hash(this, key);
  std_map_Item__4 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__4 *std_map_Map__4_new(u32 capacity) {
  std_map_Map__4 *map = std_mem_alloc__43(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__42(map->num_buckets);
  return map;
}

void std_map_Item__5_free_list(std_map_Item__5 *this) {
  std_map_Item__5 *cur = this;
  while (((bool)cur)) {
    std_map_Item__5 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__5 *std_map_Item__5_new(compiler_ast_operators_OperatorOverload key, compiler_ast_nodes_Function *value, std_map_Item__5 *next) {
  std_map_Item__5 *node = std_mem_alloc__44(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

void std_map_Map__5_insert(std_map_Map__5 *this, compiler_ast_operators_OperatorOverload key, compiler_ast_nodes_Function *value) {
  std_map_Item__5 *node = std_map_Map__5_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__5_hash(this, key);
    std_map_Item__5 *new_node = std_map_Item__5_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__5_resize(this);
    }
  }
}

compiler_ast_nodes_Function *std_map_Map__5_get(std_map_Map__5 *this, compiler_ast_operators_OperatorOverload key, compiler_ast_nodes_Function *defolt) {
  std_map_Item__5 *node = std_map_Map__5_get_item(this, key);
  if (!((bool)node)) {
  return defolt;
  }
  return node->value;
}

void std_map_Map__5_resize(std_map_Map__5 *this) {
  std_map_Item__5 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__45(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__5 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__5_hash(this, node->key);
      std_map_Item__5 *new_node = std_map_Item__5_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__5_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__5_hash(std_map_Map__5 *this, compiler_ast_operators_OperatorOverload key) {
  u32 hash = compiler_ast_operators_OperatorOverload_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__5 *std_map_Map__5_get_item(std_map_Map__5 *this, compiler_ast_operators_OperatorOverload key) {
  u32 hash = std_map_Map__5_hash(this, key);
  std_map_Item__5 *node = this->buckets[hash];
  while (((bool)node)) {
    if (compiler_ast_operators_OperatorOverload_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__5 *std_map_Map__5_new(u32 capacity) {
  std_map_Map__5 *map = std_mem_alloc__46(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__45(map->num_buckets);
  return map;
}

void std_map_Item__6_free_list(std_map_Item__6 *this) {
  std_map_Item__6 *cur = this;
  while (((bool)cur)) {
    std_map_Item__6 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__6 *std_map_Item__6_new(char *key, char *value, std_map_Item__6 *next) {
  std_map_Item__6 *node = std_mem_alloc__47(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

bool std_map_Map__6_contains(std_map_Map__6 *this, char *key) {
  return ((bool)std_map_Map__6_get_item(this, key));
}

std_map_Item__6 *std_map_Iterator__6_cur(std_map_Iterator__6 *this) {
  return this->node;
}

void std_map_Iterator__6_next(std_map_Iterator__6 *this) {
  while (this->idx < ((i32)this->map->num_buckets)) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) {
      return;
      }
    }
    this->idx+=1;
    this->node=({ std_map_Item__6 *__yield_0;
      if (this->idx < ((i32)this->map->num_buckets)) {
        __yield_0 = this->map->buckets[this->idx];
      } else {
        __yield_0 = NULL;
      }
;__yield_0; });
    if (((bool)this->node)) {
    return;
    }
  }
}

std_map_Iterator__6 std_map_Iterator__6_make(std_map_Map__6 *map) {
  std_map_Iterator__6 it = (std_map_Iterator__6){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__6_next(&it);
  return it;
}

bool std_map_Iterator__6_has_value(std_map_Iterator__6 *this) {
  return ((bool)this->node);
}

std_map_Iterator__6 std_map_Map__6_iter(std_map_Map__6 *this) {
  return std_map_Iterator__6_make(this);
}

void std_map_Map__6_insert(std_map_Map__6 *this, char *key, char *value) {
  std_map_Item__6 *node = std_map_Map__6_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__6_hash(this, key);
    std_map_Item__6 *new_node = std_map_Item__6_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__6_resize(this);
    }
  }
}

char *std_map_Map__6_get(std_map_Map__6 *this, char *key, char *defolt) {
  std_map_Item__6 *node = std_map_Map__6_get_item(this, key);
  if (!((bool)node)) {
  return defolt;
  }
  return node->value;
}

void std_map_Map__6_resize(std_map_Map__6 *this) {
  std_map_Item__6 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__48(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__6 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__6_hash(this, node->key);
      std_map_Item__6 *new_node = std_map_Item__6_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__6_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__6_hash(std_map_Map__6 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__6 *std_map_Map__6_get_item(std_map_Map__6 *this, char *key) {
  u32 hash = std_map_Map__6_hash(this, key);
  std_map_Item__6 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__6 *std_map_Map__6_new(u32 capacity) {
  std_map_Map__6 *map = std_mem_alloc__49(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__48(map->num_buckets);
  return map;
}

void std_map_Item__7_free_list(std_map_Item__7 *this) {
  std_map_Item__7 *cur = this;
  while (((bool)cur)) {
    std_map_Item__7 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__7 *std_map_Item__7_new(char *key, compiler_ast_nodes_Function *value, std_map_Item__7 *next) {
  std_map_Item__7 *node = std_mem_alloc__50(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

std_map_ValueIterator__7 std_map_Map__7_iter_values(std_map_Map__7 *this) {
  return (std_map_ValueIterator__7){.map_iter=std_map_Map__7_iter(this)};
}

std_map_Item__7 *std_map_Iterator__7_cur(std_map_Iterator__7 *this) {
  return this->node;
}

void std_map_Iterator__7_next(std_map_Iterator__7 *this) {
  while (this->idx < ((i32)this->map->num_buckets)) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) {
      return;
      }
    }
    this->idx+=1;
    this->node=({ std_map_Item__7 *__yield_0;
      if (this->idx < ((i32)this->map->num_buckets)) {
        __yield_0 = this->map->buckets[this->idx];
      } else {
        __yield_0 = NULL;
      }
;__yield_0; });
    if (((bool)this->node)) {
    return;
    }
  }
}

std_map_Iterator__7 std_map_Iterator__7_make(std_map_Map__7 *map) {
  std_map_Iterator__7 it = (std_map_Iterator__7){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__7_next(&it);
  return it;
}

bool std_map_Iterator__7_has_value(std_map_Iterator__7 *this) {
  return ((bool)this->node);
}

compiler_ast_nodes_Function *std_map_ValueIterator__7_cur(std_map_ValueIterator__7 *this) {
  return std_map_Iterator__7_cur(&this->map_iter)->value;
}

void std_map_ValueIterator__7_next(std_map_ValueIterator__7 *this) {
  std_map_Iterator__7_next(&this->map_iter);
}

bool std_map_ValueIterator__7_has_value(std_map_ValueIterator__7 *this) {
  return std_map_Iterator__7_has_value(&this->map_iter);
}

std_map_Iterator__7 std_map_Map__7_iter(std_map_Map__7 *this) {
  return std_map_Iterator__7_make(this);
}

compiler_ast_nodes_Function *std_map_Map__7_at(std_map_Map__7 *this, char *key) {
  std_map_Item__7 *node = std_map_Map__7_get_item(this, key);
  ae_assert(((bool)node), "std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__7_insert(std_map_Map__7 *this, char *key, compiler_ast_nodes_Function *value) {
  std_map_Item__7 *node = std_map_Map__7_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__7_hash(this, key);
    std_map_Item__7 *new_node = std_map_Item__7_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__7_resize(this);
    }
  }
}

compiler_ast_nodes_Function *std_map_Map__7_get(std_map_Map__7 *this, char *key, compiler_ast_nodes_Function *defolt) {
  std_map_Item__7 *node = std_map_Map__7_get_item(this, key);
  if (!((bool)node)) {
  return defolt;
  }
  return node->value;
}

void std_map_Map__7_resize(std_map_Map__7 *this) {
  std_map_Item__7 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__51(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__7 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__7_hash(this, node->key);
      std_map_Item__7 *new_node = std_map_Item__7_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__7_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__7_hash(std_map_Map__7 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__7 *std_map_Map__7_get_item(std_map_Map__7 *this, char *key) {
  u32 hash = std_map_Map__7_hash(this, key);
  std_map_Item__7 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__7 *std_map_Map__7_new(u32 capacity) {
  std_map_Map__7 *map = std_mem_alloc__52(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__51(map->num_buckets);
  return map;
}

void std_map_Item__8_free_list(std_map_Item__8 *this) {
  std_map_Item__8 *cur = this;
  while (((bool)cur)) {
    std_map_Item__8 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__8 *std_map_Item__8_new(char *key, compiler_ast_nodes_MatchCase *value, std_map_Item__8 *next) {
  std_map_Item__8 *node = std_mem_alloc__53(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

bool std_map_Map__8_contains(std_map_Map__8 *this, char *key) {
  return ((bool)std_map_Map__8_get_item(this, key));
}

void std_map_Map__8_free(std_map_Map__8 *this) {
  for (u32 i = 0; i < this->num_buckets; i+=1) {
    std_map_Item__8_free_list(this->buckets[i]);
  }
  std_mem_free(this->buckets);
  std_mem_free(this);
}

void std_map_Map__8_insert(std_map_Map__8 *this, char *key, compiler_ast_nodes_MatchCase *value) {
  std_map_Item__8 *node = std_map_Map__8_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__8_hash(this, key);
    std_map_Item__8 *new_node = std_map_Item__8_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__8_resize(this);
    }
  }
}

compiler_ast_nodes_MatchCase *std_map_Map__8_get(std_map_Map__8 *this, char *key, compiler_ast_nodes_MatchCase *defolt) {
  std_map_Item__8 *node = std_map_Map__8_get_item(this, key);
  if (!((bool)node)) {
  return defolt;
  }
  return node->value;
}

void std_map_Map__8_resize(std_map_Map__8 *this) {
  std_map_Item__8 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__54(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__8 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__8_hash(this, node->key);
      std_map_Item__8 *new_node = std_map_Item__8_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__8_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__8_hash(std_map_Map__8 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__8 *std_map_Map__8_get_item(std_map_Map__8 *this, char *key) {
  u32 hash = std_map_Map__8_hash(this, key);
  std_map_Item__8 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__8 *std_map_Map__8_new(u32 capacity) {
  std_map_Map__8 *map = std_mem_alloc__55(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__54(map->num_buckets);
  return map;
}

void std_map_Item__9_free_list(std_map_Item__9 *this) {
  std_map_Item__9 *cur = this;
  while (((bool)cur)) {
    std_map_Item__9 *next = cur->next;
    std_mem_free(cur);
    cur=next;
  }
}

std_map_Item__9 *std_map_Item__9_new(std_span_Span key, bool value, std_map_Item__9 *next) {
  std_map_Item__9 *node = std_mem_alloc__56(1);
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

std_map_Item__9 *std_map_Iterator__9_cur(std_map_Iterator__9 *this) {
  return this->node;
}

void std_map_Iterator__9_next(std_map_Iterator__9 *this) {
  while (this->idx < ((i32)this->map->num_buckets)) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) {
      return;
      }
    }
    this->idx+=1;
    this->node=({ std_map_Item__9 *__yield_0;
      if (this->idx < ((i32)this->map->num_buckets)) {
        __yield_0 = this->map->buckets[this->idx];
      } else {
        __yield_0 = NULL;
      }
;__yield_0; });
    if (((bool)this->node)) {
    return;
    }
  }
}

std_map_Iterator__9 std_map_Iterator__9_make(std_map_Map__9 *map) {
  std_map_Iterator__9 it = (std_map_Iterator__9){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__9_next(&it);
  return it;
}

bool std_map_Iterator__9_has_value(std_map_Iterator__9 *this) {
  return ((bool)this->node);
}

std_map_Iterator__9 std_map_Map__9_iter(std_map_Map__9 *this) {
  return std_map_Iterator__9_make(this);
}

void std_map_Map__9_free(std_map_Map__9 *this) {
  for (u32 i = 0; i < this->num_buckets; i+=1) {
    std_map_Item__9_free_list(this->buckets[i]);
  }
  std_mem_free(this->buckets);
  std_mem_free(this);
}

void std_map_Map__9_insert(std_map_Map__9 *this, std_span_Span key, bool value) {
  std_map_Item__9 *node = std_map_Map__9_get_item(this, key);
  if (((bool)node)) {
    node->value=value;
  } else {
    u32 hash = std_map_Map__9_hash(this, key);
    std_map_Item__9 *new_node = std_map_Item__9_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=1;
    }
    this->buckets[hash]=new_node;
    this->size+=1;
    if (this->size > this->num_buckets) {
      std_map_Map__9_resize(this);
    }
  }
}

void std_map_Map__9_resize(std_map_Map__9 *this) {
  std_map_Item__9 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_size = this->size;
  this->num_collisions=0;
  this->num_buckets*=2;
  this->buckets=std_mem_alloc__57(this->num_buckets);
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__9 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__9_hash(this, node->key);
      std_map_Item__9 *new_node = std_map_Item__9_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=1;
      }
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = 0; i < old_num_buckets; i+=1) {
    std_map_Item__9_free_list(old_buckets[i]);
  }
  std_mem_free(old_buckets);
}

u32 std_map_Map__9_hash(std_map_Map__9 *this, std_span_Span key) {
  u32 hash = std_span_Span_hash(key);
  hash=(hash % this->num_buckets);
  if (hash < 0) {
    hash+=this->num_buckets;
  }
  return hash;
}

std_map_Item__9 *std_map_Map__9_get_item(std_map_Map__9 *this, std_span_Span key) {
  u32 hash = std_map_Map__9_hash(this, key);
  std_map_Item__9 *node = this->buckets[hash];
  while (((bool)node)) {
    if (std_span_Span_eq(node->key, key)) {
      return node;
    }
    node=node->next;
  }
  return NULL;
}

std_map_Map__9 *std_map_Map__9_new(u32 capacity) {
  std_map_Map__9 *map = std_mem_alloc__58(1);
  map->num_buckets=capacity;
  map->buckets=std_mem_alloc__57(map->num_buckets);
  return map;
}

std_vector_Iterator__0 std_vector_Vector__0_iter(std_vector_Vector__0 *this) {
  return std_vector_Iterator__0_make(this);
}

compiler_types_Type *std_vector_Iterator__0_cur(std_vector_Iterator__0 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__0_next(std_vector_Iterator__0 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__0 std_vector_Iterator__0_make(std_vector_Vector__0 *vec) {
  return (std_vector_Iterator__0){.vec=vec, .index=0};
}

bool std_vector_Iterator__0_has_value(std_vector_Iterator__0 *this) {
  return this->index < this->vec->size;
}

compiler_types_Type *std_vector_Vector__0_at(std_vector_Vector__0 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__0_resize(std_vector_Vector__0 *this, u32 new_capacity) {
  this->data=std_mem_realloc__1(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__0 *std_vector_Vector__0_new(u32 capacity) {
  std_vector_Vector__0 *list = std_mem_alloc__59(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__60(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__0_push(std_vector_Vector__0 *this, compiler_types_Type *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__0_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

char *std_vector_Vector__1_pop(std_vector_Vector__1 *this) {
  ae_assert(this->size > 0, "std/vector.oc:51:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=1;
  return this->data[this->size];
}

char *std_vector_Vector__1_back(std_vector_Vector__1 *this, u32 i) {
  ae_assert(this->size > 0, "std/vector.oc:85:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  ae_assert(i < this->size, "std/vector.oc:86:12: Assertion failed: `i < .size`", "Out of bounds in Vector::back");
  return this->data[((this->size - i) - 1)];
}

std_vector_Iterator__1 std_vector_Vector__1_iter(std_vector_Vector__1 *this) {
  return std_vector_Iterator__1_make(this);
}

char *std_vector_Iterator__1_cur(std_vector_Iterator__1 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__1_next(std_vector_Iterator__1 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__1 std_vector_Iterator__1_make(std_vector_Vector__1 *vec) {
  return (std_vector_Iterator__1){.vec=vec, .index=0};
}

bool std_vector_Iterator__1_has_value(std_vector_Iterator__1 *this) {
  return this->index < this->vec->size;
}

char *std_vector_Vector__1_at(std_vector_Vector__1 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__1_free(std_vector_Vector__1 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

void std_vector_Vector__1_resize(std_vector_Vector__1 *this, u32 new_capacity) {
  this->data=std_mem_realloc__2(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__1 *std_vector_Vector__1_new(u32 capacity) {
  std_vector_Vector__1 *list = std_mem_alloc__61(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__62(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__1_push(std_vector_Vector__1 *this, char *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__1_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__2 std_vector_Vector__2_iter(std_vector_Vector__2 *this) {
  return std_vector_Iterator__2_make(this);
}

compiler_ast_nodes_Structure *std_vector_Iterator__2_cur(std_vector_Iterator__2 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__2_next(std_vector_Iterator__2 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__2 std_vector_Iterator__2_make(std_vector_Vector__2 *vec) {
  return (std_vector_Iterator__2){.vec=vec, .index=0};
}

bool std_vector_Iterator__2_has_value(std_vector_Iterator__2 *this) {
  return this->index < this->vec->size;
}

void std_vector_Vector__2_free(std_vector_Vector__2 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

bool std_vector_Vector__2_is_empty(std_vector_Vector__2 *this) {
  return this->size==0;
}

void std_vector_Vector__2_resize(std_vector_Vector__2 *this, u32 new_capacity) {
  this->data=std_mem_realloc__3(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__2 *std_vector_Vector__2_new(u32 capacity) {
  std_vector_Vector__2 *list = std_mem_alloc__63(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__64(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__2_push(std_vector_Vector__2 *this, compiler_ast_nodes_Structure *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__2_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__3 std_vector_Vector__3_iter(std_vector_Vector__3 *this) {
  return std_vector_Iterator__3_make(this);
}

compiler_ast_scopes_TemplateInstance *std_vector_Iterator__3_cur(std_vector_Iterator__3 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__3_next(std_vector_Iterator__3 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__3 std_vector_Iterator__3_make(std_vector_Vector__3 *vec) {
  return (std_vector_Iterator__3){.vec=vec, .index=0};
}

bool std_vector_Iterator__3_has_value(std_vector_Iterator__3 *this) {
  return this->index < this->vec->size;
}

void std_vector_Vector__3_resize(std_vector_Vector__3 *this, u32 new_capacity) {
  this->data=std_mem_realloc__4(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__3 *std_vector_Vector__3_new(u32 capacity) {
  std_vector_Vector__3 *list = std_mem_alloc__65(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__66(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__3_push(std_vector_Vector__3 *this, compiler_ast_scopes_TemplateInstance *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__3_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

compiler_ast_nodes_Variable *std_vector_Vector__4_back(std_vector_Vector__4 *this, u32 i) {
  ae_assert(this->size > 0, "std/vector.oc:85:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  ae_assert(i < this->size, "std/vector.oc:86:12: Assertion failed: `i < .size`", "Out of bounds in Vector::back");
  return this->data[((this->size - i) - 1)];
}

std_vector_Iterator__4 std_vector_Vector__4_iter(std_vector_Vector__4 *this) {
  return std_vector_Iterator__4_make(this);
}

compiler_ast_nodes_Variable *std_vector_Iterator__4_cur(std_vector_Iterator__4 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__4_next(std_vector_Iterator__4 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__4 std_vector_Iterator__4_make(std_vector_Vector__4 *vec) {
  return (std_vector_Iterator__4){.vec=vec, .index=0};
}

bool std_vector_Iterator__4_has_value(std_vector_Iterator__4 *this) {
  return this->index < this->vec->size;
}

compiler_ast_nodes_Variable *std_vector_Vector__4_at(std_vector_Vector__4 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__4_free(std_vector_Vector__4 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

bool std_vector_Vector__4_is_empty(std_vector_Vector__4 *this) {
  return this->size==0;
}

void std_vector_Vector__4_resize(std_vector_Vector__4 *this, u32 new_capacity) {
  this->data=std_mem_realloc__5(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__4 *std_vector_Vector__4_new(u32 capacity) {
  std_vector_Vector__4 *list = std_mem_alloc__67(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__68(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__4_push(std_vector_Vector__4 *this, compiler_ast_nodes_Variable *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__4_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__5 std_vector_Vector__5_iter(std_vector_Vector__5 *this) {
  return std_vector_Iterator__5_make(this);
}

compiler_ast_nodes_ImportPart *std_vector_Iterator__5_cur(std_vector_Iterator__5 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__5_next(std_vector_Iterator__5 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__5 std_vector_Iterator__5_make(std_vector_Vector__5 *vec) {
  return (std_vector_Iterator__5){.vec=vec, .index=0};
}

bool std_vector_Iterator__5_has_value(std_vector_Iterator__5 *this) {
  return this->index < this->vec->size;
}

compiler_ast_nodes_ImportPart *std_vector_Vector__5_at(std_vector_Vector__5 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__5_resize(std_vector_Vector__5 *this, u32 new_capacity) {
  this->data=std_mem_realloc__6(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__5 *std_vector_Vector__5_new(u32 capacity) {
  std_vector_Vector__5 *list = std_mem_alloc__69(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__70(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__5_push(std_vector_Vector__5 *this, compiler_ast_nodes_ImportPart *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__5_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

compiler_ast_nodes_Function *std_vector_Vector__6_pop(std_vector_Vector__6 *this) {
  ae_assert(this->size > 0, "std/vector.oc:51:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=1;
  return this->data[this->size];
}

std_vector_Iterator__6 std_vector_Vector__6_iter(std_vector_Vector__6 *this) {
  return std_vector_Iterator__6_make(this);
}

compiler_ast_nodes_Function *std_vector_Iterator__6_cur(std_vector_Iterator__6 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__6_next(std_vector_Iterator__6 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__6 std_vector_Iterator__6_make(std_vector_Vector__6 *vec) {
  return (std_vector_Iterator__6){.vec=vec, .index=0};
}

bool std_vector_Iterator__6_has_value(std_vector_Iterator__6 *this) {
  return this->index < this->vec->size;
}

bool std_vector_Vector__6_is_empty(std_vector_Vector__6 *this) {
  return this->size==0;
}

void std_vector_Vector__6_resize(std_vector_Vector__6 *this, u32 new_capacity) {
  this->data=std_mem_realloc__7(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__6 *std_vector_Vector__6_new(u32 capacity) {
  std_vector_Vector__6 *list = std_mem_alloc__71(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__72(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__6_push(std_vector_Vector__6 *this, compiler_ast_nodes_Function *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__6_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

compiler_ast_scopes_Scope *std_vector_Vector__7_pop(std_vector_Vector__7 *this) {
  ae_assert(this->size > 0, "std/vector.oc:51:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=1;
  return this->data[this->size];
}

compiler_ast_scopes_Scope *std_vector_Vector__7_at(std_vector_Vector__7 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__7_resize(std_vector_Vector__7 *this, u32 new_capacity) {
  this->data=std_mem_realloc__8(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__7 *std_vector_Vector__7_new(u32 capacity) {
  std_vector_Vector__7 *list = std_mem_alloc__73(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__74(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__7_push(std_vector_Vector__7 *this, compiler_ast_scopes_Scope *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__7_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

compiler_ast_program_Namespace *std_vector_Vector__8_pop(std_vector_Vector__8 *this) {
  ae_assert(this->size > 0, "std/vector.oc:51:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=1;
  return this->data[this->size];
}

compiler_ast_program_Namespace *std_vector_Vector__8_at(std_vector_Vector__8 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__8_free(std_vector_Vector__8 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

bool std_vector_Vector__8_is_empty(std_vector_Vector__8 *this) {
  return this->size==0;
}

void std_vector_Vector__8_resize(std_vector_Vector__8 *this, u32 new_capacity) {
  this->data=std_mem_realloc__9(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__8 *std_vector_Vector__8_new(u32 capacity) {
  std_vector_Vector__8 *list = std_mem_alloc__75(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__76(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__8_push(std_vector_Vector__8 *this, compiler_ast_program_Namespace *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__8_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

compiler_tokens_Token *std_vector_Vector__9_at(std_vector_Vector__9 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__9_free(std_vector_Vector__9 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

compiler_tokens_Token *std_vector_Vector__9_unchecked_at(std_vector_Vector__9 *this, u32 i) {
  return this->data[i];
}

void std_vector_Vector__9_resize(std_vector_Vector__9 *this, u32 new_capacity) {
  this->data=std_mem_realloc__10(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__9 *std_vector_Vector__9_new(u32 capacity) {
  std_vector_Vector__9 *list = std_mem_alloc__77(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__78(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__9_push(std_vector_Vector__9 *this, compiler_tokens_Token *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__9_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__10 std_vector_Vector__10_iter(std_vector_Vector__10 *this) {
  return std_vector_Iterator__10_make(this);
}

compiler_attributes_Attribute *std_vector_Iterator__10_cur(std_vector_Iterator__10 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__10_next(std_vector_Iterator__10 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__10 std_vector_Iterator__10_make(std_vector_Vector__10 *vec) {
  return (std_vector_Iterator__10){.vec=vec, .index=0};
}

bool std_vector_Iterator__10_has_value(std_vector_Iterator__10 *this) {
  return this->index < this->vec->size;
}

void std_vector_Vector__10_resize(std_vector_Vector__10 *this, u32 new_capacity) {
  this->data=std_mem_realloc__11(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__10 *std_vector_Vector__10_new(u32 capacity) {
  std_vector_Vector__10 *list = std_mem_alloc__79(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__80(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__10_push(std_vector_Vector__10 *this, compiler_attributes_Attribute *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__10_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

void std_vector_Vector__10_clear(std_vector_Vector__10 *this) {
  this->size=0;
}

std_vector_Iterator__11 std_vector_Vector__11_iter(std_vector_Vector__11 *this) {
  return std_vector_Iterator__11_make(this);
}

compiler_errors_Error *std_vector_Iterator__11_cur(std_vector_Iterator__11 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__11_next(std_vector_Iterator__11 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__11 std_vector_Iterator__11_make(std_vector_Vector__11 *vec) {
  return (std_vector_Iterator__11){.vec=vec, .index=0};
}

bool std_vector_Iterator__11_has_value(std_vector_Iterator__11 *this) {
  return this->index < this->vec->size;
}

compiler_errors_Error *std_vector_Vector__11_at(std_vector_Vector__11 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__11_free(std_vector_Vector__11 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

bool std_vector_Vector__11_is_empty(std_vector_Vector__11 *this) {
  return this->size==0;
}

void std_vector_Vector__11_resize(std_vector_Vector__11 *this, u32 new_capacity) {
  this->data=std_mem_realloc__12(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__11 *std_vector_Vector__11_new(u32 capacity) {
  std_vector_Vector__11 *list = std_mem_alloc__81(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__82(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__11_push(std_vector_Vector__11 *this, compiler_errors_Error *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__11_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__12 std_vector_Vector__12_iter(std_vector_Vector__12 *this) {
  return std_vector_Iterator__12_make(this);
}

compiler_ast_nodes_Enum *std_vector_Iterator__12_cur(std_vector_Iterator__12 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__12_next(std_vector_Iterator__12 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__12 std_vector_Iterator__12_make(std_vector_Vector__12 *vec) {
  return (std_vector_Iterator__12){.vec=vec, .index=0};
}

bool std_vector_Iterator__12_has_value(std_vector_Iterator__12 *this) {
  return this->index < this->vec->size;
}

bool std_vector_Vector__12_is_empty(std_vector_Vector__12 *this) {
  return this->size==0;
}

void std_vector_Vector__12_resize(std_vector_Vector__12 *this, u32 new_capacity) {
  this->data=std_mem_realloc__13(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__12 *std_vector_Vector__12_new(u32 capacity) {
  std_vector_Vector__12 *list = std_mem_alloc__83(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__84(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__12_push(std_vector_Vector__12 *this, compiler_ast_nodes_Enum *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__12_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__13 std_vector_Vector__13_iter(std_vector_Vector__13 *this) {
  return std_vector_Iterator__13_make(this);
}

compiler_ast_nodes_AST *std_vector_Iterator__13_cur(std_vector_Iterator__13 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__13_next(std_vector_Iterator__13 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__13 std_vector_Iterator__13_make(std_vector_Vector__13 *vec) {
  return (std_vector_Iterator__13){.vec=vec, .index=0};
}

bool std_vector_Iterator__13_has_value(std_vector_Iterator__13 *this) {
  return this->index < this->vec->size;
}

compiler_ast_nodes_AST *std_vector_Vector__13_at(std_vector_Vector__13 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__13_free(std_vector_Vector__13 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

bool std_vector_Vector__13_is_empty(std_vector_Vector__13 *this) {
  return this->size==0;
}

void std_vector_Vector__13_resize(std_vector_Vector__13 *this, u32 new_capacity) {
  this->data=std_mem_realloc__14(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__13 *std_vector_Vector__13_new(u32 capacity) {
  std_vector_Vector__13 *list = std_mem_alloc__85(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__86(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__13_push(std_vector_Vector__13 *this, compiler_ast_nodes_AST *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__13_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__14 std_vector_Vector__14_iter(std_vector_Vector__14 *this) {
  return std_vector_Iterator__14_make(this);
}

compiler_ast_operators_Operator std_vector_Iterator__14_cur(std_vector_Iterator__14 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__14_next(std_vector_Iterator__14 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__14 std_vector_Iterator__14_make(std_vector_Vector__14 *vec) {
  return (std_vector_Iterator__14){.vec=vec, .index=0};
}

bool std_vector_Iterator__14_has_value(std_vector_Iterator__14 *this) {
  return this->index < this->vec->size;
}

void std_vector_Vector__14_resize(std_vector_Vector__14 *this, u32 new_capacity) {
  this->data=std_mem_realloc__15(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__14 *std_vector_Vector__14_new(u32 capacity) {
  std_vector_Vector__14 *list = std_mem_alloc__87(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__88(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__14_push(std_vector_Vector__14 *this, compiler_ast_operators_Operator value) {
  if (this->size==this->capacity) {
    std_vector_Vector__14_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

void std_vector_Vector__15_push_front(std_vector_Vector__15 *this, compiler_ast_nodes_Argument *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__15_resize(this, (this->capacity * 2));
  }
  for (u32 i = this->size; i > 0; i-=1) {
    this->data[i]=this->data[(i - 1)];
  }
  this->data[0]=value;
  this->size+=1;
}

std_vector_Iterator__15 std_vector_Vector__15_iter(std_vector_Vector__15 *this) {
  return std_vector_Iterator__15_make(this);
}

compiler_ast_nodes_Argument *std_vector_Iterator__15_cur(std_vector_Iterator__15 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__15_next(std_vector_Iterator__15 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__15 std_vector_Iterator__15_make(std_vector_Vector__15 *vec) {
  return (std_vector_Iterator__15){.vec=vec, .index=0};
}

bool std_vector_Iterator__15_has_value(std_vector_Iterator__15 *this) {
  return this->index < this->vec->size;
}

compiler_ast_nodes_Argument *std_vector_Vector__15_at(std_vector_Vector__15 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__15_resize(std_vector_Vector__15 *this, u32 new_capacity) {
  this->data=std_mem_realloc__16(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__15 *std_vector_Vector__15_new(u32 capacity) {
  std_vector_Vector__15 *list = std_mem_alloc__89(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__90(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__15_push(std_vector_Vector__15 *this, compiler_ast_nodes_Argument *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__15_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__16 std_vector_Vector__16_iter(std_vector_Vector__16 *this) {
  return std_vector_Iterator__16_make(this);
}

std_vector_Vector__5 *std_vector_Iterator__16_cur(std_vector_Iterator__16 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__16_next(std_vector_Iterator__16 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__16 std_vector_Iterator__16_make(std_vector_Vector__16 *vec) {
  return (std_vector_Iterator__16){.vec=vec, .index=0};
}

bool std_vector_Iterator__16_has_value(std_vector_Iterator__16 *this) {
  return this->index < this->vec->size;
}

std_vector_Vector__5 *std_vector_Vector__16_at(std_vector_Vector__16 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__16_resize(std_vector_Vector__16 *this, u32 new_capacity) {
  this->data=std_mem_realloc__17(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__16 *std_vector_Vector__16_new(u32 capacity) {
  std_vector_Vector__16 *list = std_mem_alloc__91(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__92(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__16_push(std_vector_Vector__16 *this, std_vector_Vector__5 *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__16_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__17 std_vector_Vector__17_iter(std_vector_Vector__17 *this) {
  return std_vector_Iterator__17_make(this);
}

compiler_ast_nodes_IfBranch std_vector_Iterator__17_cur(std_vector_Iterator__17 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__17_next(std_vector_Iterator__17 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__17 std_vector_Iterator__17_make(std_vector_Vector__17 *vec) {
  return (std_vector_Iterator__17){.vec=vec, .index=0};
}

bool std_vector_Iterator__17_has_value(std_vector_Iterator__17 *this) {
  return this->index < this->vec->size;
}

compiler_ast_nodes_IfBranch std_vector_Vector__17_at(std_vector_Vector__17 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__17_resize(std_vector_Vector__17 *this, u32 new_capacity) {
  this->data=std_mem_realloc__18(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__17 *std_vector_Vector__17_new(u32 capacity) {
  std_vector_Vector__17 *list = std_mem_alloc__93(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__94(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__17_push(std_vector_Vector__17 *this, compiler_ast_nodes_IfBranch value) {
  if (this->size==this->capacity) {
    std_vector_Vector__17_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__18 std_vector_Vector__18_iter(std_vector_Vector__18 *this) {
  return std_vector_Iterator__18_make(this);
}

compiler_ast_nodes_MatchCase *std_vector_Iterator__18_cur(std_vector_Iterator__18 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__18_next(std_vector_Iterator__18 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__18 std_vector_Iterator__18_make(std_vector_Vector__18 *vec) {
  return (std_vector_Iterator__18){.vec=vec, .index=0};
}

bool std_vector_Iterator__18_has_value(std_vector_Iterator__18 *this) {
  return this->index < this->vec->size;
}

compiler_ast_nodes_MatchCase *std_vector_Vector__18_at(std_vector_Vector__18 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__18_resize(std_vector_Vector__18 *this, u32 new_capacity) {
  this->data=std_mem_realloc__19(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__18 *std_vector_Vector__18_new(u32 capacity) {
  std_vector_Vector__18 *list = std_mem_alloc__95(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__96(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__18_push(std_vector_Vector__18 *this, compiler_ast_nodes_MatchCase *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__18_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__19 std_vector_Vector__19_iter(std_vector_Vector__19 *this) {
  return std_vector_Iterator__19_make(this);
}

compiler_ast_scopes_Symbol *std_vector_Iterator__19_cur(std_vector_Iterator__19 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__19_next(std_vector_Iterator__19 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__19 std_vector_Iterator__19_make(std_vector_Vector__19 *vec) {
  return (std_vector_Iterator__19){.vec=vec, .index=0};
}

bool std_vector_Iterator__19_has_value(std_vector_Iterator__19 *this) {
  return this->index < this->vec->size;
}

compiler_ast_scopes_Symbol *std_vector_Vector__19_at(std_vector_Vector__19 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__19_resize(std_vector_Vector__19 *this, u32 new_capacity) {
  this->data=std_mem_realloc__20(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__19 *std_vector_Vector__19_new(u32 capacity) {
  std_vector_Vector__19 *list = std_mem_alloc__97(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__98(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__19_push(std_vector_Vector__19 *this, compiler_ast_scopes_Symbol *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__19_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__20 std_vector_Vector__20_iter(std_vector_Vector__20 *this) {
  return std_vector_Iterator__20_make(this);
}

compiler_ast_scopes_Reference std_vector_Iterator__20_cur(std_vector_Iterator__20 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__20_next(std_vector_Iterator__20 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__20 std_vector_Iterator__20_make(std_vector_Vector__20 *vec) {
  return (std_vector_Iterator__20){.vec=vec, .index=0};
}

bool std_vector_Iterator__20_has_value(std_vector_Iterator__20 *this) {
  return this->index < this->vec->size;
}

void std_vector_Vector__20_resize(std_vector_Vector__20 *this, u32 new_capacity) {
  this->data=std_mem_realloc__21(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__20 *std_vector_Vector__20_new(u32 capacity) {
  std_vector_Vector__20 *list = std_mem_alloc__99(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__100(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__20_push(std_vector_Vector__20 *this, compiler_ast_scopes_Reference value) {
  if (this->size==this->capacity) {
    std_vector_Vector__20_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__21 std_vector_Vector__21_iter(std_vector_Vector__21 *this) {
  return std_vector_Iterator__21_make(this);
}

std_span_Span std_vector_Iterator__21_cur(std_vector_Iterator__21 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__21_next(std_vector_Iterator__21 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__21 std_vector_Iterator__21_make(std_vector_Vector__21 *vec) {
  return (std_vector_Iterator__21){.vec=vec, .index=0};
}

bool std_vector_Iterator__21_has_value(std_vector_Iterator__21 *this) {
  return this->index < this->vec->size;
}

void std_vector_Vector__21_free(std_vector_Vector__21 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

void std_vector_Vector__21_resize(std_vector_Vector__21 *this, u32 new_capacity) {
  this->data=std_mem_realloc__22(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__21 *std_vector_Vector__21_new(u32 capacity) {
  std_vector_Vector__21 *list = std_mem_alloc__101(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__102(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__21_push(std_vector_Vector__21 *this, std_span_Span value) {
  if (this->size==this->capacity) {
    std_vector_Vector__21_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_value_Value *std_vector_Vector__22_at(std_vector_Vector__22 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__22_resize(std_vector_Vector__22 *this, u32 new_capacity) {
  this->data=std_mem_realloc__23(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__22 *std_vector_Vector__22_new(u32 capacity) {
  std_vector_Vector__22 *list = std_mem_alloc__103(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__104(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__22_push(std_vector_Vector__22 *this, std_value_Value *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__22_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

std_vector_Iterator__23 std_vector_Vector__23_iter(std_vector_Vector__23 *this) {
  return std_vector_Iterator__23_make(this);
}

std_compact_map_Item__0 std_vector_Iterator__23_cur(std_vector_Iterator__23 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:138:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__23_next(std_vector_Iterator__23 *this) {
  ae_assert(this->index < this->vec->size, "std/vector.oc:133:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=1;
}

std_vector_Iterator__23 std_vector_Iterator__23_make(std_vector_Vector__23 *vec) {
  return (std_vector_Iterator__23){.vec=vec, .index=0};
}

bool std_vector_Iterator__23_has_value(std_vector_Iterator__23 *this) {
  return this->index < this->vec->size;
}

std_compact_map_Item__0 std_vector_Vector__23_at(std_vector_Vector__23 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__23_resize(std_vector_Vector__23 *this, u32 new_capacity) {
  this->data=std_mem_realloc__24(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__23 *std_vector_Vector__23_new(u32 capacity) {
  std_vector_Vector__23 *list = std_mem_alloc__105(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__106(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__23_push(std_vector_Vector__23 *this, std_compact_map_Item__0 value) {
  if (this->size==this->capacity) {
    std_vector_Vector__23_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

u32 std_vector_Vector__24_at(std_vector_Vector__24 *this, u32 i) {
  ae_assert(i < this->size, "std/vector.oc:93:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__24_free(std_vector_Vector__24 *this) {
  std_mem_free(this->data);
  std_mem_free(this);
}

void std_vector_Vector__24_resize(std_vector_Vector__24 *this, u32 new_capacity) {
  this->data=std_mem_realloc__25(this->data, this->capacity, new_capacity);
  this->capacity=new_capacity;
}

std_vector_Vector__24 *std_vector_Vector__24_new(u32 capacity) {
  std_vector_Vector__24 *list = std_mem_alloc__107(1);
  list->capacity=capacity;
  list->data=std_mem_alloc__108(capacity);
  list->size=0;
  return list;
}

void std_vector_Vector__24_push(std_vector_Vector__24 *this, u32 value) {
  if (this->size==this->capacity) {
    std_vector_Vector__24_resize(this, (this->capacity * 2));
  }
  this->data[this->size]=value;
  this->size+=1;
}

void std_json_serialize_into(std_value_Value *val, std_buffer_Buffer *sb) {
  switch (val->type) {
    case std_value_ValueType_Null: {
      std_buffer_Buffer_write_str(sb, "null");
    } break;
    case std_value_ValueType_Bool: {
      std_buffer_Buffer_write_str(sb, (val->u.as_bool ? "true" : "false"));
    } break;
    case std_value_ValueType_Integer: {
      std_buffer_Buffer_write_str_f(sb, format_string("%" PRId64 "", val->u.as_int));
    } break;
    case std_value_ValueType_Float: {
      std_buffer_Buffer_write_str_f(sb, format_string("%f", val->u.as_float));
    } break;
    case std_value_ValueType_String: {
      std_buffer_Buffer_write_str(sb, "\"");
      std_buffer_Buffer buf = val->u.as_str;
      for (u32 i = 0; i < buf.size; i+=1) {
        char c = ((char)buf.data[i]);
        switch (c) {
          case '\b': {
            std_buffer_Buffer_write_str(sb, "\\b");
          } break;
          case '\f': {
            std_buffer_Buffer_write_str(sb, "\\f");
          } break;
          case '\n': {
            std_buffer_Buffer_write_str(sb, "\\n");
          } break;
          case '\r': {
            std_buffer_Buffer_write_str(sb, "\\r");
          } break;
          case '\t': {
            std_buffer_Buffer_write_str(sb, "\\t");
          } break;
          case '\\': {
            std_buffer_Buffer_write_str(sb, "\\\\");
          } break;
          case '"': {
            std_buffer_Buffer_write_str(sb, "\\\"");
          } break;
          default: {
            if (char_is_print(c)) {
              std_buffer_Buffer_write_char(sb, c);
            } else {
              std_buffer_Buffer_write_str_f(sb, format_string("\\x%02x", buf.data[i]));
            }
          } break;
        }
      }
      std_buffer_Buffer_write_str(sb, "\"");
    } break;
    case std_value_ValueType_List: {
      std_buffer_Buffer_write_str(sb, "[");
      std_vector_Vector__22 *lst = val->u.as_list;
      for (u32 i = 0; i < lst->size; i+=1) {
        std_value_Value *value = std_vector_Vector__22_at(lst, i);
        if (i > 0) {
          std_buffer_Buffer_write_str(sb, ",");
        }
        std_json_serialize_into(value, sb);
      }
      std_buffer_Buffer_write_str(sb, "]");
    } break;
    case std_value_ValueType_Dictionary: {
      std_buffer_Buffer_write_str(sb, "{");
      bool first = true;
      for (std_compact_map_Iterator__0 __iter = std_compact_map_Map__0_iter(val->u.as_dict); std_compact_map_Iterator__0_has_value(&__iter); std_compact_map_Iterator__0_next(&__iter)) {
        std_compact_map_Item__0 iter = std_compact_map_Iterator__0_cur(&__iter);
        {
          if (!first) {
            std_buffer_Buffer_write_str(sb, ",");
          }
          first=false;
          std_buffer_Buffer_write_str(sb, "\"");
          std_buffer_Buffer_write_str(sb, iter.key);
          std_buffer_Buffer_write_str(sb, "\":");
          std_value_Value *value = iter.value;
          std_json_serialize_into(value, sb);
        }
      }
      std_buffer_Buffer_write_str(sb, "}");
    } break;
  }
}

std_buffer_Buffer std_json_serialize(std_value_Value *val) {
  std_buffer_Buffer sb = std_buffer_Buffer_make(16);
  std_json_serialize_into(val, &sb);
  return sb;
}

void std_json_write_to_file(std_value_Value *val, char *filename) {
  std_buffer_Buffer sb = std_json_serialize(val);
  std_fs_write_file(filename, sb);
  std_buffer_Buffer_free(&sb);
}

