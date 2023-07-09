#include "ctype.h"
#include "dirent.h"
#include "libgen.h"

/* Embed: .//std/./prelude.h */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>

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
typedef enum std_json_TokenType {
  std_json_TokenType_OpenCurly,
  std_json_TokenType_OpenSquare,
  std_json_TokenType_CloseCurly,
  std_json_TokenType_CloseSquare,
  std_json_TokenType_Colon,
  std_json_TokenType_Comma,
  std_json_TokenType_EOF,
  std_json_TokenType_FloatLiteral,
  std_json_TokenType_Identifier,
  std_json_TokenType_IntLiteral,
  std_json_TokenType_StringLiteral,
  std_json_TokenType_BEGIN_KEYWORDS,
  std_json_TokenType_False,
  std_json_TokenType_Null,
  std_json_TokenType_True,
} std_json_TokenType;

char *std_json_TokenType_dbg(std_json_TokenType this) {
  switch (this) {
    case std_json_TokenType_OpenCurly: return "OpenCurly";
    case std_json_TokenType_OpenSquare: return "OpenSquare";
    case std_json_TokenType_CloseCurly: return "CloseCurly";
    case std_json_TokenType_CloseSquare: return "CloseSquare";
    case std_json_TokenType_Colon: return "Colon";
    case std_json_TokenType_Comma: return "Comma";
    case std_json_TokenType_EOF: return "EOF";
    case std_json_TokenType_FloatLiteral: return "FloatLiteral";
    case std_json_TokenType_Identifier: return "Identifier";
    case std_json_TokenType_IntLiteral: return "IntLiteral";
    case std_json_TokenType_StringLiteral: return "StringLiteral";
    case std_json_TokenType_BEGIN_KEYWORDS: return "BEGIN_KEYWORDS";
    case std_json_TokenType_False: return "False";
    case std_json_TokenType_Null: return "Null";
    case std_json_TokenType_True: return "True";
    default: return "<unknown>";
  }
}

typedef enum std_value_ValueType {
  std_value_ValueType_Null,
  std_value_ValueType_Bool,
  std_value_ValueType_Integer,
  std_value_ValueType_String,
  std_value_ValueType_List,
  std_value_ValueType_Dictionary,
} std_value_ValueType;

char *std_value_ValueType_dbg(std_value_ValueType this) {
  switch (this) {
    case std_value_ValueType_Null: return "Null";
    case std_value_ValueType_Bool: return "Bool";
    case std_value_ValueType_Integer: return "Integer";
    case std_value_ValueType_String: return "String";
    case std_value_ValueType_List: return "List";
    case std_value_ValueType_Dictionary: return "Dictionary";
    default: return "<unknown>";
  }
}

typedef enum ast_scopes_SymbolType {
  ast_scopes_SymbolType_Function,
  ast_scopes_SymbolType_Structure,
  ast_scopes_SymbolType_Enum,
  ast_scopes_SymbolType_TypeDef,
  ast_scopes_SymbolType_Namespace,
  ast_scopes_SymbolType_Variable,
  ast_scopes_SymbolType_Constant,
} ast_scopes_SymbolType;

char *ast_scopes_SymbolType_dbg(ast_scopes_SymbolType this) {
  switch (this) {
    case ast_scopes_SymbolType_Function: return "Function";
    case ast_scopes_SymbolType_Structure: return "Structure";
    case ast_scopes_SymbolType_Enum: return "Enum";
    case ast_scopes_SymbolType_TypeDef: return "TypeDef";
    case ast_scopes_SymbolType_Namespace: return "Namespace";
    case ast_scopes_SymbolType_Variable: return "Variable";
    case ast_scopes_SymbolType_Constant: return "Constant";
    default: return "<unknown>";
  }
}

typedef enum ast_nodes_ASTType {
  ast_nodes_ASTType_Assert,
  ast_nodes_ASTType_Block,
  ast_nodes_ASTType_BoolLiteral,
  ast_nodes_ASTType_Break,
  ast_nodes_ASTType_Call,
  ast_nodes_ASTType_Constant,
  ast_nodes_ASTType_Continue,
  ast_nodes_ASTType_Error,
  ast_nodes_ASTType_Identifier,
  ast_nodes_ASTType_If,
  ast_nodes_ASTType_Import,
  ast_nodes_ASTType_IntLiteral,
  ast_nodes_ASTType_Member,
  ast_nodes_ASTType_NSLookup,
  ast_nodes_ASTType_Return,
  ast_nodes_ASTType_Yield,
  ast_nodes_ASTType_StringLiteral,
  ast_nodes_ASTType_SizeOf,
  ast_nodes_ASTType_VarDeclaration,
  ast_nodes_ASTType_While,
  ast_nodes_ASTType_For,
  ast_nodes_ASTType_CharLiteral,
  ast_nodes_ASTType_FloatLiteral,
  ast_nodes_ASTType_FormatStringLiteral,
  ast_nodes_ASTType_Cast,
  ast_nodes_ASTType_Null,
  ast_nodes_ASTType_MethodCall,
  ast_nodes_ASTType_Match,
  ast_nodes_ASTType_Index,
  ast_nodes_ASTType_Defer,
  ast_nodes_ASTType_Specialization,
  ast_nodes_ASTType_PreIncrement,
  ast_nodes_ASTType_PreDecrement,
  ast_nodes_ASTType_PostIncrement,
  ast_nodes_ASTType_PostDecrement,
  ast_nodes_ASTType_Address,
  ast_nodes_ASTType_Dereference,
  ast_nodes_ASTType_Negate,
  ast_nodes_ASTType_Not,
  ast_nodes_ASTType_IsNotNull,
  ast_nodes_ASTType_And,
  ast_nodes_ASTType_Assignment,
  ast_nodes_ASTType_BitwiseAnd,
  ast_nodes_ASTType_BitwiseNot,
  ast_nodes_ASTType_BitwiseOr,
  ast_nodes_ASTType_BitwiseXor,
  ast_nodes_ASTType_Divide,
  ast_nodes_ASTType_DivideEquals,
  ast_nodes_ASTType_Equals,
  ast_nodes_ASTType_GreaterThan,
  ast_nodes_ASTType_GreaterThanEquals,
  ast_nodes_ASTType_LeftShift,
  ast_nodes_ASTType_LessThan,
  ast_nodes_ASTType_LessThanEquals,
  ast_nodes_ASTType_Minus,
  ast_nodes_ASTType_MinusEquals,
  ast_nodes_ASTType_Modulus,
  ast_nodes_ASTType_Multiply,
  ast_nodes_ASTType_MultiplyEquals,
  ast_nodes_ASTType_NotEquals,
  ast_nodes_ASTType_Or,
  ast_nodes_ASTType_Plus,
  ast_nodes_ASTType_PlusEquals,
  ast_nodes_ASTType_RightShift,
} ast_nodes_ASTType;

char *ast_nodes_ASTType_dbg(ast_nodes_ASTType this) {
  switch (this) {
    case ast_nodes_ASTType_Assert: return "Assert";
    case ast_nodes_ASTType_Block: return "Block";
    case ast_nodes_ASTType_BoolLiteral: return "BoolLiteral";
    case ast_nodes_ASTType_Break: return "Break";
    case ast_nodes_ASTType_Call: return "Call";
    case ast_nodes_ASTType_Constant: return "Constant";
    case ast_nodes_ASTType_Continue: return "Continue";
    case ast_nodes_ASTType_Error: return "Error";
    case ast_nodes_ASTType_Identifier: return "Identifier";
    case ast_nodes_ASTType_If: return "If";
    case ast_nodes_ASTType_Import: return "Import";
    case ast_nodes_ASTType_IntLiteral: return "IntLiteral";
    case ast_nodes_ASTType_Member: return "Member";
    case ast_nodes_ASTType_NSLookup: return "NSLookup";
    case ast_nodes_ASTType_Return: return "Return";
    case ast_nodes_ASTType_Yield: return "Yield";
    case ast_nodes_ASTType_StringLiteral: return "StringLiteral";
    case ast_nodes_ASTType_SizeOf: return "SizeOf";
    case ast_nodes_ASTType_VarDeclaration: return "VarDeclaration";
    case ast_nodes_ASTType_While: return "While";
    case ast_nodes_ASTType_For: return "For";
    case ast_nodes_ASTType_CharLiteral: return "CharLiteral";
    case ast_nodes_ASTType_FloatLiteral: return "FloatLiteral";
    case ast_nodes_ASTType_FormatStringLiteral: return "FormatStringLiteral";
    case ast_nodes_ASTType_Cast: return "Cast";
    case ast_nodes_ASTType_Null: return "Null";
    case ast_nodes_ASTType_MethodCall: return "MethodCall";
    case ast_nodes_ASTType_Match: return "Match";
    case ast_nodes_ASTType_Index: return "Index";
    case ast_nodes_ASTType_Defer: return "Defer";
    case ast_nodes_ASTType_Specialization: return "Specialization";
    case ast_nodes_ASTType_PreIncrement: return "PreIncrement";
    case ast_nodes_ASTType_PreDecrement: return "PreDecrement";
    case ast_nodes_ASTType_PostIncrement: return "PostIncrement";
    case ast_nodes_ASTType_PostDecrement: return "PostDecrement";
    case ast_nodes_ASTType_Address: return "Address";
    case ast_nodes_ASTType_Dereference: return "Dereference";
    case ast_nodes_ASTType_Negate: return "Negate";
    case ast_nodes_ASTType_Not: return "Not";
    case ast_nodes_ASTType_IsNotNull: return "IsNotNull";
    case ast_nodes_ASTType_And: return "And";
    case ast_nodes_ASTType_Assignment: return "Assignment";
    case ast_nodes_ASTType_BitwiseAnd: return "BitwiseAnd";
    case ast_nodes_ASTType_BitwiseNot: return "BitwiseNot";
    case ast_nodes_ASTType_BitwiseOr: return "BitwiseOr";
    case ast_nodes_ASTType_BitwiseXor: return "BitwiseXor";
    case ast_nodes_ASTType_Divide: return "Divide";
    case ast_nodes_ASTType_DivideEquals: return "DivideEquals";
    case ast_nodes_ASTType_Equals: return "Equals";
    case ast_nodes_ASTType_GreaterThan: return "GreaterThan";
    case ast_nodes_ASTType_GreaterThanEquals: return "GreaterThanEquals";
    case ast_nodes_ASTType_LeftShift: return "LeftShift";
    case ast_nodes_ASTType_LessThan: return "LessThan";
    case ast_nodes_ASTType_LessThanEquals: return "LessThanEquals";
    case ast_nodes_ASTType_Minus: return "Minus";
    case ast_nodes_ASTType_MinusEquals: return "MinusEquals";
    case ast_nodes_ASTType_Modulus: return "Modulus";
    case ast_nodes_ASTType_Multiply: return "Multiply";
    case ast_nodes_ASTType_MultiplyEquals: return "MultiplyEquals";
    case ast_nodes_ASTType_NotEquals: return "NotEquals";
    case ast_nodes_ASTType_Or: return "Or";
    case ast_nodes_ASTType_Plus: return "Plus";
    case ast_nodes_ASTType_PlusEquals: return "PlusEquals";
    case ast_nodes_ASTType_RightShift: return "RightShift";
    default: return "<unknown>";
  }
}

typedef enum ast_nodes_ImportPartType {
  ast_nodes_ImportPartType_Single,
  ast_nodes_ImportPartType_Multiple,
  ast_nodes_ImportPartType_Wildcard,
} ast_nodes_ImportPartType;

char *ast_nodes_ImportPartType_dbg(ast_nodes_ImportPartType this) {
  switch (this) {
    case ast_nodes_ImportPartType_Single: return "Single";
    case ast_nodes_ImportPartType_Multiple: return "Multiple";
    case ast_nodes_ImportPartType_Wildcard: return "Wildcard";
    default: return "<unknown>";
  }
}

typedef enum ast_nodes_ImportType {
  ast_nodes_ImportType_GlobalNamespace,
  ast_nodes_ImportType_ProjectNamespace,
  ast_nodes_ImportType_ParentNamespace,
  ast_nodes_ImportType_CurrentScope,
} ast_nodes_ImportType;

char *ast_nodes_ImportType_dbg(ast_nodes_ImportType this) {
  switch (this) {
    case ast_nodes_ImportType_GlobalNamespace: return "GlobalNamespace";
    case ast_nodes_ImportType_ProjectNamespace: return "ProjectNamespace";
    case ast_nodes_ImportType_ParentNamespace: return "ParentNamespace";
    case ast_nodes_ImportType_CurrentScope: return "CurrentScope";
    default: return "<unknown>";
  }
}

typedef enum tokens_TokenType {
  tokens_TokenType_AtSign,
  tokens_TokenType_Ampersand,
  tokens_TokenType_Backtick,
  tokens_TokenType_Caret,
  tokens_TokenType_CharLiteral,
  tokens_TokenType_CloseCurly,
  tokens_TokenType_CloseParen,
  tokens_TokenType_CloseSquare,
  tokens_TokenType_Colon,
  tokens_TokenType_ColonColon,
  tokens_TokenType_Comma,
  tokens_TokenType_Dot,
  tokens_TokenType_EOF,
  tokens_TokenType_Equals,
  tokens_TokenType_EqualEquals,
  tokens_TokenType_Exclamation,
  tokens_TokenType_FatArrow,
  tokens_TokenType_FloatLiteral,
  tokens_TokenType_FormatStringLiteral,
  tokens_TokenType_GreaterThan,
  tokens_TokenType_GreaterThanEquals,
  tokens_TokenType_Identifier,
  tokens_TokenType_IntLiteral,
  tokens_TokenType_LessThan,
  tokens_TokenType_LessThanEquals,
  tokens_TokenType_Line,
  tokens_TokenType_Minus,
  tokens_TokenType_MinusEquals,
  tokens_TokenType_MinusMinus,
  tokens_TokenType_NotEquals,
  tokens_TokenType_OpenCurly,
  tokens_TokenType_OpenParen,
  tokens_TokenType_OpenSquare,
  tokens_TokenType_Percent,
  tokens_TokenType_Plus,
  tokens_TokenType_PlusEquals,
  tokens_TokenType_PlusPlus,
  tokens_TokenType_Question,
  tokens_TokenType_Semicolon,
  tokens_TokenType_Slash,
  tokens_TokenType_SlashEquals,
  tokens_TokenType_Star,
  tokens_TokenType_StarEquals,
  tokens_TokenType_StringLiteral,
  tokens_TokenType_Tilde,
  tokens_TokenType_Newline,
  tokens_TokenType_BEGIN_KEYWORDS,
  tokens_TokenType_And,
  tokens_TokenType_As,
  tokens_TokenType_Assert,
  tokens_TokenType_Break,
  tokens_TokenType_Const,
  tokens_TokenType_Continue,
  tokens_TokenType_Def,
  tokens_TokenType_Defer,
  tokens_TokenType_Else,
  tokens_TokenType_Enum,
  tokens_TokenType_Extern,
  tokens_TokenType_False,
  tokens_TokenType_For,
  tokens_TokenType_Fn,
  tokens_TokenType_If,
  tokens_TokenType_Let,
  tokens_TokenType_Match,
  tokens_TokenType_Namespace,
  tokens_TokenType_Null,
  tokens_TokenType_Not,
  tokens_TokenType_Or,
  tokens_TokenType_Return,
  tokens_TokenType_SizeOf,
  tokens_TokenType_Struct,
  tokens_TokenType_True,
  tokens_TokenType_Then,
  tokens_TokenType_Union,
  tokens_TokenType_Import,
  tokens_TokenType_Void,
  tokens_TokenType_Yield,
  tokens_TokenType_While,
} tokens_TokenType;

char *tokens_TokenType_dbg(tokens_TokenType this) {
  switch (this) {
    case tokens_TokenType_AtSign: return "AtSign";
    case tokens_TokenType_Ampersand: return "Ampersand";
    case tokens_TokenType_Backtick: return "Backtick";
    case tokens_TokenType_Caret: return "Caret";
    case tokens_TokenType_CharLiteral: return "CharLiteral";
    case tokens_TokenType_CloseCurly: return "CloseCurly";
    case tokens_TokenType_CloseParen: return "CloseParen";
    case tokens_TokenType_CloseSquare: return "CloseSquare";
    case tokens_TokenType_Colon: return "Colon";
    case tokens_TokenType_ColonColon: return "ColonColon";
    case tokens_TokenType_Comma: return "Comma";
    case tokens_TokenType_Dot: return "Dot";
    case tokens_TokenType_EOF: return "EOF";
    case tokens_TokenType_Equals: return "Equals";
    case tokens_TokenType_EqualEquals: return "EqualEquals";
    case tokens_TokenType_Exclamation: return "Exclamation";
    case tokens_TokenType_FatArrow: return "FatArrow";
    case tokens_TokenType_FloatLiteral: return "FloatLiteral";
    case tokens_TokenType_FormatStringLiteral: return "FormatStringLiteral";
    case tokens_TokenType_GreaterThan: return "GreaterThan";
    case tokens_TokenType_GreaterThanEquals: return "GreaterThanEquals";
    case tokens_TokenType_Identifier: return "Identifier";
    case tokens_TokenType_IntLiteral: return "IntLiteral";
    case tokens_TokenType_LessThan: return "LessThan";
    case tokens_TokenType_LessThanEquals: return "LessThanEquals";
    case tokens_TokenType_Line: return "Line";
    case tokens_TokenType_Minus: return "Minus";
    case tokens_TokenType_MinusEquals: return "MinusEquals";
    case tokens_TokenType_MinusMinus: return "MinusMinus";
    case tokens_TokenType_NotEquals: return "NotEquals";
    case tokens_TokenType_OpenCurly: return "OpenCurly";
    case tokens_TokenType_OpenParen: return "OpenParen";
    case tokens_TokenType_OpenSquare: return "OpenSquare";
    case tokens_TokenType_Percent: return "Percent";
    case tokens_TokenType_Plus: return "Plus";
    case tokens_TokenType_PlusEquals: return "PlusEquals";
    case tokens_TokenType_PlusPlus: return "PlusPlus";
    case tokens_TokenType_Question: return "Question";
    case tokens_TokenType_Semicolon: return "Semicolon";
    case tokens_TokenType_Slash: return "Slash";
    case tokens_TokenType_SlashEquals: return "SlashEquals";
    case tokens_TokenType_Star: return "Star";
    case tokens_TokenType_StarEquals: return "StarEquals";
    case tokens_TokenType_StringLiteral: return "StringLiteral";
    case tokens_TokenType_Tilde: return "Tilde";
    case tokens_TokenType_Newline: return "Newline";
    case tokens_TokenType_BEGIN_KEYWORDS: return "BEGIN_KEYWORDS";
    case tokens_TokenType_And: return "And";
    case tokens_TokenType_As: return "As";
    case tokens_TokenType_Assert: return "Assert";
    case tokens_TokenType_Break: return "Break";
    case tokens_TokenType_Const: return "Const";
    case tokens_TokenType_Continue: return "Continue";
    case tokens_TokenType_Def: return "Def";
    case tokens_TokenType_Defer: return "Defer";
    case tokens_TokenType_Else: return "Else";
    case tokens_TokenType_Enum: return "Enum";
    case tokens_TokenType_Extern: return "Extern";
    case tokens_TokenType_False: return "False";
    case tokens_TokenType_For: return "For";
    case tokens_TokenType_Fn: return "Fn";
    case tokens_TokenType_If: return "If";
    case tokens_TokenType_Let: return "Let";
    case tokens_TokenType_Match: return "Match";
    case tokens_TokenType_Namespace: return "Namespace";
    case tokens_TokenType_Null: return "Null";
    case tokens_TokenType_Not: return "Not";
    case tokens_TokenType_Or: return "Or";
    case tokens_TokenType_Return: return "Return";
    case tokens_TokenType_SizeOf: return "SizeOf";
    case tokens_TokenType_Struct: return "Struct";
    case tokens_TokenType_True: return "True";
    case tokens_TokenType_Then: return "Then";
    case tokens_TokenType_Union: return "Union";
    case tokens_TokenType_Import: return "Import";
    case tokens_TokenType_Void: return "Void";
    case tokens_TokenType_Yield: return "Yield";
    case tokens_TokenType_While: return "While";
    default: return "<unknown>";
  }
}

typedef enum errors_ErrorType {
  errors_ErrorType_Standard,
  errors_ErrorType_WithNote,
  errors_ErrorType_WithHint,
} errors_ErrorType;

char *errors_ErrorType_dbg(errors_ErrorType this) {
  switch (this) {
    case errors_ErrorType_Standard: return "Standard";
    case errors_ErrorType_WithNote: return "WithNote";
    case errors_ErrorType_WithHint: return "WithHint";
    default: return "<unknown>";
  }
}

typedef enum errors_MessageType {
  errors_MessageType_Error,
  errors_MessageType_Warning,
  errors_MessageType_Note,
} errors_MessageType;

char *errors_MessageType_dbg(errors_MessageType this) {
  switch (this) {
    case errors_MessageType_Error: return "Error";
    case errors_MessageType_Warning: return "Warning";
    case errors_MessageType_Note: return "Note";
    default: return "<unknown>";
  }
}

typedef enum types_BaseType {
  types_BaseType_Char,
  types_BaseType_Bool,
  types_BaseType_Void,
  types_BaseType_I8,
  types_BaseType_I16,
  types_BaseType_I32,
  types_BaseType_I64,
  types_BaseType_U8,
  types_BaseType_U16,
  types_BaseType_U32,
  types_BaseType_U64,
  types_BaseType_F32,
  types_BaseType_F64,
  types_BaseType_NUM_BASE_TYPES,
  types_BaseType_Pointer,
  types_BaseType_Function,
  types_BaseType_Structure,
  types_BaseType_Enum,
  types_BaseType_Unresolved,
  types_BaseType_Array,
  types_BaseType_Alias,
  types_BaseType_UnresolvedTemplate,
  types_BaseType_Error,
} types_BaseType;

char *types_BaseType_dbg(types_BaseType this) {
  switch (this) {
    case types_BaseType_Char: return "Char";
    case types_BaseType_Bool: return "Bool";
    case types_BaseType_Void: return "Void";
    case types_BaseType_I8: return "I8";
    case types_BaseType_I16: return "I16";
    case types_BaseType_I32: return "I32";
    case types_BaseType_I64: return "I64";
    case types_BaseType_U8: return "U8";
    case types_BaseType_U16: return "U16";
    case types_BaseType_U32: return "U32";
    case types_BaseType_U64: return "U64";
    case types_BaseType_F32: return "F32";
    case types_BaseType_F64: return "F64";
    case types_BaseType_NUM_BASE_TYPES: return "NUM_BASE_TYPES";
    case types_BaseType_Pointer: return "Pointer";
    case types_BaseType_Function: return "Function";
    case types_BaseType_Structure: return "Structure";
    case types_BaseType_Enum: return "Enum";
    case types_BaseType_Unresolved: return "Unresolved";
    case types_BaseType_Array: return "Array";
    case types_BaseType_Alias: return "Alias";
    case types_BaseType_UnresolvedTemplate: return "UnresolvedTemplate";
    case types_BaseType_Error: return "Error";
    default: return "<unknown>";
  }
}

/* Constants */
#define std_compact_map_INDEX_FREE (-1)
#define std_compact_map_INDEX_DELETED (-2)
/* Struct typedefs */
typedef struct std_compact_map_Item__0 std_compact_map_Item__0;
typedef struct std_compact_map_Map__0 std_compact_map_Map__0;
typedef struct std_vector_Iterator__2 std_vector_Iterator__2;
typedef struct std_compact_map_KeyIterator__0 std_compact_map_KeyIterator__0;
typedef struct std_compact_map_ValueIterator__0 std_compact_map_ValueIterator__0;
typedef struct std_buffer_Buffer std_buffer_Buffer;
typedef struct std_map_Node__0 std_map_Node__0;
typedef struct std_map_Node__1 std_map_Node__1;
typedef struct std_map_Node__2 std_map_Node__2;
typedef struct std_map_Node__3 std_map_Node__3;
typedef struct std_map_Node__4 std_map_Node__4;
typedef struct std_map_Node__5 std_map_Node__5;
typedef struct std_map_Map__0 std_map_Map__0;
typedef struct std_map_Map__1 std_map_Map__1;
typedef struct std_map_Map__2 std_map_Map__2;
typedef struct std_map_Map__3 std_map_Map__3;
typedef struct std_map_Map__4 std_map_Map__4;
typedef struct std_map_Map__5 std_map_Map__5;
typedef struct std_map_Iterator__0 std_map_Iterator__0;
typedef struct std_map_Iterator__1 std_map_Iterator__1;
typedef struct std_map_Iterator__2 std_map_Iterator__2;
typedef struct std_map_Iterator__3 std_map_Iterator__3;
typedef struct std_map_Iterator__4 std_map_Iterator__4;
typedef struct std_map_Iterator__5 std_map_Iterator__5;
typedef struct std_map_KeyIterator__0 std_map_KeyIterator__0;
typedef struct std_map_KeyIterator__1 std_map_KeyIterator__1;
typedef struct std_map_KeyIterator__2 std_map_KeyIterator__2;
typedef struct std_map_KeyIterator__3 std_map_KeyIterator__3;
typedef struct std_map_KeyIterator__4 std_map_KeyIterator__4;
typedef struct std_map_KeyIterator__5 std_map_KeyIterator__5;
typedef struct std_map_ValueIterator__0 std_map_ValueIterator__0;
typedef struct std_map_ValueIterator__1 std_map_ValueIterator__1;
typedef struct std_map_ValueIterator__2 std_map_ValueIterator__2;
typedef struct std_map_ValueIterator__3 std_map_ValueIterator__3;
typedef struct std_map_ValueIterator__4 std_map_ValueIterator__4;
typedef struct std_map_ValueIterator__5 std_map_ValueIterator__5;
typedef struct std_span_Location std_span_Location;
typedef struct std_span_Span std_span_Span;
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
typedef struct std_vector_Iterator__0 std_vector_Iterator__0;
typedef struct std_vector_Iterator__1 std_vector_Iterator__1;
typedef struct std_vector_Iterator__3 std_vector_Iterator__3;
typedef struct std_vector_Iterator__4 std_vector_Iterator__4;
typedef struct std_vector_Iterator__5 std_vector_Iterator__5;
typedef struct std_vector_Iterator__6 std_vector_Iterator__6;
typedef struct std_vector_Iterator__7 std_vector_Iterator__7;
typedef struct std_vector_Iterator__8 std_vector_Iterator__8;
typedef struct std_vector_Iterator__9 std_vector_Iterator__9;
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
typedef struct std_json_Parser std_json_Parser;
typedef struct std_json_Lexer std_json_Lexer;
typedef struct std_json_Token std_json_Token;
typedef union std_value_ValueUnion std_value_ValueUnion;
typedef struct std_value_Value std_value_Value;
typedef struct docgen_DocGenerator docgen_DocGenerator;
typedef struct parser_Parser parser_Parser;
typedef struct passes_register_types_RegisterTypes passes_register_types_RegisterTypes;
typedef struct passes_generic_pass_GenericPass passes_generic_pass_GenericPass;
typedef struct passes_code_generator_CodeGenerator passes_code_generator_CodeGenerator;
typedef struct passes_typechecker_TypeChecker passes_typechecker_TypeChecker;
typedef struct passes_namespace_dump_NamespaceDump passes_namespace_dump_NamespaceDump;
typedef struct passes_reorder_structs_ReorderStructs passes_reorder_structs_ReorderStructs;
typedef union ast_scopes_SymbolUnion ast_scopes_SymbolUnion;
typedef struct ast_scopes_Symbol ast_scopes_Symbol;
typedef struct ast_scopes_Scope ast_scopes_Scope;
typedef struct ast_program_Namespace ast_program_Namespace;
typedef struct ast_program_Program ast_program_Program;
typedef struct ast_program_NSIterator ast_program_NSIterator;
typedef struct ast_nodes_Variable ast_nodes_Variable;
typedef struct ast_nodes_VarDeclaration ast_nodes_VarDeclaration;
typedef struct ast_nodes_TemplateInstance ast_nodes_TemplateInstance;
typedef struct ast_nodes_Structure ast_nodes_Structure;
typedef struct ast_nodes_Enum ast_nodes_Enum;
typedef struct ast_nodes_Function ast_nodes_Function;
typedef struct ast_nodes_Block ast_nodes_Block;
typedef struct ast_nodes_Identifier ast_nodes_Identifier;
typedef struct ast_nodes_Argument ast_nodes_Argument;
typedef struct ast_nodes_FuncCall ast_nodes_FuncCall;
typedef struct ast_nodes_ImportPartSingle ast_nodes_ImportPartSingle;
typedef union ast_nodes_ImportPartUnion ast_nodes_ImportPartUnion;
typedef struct ast_nodes_ImportPart ast_nodes_ImportPart;
typedef struct ast_nodes_Import ast_nodes_Import;
typedef struct ast_nodes_NumLiteral ast_nodes_NumLiteral;
typedef struct ast_nodes_Binary ast_nodes_Binary;
typedef struct ast_nodes_NSLookup ast_nodes_NSLookup;
typedef struct ast_nodes_Member ast_nodes_Member;
typedef struct ast_nodes_Assertion ast_nodes_Assertion;
typedef struct ast_nodes_IfStatement ast_nodes_IfStatement;
typedef struct ast_nodes_Loop ast_nodes_Loop;
typedef struct ast_nodes_Cast ast_nodes_Cast;
typedef struct ast_nodes_FormatString ast_nodes_FormatString;
typedef struct ast_nodes_MatchCase ast_nodes_MatchCase;
typedef struct ast_nodes_Match ast_nodes_Match;
typedef struct ast_nodes_Specialization ast_nodes_Specialization;
typedef union ast_nodes_ASTUnion ast_nodes_ASTUnion;
typedef struct ast_nodes_AST ast_nodes_AST;
typedef struct lexer_Lexer lexer_Lexer;
typedef struct tokens_Token tokens_Token;
typedef struct errors_Error errors_Error;
typedef struct types_FunctionType types_FunctionType;
typedef struct types_ArrayType types_ArrayType;
typedef struct types_UnresolvedTemplate types_UnresolvedTemplate;
typedef struct types_TypeUnion types_TypeUnion;
typedef struct types_Type types_Type;

/* Struct definitions */
struct std_compact_map_Item__0 {
  u32 hash;
  char *key;
  std_value_Value *value;
};

struct std_compact_map_Map__0 {
  std_vector_Vector__2 *items;
  i32 *indices;
  u32 capacity;
};

struct std_vector_Iterator__2 {
  std_vector_Vector__2 *vec;
  u32 index;
};

struct std_compact_map_KeyIterator__0 {
  std_vector_Iterator__2 iter;
};

struct std_compact_map_ValueIterator__0 {
  std_vector_Iterator__2 iter;
};

struct std_buffer_Buffer {
  u8 *data;
  u32 size;
  u32 capacity;
};

struct std_map_Node__0 {
  char *key;
  ast_nodes_Structure *value;
  std_map_Node__0 *next;
};

struct std_map_Node__1 {
  char *key;
  ast_scopes_Symbol *value;
  std_map_Node__1 *next;
};

struct std_map_Node__2 {
  char *key;
  ast_program_Namespace *value;
  std_map_Node__2 *next;
};

struct std_map_Node__3 {
  char *key;
  char *value;
  std_map_Node__3 *next;
};

struct std_map_Node__4 {
  char *key;
  ast_nodes_Function *value;
  std_map_Node__4 *next;
};

struct std_map_Node__5 {
  char *key;
  ast_nodes_MatchCase *value;
  std_map_Node__5 *next;
};

struct std_map_Map__0 {
  std_map_Node__0 **buckets;
  u32 num_items;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__1 {
  std_map_Node__1 **buckets;
  u32 num_items;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__2 {
  std_map_Node__2 **buckets;
  u32 num_items;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__3 {
  std_map_Node__3 **buckets;
  u32 num_items;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__4 {
  std_map_Node__4 **buckets;
  u32 num_items;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Map__5 {
  std_map_Node__5 **buckets;
  u32 num_items;
  u32 num_buckets;
  u32 num_collisions;
};

struct std_map_Iterator__0 {
  i32 idx;
  std_map_Node__0 *node;
  std_map_Map__0 *map;
};

struct std_map_Iterator__1 {
  i32 idx;
  std_map_Node__1 *node;
  std_map_Map__1 *map;
};

struct std_map_Iterator__2 {
  i32 idx;
  std_map_Node__2 *node;
  std_map_Map__2 *map;
};

struct std_map_Iterator__3 {
  i32 idx;
  std_map_Node__3 *node;
  std_map_Map__3 *map;
};

struct std_map_Iterator__4 {
  i32 idx;
  std_map_Node__4 *node;
  std_map_Map__4 *map;
};

struct std_map_Iterator__5 {
  i32 idx;
  std_map_Node__5 *node;
  std_map_Map__5 *map;
};

struct std_map_KeyIterator__0 {
  std_map_Iterator__0 map_iter;
};

struct std_map_KeyIterator__1 {
  std_map_Iterator__1 map_iter;
};

struct std_map_KeyIterator__2 {
  std_map_Iterator__2 map_iter;
};

struct std_map_KeyIterator__3 {
  std_map_Iterator__3 map_iter;
};

struct std_map_KeyIterator__4 {
  std_map_Iterator__4 map_iter;
};

struct std_map_KeyIterator__5 {
  std_map_Iterator__5 map_iter;
};

struct std_map_ValueIterator__0 {
  std_map_Iterator__0 map_iter;
};

struct std_map_ValueIterator__1 {
  std_map_Iterator__1 map_iter;
};

struct std_map_ValueIterator__2 {
  std_map_Iterator__2 map_iter;
};

struct std_map_ValueIterator__3 {
  std_map_Iterator__3 map_iter;
};

struct std_map_ValueIterator__4 {
  std_map_Iterator__4 map_iter;
};

struct std_map_ValueIterator__5 {
  std_map_Iterator__5 map_iter;
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

struct std_vector_Vector__0 {
  std_json_Token **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__1 {
  std_value_Value **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__2 {
  std_compact_map_Item__0 *data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__3 {
  types_Type **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__4 {
  ast_nodes_Variable **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__5 {
  ast_nodes_ImportPart **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__6 {
  tokens_Token **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__7 {
  char **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__8 {
  ast_scopes_Scope **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__9 {
  ast_program_Namespace **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__10 {
  ast_nodes_Function **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__11 {
  ast_nodes_Structure **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__12 {
  ast_nodes_AST **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__13 {
  ast_nodes_Enum **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__14 {
  errors_Error **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__15 {
  ast_nodes_TemplateInstance **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__16 {
  ast_nodes_Argument **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__17 {
  std_vector_Vector__5 **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__18 {
  ast_nodes_MatchCase **data;
  u32 size;
  u32 capacity;
};

struct std_vector_Vector__19 {
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

struct std_vector_Iterator__7 {
  std_vector_Vector__7 *vec;
  u32 index;
};

struct std_vector_Iterator__8 {
  std_vector_Vector__8 *vec;
  u32 index;
};

struct std_vector_Iterator__9 {
  std_vector_Vector__9 *vec;
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

struct std_json_Parser {
  std_vector_Vector__0 *tokens;
  u32 curr;
};

struct std_json_Lexer {
  char *source;
  u32 source_len;
  u32 i;
  std_span_Location loc;
  std_vector_Vector__0 *tokens;
};

struct std_json_Token {
  std_json_TokenType type;
  std_span_Span span;
  char *text;
};

union std_value_ValueUnion {
  bool as_bool;
  i64 as_num;
  std_buffer_Buffer as_str;
  std_vector_Vector__1 *as_list;
  std_compact_map_Map__0 *as_dict;
};

struct std_value_Value {
  std_value_ValueType type;
  std_value_ValueUnion u;
  std_span_Span span;
};

struct docgen_DocGenerator {
};

struct parser_Parser {
  std_vector_Vector__6 *tokens;
  u32 curr;
  ast_nodes_Function *curr_func;
  ast_program_Program *program;
  ast_program_Namespace *ns;
};

struct passes_register_types_RegisterTypes {
  passes_generic_pass_GenericPass *o;
};

struct passes_generic_pass_GenericPass {
  std_vector_Vector__8 *scope_stack;
  std_vector_Vector__9 *namespace_stack;
  ast_program_Program *program;
  types_Type *error_type;
};

struct passes_code_generator_CodeGenerator {
  passes_generic_pass_GenericPass *o;
  std_buffer_Buffer out;
  u32 indent;
  std_vector_Vector__7 *yield_vars;
};

struct passes_typechecker_TypeChecker {
  passes_generic_pass_GenericPass *o;
  std_vector_Vector__10 *unchecked_functions;
};

struct passes_namespace_dump_NamespaceDump {
  u32 indent;
};

struct passes_reorder_structs_ReorderStructs {
  passes_generic_pass_GenericPass *o;
  std_vector_Vector__11 *all_structs;
  std_map_Map__0 *done;
};

union ast_scopes_SymbolUnion {
  ast_nodes_Function *func;
  ast_nodes_Structure *struc;
  ast_nodes_Enum *enum_;
  ast_program_Namespace *ns;
  types_Type *type_def;
  ast_nodes_Variable *var;
};

struct ast_scopes_Symbol {
  char *name;
  char *display;
  char *out_name;
  std_span_Span span;
  ast_program_Namespace *ns;
  char *comment;
  ast_scopes_SymbolType type;
  ast_scopes_SymbolUnion u;
  bool is_extern;
};

struct ast_scopes_Scope {
  std_map_Map__1 *items;
  std_vector_Vector__12 *defers;
  u32 loop_count;
  bool can_yield;
  ast_nodes_Function *cur_func;
  ast_scopes_Scope *parent;
};

struct ast_program_Namespace {
  ast_program_Namespace *parent;
  std_vector_Vector__10 *functions;
  std_vector_Vector__11 *structs;
  std_vector_Vector__13 *enums;
  std_vector_Vector__12 *constants;
  std_vector_Vector__12 *variables;
  std_vector_Vector__12 *imports;
  std_map_Map__2 *namespaces;
  ast_scopes_Symbol *sym;
  ast_scopes_Scope *scope;
  char *path;
  std_span_Span span;
  bool is_a_file;
  bool always_add_to_scope;
  bool is_top_level;
};

struct ast_program_Program {
  ast_program_Namespace *global;
  ast_program_Namespace *project_root;
  std_vector_Vector__11 *ordered_structs;
  std_vector_Vector__14 *errors;
  u32 error_level;
  std_vector_Vector__7 *c_includes;
  std_vector_Vector__7 *c_flags;
  std_map_Map__3 *c_embeds;
  bool gen_debug_info;
  std_map_Map__3 *sources;
};

struct ast_program_NSIterator {
  std_vector_Vector__9 *stack;
  ast_program_Namespace *curr;
};

struct ast_nodes_Variable {
  ast_scopes_Symbol *sym;
  types_Type *type;
  ast_nodes_AST *default_value;
};

struct ast_nodes_VarDeclaration {
  ast_nodes_Variable *var;
  ast_nodes_AST *init;
};

struct ast_nodes_TemplateInstance {
  std_vector_Vector__3 *params;
  ast_scopes_Symbol *parent;
  ast_scopes_Symbol *resolved;
};

struct ast_nodes_Structure {
  ast_scopes_Symbol *sym;
  std_vector_Vector__4 *fields;
  types_Type *type;
  bool is_union;
  std_span_Span span;
  bool is_templated;
  std_vector_Vector__4 *template_params;
  std_vector_Vector__15 *template_instances;
};

struct ast_nodes_Enum {
  ast_scopes_Symbol *sym;
  std_span_Span span;
  std_vector_Vector__4 *fields;
  types_Type *type;
};

struct ast_nodes_Function {
  ast_scopes_Symbol *sym;
  std_vector_Vector__4 *params;
  types_Type *return_type;
  ast_nodes_AST *body;
  bool exits;
  std_span_Span span;
  ast_scopes_Scope *scope;
  types_Type *type;
  bool checked;
  bool is_method;
  bool is_static;
  types_Type *parent_type;
};

struct ast_nodes_Block {
  std_vector_Vector__12 *statements;
  ast_scopes_Scope *scope;
};

struct ast_nodes_Identifier {
  char *name;
  ast_nodes_Variable *var;
  bool is_function;
  ast_nodes_Function *func;
};

struct ast_nodes_Argument {
  ast_nodes_AST *expr;
  char *label;
  std_span_Span label_span;
};

struct ast_nodes_FuncCall {
  ast_nodes_AST *callee;
  std_vector_Vector__16 *args;
  ast_nodes_Function *func;
  bool is_constructor;
  bool is_function_pointer;
  bool added_method_arg;
};

struct ast_nodes_ImportPartSingle {
  char *name;
  char *alias;
  std_span_Span alias_span;
};

union ast_nodes_ImportPartUnion {
  ast_nodes_ImportPartSingle single;
  std_vector_Vector__17 *paths;
};

struct ast_nodes_ImportPart {
  ast_nodes_ImportPartType type;
  ast_nodes_ImportPartUnion u;
  std_span_Span span;
};

struct ast_nodes_Import {
  std_vector_Vector__5 *parts;
  ast_nodes_ImportType type;
  u32 parent_count;
};

struct ast_nodes_NumLiteral {
  char *text;
  types_Type *suffix;
};

struct ast_nodes_Binary {
  ast_nodes_AST *lhs;
  ast_nodes_AST *rhs;
};

struct ast_nodes_NSLookup {
  ast_nodes_AST *lhs;
  char *rhs_name;
  std_span_Span rhs_span;
};

struct ast_nodes_Member {
  ast_nodes_AST *lhs;
  char *rhs_name;
  std_span_Span rhs_span;
  bool is_pointer;
};

struct ast_nodes_Assertion {
  ast_nodes_AST *expr;
  ast_nodes_AST *msg;
};

struct ast_nodes_IfStatement {
  ast_nodes_AST *cond;
  ast_nodes_AST *body;
  ast_nodes_AST *els;
};

struct ast_nodes_Loop {
  ast_nodes_AST *init;
  ast_nodes_AST *cond;
  ast_nodes_AST *step;
  ast_nodes_AST *body;
};

struct ast_nodes_Cast {
  ast_nodes_AST *lhs;
  types_Type *to;
};

struct ast_nodes_FormatString {
  std_vector_Vector__7 *parts;
  std_vector_Vector__7 *specs;
  std_vector_Vector__12 *exprs;
};

struct ast_nodes_MatchCase {
  ast_nodes_AST *cond;
  ast_nodes_AST *body;
};

struct ast_nodes_Match {
  ast_nodes_AST *expr;
  std_vector_Vector__18 *cases;
  ast_nodes_AST *defolt;
  std_span_Span defolt_span;
};

struct ast_nodes_Specialization {
  ast_nodes_AST *base;
  std_vector_Vector__3 *template_args;
};

union ast_nodes_ASTUnion {
  ast_nodes_Assertion assertion;
  ast_nodes_Binary binary;
  ast_nodes_Block block;
  bool bool_literal;
  ast_nodes_FuncCall call;
  ast_nodes_Cast cast;
  ast_nodes_Identifier ident;
  ast_nodes_IfStatement if_stmt;
  ast_nodes_Import import_path;
  ast_nodes_NSLookup lookup;
  ast_nodes_Loop loop;
  ast_nodes_Member member;
  ast_nodes_NumLiteral num_literal;
  char *string_literal;
  char *char_literal;
  ast_nodes_AST *unary;
  ast_nodes_VarDeclaration var_decl;
  ast_nodes_FormatString fmt_str;
  types_Type *size_of_type;
  ast_nodes_Match match_stmt;
  ast_nodes_Specialization spec;
};

struct ast_nodes_AST {
  ast_nodes_ASTType type;
  std_span_Span span;
  ast_nodes_ASTUnion u;
  types_Type *etype;
  ast_scopes_Symbol *resolved_symbol;
  bool returns;
};

struct lexer_Lexer {
  char *source;
  u32 source_len;
  u32 i;
  std_span_Location loc;
  bool seen_newline;
  std_vector_Vector__6 *tokens;
  std_vector_Vector__14 *errors;
  bool in_comment;
  std_buffer_Buffer comment;
  std_span_Location comment_start;
};

struct tokens_Token {
  tokens_TokenType type;
  std_span_Span span;
  char *text;
  tokens_Token *suffix;
  bool seen_newline;
  char *comment;
  std_span_Location comment_loc;
};

struct errors_Error {
  errors_ErrorType type;
  char *msg1;
  std_span_Span span1;
  char *msg2;
  std_span_Span span2;
};

struct types_FunctionType {
  ast_nodes_Function *orig;
  types_Type *return_type;
  std_vector_Vector__4 *params;
};

struct types_ArrayType {
  types_Type *elem_type;
  ast_nodes_AST *size_expr;
};

struct types_UnresolvedTemplate {
  types_Type *base;
  std_vector_Vector__3 *args;
};

struct types_TypeUnion {
  types_Type *ptr;
  types_FunctionType *func;
  ast_nodes_Structure *struc;
  ast_nodes_Enum *enum_;
  ast_nodes_AST *unresolved;
  types_ArrayType *arr;
  types_UnresolvedTemplate *unresolved_spec;
};

struct types_Type {
  char *name;
  types_BaseType base;
  std_span_Span span;
  types_TypeUnion u;
  std_map_Map__4 *methods;
  ast_scopes_Symbol *sym;
  ast_nodes_TemplateInstance *template_instance;
};


char *exec_path = "./out";
char *c_path = NULL;
char *filename = NULL;
bool compile_c = true;
bool silent = false;
bool debug = false;
u32 error_level = ((u32)2);
char *docs_path = NULL;
/* function declarations */
FILE *FILE_open(char *path, char *mode);
i32 FILE_read(FILE *this, void *buf, u32 size);
i32 FILE_write(FILE *this, void *buf, u32 size);
char *FILE_slurp(FILE *this);
bool FILE_exists(char *path);
void FILE_puts(FILE *this, char *s);
void std_panic(char *msg) __attribute__((noreturn));
u32 str_to_u32(char *this);
bool str_eq(char *this, char *other);
u32 str_len(char *this);
char *str_substring(char *this, u32 start, u32 len);
void str_strip_trailing_whitespace(char *this);
void str_remove_last_n(char *this, u32 n);
void str_replace_with(char **this, char *other);
bool char_is_digit(char this);
bool char_is_alpha(char this);
bool char_is_alnum(char this);
bool char_is_print(char this);
i32 std_min(i32 a, i32 b);
i32 std_max(i32 a, i32 b);
u32 u32_min(u32 this, u32 other);
u32 u32_max(u32 this, u32 other);
std_value_Value *std_compact_map_Map__0_at(std_compact_map_Map__0 *this, char *key);
void std_compact_map_Map__0_free(std_compact_map_Map__0 *this);
u32 std_compact_map_Map__0_get_index(std_compact_map_Map__0 *this, char *key, u32 hash);
void std_compact_map_Map__0_insert(std_compact_map_Map__0 *this, char *key, std_value_Value *value);
void std_compact_map_Map__0_remove(std_compact_map_Map__0 *this, char *key);
std_value_Value *std_compact_map_Map__0_get(std_compact_map_Map__0 *this, char *key, std_value_Value *defolt);
std_compact_map_KeyIterator__0 std_compact_map_Map__0_iter_keys(std_compact_map_Map__0 *this);
char *std_compact_map_KeyIterator__0_cur(std_compact_map_KeyIterator__0 *this);
void std_compact_map_KeyIterator__0_next(std_compact_map_KeyIterator__0 *this);
bool std_compact_map_KeyIterator__0_has_value(std_compact_map_KeyIterator__0 *this);
bool std_compact_map_Map__0_is_empty(std_compact_map_Map__0 *this);
void std_compact_map_Map__0_resize(std_compact_map_Map__0 *this, u32 new_capacity);
std_compact_map_ValueIterator__0 std_compact_map_Map__0_iter_values(std_compact_map_Map__0 *this);
std_value_Value *std_compact_map_ValueIterator__0_cur(std_compact_map_ValueIterator__0 *this);
void std_compact_map_ValueIterator__0_next(std_compact_map_ValueIterator__0 *this);
bool std_compact_map_ValueIterator__0_has_value(std_compact_map_ValueIterator__0 *this);
std_compact_map_Map__0 *std_compact_map_Map__0_new(u32 capacity);
std_vector_Iterator__2 std_compact_map_Map__0_iter(std_compact_map_Map__0 *this);
std_buffer_Buffer std_buffer_Buffer_make(u32 capacity);
std_buffer_Buffer std_buffer_Buffer_from_str(char *s);
std_buffer_Buffer std_buffer_Buffer_from_sized_str(char *s, u32 size);
void std_buffer_Buffer_resize_if_necessary(std_buffer_Buffer *this, u32 new_size);
void std_buffer_Buffer_hex_dump(std_buffer_Buffer *this);
void std_buffer_Buffer_putb(std_buffer_Buffer *this, std_buffer_Buffer *buf);
void std_buffer_Buffer_putbf(std_buffer_Buffer *this, std_buffer_Buffer *buf);
void std_buffer_Buffer_puts(std_buffer_Buffer *this, char *s);
void std_buffer_Buffer_putc(std_buffer_Buffer *this, char c);
void std_buffer_Buffer_putsf(std_buffer_Buffer *this, char *s);
char *std_buffer_Buffer_str(std_buffer_Buffer *this);
char *std_buffer_Buffer_new_str(std_buffer_Buffer *this);
void std_buffer_Buffer_clear(std_buffer_Buffer *this);
void std_buffer_Buffer_free(std_buffer_Buffer *this);
void std_map_Node__0_free_list(std_map_Node__0 *this);
std_map_Node__0 *std_map_Node__0_new(char *key, ast_nodes_Structure *value, std_map_Node__0 *next);
void std_map_Map__0_push_keys(std_map_Map__0 *this, std_vector_Vector__7 *vec);
ast_nodes_Structure *std_map_Map__0_at(std_map_Map__0 *this, char *key);
void std_map_Map__0_free(std_map_Map__0 *this);
void std_map_Map__0_insert(std_map_Map__0 *this, char *key, ast_nodes_Structure *value);
ast_nodes_Structure *std_map_Map__0_get(std_map_Map__0 *this, char *key, ast_nodes_Structure *defolt);
void std_map_Map__0_extend(std_map_Map__0 *this, std_map_Map__0 *other);
void std_map_Map__0_remove(std_map_Map__0 *this, char *key);
std_map_KeyIterator__0 std_map_Map__0_iter_keys(std_map_Map__0 *this);
std_map_Node__0 *std_map_Iterator__0_cur(std_map_Iterator__0 *this);
void std_map_Iterator__0_next(std_map_Iterator__0 *this);
char *std_map_Iterator__0_key(std_map_Iterator__0 *this);
bool std_map_Iterator__0_has_value(std_map_Iterator__0 *this);
ast_nodes_Structure *std_map_Iterator__0_value(std_map_Iterator__0 *this);
std_map_Iterator__0 std_map_Iterator__0_make(std_map_Map__0 *map);
char *std_map_KeyIterator__0_cur(std_map_KeyIterator__0 *this);
void std_map_KeyIterator__0_next(std_map_KeyIterator__0 *this);
bool std_map_KeyIterator__0_has_value(std_map_KeyIterator__0 *this);
bool std_map_Map__0_is_empty(std_map_Map__0 *this);
bool std_map_Map__0_contains(std_map_Map__0 *this, char *key);
void std_map_Map__0_resize(std_map_Map__0 *this);
u32 std_map_Map__0_hash(std_map_Map__0 *this, char *key);
std_map_ValueIterator__0 std_map_Map__0_iter_values(std_map_Map__0 *this);
ast_nodes_Structure *std_map_ValueIterator__0_cur(std_map_ValueIterator__0 *this);
void std_map_ValueIterator__0_next(std_map_ValueIterator__0 *this);
bool std_map_ValueIterator__0_has_value(std_map_ValueIterator__0 *this);
std_map_Map__0 *std_map_Map__0_new(void);
std_map_Node__0 *std_map_Map__0_get_node(std_map_Map__0 *this, char *key);
std_map_Iterator__0 std_map_Map__0_iter(std_map_Map__0 *this);
void std_map_Node__1_free_list(std_map_Node__1 *this);
std_map_Node__1 *std_map_Node__1_new(char *key, ast_scopes_Symbol *value, std_map_Node__1 *next);
void std_map_Map__1_push_keys(std_map_Map__1 *this, std_vector_Vector__7 *vec);
ast_scopes_Symbol *std_map_Map__1_at(std_map_Map__1 *this, char *key);
void std_map_Map__1_free(std_map_Map__1 *this);
void std_map_Map__1_insert(std_map_Map__1 *this, char *key, ast_scopes_Symbol *value);
ast_scopes_Symbol *std_map_Map__1_get(std_map_Map__1 *this, char *key, ast_scopes_Symbol *defolt);
void std_map_Map__1_extend(std_map_Map__1 *this, std_map_Map__1 *other);
void std_map_Map__1_remove(std_map_Map__1 *this, char *key);
std_map_KeyIterator__1 std_map_Map__1_iter_keys(std_map_Map__1 *this);
std_map_Node__1 *std_map_Iterator__1_cur(std_map_Iterator__1 *this);
void std_map_Iterator__1_next(std_map_Iterator__1 *this);
char *std_map_Iterator__1_key(std_map_Iterator__1 *this);
bool std_map_Iterator__1_has_value(std_map_Iterator__1 *this);
ast_scopes_Symbol *std_map_Iterator__1_value(std_map_Iterator__1 *this);
std_map_Iterator__1 std_map_Iterator__1_make(std_map_Map__1 *map);
char *std_map_KeyIterator__1_cur(std_map_KeyIterator__1 *this);
void std_map_KeyIterator__1_next(std_map_KeyIterator__1 *this);
bool std_map_KeyIterator__1_has_value(std_map_KeyIterator__1 *this);
bool std_map_Map__1_is_empty(std_map_Map__1 *this);
bool std_map_Map__1_contains(std_map_Map__1 *this, char *key);
void std_map_Map__1_resize(std_map_Map__1 *this);
u32 std_map_Map__1_hash(std_map_Map__1 *this, char *key);
std_map_ValueIterator__1 std_map_Map__1_iter_values(std_map_Map__1 *this);
ast_scopes_Symbol *std_map_ValueIterator__1_cur(std_map_ValueIterator__1 *this);
void std_map_ValueIterator__1_next(std_map_ValueIterator__1 *this);
bool std_map_ValueIterator__1_has_value(std_map_ValueIterator__1 *this);
std_map_Map__1 *std_map_Map__1_new(void);
std_map_Node__1 *std_map_Map__1_get_node(std_map_Map__1 *this, char *key);
std_map_Iterator__1 std_map_Map__1_iter(std_map_Map__1 *this);
void std_map_Node__2_free_list(std_map_Node__2 *this);
std_map_Node__2 *std_map_Node__2_new(char *key, ast_program_Namespace *value, std_map_Node__2 *next);
void std_map_Map__2_push_keys(std_map_Map__2 *this, std_vector_Vector__7 *vec);
ast_program_Namespace *std_map_Map__2_at(std_map_Map__2 *this, char *key);
void std_map_Map__2_free(std_map_Map__2 *this);
void std_map_Map__2_insert(std_map_Map__2 *this, char *key, ast_program_Namespace *value);
ast_program_Namespace *std_map_Map__2_get(std_map_Map__2 *this, char *key, ast_program_Namespace *defolt);
void std_map_Map__2_extend(std_map_Map__2 *this, std_map_Map__2 *other);
void std_map_Map__2_remove(std_map_Map__2 *this, char *key);
std_map_KeyIterator__2 std_map_Map__2_iter_keys(std_map_Map__2 *this);
std_map_Node__2 *std_map_Iterator__2_cur(std_map_Iterator__2 *this);
void std_map_Iterator__2_next(std_map_Iterator__2 *this);
char *std_map_Iterator__2_key(std_map_Iterator__2 *this);
bool std_map_Iterator__2_has_value(std_map_Iterator__2 *this);
ast_program_Namespace *std_map_Iterator__2_value(std_map_Iterator__2 *this);
std_map_Iterator__2 std_map_Iterator__2_make(std_map_Map__2 *map);
char *std_map_KeyIterator__2_cur(std_map_KeyIterator__2 *this);
void std_map_KeyIterator__2_next(std_map_KeyIterator__2 *this);
bool std_map_KeyIterator__2_has_value(std_map_KeyIterator__2 *this);
bool std_map_Map__2_is_empty(std_map_Map__2 *this);
bool std_map_Map__2_contains(std_map_Map__2 *this, char *key);
void std_map_Map__2_resize(std_map_Map__2 *this);
u32 std_map_Map__2_hash(std_map_Map__2 *this, char *key);
std_map_ValueIterator__2 std_map_Map__2_iter_values(std_map_Map__2 *this);
ast_program_Namespace *std_map_ValueIterator__2_cur(std_map_ValueIterator__2 *this);
void std_map_ValueIterator__2_next(std_map_ValueIterator__2 *this);
bool std_map_ValueIterator__2_has_value(std_map_ValueIterator__2 *this);
std_map_Map__2 *std_map_Map__2_new(void);
std_map_Node__2 *std_map_Map__2_get_node(std_map_Map__2 *this, char *key);
std_map_Iterator__2 std_map_Map__2_iter(std_map_Map__2 *this);
void std_map_Node__3_free_list(std_map_Node__3 *this);
std_map_Node__3 *std_map_Node__3_new(char *key, char *value, std_map_Node__3 *next);
void std_map_Map__3_push_keys(std_map_Map__3 *this, std_vector_Vector__7 *vec);
char *std_map_Map__3_at(std_map_Map__3 *this, char *key);
void std_map_Map__3_free(std_map_Map__3 *this);
void std_map_Map__3_insert(std_map_Map__3 *this, char *key, char *value);
char *std_map_Map__3_get(std_map_Map__3 *this, char *key, char *defolt);
void std_map_Map__3_extend(std_map_Map__3 *this, std_map_Map__3 *other);
void std_map_Map__3_remove(std_map_Map__3 *this, char *key);
std_map_KeyIterator__3 std_map_Map__3_iter_keys(std_map_Map__3 *this);
std_map_Node__3 *std_map_Iterator__3_cur(std_map_Iterator__3 *this);
void std_map_Iterator__3_next(std_map_Iterator__3 *this);
char *std_map_Iterator__3_key(std_map_Iterator__3 *this);
bool std_map_Iterator__3_has_value(std_map_Iterator__3 *this);
char *std_map_Iterator__3_value(std_map_Iterator__3 *this);
std_map_Iterator__3 std_map_Iterator__3_make(std_map_Map__3 *map);
char *std_map_KeyIterator__3_cur(std_map_KeyIterator__3 *this);
void std_map_KeyIterator__3_next(std_map_KeyIterator__3 *this);
bool std_map_KeyIterator__3_has_value(std_map_KeyIterator__3 *this);
bool std_map_Map__3_is_empty(std_map_Map__3 *this);
bool std_map_Map__3_contains(std_map_Map__3 *this, char *key);
void std_map_Map__3_resize(std_map_Map__3 *this);
u32 std_map_Map__3_hash(std_map_Map__3 *this, char *key);
std_map_ValueIterator__3 std_map_Map__3_iter_values(std_map_Map__3 *this);
char *std_map_ValueIterator__3_cur(std_map_ValueIterator__3 *this);
void std_map_ValueIterator__3_next(std_map_ValueIterator__3 *this);
bool std_map_ValueIterator__3_has_value(std_map_ValueIterator__3 *this);
std_map_Map__3 *std_map_Map__3_new(void);
std_map_Node__3 *std_map_Map__3_get_node(std_map_Map__3 *this, char *key);
std_map_Iterator__3 std_map_Map__3_iter(std_map_Map__3 *this);
void std_map_Node__4_free_list(std_map_Node__4 *this);
std_map_Node__4 *std_map_Node__4_new(char *key, ast_nodes_Function *value, std_map_Node__4 *next);
void std_map_Map__4_push_keys(std_map_Map__4 *this, std_vector_Vector__7 *vec);
ast_nodes_Function *std_map_Map__4_at(std_map_Map__4 *this, char *key);
void std_map_Map__4_free(std_map_Map__4 *this);
void std_map_Map__4_insert(std_map_Map__4 *this, char *key, ast_nodes_Function *value);
ast_nodes_Function *std_map_Map__4_get(std_map_Map__4 *this, char *key, ast_nodes_Function *defolt);
void std_map_Map__4_extend(std_map_Map__4 *this, std_map_Map__4 *other);
void std_map_Map__4_remove(std_map_Map__4 *this, char *key);
std_map_KeyIterator__4 std_map_Map__4_iter_keys(std_map_Map__4 *this);
std_map_Node__4 *std_map_Iterator__4_cur(std_map_Iterator__4 *this);
void std_map_Iterator__4_next(std_map_Iterator__4 *this);
char *std_map_Iterator__4_key(std_map_Iterator__4 *this);
bool std_map_Iterator__4_has_value(std_map_Iterator__4 *this);
ast_nodes_Function *std_map_Iterator__4_value(std_map_Iterator__4 *this);
std_map_Iterator__4 std_map_Iterator__4_make(std_map_Map__4 *map);
char *std_map_KeyIterator__4_cur(std_map_KeyIterator__4 *this);
void std_map_KeyIterator__4_next(std_map_KeyIterator__4 *this);
bool std_map_KeyIterator__4_has_value(std_map_KeyIterator__4 *this);
bool std_map_Map__4_is_empty(std_map_Map__4 *this);
bool std_map_Map__4_contains(std_map_Map__4 *this, char *key);
void std_map_Map__4_resize(std_map_Map__4 *this);
u32 std_map_Map__4_hash(std_map_Map__4 *this, char *key);
std_map_ValueIterator__4 std_map_Map__4_iter_values(std_map_Map__4 *this);
ast_nodes_Function *std_map_ValueIterator__4_cur(std_map_ValueIterator__4 *this);
void std_map_ValueIterator__4_next(std_map_ValueIterator__4 *this);
bool std_map_ValueIterator__4_has_value(std_map_ValueIterator__4 *this);
std_map_Map__4 *std_map_Map__4_new(void);
std_map_Node__4 *std_map_Map__4_get_node(std_map_Map__4 *this, char *key);
std_map_Iterator__4 std_map_Map__4_iter(std_map_Map__4 *this);
void std_map_Node__5_free_list(std_map_Node__5 *this);
std_map_Node__5 *std_map_Node__5_new(char *key, ast_nodes_MatchCase *value, std_map_Node__5 *next);
void std_map_Map__5_push_keys(std_map_Map__5 *this, std_vector_Vector__7 *vec);
ast_nodes_MatchCase *std_map_Map__5_at(std_map_Map__5 *this, char *key);
void std_map_Map__5_free(std_map_Map__5 *this);
void std_map_Map__5_insert(std_map_Map__5 *this, char *key, ast_nodes_MatchCase *value);
ast_nodes_MatchCase *std_map_Map__5_get(std_map_Map__5 *this, char *key, ast_nodes_MatchCase *defolt);
void std_map_Map__5_extend(std_map_Map__5 *this, std_map_Map__5 *other);
void std_map_Map__5_remove(std_map_Map__5 *this, char *key);
std_map_KeyIterator__5 std_map_Map__5_iter_keys(std_map_Map__5 *this);
std_map_Node__5 *std_map_Iterator__5_cur(std_map_Iterator__5 *this);
void std_map_Iterator__5_next(std_map_Iterator__5 *this);
char *std_map_Iterator__5_key(std_map_Iterator__5 *this);
bool std_map_Iterator__5_has_value(std_map_Iterator__5 *this);
ast_nodes_MatchCase *std_map_Iterator__5_value(std_map_Iterator__5 *this);
std_map_Iterator__5 std_map_Iterator__5_make(std_map_Map__5 *map);
char *std_map_KeyIterator__5_cur(std_map_KeyIterator__5 *this);
void std_map_KeyIterator__5_next(std_map_KeyIterator__5 *this);
bool std_map_KeyIterator__5_has_value(std_map_KeyIterator__5 *this);
bool std_map_Map__5_is_empty(std_map_Map__5 *this);
bool std_map_Map__5_contains(std_map_Map__5 *this, char *key);
void std_map_Map__5_resize(std_map_Map__5 *this);
u32 std_map_Map__5_hash(std_map_Map__5 *this, char *key);
std_map_ValueIterator__5 std_map_Map__5_iter_values(std_map_Map__5 *this);
ast_nodes_MatchCase *std_map_ValueIterator__5_cur(std_map_ValueIterator__5 *this);
void std_map_ValueIterator__5_next(std_map_ValueIterator__5 *this);
bool std_map_ValueIterator__5_has_value(std_map_ValueIterator__5 *this);
std_map_Map__5 *std_map_Map__5_new(void);
std_map_Node__5 *std_map_Map__5_get_node(std_map_Map__5 *this, char *key);
std_map_Iterator__5 std_map_Map__5_iter(std_map_Map__5 *this);
std_span_Location std_span_Location_default(void);
char *std_span_Location_str(std_span_Location *this);
bool std_span_Location_is_before(std_span_Location *this, std_span_Location other);
char *std_span_Span_str(std_span_Span this);
std_span_Span std_span_Span_default(void);
std_span_Span std_span_Span_join(std_span_Span this, std_span_Span other);
bool std_span_Span_contains_loc(std_span_Span this, std_span_Location loc);
bool std_span_Span_starts_right_after(std_span_Span this, std_span_Span other);
bool i32_eq(i32 this, i32 other);
bool u32_eq(u32 this, u32 other);
u32 str_hash(char *this);
u32 i32_hash(i32 this);
u32 u32_hash(u32 this);
std_json_Token *std_vector_Vector__0_at(std_vector_Vector__0 *this, u32 i);
void std_vector_Vector__0_free(std_vector_Vector__0 *this);
void std_vector_Vector__0_extend(std_vector_Vector__0 *this, std_vector_Vector__0 *other);
bool std_vector_Vector__0_is_empty(std_vector_Vector__0 *this);
void std_vector_Vector__0_push_front(std_vector_Vector__0 *this, std_json_Token *value);
void std_vector_Vector__0_resize(std_vector_Vector__0 *this, u32 new_capacity);
std_vector_Vector__0 *std_vector_Vector__0_new(u32 capacity);
std_json_Token *std_vector_Vector__0_pop(std_vector_Vector__0 *this);
void std_vector_Vector__0_push(std_vector_Vector__0 *this, std_json_Token *value);
void std_vector_Vector__0_clear(std_vector_Vector__0 *this);
std_json_Token *std_vector_Vector__0_back(std_vector_Vector__0 *this);
std_vector_Iterator__0 std_vector_Vector__0_iter(std_vector_Vector__0 *this);
std_json_Token *std_vector_Iterator__0_cur(std_vector_Iterator__0 *this);
void std_vector_Iterator__0_next(std_vector_Iterator__0 *this);
bool std_vector_Iterator__0_has_value(std_vector_Iterator__0 *this);
std_vector_Iterator__0 std_vector_Iterator__0_make(std_vector_Vector__0 *vec);
std_value_Value *std_vector_Vector__1_at(std_vector_Vector__1 *this, u32 i);
void std_vector_Vector__1_free(std_vector_Vector__1 *this);
void std_vector_Vector__1_extend(std_vector_Vector__1 *this, std_vector_Vector__1 *other);
bool std_vector_Vector__1_is_empty(std_vector_Vector__1 *this);
void std_vector_Vector__1_push_front(std_vector_Vector__1 *this, std_value_Value *value);
void std_vector_Vector__1_resize(std_vector_Vector__1 *this, u32 new_capacity);
std_vector_Vector__1 *std_vector_Vector__1_new(u32 capacity);
std_value_Value *std_vector_Vector__1_pop(std_vector_Vector__1 *this);
void std_vector_Vector__1_push(std_vector_Vector__1 *this, std_value_Value *value);
void std_vector_Vector__1_clear(std_vector_Vector__1 *this);
std_value_Value *std_vector_Vector__1_back(std_vector_Vector__1 *this);
std_vector_Iterator__1 std_vector_Vector__1_iter(std_vector_Vector__1 *this);
std_value_Value *std_vector_Iterator__1_cur(std_vector_Iterator__1 *this);
void std_vector_Iterator__1_next(std_vector_Iterator__1 *this);
bool std_vector_Iterator__1_has_value(std_vector_Iterator__1 *this);
std_vector_Iterator__1 std_vector_Iterator__1_make(std_vector_Vector__1 *vec);
std_compact_map_Item__0 std_vector_Vector__2_at(std_vector_Vector__2 *this, u32 i);
void std_vector_Vector__2_free(std_vector_Vector__2 *this);
void std_vector_Vector__2_extend(std_vector_Vector__2 *this, std_vector_Vector__2 *other);
bool std_vector_Vector__2_is_empty(std_vector_Vector__2 *this);
void std_vector_Vector__2_push_front(std_vector_Vector__2 *this, std_compact_map_Item__0 value);
void std_vector_Vector__2_resize(std_vector_Vector__2 *this, u32 new_capacity);
std_vector_Vector__2 *std_vector_Vector__2_new(u32 capacity);
std_compact_map_Item__0 std_vector_Vector__2_pop(std_vector_Vector__2 *this);
void std_vector_Vector__2_push(std_vector_Vector__2 *this, std_compact_map_Item__0 value);
void std_vector_Vector__2_clear(std_vector_Vector__2 *this);
std_compact_map_Item__0 std_vector_Vector__2_back(std_vector_Vector__2 *this);
std_vector_Iterator__2 std_vector_Vector__2_iter(std_vector_Vector__2 *this);
std_compact_map_Item__0 std_vector_Iterator__2_cur(std_vector_Iterator__2 *this);
void std_vector_Iterator__2_next(std_vector_Iterator__2 *this);
bool std_vector_Iterator__2_has_value(std_vector_Iterator__2 *this);
std_vector_Iterator__2 std_vector_Iterator__2_make(std_vector_Vector__2 *vec);
types_Type *std_vector_Vector__3_at(std_vector_Vector__3 *this, u32 i);
void std_vector_Vector__3_free(std_vector_Vector__3 *this);
void std_vector_Vector__3_extend(std_vector_Vector__3 *this, std_vector_Vector__3 *other);
bool std_vector_Vector__3_is_empty(std_vector_Vector__3 *this);
void std_vector_Vector__3_push_front(std_vector_Vector__3 *this, types_Type *value);
void std_vector_Vector__3_resize(std_vector_Vector__3 *this, u32 new_capacity);
std_vector_Vector__3 *std_vector_Vector__3_new(u32 capacity);
types_Type *std_vector_Vector__3_pop(std_vector_Vector__3 *this);
void std_vector_Vector__3_push(std_vector_Vector__3 *this, types_Type *value);
void std_vector_Vector__3_clear(std_vector_Vector__3 *this);
types_Type *std_vector_Vector__3_back(std_vector_Vector__3 *this);
std_vector_Iterator__3 std_vector_Vector__3_iter(std_vector_Vector__3 *this);
types_Type *std_vector_Iterator__3_cur(std_vector_Iterator__3 *this);
void std_vector_Iterator__3_next(std_vector_Iterator__3 *this);
bool std_vector_Iterator__3_has_value(std_vector_Iterator__3 *this);
std_vector_Iterator__3 std_vector_Iterator__3_make(std_vector_Vector__3 *vec);
ast_nodes_Variable *std_vector_Vector__4_at(std_vector_Vector__4 *this, u32 i);
void std_vector_Vector__4_free(std_vector_Vector__4 *this);
void std_vector_Vector__4_extend(std_vector_Vector__4 *this, std_vector_Vector__4 *other);
bool std_vector_Vector__4_is_empty(std_vector_Vector__4 *this);
void std_vector_Vector__4_push_front(std_vector_Vector__4 *this, ast_nodes_Variable *value);
void std_vector_Vector__4_resize(std_vector_Vector__4 *this, u32 new_capacity);
std_vector_Vector__4 *std_vector_Vector__4_new(u32 capacity);
ast_nodes_Variable *std_vector_Vector__4_pop(std_vector_Vector__4 *this);
void std_vector_Vector__4_push(std_vector_Vector__4 *this, ast_nodes_Variable *value);
void std_vector_Vector__4_clear(std_vector_Vector__4 *this);
ast_nodes_Variable *std_vector_Vector__4_back(std_vector_Vector__4 *this);
std_vector_Iterator__4 std_vector_Vector__4_iter(std_vector_Vector__4 *this);
ast_nodes_Variable *std_vector_Iterator__4_cur(std_vector_Iterator__4 *this);
void std_vector_Iterator__4_next(std_vector_Iterator__4 *this);
bool std_vector_Iterator__4_has_value(std_vector_Iterator__4 *this);
std_vector_Iterator__4 std_vector_Iterator__4_make(std_vector_Vector__4 *vec);
ast_nodes_ImportPart *std_vector_Vector__5_at(std_vector_Vector__5 *this, u32 i);
void std_vector_Vector__5_free(std_vector_Vector__5 *this);
void std_vector_Vector__5_extend(std_vector_Vector__5 *this, std_vector_Vector__5 *other);
bool std_vector_Vector__5_is_empty(std_vector_Vector__5 *this);
void std_vector_Vector__5_push_front(std_vector_Vector__5 *this, ast_nodes_ImportPart *value);
void std_vector_Vector__5_resize(std_vector_Vector__5 *this, u32 new_capacity);
std_vector_Vector__5 *std_vector_Vector__5_new(u32 capacity);
ast_nodes_ImportPart *std_vector_Vector__5_pop(std_vector_Vector__5 *this);
void std_vector_Vector__5_push(std_vector_Vector__5 *this, ast_nodes_ImportPart *value);
void std_vector_Vector__5_clear(std_vector_Vector__5 *this);
ast_nodes_ImportPart *std_vector_Vector__5_back(std_vector_Vector__5 *this);
std_vector_Iterator__5 std_vector_Vector__5_iter(std_vector_Vector__5 *this);
ast_nodes_ImportPart *std_vector_Iterator__5_cur(std_vector_Iterator__5 *this);
void std_vector_Iterator__5_next(std_vector_Iterator__5 *this);
bool std_vector_Iterator__5_has_value(std_vector_Iterator__5 *this);
std_vector_Iterator__5 std_vector_Iterator__5_make(std_vector_Vector__5 *vec);
tokens_Token *std_vector_Vector__6_at(std_vector_Vector__6 *this, u32 i);
void std_vector_Vector__6_free(std_vector_Vector__6 *this);
void std_vector_Vector__6_extend(std_vector_Vector__6 *this, std_vector_Vector__6 *other);
bool std_vector_Vector__6_is_empty(std_vector_Vector__6 *this);
void std_vector_Vector__6_push_front(std_vector_Vector__6 *this, tokens_Token *value);
void std_vector_Vector__6_resize(std_vector_Vector__6 *this, u32 new_capacity);
std_vector_Vector__6 *std_vector_Vector__6_new(u32 capacity);
tokens_Token *std_vector_Vector__6_pop(std_vector_Vector__6 *this);
void std_vector_Vector__6_push(std_vector_Vector__6 *this, tokens_Token *value);
void std_vector_Vector__6_clear(std_vector_Vector__6 *this);
tokens_Token *std_vector_Vector__6_back(std_vector_Vector__6 *this);
std_vector_Iterator__6 std_vector_Vector__6_iter(std_vector_Vector__6 *this);
tokens_Token *std_vector_Iterator__6_cur(std_vector_Iterator__6 *this);
void std_vector_Iterator__6_next(std_vector_Iterator__6 *this);
bool std_vector_Iterator__6_has_value(std_vector_Iterator__6 *this);
std_vector_Iterator__6 std_vector_Iterator__6_make(std_vector_Vector__6 *vec);
char *std_vector_Vector__7_at(std_vector_Vector__7 *this, u32 i);
void std_vector_Vector__7_free(std_vector_Vector__7 *this);
void std_vector_Vector__7_extend(std_vector_Vector__7 *this, std_vector_Vector__7 *other);
bool std_vector_Vector__7_is_empty(std_vector_Vector__7 *this);
void std_vector_Vector__7_push_front(std_vector_Vector__7 *this, char *value);
void std_vector_Vector__7_resize(std_vector_Vector__7 *this, u32 new_capacity);
std_vector_Vector__7 *std_vector_Vector__7_new(u32 capacity);
char *std_vector_Vector__7_pop(std_vector_Vector__7 *this);
void std_vector_Vector__7_push(std_vector_Vector__7 *this, char *value);
void std_vector_Vector__7_clear(std_vector_Vector__7 *this);
char *std_vector_Vector__7_back(std_vector_Vector__7 *this);
std_vector_Iterator__7 std_vector_Vector__7_iter(std_vector_Vector__7 *this);
char *std_vector_Iterator__7_cur(std_vector_Iterator__7 *this);
void std_vector_Iterator__7_next(std_vector_Iterator__7 *this);
bool std_vector_Iterator__7_has_value(std_vector_Iterator__7 *this);
std_vector_Iterator__7 std_vector_Iterator__7_make(std_vector_Vector__7 *vec);
ast_scopes_Scope *std_vector_Vector__8_at(std_vector_Vector__8 *this, u32 i);
void std_vector_Vector__8_free(std_vector_Vector__8 *this);
void std_vector_Vector__8_extend(std_vector_Vector__8 *this, std_vector_Vector__8 *other);
bool std_vector_Vector__8_is_empty(std_vector_Vector__8 *this);
void std_vector_Vector__8_push_front(std_vector_Vector__8 *this, ast_scopes_Scope *value);
void std_vector_Vector__8_resize(std_vector_Vector__8 *this, u32 new_capacity);
std_vector_Vector__8 *std_vector_Vector__8_new(u32 capacity);
ast_scopes_Scope *std_vector_Vector__8_pop(std_vector_Vector__8 *this);
void std_vector_Vector__8_push(std_vector_Vector__8 *this, ast_scopes_Scope *value);
void std_vector_Vector__8_clear(std_vector_Vector__8 *this);
ast_scopes_Scope *std_vector_Vector__8_back(std_vector_Vector__8 *this);
std_vector_Iterator__8 std_vector_Vector__8_iter(std_vector_Vector__8 *this);
ast_scopes_Scope *std_vector_Iterator__8_cur(std_vector_Iterator__8 *this);
void std_vector_Iterator__8_next(std_vector_Iterator__8 *this);
bool std_vector_Iterator__8_has_value(std_vector_Iterator__8 *this);
std_vector_Iterator__8 std_vector_Iterator__8_make(std_vector_Vector__8 *vec);
ast_program_Namespace *std_vector_Vector__9_at(std_vector_Vector__9 *this, u32 i);
void std_vector_Vector__9_free(std_vector_Vector__9 *this);
void std_vector_Vector__9_extend(std_vector_Vector__9 *this, std_vector_Vector__9 *other);
bool std_vector_Vector__9_is_empty(std_vector_Vector__9 *this);
void std_vector_Vector__9_push_front(std_vector_Vector__9 *this, ast_program_Namespace *value);
void std_vector_Vector__9_resize(std_vector_Vector__9 *this, u32 new_capacity);
std_vector_Vector__9 *std_vector_Vector__9_new(u32 capacity);
ast_program_Namespace *std_vector_Vector__9_pop(std_vector_Vector__9 *this);
void std_vector_Vector__9_push(std_vector_Vector__9 *this, ast_program_Namespace *value);
void std_vector_Vector__9_clear(std_vector_Vector__9 *this);
ast_program_Namespace *std_vector_Vector__9_back(std_vector_Vector__9 *this);
std_vector_Iterator__9 std_vector_Vector__9_iter(std_vector_Vector__9 *this);
ast_program_Namespace *std_vector_Iterator__9_cur(std_vector_Iterator__9 *this);
void std_vector_Iterator__9_next(std_vector_Iterator__9 *this);
bool std_vector_Iterator__9_has_value(std_vector_Iterator__9 *this);
std_vector_Iterator__9 std_vector_Iterator__9_make(std_vector_Vector__9 *vec);
ast_nodes_Function *std_vector_Vector__10_at(std_vector_Vector__10 *this, u32 i);
void std_vector_Vector__10_free(std_vector_Vector__10 *this);
void std_vector_Vector__10_extend(std_vector_Vector__10 *this, std_vector_Vector__10 *other);
bool std_vector_Vector__10_is_empty(std_vector_Vector__10 *this);
void std_vector_Vector__10_push_front(std_vector_Vector__10 *this, ast_nodes_Function *value);
void std_vector_Vector__10_resize(std_vector_Vector__10 *this, u32 new_capacity);
std_vector_Vector__10 *std_vector_Vector__10_new(u32 capacity);
ast_nodes_Function *std_vector_Vector__10_pop(std_vector_Vector__10 *this);
void std_vector_Vector__10_push(std_vector_Vector__10 *this, ast_nodes_Function *value);
void std_vector_Vector__10_clear(std_vector_Vector__10 *this);
ast_nodes_Function *std_vector_Vector__10_back(std_vector_Vector__10 *this);
std_vector_Iterator__10 std_vector_Vector__10_iter(std_vector_Vector__10 *this);
ast_nodes_Function *std_vector_Iterator__10_cur(std_vector_Iterator__10 *this);
void std_vector_Iterator__10_next(std_vector_Iterator__10 *this);
bool std_vector_Iterator__10_has_value(std_vector_Iterator__10 *this);
std_vector_Iterator__10 std_vector_Iterator__10_make(std_vector_Vector__10 *vec);
ast_nodes_Structure *std_vector_Vector__11_at(std_vector_Vector__11 *this, u32 i);
void std_vector_Vector__11_free(std_vector_Vector__11 *this);
void std_vector_Vector__11_extend(std_vector_Vector__11 *this, std_vector_Vector__11 *other);
bool std_vector_Vector__11_is_empty(std_vector_Vector__11 *this);
void std_vector_Vector__11_push_front(std_vector_Vector__11 *this, ast_nodes_Structure *value);
void std_vector_Vector__11_resize(std_vector_Vector__11 *this, u32 new_capacity);
std_vector_Vector__11 *std_vector_Vector__11_new(u32 capacity);
ast_nodes_Structure *std_vector_Vector__11_pop(std_vector_Vector__11 *this);
void std_vector_Vector__11_push(std_vector_Vector__11 *this, ast_nodes_Structure *value);
void std_vector_Vector__11_clear(std_vector_Vector__11 *this);
ast_nodes_Structure *std_vector_Vector__11_back(std_vector_Vector__11 *this);
std_vector_Iterator__11 std_vector_Vector__11_iter(std_vector_Vector__11 *this);
ast_nodes_Structure *std_vector_Iterator__11_cur(std_vector_Iterator__11 *this);
void std_vector_Iterator__11_next(std_vector_Iterator__11 *this);
bool std_vector_Iterator__11_has_value(std_vector_Iterator__11 *this);
std_vector_Iterator__11 std_vector_Iterator__11_make(std_vector_Vector__11 *vec);
ast_nodes_AST *std_vector_Vector__12_at(std_vector_Vector__12 *this, u32 i);
void std_vector_Vector__12_free(std_vector_Vector__12 *this);
void std_vector_Vector__12_extend(std_vector_Vector__12 *this, std_vector_Vector__12 *other);
bool std_vector_Vector__12_is_empty(std_vector_Vector__12 *this);
void std_vector_Vector__12_push_front(std_vector_Vector__12 *this, ast_nodes_AST *value);
void std_vector_Vector__12_resize(std_vector_Vector__12 *this, u32 new_capacity);
std_vector_Vector__12 *std_vector_Vector__12_new(u32 capacity);
ast_nodes_AST *std_vector_Vector__12_pop(std_vector_Vector__12 *this);
void std_vector_Vector__12_push(std_vector_Vector__12 *this, ast_nodes_AST *value);
void std_vector_Vector__12_clear(std_vector_Vector__12 *this);
ast_nodes_AST *std_vector_Vector__12_back(std_vector_Vector__12 *this);
std_vector_Iterator__12 std_vector_Vector__12_iter(std_vector_Vector__12 *this);
ast_nodes_AST *std_vector_Iterator__12_cur(std_vector_Iterator__12 *this);
void std_vector_Iterator__12_next(std_vector_Iterator__12 *this);
bool std_vector_Iterator__12_has_value(std_vector_Iterator__12 *this);
std_vector_Iterator__12 std_vector_Iterator__12_make(std_vector_Vector__12 *vec);
ast_nodes_Enum *std_vector_Vector__13_at(std_vector_Vector__13 *this, u32 i);
void std_vector_Vector__13_free(std_vector_Vector__13 *this);
void std_vector_Vector__13_extend(std_vector_Vector__13 *this, std_vector_Vector__13 *other);
bool std_vector_Vector__13_is_empty(std_vector_Vector__13 *this);
void std_vector_Vector__13_push_front(std_vector_Vector__13 *this, ast_nodes_Enum *value);
void std_vector_Vector__13_resize(std_vector_Vector__13 *this, u32 new_capacity);
std_vector_Vector__13 *std_vector_Vector__13_new(u32 capacity);
ast_nodes_Enum *std_vector_Vector__13_pop(std_vector_Vector__13 *this);
void std_vector_Vector__13_push(std_vector_Vector__13 *this, ast_nodes_Enum *value);
void std_vector_Vector__13_clear(std_vector_Vector__13 *this);
ast_nodes_Enum *std_vector_Vector__13_back(std_vector_Vector__13 *this);
std_vector_Iterator__13 std_vector_Vector__13_iter(std_vector_Vector__13 *this);
ast_nodes_Enum *std_vector_Iterator__13_cur(std_vector_Iterator__13 *this);
void std_vector_Iterator__13_next(std_vector_Iterator__13 *this);
bool std_vector_Iterator__13_has_value(std_vector_Iterator__13 *this);
std_vector_Iterator__13 std_vector_Iterator__13_make(std_vector_Vector__13 *vec);
errors_Error *std_vector_Vector__14_at(std_vector_Vector__14 *this, u32 i);
void std_vector_Vector__14_free(std_vector_Vector__14 *this);
void std_vector_Vector__14_extend(std_vector_Vector__14 *this, std_vector_Vector__14 *other);
bool std_vector_Vector__14_is_empty(std_vector_Vector__14 *this);
void std_vector_Vector__14_push_front(std_vector_Vector__14 *this, errors_Error *value);
void std_vector_Vector__14_resize(std_vector_Vector__14 *this, u32 new_capacity);
std_vector_Vector__14 *std_vector_Vector__14_new(u32 capacity);
errors_Error *std_vector_Vector__14_pop(std_vector_Vector__14 *this);
void std_vector_Vector__14_push(std_vector_Vector__14 *this, errors_Error *value);
void std_vector_Vector__14_clear(std_vector_Vector__14 *this);
errors_Error *std_vector_Vector__14_back(std_vector_Vector__14 *this);
std_vector_Iterator__14 std_vector_Vector__14_iter(std_vector_Vector__14 *this);
errors_Error *std_vector_Iterator__14_cur(std_vector_Iterator__14 *this);
void std_vector_Iterator__14_next(std_vector_Iterator__14 *this);
bool std_vector_Iterator__14_has_value(std_vector_Iterator__14 *this);
std_vector_Iterator__14 std_vector_Iterator__14_make(std_vector_Vector__14 *vec);
ast_nodes_TemplateInstance *std_vector_Vector__15_at(std_vector_Vector__15 *this, u32 i);
void std_vector_Vector__15_free(std_vector_Vector__15 *this);
void std_vector_Vector__15_extend(std_vector_Vector__15 *this, std_vector_Vector__15 *other);
bool std_vector_Vector__15_is_empty(std_vector_Vector__15 *this);
void std_vector_Vector__15_push_front(std_vector_Vector__15 *this, ast_nodes_TemplateInstance *value);
void std_vector_Vector__15_resize(std_vector_Vector__15 *this, u32 new_capacity);
std_vector_Vector__15 *std_vector_Vector__15_new(u32 capacity);
ast_nodes_TemplateInstance *std_vector_Vector__15_pop(std_vector_Vector__15 *this);
void std_vector_Vector__15_push(std_vector_Vector__15 *this, ast_nodes_TemplateInstance *value);
void std_vector_Vector__15_clear(std_vector_Vector__15 *this);
ast_nodes_TemplateInstance *std_vector_Vector__15_back(std_vector_Vector__15 *this);
std_vector_Iterator__15 std_vector_Vector__15_iter(std_vector_Vector__15 *this);
ast_nodes_TemplateInstance *std_vector_Iterator__15_cur(std_vector_Iterator__15 *this);
void std_vector_Iterator__15_next(std_vector_Iterator__15 *this);
bool std_vector_Iterator__15_has_value(std_vector_Iterator__15 *this);
std_vector_Iterator__15 std_vector_Iterator__15_make(std_vector_Vector__15 *vec);
ast_nodes_Argument *std_vector_Vector__16_at(std_vector_Vector__16 *this, u32 i);
void std_vector_Vector__16_free(std_vector_Vector__16 *this);
void std_vector_Vector__16_extend(std_vector_Vector__16 *this, std_vector_Vector__16 *other);
bool std_vector_Vector__16_is_empty(std_vector_Vector__16 *this);
void std_vector_Vector__16_push_front(std_vector_Vector__16 *this, ast_nodes_Argument *value);
void std_vector_Vector__16_resize(std_vector_Vector__16 *this, u32 new_capacity);
std_vector_Vector__16 *std_vector_Vector__16_new(u32 capacity);
ast_nodes_Argument *std_vector_Vector__16_pop(std_vector_Vector__16 *this);
void std_vector_Vector__16_push(std_vector_Vector__16 *this, ast_nodes_Argument *value);
void std_vector_Vector__16_clear(std_vector_Vector__16 *this);
ast_nodes_Argument *std_vector_Vector__16_back(std_vector_Vector__16 *this);
std_vector_Iterator__16 std_vector_Vector__16_iter(std_vector_Vector__16 *this);
ast_nodes_Argument *std_vector_Iterator__16_cur(std_vector_Iterator__16 *this);
void std_vector_Iterator__16_next(std_vector_Iterator__16 *this);
bool std_vector_Iterator__16_has_value(std_vector_Iterator__16 *this);
std_vector_Iterator__16 std_vector_Iterator__16_make(std_vector_Vector__16 *vec);
std_vector_Vector__5 *std_vector_Vector__17_at(std_vector_Vector__17 *this, u32 i);
void std_vector_Vector__17_free(std_vector_Vector__17 *this);
void std_vector_Vector__17_extend(std_vector_Vector__17 *this, std_vector_Vector__17 *other);
bool std_vector_Vector__17_is_empty(std_vector_Vector__17 *this);
void std_vector_Vector__17_push_front(std_vector_Vector__17 *this, std_vector_Vector__5 *value);
void std_vector_Vector__17_resize(std_vector_Vector__17 *this, u32 new_capacity);
std_vector_Vector__17 *std_vector_Vector__17_new(u32 capacity);
std_vector_Vector__5 *std_vector_Vector__17_pop(std_vector_Vector__17 *this);
void std_vector_Vector__17_push(std_vector_Vector__17 *this, std_vector_Vector__5 *value);
void std_vector_Vector__17_clear(std_vector_Vector__17 *this);
std_vector_Vector__5 *std_vector_Vector__17_back(std_vector_Vector__17 *this);
std_vector_Iterator__17 std_vector_Vector__17_iter(std_vector_Vector__17 *this);
std_vector_Vector__5 *std_vector_Iterator__17_cur(std_vector_Iterator__17 *this);
void std_vector_Iterator__17_next(std_vector_Iterator__17 *this);
bool std_vector_Iterator__17_has_value(std_vector_Iterator__17 *this);
std_vector_Iterator__17 std_vector_Iterator__17_make(std_vector_Vector__17 *vec);
ast_nodes_MatchCase *std_vector_Vector__18_at(std_vector_Vector__18 *this, u32 i);
void std_vector_Vector__18_free(std_vector_Vector__18 *this);
void std_vector_Vector__18_extend(std_vector_Vector__18 *this, std_vector_Vector__18 *other);
bool std_vector_Vector__18_is_empty(std_vector_Vector__18 *this);
void std_vector_Vector__18_push_front(std_vector_Vector__18 *this, ast_nodes_MatchCase *value);
void std_vector_Vector__18_resize(std_vector_Vector__18 *this, u32 new_capacity);
std_vector_Vector__18 *std_vector_Vector__18_new(u32 capacity);
ast_nodes_MatchCase *std_vector_Vector__18_pop(std_vector_Vector__18 *this);
void std_vector_Vector__18_push(std_vector_Vector__18 *this, ast_nodes_MatchCase *value);
void std_vector_Vector__18_clear(std_vector_Vector__18 *this);
ast_nodes_MatchCase *std_vector_Vector__18_back(std_vector_Vector__18 *this);
std_vector_Iterator__18 std_vector_Vector__18_iter(std_vector_Vector__18 *this);
ast_nodes_MatchCase *std_vector_Iterator__18_cur(std_vector_Iterator__18 *this);
void std_vector_Iterator__18_next(std_vector_Iterator__18 *this);
bool std_vector_Iterator__18_has_value(std_vector_Iterator__18 *this);
std_vector_Iterator__18 std_vector_Iterator__18_make(std_vector_Vector__18 *vec);
u32 std_vector_Vector__19_at(std_vector_Vector__19 *this, u32 i);
void std_vector_Vector__19_free(std_vector_Vector__19 *this);
void std_vector_Vector__19_extend(std_vector_Vector__19 *this, std_vector_Vector__19 *other);
bool std_vector_Vector__19_is_empty(std_vector_Vector__19 *this);
void std_vector_Vector__19_push_front(std_vector_Vector__19 *this, u32 value);
void std_vector_Vector__19_resize(std_vector_Vector__19 *this, u32 new_capacity);
std_vector_Vector__19 *std_vector_Vector__19_new(u32 capacity);
u32 std_vector_Vector__19_pop(std_vector_Vector__19 *this);
void std_vector_Vector__19_push(std_vector_Vector__19 *this, u32 value);
void std_vector_Vector__19_clear(std_vector_Vector__19 *this);
u32 std_vector_Vector__19_back(std_vector_Vector__19 *this);
std_vector_Iterator__19 std_vector_Vector__19_iter(std_vector_Vector__19 *this);
u32 std_vector_Iterator__19_cur(std_vector_Iterator__19 *this);
void std_vector_Iterator__19_next(std_vector_Iterator__19 *this);
bool std_vector_Iterator__19_has_value(std_vector_Iterator__19 *this);
std_vector_Iterator__19 std_vector_Iterator__19_make(std_vector_Vector__19 *vec);
std_json_Parser std_json_Parser_make(std_vector_Vector__0 *tokens);
std_json_Token *std_json_Parser_token(std_json_Parser *this);
std_json_Token *std_json_Parser_consume(std_json_Parser *this, std_json_TokenType type);
std_value_Value *std_json_Parser_parse_object(std_json_Parser *this);
std_value_Value *std_json_Parser_parse_array(std_json_Parser *this);
std_value_Value *std_json_Parser_parse_value(std_json_Parser *this);
std_value_Value *std_json_Parser_parse(std_json_Parser *this);
std_value_Value *std_json_parse(char *source, char *filename);
std_value_Value *std_json_parse_from_file(char *filename);
void std_json_serialize_into(std_value_Value *val, std_buffer_Buffer *sb);
std_buffer_Buffer std_json_serialize(std_value_Value *val);
void std_json_write_to_file(std_value_Value *val, char *filename);
std_json_Lexer std_json_Lexer_make(char *source, char *filename);
void std_json_Lexer_push(std_json_Lexer *this, std_json_Token *token);
void std_json_Lexer_push_type(std_json_Lexer *this, std_json_TokenType type, u32 len);
char std_json_Lexer_cur(std_json_Lexer *this);
void std_json_Lexer_inc(std_json_Lexer *this);
char std_json_Lexer_peek(std_json_Lexer *this, i32 offset);
void std_json_Lexer_error(std_json_Lexer *this, char *msg);
void std_json_Lexer_lex_string_literal(std_json_Lexer *this);
void std_json_Lexer_lex_numeric_literal(std_json_Lexer *this);
std_vector_Vector__0 *std_json_Lexer_lex(std_json_Lexer *this);
std_json_Token *std_json_Token_new(std_json_TokenType type, std_span_Span span, char *text);
std_json_Token *std_json_Token_from_type(std_json_TokenType type, std_span_Span span);
std_json_Token *std_json_Token_from_ident(char *text, std_span_Span span);
char *std_json_Token_str(std_json_Token *this);
std_json_TokenType std_json_TokenType_from_text(char *text);
char *std_json_TokenType_str(std_json_TokenType this);
char *std_value_ValueType_str(std_value_ValueType this);
std_value_Value *std_value_Value_new(std_value_ValueType type);
std_value_Value *std_value_Value_new_str_buf(std_buffer_Buffer buf);
std_value_Value *std_value_Value_new_str(char *s);
std_value_Value *std_value_Value_new_bool(bool bul);
std_value_Value *std_value_Value_new_number(i64 num);
std_value_Value *std_value_Value_new_list(std_vector_Vector__1 *vec);
std_value_Value *std_value_Value_new_dict(std_compact_map_Map__0 *map);
void std_value_Value_ensure(std_value_Value *this, std_value_ValueType type);
bool std_value_Value_is(std_value_Value this, std_value_ValueType type);
std_value_Value *std_value_Value_at(std_value_Value *this, u32 idx);
void std_value_Value_set(std_value_Value *this, u32 idx, std_value_Value *value);
void std_value_Value_push(std_value_Value *this, std_value_Value *value);
std_value_Value *std_value_Value_get(std_value_Value *this, char *key, std_value_Value *defolt);
void std_value_Value_insert(std_value_Value *this, char *key, std_value_Value *value);
bool std_value_Value_as_bool(std_value_Value *this);
i64 std_value_Value_as_num(std_value_Value *this);
std_buffer_Buffer std_value_Value_as_str(std_value_Value *this);
std_vector_Vector__1 *std_value_Value_as_list(std_value_Value *this);
std_compact_map_Map__0 *std_value_Value_as_dict(std_value_Value *this);
char *std_value_Value_dbg(std_value_Value *this);
void std_value_Value_free(std_value_Value *this);
std_value_Value *docgen_DocGenerator_gen_enum(docgen_DocGenerator *this, ast_nodes_Enum *enum_);
char *docgen_DocGenerator_gen_templated_type(docgen_DocGenerator *this, types_Type *base, std_vector_Vector__3 *args);
char *docgen_DocGenerator_gen_typename_str(docgen_DocGenerator *this, types_Type *type);
std_value_Value *docgen_DocGenerator_gen_typename(docgen_DocGenerator *this, types_Type *type);
std_value_Value *docgen_DocGenerator_gen_methods(docgen_DocGenerator *this, types_Type *type);
std_value_Value *docgen_DocGenerator_gen_function(docgen_DocGenerator *this, ast_nodes_Function *func);
std_value_Value *docgen_DocGenerator_gen_struct(docgen_DocGenerator *this, ast_nodes_Structure *struc);
std_value_Value *docgen_DocGenerator_gen_ns(docgen_DocGenerator *this, ast_program_Namespace *ns);
std_value_Value *docgen_DocGenerator_gen_builtin(docgen_DocGenerator *this, types_Type *type);
std_value_Value *docgen_DocGenerator_gen_builtins(docgen_DocGenerator *this, ast_program_Program *program);
void docgen_generate_doc_json(ast_program_Program *program, char *json_path);
parser_Parser parser_Parser_make(ast_program_Program *program, ast_program_Namespace *ns);
tokens_Token *parser_Parser_peek(parser_Parser *this, i32 off);
errors_Error *parser_Parser_error_msg(parser_Parser *this, char *msg);
errors_Error *parser_Parser_error(parser_Parser *this, errors_Error *err);
void parser_Parser_unhandled_type(parser_Parser *this, char *func);
tokens_Token *parser_Parser_token(parser_Parser *this);
bool parser_Parser_token_is(parser_Parser *this, tokens_TokenType type);
bool parser_Parser_consume_if(parser_Parser *this, tokens_TokenType type);
void parser_Parser_consume_newline_or(parser_Parser *this, tokens_TokenType type);
tokens_Token *parser_Parser_consume(parser_Parser *this, tokens_TokenType type);
void parser_Parser_consume_end_of_statement(parser_Parser *this);
types_Type *parser_Parser_parse_type(parser_Parser *this);
ast_nodes_AST *parser_Parser_parse_scoped_identifier(parser_Parser *this);
ast_nodes_AST *parser_Parser_parse_format_string(parser_Parser *this);
ast_nodes_AST *parser_Parser_parse_match(parser_Parser *this);
types_Type *parser_Parser_parse_literal_suffix_type(parser_Parser *this, tokens_Token *suffix);
ast_nodes_AST *parser_Parser_parse_call(parser_Parser *this, ast_nodes_AST *callee);
ast_nodes_AST *parser_Parser_parse_global_value(parser_Parser *this, bool is_const);
ast_nodes_AST *parser_Parser_parse_atom(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_postfix(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_prefix(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_cast(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_term(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_additive(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_shift(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_bw_and(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_bw_xor(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_bw_or(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_relational(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_logical_and(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_logical_or(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_expression(parser_Parser *this, tokens_TokenType end_type);
ast_nodes_AST *parser_Parser_parse_if(parser_Parser *this);
ast_nodes_AST *parser_Parser_parse_for_each(parser_Parser *this, std_span_Span start_span);
ast_nodes_AST *parser_Parser_parse_for(parser_Parser *this);
ast_nodes_AST *parser_Parser_parse_statement(parser_Parser *this);
ast_nodes_AST *parser_Parser_parse_block(parser_Parser *this);
std_vector_Vector__4 *parser_Parser_parse_template_params(parser_Parser *this, std_span_Span *out_span);
ast_nodes_Function *parser_Parser_parse_function(parser_Parser *this);
void parser_Parser_parse_extern_into_symbol(parser_Parser *this, ast_scopes_Symbol *sym);
std_vector_Vector__5 *parser_Parser_parse_import_path(parser_Parser *this);
ast_nodes_AST *parser_Parser_parse_import(parser_Parser *this);
ast_nodes_Structure *parser_Parser_parse_struct(parser_Parser *this);
ast_nodes_Enum *parser_Parser_parse_enum(parser_Parser *this);
void parser_Parser_parse_namespace_until(parser_Parser *this, tokens_TokenType end_type);
void parser_Parser_parse_compiler_option(parser_Parser *this);
bool parser_Parser_load_import_path_from_base(parser_Parser *this, std_vector_Vector__5 *parts, ast_program_Namespace *base);
bool parser_Parser_load_import_path(parser_Parser *this, ast_nodes_AST *import_stmt);
void parser_Parser_load_file(parser_Parser *this, char *filename);
void parser_couldnt_find_stdlib(void);
void parser_Parser_find_and_import_stdlib(parser_Parser *this);
void parser_Parser_parse_toplevel(ast_program_Program *program, char *filename);
u32 utils_edit_distance(char *str1, char *str2);
char *utils_find_word_suggestion(char *s, std_vector_Vector__7 *options);
bool utils_directory_exists(char *path);
void passes_register_types_RegisterTypes_register_struct(passes_register_types_RegisterTypes *this, ast_program_Namespace *ns, ast_nodes_Structure *struc);
void passes_register_types_RegisterTypes_register_enum(passes_register_types_RegisterTypes *this, ast_program_Namespace *ns, ast_nodes_Enum *enum_);
void passes_register_types_RegisterTypes_add_dbg_method_for_enum(passes_register_types_RegisterTypes *this, ast_nodes_Enum *enum_);
void passes_register_types_RegisterTypes_register_namespace(passes_register_types_RegisterTypes *this, ast_program_Namespace *ns);
void passes_register_types_RegisterTypes_register_base_type(passes_register_types_RegisterTypes *this, types_BaseType base);
void passes_register_types_RegisterTypes_register_alias(passes_register_types_RegisterTypes *this, char *name, types_Type *orig);
void passes_register_types_RegisterTypes_register_builtin_types(passes_register_types_RegisterTypes *this);
void passes_register_types_RegisterTypes_run(ast_program_Program *program);
passes_generic_pass_GenericPass *passes_generic_pass_GenericPass_new(ast_program_Program *program);
void passes_generic_pass_GenericPass_push_scope(passes_generic_pass_GenericPass *this, ast_scopes_Scope *scope);
void passes_generic_pass_GenericPass_push_namespace(passes_generic_pass_GenericPass *this, ast_program_Namespace *ns);
ast_scopes_Scope *passes_generic_pass_GenericPass_pop_scope(passes_generic_pass_GenericPass *this);
ast_program_Namespace *passes_generic_pass_GenericPass_pop_namespace(passes_generic_pass_GenericPass *this);
ast_scopes_Scope *passes_generic_pass_GenericPass_scope(passes_generic_pass_GenericPass *this);
ast_program_Namespace *passes_generic_pass_GenericPass_ns(passes_generic_pass_GenericPass *this);
errors_Error *passes_generic_pass_GenericPass_error(passes_generic_pass_GenericPass *this, errors_Error *err);
void passes_generic_pass_GenericPass_insert_into_scope_checked(passes_generic_pass_GenericPass *this, ast_scopes_Symbol *item, char *name);
ast_scopes_Symbol *passes_generic_pass_GenericPass_lookup_in_symbol(passes_generic_pass_GenericPass *this, ast_scopes_Symbol *sym, char *name, std_span_Span span, bool error);
void passes_generic_pass_GenericPass_import_all_from_namespace(passes_generic_pass_GenericPass *this, ast_program_Namespace *ns);
void passes_generic_pass_GenericPass_import_all_from_symbol(passes_generic_pass_GenericPass *this, ast_scopes_Symbol *sym);
void passes_mod_run_typecheck_passes(ast_program_Program *program);
char *passes_mod_run_codegen_passes(ast_program_Program *program);
void passes_code_generator_CodeGenerator_gen_indent(passes_code_generator_CodeGenerator *this);
void str_replace(char **this, char *other);
errors_Error *passes_code_generator_CodeGenerator_error(passes_code_generator_CodeGenerator *this, errors_Error *err);
ast_scopes_Scope *passes_code_generator_CodeGenerator_scope(passes_code_generator_CodeGenerator *this);
void passes_code_generator_CodeGenerator_gen_debug_info(passes_code_generator_CodeGenerator *this, std_span_Span span, bool force);
char *passes_code_generator_CodeGenerator_get_op(passes_code_generator_CodeGenerator *this, ast_nodes_ASTType type);
void passes_code_generator_CodeGenerator_gen_internal_print(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_format_string_part(passes_code_generator_CodeGenerator *this, char *part);
void passes_code_generator_CodeGenerator_gen_format_string_variadic(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, bool newline_after);
void passes_code_generator_CodeGenerator_gen_format_string(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_yield_expression(passes_code_generator_CodeGenerator *this, ast_nodes_AST *expr);
void passes_code_generator_CodeGenerator_gen_constant(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_constants(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns);
void passes_code_generator_CodeGenerator_gen_global_variables(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns);
void passes_code_generator_CodeGenerator_gen_control_body(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, ast_nodes_AST *body);
void passes_code_generator_CodeGenerator_gen_in_yield_context(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_constructor(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, ast_nodes_Structure *struc);
void passes_code_generator_CodeGenerator_gen_expression(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_var_declaration(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_match_case_body(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, ast_nodes_AST *body);
void passes_code_generator_CodeGenerator_gen_match_string(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_match(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_defers_upto(passes_code_generator_CodeGenerator *this, ast_scopes_Scope *end_scope);
void passes_code_generator_CodeGenerator_gen_statement(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node);
void passes_code_generator_CodeGenerator_gen_block(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, bool with_braces);
char *passes_code_generator_CodeGenerator_helper_gen_type(passes_code_generator_CodeGenerator *this, types_Type *top, types_Type *cur, char *acc, bool is_func_def);
char *passes_code_generator_CodeGenerator_get_type_name_string(passes_code_generator_CodeGenerator *this, types_Type *type, char *name, bool is_func_def);
void passes_code_generator_CodeGenerator_gen_type_and_name(passes_code_generator_CodeGenerator *this, types_Type *type, char *name);
void passes_code_generator_CodeGenerator_gen_type(passes_code_generator_CodeGenerator *this, types_Type *type);
void passes_code_generator_CodeGenerator_gen_function(passes_code_generator_CodeGenerator *this, ast_nodes_Function *func);
void passes_code_generator_CodeGenerator_gen_function_decl(passes_code_generator_CodeGenerator *this, ast_nodes_Function *func);
void passes_code_generator_CodeGenerator_gen_functions(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns);
void passes_code_generator_CodeGenerator_gen_function_decls(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns);
void passes_code_generator_CodeGenerator_gen_enum_types(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns);
void passes_code_generator_CodeGenerator_gen_enum_dbg_method(passes_code_generator_CodeGenerator *this, ast_nodes_Enum *enum_);
void passes_code_generator_CodeGenerator_gen_enum(passes_code_generator_CodeGenerator *this, ast_nodes_Enum *enum_);
void passes_code_generator_CodeGenerator_gen_struct(passes_code_generator_CodeGenerator *this, ast_nodes_Structure *struc);
void passes_code_generator_CodeGenerator_gen_structs_and_typedefs(passes_code_generator_CodeGenerator *this);
char *passes_code_generator_CodeGenerator_generate(passes_code_generator_CodeGenerator *this);
passes_code_generator_CodeGenerator passes_code_generator_CodeGenerator_make(ast_program_Program *program);
char *passes_code_generator_CodeGenerator_run(ast_program_Program *program);
errors_Error *passes_typechecker_TypeChecker_error(passes_typechecker_TypeChecker *this, errors_Error *err);
ast_scopes_Scope *passes_typechecker_TypeChecker_scope(passes_typechecker_TypeChecker *this);
types_Type *passes_typechecker_TypeChecker_get_base_type(passes_typechecker_TypeChecker *this, types_BaseType base, std_span_Span span);
types_Type *passes_typechecker_TypeChecker_get_type_by_name(passes_typechecker_TypeChecker *this, char *name, std_span_Span span);
types_Type *passes_typechecker_TypeChecker_resolve_type(passes_typechecker_TypeChecker *this, types_Type *old, bool allow_incomplete, bool error, bool resolve_templates);
void passes_typechecker_TypeChecker_resolve_templated_struct_methods(passes_typechecker_TypeChecker *this, ast_nodes_Structure *old, ast_nodes_Structure *cur);
ast_scopes_Symbol *passes_typechecker_TypeChecker_resolve_templated_struct(passes_typechecker_TypeChecker *this, ast_nodes_Structure *struc, ast_nodes_AST *node);
ast_scopes_Symbol *passes_typechecker_TypeChecker_resolve_scoped_identifier(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool error, types_Type *hint, bool resolve_templates);
void passes_typechecker_TypeChecker_check_block(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_expr, types_Type *hint);
void passes_typechecker_TypeChecker_check_method_call(passes_typechecker_TypeChecker *this, ast_nodes_Function *method, ast_nodes_AST *node);
types_Type *passes_typechecker_TypeChecker_check_internal_print(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
types_Type *passes_typechecker_TypeChecker_check_constructor(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
void passes_typechecker_TypeChecker_check_call_args(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, std_vector_Vector__4 *params);
types_Type *passes_typechecker_TypeChecker_check_call(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
types_Type *passes_typechecker_TypeChecker_check_pointer_arith(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *_lhs, types_Type *_rhs);
types_Type *passes_typechecker_TypeChecker_check_binary_op(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *lhs, types_Type *rhs);
types_Type *passes_typechecker_TypeChecker_check_format_string(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
types_Type *passes_typechecker_TypeChecker_check_member(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_being_called);
types_Type *passes_typechecker_TypeChecker_check_expression(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *hint);
types_Type *passes_typechecker_TypeChecker_check_expression_helper(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *hint);
types_Type *passes_typechecker_TypeChecker_call_dbg_on_enum_value(passes_typechecker_TypeChecker *this, ast_nodes_AST **node_ptr);
void passes_typechecker_TypeChecker_check_match_for_enum(passes_typechecker_TypeChecker *this, ast_nodes_Enum *enum_, ast_nodes_AST *node, bool is_expr, types_Type *hint);
void passes_typechecker_TypeChecker_check_match(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_expr, types_Type *hint);
void passes_typechecker_TypeChecker_check_if(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_expr, types_Type *hint);
void passes_typechecker_TypeChecker_check_expression_statement(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, ast_nodes_AST *body, bool is_expr, types_Type *hint);
void passes_typechecker_TypeChecker_check_while(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
void passes_typechecker_TypeChecker_check_for(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
void passes_typechecker_TypeChecker_check_statement(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
void passes_typechecker_TypeChecker_check_function(passes_typechecker_TypeChecker *this, ast_nodes_Function *func);
void passes_typechecker_TypeChecker_handle_namespace_imports(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns);
void passes_typechecker_TypeChecker_check_global_variable(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
void passes_typechecker_TypeChecker_check_namespace(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns);
char *passes_typechecker_TypeChecker_resolve_doc_links(passes_typechecker_TypeChecker *this, char *doc, std_span_Span obj_span);
types_Type *passes_typechecker_TypeChecker_check_const_expression(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *hint);
void passes_typechecker_TypeChecker_handle_import_path_base(passes_typechecker_TypeChecker *this, std_vector_Vector__5 *parts, ast_scopes_Symbol *base, char *alias, i32 start_idx);
void passes_typechecker_TypeChecker_handle_import_statement(passes_typechecker_TypeChecker *this, ast_nodes_AST *node);
void passes_typechecker_TypeChecker_pre_check_function(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns, ast_nodes_Function *func);
void passes_typechecker_TypeChecker_loosely_resolve_templated_struct(passes_typechecker_TypeChecker *this, ast_nodes_Structure *struc);
void passes_typechecker_TypeChecker_resolve_struct(passes_typechecker_TypeChecker *this, ast_nodes_Structure *struc);
void passes_typechecker_TypeChecker_check_function_declaration(passes_typechecker_TypeChecker *this, ast_nodes_Function *func);
void passes_typechecker_TypeChecker_check_post_import(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns);
void passes_typechecker_TypeChecker_pre_check_globals(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_const);
void passes_typechecker_TypeChecker_pre_check_constants(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns);
void passes_typechecker_TypeChecker_pre_check_namespace(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns);
void passes_typechecker_TypeChecker_run(ast_program_Program *program);
void passes_namespace_dump_NamespaceDump_print_indent(passes_namespace_dump_NamespaceDump *this);
void passes_namespace_dump_NamespaceDump_print_namespace(passes_namespace_dump_NamespaceDump *this, ast_program_Namespace *ns);
void passes_namespace_dump_NamespaceDump_run(ast_program_Program *program);
passes_reorder_structs_ReorderStructs *passes_reorder_structs_ReorderStructs_new(ast_program_Program *program);
void passes_reorder_structs_ReorderStructs_free(passes_reorder_structs_ReorderStructs *this);
void passes_reorder_structs_ReorderStructs_collect_all_structs(passes_reorder_structs_ReorderStructs *this, ast_program_Namespace *ns);
void passes_reorder_structs_ReorderStructs_dfs(passes_reorder_structs_ReorderStructs *this, ast_nodes_Structure *struc);
void passes_reorder_structs_ReorderStructs_reorder_structs(passes_reorder_structs_ReorderStructs *this);
void passes_reorder_structs_ReorderStructs_run(ast_program_Program *program);
ast_scopes_Symbol *ast_scopes_Symbol_new(ast_scopes_SymbolType type, ast_program_Namespace *ns, char *name, char *display, char *out_name, std_span_Span span);
char *ast_scopes_Symbol_join_display(char *a, char *b);
char *ast_scopes_Symbol_join_out_name(char *a, char *b);
ast_scopes_Symbol *ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType type, ast_program_Namespace *ns, ast_scopes_Symbol *parent, char *name, std_span_Span span);
void ast_scopes_Symbol_update_parent(ast_scopes_Symbol *this, ast_scopes_Symbol *parent);
ast_scopes_Symbol *ast_scopes_Symbol_from_local_variable(char *name, ast_nodes_Variable *var, std_span_Span span);
ast_scopes_Symbol *ast_scopes_Symbol_remove_alias(ast_scopes_Symbol *this);
ast_scopes_Scope *ast_scopes_Scope_new(ast_scopes_Scope *parent);
ast_scopes_Symbol *ast_scopes_Scope_lookup_recursive(ast_scopes_Scope *this, char *name);
ast_scopes_Symbol *ast_scopes_Scope_lookup_local(ast_scopes_Scope *this, char *name);
void ast_scopes_Scope_insert(ast_scopes_Scope *this, char *name, ast_scopes_Symbol *symbol);
ast_program_Namespace *ast_program_Namespace_new(ast_program_Namespace *parent, char *path);
ast_scopes_Symbol *ast_program_Namespace_find_importable_symbol(ast_program_Namespace *this, char *name);
ast_program_Program *ast_program_Program_new(void);
void ast_program_Program_exit_with_errors(ast_program_Program *this);
char *ast_program_Program_get_source_text(ast_program_Program *this, std_span_Span span);
errors_Error *ast_program_Program_error(ast_program_Program *this, errors_Error *err);
ast_nodes_Function *ast_program_Program_get_function_deep_copy(ast_program_Program *this, ast_nodes_Function *old, ast_program_Namespace *ns);
ast_nodes_Structure *ast_program_Program_get_struct_deep_copy(ast_program_Program *this, ast_nodes_Structure *old, ast_program_Namespace *ns);
types_Type *ast_program_Program_get_base_type(ast_program_Program *this, types_BaseType base, std_span_Span span);
types_Type *ast_program_Program_get_type_by_name(ast_program_Program *this, char *name, std_span_Span span);
ast_program_NSIterator ast_program_Program_iter_namespaces(ast_program_Program *this);
bool ast_program_NSIterator_has_value(ast_program_NSIterator *this);
void ast_program_NSIterator_next(ast_program_NSIterator *this);
ast_program_Namespace *ast_program_NSIterator_cur(ast_program_NSIterator *this);
ast_nodes_ASTType ast_nodes_ASTType_from_token(tokens_TokenType type);
ast_nodes_Variable *ast_nodes_Variable_new(types_Type *type);
ast_nodes_TemplateInstance *ast_nodes_TemplateInstance_new(std_vector_Vector__3 *params, ast_scopes_Symbol *parent, ast_scopes_Symbol *resolved);
ast_nodes_Structure *ast_nodes_Structure_new(void);
ast_nodes_Variable *ast_nodes_Structure_get_field(ast_nodes_Structure *this, char *name);
ast_scopes_Symbol *ast_nodes_Structure_find_template_instance(ast_nodes_Structure *this, std_vector_Vector__3 *template_args);
ast_nodes_Enum *ast_nodes_Enum_new(void);
ast_nodes_Variable *ast_nodes_Enum_get_field(ast_nodes_Enum *this, char *name);
ast_nodes_Function *ast_nodes_Function_new(void);
ast_nodes_Argument *ast_nodes_Argument_new(char *label, std_span_Span label_span, ast_nodes_AST *expr);
ast_nodes_ImportPart *ast_nodes_ImportPart_new(ast_nodes_ImportPartType type, std_span_Span span);
ast_nodes_MatchCase *ast_nodes_MatchCase_new(ast_nodes_AST *cond, ast_nodes_AST *body);
ast_nodes_AST *ast_nodes_AST_new(ast_nodes_ASTType type, std_span_Span span);
ast_nodes_AST *ast_nodes_AST_new_unop(ast_nodes_ASTType type, std_span_Span span, ast_nodes_AST *expr);
ast_nodes_AST *ast_nodes_AST_new_binop(ast_nodes_ASTType type, ast_nodes_AST *lhs, ast_nodes_AST *rhs);
bool ast_nodes_AST_is_identifier(ast_nodes_AST *this);
ast_scopes_Symbol *ast_nodes_AST_symbol(ast_nodes_AST *this);
bool ast_nodes_AST_is_lvalue(ast_nodes_AST *this);
bool lexer_is_hex_digit(char c);
lexer_Lexer lexer_Lexer_make(char *source, char *filename);
void lexer_Lexer_push(lexer_Lexer *this, tokens_Token *token);
void lexer_Lexer_push_type(lexer_Lexer *this, tokens_TokenType type, u32 len);
char lexer_Lexer_cur(lexer_Lexer *this);
void lexer_Lexer_inc(lexer_Lexer *this);
char lexer_Lexer_peek(lexer_Lexer *this, i32 offset);
void lexer_Lexer_lex_char_literal(lexer_Lexer *this);
void lexer_Lexer_lex_string_literal(lexer_Lexer *this, bool has_seen_f);
tokens_Token *lexer_Lexer_lex_int_literal_different_base(lexer_Lexer *this);
tokens_Token *lexer_Lexer_lex_numeric_literal_helper(lexer_Lexer *this);
void lexer_Lexer_lex_numeric_literal(lexer_Lexer *this);
void lexer_Lexer_lex_comment(lexer_Lexer *this);
std_vector_Vector__6 *lexer_Lexer_lex(lexer_Lexer *this);
tokens_Token *tokens_Token_new(tokens_TokenType type, std_span_Span span, char *text);
tokens_Token *tokens_Token_from_type(tokens_TokenType type, std_span_Span span);
tokens_Token *tokens_Token_from_ident(char *text, std_span_Span span);
char *tokens_Token_str(tokens_Token *this);
bool tokens_Token_is_word(tokens_Token this);
tokens_TokenType tokens_TokenType_from_text(char *text);
char *tokens_TokenType_str(tokens_TokenType this);
char *errors_ErrorType_str(errors_ErrorType this);
char *errors_MessageType_to_color(errors_MessageType this);
char *errors_MessageType_str(errors_MessageType this);
void errors_display_line(void);
void errors_display_message(errors_MessageType type, std_span_Span span, char *msg);
void errors_display_message_span(errors_MessageType type, std_span_Span span, char *msg);
void errors_Error_display(errors_Error *this);
void errors_Error_panic(errors_Error *this) __attribute__((noreturn));
errors_Error *errors_Error_new(std_span_Span span, char *msg);
errors_Error *errors_Error_new_note(std_span_Span span, char *msg, char *note);
errors_Error *errors_Error_new_hint(std_span_Span span, char *msg, std_span_Span span2, char *hint);
void errors_display_error_messages(std_vector_Vector__14 *errors, u32 detail_level);
char *types_BaseType_str(types_BaseType this);
types_FunctionType *types_FunctionType_from_func(ast_nodes_Function *func);
types_FunctionType *types_FunctionType_from_args(std_vector_Vector__4 *params, types_Type *return_type);
types_ArrayType *types_ArrayType_new(types_Type *elem_type, ast_nodes_AST *size_expr);
types_UnresolvedTemplate *types_UnresolvedTemplate_new(types_Type *base, std_vector_Vector__3 *args);
types_Type *types_Type_new_resolved(types_BaseType base, std_span_Span span);
types_Type *types_Type_new_unresolved(char *name, std_span_Span span);
types_Type *types_Type_new_unresolved_base(types_BaseType base, std_span_Span span);
bool types_Type_is_integer(types_Type *this);
bool types_Type_is_float(types_Type *this);
bool types_Type_is_numeric(types_Type *this);
bool types_Type_is_numeric_or_char(types_Type *this);
bool types_Type_can_have_methods(types_Type *this);
bool types_Type_eq(types_Type *this, types_Type *other);
bool types_Type_is_str(types_Type *this);
types_Type *types_Type_unaliased(types_Type *this);
types_Type *types_Type_decay_array(types_Type *this);
char *types_Type_str(types_Type *this);
void usage(i32 code);
void save_and_compile_code(ast_program_Program *program, char *code);
i32 main(i32 argc, char **argv);
/* function implementations */
FILE *FILE_open(char *path, char *mode) {
  FILE *file = fopen(path, mode);
  if (!((bool)file)) {
    std_panic(format_string("failed to open file: %s", path));
  } 
  return file;
}

i32 FILE_read(FILE *this, void *buf, u32 size) {
  return fread(buf, ((u32)1), size, this);
}

i32 FILE_write(FILE *this, void *buf, u32 size) {
  return fwrite(buf, ((u32)1), size, this);
}

char *FILE_slurp(FILE *this) {
  i32 pos = ftell(this);
  fseek(this, 0, SEEK_END);
  u32 size = ((u32)ftell(this));
  fseek(this, pos, SEEK_SET);
  void *buf = calloc((((u32)size) + ((u32)1)), ((u32)sizeof(char)));
  fread(buf, ((u32)1), size, this);
  fseek(this, pos, SEEK_SET);
  return ((char *)buf);
}

bool FILE_exists(char *path) {
  FILE *file = fopen(path, "r");
  if (!((bool)file)) {
    return false;
  } 
  fclose(file);
  return true;
}

void FILE_puts(FILE *this, char *s) {
  fwrite(s, ((u32)1), str_len(s), this);
}

void std_panic(char *msg) {
  printf("%s""\n", msg);
  exit(1);
}

u32 str_to_u32(char *this) {
  return ((u32)atoi(this));
}

bool str_eq(char *this, char *other) {
  return strcmp(this, other)==0;
}

u32 str_len(char *this) {
  return strlen(this);
}

char *str_substring(char *this, u32 start, u32 len) {
  char *new_str = ((char *)calloc(((u32)(len + ((u32)1))), ((u32)sizeof(char))));
  memcpy(new_str, &this[start], len);
  return new_str;
}

void str_strip_trailing_whitespace(char *this) {
  for (u32 i = (str_len(this) - ((u32)1)); (i >= ((u32)0)); i-=((u32)1)) {
    if ((this[i] != ' ')) 
    break;
    
    this[i]='\0';
  }
}

void str_remove_last_n(char *this, u32 n) {
  this[(str_len(this) - n)]='\0';
}

void str_replace_with(char **this, char *other) {
  free(*this);
  *this=other;
}

bool char_is_digit(char this) {
  return isdigit(this);
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

i32 std_min(i32 a, i32 b) {
  return ((a < b) ? a : b);
}

i32 std_max(i32 a, i32 b) {
  return ((a > b) ? a : b);
}

u32 u32_min(u32 this, u32 other) {
  return ((this < other) ? this : other);
}

u32 u32_max(u32 this, u32 other) {
  return ((this > other) ? this : other);
}

std_value_Value *std_compact_map_Map__0_at(std_compact_map_Map__0 *this, char *key) {
  u32 hash = str_hash(key);
  u32 index = std_compact_map_Map__0_get_index(this, key, hash);
  if (this->indices[index]==std_compact_map_INDEX_FREE) {
    ae_assert(false, ".//std/compact_map.oc:149:16: Assertion failed: `false`", "Key not found in Map::at()");
  } 
  ae_assert((index >= ((u32)0)), ".//std/compact_map.oc:152:12: Assertion failed: `index >= 0`", NULL);
  std_compact_map_Item__0 item = std_vector_Vector__2_at(this->items, ((u32)this->indices[index]));
  return item.value;
}

void std_compact_map_Map__0_free(std_compact_map_Map__0 *this) {
  free(this->indices);
  std_vector_Vector__2_free(this->items);
  free(this);
}

u32 std_compact_map_Map__0_get_index(std_compact_map_Map__0 *this, char *key, u32 hash) {
  u32 perturb = hash;
  u32 j = (hash % this->capacity);
  u32 i = j;
  i32 first_deleted = -1;
  while (true) {
    if (this->indices[i]==std_compact_map_INDEX_FREE) 
    break;
    
    if (this->indices[i]==std_compact_map_INDEX_DELETED) {
      if ((first_deleted < 0)) {
        first_deleted=((i32)i);
      } 
    }  else {
      std_compact_map_Item__0 item = std_vector_Vector__2_at(this->items, ((u32)this->indices[i]));
      if ((item.hash==hash && str_eq(item.key, key))) {
        return i;
      } 
    } 
    j=(((((u32)5) * j) + perturb) + ((u32)1));
    i=(j % this->capacity);
    perturb=(perturb >> ((u32)5));
  }
  if ((first_deleted < 0)) {
    return i;
  } 
  return ((u32)first_deleted);
}

void std_compact_map_Map__0_insert(std_compact_map_Map__0 *this, char *key, std_value_Value *value) {
  u32 hash = str_hash(key);
  u32 index = std_compact_map_Map__0_get_index(this, key, hash);
  if (this->indices[index]==std_compact_map_INDEX_FREE) {
    this->indices[index]=((i32)this->items->size);
    std_vector_Vector__2_push(this->items, (std_compact_map_Item__0){.hash=hash, .key=key, .value=value});
    if ((((u32)this->items->size) >= this->capacity)) {
      std_compact_map_Map__0_resize(this, (this->capacity * ((u32)2)));
    } 
  }  else {
    std_compact_map_Item__0 item = std_vector_Vector__2_at(this->items, ((u32)this->indices[index]));
    item.value=value;
  } 
}

void std_compact_map_Map__0_remove(std_compact_map_Map__0 *this, char *key) {
  u32 hash = str_hash(key);
  u32 index = std_compact_map_Map__0_get_index(this, key, hash);
  if (this->indices[index]==std_compact_map_INDEX_FREE) {
    return ;
  } 
  ae_assert((index >= ((u32)0)), ".//std/compact_map.oc:79:12: Assertion failed: `index >= 0`", NULL);
  u32 item_index = ((u32)this->indices[index]);
  std_compact_map_Item__0 item = std_vector_Vector__2_at(this->items, item_index);
  std_compact_map_Item__0 last_item = std_vector_Vector__2_at(this->items, (this->items->size - ((u32)1)));
  u32 last_index = std_compact_map_Map__0_get_index(this, last_item.key, last_item.hash);
  ae_assert((last_index >= ((u32)0)), ".//std/compact_map.oc:87:12: Assertion failed: `last_index >= 0`", NULL);
  ae_assert((this->indices[last_index] >= 0), ".//std/compact_map.oc:88:12: Assertion failed: `.indices[last_index] >= 0`", NULL);
  u32 last_item_index = ((u32)this->indices[last_index]);
  ae_assert(last_item_index==(this->items->size - ((u32)1)), ".//std/compact_map.oc:91:12: Assertion failed: `last_item_index == .items.size - 1`", NULL);
  this->indices[index]=std_compact_map_INDEX_DELETED;
  this->items->data[item_index]=last_item;
  std_vector_Vector__2_pop(this->items);
  this->indices[last_index]=((i32)index);
}

std_value_Value *std_compact_map_Map__0_get(std_compact_map_Map__0 *this, char *key, std_value_Value *defolt) {
  u32 hash = str_hash(key);
  u32 index = std_compact_map_Map__0_get_index(this, key, hash);
  if (this->indices[index]==std_compact_map_INDEX_FREE) {
    return defolt;
  } 
  ae_assert((index >= ((u32)0)), ".//std/compact_map.oc:140:12: Assertion failed: `index >= 0`", NULL);
  std_compact_map_Item__0 item = std_vector_Vector__2_at(this->items, ((u32)this->indices[index]));
  return item.value;
}

std_compact_map_KeyIterator__0 std_compact_map_Map__0_iter_keys(std_compact_map_Map__0 *this) {
  return (std_compact_map_KeyIterator__0){.iter=std_vector_Vector__2_iter(this->items)};
}

char *std_compact_map_KeyIterator__0_cur(std_compact_map_KeyIterator__0 *this) {
  return std_vector_Iterator__2_cur(&this->iter).key;
}

void std_compact_map_KeyIterator__0_next(std_compact_map_KeyIterator__0 *this) {
  std_vector_Iterator__2_next(&this->iter);
}

bool std_compact_map_KeyIterator__0_has_value(std_compact_map_KeyIterator__0 *this) {
  return std_vector_Iterator__2_has_value(&this->iter);
}

bool std_compact_map_Map__0_is_empty(std_compact_map_Map__0 *this) {
  return this->items->size==((u32)0);
}

void std_compact_map_Map__0_resize(std_compact_map_Map__0 *this, u32 new_capacity) {
  free(this->indices);
  this->indices=((i32 *)calloc(new_capacity, ((u32)sizeof(i32))));
  this->capacity=new_capacity;
  for (u32 i = ((u32)0); (i < new_capacity); i++) {
    this->indices[i]=std_compact_map_INDEX_FREE;
  }
  for (u32 i = ((u32)0); (i < this->items->size); i++) {
    std_compact_map_Item__0 item = std_vector_Vector__2_at(this->items, i);
    u32 index = std_compact_map_Map__0_get_index(this, item.key, item.hash);
    if (this->indices[index]==std_compact_map_INDEX_FREE) {
      this->indices[index]=((i32)i);
    } 
  }
}

std_compact_map_ValueIterator__0 std_compact_map_Map__0_iter_values(std_compact_map_Map__0 *this) {
  return (std_compact_map_ValueIterator__0){.iter=std_vector_Vector__2_iter(this->items)};
}

std_value_Value *std_compact_map_ValueIterator__0_cur(std_compact_map_ValueIterator__0 *this) {
  return std_vector_Iterator__2_cur(&this->iter).value;
}

void std_compact_map_ValueIterator__0_next(std_compact_map_ValueIterator__0 *this) {
  std_vector_Iterator__2_next(&this->iter);
}

bool std_compact_map_ValueIterator__0_has_value(std_compact_map_ValueIterator__0 *this) {
  return std_vector_Iterator__2_has_value(&this->iter);
}

std_compact_map_Map__0 *std_compact_map_Map__0_new(u32 capacity) {
  std_vector_Vector__2 *items = std_vector_Vector__2_new(((u32)16));
  i32 *indices = ((i32 *)calloc(capacity, ((u32)sizeof(i32))));
  for (u32 i = ((u32)0); (i < capacity); i++) {
    indices[i]=std_compact_map_INDEX_FREE;
  }
  std_compact_map_Map__0 *map = ((std_compact_map_Map__0 *)calloc(((u32)1), ((u32)sizeof(std_compact_map_Map__0))));
  map->items=items;
  map->indices=indices;
  map->capacity=capacity;
  return map;
}

std_vector_Iterator__2 std_compact_map_Map__0_iter(std_compact_map_Map__0 *this) {
  return std_vector_Vector__2_iter(this->items);
}

std_buffer_Buffer std_buffer_Buffer_make(u32 capacity) {
  return (std_buffer_Buffer){.data=calloc(capacity, ((u32)1)), .size=((u32)0), .capacity=capacity};
}

std_buffer_Buffer std_buffer_Buffer_from_str(char *s) {
  return (std_buffer_Buffer){.data=((u8 *)s), .size=((u32)str_len(s)), .capacity=((u32)str_len(s))};
}

std_buffer_Buffer std_buffer_Buffer_from_sized_str(char *s, u32 size) {
  return (std_buffer_Buffer){.data=((u8 *)s), .size=((u32)size), .capacity=((u32)str_len(s))};
}

void std_buffer_Buffer_resize_if_necessary(std_buffer_Buffer *this, u32 new_size) {
  if ((new_size >= this->capacity)) {
    i32 new_capacity = std_max((((i32)this->capacity) * 2), ((i32)new_size));
    this->data=((u8 *)realloc(this->data, ((u32)new_capacity)));
    ae_assert(((bool)this->data), ".//std/buffer.oc:45:16: Assertion failed: `.data?`", "Out of memory!");
  } 
}

void std_buffer_Buffer_hex_dump(std_buffer_Buffer *this) {
  printf("(%d bytes): ", this->size);
  for (u32 i = ((u32)0); (i < this->size); i+=((u32)1)) {
    if (((i % ((u32)4))==((u32)0) && (i > ((u32)0)))) 
    printf("_");
    
    printf("%02x", this->data[i]);
  }
  printf("""\n");
}

void std_buffer_Buffer_putb(std_buffer_Buffer *this, std_buffer_Buffer *buf) {
  std_buffer_Buffer_resize_if_necessary(this, ((this->size + buf->size) + ((u32)1)));
  memcpy((this->data + this->size), buf->data, buf->size);
  this->data[(this->size + buf->size)]=((u8)'\0');
  this->size+=buf->size;
}

void std_buffer_Buffer_putbf(std_buffer_Buffer *this, std_buffer_Buffer *buf) {
  std_buffer_Buffer_putb(this, buf);
  std_buffer_Buffer_free(buf);
}

void std_buffer_Buffer_puts(std_buffer_Buffer *this, char *s) {
  u32 len = ((u32)str_len(s));
  std_buffer_Buffer_resize_if_necessary(this, ((this->size + ((u32)len)) + ((u32)1)));
  memcpy((this->data + this->size), s, (len + ((u32)1)));
  this->size+=len;
}

void std_buffer_Buffer_putc(std_buffer_Buffer *this, char c) {
  std_buffer_Buffer_resize_if_necessary(this, (this->size + ((u32)2)));
  this->data[this->size]=((u8)c);
  this->size+=((u32)1);
  this->data[this->size]=((u8)'\0');
}

void std_buffer_Buffer_putsf(std_buffer_Buffer *this, char *s) {
  std_buffer_Buffer_puts(this, s);
  free(s);
}

char *std_buffer_Buffer_str(std_buffer_Buffer *this) {
  return ((char *)this->data);
}

char *std_buffer_Buffer_new_str(std_buffer_Buffer *this) {
  return strdup(((char *)this->data));
}

void std_buffer_Buffer_clear(std_buffer_Buffer *this) {
  this->size=((u32)0);
}

void std_buffer_Buffer_free(std_buffer_Buffer *this) {
  free(this->data);
}

void std_map_Node__0_free_list(std_map_Node__0 *this) {
  std_map_Node__0 *cur = this;
  while (((bool)cur)) {
    std_map_Node__0 *next = cur->next;
    free(cur);
    cur=next;
  }
}

std_map_Node__0 *std_map_Node__0_new(char *key, ast_nodes_Structure *value, std_map_Node__0 *next) {
  std_map_Node__0 *node = ((std_map_Node__0 *)calloc(((u32)1), ((u32)sizeof(std_map_Node__0))));
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

void std_map_Map__0_push_keys(std_map_Map__0 *this, std_vector_Vector__7 *vec) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__0 *node = this->buckets[i];
    while (((bool)node)) {
      std_vector_Vector__7_push(vec, node->key);
      node=node->next;
    }
  }
}

ast_nodes_Structure *std_map_Map__0_at(std_map_Map__0 *this, char *key) {
  std_map_Node__0 *node = std_map_Map__0_get_node(this, key);
  ae_assert(((bool)node), ".//std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__0_free(std_map_Map__0 *this) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__0_free_list(this->buckets[i]);
  }
  free(this->buckets);
}

void std_map_Map__0_insert(std_map_Map__0 *this, char *key, ast_nodes_Structure *value) {
  std_map_Node__0 *node = std_map_Map__0_get_node(this, key);
  if (((bool)node)) {
    node->value=value;
  }  else {
    u32 hash = std_map_Map__0_hash(this, key);
    std_map_Node__0 *new_node = std_map_Node__0_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=((u32)1);
    } 
    this->buckets[hash]=new_node;
    this->num_items+=((u32)1);
    if ((this->num_items > this->num_buckets)) {
      std_map_Map__0_resize(this);
    } 
  } 
}

ast_nodes_Structure *std_map_Map__0_get(std_map_Map__0 *this, char *key, ast_nodes_Structure *defolt) {
  std_map_Node__0 *node = std_map_Map__0_get_node(this, key);
  if (!((bool)node)) 
  return defolt;
  
  return node->value;
}

void std_map_Map__0_extend(std_map_Map__0 *this, std_map_Map__0 *other) {
  for (std_map_Iterator__0 __iter = std_map_Map__0_iter(other); std_map_Iterator__0_has_value(&__iter); std_map_Iterator__0_next(&__iter)) {
    std_map_Node__0 *iter = std_map_Iterator__0_cur(&__iter);
    {
      std_map_Map__0_insert(this, iter->key, iter->value);
    }
  }
}

void std_map_Map__0_remove(std_map_Map__0 *this, char *key) {
  std_map_Node__0 *node = std_map_Map__0_get_node(this, key);
  if (((bool)node)) {
    u32 hash = std_map_Map__0_hash(this, key);
    if (this->buckets[hash]==node) {
      this->buckets[hash]=node->next;
    }  else {
      std_map_Node__0 *prev = this->buckets[hash];
      while ((prev->next != node)) {
        prev=prev->next;
      }
      prev->next=node->next;
    } 
    free(node);
    this->num_items-=((u32)1);
  } 
}

std_map_KeyIterator__0 std_map_Map__0_iter_keys(std_map_Map__0 *this) {
  return (std_map_KeyIterator__0){.map_iter=std_map_Map__0_iter(this)};
}

std_map_Node__0 *std_map_Iterator__0_cur(std_map_Iterator__0 *this) {
  return this->node;
}

void std_map_Iterator__0_next(std_map_Iterator__0 *this) {
  while ((this->idx < ((i32)this->map->num_buckets))) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) 
      return ;
      
    }
    this->idx+=1;
    this->node=({ std_map_Node__0 *__yield_0;
      if ((this->idx < ((i32)this->map->num_buckets))) {
        __yield_0 = this->map->buckets[this->idx];
      }  else {
        __yield_0 = NULL;
      } 
;__yield_0; });
    if (((bool)this->node)) 
    return ;
    
  }
}

char *std_map_Iterator__0_key(std_map_Iterator__0 *this) {
  return this->node->key;
}

bool std_map_Iterator__0_has_value(std_map_Iterator__0 *this) {
  return ((bool)this->node);
}

ast_nodes_Structure *std_map_Iterator__0_value(std_map_Iterator__0 *this) {
  return this->node->value;
}

std_map_Iterator__0 std_map_Iterator__0_make(std_map_Map__0 *map) {
  std_map_Iterator__0 it = (std_map_Iterator__0){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__0_next(&it);
  return it;
}

char *std_map_KeyIterator__0_cur(std_map_KeyIterator__0 *this) {
  return std_map_Iterator__0_cur(&this->map_iter)->key;
}

void std_map_KeyIterator__0_next(std_map_KeyIterator__0 *this) {
  std_map_Iterator__0_next(&this->map_iter);
}

bool std_map_KeyIterator__0_has_value(std_map_KeyIterator__0 *this) {
  return std_map_Iterator__0_has_value(&this->map_iter);
}

bool std_map_Map__0_is_empty(std_map_Map__0 *this) {
  return this->num_items==((u32)0);
}

bool std_map_Map__0_contains(std_map_Map__0 *this, char *key) {
  return ((bool)std_map_Map__0_get_node(this, key));
}

void std_map_Map__0_resize(std_map_Map__0 *this) {
  std_map_Node__0 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_num_items = this->num_items;
  this->num_collisions=((u32)0);
  this->num_buckets*=((u32)2);
  this->buckets=((std_map_Node__0 **)calloc(this->num_buckets, ((u32)sizeof(std_map_Node__0 *))));
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__0 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__0_hash(this, node->key);
      std_map_Node__0 *new_node = std_map_Node__0_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=((u32)1);
      } 
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__0_free_list(old_buckets[i]);
  }
  free(old_buckets);
}

u32 std_map_Map__0_hash(std_map_Map__0 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if ((hash < ((u32)0))) {
    hash+=this->num_buckets;
  } 
  return hash;
}

std_map_ValueIterator__0 std_map_Map__0_iter_values(std_map_Map__0 *this) {
  return (std_map_ValueIterator__0){.map_iter=std_map_Map__0_iter(this)};
}

ast_nodes_Structure *std_map_ValueIterator__0_cur(std_map_ValueIterator__0 *this) {
  return std_map_Iterator__0_cur(&this->map_iter)->value;
}

void std_map_ValueIterator__0_next(std_map_ValueIterator__0 *this) {
  std_map_Iterator__0_next(&this->map_iter);
}

bool std_map_ValueIterator__0_has_value(std_map_ValueIterator__0 *this) {
  return std_map_Iterator__0_has_value(&this->map_iter);
}

std_map_Map__0 *std_map_Map__0_new(void) {
  std_map_Map__0 *map = ((std_map_Map__0 *)calloc(((u32)1), ((u32)sizeof(std_map_Map__0))));
  map->num_buckets=((u32)4);
  map->buckets=((std_map_Node__0 **)calloc(map->num_buckets, ((u32)sizeof(std_map_Node__0 *))));
  return map;
}

std_map_Node__0 *std_map_Map__0_get_node(std_map_Map__0 *this, char *key) {
  u32 hash = std_map_Map__0_hash(this, key);
  std_map_Node__0 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    } 
    node=node->next;
  }
  return NULL;
}

std_map_Iterator__0 std_map_Map__0_iter(std_map_Map__0 *this) {
  return std_map_Iterator__0_make(this);
}

void std_map_Node__1_free_list(std_map_Node__1 *this) {
  std_map_Node__1 *cur = this;
  while (((bool)cur)) {
    std_map_Node__1 *next = cur->next;
    free(cur);
    cur=next;
  }
}

std_map_Node__1 *std_map_Node__1_new(char *key, ast_scopes_Symbol *value, std_map_Node__1 *next) {
  std_map_Node__1 *node = ((std_map_Node__1 *)calloc(((u32)1), ((u32)sizeof(std_map_Node__1))));
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

void std_map_Map__1_push_keys(std_map_Map__1 *this, std_vector_Vector__7 *vec) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__1 *node = this->buckets[i];
    while (((bool)node)) {
      std_vector_Vector__7_push(vec, node->key);
      node=node->next;
    }
  }
}

ast_scopes_Symbol *std_map_Map__1_at(std_map_Map__1 *this, char *key) {
  std_map_Node__1 *node = std_map_Map__1_get_node(this, key);
  ae_assert(((bool)node), ".//std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__1_free(std_map_Map__1 *this) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__1_free_list(this->buckets[i]);
  }
  free(this->buckets);
}

void std_map_Map__1_insert(std_map_Map__1 *this, char *key, ast_scopes_Symbol *value) {
  std_map_Node__1 *node = std_map_Map__1_get_node(this, key);
  if (((bool)node)) {
    node->value=value;
  }  else {
    u32 hash = std_map_Map__1_hash(this, key);
    std_map_Node__1 *new_node = std_map_Node__1_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=((u32)1);
    } 
    this->buckets[hash]=new_node;
    this->num_items+=((u32)1);
    if ((this->num_items > this->num_buckets)) {
      std_map_Map__1_resize(this);
    } 
  } 
}

ast_scopes_Symbol *std_map_Map__1_get(std_map_Map__1 *this, char *key, ast_scopes_Symbol *defolt) {
  std_map_Node__1 *node = std_map_Map__1_get_node(this, key);
  if (!((bool)node)) 
  return defolt;
  
  return node->value;
}

void std_map_Map__1_extend(std_map_Map__1 *this, std_map_Map__1 *other) {
  for (std_map_Iterator__1 __iter = std_map_Map__1_iter(other); std_map_Iterator__1_has_value(&__iter); std_map_Iterator__1_next(&__iter)) {
    std_map_Node__1 *iter = std_map_Iterator__1_cur(&__iter);
    {
      std_map_Map__1_insert(this, iter->key, iter->value);
    }
  }
}

void std_map_Map__1_remove(std_map_Map__1 *this, char *key) {
  std_map_Node__1 *node = std_map_Map__1_get_node(this, key);
  if (((bool)node)) {
    u32 hash = std_map_Map__1_hash(this, key);
    if (this->buckets[hash]==node) {
      this->buckets[hash]=node->next;
    }  else {
      std_map_Node__1 *prev = this->buckets[hash];
      while ((prev->next != node)) {
        prev=prev->next;
      }
      prev->next=node->next;
    } 
    free(node);
    this->num_items-=((u32)1);
  } 
}

std_map_KeyIterator__1 std_map_Map__1_iter_keys(std_map_Map__1 *this) {
  return (std_map_KeyIterator__1){.map_iter=std_map_Map__1_iter(this)};
}

std_map_Node__1 *std_map_Iterator__1_cur(std_map_Iterator__1 *this) {
  return this->node;
}

void std_map_Iterator__1_next(std_map_Iterator__1 *this) {
  while ((this->idx < ((i32)this->map->num_buckets))) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) 
      return ;
      
    }
    this->idx+=1;
    this->node=({ std_map_Node__1 *__yield_0;
      if ((this->idx < ((i32)this->map->num_buckets))) {
        __yield_0 = this->map->buckets[this->idx];
      }  else {
        __yield_0 = NULL;
      } 
;__yield_0; });
    if (((bool)this->node)) 
    return ;
    
  }
}

char *std_map_Iterator__1_key(std_map_Iterator__1 *this) {
  return this->node->key;
}

bool std_map_Iterator__1_has_value(std_map_Iterator__1 *this) {
  return ((bool)this->node);
}

ast_scopes_Symbol *std_map_Iterator__1_value(std_map_Iterator__1 *this) {
  return this->node->value;
}

std_map_Iterator__1 std_map_Iterator__1_make(std_map_Map__1 *map) {
  std_map_Iterator__1 it = (std_map_Iterator__1){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__1_next(&it);
  return it;
}

char *std_map_KeyIterator__1_cur(std_map_KeyIterator__1 *this) {
  return std_map_Iterator__1_cur(&this->map_iter)->key;
}

void std_map_KeyIterator__1_next(std_map_KeyIterator__1 *this) {
  std_map_Iterator__1_next(&this->map_iter);
}

bool std_map_KeyIterator__1_has_value(std_map_KeyIterator__1 *this) {
  return std_map_Iterator__1_has_value(&this->map_iter);
}

bool std_map_Map__1_is_empty(std_map_Map__1 *this) {
  return this->num_items==((u32)0);
}

bool std_map_Map__1_contains(std_map_Map__1 *this, char *key) {
  return ((bool)std_map_Map__1_get_node(this, key));
}

void std_map_Map__1_resize(std_map_Map__1 *this) {
  std_map_Node__1 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_num_items = this->num_items;
  this->num_collisions=((u32)0);
  this->num_buckets*=((u32)2);
  this->buckets=((std_map_Node__1 **)calloc(this->num_buckets, ((u32)sizeof(std_map_Node__1 *))));
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__1 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__1_hash(this, node->key);
      std_map_Node__1 *new_node = std_map_Node__1_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=((u32)1);
      } 
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__1_free_list(old_buckets[i]);
  }
  free(old_buckets);
}

u32 std_map_Map__1_hash(std_map_Map__1 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if ((hash < ((u32)0))) {
    hash+=this->num_buckets;
  } 
  return hash;
}

std_map_ValueIterator__1 std_map_Map__1_iter_values(std_map_Map__1 *this) {
  return (std_map_ValueIterator__1){.map_iter=std_map_Map__1_iter(this)};
}

ast_scopes_Symbol *std_map_ValueIterator__1_cur(std_map_ValueIterator__1 *this) {
  return std_map_Iterator__1_cur(&this->map_iter)->value;
}

void std_map_ValueIterator__1_next(std_map_ValueIterator__1 *this) {
  std_map_Iterator__1_next(&this->map_iter);
}

bool std_map_ValueIterator__1_has_value(std_map_ValueIterator__1 *this) {
  return std_map_Iterator__1_has_value(&this->map_iter);
}

std_map_Map__1 *std_map_Map__1_new(void) {
  std_map_Map__1 *map = ((std_map_Map__1 *)calloc(((u32)1), ((u32)sizeof(std_map_Map__1))));
  map->num_buckets=((u32)4);
  map->buckets=((std_map_Node__1 **)calloc(map->num_buckets, ((u32)sizeof(std_map_Node__1 *))));
  return map;
}

std_map_Node__1 *std_map_Map__1_get_node(std_map_Map__1 *this, char *key) {
  u32 hash = std_map_Map__1_hash(this, key);
  std_map_Node__1 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    } 
    node=node->next;
  }
  return NULL;
}

std_map_Iterator__1 std_map_Map__1_iter(std_map_Map__1 *this) {
  return std_map_Iterator__1_make(this);
}

void std_map_Node__2_free_list(std_map_Node__2 *this) {
  std_map_Node__2 *cur = this;
  while (((bool)cur)) {
    std_map_Node__2 *next = cur->next;
    free(cur);
    cur=next;
  }
}

std_map_Node__2 *std_map_Node__2_new(char *key, ast_program_Namespace *value, std_map_Node__2 *next) {
  std_map_Node__2 *node = ((std_map_Node__2 *)calloc(((u32)1), ((u32)sizeof(std_map_Node__2))));
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

void std_map_Map__2_push_keys(std_map_Map__2 *this, std_vector_Vector__7 *vec) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__2 *node = this->buckets[i];
    while (((bool)node)) {
      std_vector_Vector__7_push(vec, node->key);
      node=node->next;
    }
  }
}

ast_program_Namespace *std_map_Map__2_at(std_map_Map__2 *this, char *key) {
  std_map_Node__2 *node = std_map_Map__2_get_node(this, key);
  ae_assert(((bool)node), ".//std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__2_free(std_map_Map__2 *this) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__2_free_list(this->buckets[i]);
  }
  free(this->buckets);
}

void std_map_Map__2_insert(std_map_Map__2 *this, char *key, ast_program_Namespace *value) {
  std_map_Node__2 *node = std_map_Map__2_get_node(this, key);
  if (((bool)node)) {
    node->value=value;
  }  else {
    u32 hash = std_map_Map__2_hash(this, key);
    std_map_Node__2 *new_node = std_map_Node__2_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=((u32)1);
    } 
    this->buckets[hash]=new_node;
    this->num_items+=((u32)1);
    if ((this->num_items > this->num_buckets)) {
      std_map_Map__2_resize(this);
    } 
  } 
}

ast_program_Namespace *std_map_Map__2_get(std_map_Map__2 *this, char *key, ast_program_Namespace *defolt) {
  std_map_Node__2 *node = std_map_Map__2_get_node(this, key);
  if (!((bool)node)) 
  return defolt;
  
  return node->value;
}

void std_map_Map__2_extend(std_map_Map__2 *this, std_map_Map__2 *other) {
  for (std_map_Iterator__2 __iter = std_map_Map__2_iter(other); std_map_Iterator__2_has_value(&__iter); std_map_Iterator__2_next(&__iter)) {
    std_map_Node__2 *iter = std_map_Iterator__2_cur(&__iter);
    {
      std_map_Map__2_insert(this, iter->key, iter->value);
    }
  }
}

void std_map_Map__2_remove(std_map_Map__2 *this, char *key) {
  std_map_Node__2 *node = std_map_Map__2_get_node(this, key);
  if (((bool)node)) {
    u32 hash = std_map_Map__2_hash(this, key);
    if (this->buckets[hash]==node) {
      this->buckets[hash]=node->next;
    }  else {
      std_map_Node__2 *prev = this->buckets[hash];
      while ((prev->next != node)) {
        prev=prev->next;
      }
      prev->next=node->next;
    } 
    free(node);
    this->num_items-=((u32)1);
  } 
}

std_map_KeyIterator__2 std_map_Map__2_iter_keys(std_map_Map__2 *this) {
  return (std_map_KeyIterator__2){.map_iter=std_map_Map__2_iter(this)};
}

std_map_Node__2 *std_map_Iterator__2_cur(std_map_Iterator__2 *this) {
  return this->node;
}

void std_map_Iterator__2_next(std_map_Iterator__2 *this) {
  while ((this->idx < ((i32)this->map->num_buckets))) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) 
      return ;
      
    }
    this->idx+=1;
    this->node=({ std_map_Node__2 *__yield_0;
      if ((this->idx < ((i32)this->map->num_buckets))) {
        __yield_0 = this->map->buckets[this->idx];
      }  else {
        __yield_0 = NULL;
      } 
;__yield_0; });
    if (((bool)this->node)) 
    return ;
    
  }
}

char *std_map_Iterator__2_key(std_map_Iterator__2 *this) {
  return this->node->key;
}

bool std_map_Iterator__2_has_value(std_map_Iterator__2 *this) {
  return ((bool)this->node);
}

ast_program_Namespace *std_map_Iterator__2_value(std_map_Iterator__2 *this) {
  return this->node->value;
}

std_map_Iterator__2 std_map_Iterator__2_make(std_map_Map__2 *map) {
  std_map_Iterator__2 it = (std_map_Iterator__2){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__2_next(&it);
  return it;
}

char *std_map_KeyIterator__2_cur(std_map_KeyIterator__2 *this) {
  return std_map_Iterator__2_cur(&this->map_iter)->key;
}

void std_map_KeyIterator__2_next(std_map_KeyIterator__2 *this) {
  std_map_Iterator__2_next(&this->map_iter);
}

bool std_map_KeyIterator__2_has_value(std_map_KeyIterator__2 *this) {
  return std_map_Iterator__2_has_value(&this->map_iter);
}

bool std_map_Map__2_is_empty(std_map_Map__2 *this) {
  return this->num_items==((u32)0);
}

bool std_map_Map__2_contains(std_map_Map__2 *this, char *key) {
  return ((bool)std_map_Map__2_get_node(this, key));
}

void std_map_Map__2_resize(std_map_Map__2 *this) {
  std_map_Node__2 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_num_items = this->num_items;
  this->num_collisions=((u32)0);
  this->num_buckets*=((u32)2);
  this->buckets=((std_map_Node__2 **)calloc(this->num_buckets, ((u32)sizeof(std_map_Node__2 *))));
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__2 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__2_hash(this, node->key);
      std_map_Node__2 *new_node = std_map_Node__2_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=((u32)1);
      } 
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__2_free_list(old_buckets[i]);
  }
  free(old_buckets);
}

u32 std_map_Map__2_hash(std_map_Map__2 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if ((hash < ((u32)0))) {
    hash+=this->num_buckets;
  } 
  return hash;
}

std_map_ValueIterator__2 std_map_Map__2_iter_values(std_map_Map__2 *this) {
  return (std_map_ValueIterator__2){.map_iter=std_map_Map__2_iter(this)};
}

ast_program_Namespace *std_map_ValueIterator__2_cur(std_map_ValueIterator__2 *this) {
  return std_map_Iterator__2_cur(&this->map_iter)->value;
}

void std_map_ValueIterator__2_next(std_map_ValueIterator__2 *this) {
  std_map_Iterator__2_next(&this->map_iter);
}

bool std_map_ValueIterator__2_has_value(std_map_ValueIterator__2 *this) {
  return std_map_Iterator__2_has_value(&this->map_iter);
}

std_map_Map__2 *std_map_Map__2_new(void) {
  std_map_Map__2 *map = ((std_map_Map__2 *)calloc(((u32)1), ((u32)sizeof(std_map_Map__2))));
  map->num_buckets=((u32)4);
  map->buckets=((std_map_Node__2 **)calloc(map->num_buckets, ((u32)sizeof(std_map_Node__2 *))));
  return map;
}

std_map_Node__2 *std_map_Map__2_get_node(std_map_Map__2 *this, char *key) {
  u32 hash = std_map_Map__2_hash(this, key);
  std_map_Node__2 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    } 
    node=node->next;
  }
  return NULL;
}

std_map_Iterator__2 std_map_Map__2_iter(std_map_Map__2 *this) {
  return std_map_Iterator__2_make(this);
}

void std_map_Node__3_free_list(std_map_Node__3 *this) {
  std_map_Node__3 *cur = this;
  while (((bool)cur)) {
    std_map_Node__3 *next = cur->next;
    free(cur);
    cur=next;
  }
}

std_map_Node__3 *std_map_Node__3_new(char *key, char *value, std_map_Node__3 *next) {
  std_map_Node__3 *node = ((std_map_Node__3 *)calloc(((u32)1), ((u32)sizeof(std_map_Node__3))));
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

void std_map_Map__3_push_keys(std_map_Map__3 *this, std_vector_Vector__7 *vec) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__3 *node = this->buckets[i];
    while (((bool)node)) {
      std_vector_Vector__7_push(vec, node->key);
      node=node->next;
    }
  }
}

char *std_map_Map__3_at(std_map_Map__3 *this, char *key) {
  std_map_Node__3 *node = std_map_Map__3_get_node(this, key);
  ae_assert(((bool)node), ".//std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__3_free(std_map_Map__3 *this) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__3_free_list(this->buckets[i]);
  }
  free(this->buckets);
}

void std_map_Map__3_insert(std_map_Map__3 *this, char *key, char *value) {
  std_map_Node__3 *node = std_map_Map__3_get_node(this, key);
  if (((bool)node)) {
    node->value=value;
  }  else {
    u32 hash = std_map_Map__3_hash(this, key);
    std_map_Node__3 *new_node = std_map_Node__3_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=((u32)1);
    } 
    this->buckets[hash]=new_node;
    this->num_items+=((u32)1);
    if ((this->num_items > this->num_buckets)) {
      std_map_Map__3_resize(this);
    } 
  } 
}

char *std_map_Map__3_get(std_map_Map__3 *this, char *key, char *defolt) {
  std_map_Node__3 *node = std_map_Map__3_get_node(this, key);
  if (!((bool)node)) 
  return defolt;
  
  return node->value;
}

void std_map_Map__3_extend(std_map_Map__3 *this, std_map_Map__3 *other) {
  for (std_map_Iterator__3 __iter = std_map_Map__3_iter(other); std_map_Iterator__3_has_value(&__iter); std_map_Iterator__3_next(&__iter)) {
    std_map_Node__3 *iter = std_map_Iterator__3_cur(&__iter);
    {
      std_map_Map__3_insert(this, iter->key, iter->value);
    }
  }
}

void std_map_Map__3_remove(std_map_Map__3 *this, char *key) {
  std_map_Node__3 *node = std_map_Map__3_get_node(this, key);
  if (((bool)node)) {
    u32 hash = std_map_Map__3_hash(this, key);
    if (this->buckets[hash]==node) {
      this->buckets[hash]=node->next;
    }  else {
      std_map_Node__3 *prev = this->buckets[hash];
      while ((prev->next != node)) {
        prev=prev->next;
      }
      prev->next=node->next;
    } 
    free(node);
    this->num_items-=((u32)1);
  } 
}

std_map_KeyIterator__3 std_map_Map__3_iter_keys(std_map_Map__3 *this) {
  return (std_map_KeyIterator__3){.map_iter=std_map_Map__3_iter(this)};
}

std_map_Node__3 *std_map_Iterator__3_cur(std_map_Iterator__3 *this) {
  return this->node;
}

void std_map_Iterator__3_next(std_map_Iterator__3 *this) {
  while ((this->idx < ((i32)this->map->num_buckets))) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) 
      return ;
      
    }
    this->idx+=1;
    this->node=({ std_map_Node__3 *__yield_0;
      if ((this->idx < ((i32)this->map->num_buckets))) {
        __yield_0 = this->map->buckets[this->idx];
      }  else {
        __yield_0 = NULL;
      } 
;__yield_0; });
    if (((bool)this->node)) 
    return ;
    
  }
}

char *std_map_Iterator__3_key(std_map_Iterator__3 *this) {
  return this->node->key;
}

bool std_map_Iterator__3_has_value(std_map_Iterator__3 *this) {
  return ((bool)this->node);
}

char *std_map_Iterator__3_value(std_map_Iterator__3 *this) {
  return this->node->value;
}

std_map_Iterator__3 std_map_Iterator__3_make(std_map_Map__3 *map) {
  std_map_Iterator__3 it = (std_map_Iterator__3){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__3_next(&it);
  return it;
}

char *std_map_KeyIterator__3_cur(std_map_KeyIterator__3 *this) {
  return std_map_Iterator__3_cur(&this->map_iter)->key;
}

void std_map_KeyIterator__3_next(std_map_KeyIterator__3 *this) {
  std_map_Iterator__3_next(&this->map_iter);
}

bool std_map_KeyIterator__3_has_value(std_map_KeyIterator__3 *this) {
  return std_map_Iterator__3_has_value(&this->map_iter);
}

bool std_map_Map__3_is_empty(std_map_Map__3 *this) {
  return this->num_items==((u32)0);
}

bool std_map_Map__3_contains(std_map_Map__3 *this, char *key) {
  return ((bool)std_map_Map__3_get_node(this, key));
}

void std_map_Map__3_resize(std_map_Map__3 *this) {
  std_map_Node__3 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_num_items = this->num_items;
  this->num_collisions=((u32)0);
  this->num_buckets*=((u32)2);
  this->buckets=((std_map_Node__3 **)calloc(this->num_buckets, ((u32)sizeof(std_map_Node__3 *))));
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__3 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__3_hash(this, node->key);
      std_map_Node__3 *new_node = std_map_Node__3_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=((u32)1);
      } 
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__3_free_list(old_buckets[i]);
  }
  free(old_buckets);
}

u32 std_map_Map__3_hash(std_map_Map__3 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if ((hash < ((u32)0))) {
    hash+=this->num_buckets;
  } 
  return hash;
}

std_map_ValueIterator__3 std_map_Map__3_iter_values(std_map_Map__3 *this) {
  return (std_map_ValueIterator__3){.map_iter=std_map_Map__3_iter(this)};
}

char *std_map_ValueIterator__3_cur(std_map_ValueIterator__3 *this) {
  return std_map_Iterator__3_cur(&this->map_iter)->value;
}

void std_map_ValueIterator__3_next(std_map_ValueIterator__3 *this) {
  std_map_Iterator__3_next(&this->map_iter);
}

bool std_map_ValueIterator__3_has_value(std_map_ValueIterator__3 *this) {
  return std_map_Iterator__3_has_value(&this->map_iter);
}

std_map_Map__3 *std_map_Map__3_new(void) {
  std_map_Map__3 *map = ((std_map_Map__3 *)calloc(((u32)1), ((u32)sizeof(std_map_Map__3))));
  map->num_buckets=((u32)4);
  map->buckets=((std_map_Node__3 **)calloc(map->num_buckets, ((u32)sizeof(std_map_Node__3 *))));
  return map;
}

std_map_Node__3 *std_map_Map__3_get_node(std_map_Map__3 *this, char *key) {
  u32 hash = std_map_Map__3_hash(this, key);
  std_map_Node__3 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    } 
    node=node->next;
  }
  return NULL;
}

std_map_Iterator__3 std_map_Map__3_iter(std_map_Map__3 *this) {
  return std_map_Iterator__3_make(this);
}

void std_map_Node__4_free_list(std_map_Node__4 *this) {
  std_map_Node__4 *cur = this;
  while (((bool)cur)) {
    std_map_Node__4 *next = cur->next;
    free(cur);
    cur=next;
  }
}

std_map_Node__4 *std_map_Node__4_new(char *key, ast_nodes_Function *value, std_map_Node__4 *next) {
  std_map_Node__4 *node = ((std_map_Node__4 *)calloc(((u32)1), ((u32)sizeof(std_map_Node__4))));
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

void std_map_Map__4_push_keys(std_map_Map__4 *this, std_vector_Vector__7 *vec) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__4 *node = this->buckets[i];
    while (((bool)node)) {
      std_vector_Vector__7_push(vec, node->key);
      node=node->next;
    }
  }
}

ast_nodes_Function *std_map_Map__4_at(std_map_Map__4 *this, char *key) {
  std_map_Node__4 *node = std_map_Map__4_get_node(this, key);
  ae_assert(((bool)node), ".//std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__4_free(std_map_Map__4 *this) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__4_free_list(this->buckets[i]);
  }
  free(this->buckets);
}

void std_map_Map__4_insert(std_map_Map__4 *this, char *key, ast_nodes_Function *value) {
  std_map_Node__4 *node = std_map_Map__4_get_node(this, key);
  if (((bool)node)) {
    node->value=value;
  }  else {
    u32 hash = std_map_Map__4_hash(this, key);
    std_map_Node__4 *new_node = std_map_Node__4_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=((u32)1);
    } 
    this->buckets[hash]=new_node;
    this->num_items+=((u32)1);
    if ((this->num_items > this->num_buckets)) {
      std_map_Map__4_resize(this);
    } 
  } 
}

ast_nodes_Function *std_map_Map__4_get(std_map_Map__4 *this, char *key, ast_nodes_Function *defolt) {
  std_map_Node__4 *node = std_map_Map__4_get_node(this, key);
  if (!((bool)node)) 
  return defolt;
  
  return node->value;
}

void std_map_Map__4_extend(std_map_Map__4 *this, std_map_Map__4 *other) {
  for (std_map_Iterator__4 __iter = std_map_Map__4_iter(other); std_map_Iterator__4_has_value(&__iter); std_map_Iterator__4_next(&__iter)) {
    std_map_Node__4 *iter = std_map_Iterator__4_cur(&__iter);
    {
      std_map_Map__4_insert(this, iter->key, iter->value);
    }
  }
}

void std_map_Map__4_remove(std_map_Map__4 *this, char *key) {
  std_map_Node__4 *node = std_map_Map__4_get_node(this, key);
  if (((bool)node)) {
    u32 hash = std_map_Map__4_hash(this, key);
    if (this->buckets[hash]==node) {
      this->buckets[hash]=node->next;
    }  else {
      std_map_Node__4 *prev = this->buckets[hash];
      while ((prev->next != node)) {
        prev=prev->next;
      }
      prev->next=node->next;
    } 
    free(node);
    this->num_items-=((u32)1);
  } 
}

std_map_KeyIterator__4 std_map_Map__4_iter_keys(std_map_Map__4 *this) {
  return (std_map_KeyIterator__4){.map_iter=std_map_Map__4_iter(this)};
}

std_map_Node__4 *std_map_Iterator__4_cur(std_map_Iterator__4 *this) {
  return this->node;
}

void std_map_Iterator__4_next(std_map_Iterator__4 *this) {
  while ((this->idx < ((i32)this->map->num_buckets))) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) 
      return ;
      
    }
    this->idx+=1;
    this->node=({ std_map_Node__4 *__yield_0;
      if ((this->idx < ((i32)this->map->num_buckets))) {
        __yield_0 = this->map->buckets[this->idx];
      }  else {
        __yield_0 = NULL;
      } 
;__yield_0; });
    if (((bool)this->node)) 
    return ;
    
  }
}

char *std_map_Iterator__4_key(std_map_Iterator__4 *this) {
  return this->node->key;
}

bool std_map_Iterator__4_has_value(std_map_Iterator__4 *this) {
  return ((bool)this->node);
}

ast_nodes_Function *std_map_Iterator__4_value(std_map_Iterator__4 *this) {
  return this->node->value;
}

std_map_Iterator__4 std_map_Iterator__4_make(std_map_Map__4 *map) {
  std_map_Iterator__4 it = (std_map_Iterator__4){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__4_next(&it);
  return it;
}

char *std_map_KeyIterator__4_cur(std_map_KeyIterator__4 *this) {
  return std_map_Iterator__4_cur(&this->map_iter)->key;
}

void std_map_KeyIterator__4_next(std_map_KeyIterator__4 *this) {
  std_map_Iterator__4_next(&this->map_iter);
}

bool std_map_KeyIterator__4_has_value(std_map_KeyIterator__4 *this) {
  return std_map_Iterator__4_has_value(&this->map_iter);
}

bool std_map_Map__4_is_empty(std_map_Map__4 *this) {
  return this->num_items==((u32)0);
}

bool std_map_Map__4_contains(std_map_Map__4 *this, char *key) {
  return ((bool)std_map_Map__4_get_node(this, key));
}

void std_map_Map__4_resize(std_map_Map__4 *this) {
  std_map_Node__4 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_num_items = this->num_items;
  this->num_collisions=((u32)0);
  this->num_buckets*=((u32)2);
  this->buckets=((std_map_Node__4 **)calloc(this->num_buckets, ((u32)sizeof(std_map_Node__4 *))));
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__4 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__4_hash(this, node->key);
      std_map_Node__4 *new_node = std_map_Node__4_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=((u32)1);
      } 
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__4_free_list(old_buckets[i]);
  }
  free(old_buckets);
}

u32 std_map_Map__4_hash(std_map_Map__4 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if ((hash < ((u32)0))) {
    hash+=this->num_buckets;
  } 
  return hash;
}

std_map_ValueIterator__4 std_map_Map__4_iter_values(std_map_Map__4 *this) {
  return (std_map_ValueIterator__4){.map_iter=std_map_Map__4_iter(this)};
}

ast_nodes_Function *std_map_ValueIterator__4_cur(std_map_ValueIterator__4 *this) {
  return std_map_Iterator__4_cur(&this->map_iter)->value;
}

void std_map_ValueIterator__4_next(std_map_ValueIterator__4 *this) {
  std_map_Iterator__4_next(&this->map_iter);
}

bool std_map_ValueIterator__4_has_value(std_map_ValueIterator__4 *this) {
  return std_map_Iterator__4_has_value(&this->map_iter);
}

std_map_Map__4 *std_map_Map__4_new(void) {
  std_map_Map__4 *map = ((std_map_Map__4 *)calloc(((u32)1), ((u32)sizeof(std_map_Map__4))));
  map->num_buckets=((u32)4);
  map->buckets=((std_map_Node__4 **)calloc(map->num_buckets, ((u32)sizeof(std_map_Node__4 *))));
  return map;
}

std_map_Node__4 *std_map_Map__4_get_node(std_map_Map__4 *this, char *key) {
  u32 hash = std_map_Map__4_hash(this, key);
  std_map_Node__4 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    } 
    node=node->next;
  }
  return NULL;
}

std_map_Iterator__4 std_map_Map__4_iter(std_map_Map__4 *this) {
  return std_map_Iterator__4_make(this);
}

void std_map_Node__5_free_list(std_map_Node__5 *this) {
  std_map_Node__5 *cur = this;
  while (((bool)cur)) {
    std_map_Node__5 *next = cur->next;
    free(cur);
    cur=next;
  }
}

std_map_Node__5 *std_map_Node__5_new(char *key, ast_nodes_MatchCase *value, std_map_Node__5 *next) {
  std_map_Node__5 *node = ((std_map_Node__5 *)calloc(((u32)1), ((u32)sizeof(std_map_Node__5))));
  node->key=key;
  node->value=value;
  node->next=next;
  return node;
}

void std_map_Map__5_push_keys(std_map_Map__5 *this, std_vector_Vector__7 *vec) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__5 *node = this->buckets[i];
    while (((bool)node)) {
      std_vector_Vector__7_push(vec, node->key);
      node=node->next;
    }
  }
}

ast_nodes_MatchCase *std_map_Map__5_at(std_map_Map__5 *this, char *key) {
  std_map_Node__5 *node = std_map_Map__5_get_node(this, key);
  ae_assert(((bool)node), ".//std/map.oc:95:12: Assertion failed: `node?`", "Key not found");
  return node->value;
}

void std_map_Map__5_free(std_map_Map__5 *this) {
  for (u32 i = ((u32)0); (i < this->num_buckets); i+=((u32)1)) {
    std_map_Node__5_free_list(this->buckets[i]);
  }
  free(this->buckets);
}

void std_map_Map__5_insert(std_map_Map__5 *this, char *key, ast_nodes_MatchCase *value) {
  std_map_Node__5 *node = std_map_Map__5_get_node(this, key);
  if (((bool)node)) {
    node->value=value;
  }  else {
    u32 hash = std_map_Map__5_hash(this, key);
    std_map_Node__5 *new_node = std_map_Node__5_new(key, value, this->buckets[hash]);
    if (((bool)this->buckets[hash])) {
      this->num_collisions+=((u32)1);
    } 
    this->buckets[hash]=new_node;
    this->num_items+=((u32)1);
    if ((this->num_items > this->num_buckets)) {
      std_map_Map__5_resize(this);
    } 
  } 
}

ast_nodes_MatchCase *std_map_Map__5_get(std_map_Map__5 *this, char *key, ast_nodes_MatchCase *defolt) {
  std_map_Node__5 *node = std_map_Map__5_get_node(this, key);
  if (!((bool)node)) 
  return defolt;
  
  return node->value;
}

void std_map_Map__5_extend(std_map_Map__5 *this, std_map_Map__5 *other) {
  for (std_map_Iterator__5 __iter = std_map_Map__5_iter(other); std_map_Iterator__5_has_value(&__iter); std_map_Iterator__5_next(&__iter)) {
    std_map_Node__5 *iter = std_map_Iterator__5_cur(&__iter);
    {
      std_map_Map__5_insert(this, iter->key, iter->value);
    }
  }
}

void std_map_Map__5_remove(std_map_Map__5 *this, char *key) {
  std_map_Node__5 *node = std_map_Map__5_get_node(this, key);
  if (((bool)node)) {
    u32 hash = std_map_Map__5_hash(this, key);
    if (this->buckets[hash]==node) {
      this->buckets[hash]=node->next;
    }  else {
      std_map_Node__5 *prev = this->buckets[hash];
      while ((prev->next != node)) {
        prev=prev->next;
      }
      prev->next=node->next;
    } 
    free(node);
    this->num_items-=((u32)1);
  } 
}

std_map_KeyIterator__5 std_map_Map__5_iter_keys(std_map_Map__5 *this) {
  return (std_map_KeyIterator__5){.map_iter=std_map_Map__5_iter(this)};
}

std_map_Node__5 *std_map_Iterator__5_cur(std_map_Iterator__5 *this) {
  return this->node;
}

void std_map_Iterator__5_next(std_map_Iterator__5 *this) {
  while ((this->idx < ((i32)this->map->num_buckets))) {
    while (((bool)this->node)) {
      this->node=this->node->next;
      if (((bool)this->node)) 
      return ;
      
    }
    this->idx+=1;
    this->node=({ std_map_Node__5 *__yield_0;
      if ((this->idx < ((i32)this->map->num_buckets))) {
        __yield_0 = this->map->buckets[this->idx];
      }  else {
        __yield_0 = NULL;
      } 
;__yield_0; });
    if (((bool)this->node)) 
    return ;
    
  }
}

char *std_map_Iterator__5_key(std_map_Iterator__5 *this) {
  return this->node->key;
}

bool std_map_Iterator__5_has_value(std_map_Iterator__5 *this) {
  return ((bool)this->node);
}

ast_nodes_MatchCase *std_map_Iterator__5_value(std_map_Iterator__5 *this) {
  return this->node->value;
}

std_map_Iterator__5 std_map_Iterator__5_make(std_map_Map__5 *map) {
  std_map_Iterator__5 it = (std_map_Iterator__5){.idx=-1, .node=NULL, .map=map};
  std_map_Iterator__5_next(&it);
  return it;
}

char *std_map_KeyIterator__5_cur(std_map_KeyIterator__5 *this) {
  return std_map_Iterator__5_cur(&this->map_iter)->key;
}

void std_map_KeyIterator__5_next(std_map_KeyIterator__5 *this) {
  std_map_Iterator__5_next(&this->map_iter);
}

bool std_map_KeyIterator__5_has_value(std_map_KeyIterator__5 *this) {
  return std_map_Iterator__5_has_value(&this->map_iter);
}

bool std_map_Map__5_is_empty(std_map_Map__5 *this) {
  return this->num_items==((u32)0);
}

bool std_map_Map__5_contains(std_map_Map__5 *this, char *key) {
  return ((bool)std_map_Map__5_get_node(this, key));
}

void std_map_Map__5_resize(std_map_Map__5 *this) {
  std_map_Node__5 **old_buckets = this->buckets;
  u32 old_num_buckets = this->num_buckets;
  u32 old_num_items = this->num_items;
  this->num_collisions=((u32)0);
  this->num_buckets*=((u32)2);
  this->buckets=((std_map_Node__5 **)calloc(this->num_buckets, ((u32)sizeof(std_map_Node__5 *))));
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__5 *node = old_buckets[i];
    while (((bool)node)) {
      u32 new_hash = std_map_Map__5_hash(this, node->key);
      std_map_Node__5 *new_node = std_map_Node__5_new(node->key, node->value, this->buckets[new_hash]);
      if (((bool)this->buckets[new_hash])) {
        this->num_collisions+=((u32)1);
      } 
      this->buckets[new_hash]=new_node;
      node=node->next;
    }
  }
  for (u32 i = ((u32)0); (i < old_num_buckets); i+=((u32)1)) {
    std_map_Node__5_free_list(old_buckets[i]);
  }
  free(old_buckets);
}

u32 std_map_Map__5_hash(std_map_Map__5 *this, char *key) {
  u32 hash = str_hash(key);
  hash=(hash % this->num_buckets);
  if ((hash < ((u32)0))) {
    hash+=this->num_buckets;
  } 
  return hash;
}

std_map_ValueIterator__5 std_map_Map__5_iter_values(std_map_Map__5 *this) {
  return (std_map_ValueIterator__5){.map_iter=std_map_Map__5_iter(this)};
}

ast_nodes_MatchCase *std_map_ValueIterator__5_cur(std_map_ValueIterator__5 *this) {
  return std_map_Iterator__5_cur(&this->map_iter)->value;
}

void std_map_ValueIterator__5_next(std_map_ValueIterator__5 *this) {
  std_map_Iterator__5_next(&this->map_iter);
}

bool std_map_ValueIterator__5_has_value(std_map_ValueIterator__5 *this) {
  return std_map_Iterator__5_has_value(&this->map_iter);
}

std_map_Map__5 *std_map_Map__5_new(void) {
  std_map_Map__5 *map = ((std_map_Map__5 *)calloc(((u32)1), ((u32)sizeof(std_map_Map__5))));
  map->num_buckets=((u32)4);
  map->buckets=((std_map_Node__5 **)calloc(map->num_buckets, ((u32)sizeof(std_map_Node__5 *))));
  return map;
}

std_map_Node__5 *std_map_Map__5_get_node(std_map_Map__5 *this, char *key) {
  u32 hash = std_map_Map__5_hash(this, key);
  std_map_Node__5 *node = this->buckets[hash];
  while (((bool)node)) {
    if (str_eq(node->key, key)) {
      return node;
    } 
    node=node->next;
  }
  return NULL;
}

std_map_Iterator__5 std_map_Map__5_iter(std_map_Map__5 *this) {
  return std_map_Iterator__5_make(this);
}

std_span_Location std_span_Location_default(void) {
  std_span_Location loc;
  loc.filename="<default>";
  loc.line=((u32)0);
  loc.col=((u32)0);
  loc.index=((u32)0);
  return loc;
}

char *std_span_Location_str(std_span_Location *this) {
  return format_string("%s:%u:%u", this->filename, this->line, this->col);
}

bool std_span_Location_is_before(std_span_Location *this, std_span_Location other) {
  if ((this->line > other.line)) 
  return false;
  
  if ((this->line < other.line)) 
  return true;
  
  return (this->col <= other.col);
}

char *std_span_Span_str(std_span_Span this) {
  return format_string("%s => %s", std_span_Location_str(&this.start), std_span_Location_str(&this.end));
}

std_span_Span std_span_Span_default(void) {
  std_span_Span span;
  span.start=(std_span_Location){.filename="<default>", .line=((u32)0), .col=((u32)0), .index=((u32)0)};
  span.end=(std_span_Location){.filename="<default>", .line=((u32)0), .col=((u32)0), .index=((u32)0)};
  return span;
}

std_span_Span std_span_Span_join(std_span_Span this, std_span_Span other) {
  std_span_Span span;
  span.start=this.start;
  span.end=other.end;
  return span;
}

bool std_span_Span_contains_loc(std_span_Span this, std_span_Location loc) {
  if (!str_eq(this.start.filename, loc.filename)) 
  return false;
  
  return (std_span_Location_is_before(&this.start, loc) && std_span_Location_is_before(&loc, this.end));
}

bool std_span_Span_starts_right_after(std_span_Span this, std_span_Span other) {
  if (!str_eq(this.start.filename, other.start.filename)) 
  return false;
  
  return this.start.index==other.end.index;
}

bool i32_eq(i32 this, i32 other) {
  return this==other;
}

bool u32_eq(u32 this, u32 other) {
  return this==other;
}

u32 str_hash(char *this) {
  u32 hash = ((u32)5381);
  u32 len = str_len(this);
  for (u32 i = ((u32)0); (i < len); i+=((u32)1)) {
    hash=((hash * ((u32)33)) ^ ((u32)this[i]));
  }
  return hash;
}

u32 i32_hash(i32 this) {
  return (((u32)this) * ((u32)7817));
}

u32 u32_hash(u32 this) {
  return (((u32)this) * ((u32)7817));
}

std_json_Token *std_vector_Vector__0_at(std_vector_Vector__0 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__0_free(std_vector_Vector__0 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__0_extend(std_vector_Vector__0 *this, std_vector_Vector__0 *other) {
  for (std_vector_Iterator__0 __iter = std_vector_Vector__0_iter(other); std_vector_Iterator__0_has_value(&__iter); std_vector_Iterator__0_next(&__iter)) {
    std_json_Token *val = std_vector_Iterator__0_cur(&__iter);
    {
      std_vector_Vector__0_push(this, val);
    }
  }
}

bool std_vector_Vector__0_is_empty(std_vector_Vector__0 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__0_push_front(std_vector_Vector__0 *this, std_json_Token *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__0_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__0_resize(std_vector_Vector__0 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((std_json_Token **)realloc(this->data, (this->capacity * ((u32)sizeof(std_json_Token *)))));
}

std_vector_Vector__0 *std_vector_Vector__0_new(u32 capacity) {
  std_vector_Vector__0 *list = ((std_vector_Vector__0 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__0))));
  list->capacity=capacity;
  list->data=((std_json_Token **)calloc(capacity, ((u32)sizeof(std_json_Token *))));
  list->size=((u32)0);
  return list;
}

std_json_Token *std_vector_Vector__0_pop(std_vector_Vector__0 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__0_push(std_vector_Vector__0 *this, std_json_Token *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__0_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__0_clear(std_vector_Vector__0 *this) {
  this->size=((u32)0);
}

std_json_Token *std_vector_Vector__0_back(std_vector_Vector__0 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__0 std_vector_Vector__0_iter(std_vector_Vector__0 *this) {
  return std_vector_Iterator__0_make(this);
}

std_json_Token *std_vector_Iterator__0_cur(std_vector_Iterator__0 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__0_next(std_vector_Iterator__0 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__0_has_value(std_vector_Iterator__0 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__0 std_vector_Iterator__0_make(std_vector_Vector__0 *vec) {
  return (std_vector_Iterator__0){.vec=vec, .index=((u32)0)};
}

std_value_Value *std_vector_Vector__1_at(std_vector_Vector__1 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__1_free(std_vector_Vector__1 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__1_extend(std_vector_Vector__1 *this, std_vector_Vector__1 *other) {
  for (std_vector_Iterator__1 __iter = std_vector_Vector__1_iter(other); std_vector_Iterator__1_has_value(&__iter); std_vector_Iterator__1_next(&__iter)) {
    std_value_Value *val = std_vector_Iterator__1_cur(&__iter);
    {
      std_vector_Vector__1_push(this, val);
    }
  }
}

bool std_vector_Vector__1_is_empty(std_vector_Vector__1 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__1_push_front(std_vector_Vector__1 *this, std_value_Value *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__1_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__1_resize(std_vector_Vector__1 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((std_value_Value **)realloc(this->data, (this->capacity * ((u32)sizeof(std_value_Value *)))));
}

std_vector_Vector__1 *std_vector_Vector__1_new(u32 capacity) {
  std_vector_Vector__1 *list = ((std_vector_Vector__1 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__1))));
  list->capacity=capacity;
  list->data=((std_value_Value **)calloc(capacity, ((u32)sizeof(std_value_Value *))));
  list->size=((u32)0);
  return list;
}

std_value_Value *std_vector_Vector__1_pop(std_vector_Vector__1 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__1_push(std_vector_Vector__1 *this, std_value_Value *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__1_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__1_clear(std_vector_Vector__1 *this) {
  this->size=((u32)0);
}

std_value_Value *std_vector_Vector__1_back(std_vector_Vector__1 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__1 std_vector_Vector__1_iter(std_vector_Vector__1 *this) {
  return std_vector_Iterator__1_make(this);
}

std_value_Value *std_vector_Iterator__1_cur(std_vector_Iterator__1 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__1_next(std_vector_Iterator__1 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__1_has_value(std_vector_Iterator__1 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__1 std_vector_Iterator__1_make(std_vector_Vector__1 *vec) {
  return (std_vector_Iterator__1){.vec=vec, .index=((u32)0)};
}

std_compact_map_Item__0 std_vector_Vector__2_at(std_vector_Vector__2 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__2_free(std_vector_Vector__2 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__2_extend(std_vector_Vector__2 *this, std_vector_Vector__2 *other) {
  for (std_vector_Iterator__2 __iter = std_vector_Vector__2_iter(other); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
    std_compact_map_Item__0 val = std_vector_Iterator__2_cur(&__iter);
    {
      std_vector_Vector__2_push(this, val);
    }
  }
}

bool std_vector_Vector__2_is_empty(std_vector_Vector__2 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__2_push_front(std_vector_Vector__2 *this, std_compact_map_Item__0 value) {
  if (this->size==this->capacity) {
    std_vector_Vector__2_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__2_resize(std_vector_Vector__2 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((std_compact_map_Item__0 *)realloc(this->data, (this->capacity * ((u32)sizeof(std_compact_map_Item__0)))));
}

std_vector_Vector__2 *std_vector_Vector__2_new(u32 capacity) {
  std_vector_Vector__2 *list = ((std_vector_Vector__2 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__2))));
  list->capacity=capacity;
  list->data=((std_compact_map_Item__0 *)calloc(capacity, ((u32)sizeof(std_compact_map_Item__0))));
  list->size=((u32)0);
  return list;
}

std_compact_map_Item__0 std_vector_Vector__2_pop(std_vector_Vector__2 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__2_push(std_vector_Vector__2 *this, std_compact_map_Item__0 value) {
  if (this->size==this->capacity) {
    std_vector_Vector__2_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__2_clear(std_vector_Vector__2 *this) {
  this->size=((u32)0);
}

std_compact_map_Item__0 std_vector_Vector__2_back(std_vector_Vector__2 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__2 std_vector_Vector__2_iter(std_vector_Vector__2 *this) {
  return std_vector_Iterator__2_make(this);
}

std_compact_map_Item__0 std_vector_Iterator__2_cur(std_vector_Iterator__2 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__2_next(std_vector_Iterator__2 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__2_has_value(std_vector_Iterator__2 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__2 std_vector_Iterator__2_make(std_vector_Vector__2 *vec) {
  return (std_vector_Iterator__2){.vec=vec, .index=((u32)0)};
}

types_Type *std_vector_Vector__3_at(std_vector_Vector__3 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__3_free(std_vector_Vector__3 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__3_extend(std_vector_Vector__3 *this, std_vector_Vector__3 *other) {
  for (std_vector_Iterator__3 __iter = std_vector_Vector__3_iter(other); std_vector_Iterator__3_has_value(&__iter); std_vector_Iterator__3_next(&__iter)) {
    types_Type *val = std_vector_Iterator__3_cur(&__iter);
    {
      std_vector_Vector__3_push(this, val);
    }
  }
}

bool std_vector_Vector__3_is_empty(std_vector_Vector__3 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__3_push_front(std_vector_Vector__3 *this, types_Type *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__3_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__3_resize(std_vector_Vector__3 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((types_Type **)realloc(this->data, (this->capacity * ((u32)sizeof(types_Type *)))));
}

std_vector_Vector__3 *std_vector_Vector__3_new(u32 capacity) {
  std_vector_Vector__3 *list = ((std_vector_Vector__3 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__3))));
  list->capacity=capacity;
  list->data=((types_Type **)calloc(capacity, ((u32)sizeof(types_Type *))));
  list->size=((u32)0);
  return list;
}

types_Type *std_vector_Vector__3_pop(std_vector_Vector__3 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__3_push(std_vector_Vector__3 *this, types_Type *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__3_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__3_clear(std_vector_Vector__3 *this) {
  this->size=((u32)0);
}

types_Type *std_vector_Vector__3_back(std_vector_Vector__3 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__3 std_vector_Vector__3_iter(std_vector_Vector__3 *this) {
  return std_vector_Iterator__3_make(this);
}

types_Type *std_vector_Iterator__3_cur(std_vector_Iterator__3 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__3_next(std_vector_Iterator__3 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__3_has_value(std_vector_Iterator__3 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__3 std_vector_Iterator__3_make(std_vector_Vector__3 *vec) {
  return (std_vector_Iterator__3){.vec=vec, .index=((u32)0)};
}

ast_nodes_Variable *std_vector_Vector__4_at(std_vector_Vector__4 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__4_free(std_vector_Vector__4 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__4_extend(std_vector_Vector__4 *this, std_vector_Vector__4 *other) {
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(other); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *val = std_vector_Iterator__4_cur(&__iter);
    {
      std_vector_Vector__4_push(this, val);
    }
  }
}

bool std_vector_Vector__4_is_empty(std_vector_Vector__4 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__4_push_front(std_vector_Vector__4 *this, ast_nodes_Variable *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__4_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__4_resize(std_vector_Vector__4 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_Variable **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_Variable *)))));
}

std_vector_Vector__4 *std_vector_Vector__4_new(u32 capacity) {
  std_vector_Vector__4 *list = ((std_vector_Vector__4 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__4))));
  list->capacity=capacity;
  list->data=((ast_nodes_Variable **)calloc(capacity, ((u32)sizeof(ast_nodes_Variable *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_Variable *std_vector_Vector__4_pop(std_vector_Vector__4 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__4_push(std_vector_Vector__4 *this, ast_nodes_Variable *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__4_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__4_clear(std_vector_Vector__4 *this) {
  this->size=((u32)0);
}

ast_nodes_Variable *std_vector_Vector__4_back(std_vector_Vector__4 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__4 std_vector_Vector__4_iter(std_vector_Vector__4 *this) {
  return std_vector_Iterator__4_make(this);
}

ast_nodes_Variable *std_vector_Iterator__4_cur(std_vector_Iterator__4 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__4_next(std_vector_Iterator__4 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__4_has_value(std_vector_Iterator__4 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__4 std_vector_Iterator__4_make(std_vector_Vector__4 *vec) {
  return (std_vector_Iterator__4){.vec=vec, .index=((u32)0)};
}

ast_nodes_ImportPart *std_vector_Vector__5_at(std_vector_Vector__5 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__5_free(std_vector_Vector__5 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__5_extend(std_vector_Vector__5 *this, std_vector_Vector__5 *other) {
  for (std_vector_Iterator__5 __iter = std_vector_Vector__5_iter(other); std_vector_Iterator__5_has_value(&__iter); std_vector_Iterator__5_next(&__iter)) {
    ast_nodes_ImportPart *val = std_vector_Iterator__5_cur(&__iter);
    {
      std_vector_Vector__5_push(this, val);
    }
  }
}

bool std_vector_Vector__5_is_empty(std_vector_Vector__5 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__5_push_front(std_vector_Vector__5 *this, ast_nodes_ImportPart *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__5_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__5_resize(std_vector_Vector__5 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_ImportPart **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_ImportPart *)))));
}

std_vector_Vector__5 *std_vector_Vector__5_new(u32 capacity) {
  std_vector_Vector__5 *list = ((std_vector_Vector__5 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__5))));
  list->capacity=capacity;
  list->data=((ast_nodes_ImportPart **)calloc(capacity, ((u32)sizeof(ast_nodes_ImportPart *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_ImportPart *std_vector_Vector__5_pop(std_vector_Vector__5 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__5_push(std_vector_Vector__5 *this, ast_nodes_ImportPart *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__5_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__5_clear(std_vector_Vector__5 *this) {
  this->size=((u32)0);
}

ast_nodes_ImportPart *std_vector_Vector__5_back(std_vector_Vector__5 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__5 std_vector_Vector__5_iter(std_vector_Vector__5 *this) {
  return std_vector_Iterator__5_make(this);
}

ast_nodes_ImportPart *std_vector_Iterator__5_cur(std_vector_Iterator__5 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__5_next(std_vector_Iterator__5 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__5_has_value(std_vector_Iterator__5 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__5 std_vector_Iterator__5_make(std_vector_Vector__5 *vec) {
  return (std_vector_Iterator__5){.vec=vec, .index=((u32)0)};
}

tokens_Token *std_vector_Vector__6_at(std_vector_Vector__6 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__6_free(std_vector_Vector__6 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__6_extend(std_vector_Vector__6 *this, std_vector_Vector__6 *other) {
  for (std_vector_Iterator__6 __iter = std_vector_Vector__6_iter(other); std_vector_Iterator__6_has_value(&__iter); std_vector_Iterator__6_next(&__iter)) {
    tokens_Token *val = std_vector_Iterator__6_cur(&__iter);
    {
      std_vector_Vector__6_push(this, val);
    }
  }
}

bool std_vector_Vector__6_is_empty(std_vector_Vector__6 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__6_push_front(std_vector_Vector__6 *this, tokens_Token *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__6_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__6_resize(std_vector_Vector__6 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((tokens_Token **)realloc(this->data, (this->capacity * ((u32)sizeof(tokens_Token *)))));
}

std_vector_Vector__6 *std_vector_Vector__6_new(u32 capacity) {
  std_vector_Vector__6 *list = ((std_vector_Vector__6 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__6))));
  list->capacity=capacity;
  list->data=((tokens_Token **)calloc(capacity, ((u32)sizeof(tokens_Token *))));
  list->size=((u32)0);
  return list;
}

tokens_Token *std_vector_Vector__6_pop(std_vector_Vector__6 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__6_push(std_vector_Vector__6 *this, tokens_Token *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__6_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__6_clear(std_vector_Vector__6 *this) {
  this->size=((u32)0);
}

tokens_Token *std_vector_Vector__6_back(std_vector_Vector__6 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__6 std_vector_Vector__6_iter(std_vector_Vector__6 *this) {
  return std_vector_Iterator__6_make(this);
}

tokens_Token *std_vector_Iterator__6_cur(std_vector_Iterator__6 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__6_next(std_vector_Iterator__6 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__6_has_value(std_vector_Iterator__6 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__6 std_vector_Iterator__6_make(std_vector_Vector__6 *vec) {
  return (std_vector_Iterator__6){.vec=vec, .index=((u32)0)};
}

char *std_vector_Vector__7_at(std_vector_Vector__7 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__7_free(std_vector_Vector__7 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__7_extend(std_vector_Vector__7 *this, std_vector_Vector__7 *other) {
  for (std_vector_Iterator__7 __iter = std_vector_Vector__7_iter(other); std_vector_Iterator__7_has_value(&__iter); std_vector_Iterator__7_next(&__iter)) {
    char *val = std_vector_Iterator__7_cur(&__iter);
    {
      std_vector_Vector__7_push(this, val);
    }
  }
}

bool std_vector_Vector__7_is_empty(std_vector_Vector__7 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__7_push_front(std_vector_Vector__7 *this, char *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__7_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__7_resize(std_vector_Vector__7 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((char **)realloc(this->data, (this->capacity * ((u32)sizeof(char *)))));
}

std_vector_Vector__7 *std_vector_Vector__7_new(u32 capacity) {
  std_vector_Vector__7 *list = ((std_vector_Vector__7 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__7))));
  list->capacity=capacity;
  list->data=((char **)calloc(capacity, ((u32)sizeof(char *))));
  list->size=((u32)0);
  return list;
}

char *std_vector_Vector__7_pop(std_vector_Vector__7 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__7_push(std_vector_Vector__7 *this, char *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__7_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__7_clear(std_vector_Vector__7 *this) {
  this->size=((u32)0);
}

char *std_vector_Vector__7_back(std_vector_Vector__7 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__7 std_vector_Vector__7_iter(std_vector_Vector__7 *this) {
  return std_vector_Iterator__7_make(this);
}

char *std_vector_Iterator__7_cur(std_vector_Iterator__7 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__7_next(std_vector_Iterator__7 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__7_has_value(std_vector_Iterator__7 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__7 std_vector_Iterator__7_make(std_vector_Vector__7 *vec) {
  return (std_vector_Iterator__7){.vec=vec, .index=((u32)0)};
}

ast_scopes_Scope *std_vector_Vector__8_at(std_vector_Vector__8 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__8_free(std_vector_Vector__8 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__8_extend(std_vector_Vector__8 *this, std_vector_Vector__8 *other) {
  for (std_vector_Iterator__8 __iter = std_vector_Vector__8_iter(other); std_vector_Iterator__8_has_value(&__iter); std_vector_Iterator__8_next(&__iter)) {
    ast_scopes_Scope *val = std_vector_Iterator__8_cur(&__iter);
    {
      std_vector_Vector__8_push(this, val);
    }
  }
}

bool std_vector_Vector__8_is_empty(std_vector_Vector__8 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__8_push_front(std_vector_Vector__8 *this, ast_scopes_Scope *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__8_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__8_resize(std_vector_Vector__8 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_scopes_Scope **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_scopes_Scope *)))));
}

std_vector_Vector__8 *std_vector_Vector__8_new(u32 capacity) {
  std_vector_Vector__8 *list = ((std_vector_Vector__8 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__8))));
  list->capacity=capacity;
  list->data=((ast_scopes_Scope **)calloc(capacity, ((u32)sizeof(ast_scopes_Scope *))));
  list->size=((u32)0);
  return list;
}

ast_scopes_Scope *std_vector_Vector__8_pop(std_vector_Vector__8 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__8_push(std_vector_Vector__8 *this, ast_scopes_Scope *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__8_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__8_clear(std_vector_Vector__8 *this) {
  this->size=((u32)0);
}

ast_scopes_Scope *std_vector_Vector__8_back(std_vector_Vector__8 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__8 std_vector_Vector__8_iter(std_vector_Vector__8 *this) {
  return std_vector_Iterator__8_make(this);
}

ast_scopes_Scope *std_vector_Iterator__8_cur(std_vector_Iterator__8 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__8_next(std_vector_Iterator__8 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__8_has_value(std_vector_Iterator__8 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__8 std_vector_Iterator__8_make(std_vector_Vector__8 *vec) {
  return (std_vector_Iterator__8){.vec=vec, .index=((u32)0)};
}

ast_program_Namespace *std_vector_Vector__9_at(std_vector_Vector__9 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__9_free(std_vector_Vector__9 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__9_extend(std_vector_Vector__9 *this, std_vector_Vector__9 *other) {
  for (std_vector_Iterator__9 __iter = std_vector_Vector__9_iter(other); std_vector_Iterator__9_has_value(&__iter); std_vector_Iterator__9_next(&__iter)) {
    ast_program_Namespace *val = std_vector_Iterator__9_cur(&__iter);
    {
      std_vector_Vector__9_push(this, val);
    }
  }
}

bool std_vector_Vector__9_is_empty(std_vector_Vector__9 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__9_push_front(std_vector_Vector__9 *this, ast_program_Namespace *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__9_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__9_resize(std_vector_Vector__9 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_program_Namespace **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_program_Namespace *)))));
}

std_vector_Vector__9 *std_vector_Vector__9_new(u32 capacity) {
  std_vector_Vector__9 *list = ((std_vector_Vector__9 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__9))));
  list->capacity=capacity;
  list->data=((ast_program_Namespace **)calloc(capacity, ((u32)sizeof(ast_program_Namespace *))));
  list->size=((u32)0);
  return list;
}

ast_program_Namespace *std_vector_Vector__9_pop(std_vector_Vector__9 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__9_push(std_vector_Vector__9 *this, ast_program_Namespace *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__9_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__9_clear(std_vector_Vector__9 *this) {
  this->size=((u32)0);
}

ast_program_Namespace *std_vector_Vector__9_back(std_vector_Vector__9 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__9 std_vector_Vector__9_iter(std_vector_Vector__9 *this) {
  return std_vector_Iterator__9_make(this);
}

ast_program_Namespace *std_vector_Iterator__9_cur(std_vector_Iterator__9 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__9_next(std_vector_Iterator__9 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__9_has_value(std_vector_Iterator__9 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__9 std_vector_Iterator__9_make(std_vector_Vector__9 *vec) {
  return (std_vector_Iterator__9){.vec=vec, .index=((u32)0)};
}

ast_nodes_Function *std_vector_Vector__10_at(std_vector_Vector__10 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__10_free(std_vector_Vector__10 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__10_extend(std_vector_Vector__10 *this, std_vector_Vector__10 *other) {
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(other); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *val = std_vector_Iterator__10_cur(&__iter);
    {
      std_vector_Vector__10_push(this, val);
    }
  }
}

bool std_vector_Vector__10_is_empty(std_vector_Vector__10 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__10_push_front(std_vector_Vector__10 *this, ast_nodes_Function *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__10_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__10_resize(std_vector_Vector__10 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_Function **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_Function *)))));
}

std_vector_Vector__10 *std_vector_Vector__10_new(u32 capacity) {
  std_vector_Vector__10 *list = ((std_vector_Vector__10 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__10))));
  list->capacity=capacity;
  list->data=((ast_nodes_Function **)calloc(capacity, ((u32)sizeof(ast_nodes_Function *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_Function *std_vector_Vector__10_pop(std_vector_Vector__10 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__10_push(std_vector_Vector__10 *this, ast_nodes_Function *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__10_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__10_clear(std_vector_Vector__10 *this) {
  this->size=((u32)0);
}

ast_nodes_Function *std_vector_Vector__10_back(std_vector_Vector__10 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__10 std_vector_Vector__10_iter(std_vector_Vector__10 *this) {
  return std_vector_Iterator__10_make(this);
}

ast_nodes_Function *std_vector_Iterator__10_cur(std_vector_Iterator__10 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__10_next(std_vector_Iterator__10 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__10_has_value(std_vector_Iterator__10 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__10 std_vector_Iterator__10_make(std_vector_Vector__10 *vec) {
  return (std_vector_Iterator__10){.vec=vec, .index=((u32)0)};
}

ast_nodes_Structure *std_vector_Vector__11_at(std_vector_Vector__11 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__11_free(std_vector_Vector__11 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__11_extend(std_vector_Vector__11 *this, std_vector_Vector__11 *other) {
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(other); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *val = std_vector_Iterator__11_cur(&__iter);
    {
      std_vector_Vector__11_push(this, val);
    }
  }
}

bool std_vector_Vector__11_is_empty(std_vector_Vector__11 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__11_push_front(std_vector_Vector__11 *this, ast_nodes_Structure *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__11_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__11_resize(std_vector_Vector__11 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_Structure **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_Structure *)))));
}

std_vector_Vector__11 *std_vector_Vector__11_new(u32 capacity) {
  std_vector_Vector__11 *list = ((std_vector_Vector__11 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__11))));
  list->capacity=capacity;
  list->data=((ast_nodes_Structure **)calloc(capacity, ((u32)sizeof(ast_nodes_Structure *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_Structure *std_vector_Vector__11_pop(std_vector_Vector__11 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__11_push(std_vector_Vector__11 *this, ast_nodes_Structure *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__11_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__11_clear(std_vector_Vector__11 *this) {
  this->size=((u32)0);
}

ast_nodes_Structure *std_vector_Vector__11_back(std_vector_Vector__11 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__11 std_vector_Vector__11_iter(std_vector_Vector__11 *this) {
  return std_vector_Iterator__11_make(this);
}

ast_nodes_Structure *std_vector_Iterator__11_cur(std_vector_Iterator__11 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__11_next(std_vector_Iterator__11 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__11_has_value(std_vector_Iterator__11 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__11 std_vector_Iterator__11_make(std_vector_Vector__11 *vec) {
  return (std_vector_Iterator__11){.vec=vec, .index=((u32)0)};
}

ast_nodes_AST *std_vector_Vector__12_at(std_vector_Vector__12 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__12_free(std_vector_Vector__12 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__12_extend(std_vector_Vector__12 *this, std_vector_Vector__12 *other) {
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(other); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *val = std_vector_Iterator__12_cur(&__iter);
    {
      std_vector_Vector__12_push(this, val);
    }
  }
}

bool std_vector_Vector__12_is_empty(std_vector_Vector__12 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__12_push_front(std_vector_Vector__12 *this, ast_nodes_AST *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__12_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__12_resize(std_vector_Vector__12 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_AST **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_AST *)))));
}

std_vector_Vector__12 *std_vector_Vector__12_new(u32 capacity) {
  std_vector_Vector__12 *list = ((std_vector_Vector__12 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__12))));
  list->capacity=capacity;
  list->data=((ast_nodes_AST **)calloc(capacity, ((u32)sizeof(ast_nodes_AST *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_AST *std_vector_Vector__12_pop(std_vector_Vector__12 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__12_push(std_vector_Vector__12 *this, ast_nodes_AST *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__12_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__12_clear(std_vector_Vector__12 *this) {
  this->size=((u32)0);
}

ast_nodes_AST *std_vector_Vector__12_back(std_vector_Vector__12 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__12 std_vector_Vector__12_iter(std_vector_Vector__12 *this) {
  return std_vector_Iterator__12_make(this);
}

ast_nodes_AST *std_vector_Iterator__12_cur(std_vector_Iterator__12 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__12_next(std_vector_Iterator__12 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__12_has_value(std_vector_Iterator__12 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__12 std_vector_Iterator__12_make(std_vector_Vector__12 *vec) {
  return (std_vector_Iterator__12){.vec=vec, .index=((u32)0)};
}

ast_nodes_Enum *std_vector_Vector__13_at(std_vector_Vector__13 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__13_free(std_vector_Vector__13 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__13_extend(std_vector_Vector__13 *this, std_vector_Vector__13 *other) {
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(other); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    ast_nodes_Enum *val = std_vector_Iterator__13_cur(&__iter);
    {
      std_vector_Vector__13_push(this, val);
    }
  }
}

bool std_vector_Vector__13_is_empty(std_vector_Vector__13 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__13_push_front(std_vector_Vector__13 *this, ast_nodes_Enum *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__13_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__13_resize(std_vector_Vector__13 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_Enum **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_Enum *)))));
}

std_vector_Vector__13 *std_vector_Vector__13_new(u32 capacity) {
  std_vector_Vector__13 *list = ((std_vector_Vector__13 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__13))));
  list->capacity=capacity;
  list->data=((ast_nodes_Enum **)calloc(capacity, ((u32)sizeof(ast_nodes_Enum *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_Enum *std_vector_Vector__13_pop(std_vector_Vector__13 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__13_push(std_vector_Vector__13 *this, ast_nodes_Enum *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__13_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__13_clear(std_vector_Vector__13 *this) {
  this->size=((u32)0);
}

ast_nodes_Enum *std_vector_Vector__13_back(std_vector_Vector__13 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__13 std_vector_Vector__13_iter(std_vector_Vector__13 *this) {
  return std_vector_Iterator__13_make(this);
}

ast_nodes_Enum *std_vector_Iterator__13_cur(std_vector_Iterator__13 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__13_next(std_vector_Iterator__13 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__13_has_value(std_vector_Iterator__13 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__13 std_vector_Iterator__13_make(std_vector_Vector__13 *vec) {
  return (std_vector_Iterator__13){.vec=vec, .index=((u32)0)};
}

errors_Error *std_vector_Vector__14_at(std_vector_Vector__14 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__14_free(std_vector_Vector__14 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__14_extend(std_vector_Vector__14 *this, std_vector_Vector__14 *other) {
  for (std_vector_Iterator__14 __iter = std_vector_Vector__14_iter(other); std_vector_Iterator__14_has_value(&__iter); std_vector_Iterator__14_next(&__iter)) {
    errors_Error *val = std_vector_Iterator__14_cur(&__iter);
    {
      std_vector_Vector__14_push(this, val);
    }
  }
}

bool std_vector_Vector__14_is_empty(std_vector_Vector__14 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__14_push_front(std_vector_Vector__14 *this, errors_Error *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__14_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__14_resize(std_vector_Vector__14 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((errors_Error **)realloc(this->data, (this->capacity * ((u32)sizeof(errors_Error *)))));
}

std_vector_Vector__14 *std_vector_Vector__14_new(u32 capacity) {
  std_vector_Vector__14 *list = ((std_vector_Vector__14 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__14))));
  list->capacity=capacity;
  list->data=((errors_Error **)calloc(capacity, ((u32)sizeof(errors_Error *))));
  list->size=((u32)0);
  return list;
}

errors_Error *std_vector_Vector__14_pop(std_vector_Vector__14 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__14_push(std_vector_Vector__14 *this, errors_Error *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__14_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__14_clear(std_vector_Vector__14 *this) {
  this->size=((u32)0);
}

errors_Error *std_vector_Vector__14_back(std_vector_Vector__14 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__14 std_vector_Vector__14_iter(std_vector_Vector__14 *this) {
  return std_vector_Iterator__14_make(this);
}

errors_Error *std_vector_Iterator__14_cur(std_vector_Iterator__14 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__14_next(std_vector_Iterator__14 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__14_has_value(std_vector_Iterator__14 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__14 std_vector_Iterator__14_make(std_vector_Vector__14 *vec) {
  return (std_vector_Iterator__14){.vec=vec, .index=((u32)0)};
}

ast_nodes_TemplateInstance *std_vector_Vector__15_at(std_vector_Vector__15 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__15_free(std_vector_Vector__15 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__15_extend(std_vector_Vector__15 *this, std_vector_Vector__15 *other) {
  for (std_vector_Iterator__15 __iter = std_vector_Vector__15_iter(other); std_vector_Iterator__15_has_value(&__iter); std_vector_Iterator__15_next(&__iter)) {
    ast_nodes_TemplateInstance *val = std_vector_Iterator__15_cur(&__iter);
    {
      std_vector_Vector__15_push(this, val);
    }
  }
}

bool std_vector_Vector__15_is_empty(std_vector_Vector__15 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__15_push_front(std_vector_Vector__15 *this, ast_nodes_TemplateInstance *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__15_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__15_resize(std_vector_Vector__15 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_TemplateInstance **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_TemplateInstance *)))));
}

std_vector_Vector__15 *std_vector_Vector__15_new(u32 capacity) {
  std_vector_Vector__15 *list = ((std_vector_Vector__15 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__15))));
  list->capacity=capacity;
  list->data=((ast_nodes_TemplateInstance **)calloc(capacity, ((u32)sizeof(ast_nodes_TemplateInstance *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_TemplateInstance *std_vector_Vector__15_pop(std_vector_Vector__15 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__15_push(std_vector_Vector__15 *this, ast_nodes_TemplateInstance *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__15_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__15_clear(std_vector_Vector__15 *this) {
  this->size=((u32)0);
}

ast_nodes_TemplateInstance *std_vector_Vector__15_back(std_vector_Vector__15 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__15 std_vector_Vector__15_iter(std_vector_Vector__15 *this) {
  return std_vector_Iterator__15_make(this);
}

ast_nodes_TemplateInstance *std_vector_Iterator__15_cur(std_vector_Iterator__15 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__15_next(std_vector_Iterator__15 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__15_has_value(std_vector_Iterator__15 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__15 std_vector_Iterator__15_make(std_vector_Vector__15 *vec) {
  return (std_vector_Iterator__15){.vec=vec, .index=((u32)0)};
}

ast_nodes_Argument *std_vector_Vector__16_at(std_vector_Vector__16 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__16_free(std_vector_Vector__16 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__16_extend(std_vector_Vector__16 *this, std_vector_Vector__16 *other) {
  for (std_vector_Iterator__16 __iter = std_vector_Vector__16_iter(other); std_vector_Iterator__16_has_value(&__iter); std_vector_Iterator__16_next(&__iter)) {
    ast_nodes_Argument *val = std_vector_Iterator__16_cur(&__iter);
    {
      std_vector_Vector__16_push(this, val);
    }
  }
}

bool std_vector_Vector__16_is_empty(std_vector_Vector__16 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__16_push_front(std_vector_Vector__16 *this, ast_nodes_Argument *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__16_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__16_resize(std_vector_Vector__16 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_Argument **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_Argument *)))));
}

std_vector_Vector__16 *std_vector_Vector__16_new(u32 capacity) {
  std_vector_Vector__16 *list = ((std_vector_Vector__16 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__16))));
  list->capacity=capacity;
  list->data=((ast_nodes_Argument **)calloc(capacity, ((u32)sizeof(ast_nodes_Argument *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_Argument *std_vector_Vector__16_pop(std_vector_Vector__16 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__16_push(std_vector_Vector__16 *this, ast_nodes_Argument *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__16_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__16_clear(std_vector_Vector__16 *this) {
  this->size=((u32)0);
}

ast_nodes_Argument *std_vector_Vector__16_back(std_vector_Vector__16 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__16 std_vector_Vector__16_iter(std_vector_Vector__16 *this) {
  return std_vector_Iterator__16_make(this);
}

ast_nodes_Argument *std_vector_Iterator__16_cur(std_vector_Iterator__16 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__16_next(std_vector_Iterator__16 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__16_has_value(std_vector_Iterator__16 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__16 std_vector_Iterator__16_make(std_vector_Vector__16 *vec) {
  return (std_vector_Iterator__16){.vec=vec, .index=((u32)0)};
}

std_vector_Vector__5 *std_vector_Vector__17_at(std_vector_Vector__17 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__17_free(std_vector_Vector__17 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__17_extend(std_vector_Vector__17 *this, std_vector_Vector__17 *other) {
  for (std_vector_Iterator__17 __iter = std_vector_Vector__17_iter(other); std_vector_Iterator__17_has_value(&__iter); std_vector_Iterator__17_next(&__iter)) {
    std_vector_Vector__5 *val = std_vector_Iterator__17_cur(&__iter);
    {
      std_vector_Vector__17_push(this, val);
    }
  }
}

bool std_vector_Vector__17_is_empty(std_vector_Vector__17 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__17_push_front(std_vector_Vector__17 *this, std_vector_Vector__5 *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__17_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__17_resize(std_vector_Vector__17 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((std_vector_Vector__5 **)realloc(this->data, (this->capacity * ((u32)sizeof(std_vector_Vector__5 *)))));
}

std_vector_Vector__17 *std_vector_Vector__17_new(u32 capacity) {
  std_vector_Vector__17 *list = ((std_vector_Vector__17 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__17))));
  list->capacity=capacity;
  list->data=((std_vector_Vector__5 **)calloc(capacity, ((u32)sizeof(std_vector_Vector__5 *))));
  list->size=((u32)0);
  return list;
}

std_vector_Vector__5 *std_vector_Vector__17_pop(std_vector_Vector__17 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__17_push(std_vector_Vector__17 *this, std_vector_Vector__5 *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__17_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__17_clear(std_vector_Vector__17 *this) {
  this->size=((u32)0);
}

std_vector_Vector__5 *std_vector_Vector__17_back(std_vector_Vector__17 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__17 std_vector_Vector__17_iter(std_vector_Vector__17 *this) {
  return std_vector_Iterator__17_make(this);
}

std_vector_Vector__5 *std_vector_Iterator__17_cur(std_vector_Iterator__17 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__17_next(std_vector_Iterator__17 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__17_has_value(std_vector_Iterator__17 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__17 std_vector_Iterator__17_make(std_vector_Vector__17 *vec) {
  return (std_vector_Iterator__17){.vec=vec, .index=((u32)0)};
}

ast_nodes_MatchCase *std_vector_Vector__18_at(std_vector_Vector__18 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__18_free(std_vector_Vector__18 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__18_extend(std_vector_Vector__18 *this, std_vector_Vector__18 *other) {
  for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(other); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
    ast_nodes_MatchCase *val = std_vector_Iterator__18_cur(&__iter);
    {
      std_vector_Vector__18_push(this, val);
    }
  }
}

bool std_vector_Vector__18_is_empty(std_vector_Vector__18 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__18_push_front(std_vector_Vector__18 *this, ast_nodes_MatchCase *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__18_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__18_resize(std_vector_Vector__18 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((ast_nodes_MatchCase **)realloc(this->data, (this->capacity * ((u32)sizeof(ast_nodes_MatchCase *)))));
}

std_vector_Vector__18 *std_vector_Vector__18_new(u32 capacity) {
  std_vector_Vector__18 *list = ((std_vector_Vector__18 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__18))));
  list->capacity=capacity;
  list->data=((ast_nodes_MatchCase **)calloc(capacity, ((u32)sizeof(ast_nodes_MatchCase *))));
  list->size=((u32)0);
  return list;
}

ast_nodes_MatchCase *std_vector_Vector__18_pop(std_vector_Vector__18 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__18_push(std_vector_Vector__18 *this, ast_nodes_MatchCase *value) {
  if (this->size==this->capacity) {
    std_vector_Vector__18_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__18_clear(std_vector_Vector__18 *this) {
  this->size=((u32)0);
}

ast_nodes_MatchCase *std_vector_Vector__18_back(std_vector_Vector__18 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__18 std_vector_Vector__18_iter(std_vector_Vector__18 *this) {
  return std_vector_Iterator__18_make(this);
}

ast_nodes_MatchCase *std_vector_Iterator__18_cur(std_vector_Iterator__18 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__18_next(std_vector_Iterator__18 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__18_has_value(std_vector_Iterator__18 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__18 std_vector_Iterator__18_make(std_vector_Vector__18 *vec) {
  return (std_vector_Iterator__18){.vec=vec, .index=((u32)0)};
}

u32 std_vector_Vector__19_at(std_vector_Vector__19 *this, u32 i) {
  ae_assert((i < this->size), ".//std/vector.oc:70:12: Assertion failed: `i < .size`", "Out of bounds in Vector::at");
  return this->data[i];
}

void std_vector_Vector__19_free(std_vector_Vector__19 *this) {
  free(this->data);
  free(this);
}

void std_vector_Vector__19_extend(std_vector_Vector__19 *this, std_vector_Vector__19 *other) {
  for (std_vector_Iterator__19 __iter = std_vector_Vector__19_iter(other); std_vector_Iterator__19_has_value(&__iter); std_vector_Iterator__19_next(&__iter)) {
    u32 val = std_vector_Iterator__19_cur(&__iter);
    {
      std_vector_Vector__19_push(this, val);
    }
  }
}

bool std_vector_Vector__19_is_empty(std_vector_Vector__19 *this) {
  return this->size==((u32)0);
}

void std_vector_Vector__19_push_front(std_vector_Vector__19 *this, u32 value) {
  if (this->size==this->capacity) {
    std_vector_Vector__19_resize(this, (this->capacity * ((u32)2)));
  } 
  for (u32 i = this->size; (i > ((u32)0)); i-=((u32)1)) {
    this->data[i]=this->data[(i - ((u32)1))];
  }
  this->data[((u32)0)]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__19_resize(std_vector_Vector__19 *this, u32 new_capacity) {
  this->capacity=new_capacity;
  this->data=((u32 *)realloc(this->data, (this->capacity * ((u32)sizeof(u32)))));
}

std_vector_Vector__19 *std_vector_Vector__19_new(u32 capacity) {
  std_vector_Vector__19 *list = ((std_vector_Vector__19 *)calloc(((u32)1), ((u32)sizeof(std_vector_Vector__19))));
  list->capacity=capacity;
  list->data=((u32 *)calloc(capacity, ((u32)sizeof(u32))));
  list->size=((u32)0);
  return list;
}

u32 std_vector_Vector__19_pop(std_vector_Vector__19 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:50:12: Assertion failed: `.size > 0`", "Empty vector in Vector::pop");
  this->size-=((u32)1);
  return this->data[this->size];
}

void std_vector_Vector__19_push(std_vector_Vector__19 *this, u32 value) {
  if (this->size==this->capacity) {
    std_vector_Vector__19_resize(this, (this->capacity * ((u32)2)));
  } 
  this->data[this->size]=value;
  this->size+=((u32)1);
}

void std_vector_Vector__19_clear(std_vector_Vector__19 *this) {
  this->size=((u32)0);
}

u32 std_vector_Vector__19_back(std_vector_Vector__19 *this) {
  ae_assert((this->size > ((u32)0)), ".//std/vector.oc:64:12: Assertion failed: `.size > 0`", "Empty vector in Vector::back");
  return this->data[(this->size - ((u32)1))];
}

std_vector_Iterator__19 std_vector_Vector__19_iter(std_vector_Vector__19 *this) {
  return std_vector_Iterator__19_make(this);
}

u32 std_vector_Iterator__19_cur(std_vector_Iterator__19 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:105:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::current");
  return this->vec->data[this->index];
}

void std_vector_Iterator__19_next(std_vector_Iterator__19 *this) {
  ae_assert((this->index < this->vec->size), ".//std/vector.oc:100:12: Assertion failed: `.index < .vec.size`", "Out of bounds in Iterator::next");
  this->index+=((u32)1);
}

bool std_vector_Iterator__19_has_value(std_vector_Iterator__19 *this) {
  return (this->index < this->vec->size);
}

std_vector_Iterator__19 std_vector_Iterator__19_make(std_vector_Vector__19 *vec) {
  return (std_vector_Iterator__19){.vec=vec, .index=((u32)0)};
}

std_json_Parser std_json_Parser_make(std_vector_Vector__0 *tokens) {
  std_json_Parser parser;
  parser.tokens=tokens;
  parser.curr=((u32)0);
  return parser;
}

std_json_Token *std_json_Parser_token(std_json_Parser *this) {
  return std_vector_Vector__0_at(this->tokens, this->curr);
}

std_json_Token *std_json_Parser_consume(std_json_Parser *this, std_json_TokenType type) {
  if ((std_json_Parser_token(this)->type != type)) {
    printf("Expected %s but got %s""\n", std_json_TokenType_str(type), std_json_TokenType_str(std_json_Parser_token(this)->type));
    exit(1);
  } 
  std_json_Token *tok = std_json_Parser_token(this);
  this->curr+=((u32)1);
  return tok;
}

std_value_Value *std_json_Parser_parse_object(std_json_Parser *this) {
  std_json_Parser_consume(this, std_json_TokenType_OpenCurly);
  std_value_Value *json = std_value_Value_new(std_value_ValueType_Dictionary);
  while ((std_json_Parser_token(this)->type != std_json_TokenType_CloseCurly)) {
    std_json_Token *key = std_json_Parser_consume(this, std_json_TokenType_StringLiteral);
    std_json_Parser_consume(this, std_json_TokenType_Colon);
    std_value_Value *value = std_json_Parser_parse_value(this);
    std_compact_map_Map__0_insert(json->u.as_dict, key->text, value);
    if (std_json_Parser_token(this)->type==std_json_TokenType_Comma) {
      std_json_Parser_consume(this, std_json_TokenType_Comma);
    } 
  }
  std_json_Parser_consume(this, std_json_TokenType_CloseCurly);
  return json;
}

std_value_Value *std_json_Parser_parse_array(std_json_Parser *this) {
  std_json_Parser_consume(this, std_json_TokenType_OpenSquare);
  std_value_Value *json = std_value_Value_new(std_value_ValueType_List);
  while ((std_json_Parser_token(this)->type != std_json_TokenType_CloseSquare)) {
    std_value_Value *value = std_json_Parser_parse_value(this);
    std_vector_Vector__1_push(json->u.as_list, value);
    if (std_json_Parser_token(this)->type==std_json_TokenType_Comma) {
      std_json_Parser_consume(this, std_json_TokenType_Comma);
    } 
  }
  std_json_Parser_consume(this, std_json_TokenType_CloseSquare);
  return json;
}

std_value_Value *std_json_Parser_parse_value(std_json_Parser *this) {
  return ({ std_value_Value *__yield_0;
    switch (std_json_Parser_token(this)->type) {
      case std_json_TokenType_Null: {
        std_json_Parser_consume(this, std_json_TokenType_Null);
        __yield_0 = std_value_Value_new(std_value_ValueType_Null);
      } break;
      case std_json_TokenType_True:
      case std_json_TokenType_False: {
        std_value_Value *json = std_value_Value_new(std_value_ValueType_Bool);
        std_json_Token *tok = std_json_Parser_token(this);
        json->u.as_bool=str_eq(tok->text, "true");
        this->curr+=((u32)1);
        __yield_0 = json;
      } break;
      case std_json_TokenType_IntLiteral: {
        std_value_Value *json = std_value_Value_new(std_value_ValueType_Integer);
        std_json_Token *tok = std_json_Parser_consume(this, std_json_TokenType_IntLiteral);
        json->u.as_num=((i64)atoi(tok->text));
        __yield_0 = json;
      } break;
      case std_json_TokenType_StringLiteral: {
        std_value_Value *json = std_value_Value_new(std_value_ValueType_String);
        json->u.as_str=std_buffer_Buffer_from_str(std_json_Parser_consume(this, std_json_TokenType_StringLiteral)->text);
        __yield_0 = json;
      } break;
      case std_json_TokenType_OpenCurly: {
        __yield_0 = std_json_Parser_parse_object(this);
      } break;
      case std_json_TokenType_OpenSquare: {
        __yield_0 = std_json_Parser_parse_array(this);
      } break;
      default: {
        printf("Unexpected token in json::Parser::parse_value: %s""\n", std_json_TokenType_str(std_json_Parser_token(this)->type));
        exit(1);
      } break;
    }
;__yield_0; });
}

std_value_Value *std_json_Parser_parse(std_json_Parser *this) {
  return ({ std_value_Value *__yield_0;
    switch (std_json_Parser_token(this)->type) {
      case std_json_TokenType_OpenCurly: {
        __yield_0 = std_json_Parser_parse_object(this);
      } break;
      case std_json_TokenType_OpenSquare: {
        __yield_0 = std_json_Parser_parse_array(this);
      } break;
      default: {
        printf("Expected { or [ at JSON top level""\n");
        exit(1);
      } break;
    }
;__yield_0; });
}

std_value_Value *std_json_parse(char *source, char *filename) {
  std_json_Lexer lexer = std_json_Lexer_make(source, filename);
  std_vector_Vector__0 *tokens = std_json_Lexer_lex(&lexer);
  std_json_Parser parser = std_json_Parser_make(tokens);
  return std_json_Parser_parse(&parser);
}

std_value_Value *std_json_parse_from_file(char *filename) {
  FILE *file = FILE_open(filename, "r");
  char *source = FILE_slurp(file);
  fclose(file);
  return std_json_parse(source, filename);
}

void std_json_serialize_into(std_value_Value *val, std_buffer_Buffer *sb) {
  switch (val->type) {
    case std_value_ValueType_Null: {
      std_buffer_Buffer_puts(sb, "null");
    } break;
    case std_value_ValueType_Bool: {
      std_buffer_Buffer_puts(sb, (val->u.as_bool ? "true" : "false"));
    } break;
    case std_value_ValueType_Integer: {
      std_buffer_Buffer_putsf(sb, format_string("%lld", val->u.as_num));
    } break;
    case std_value_ValueType_String: {
      std_buffer_Buffer_puts(sb, "\"");
      std_buffer_Buffer buf = val->u.as_str;
      for (u32 i = ((u32)0); (i < buf.size); i+=((u32)1)) {
        char c = ((char)buf.data[i]);
        switch (c) {
          case '\b': {
            std_buffer_Buffer_puts(sb, "\\b");
          } break;
          case '\f': {
            std_buffer_Buffer_puts(sb, "\\f");
          } break;
          case '\n': {
            std_buffer_Buffer_puts(sb, "\\n");
          } break;
          case '\r': {
            std_buffer_Buffer_puts(sb, "\\r");
          } break;
          case '\t': {
            std_buffer_Buffer_puts(sb, "\\t");
          } break;
          case '\\': {
            std_buffer_Buffer_puts(sb, "\\\\");
          } break;
          case '"': {
            std_buffer_Buffer_puts(sb, "\\\"");
          } break;
          default: {
            if (char_is_print(c)) {
              std_buffer_Buffer_putc(sb, c);
            }  else {
              std_buffer_Buffer_putsf(sb, format_string("\\x%02x", buf.data[i]));
            } 
          } break;
        }
      }
      std_buffer_Buffer_puts(sb, "\"");
    } break;
    case std_value_ValueType_List: {
      std_buffer_Buffer_puts(sb, "[");
      std_vector_Vector__1 *lst = val->u.as_list;
      for (u32 i = ((u32)0); (i < lst->size); i+=((u32)1)) {
        std_value_Value *value = std_vector_Vector__1_at(lst, i);
        if ((i > ((u32)0))) {
          std_buffer_Buffer_puts(sb, ", ");
        } 
        std_json_serialize_into(value, sb);
      }
      std_buffer_Buffer_puts(sb, "]");
    } break;
    case std_value_ValueType_Dictionary: {
      std_buffer_Buffer_puts(sb, "{");
      bool first = true;
      for (std_vector_Iterator__2 __iter = std_compact_map_Map__0_iter(val->u.as_dict); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
        std_compact_map_Item__0 iter = std_vector_Iterator__2_cur(&__iter);
        {
          if (!first) {
            std_buffer_Buffer_puts(sb, ", ");
          } 
          first=false;
          std_buffer_Buffer_puts(sb, "\"");
          std_buffer_Buffer_puts(sb, iter.key);
          std_buffer_Buffer_puts(sb, "\": ");
          std_value_Value *value = iter.value;
          std_json_serialize_into(value, sb);
        }
      }
      std_buffer_Buffer_puts(sb, "}");
    } break;
  }
}

std_buffer_Buffer std_json_serialize(std_value_Value *val) {
  std_buffer_Buffer sb = std_buffer_Buffer_make(((u32)16));
  std_json_serialize_into(val, &sb);
  return sb;
}

void std_json_write_to_file(std_value_Value *val, char *filename) {
  FILE *file = FILE_open(filename, "w");
  std_buffer_Buffer sb = std_json_serialize(val);
  FILE_write(file, sb.data, sb.size);
  std_buffer_Buffer_free(&sb);
  fclose(file);
}

std_json_Lexer std_json_Lexer_make(char *source, char *filename) {
  return (std_json_Lexer){.source=source, .source_len=str_len(source), .i=((u32)0), .loc=(std_span_Location){.filename=filename, .line=((u32)1), .col=((u32)1), .index=((u32)0)}, .tokens=std_vector_Vector__0_new(((u32)16))};
}

void std_json_Lexer_push(std_json_Lexer *this, std_json_Token *token) {
  std_vector_Vector__0_push(this->tokens, token);
}

void std_json_Lexer_push_type(std_json_Lexer *this, std_json_TokenType type, u32 len) {
  std_span_Location start_loc = this->loc;
  for (u32 i = ((u32)0); (i < len); i+=((u32)1)) {
    std_json_Lexer_inc(this);
  }
  std_json_Lexer_push(this, std_json_Token_from_type(type, (std_span_Span){.start=start_loc, .end=this->loc}));
}

char std_json_Lexer_cur(std_json_Lexer *this) {
  return this->source[this->i];
}

void std_json_Lexer_inc(std_json_Lexer *this) {
  switch (std_json_Lexer_cur(this)) {
    case '\n': {
      this->loc.line+=((u32)1);
      this->loc.col=((u32)1);
    } break;
    default: {
      this->loc.col+=((u32)1);
    } break;
  }
  this->i+=((u32)1);
  this->loc.index+=((u32)1);
}

char std_json_Lexer_peek(std_json_Lexer *this, i32 offset) {
  if (std_json_Lexer_cur(this)=='\0') {
    return std_json_Lexer_cur(this);
  } 
  return this->source[(this->i + ((u32)1))];
}

void std_json_Lexer_error(std_json_Lexer *this, char *msg) {
  printf("%s Error: %s\n", std_span_Location_str(&this->loc), msg);
  exit(1);
}

void std_json_Lexer_lex_string_literal(std_json_Lexer *this) {
  std_span_Location start_loc = this->loc;
  char end_char = std_json_Lexer_cur(this);
  u32 start = (this->i + ((u32)1));
  std_json_Lexer_inc(this);
  while (((this->i < this->source_len) && (std_json_Lexer_cur(this) != end_char))) {
    if (std_json_Lexer_cur(this)=='\\') {
      std_json_Lexer_inc(this);
    } 
    std_json_Lexer_inc(this);
  }
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  std_json_Lexer_inc(this);
  if ((this->i >= this->source_len)) {
    std_json_Lexer_error(this, "Unterminated string literal");
  } 
  std_json_Lexer_push(this, std_json_Token_new(std_json_TokenType_StringLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text));
}

void std_json_Lexer_lex_numeric_literal(std_json_Lexer *this) {
  std_span_Location start_loc = this->loc;
  u32 start = this->i;
  std_json_TokenType token_type;
  while (char_is_digit(std_json_Lexer_cur(this))) {
    std_json_Lexer_inc(this);
  }
  if (std_json_Lexer_cur(this)=='.') {
    std_json_Lexer_inc(this);
    while (char_is_digit(std_json_Lexer_cur(this))) {
      std_json_Lexer_inc(this);
    }
    token_type=std_json_TokenType_FloatLiteral;
  }  else {
    token_type=std_json_TokenType_IntLiteral;
  } 
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  std_json_Token *token = std_json_Token_new(token_type, (std_span_Span){.start=start_loc, .end=this->loc}, text);
  std_json_Lexer_push(this, token);
}

std_vector_Vector__0 *std_json_Lexer_lex(std_json_Lexer *this) {
  while ((this->i < this->source_len)) {
    char c = std_json_Lexer_cur(this);
    switch (c) {
      case ' ':
      case '\t':
      case '\v':
      case '\r':
      case '\b':
      case '\n': {
        std_json_Lexer_inc(this);
      } break;
      case ',': {
        std_json_Lexer_push_type(this, std_json_TokenType_Comma, ((u32)1));
      } break;
      case '[': {
        std_json_Lexer_push_type(this, std_json_TokenType_OpenSquare, ((u32)1));
      } break;
      case ']': {
        std_json_Lexer_push_type(this, std_json_TokenType_CloseSquare, ((u32)1));
      } break;
      case '{': {
        std_json_Lexer_push_type(this, std_json_TokenType_OpenCurly, ((u32)1));
      } break;
      case '}': {
        std_json_Lexer_push_type(this, std_json_TokenType_CloseCurly, ((u32)1));
      } break;
      case ':': {
        std_json_Lexer_push_type(this, std_json_TokenType_Colon, ((u32)1));
      } break;
      case '"': {
        std_json_Lexer_lex_string_literal(this);
      } break;
      default: {
        std_span_Location start_loc = this->loc;
        if (char_is_digit(c)) {
          std_json_Lexer_lex_numeric_literal(this);
        }  else         if ((char_is_alpha(c) || c=='_')) {
          u32 start = this->i;
          while ((char_is_alnum(std_json_Lexer_cur(this)) || std_json_Lexer_cur(this)=='_')) {
            std_json_Lexer_inc(this);
          }
          u32 len = (this->i - start);
          char *text = str_substring(this->source, start, len);
          std_json_Lexer_push(this, std_json_Token_from_ident(text, (std_span_Span){.start=start_loc, .end=this->loc}));
        }  else {
          std_json_Lexer_error(this, format_string("Unrecognized char in lexer: '%c'", c));
          std_json_Lexer_inc(this);
        } 
        
      } break;
    }
  }
  std_json_Lexer_push_type(this, std_json_TokenType_EOF, ((u32)0));
  return this->tokens;
}

std_json_Token *std_json_Token_new(std_json_TokenType type, std_span_Span span, char *text) {
  std_json_Token *tok = ((std_json_Token *)calloc(((u32)1), ((u32)sizeof(std_json_Token))));
  *tok=(std_json_Token){.type=type, .span=span, .text=text};
  return tok;
}

std_json_Token *std_json_Token_from_type(std_json_TokenType type, std_span_Span span) {
  return std_json_Token_new(type, span, "");
}

std_json_Token *std_json_Token_from_ident(char *text, std_span_Span span) {
  std_json_TokenType type = std_json_TokenType_from_text(text);
  return std_json_Token_new(type, span, text);
}

char *std_json_Token_str(std_json_Token *this) {
  return format_string("%s: %s", std_span_Span_str(this->span), std_json_TokenType_str(this->type));
}

std_json_TokenType std_json_TokenType_from_text(char *text) {
  return ({ std_json_TokenType __yield_0;
    {
      char *__match_str = text;
      if (!strcmp(__match_str, "false")) {
        __yield_0 = std_json_TokenType_False;
      } else if (!strcmp(__match_str, "null")) {
        __yield_0 = std_json_TokenType_Null;
      } else if (!strcmp(__match_str, "true")) {
        __yield_0 = std_json_TokenType_True;
      } else  {
        __yield_0 = std_json_TokenType_Identifier;
      }
    }
;__yield_0; });
}

char *std_json_TokenType_str(std_json_TokenType this) {
  return ({ char *__yield_0;
    switch (this) {
      case std_json_TokenType_False: {
        __yield_0 = "false";
      } break;
      case std_json_TokenType_Null: {
        __yield_0 = "null";
      } break;
      case std_json_TokenType_True: {
        __yield_0 = "true";
      } break;
      default: {
        __yield_0 = std_json_TokenType_dbg(this);
      } break;
    }
;__yield_0; });
}

char *std_value_ValueType_str(std_value_ValueType this) {
  return std_value_ValueType_dbg(this);
}

std_value_Value *std_value_Value_new(std_value_ValueType type) {
  std_value_Value *val = ((std_value_Value *)calloc(((u32)1), ((u32)sizeof(std_value_Value))));
  val->type=type;
  switch (type) {
    case std_value_ValueType_Dictionary: {
      val->u.as_dict=std_compact_map_Map__0_new(((u32)16));
    } break;
    case std_value_ValueType_List: {
      val->u.as_list=std_vector_Vector__1_new(((u32)16));
    } break;
    default: {
    } break;
  }
  val->span=std_span_Span_default();
  return val;
}

std_value_Value *std_value_Value_new_str_buf(std_buffer_Buffer buf) {
  std_value_Value *val = std_value_Value_new(std_value_ValueType_String);
  val->u.as_str=buf;
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

std_value_Value *std_value_Value_new_number(i64 num) {
  std_value_Value *val = std_value_Value_new(std_value_ValueType_Integer);
  val->u.as_num=num;
  return val;
}

std_value_Value *std_value_Value_new_list(std_vector_Vector__1 *vec) {
  std_value_Value *val = std_value_Value_new(std_value_ValueType_List);
  free(val->u.as_list);
  val->u.as_list=vec;
  return val;
}

std_value_Value *std_value_Value_new_dict(std_compact_map_Map__0 *map) {
  std_value_Value *val = std_value_Value_new(std_value_ValueType_Dictionary);
  free(val->u.as_dict);
  val->u.as_dict=map;
  return val;
}

void std_value_Value_ensure(std_value_Value *this, std_value_ValueType type) {
  if ((this->type != type)) {
    printf("Value type mismatch, expected %s but got %s""\n", std_value_ValueType_str(this->type), std_value_ValueType_str(type));
    exit(1);
  } 
}

bool std_value_Value_is(std_value_Value this, std_value_ValueType type) {
  return this.type==type;
}

std_value_Value *std_value_Value_at(std_value_Value *this, u32 idx) {
  std_value_Value_ensure(this, std_value_ValueType_List);
  return std_vector_Vector__1_at(this->u.as_list, idx);
}

void std_value_Value_set(std_value_Value *this, u32 idx, std_value_Value *value) {
  std_value_Value_ensure(this, std_value_ValueType_List);
  this->u.as_list->data[idx]=value;
}

void std_value_Value_push(std_value_Value *this, std_value_Value *value) {
  std_value_Value_ensure(this, std_value_ValueType_List);
  std_vector_Vector__1_push(this->u.as_list, value);
}

std_value_Value *std_value_Value_get(std_value_Value *this, char *key, std_value_Value *defolt) {
  std_value_Value_ensure(this, std_value_ValueType_Dictionary);
  return std_compact_map_Map__0_get(this->u.as_dict, key, defolt);
}

void std_value_Value_insert(std_value_Value *this, char *key, std_value_Value *value) {
  std_value_Value_ensure(this, std_value_ValueType_Dictionary);
  std_compact_map_Map__0_insert(this->u.as_dict, key, value);
}

bool std_value_Value_as_bool(std_value_Value *this) {
  std_value_Value_ensure(this, std_value_ValueType_Bool);
  return this->u.as_bool;
}

i64 std_value_Value_as_num(std_value_Value *this) {
  std_value_Value_ensure(this, std_value_ValueType_Integer);
  return this->u.as_num;
}

std_buffer_Buffer std_value_Value_as_str(std_value_Value *this) {
  std_value_Value_ensure(this, std_value_ValueType_String);
  return this->u.as_str;
}

std_vector_Vector__1 *std_value_Value_as_list(std_value_Value *this) {
  std_value_Value_ensure(this, std_value_ValueType_List);
  return this->u.as_list;
}

std_compact_map_Map__0 *std_value_Value_as_dict(std_value_Value *this) {
  std_value_Value_ensure(this, std_value_ValueType_Dictionary);
  return this->u.as_dict;
}

char *std_value_Value_dbg(std_value_Value *this) {
  std_buffer_Buffer buf = std_json_serialize(this);
  return std_buffer_Buffer_str(&buf);
}

void std_value_Value_free(std_value_Value *this) {
  switch (this->type) {
    case std_value_ValueType_String: {
      std_buffer_Buffer_free(&this->u.as_str);
    } break;
    case std_value_ValueType_List: {
      for (std_vector_Iterator__1 __iter = std_vector_Vector__1_iter(this->u.as_list); std_vector_Iterator__1_has_value(&__iter); std_vector_Iterator__1_next(&__iter)) {
        std_value_Value *val = std_vector_Iterator__1_cur(&__iter);
        {
          std_value_Value_free(val);
        }
      }
      std_vector_Vector__1_free(this->u.as_list);
    } break;
    case std_value_ValueType_Dictionary: {
      for (std_vector_Iterator__2 __iter = std_compact_map_Map__0_iter(this->u.as_dict); std_vector_Iterator__2_has_value(&__iter); std_vector_Iterator__2_next(&__iter)) {
        std_compact_map_Item__0 iter = std_vector_Iterator__2_cur(&__iter);
        {
          free(iter.key);
          std_value_Value_free(iter.value);
        }
      }
      std_compact_map_Map__0_free(this->u.as_dict);
    } break;
    default: {
    } break;
  }
  free(this);
}

std_value_Value *docgen_DocGenerator_gen_enum(docgen_DocGenerator *this, ast_nodes_Enum *enum_) {
  std_value_Value *enum_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(enum_doc, "id", std_value_Value_new_str(format_string("%x", enum_->type)));
  std_value_Value_insert(enum_doc, "name", std_value_Value_new_str(enum_->sym->name));
  if (((bool)enum_->sym->comment)) {
    std_value_Value_insert(enum_doc, "description", std_value_Value_new_str(enum_->sym->comment));
  } 
  std_value_Value_insert(enum_doc, "kind", std_value_Value_new_str("enum"));
  if (enum_->sym->is_extern) {
    std_value_Value_insert(enum_doc, "extern", std_value_Value_new_str(enum_->sym->out_name));
  } 
  std_value_Value *fields_doc = std_value_Value_new(std_value_ValueType_List);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      std_value_Value *field_doc = std_value_Value_new(std_value_ValueType_Dictionary);
      std_value_Value_insert(field_doc, "name", std_value_Value_new_str(field->sym->name));
      if (((bool)field->sym->comment)) {
        std_value_Value_insert(field_doc, "description", std_value_Value_new_str(field->sym->comment));
      } 
      if (field->sym->is_extern) {
        std_value_Value_insert(field_doc, "extern", std_value_Value_new_str(field->sym->out_name));
      } 
      std_value_Value_push(fields_doc, field_doc);
    }
  }
  std_value_Value_insert(enum_doc, "fields", fields_doc);
  std_value_Value *methods_doc = docgen_DocGenerator_gen_methods(this, enum_->type);
  std_value_Value_insert(enum_doc, "methods", methods_doc);
  return enum_doc;
}

char *docgen_DocGenerator_gen_templated_type(docgen_DocGenerator *this, types_Type *base, std_vector_Vector__3 *args) {
  char *base_name = docgen_DocGenerator_gen_typename_str(this, base);
  std_buffer_Buffer buf = std_buffer_Buffer_make(((u32)16));
  std_buffer_Buffer_putsf(&buf, base_name);
  std_buffer_Buffer_puts(&buf, "<");
  bool first = true;
  for (std_vector_Iterator__3 __iter = std_vector_Vector__3_iter(args); std_vector_Iterator__3_has_value(&__iter); std_vector_Iterator__3_next(&__iter)) {
    types_Type *arg_type = std_vector_Iterator__3_cur(&__iter);
    {
      if (!first) 
      std_buffer_Buffer_puts(&buf, ", ");
      
      first=false;
      char *param_name = docgen_DocGenerator_gen_typename_str(this, arg_type);
      std_buffer_Buffer_putsf(&buf, param_name);
    }
  }
  std_buffer_Buffer_puts(&buf, ">");
  return std_buffer_Buffer_str(&buf);
}

char *docgen_DocGenerator_gen_typename_str(docgen_DocGenerator *this, types_Type *type) {
  ae_assert(((bool)type), "compiler/docgen.oc:64:12: Assertion failed: `type?`", "gen_typename_str called with null type");
  switch (type->base) {
    case types_BaseType_Char:
    case types_BaseType_Bool:
    case types_BaseType_Void:
    case types_BaseType_I8:
    case types_BaseType_I16:
    case types_BaseType_I32:
    case types_BaseType_I64:
    case types_BaseType_U8:
    case types_BaseType_U16:
    case types_BaseType_U32:
    case types_BaseType_U64:
    case types_BaseType_F32:
    case types_BaseType_F64:
    case types_BaseType_Alias:
    case types_BaseType_Enum: {
      return format_string("{{%x}}", type);
    } break;
    case types_BaseType_Structure: {
      if (!((bool)type->template_instance)) {
        return format_string("{{%x}}", type);
      }  else {
        ast_nodes_Structure *struc = type->u.struc;
        ast_nodes_TemplateInstance *instance = type->template_instance;
        ast_scopes_Symbol *parent = instance->parent;
        ae_assert(parent->type==ast_scopes_SymbolType_Structure, "compiler/docgen.oc:79:24: Assertion failed: `parent.type == Structure`", "Template instance parent is not a structure");
        ast_nodes_Structure *parent_struc = parent->u.struc;
        return docgen_DocGenerator_gen_templated_type(this, parent_struc->type, instance->params);
      } 
    } break;
    case types_BaseType_UnresolvedTemplate: {
      types_UnresolvedTemplate *uspec = type->u.unresolved_spec;
      return docgen_DocGenerator_gen_templated_type(this, uspec->base, uspec->args);
    } break;
    case types_BaseType_Pointer: {
      char *sub = docgen_DocGenerator_gen_typename_str(this, type->u.ptr);
      str_replace_with(&sub, format_string("&%s", sub));
      return sub;
    } break;
    case types_BaseType_Array: {
      char *sub = docgen_DocGenerator_gen_typename_str(this, type->u.arr->elem_type);
      str_replace_with(&sub, format_string("[%s]", sub));
      return sub;
    } break;
    case types_BaseType_Function: {
      std_buffer_Buffer buf = std_buffer_Buffer_make(((u32)16));
      std_buffer_Buffer_puts(&buf, "fn(");
      bool first = true;
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(type->u.func->params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
        {
          if (!first) 
          std_buffer_Buffer_puts(&buf, ", ");
          
          first=false;
          char *param_name = docgen_DocGenerator_gen_typename_str(this, param->type);
          std_buffer_Buffer_putsf(&buf, param_name);
        }
      }
      std_buffer_Buffer_puts(&buf, "): ");
      char *return_name = docgen_DocGenerator_gen_typename_str(this, type->u.func->return_type);
      std_buffer_Buffer_putsf(&buf, return_name);
      return std_buffer_Buffer_str(&buf);
    } break;
    case types_BaseType_Unresolved: {
      ast_nodes_AST *node = type->u.unresolved;
      switch (node->type) {
        case ast_nodes_ASTType_Identifier: {
          return strdup(node->u.ident.name);
        } break;
        default: {
          ae_assert(false, "compiler/docgen.oc:122:28: Assertion failed: `false`", format_string("Unhandled type in gen_typename_str: %s", ast_nodes_ASTType_dbg(node->type)));
          return "<unknown>";
        } break;
      }
    } break;
    default: {
      ae_assert(false, "compiler/docgen.oc:128:20: Assertion failed: `false`", format_string("Unhandled type in gen_typename_str: %s", types_BaseType_dbg(type->base)));
      return "<unknown>";
    } break;
  }
}

std_value_Value *docgen_DocGenerator_gen_typename(docgen_DocGenerator *this, types_Type *type) {
  ae_assert(((bool)type), "compiler/docgen.oc:135:12: Assertion failed: `type?`", "gen_typename called with null type");
  return std_value_Value_new_str(docgen_DocGenerator_gen_typename_str(this, type));
}

std_value_Value *docgen_DocGenerator_gen_methods(docgen_DocGenerator *this, types_Type *type) {
  ae_assert(types_Type_can_have_methods(type), "compiler/docgen.oc:140:12: Assertion failed: `type.can_have_methods()`", "gen_methods called with type that can't have methods");
  std_value_Value *methods_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  for (std_map_Iterator__4 __iter = std_map_Map__4_iter(type->methods); std_map_Iterator__4_has_value(&__iter); std_map_Iterator__4_next(&__iter)) {
    std_map_Node__4 *it = std_map_Iterator__4_cur(&__iter);
    {
      ast_nodes_Function *method = it->value;
      std_value_Value *method_doc = docgen_DocGenerator_gen_function(this, method);
      std_value_Value_insert(methods_doc, method->sym->name, method_doc);
    }
  }
  return methods_doc;
}

std_value_Value *docgen_DocGenerator_gen_function(docgen_DocGenerator *this, ast_nodes_Function *func) {
  std_value_Value *func_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(func_doc, "id", std_value_Value_new_str(format_string("%x", func)));
  std_value_Value_insert(func_doc, "name", std_value_Value_new_str(format_string("%s", func->sym->name)));
  if (((bool)func->sym->comment)) {
    std_value_Value_insert(func_doc, "description", std_value_Value_new_str(func->sym->comment));
  } 
  if (func->is_method) {
    std_value_Value_insert(func_doc, "kind", std_value_Value_new_str("method"));
    std_value_Value_insert(func_doc, "parent", docgen_DocGenerator_gen_typename(this, func->parent_type));
  }  else {
    std_value_Value_insert(func_doc, "kind", std_value_Value_new_str("function"));
  } 
  if (func->sym->is_extern) {
    std_value_Value_insert(func_doc, "extern", std_value_Value_new_str(func->sym->out_name));
  } 
  std_value_Value *return_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  types_Type *ret_type = func->return_type;
  std_value_Value_insert(return_doc, "type", docgen_DocGenerator_gen_typename(this, ret_type));
  std_value_Value *params_doc = std_value_Value_new(std_value_ValueType_List);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(func->params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
    {
      std_value_Value *param_doc = std_value_Value_new(std_value_ValueType_Dictionary);
      std_value_Value_insert(param_doc, "name", std_value_Value_new_str(format_string("%s", param->sym->name)));
      if (((bool)param->sym->comment)) {
        std_value_Value_insert(param_doc, "description", std_value_Value_new_str(param->sym->comment));
      } 
      std_value_Value_insert(param_doc, "type", docgen_DocGenerator_gen_typename(this, param->type));
      std_value_Value_push(params_doc, param_doc);
    }
  }
  std_value_Value_insert(func_doc, "params", params_doc);
  std_value_Value_insert(func_doc, "return", return_doc);
  return func_doc;
}

std_value_Value *docgen_DocGenerator_gen_struct(docgen_DocGenerator *this, ast_nodes_Structure *struc) {
  std_value_Value *struc_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(struc_doc, "id", std_value_Value_new_str(format_string("%x", struc->type)));
  std_value_Value_insert(struc_doc, "name", std_value_Value_new_str(format_string("%s", struc->sym->name)));
  if (((bool)struc->sym->comment)) {
    std_value_Value_insert(struc_doc, "description", std_value_Value_new_str(struc->sym->comment));
  } 
  if (struc->is_union) {
    std_value_Value_insert(struc_doc, "kind", std_value_Value_new_str("union"));
  }  else {
    std_value_Value_insert(struc_doc, "kind", std_value_Value_new_str("struct"));
  } 
  std_value_Value_insert(struc_doc, "is_templated", std_value_Value_new_bool(struc->is_templated));
  if (struc->is_templated) {
    std_value_Value *params_doc = std_value_Value_new(std_value_ValueType_List);
    for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->template_params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
      ast_nodes_Variable *var = std_vector_Iterator__4_cur(&__iter);
      {
        char *name = var->sym->name;
        std_value_Value_push(params_doc, std_value_Value_new_str(name));
      }
    }
    std_value_Value_insert(struc_doc, "template_params", params_doc);
  } 
  if (struc->sym->is_extern) {
    std_value_Value_insert(struc_doc, "extern", std_value_Value_new_str(struc->sym->out_name));
  } 
  std_value_Value *fields_doc = std_value_Value_new(std_value_ValueType_List);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      std_value_Value *field_doc = std_value_Value_new(std_value_ValueType_Dictionary);
      std_value_Value_insert(field_doc, "name", std_value_Value_new_str(field->sym->name));
      std_value_Value_insert(field_doc, "type", docgen_DocGenerator_gen_typename(this, field->type));
      if (((bool)field->sym->comment)) {
        std_value_Value_insert(field_doc, "description", std_value_Value_new_str(field->sym->comment));
      } 
      if (field->sym->is_extern) {
        std_value_Value_insert(field_doc, "extern", std_value_Value_new_str(field->sym->out_name));
      } 
      std_value_Value_push(fields_doc, field_doc);
    }
  }
  std_value_Value_insert(struc_doc, "fields", fields_doc);
  std_value_Value *methods_doc = docgen_DocGenerator_gen_methods(this, struc->type);
  std_value_Value_insert(struc_doc, "methods", methods_doc);
  return struc_doc;
}

std_value_Value *docgen_DocGenerator_gen_ns(docgen_DocGenerator *this, ast_program_Namespace *ns) {
  std_value_Value *ns_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(ns_doc, "id", std_value_Value_new_str(format_string("%x", ns)));
  if (((bool)ns->sym->comment)) {
    std_value_Value_insert(ns_doc, "description", std_value_Value_new_str(ns->sym->comment));
  } 
  std_value_Value_insert(ns_doc, "name", std_value_Value_new_str(ns->sym->name));
  std_value_Value_insert(ns_doc, "kind", std_value_Value_new_str("namespace"));
  if (!std_vector_Vector__13_is_empty(ns->enums)) {
    std_value_Value *enums_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->enums); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
      ast_nodes_Enum *enum_ = std_vector_Iterator__13_cur(&__iter);
      {
        std_value_Value *enum_doc = docgen_DocGenerator_gen_enum(this, enum_);
        std_value_Value_insert(enums_doc, enum_->sym->name, enum_doc);
      }
    }
    std_value_Value_insert(ns_doc, "enums", enums_doc);
  } 
  if (!std_vector_Vector__11_is_empty(ns->structs)) {
    std_value_Value *structs_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    std_value_Value *unions_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(ns->structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
      ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
      {
        std_value_Value *struct_doc = docgen_DocGenerator_gen_struct(this, struc);
        if (!struc->is_union) {
          std_value_Value_insert(structs_doc, struc->sym->name, struct_doc);
        }  else {
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
  if (!std_vector_Vector__12_is_empty(ns->variables)) {
    std_value_Value *vars_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->variables); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
      ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
      {
        ast_nodes_Variable *var = node->u.var_decl.var;
        std_value_Value *var_doc = std_value_Value_new(std_value_ValueType_Dictionary);
        std_value_Value_insert(var_doc, "id", std_value_Value_new_str(format_string("%x", var)));
        if (((bool)var->sym->comment)) {
          std_value_Value_insert(var_doc, "description", std_value_Value_new_str(var->sym->comment));
        } 
        std_value_Value_insert(var_doc, "name", std_value_Value_new_str(var->sym->name));
        std_value_Value_insert(var_doc, "kind", std_value_Value_new_str("variable"));
        std_value_Value_insert(var_doc, "type", docgen_DocGenerator_gen_typename(this, var->type));
        if (var->sym->is_extern) {
          std_value_Value_insert(var_doc, "extern", std_value_Value_new_str(var->sym->out_name));
        } 
        std_value_Value_insert(vars_doc, var->sym->name, var_doc);
      }
    }
    std_value_Value_insert(ns_doc, "variables", vars_doc);
  } 
  if (!std_vector_Vector__12_is_empty(ns->constants)) {
    std_value_Value *consts_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->constants); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
      ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
      {
        ast_nodes_Variable *var = node->u.var_decl.var;
        std_value_Value *const_doc = std_value_Value_new(std_value_ValueType_Dictionary);
        std_value_Value_insert(const_doc, "id", std_value_Value_new_str(format_string("%x", var)));
        if (((bool)var->sym->comment)) {
          std_value_Value_insert(const_doc, "description", std_value_Value_new_str(var->sym->comment));
        } 
        std_value_Value_insert(const_doc, "name", std_value_Value_new_str(var->sym->name));
        std_value_Value_insert(const_doc, "kind", std_value_Value_new_str("constant"));
        std_value_Value_insert(const_doc, "type", docgen_DocGenerator_gen_typename(this, var->type));
        if (var->sym->is_extern) {
          std_value_Value_insert(const_doc, "extern", std_value_Value_new_str(var->sym->out_name));
        } 
        std_value_Value_insert(consts_doc, var->sym->name, const_doc);
      }
    }
    std_value_Value_insert(ns_doc, "constants", consts_doc);
  } 
  if (!std_vector_Vector__10_is_empty(ns->functions)) {
    std_value_Value *funcs_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(ns->functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
      ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
      {
        if (func->is_method) {
          continue;
        } 
        std_value_Value *func_doc = docgen_DocGenerator_gen_function(this, func);
        std_value_Value_insert(funcs_doc, func->sym->name, func_doc);
      }
    }
    std_value_Value_insert(ns_doc, "functions", funcs_doc);
  } 
  if (!std_map_Map__2_is_empty(ns->namespaces)) {
    std_value_Value *namespaces_doc = std_value_Value_new(std_value_ValueType_Dictionary);
    for (std_map_Iterator__2 __iter = std_map_Map__2_iter(ns->namespaces); std_map_Iterator__2_has_value(&__iter); std_map_Iterator__2_next(&__iter)) {
      std_map_Node__2 *it = std_map_Iterator__2_cur(&__iter);
      {
        std_value_Value *ns_doc = docgen_DocGenerator_gen_ns(this, it->value);
        std_value_Value_insert(namespaces_doc, it->key, ns_doc);
      }
    }
    std_value_Value_insert(ns_doc, "namespaces", namespaces_doc);
  } 
  return ns_doc;
}

std_value_Value *docgen_DocGenerator_gen_builtin(docgen_DocGenerator *this, types_Type *type) {
  std_value_Value *type_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(type_doc, "id", std_value_Value_new_str(format_string("%x", type)));
  std_value_Value_insert(type_doc, "name", std_value_Value_new_str(format_string("%s", type->sym->name)));
  std_value_Value_insert(type_doc, "description", std_value_Value_new_str(format_string("Built-in type %s", type->sym->name)));
  std_value_Value_insert(type_doc, "kind", std_value_Value_new_str("builtin"));
  std_value_Value *methods_doc = docgen_DocGenerator_gen_methods(this, type);
  std_value_Value_insert(type_doc, "methods", methods_doc);
  return type_doc;
}

std_value_Value *docgen_DocGenerator_gen_builtins(docgen_DocGenerator *this, ast_program_Program *program) {
  std_value_Value *builtins_doc = std_value_Value_new(std_value_ValueType_Dictionary);
  for (u32 i = ((u32)0); (i < ((u32)types_BaseType_NUM_BASE_TYPES)); i+=((u32)1)) {
    types_Type *type = ast_program_Program_get_base_type(program, ((types_BaseType)i), std_span_Span_default());
    std_value_Value *type_doc = docgen_DocGenerator_gen_builtin(this, type);
    std_value_Value_insert(builtins_doc, type->name, type_doc);
  }
  types_Type *str_type = ast_program_Program_get_type_by_name(program, "str", std_span_Span_default());
  std_value_Value *str_type_doc = docgen_DocGenerator_gen_builtin(this, str_type);
  std_value_Value_insert(builtins_doc, "str", str_type_doc);
  types_Type *untyped_ptr_type = ast_program_Program_get_type_by_name(program, "untyped_ptr", std_span_Span_default());
  std_value_Value *untyped_ptr_type_doc = docgen_DocGenerator_gen_builtin(this, untyped_ptr_type);
  std_value_Value_insert(builtins_doc, "untyped_ptr", untyped_ptr_type_doc);
  return builtins_doc;
}

void docgen_generate_doc_json(ast_program_Program *program, char *json_path) {
  docgen_DocGenerator docs_generator = (docgen_DocGenerator){};
  std_value_Value *docs = docgen_DocGenerator_gen_ns(&docs_generator, program->global);
  std_value_Value *builtins = docgen_DocGenerator_gen_builtins(&docs_generator, program);
  std_value_Value_insert(docs, "builtins", builtins);
  std_value_Value *container = std_value_Value_new(std_value_ValueType_Dictionary);
  std_value_Value_insert(container, "ocen", docs);
  std_json_write_to_file(container, json_path);
}

parser_Parser parser_Parser_make(ast_program_Program *program, ast_program_Namespace *ns) {
  return (parser_Parser){.tokens=NULL, .curr=((u32)0), .curr_func=NULL, .program=program, .ns=ns};
}

tokens_Token *parser_Parser_peek(parser_Parser *this, i32 off) {
  i32 idx = (((i32)this->curr) + off);
  ae_assert(((0 <= idx) && (idx < ((i32)this->tokens->size))), "compiler/parser.oc:41:12: Assertion failed: `0i32 <= idx < (.tokens.size as i32`", NULL);
  return std_vector_Vector__6_at(this->tokens, ((u32)idx));
}

errors_Error *parser_Parser_error_msg(parser_Parser *this, char *msg) {
  errors_Error *err = errors_Error_new(parser_Parser_token(this)->span, msg);
  std_vector_Vector__14_push(this->program->errors, err);
  return err;
}

errors_Error *parser_Parser_error(parser_Parser *this, errors_Error *err) {
  std_vector_Vector__14_push(this->program->errors, err);
  return err;
}

void parser_Parser_unhandled_type(parser_Parser *this, char *func) {
  parser_Parser_error_msg(this, format_string("Unexpected token in %s: %s", func, tokens_TokenType_str(parser_Parser_token(this)->type)));
}

tokens_Token *parser_Parser_token(parser_Parser *this) {
  return std_vector_Vector__6_at(this->tokens, ((u32)this->curr));
}

bool parser_Parser_token_is(parser_Parser *this, tokens_TokenType type) {
  if (type==tokens_TokenType_Newline) {
    return parser_Parser_token(this)->seen_newline;
  } 
  return parser_Parser_token(this)->type==type;
}

bool parser_Parser_consume_if(parser_Parser *this, tokens_TokenType type) {
  if (parser_Parser_token_is(this, type)) {
    if ((type != tokens_TokenType_Newline)) {
      this->curr+=((u32)1);
    } 
    return true;
  } 
  return false;
}

void parser_Parser_consume_newline_or(parser_Parser *this, tokens_TokenType type) {
  if (parser_Parser_token_is(this, type)) {
    this->curr+=((u32)1);
  }  else   if (!parser_Parser_token(this)->seen_newline) {
    parser_Parser_error_msg(this, format_string("Expected %s or newline", tokens_TokenType_str(type)));
    ast_program_Program_exit_with_errors(this->program);
  } 
  
}

tokens_Token *parser_Parser_consume(parser_Parser *this, tokens_TokenType type) {
  tokens_Token *tok = parser_Parser_token(this);
  if (!parser_Parser_consume_if(this, type)) {
    parser_Parser_error_msg(this, format_string("Expected TokenType::%s", tokens_TokenType_str(type)));
    ast_program_Program_exit_with_errors(this->program);
  } 
  return tok;
}

void parser_Parser_consume_end_of_statement(parser_Parser *this) {
  if (parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) 
  return ;
  
  parser_Parser_consume_newline_or(this, tokens_TokenType_Semicolon);
}

types_Type *parser_Parser_parse_type(parser_Parser *this) {
  return ({ types_Type *__yield_0;
    switch (parser_Parser_token(this)->type) {
      case tokens_TokenType_Identifier: {
        ast_nodes_AST *ident = parser_Parser_parse_scoped_identifier(this);
        char *name = (ident->type==ast_nodes_ASTType_Identifier ? ident->u.ident.name : "<unresolved>");
        types_Type *typ = types_Type_new_unresolved(name, ident->span);
        typ->u.unresolved=ident;
        __yield_0 = typ;
      } break;
      case tokens_TokenType_Ampersand: {
        tokens_Token *amp = parser_Parser_consume(this, tokens_TokenType_Ampersand);
        types_Type *base = parser_Parser_parse_type(this);
        types_Type *typ = types_Type_new_resolved(types_BaseType_Pointer, std_span_Span_join(amp->span, base->span));
        typ->u.ptr=base;
        __yield_0 = typ;
      } break;
      case tokens_TokenType_Fn: {
        std_span_Span start_span = parser_Parser_token(this)->span;
        parser_Parser_consume(this, tokens_TokenType_Fn);
        parser_Parser_consume(this, tokens_TokenType_OpenParen);
        std_vector_Vector__4 *params = std_vector_Vector__4_new(((u32)16));
        while (!parser_Parser_token_is(this, tokens_TokenType_CloseParen)) {
          types_Type *param_type = parser_Parser_parse_type(this);
          ast_nodes_Variable *var = ast_nodes_Variable_new(param_type);
          var->sym=ast_scopes_Symbol_from_local_variable("", var, param_type->span);
          std_vector_Vector__4_push(params, var);
          if (!parser_Parser_token_is(this, tokens_TokenType_CloseParen)) {
            parser_Parser_consume(this, tokens_TokenType_Comma);
          } 
        }
        tokens_Token *close = parser_Parser_consume(this, tokens_TokenType_CloseParen);
        types_Type *return_type;
        if (parser_Parser_consume_if(this, tokens_TokenType_Colon)) {
          return_type=parser_Parser_parse_type(this);
        }  else {
          return_type=types_Type_new_unresolved_base(types_BaseType_Void, start_span);
        } 
        types_Type *type = types_Type_new_resolved(types_BaseType_Function, std_span_Span_join(start_span, close->span));
        type->u.func=types_FunctionType_from_args(params, return_type);
        __yield_0 = type;
      } break;
      case tokens_TokenType_OpenSquare: {
        std_span_Span start_span = parser_Parser_token(this)->span;
        parser_Parser_consume(this, tokens_TokenType_OpenSquare);
        types_Type *elem_type = parser_Parser_parse_type(this);
        parser_Parser_consume(this, tokens_TokenType_Semicolon);
        ast_nodes_AST *size_expr = parser_Parser_parse_expression(this, tokens_TokenType_CloseSquare);
        tokens_Token *close = parser_Parser_consume(this, tokens_TokenType_CloseSquare);
        types_Type *typ = types_Type_new_resolved(types_BaseType_Array, std_span_Span_join(start_span, close->span));
        typ->u.arr=types_ArrayType_new(elem_type, size_expr);
        __yield_0 = typ;
      } break;
      default: {
        parser_Parser_unhandled_type(this, "parse_type");
        __yield_0 = types_Type_new_unresolved_base(types_BaseType_Error, parser_Parser_token(this)->span);
      } break;
    }
;__yield_0; });
}

ast_nodes_AST *parser_Parser_parse_scoped_identifier(parser_Parser *this) {
  tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_Identifier);
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Identifier, tok->span);
  node->u.ident.name=tok->text;
  while (true) {
    switch (parser_Parser_token(this)->type) {
      case tokens_TokenType_ColonColon: {
        parser_Parser_consume(this, tokens_TokenType_ColonColon);
        tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
        ast_nodes_AST *lookup = ast_nodes_AST_new(ast_nodes_ASTType_NSLookup, std_span_Span_join(node->span, name->span));
        lookup->u.lookup=(ast_nodes_NSLookup){.lhs=node, .rhs_name=name->text, .rhs_span=name->span};
        node=lookup;
      } break;
      case tokens_TokenType_LessThan: {
        tokens_Token *prev_token = std_vector_Vector__6_at(this->tokens, (((u32)this->curr) - ((u32)1)));
        if (!std_span_Span_starts_right_after(parser_Parser_token(this)->span, prev_token->span)) {
          return node;
        } 
        tokens_Token *next_next_token = std_vector_Vector__6_at(this->tokens, (((u32)this->curr) + ((u32)2)));
        if (next_next_token->type==tokens_TokenType_Dot) {
          return node;
        } 
        tokens_Token *start = parser_Parser_consume(this, tokens_TokenType_LessThan);
        std_vector_Vector__3 *args = std_vector_Vector__3_new(((u32)16));
        while (!parser_Parser_token_is(this, tokens_TokenType_GreaterThan)) {
          std_vector_Vector__3_push(args, parser_Parser_parse_type(this));
          if (!parser_Parser_token_is(this, tokens_TokenType_GreaterThan)) {
            if (!parser_Parser_consume_if(this, tokens_TokenType_Comma)) {
              parser_Parser_error(this, errors_Error_new_note(parser_Parser_token(this)->span, "Parsing template specialization: expected `,` or `>`", "If you're comparing values, put a space before the `<` earlier"));
              return ast_nodes_AST_new(ast_nodes_ASTType_Error, node->span);
            } 
          } 
        }
        tokens_Token *end = parser_Parser_consume(this, tokens_TokenType_GreaterThan);
        ast_nodes_AST *spec = ast_nodes_AST_new(ast_nodes_ASTType_Specialization, std_span_Span_join(node->span, end->span));
        spec->u.spec=(ast_nodes_Specialization){.base=node, .template_args=args};
        node=spec;
      } break;
      default: {
        return node;
      } break;
    }
  }
  return NULL;
}

ast_nodes_AST *parser_Parser_parse_format_string(parser_Parser *this) {
  tokens_Token *fstr = parser_Parser_consume(this, tokens_TokenType_FormatStringLiteral);
  u32 fstr_len = str_len(fstr->text);
  std_vector_Vector__7 *expr_parts = std_vector_Vector__7_new(((u32)16));
  std_vector_Vector__19 *expr_start = std_vector_Vector__19_new(((u32)16));
  std_vector_Vector__7 *format_parts = std_vector_Vector__7_new(((u32)16));
  std_vector_Vector__7 *specifiers = std_vector_Vector__7_new(((u32)16));
  u32 specifier_loc = ((u32)0);
  bool specifier_found = false;
  u32 count = ((u32)0);
  u32 cur_start = ((u32)0);
  for (u32 i = ((u32)0); (i < fstr_len); i+=((u32)1)) {
    if (fstr->text[i]=='\\') {
      i+=((u32)1);
    }  else     if (fstr->text[i]=='{') {
      if (count==((u32)0)) {
        char *part = str_substring(fstr->text, cur_start, (i - cur_start));
        std_vector_Vector__7_push(format_parts, part);
        cur_start=(i + ((u32)1));
      } 
      count+=((u32)1);
    }  else     if (fstr->text[i]=='}') {
      count-=((u32)1);
      if (count==((u32)0)) {
        if ((specifier_loc > ((u32)0))) {
          u32 len = (specifier_loc - cur_start);
          char *part = str_substring(fstr->text, cur_start, len);
          std_vector_Vector__7_push(expr_parts, part);
          std_vector_Vector__19_push(expr_start, cur_start);
          specifier_loc+=((u32)1);
          while (((specifier_loc < i) && fstr->text[specifier_loc]==' ')) {
            specifier_loc+=((u32)1);
          }
          if (specifier_loc==i) {
            std_span_Location loc = fstr->span.start;
            loc.col+=(specifier_loc + ((u32)1));
            std_span_Span span = (std_span_Span){.start=loc, .end=loc};
            parser_Parser_error(this, errors_Error_new(span, "Expected format specifier"));
            return NULL;
          } 
          char *spec = str_substring(fstr->text, specifier_loc, (i - specifier_loc));
          std_vector_Vector__7_push(specifiers, spec);
        }  else {
          char *part = str_substring(fstr->text, cur_start, (i - cur_start));
          std_vector_Vector__7_push(expr_parts, part);
          std_vector_Vector__19_push(expr_start, cur_start);
          std_vector_Vector__7_push(specifiers, NULL);
        } 
        cur_start=(i + ((u32)1));
        specifier_loc=((u32)0);
        specifier_found=false;
      }  else       if ((count < ((u32)0))) {
        parser_Parser_error(this, errors_Error_new(fstr->span, "Unmatched '}' in format string"));
        return NULL;
      } 
      
    }  else     if (fstr->text[i]==':') {
      if (((count==((u32)1) && (fstr->text[(i - ((u32)1))] != ':')) && (fstr->text[(i + ((u32)1))] != ':'))) {
        specifier_loc=i;
        specifier_found=true;
      } 
    } 
    
    
    
  }
  if ((count != ((u32)0))) {
    parser_Parser_error(this, errors_Error_new(fstr->span, "Unmatched '{' in format string"));
    return NULL;
  } 
  char *part = str_substring(fstr->text, cur_start, (fstr_len - cur_start));
  std_vector_Vector__7_push(format_parts, part);
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_FormatStringLiteral, fstr->span);
  node->u.fmt_str.parts=format_parts;
  std_span_Location fstr_start = fstr->span.start;
  std_vector_Vector__12 *expr_nodes = std_vector_Vector__12_new(((u32)16));
  for (u32 i = ((u32)0); (i < expr_parts->size); i+=((u32)1)) {
    char *part = std_vector_Vector__7_at(expr_parts, i);
    u32 start = std_vector_Vector__19_at(expr_start, i);
    lexer_Lexer lexer = lexer_Lexer_make(part, fstr_start.filename);
    lexer.loc=fstr_start;
    lexer.loc.col+=(start + ((u32)1));
    std_vector_Vector__6 *tokens = lexer_Lexer_lex(&lexer);
    for (std_vector_Iterator__14 __iter = std_vector_Vector__14_iter(lexer.errors); std_vector_Iterator__14_has_value(&__iter); std_vector_Iterator__14_next(&__iter)) {
      errors_Error *error = std_vector_Iterator__14_cur(&__iter);
      {
        parser_Parser_error(this, error);
      }
    }
    std_vector_Vector__14_free(lexer.errors);
    parser_Parser sub_parser = parser_Parser_make(this->program, this->ns);
    sub_parser.tokens=tokens;
    sub_parser.curr=((u32)0);
    sub_parser.curr_func=this->curr_func;
    ast_nodes_AST *expr = parser_Parser_parse_expression(&sub_parser, tokens_TokenType_CloseCurly);
    if (!parser_Parser_token_is(&sub_parser, tokens_TokenType_EOF)) {
      parser_Parser_error(this, errors_Error_new(expr->span, "Invalid expression in format string"));
    } 
    std_vector_Vector__12_push(expr_nodes, expr);
  }
  node->u.fmt_str.exprs=expr_nodes;
  node->u.fmt_str.specs=specifiers;
  std_vector_Vector__7_free(expr_parts);
  std_vector_Vector__19_free(expr_start);
  return node;
}

ast_nodes_AST *parser_Parser_parse_match(parser_Parser *this) {
  tokens_Token *op = parser_Parser_consume(this, tokens_TokenType_Match);
  ast_nodes_AST *expr = parser_Parser_parse_expression(this, tokens_TokenType_OpenCurly);
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Match, std_span_Span_join(op->span, expr->span));
  node->u.match_stmt.expr=expr;
  std_vector_Vector__18 *cases = std_vector_Vector__18_new(((u32)16));
  parser_Parser_consume(this, tokens_TokenType_OpenCurly);
  while (!parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) {
    if (parser_Parser_token_is(this, tokens_TokenType_Else)) {
      node->u.match_stmt.defolt_span=parser_Parser_token(this)->span;
      parser_Parser_consume(this, tokens_TokenType_Else);
      parser_Parser_consume(this, tokens_TokenType_FatArrow);
      node->u.match_stmt.defolt=parser_Parser_parse_statement(this);
    }  else {
      ast_nodes_AST *cond = parser_Parser_parse_atom(this, tokens_TokenType_Line);
      ast_nodes_AST *body = ((ast_nodes_AST *)NULL);
      if (!parser_Parser_consume_if(this, tokens_TokenType_Line)) {
        parser_Parser_consume(this, tokens_TokenType_FatArrow);
        body=parser_Parser_parse_statement(this);
        if (!parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) {
          parser_Parser_consume_newline_or(this, tokens_TokenType_Comma);
        } 
      } 
      ast_nodes_MatchCase *_case = ast_nodes_MatchCase_new(cond, body);
      std_vector_Vector__18_push(cases, _case);
    } 
  }
  node->span=std_span_Span_join(op->span, parser_Parser_token(this)->span);
  parser_Parser_consume(this, tokens_TokenType_CloseCurly);
  node->u.match_stmt.cases=cases;
  return node;
}

types_Type *parser_Parser_parse_literal_suffix_type(parser_Parser *this, tokens_Token *suffix) {
  if (!((bool)suffix)) 
  return NULL;
  
  ast_nodes_AST *ident = ast_nodes_AST_new(ast_nodes_ASTType_Identifier, suffix->span);
  ident->u.ident.name=suffix->text;
  types_Type *typ = types_Type_new_unresolved(suffix->text, suffix->span);
  typ->u.unresolved=ident;
  return typ;
}

ast_nodes_AST *parser_Parser_parse_call(parser_Parser *this, ast_nodes_AST *callee) {
  parser_Parser_consume(this, tokens_TokenType_OpenParen);
  std_vector_Vector__16 *args = std_vector_Vector__16_new(((u32)16));
  while (!parser_Parser_token_is(this, tokens_TokenType_CloseParen)) {
    char *label = ((char *)NULL);
    std_span_Span label_span = std_span_Span_default();
    ast_nodes_AST *expr = parser_Parser_parse_expression(this, tokens_TokenType_Comma);
    if ((expr->type==ast_nodes_ASTType_Identifier && parser_Parser_token_is(this, tokens_TokenType_Colon))) {
      parser_Parser_consume(this, tokens_TokenType_Colon);
      label=expr->u.ident.name;
      label_span=expr->span;
      expr=parser_Parser_parse_expression(this, tokens_TokenType_Comma);
    } 
    std_vector_Vector__16_push(args, ast_nodes_Argument_new(label, label_span, expr));
    if (!parser_Parser_token_is(this, tokens_TokenType_CloseParen)) {
      parser_Parser_consume(this, tokens_TokenType_Comma);
    } 
  }
  tokens_Token *end = parser_Parser_consume(this, tokens_TokenType_CloseParen);
  ast_nodes_ASTType call_type = ast_nodes_ASTType_Call;
  ast_nodes_AST *call = ast_nodes_AST_new(call_type, std_span_Span_join(callee->span, end->span));
  call->u.call.callee=callee;
  call->u.call.args=args;
  return call;
}

ast_nodes_AST *parser_Parser_parse_global_value(parser_Parser *this, bool is_const) {
  tokens_Token *start_token = ({ tokens_Token *__yield_0;
    if (is_const) {
      __yield_0 = parser_Parser_consume(this, tokens_TokenType_Const);
    }  else {
      __yield_0 = parser_Parser_consume(this, tokens_TokenType_Let);
    } 
;__yield_0; });
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_VarDeclaration, parser_Parser_token(this)->span);
  tokens_Token *name = ({ tokens_Token *__yield_0;
    if (parser_Parser_token_is(this, tokens_TokenType_Identifier)) {
      __yield_0 = parser_Parser_consume(this, tokens_TokenType_Identifier);
    }  else {
      parser_Parser_error(this, errors_Error_new(parser_Parser_token(this)->span, "Expected identifier"));
      return node;
    } 
;__yield_0; });
  types_Type *type = ((types_Type *)NULL);
  if (parser_Parser_consume_if(this, tokens_TokenType_Colon)) {
    type=parser_Parser_parse_type(this);
  } 
  ast_nodes_Variable *var = ast_nodes_Variable_new(type);
  var->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Variable, this->ns, this->ns->sym, name->text, name->span);
  var->sym->u.var=var;
  if (is_const) {
    var->sym->type=ast_scopes_SymbolType_Constant;
  } 
  parser_Parser_parse_extern_into_symbol(this, var->sym);
  node->u.var_decl.var=var;
  if (parser_Parser_consume_if(this, tokens_TokenType_Equals)) {
    node->u.var_decl.init=parser_Parser_parse_expression(this, tokens_TokenType_Newline);
  } 
  parser_Parser_consume_newline_or(this, tokens_TokenType_Semicolon);
  return node;
}

ast_nodes_AST *parser_Parser_parse_atom(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *node = ((ast_nodes_AST *)NULL);
  switch (parser_Parser_token(this)->type) {
    case tokens_TokenType_If: {
      node=parser_Parser_parse_if(this);
    } break;
    case tokens_TokenType_Match: {
      node=parser_Parser_parse_match(this);
    } break;
    case tokens_TokenType_OpenCurly: {
      node=parser_Parser_parse_block(this);
    } break;
    case tokens_TokenType_FormatStringLiteral: {
      node=parser_Parser_parse_format_string(this);
    } break;
    case tokens_TokenType_Null: {
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_Null);
      node=ast_nodes_AST_new(ast_nodes_ASTType_Null, tok->span);
    } break;
    case tokens_TokenType_IntLiteral: {
      node=ast_nodes_AST_new(ast_nodes_ASTType_IntLiteral, parser_Parser_token(this)->span);
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_IntLiteral);
      node->u.num_literal=(ast_nodes_NumLiteral){.text=tok->text, .suffix=parser_Parser_parse_literal_suffix_type(this, tok->suffix)};
    } break;
    case tokens_TokenType_FloatLiteral: {
      node=ast_nodes_AST_new(ast_nodes_ASTType_FloatLiteral, parser_Parser_token(this)->span);
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_FloatLiteral);
      node->u.num_literal=(ast_nodes_NumLiteral){.text=tok->text, .suffix=parser_Parser_parse_literal_suffix_type(this, tok->suffix)};
    } break;
    case tokens_TokenType_StringLiteral: {
      node=ast_nodes_AST_new(ast_nodes_ASTType_StringLiteral, parser_Parser_token(this)->span);
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_StringLiteral);
      node->u.string_literal=tok->text;
    } break;
    case tokens_TokenType_CharLiteral: {
      node=ast_nodes_AST_new(ast_nodes_ASTType_CharLiteral, parser_Parser_token(this)->span);
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_CharLiteral);
      node->u.char_literal=tok->text;
    } break;
    case tokens_TokenType_Identifier: {
      node=parser_Parser_parse_scoped_identifier(this);
    } break;
    case tokens_TokenType_True:
    case tokens_TokenType_False: {
      tokens_Token *tok = parser_Parser_consume(this, parser_Parser_token(this)->type);
      node=ast_nodes_AST_new(ast_nodes_ASTType_BoolLiteral, tok->span);
      node->u.bool_literal=tok->type==tokens_TokenType_True;
    } break;
    case tokens_TokenType_OpenParen: {
      parser_Parser_consume(this, tokens_TokenType_OpenParen);
      node=parser_Parser_parse_expression(this, tokens_TokenType_CloseParen);
      parser_Parser_consume(this, tokens_TokenType_CloseParen);
    } break;
    case tokens_TokenType_Dot: {
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_Dot);
      tokens_Token *ident = parser_Parser_consume(this, tokens_TokenType_Identifier);
      if (((!((bool)this->curr_func) || !this->curr_func->is_method) || this->curr_func->is_static)) {
        parser_Parser_error(this, errors_Error_new(tok->span, "Cannot use dot shorthand outside of a method"));
        return ast_nodes_AST_new(ast_nodes_ASTType_Error, tok->span);
      } 
      ast_nodes_AST *lhs = ast_nodes_AST_new(ast_nodes_ASTType_Identifier, tok->span);
      lhs->u.ident.name="this";
      node=ast_nodes_AST_new(ast_nodes_ASTType_Member, std_span_Span_join(tok->span, ident->span));
      node->u.member.lhs=lhs;
      node->u.member.rhs_name=ident->text;
      node->u.member.rhs_span=ident->span;
    } break;
    default: {
      parser_Parser_unhandled_type(this, "parse_expression");
      node=ast_nodes_AST_new(ast_nodes_ASTType_Error, parser_Parser_token(this)->span);
      this->curr+=((u32)1);
    } break;
  }
  return node;
}

ast_nodes_AST *parser_Parser_parse_postfix(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *node = parser_Parser_parse_atom(this, end_type);
  bool running = true;
  while (running) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    switch (parser_Parser_token(this)->type) {
      case tokens_TokenType_OpenParen: {
        node=parser_Parser_parse_call(this, node);
      } break;
      case tokens_TokenType_Dot: {
        parser_Parser_consume(this, tokens_TokenType_Dot);
        tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
        ast_nodes_AST *member = ast_nodes_AST_new(ast_nodes_ASTType_Member, std_span_Span_join(node->span, name->span));
        member->u.member.lhs=node;
        member->u.member.rhs_name=name->text;
        member->u.member.rhs_span=name->span;
        node=member;
      } break;
      case tokens_TokenType_Question: {
        tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_Question);
        ast_nodes_AST *op = ast_nodes_AST_new(ast_nodes_ASTType_IsNotNull, std_span_Span_join(node->span, tok->span));
        op->u.unary=node;
        node=op;
      } break;
      case tokens_TokenType_OpenSquare: {
        parser_Parser_consume(this, tokens_TokenType_OpenSquare);
        ast_nodes_AST *index = parser_Parser_parse_expression(this, tokens_TokenType_CloseSquare);
        tokens_Token *close = parser_Parser_consume(this, tokens_TokenType_CloseSquare);
        ast_nodes_AST *op = ast_nodes_AST_new(ast_nodes_ASTType_Index, std_span_Span_join(node->span, close->span));
        op->u.binary.lhs=node;
        op->u.binary.rhs=index;
        node=op;
      } break;
      case tokens_TokenType_MinusMinus:
      case tokens_TokenType_PlusPlus: {
        std_span_Span span = std_span_Span_join(node->span, parser_Parser_token(this)->span);
        ast_nodes_ASTType op = ({ ast_nodes_ASTType __yield_0;
          if (parser_Parser_token_is(this, tokens_TokenType_MinusMinus)) {
            parser_Parser_consume(this, tokens_TokenType_MinusMinus);
            __yield_0 = ast_nodes_ASTType_PostDecrement;
          }  else {
            parser_Parser_consume(this, tokens_TokenType_PlusPlus);
            __yield_0 = ast_nodes_ASTType_PostIncrement;
          } 
;__yield_0; });
        node=ast_nodes_AST_new_unop(op, span, node);
      } break;
      default: {
        running=false;
      } break;
    }
  }
  return node;
}

ast_nodes_AST *parser_Parser_parse_prefix(parser_Parser *this, tokens_TokenType end_type) {
  switch (parser_Parser_token(this)->type) {
    case tokens_TokenType_Ampersand: {
      tokens_Token *amp = parser_Parser_consume(this, tokens_TokenType_Ampersand);
      ast_nodes_AST *expr = parser_Parser_parse_prefix(this, end_type);
      ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Address, std_span_Span_join(amp->span, expr->span));
      node->u.unary=expr;
      return node;
    } break;
    case tokens_TokenType_MinusMinus:
    case tokens_TokenType_PlusPlus: {
      std_span_Span start_span = parser_Parser_token(this)->span;
      ast_nodes_ASTType op = ({ ast_nodes_ASTType __yield_0;
        if (parser_Parser_token_is(this, tokens_TokenType_MinusMinus)) {
          parser_Parser_consume(this, tokens_TokenType_MinusMinus);
          __yield_0 = ast_nodes_ASTType_PreDecrement;
        }  else {
          parser_Parser_consume(this, tokens_TokenType_PlusPlus);
          __yield_0 = ast_nodes_ASTType_PreIncrement;
        } 
;__yield_0; });
      ast_nodes_AST *expr = parser_Parser_parse_prefix(this, end_type);
      return ast_nodes_AST_new_unop(op, std_span_Span_join(start_span, expr->span), expr);
    } break;
    case tokens_TokenType_SizeOf: {
      tokens_Token *start = parser_Parser_consume(this, tokens_TokenType_SizeOf);
      parser_Parser_consume(this, tokens_TokenType_OpenParen);
      types_Type *type = parser_Parser_parse_type(this);
      tokens_Token *close = parser_Parser_consume(this, tokens_TokenType_CloseParen);
      ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_SizeOf, std_span_Span_join(start->span, close->span));
      node->u.size_of_type=type;
      return node;
    } break;
    case tokens_TokenType_Star: {
      tokens_Token *star = parser_Parser_consume(this, tokens_TokenType_Star);
      ast_nodes_AST *expr = parser_Parser_parse_prefix(this, end_type);
      ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Dereference, std_span_Span_join(star->span, expr->span));
      node->u.unary=expr;
      return node;
    } break;
    case tokens_TokenType_Minus: {
      tokens_Token *minus = parser_Parser_consume(this, tokens_TokenType_Minus);
      ast_nodes_AST *expr = parser_Parser_parse_prefix(this, end_type);
      ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Negate, std_span_Span_join(minus->span, expr->span));
      node->u.unary=expr;
      return node;
    } break;
    case tokens_TokenType_Not: {
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_Not);
      ast_nodes_AST *expr = parser_Parser_parse_prefix(this, end_type);
      ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Not, std_span_Span_join(tok->span, expr->span));
      node->u.unary=expr;
      return node;
    } break;
    case tokens_TokenType_Tilde: {
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_Tilde);
      ast_nodes_AST *expr = parser_Parser_parse_prefix(this, end_type);
      ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_BitwiseNot, std_span_Span_join(tok->span, expr->span));
      node->u.unary=expr;
      return node;
    } break;
    default: {
      return parser_Parser_parse_postfix(this, end_type);
    } break;
  }
}

ast_nodes_AST *parser_Parser_parse_cast(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_prefix(this, end_type);
  while (parser_Parser_token_is(this, tokens_TokenType_As)) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_As);
    types_Type *type_node = parser_Parser_parse_type(this);
    ast_nodes_AST *op = ast_nodes_AST_new(ast_nodes_ASTType_Cast, std_span_Span_join(lhs->span, type_node->span));
    op->u.cast.lhs=lhs;
    op->u.cast.to=type_node;
    lhs=op;
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_term(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_cast(this, end_type);
  while (((parser_Parser_token_is(this, tokens_TokenType_Star) || parser_Parser_token_is(this, tokens_TokenType_Slash)) || parser_Parser_token_is(this, tokens_TokenType_Percent))) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ast_nodes_ASTType_from_token(parser_Parser_token(this)->type);
    this->curr+=((u32)1);
    ast_nodes_AST *rhs = parser_Parser_parse_cast(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_additive(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_term(this, end_type);
  while ((parser_Parser_token_is(this, tokens_TokenType_Plus) || parser_Parser_token_is(this, tokens_TokenType_Minus))) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ast_nodes_ASTType_from_token(parser_Parser_token(this)->type);
    this->curr+=((u32)1);
    ast_nodes_AST *rhs = parser_Parser_parse_term(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_shift(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_additive(this, end_type);
  while ((parser_Parser_token_is(this, tokens_TokenType_LessThan) || parser_Parser_token_is(this, tokens_TokenType_GreaterThan))) {
    tokens_Token *next_token = std_vector_Vector__6_at(this->tokens, (this->curr + ((u32)1)));
    if ((parser_Parser_token(this)->type != next_token->type)) 
    break;
    
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ({ ast_nodes_ASTType __yield_0;
      if (next_token->type==tokens_TokenType_LessThan) {
        __yield_0 = ast_nodes_ASTType_LeftShift;
      }  else {
        __yield_0 = ast_nodes_ASTType_RightShift;
      } 
;__yield_0; });
    this->curr+=((u32)2);
    ast_nodes_AST *rhs = parser_Parser_parse_additive(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_bw_and(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_shift(this, end_type);
  while (parser_Parser_token_is(this, tokens_TokenType_Ampersand)) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ast_nodes_ASTType_from_token(parser_Parser_token(this)->type);
    this->curr+=((u32)1);
    ast_nodes_AST *rhs = parser_Parser_parse_shift(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_bw_xor(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_bw_and(this, end_type);
  while (parser_Parser_token_is(this, tokens_TokenType_Caret)) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ast_nodes_ASTType_from_token(parser_Parser_token(this)->type);
    this->curr+=((u32)1);
    ast_nodes_AST *rhs = parser_Parser_parse_bw_and(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_bw_or(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_bw_xor(this, end_type);
  while (parser_Parser_token_is(this, tokens_TokenType_Line)) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ast_nodes_ASTType_from_token(parser_Parser_token(this)->type);
    this->curr+=((u32)1);
    ast_nodes_AST *rhs = parser_Parser_parse_bw_xor(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_relational(parser_Parser *this, tokens_TokenType end_type) {
  std_vector_Vector__12 *operands = std_vector_Vector__12_new(((u32)16));
  std_vector_Vector__6 *operators = std_vector_Vector__6_new(((u32)16));
  std_vector_Vector__12_push(operands, parser_Parser_parse_bw_or(this, end_type));
  while ((((((parser_Parser_token_is(this, tokens_TokenType_LessThan) || parser_Parser_token_is(this, tokens_TokenType_GreaterThan)) || parser_Parser_token_is(this, tokens_TokenType_LessThanEquals)) || parser_Parser_token_is(this, tokens_TokenType_GreaterThanEquals)) || parser_Parser_token_is(this, tokens_TokenType_EqualEquals)) || parser_Parser_token_is(this, tokens_TokenType_NotEquals))) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    std_vector_Vector__6_push(operators, parser_Parser_token(this));
    this->curr+=((u32)1);
    ast_nodes_AST *term = parser_Parser_parse_bw_or(this, end_type);
    std_vector_Vector__12_push(operands, term);
  }
  if (operators->size==((u32)0)) 
  return std_vector_Vector__12_at(operands, ((u32)0));
  
  ast_nodes_AST *root = ((ast_nodes_AST *)NULL);
  for (u32 i = ((u32)0); (i < operators->size); i+=((u32)1)) {
    tokens_Token *tok = std_vector_Vector__6_at(operators, i);
    ast_nodes_AST *lhs = std_vector_Vector__12_at(operands, i);
    ast_nodes_AST *rhs = std_vector_Vector__12_at(operands, (i + ((u32)1)));
    ast_nodes_AST *op = ast_nodes_AST_new_binop(ast_nodes_ASTType_from_token(tok->type), lhs, rhs);
    if (((bool)root)) {
      root=ast_nodes_AST_new_binop(ast_nodes_ASTType_And, root, op);
    }  else {
      root=op;
    } 
  }
  std_vector_Vector__12_free(operands);
  std_vector_Vector__6_free(operators);
  return root;
}

ast_nodes_AST *parser_Parser_parse_logical_and(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_relational(this, end_type);
  while (parser_Parser_token_is(this, tokens_TokenType_And)) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ast_nodes_ASTType_from_token(parser_Parser_token(this)->type);
    this->curr+=((u32)1);
    ast_nodes_AST *rhs = parser_Parser_parse_relational(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_logical_or(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_logical_and(this, end_type);
  while (parser_Parser_token_is(this, tokens_TokenType_Or)) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ast_nodes_ASTType_from_token(parser_Parser_token(this)->type);
    this->curr+=((u32)1);
    ast_nodes_AST *rhs = parser_Parser_parse_logical_and(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_expression(parser_Parser *this, tokens_TokenType end_type) {
  ast_nodes_AST *lhs = parser_Parser_parse_logical_or(this, end_type);
  while (((((parser_Parser_token_is(this, tokens_TokenType_Equals) || parser_Parser_token_is(this, tokens_TokenType_PlusEquals)) || parser_Parser_token_is(this, tokens_TokenType_MinusEquals)) || parser_Parser_token_is(this, tokens_TokenType_StarEquals)) || parser_Parser_token_is(this, tokens_TokenType_SlashEquals))) {
    if (parser_Parser_token_is(this, end_type)) 
    break;
    
    ast_nodes_ASTType op = ast_nodes_ASTType_from_token(parser_Parser_token(this)->type);
    this->curr+=((u32)1);
    ast_nodes_AST *rhs = parser_Parser_parse_expression(this, end_type);
    lhs=ast_nodes_AST_new_binop(op, lhs, rhs);
  }
  return lhs;
}

ast_nodes_AST *parser_Parser_parse_if(parser_Parser *this) {
  std_span_Span start_span = parser_Parser_token(this)->span;
  parser_Parser_consume(this, tokens_TokenType_If);
  ast_nodes_AST *cond = parser_Parser_parse_expression(this, tokens_TokenType_Newline);
  parser_Parser_consume_if(this, tokens_TokenType_Then);
  ast_nodes_AST *body = parser_Parser_parse_statement(this);
  std_span_Span end_span = body->span;
  ast_nodes_AST *els = ((ast_nodes_AST *)NULL);
  if (parser_Parser_consume_if(this, tokens_TokenType_Else)) {
    els=parser_Parser_parse_statement(this);
    end_span=els->span;
  } 
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_If, std_span_Span_join(start_span, end_span));
  node->u.if_stmt.cond=cond;
  node->u.if_stmt.body=body;
  node->u.if_stmt.els=els;
  return node;
}

ast_nodes_AST *parser_Parser_parse_for_each(parser_Parser *this, std_span_Span start_span) {
  tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
  parser_Parser_consume(this, tokens_TokenType_Colon);
  ast_nodes_AST *expr = parser_Parser_parse_expression(this, tokens_TokenType_Newline);
  char *iter_var_name = "__iter";
  ast_nodes_AST *init = ({ ast_nodes_AST *__yield_0;
    ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_VarDeclaration, expr->span);
    ast_nodes_Variable *var = ast_nodes_Variable_new(NULL);
    var->sym=ast_scopes_Symbol_from_local_variable(iter_var_name, var, name->span);
    node->u.var_decl.var=var;
    node->u.var_decl.init=expr;
    __yield_0 = node;
  ;__yield_0; });
  ast_nodes_AST *cond = ({ ast_nodes_AST *__yield_0;
    ast_nodes_AST *iter_name = ast_nodes_AST_new(ast_nodes_ASTType_Identifier, expr->span);
    iter_name->u.ident.name=iter_var_name;
    ast_nodes_AST *member = ast_nodes_AST_new(ast_nodes_ASTType_Member, expr->span);
    member->u.member.lhs=iter_name;
    member->u.member.rhs_name="has_value";
    member->u.member.rhs_span=expr->span;
    ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Call, expr->span);
    node->u.call.callee=member;
    node->u.call.args=std_vector_Vector__16_new(((u32)16));
    __yield_0 = node;
  ;__yield_0; });
  ast_nodes_AST *step = ({ ast_nodes_AST *__yield_0;
    ast_nodes_AST *iter_name = ast_nodes_AST_new(ast_nodes_ASTType_Identifier, expr->span);
    iter_name->u.ident.name=iter_var_name;
    ast_nodes_AST *member = ast_nodes_AST_new(ast_nodes_ASTType_Member, expr->span);
    member->u.member.lhs=iter_name;
    member->u.member.rhs_name="next";
    member->u.member.rhs_span=expr->span;
    ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Call, expr->span);
    node->u.call.callee=member;
    node->u.call.args=std_vector_Vector__16_new(((u32)16));
    __yield_0 = node;
  ;__yield_0; });
  ast_nodes_AST *loop_var_decl = ({ ast_nodes_AST *__yield_0;
    ast_nodes_Variable *var = ast_nodes_Variable_new(NULL);
    var->sym=ast_scopes_Symbol_from_local_variable(name->text, var, name->span);
    ast_nodes_AST *iter_name = ast_nodes_AST_new(ast_nodes_ASTType_Identifier, name->span);
    iter_name->u.ident.name=iter_var_name;
    ast_nodes_AST *member = ast_nodes_AST_new(ast_nodes_ASTType_Member, expr->span);
    member->u.member.lhs=iter_name;
    member->u.member.rhs_name="cur";
    member->u.member.rhs_span=expr->span;
    ast_nodes_AST *call = ast_nodes_AST_new(ast_nodes_ASTType_Call, expr->span);
    call->u.call.callee=member;
    call->u.call.args=std_vector_Vector__16_new(((u32)16));
    ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_VarDeclaration, expr->span);
    node->u.var_decl.var=var;
    node->u.var_decl.init=call;
    __yield_0 = node;
  ;__yield_0; });
  ast_nodes_AST *inner_body = parser_Parser_parse_block(this);
  std_vector_Vector__12 *statements = std_vector_Vector__12_new(((u32)16));
  std_vector_Vector__12_push(statements, loop_var_decl);
  std_vector_Vector__12_push(statements, inner_body);
  ast_nodes_AST *body = ast_nodes_AST_new(ast_nodes_ASTType_Block, inner_body->span);
  body->u.block.statements=statements;
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_For, std_span_Span_join(start_span, body->span));
  node->u.loop.init=init;
  node->u.loop.cond=cond;
  node->u.loop.step=step;
  node->u.loop.body=body;
  return node;
}

ast_nodes_AST *parser_Parser_parse_for(parser_Parser *this) {
  tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_For);
  if ((parser_Parser_token_is(this, tokens_TokenType_Identifier) && parser_Parser_peek(this, 1)->type==tokens_TokenType_Colon)) {
    return parser_Parser_parse_for_each(this, tok->span);
  } 
  ast_nodes_AST *init = ((ast_nodes_AST *)NULL);
  if (parser_Parser_token_is(this, tokens_TokenType_Let)) {
    init=parser_Parser_parse_statement(this);
  }  else {
    init=parser_Parser_parse_expression(this, tokens_TokenType_Semicolon);
    parser_Parser_consume(this, tokens_TokenType_Semicolon);
  } 
  ast_nodes_AST *cond = parser_Parser_parse_expression(this, tokens_TokenType_Semicolon);
  parser_Parser_consume(this, tokens_TokenType_Semicolon);
  ast_nodes_AST *step = parser_Parser_parse_expression(this, tokens_TokenType_OpenCurly);
  ast_nodes_AST *body = parser_Parser_parse_block(this);
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_For, std_span_Span_join(tok->span, body->span));
  node->u.loop.init=init;
  node->u.loop.cond=cond;
  node->u.loop.step=step;
  node->u.loop.body=body;
  return node;
}

ast_nodes_AST *parser_Parser_parse_statement(parser_Parser *this) {
  ast_nodes_AST *node = ((ast_nodes_AST *)NULL);
  std_span_Span start_span = parser_Parser_token(this)->span;
  switch (parser_Parser_token(this)->type) {
    case tokens_TokenType_OpenCurly: {
      node=parser_Parser_parse_block(this);
    } break;
    case tokens_TokenType_Return: {
      parser_Parser_consume(this, tokens_TokenType_Return);
      ast_nodes_AST *expr = ((ast_nodes_AST *)NULL);
      if (!parser_Parser_token(this)->seen_newline) {
        expr=parser_Parser_parse_expression(this, tokens_TokenType_Newline);
      } 
      node=ast_nodes_AST_new_unop(ast_nodes_ASTType_Return, std_span_Span_join(start_span, parser_Parser_token(this)->span), expr);
      parser_Parser_consume_end_of_statement(this);
    } break;
    case tokens_TokenType_Yield: {
      parser_Parser_consume(this, tokens_TokenType_Yield);
      ast_nodes_AST *expr = parser_Parser_parse_expression(this, tokens_TokenType_Newline);
      node=ast_nodes_AST_new_unop(ast_nodes_ASTType_Yield, std_span_Span_join(start_span, parser_Parser_token(this)->span), expr);
      parser_Parser_consume_end_of_statement(this);
    } break;
    case tokens_TokenType_Break: {
      parser_Parser_consume(this, tokens_TokenType_Break);
      node=ast_nodes_AST_new(ast_nodes_ASTType_Break, std_span_Span_join(start_span, parser_Parser_token(this)->span));
      parser_Parser_consume_end_of_statement(this);
    } break;
    case tokens_TokenType_Continue: {
      parser_Parser_consume(this, tokens_TokenType_Continue);
      node=ast_nodes_AST_new(ast_nodes_ASTType_Continue, std_span_Span_join(start_span, parser_Parser_token(this)->span));
      parser_Parser_consume_end_of_statement(this);
    } break;
    case tokens_TokenType_While: {
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_While);
      ast_nodes_AST *cond = parser_Parser_parse_expression(this, tokens_TokenType_OpenCurly);
      ast_nodes_AST *body = parser_Parser_parse_block(this);
      node=ast_nodes_AST_new(ast_nodes_ASTType_While, std_span_Span_join(tok->span, body->span));
      node->u.loop.cond=cond;
      node->u.loop.body=body;
    } break;
    case tokens_TokenType_Assert: {
      tokens_Token *start = parser_Parser_consume(this, tokens_TokenType_Assert);
      ast_nodes_AST *expr = parser_Parser_parse_expression(this, tokens_TokenType_Newline);
      ast_nodes_AST *msg = ((ast_nodes_AST *)NULL);
      std_span_Span end_span = expr->span;
      if (parser_Parser_consume_if(this, tokens_TokenType_Comma)) {
        msg=parser_Parser_parse_expression(this, tokens_TokenType_Newline);
        end_span=msg->span;
      } 
      ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Assert, std_span_Span_join(start->span, end_span));
      node->u.assertion.expr=expr;
      node->u.assertion.msg=msg;
      return node;
    } break;
    case tokens_TokenType_Defer: {
      parser_Parser_consume(this, tokens_TokenType_Defer);
      ast_nodes_AST *expr = parser_Parser_parse_expression(this, tokens_TokenType_Newline);
      node=ast_nodes_AST_new_unop(ast_nodes_ASTType_Defer, std_span_Span_join(start_span, parser_Parser_token(this)->span), expr);
      parser_Parser_consume_end_of_statement(this);
    } break;
    case tokens_TokenType_Import: {
      node=parser_Parser_parse_import(this);
      parser_Parser_consume_end_of_statement(this);
    } break;
    case tokens_TokenType_For: {
      node=parser_Parser_parse_for(this);
    } break;
    case tokens_TokenType_Let: {
      parser_Parser_consume(this, tokens_TokenType_Let);
      tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
      std_span_Span end_span = name->span;
      types_Type *type = ((types_Type *)NULL);
      if (parser_Parser_consume_if(this, tokens_TokenType_Colon)) {
        type=parser_Parser_parse_type(this);
        end_span=type->span;
      } 
      ast_nodes_AST *init = ((ast_nodes_AST *)NULL);
      if (parser_Parser_consume_if(this, tokens_TokenType_Equals)) {
        init=parser_Parser_parse_expression(this, tokens_TokenType_Newline);
        end_span=init->span;
      } 
      parser_Parser_consume_end_of_statement(this);
      node=ast_nodes_AST_new(ast_nodes_ASTType_VarDeclaration, std_span_Span_join(start_span, end_span));
      ast_nodes_Variable *var = ast_nodes_Variable_new(type);
      var->sym=ast_scopes_Symbol_from_local_variable(name->text, var, name->span);
      node->u.var_decl.var=var;
      node->u.var_decl.init=init;
    } break;
    default: {
      node=parser_Parser_parse_expression(this, tokens_TokenType_Newline);
      parser_Parser_consume_if(this, tokens_TokenType_Semicolon);
    } break;
  }
  return node;
}

ast_nodes_AST *parser_Parser_parse_block(parser_Parser *this) {
  tokens_Token *start = parser_Parser_consume(this, tokens_TokenType_OpenCurly);
  std_vector_Vector__12 *statements = std_vector_Vector__12_new(((u32)16));
  while (!parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) {
    ast_nodes_AST *statement = parser_Parser_parse_statement(this);
    std_vector_Vector__12_push(statements, statement);
  }
  tokens_Token *end = parser_Parser_consume(this, tokens_TokenType_CloseCurly);
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Block, std_span_Span_join(start->span, end->span));
  node->u.block.statements=statements;
  return node;
}

std_vector_Vector__4 *parser_Parser_parse_template_params(parser_Parser *this, std_span_Span *out_span) {
  std_span_Span start = parser_Parser_consume(this, tokens_TokenType_LessThan)->span;
  std_vector_Vector__4 *params = std_vector_Vector__4_new(((u32)16));
  while (!parser_Parser_token_is(this, tokens_TokenType_GreaterThan)) {
    tokens_Token *type = parser_Parser_consume(this, tokens_TokenType_Identifier);
    ast_nodes_Variable *var = ast_nodes_Variable_new(types_Type_new_unresolved(type->text, type->span));
    var->sym=ast_scopes_Symbol_from_local_variable(type->text, var, type->span);
    std_vector_Vector__4_push(params, var);
    if (!parser_Parser_token_is(this, tokens_TokenType_GreaterThan)) {
      parser_Parser_consume(this, tokens_TokenType_Comma);
    } 
  }
  std_span_Span end = parser_Parser_consume(this, tokens_TokenType_GreaterThan)->span;
  if (((bool)out_span)) {
    *out_span=std_span_Span_join(start, end);
  } 
  return params;
}

ast_nodes_Function *parser_Parser_parse_function(parser_Parser *this) {
  tokens_Token *start = parser_Parser_consume(this, tokens_TokenType_Def);
  types_Type *parent_type = ((types_Type *)NULL);
  bool is_method = false;
  bool is_static = true;
  ast_nodes_AST *ident = parser_Parser_parse_scoped_identifier(this);
  if (!((bool)ident)) 
  return NULL;
  
  std_span_Span name_span = ident->span;
  ast_nodes_Function *func = ast_nodes_Function_new();
  char *name = ({ char *__yield_0;
    switch (ident->type) {
      case ast_nodes_ASTType_Identifier: {
        __yield_0 = ident->u.ident.name;
      } break;
      case ast_nodes_ASTType_NSLookup: {
        parent_type=types_Type_new_unresolved("<unresolved>", ident->span);
        parent_type->u.unresolved=ident->u.lookup.lhs;
        is_method=true;
        __yield_0 = ident->u.lookup.rhs_name;
      } break;
      default: {
        parser_Parser_error(this, errors_Error_new(ident->span, "Expected identifier"));
        __yield_0 = "<error>";
      } break;
    }
;__yield_0; });
  func->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Function, this->ns, this->ns->sym, name, name_span);
  func->sym->u.func=func;
  if (((bool)start->comment)) {
    func->sym->comment=start->comment;
  } 
  parser_Parser_consume(this, tokens_TokenType_OpenParen);
  while (!parser_Parser_token_is(this, tokens_TokenType_CloseParen)) {
    bool found_amp = parser_Parser_consume_if(this, tokens_TokenType_Ampersand);
    tokens_Token *var_name = parser_Parser_consume(this, tokens_TokenType_Identifier);
    types_Type *type = ((types_Type *)NULL);
    if ((std_vector_Vector__4_is_empty(func->params) && is_method)) {
      if (str_eq(var_name->text, "this")) {
        type=parent_type;
        if (found_amp) {
          type=types_Type_new_resolved(types_BaseType_Pointer, parent_type->span);
          type->u.ptr=parent_type;
        } 
        is_static=false;
      }  else       if (found_amp) {
        parser_Parser_error(this, errors_Error_new(var_name->span, "Expected 'this' over here"));
      } 
      
    } 
    if (!((bool)type)) {
      parser_Parser_consume(this, tokens_TokenType_Colon);
      type=parser_Parser_parse_type(this);
    } 
    ast_nodes_AST *default_value = ((ast_nodes_AST *)NULL);
    if (parser_Parser_consume_if(this, tokens_TokenType_Equals)) {
      default_value=parser_Parser_parse_expression(this, tokens_TokenType_Comma);
    } 
    ast_nodes_Variable *var = ast_nodes_Variable_new(type);
    var->sym=ast_scopes_Symbol_from_local_variable(var_name->text, var, var_name->span);
    var->default_value=default_value;
    std_vector_Vector__4_push(func->params, var);
    if (((bool)var_name->comment)) {
      var->sym->comment=var_name->comment;
    } 
    if (!parser_Parser_token_is(this, tokens_TokenType_CloseParen)) {
      parser_Parser_consume(this, tokens_TokenType_Comma);
    } 
  }
  parser_Parser_consume(this, tokens_TokenType_CloseParen);
  if (parser_Parser_consume_if(this, tokens_TokenType_Colon)) {
    func->return_type=parser_Parser_parse_type(this);
  }  else   if (str_eq(name, "main")) {
    func->return_type=types_Type_new_unresolved_base(types_BaseType_I32, name_span);
  }  else {
    func->return_type=types_Type_new_unresolved_base(types_BaseType_Void, name_span);
    if ((parser_Parser_token_is(this, tokens_TokenType_Identifier) && str_eq(parser_Parser_token(this)->text, "exits"))) {
      parser_Parser_consume(this, tokens_TokenType_Identifier);
      func->exits=true;
    } 
  } 
  
  func->is_method=is_method;
  func->is_static=is_static;
  func->parent_type=parent_type;
  parser_Parser_parse_extern_into_symbol(this, func->sym);
  if (func->sym->is_extern) 
  return func;
  
  this->curr_func=func;
  if (parser_Parser_token_is(this, tokens_TokenType_FatArrow)) {
    tokens_Token *arrow = parser_Parser_consume(this, tokens_TokenType_FatArrow);
    ast_nodes_AST *expr = parser_Parser_parse_expression(this, tokens_TokenType_Newline);
    ast_nodes_AST *ret = ast_nodes_AST_new(ast_nodes_ASTType_Return, expr->span);
    ret->u.unary=expr;
    ast_nodes_AST *body = ast_nodes_AST_new(ast_nodes_ASTType_Block, ret->span);
    std_vector_Vector__12 *statements = std_vector_Vector__12_new(((u32)16));
    std_vector_Vector__12_push(statements, ret);
    body->u.block.statements=statements;
    func->body=body;
  }  else {
    func->body=parser_Parser_parse_block(this);
  } 
  this->curr_func=NULL;
  func->span=std_span_Span_join(start->span, func->body->span);
  return func;
}

void parser_Parser_parse_extern_into_symbol(parser_Parser *this, ast_scopes_Symbol *sym) {
  if (!parser_Parser_consume_if(this, tokens_TokenType_Extern)) 
  return ;
  
  sym->is_extern=true;
  if (parser_Parser_token_is(this, tokens_TokenType_OpenParen)) {
    parser_Parser_consume(this, tokens_TokenType_OpenParen);
    tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_StringLiteral);
    parser_Parser_consume(this, tokens_TokenType_CloseParen);
    sym->out_name=name->text;
  }  else {
    sym->out_name=sym->name;
  } 
}

std_vector_Vector__5 *parser_Parser_parse_import_path(parser_Parser *this) {
  std_vector_Vector__5 *parts = std_vector_Vector__5_new(((u32)16));
  while (true) {
    bool done = false;
    if (tokens_Token_is_word(*parser_Parser_token(this))) {
      tokens_Token *word = parser_Parser_token(this);
      this->curr+=((u32)1);
      ast_nodes_ImportPart *part = ast_nodes_ImportPart_new(ast_nodes_ImportPartType_Single, word->span);
      part->u.single.name=word->text;
      if (parser_Parser_consume_if(this, tokens_TokenType_As)) {
        tokens_Token *alias = parser_Parser_consume(this, tokens_TokenType_Identifier);
        part->u.single.alias=alias->text;
        part->u.single.alias_span=alias->span;
        done=true;
      } 
      std_vector_Vector__5_push(parts, part);
    }  else     if (parser_Parser_token_is(this, tokens_TokenType_Star)) {
      tokens_Token *tok = parser_Parser_consume(this, tokens_TokenType_Star);
      ast_nodes_ImportPart *part = ast_nodes_ImportPart_new(ast_nodes_ImportPartType_Wildcard, tok->span);
      std_vector_Vector__5_push(parts, part);
      done=true;
    }  else     if (parser_Parser_token_is(this, tokens_TokenType_OpenCurly)) {
      tokens_Token *open = parser_Parser_consume(this, tokens_TokenType_OpenCurly);
      std_vector_Vector__17 *sub_paths = std_vector_Vector__17_new(((u32)16));
      while (!parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) {
        std_vector_Vector__5 *sub_path = parser_Parser_parse_import_path(this);
        if (!((bool)sub_path)) 
        return NULL;
        
        std_vector_Vector__17_push(sub_paths, sub_path);
        if (!parser_Parser_consume_if(this, tokens_TokenType_Comma)) 
        break;
        
      }
      tokens_Token *close = parser_Parser_consume(this, tokens_TokenType_CloseCurly);
      ast_nodes_ImportPart *part = ast_nodes_ImportPart_new(ast_nodes_ImportPartType_Multiple, std_span_Span_join(open->span, close->span));
      part->u.paths=sub_paths;
      std_vector_Vector__5_push(parts, part);
      done=true;
    }  else {
      parser_Parser_error(this, errors_Error_new(parser_Parser_token(this)->span, "Expected identifier"));
      return NULL;
    } 
    
    
    if (done) 
    break;
    
    if (!parser_Parser_consume_if(this, tokens_TokenType_ColonColon)) 
    break;
    
  }
  return parts;
}

ast_nodes_AST *parser_Parser_parse_import(parser_Parser *this) {
  std_span_Span span = parser_Parser_token(this)->span;
  parser_Parser_consume(this, tokens_TokenType_Import);
  u32 parent_count = ((u32)0);
  ast_nodes_ImportType type = ({ ast_nodes_ImportType __yield_0;
    switch (parser_Parser_token(this)->type) {
      case tokens_TokenType_AtSign: {
        parser_Parser_consume(this, tokens_TokenType_AtSign);
        __yield_0 = ast_nodes_ImportType_ProjectNamespace;
      } break;
      case tokens_TokenType_ColonColon: {
        parser_Parser_consume(this, tokens_TokenType_ColonColon);
        __yield_0 = ast_nodes_ImportType_CurrentScope;
      } break;
      case tokens_TokenType_Dot: {
        while (parser_Parser_consume_if(this, tokens_TokenType_Dot)) {
          parent_count+=((u32)1);
        }
        __yield_0 = ast_nodes_ImportType_ParentNamespace;
      } break;
      default: {
        __yield_0 = ast_nodes_ImportType_GlobalNamespace;
      } break;
    }
;__yield_0; });
  if (this->ns->is_top_level) {
    parent_count-=((u32)1);
  } 
  std_vector_Vector__5 *parts = parser_Parser_parse_import_path(this);
  if (!((bool)parts)) 
  return NULL;
  
  if (parts->size==((u32)0)) {
    parser_Parser_error(this, errors_Error_new(span, "Invalid import statement"));
    return NULL;
  } 
  ast_nodes_AST *node = ast_nodes_AST_new(ast_nodes_ASTType_Import, span);
  node->u.import_path=(ast_nodes_Import){.parts=parts, .type=type, .parent_count=parent_count};
  parser_Parser_load_import_path(this, node);
  return node;
}

ast_nodes_Structure *parser_Parser_parse_struct(parser_Parser *this) {
  tokens_Token *start = parser_Parser_token(this);
  bool is_union = ({ bool __yield_0;
    if (parser_Parser_token_is(this, tokens_TokenType_Union)) {
      parser_Parser_consume(this, tokens_TokenType_Union);
      __yield_0 = true;
    }  else {
      parser_Parser_consume(this, tokens_TokenType_Struct);
      __yield_0 = false;
    } 
;__yield_0; });
  tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
  ast_nodes_Structure *struc = ast_nodes_Structure_new();
  struc->is_union=is_union;
  struc->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Structure, this->ns, this->ns->sym, name->text, name->span);
  struc->sym->u.struc=struc;
  if (((bool)start->comment)) {
    struc->sym->comment=start->comment;
  } 
  if (parser_Parser_token_is(this, tokens_TokenType_LessThan)) {
    struc->is_templated=true;
    struc->template_params=parser_Parser_parse_template_params(this, NULL);
    struc->template_instances=std_vector_Vector__15_new(((u32)16));
  } 
  parser_Parser_parse_extern_into_symbol(this, struc->sym);
  if ((!struc->sym->is_extern || parser_Parser_token_is(this, tokens_TokenType_OpenCurly))) {
    parser_Parser_consume(this, tokens_TokenType_OpenCurly);
    while (!parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) {
      tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
      parser_Parser_consume(this, tokens_TokenType_Colon);
      types_Type *type = parser_Parser_parse_type(this);
      ast_nodes_Variable *var = ast_nodes_Variable_new(type);
      var->sym=ast_scopes_Symbol_from_local_variable(name->text, var, name->span);
      if (((bool)name->comment)) {
        var->sym->comment=name->comment;
      } 
      std_vector_Vector__4_push(struc->fields, var);
      if (!parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) {
        parser_Parser_consume_newline_or(this, tokens_TokenType_Comma);
      } 
    }
    tokens_Token *end = parser_Parser_consume(this, tokens_TokenType_CloseCurly);
    struc->span=std_span_Span_join(start->span, end->span);
  } 
  return struc;
}

ast_nodes_Enum *parser_Parser_parse_enum(parser_Parser *this) {
  tokens_Token *start = parser_Parser_consume(this, tokens_TokenType_Enum);
  tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
  ast_nodes_Enum *enum_def = ast_nodes_Enum_new();
  enum_def->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Enum, this->ns, this->ns->sym, name->text, start->span);
  enum_def->sym->u.enum_=enum_def;
  if (((bool)start->comment)) {
    enum_def->sym->comment=start->comment;
  } 
  parser_Parser_parse_extern_into_symbol(this, enum_def->sym);
  parser_Parser_consume(this, tokens_TokenType_OpenCurly);
  while (!parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) {
    tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
    ast_nodes_Variable *var = ast_nodes_Variable_new(NULL);
    var->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Variable, this->ns, enum_def->sym, name->text, name->span);
    var->sym->u.var=var;
    if (((bool)name->comment)) {
      var->sym->comment=name->comment;
    } 
    if (parser_Parser_consume_if(this, tokens_TokenType_Equals)) {
      parser_Parser_parse_extern_into_symbol(this, var->sym);
    } 
    std_vector_Vector__4_push(enum_def->fields, var);
    if (!parser_Parser_token_is(this, tokens_TokenType_CloseCurly)) {
      parser_Parser_consume_newline_or(this, tokens_TokenType_Comma);
    } 
  }
  tokens_Token *end = parser_Parser_consume(this, tokens_TokenType_CloseCurly);
  enum_def->span=std_span_Span_join(start->span, end->span);
  return enum_def;
}

void parser_Parser_parse_namespace_until(parser_Parser *this, tokens_TokenType end_type) {
  if (((bool)parser_Parser_token(this)->comment)) {
    this->ns->sym->comment=parser_Parser_token(this)->comment;
  } 
  while (!parser_Parser_token_is(this, end_type)) {
    switch (parser_Parser_token(this)->type) {
      case tokens_TokenType_Def: {
        ast_nodes_Function *func = parser_Parser_parse_function(this);
        if (((bool)func)) 
        std_vector_Vector__10_push(this->ns->functions, func);
        
      } break;
      case tokens_TokenType_Import: {
        ast_nodes_AST *import_ = parser_Parser_parse_import(this);
        if (((bool)import_)) {
          std_vector_Vector__12_push(this->ns->imports, import_);
        } 
      } break;
      case tokens_TokenType_Namespace: {
        std_span_Span start = parser_Parser_consume(this, tokens_TokenType_Namespace)->span;
        tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
        ast_program_Namespace *old_ns = this->ns;
        ast_program_Namespace *new_ns = ast_program_Namespace_new(this->ns, this->ns->path);
        new_ns->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Namespace, old_ns, old_ns->sym, name->text, name->span);
        new_ns->sym->u.ns=new_ns;
        new_ns->always_add_to_scope=true;
        std_map_Map__2_insert(old_ns->namespaces, name->text, new_ns);
        this->ns=new_ns;
        parser_Parser_consume(this, tokens_TokenType_OpenCurly);
        parser_Parser_parse_namespace_until(this, tokens_TokenType_CloseCurly);
        std_span_Span end = parser_Parser_consume(this, tokens_TokenType_CloseCurly)->span;
        new_ns->span=std_span_Span_join(start, end);
        this->ns=old_ns;
      } break;
      case tokens_TokenType_Struct:
      case tokens_TokenType_Union: {
        ast_nodes_Structure *struc = parser_Parser_parse_struct(this);
        if (((bool)struc)) 
        std_vector_Vector__11_push(this->ns->structs, struc);
        
      } break;
      case tokens_TokenType_Enum: {
        ast_nodes_Enum *enum_value = parser_Parser_parse_enum(this);
        if (((bool)enum_value)) 
        std_vector_Vector__13_push(this->ns->enums, enum_value);
        
      } break;
      case tokens_TokenType_Let: {
        ast_nodes_AST *var = parser_Parser_parse_global_value(this, false);
        if (((bool)var)) 
        std_vector_Vector__12_push(this->ns->variables, var);
        
      } break;
      case tokens_TokenType_Const: {
        ast_nodes_AST *con = parser_Parser_parse_global_value(this, true);
        if (((bool)con)) 
        std_vector_Vector__12_push(this->ns->constants, con);
        
      } break;
      case tokens_TokenType_AtSign: {
        parser_Parser_parse_compiler_option(this);
      } break;
      default: {
        parser_Parser_error(this, errors_Error_new(parser_Parser_token(this)->span, format_string("Unexpected token '%s' in Parser", parser_Parser_token(this)->text)));
        this->curr+=((u32)1);
      } break;
    }
  }
}

void parser_Parser_parse_compiler_option(parser_Parser *this) {
  parser_Parser_consume(this, tokens_TokenType_AtSign);
  tokens_Token *compiler = parser_Parser_consume(this, tokens_TokenType_Identifier);
  if (!str_eq(compiler->text, "compiler")) {
    parser_Parser_error(this, errors_Error_new(compiler->span, "Expected 'compiler'"));
  } 
  tokens_Token *name = parser_Parser_consume(this, tokens_TokenType_Identifier);
  {
    char *__match_str = name->text;
    if (!strcmp(__match_str, "c_include")) {
      tokens_Token *filename = parser_Parser_consume(this, tokens_TokenType_StringLiteral);
      std_vector_Vector__7_push(this->program->c_includes, filename->text);
    } else if (!strcmp(__match_str, "c_flag")) {
      tokens_Token *flag = parser_Parser_consume(this, tokens_TokenType_StringLiteral);
      std_vector_Vector__7_push(this->program->c_flags, flag->text);
    } else if (!strcmp(__match_str, "c_embed")) {
      tokens_Token *path = parser_Parser_consume(this, tokens_TokenType_StringLiteral);
      char *cur_dir = ({ char *__yield_0;
        if (this->ns->is_top_level) {
          __yield_0 = this->ns->path;
        }  else {
          __yield_0 = this->ns->parent->path;
        } 
;__yield_0; });
      char *full_path = format_string("%s/%s", cur_dir, path->text);
      if (!FILE_exists(full_path)) {
        parser_Parser_error(this, errors_Error_new(path->span, format_string("File '%s' does not exist", full_path)));
        return ;
      } 
      FILE *file = FILE_open(full_path, "r");
      char *contents = FILE_slurp(file);
      std_map_Map__3_insert(this->program->c_embeds, full_path, contents);
      /* defers */
      fclose(file);
    } else  {
      parser_Parser_error(this, errors_Error_new(name->span, "Unknown compiler option"));
    }
  }
}

bool parser_Parser_load_import_path_from_base(parser_Parser *this, std_vector_Vector__5 *parts, ast_program_Namespace *base) {
  for (u32 i = ((u32)0); ((i < parts->size) && (!base->is_a_file || base->is_top_level)); i+=((u32)1)) {
    ast_nodes_ImportPart *part = std_vector_Vector__5_at(parts, i);
    switch (part->type) {
      case ast_nodes_ImportPartType_Wildcard: {
        parser_Parser_error(this, errors_Error_new(part->span, format_string("Wildcard import is not allowed from non-module")));
        return false;
      } break;
      case ast_nodes_ImportPartType_Multiple: {
        std_vector_Vector__17 *paths = part->u.paths;
        bool success = true;
        for (std_vector_Iterator__17 __iter = std_vector_Vector__17_iter(paths); std_vector_Iterator__17_has_value(&__iter); std_vector_Iterator__17_next(&__iter)) {
          std_vector_Vector__5 *path = std_vector_Iterator__17_cur(&__iter);
          {
            success=(parser_Parser_load_import_path_from_base(this, path, base) && success);
          }
        }
        return success;
      } break;
      case ast_nodes_ImportPartType_Single: {
      } break;
    }
    char *part_name = part->u.single.name;
    ast_scopes_Symbol *sym = ast_program_Namespace_find_importable_symbol(base, part_name);
    if ((((bool)sym) && (sym->type != ast_scopes_SymbolType_Namespace))) 
    return true;
    
    ast_program_Namespace *next = (((bool)sym) ? sym->u.ns : NULL);
    char *part_path = format_string("%s/%s", base->path, part_name);
    if (!((bool)next)) {
      bool dir_exists = utils_directory_exists(part_path);
      char *path = format_string("%s/%s.oc", base->path, part_name);
      bool file_exists = FILE_exists(path);
      if ((!dir_exists && !file_exists)) {
        parser_Parser_error(this, errors_Error_new(part->span, format_string("Could not find import path %s(.oc)", part_path)));
        ast_program_Program_exit_with_errors(this->program);
      } 
      next=ast_program_Namespace_new(base, part_path);
      next->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Namespace, base, base->sym, part_name, part->span);
      next->sym->u.ns=next;
      std_map_Map__2_insert(base->namespaces, part_name, next);
      if (file_exists) {
        parser_Parser parser = parser_Parser_make(this->program, next);
        parser_Parser_load_file(&parser, strdup(path));
      } 
      free(path);
    } 
    base=next;
  }
  return true;
}

bool parser_Parser_load_import_path(parser_Parser *this, ast_nodes_AST *import_stmt) {
  ast_nodes_Import *path = &import_stmt->u.import_path;
  ast_program_Namespace *base = ({ ast_program_Namespace *__yield_0;
    switch (path->type) {
      case ast_nodes_ImportType_CurrentScope: {
        return true;
      } break;
      case ast_nodes_ImportType_GlobalNamespace: {
        __yield_0 = this->program->global;
      } break;
      case ast_nodes_ImportType_ProjectNamespace: {
        __yield_0 = this->program->project_root;
      } break;
      case ast_nodes_ImportType_ParentNamespace: {
        ast_program_Namespace *cur = this->ns;
        for (u32 i = ((u32)0); (i < path->parent_count); i+=((u32)1)) {
          if (!((bool)cur->parent)) {
            ast_nodes_ImportPart *first_part = std_vector_Vector__5_at(path->parts, ((u32)0));
            parser_Parser_error(this, errors_Error_new(first_part->span, "Cannot import from parent of root namespace"));
            ast_program_Program_exit_with_errors(this->program);
          } 
          cur=cur->parent;
        }
        __yield_0 = cur;
      } break;
    }
;__yield_0; });
  return parser_Parser_load_import_path_from_base(this, path->parts, base);
}

void parser_Parser_load_file(parser_Parser *this, char *filename) {
  FILE *file = FILE_open(filename, "r");
  char *contents = FILE_slurp(file);
  std_map_Map__3_insert(this->program->sources, filename, contents);
  lexer_Lexer lexer = lexer_Lexer_make(contents, filename);
  this->tokens=lexer_Lexer_lex(&lexer);
  this->curr=((u32)0);
  this->ns->is_a_file=true;
  std_span_Span start = parser_Parser_token(this)->span;
  parser_Parser_parse_namespace_until(this, tokens_TokenType_EOF);
  std_span_Span end = parser_Parser_token(this)->span;
  this->ns->span=std_span_Span_join(start, end);
}

void parser_couldnt_find_stdlib(void) {
  printf("--------------------------------------------------------------------------------""\n");
  printf("    Could not find standard library. Set OCEN_ROOT environment variable.""\n");
  printf("      Alternatively, compile from the root of `ocen` repository.""\n");
  printf("--------------------------------------------------------------------------------""\n");
  exit(1);
}

void parser_Parser_find_and_import_stdlib(parser_Parser *this) {
  char *ocen_root = "./";
  char *std_path = format_string("%s/std", ocen_root);
  char *prelude_path = format_string("%s/prelude.oc", std_path);
  if (!FILE_exists(prelude_path)) {
    ocen_root=getenv("OCEN_ROOT");
    if (!((bool)ocen_root)) 
    parser_couldnt_find_stdlib();
    
    free(std_path);
    free(prelude_path);
    std_path=format_string("%s/std", ocen_root);
    prelude_path=format_string("%s/prelude.oc", std_path);
    if (!FILE_exists(prelude_path)) 
    parser_couldnt_find_stdlib();
    
  } 
  ast_program_Namespace *std_ns = ast_program_Namespace_new(this->program->global, std_path);
  std_ns->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Namespace, this->program->global, this->program->global->sym, "std", std_span_Span_default());
  std_ns->sym->u.ns=std_ns;
  std_map_Map__2_insert(this->program->global->namespaces, "std", std_ns);
  std_ns->always_add_to_scope=true;
  std_ns->is_top_level=true;
  parser_Parser parser = parser_Parser_make(this->program, std_ns);
  parser_Parser_load_file(&parser, prelude_path);
}

void parser_Parser_parse_toplevel(ast_program_Program *program, char *filename) {
  char *t1 = strdup(filename);
  char *dir = strdup(dirname(t1));
  free(t1);
  char *t2 = strdup(filename);
  char *base = strdup(basename(t2));
  str_remove_last_n(base, ((u32)3));
  free(t2);
  char *t3 = strdup(dir);
  char *dir_base = strdup(basename(t3));
  free(t3);
  ast_program_Namespace *project_ns = ast_program_Namespace_new(program->global, dir);
  project_ns->sym=ast_scopes_Symbol_new(ast_scopes_SymbolType_Namespace, program->global, dir_base, "", "", std_span_Span_default());
  project_ns->sym->u.ns=project_ns;
  std_map_Map__2_insert(program->global->namespaces, dir_base, project_ns);
  program->project_root=project_ns;
  ast_program_Namespace *ns = ast_program_Namespace_new(project_ns, filename);
  ns->sym=ast_scopes_Symbol_new(ast_scopes_SymbolType_Namespace, project_ns, base, "", "", std_span_Span_default());
  ns->sym->u.ns=ns;
  std_map_Map__2_insert(project_ns->namespaces, base, ns);
  parser_Parser parser = parser_Parser_make(program, ns);
  parser_Parser_find_and_import_stdlib(&parser);
  parser_Parser_load_file(&parser, filename);
}

u32 utils_edit_distance(char *str1, char *str2) {
  u32 n = str_len(str1);
  u32 m = str_len(str2);
  u32 stride = (m + ((u32)1));
  u32 d[(n + ((u32)1))][(m + ((u32)1))];
  for (u32 i = ((u32)0); (i <= n); i+=((u32)1)) {
    d[i][((u32)0)]=i;
  }
  for (u32 j = ((u32)0); (j <= m); j+=((u32)1)) {
    d[((u32)0)][j]=j;
  }
  for (u32 i = ((u32)1); (i <= n); i+=((u32)1)) {
    for (u32 j = ((u32)1); (j <= m); j+=((u32)1)) {
      u32 x = (d[(i - ((u32)1))][j] + ((u32)1));
      u32 y = (d[i][(j - ((u32)1))] + ((u32)1));
      u32 z = ({ u32 __yield_0;
        if (str1[(i - ((u32)1))]==str2[(j - ((u32)1))]) {
          __yield_0 = d[(i - ((u32)1))][(j - ((u32)1))];
        }  else {
          __yield_0 = (d[(i - ((u32)1))][(j - ((u32)1))] + ((u32)1));
        } 
;__yield_0; });
      d[i][j]=u32_min(u32_min(x, y), z);
    }
  }
  u32 result = d[n][m];
  return result;
}

char *utils_find_word_suggestion(char *s, std_vector_Vector__7 *options) {
  u32 threshold = ((u32)5);
  if (options->size==((u32)0)) 
  return NULL;
  
  char *closest = ((char *)std_vector_Vector__7_at(options, ((u32)0)));
  u32 closest_distance = utils_edit_distance(s, closest);
  for (std_vector_Iterator__7 __iter = std_vector_Vector__7_iter(options); std_vector_Iterator__7_has_value(&__iter); std_vector_Iterator__7_next(&__iter)) {
    char *option = std_vector_Iterator__7_cur(&__iter);
    {
      u32 distance = utils_edit_distance(s, option);
      if ((distance < closest_distance)) {
        closest=option;
        closest_distance=distance;
      } 
    }
  }
  if ((closest_distance > threshold)) 
  return NULL;
  
  return closest;
}

bool utils_directory_exists(char *path) {
  DIR *dir = opendir(path);
  if (dir==NULL) 
  return false;
  
  closedir(dir);
  return true;
}

void passes_register_types_RegisterTypes_register_struct(passes_register_types_RegisterTypes *this, ast_program_Namespace *ns, ast_nodes_Structure *struc) {
  ast_scopes_Symbol *item = struc->sym;
  passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, item, NULL);
  types_Type *typ = types_Type_new_resolved(types_BaseType_Structure, struc->sym->span);
  typ->u.struc=struc;
  struc->type=typ;
  typ->sym=struc->sym;
}

void passes_register_types_RegisterTypes_register_enum(passes_register_types_RegisterTypes *this, ast_program_Namespace *ns, ast_nodes_Enum *enum_) {
  ast_scopes_Symbol *item = enum_->sym;
  passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, item, NULL);
  types_Type *typ = types_Type_new_resolved(types_BaseType_Enum, enum_->sym->span);
  typ->u.enum_=enum_;
  enum_->type=typ;
  typ->sym=enum_->sym;
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      field->type=typ;
    }
  }
}

void passes_register_types_RegisterTypes_add_dbg_method_for_enum(passes_register_types_RegisterTypes *this, ast_nodes_Enum *enum_) {
  std_span_Span span = enum_->sym->span;
  ast_nodes_Function *func = ast_nodes_Function_new();
  func->sym=ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Function, enum_->sym->ns, enum_->sym, "dbg", span);
  func->sym->u.func=func;
  func->return_type=ast_program_Program_get_type_by_name(this->o->program, "str", span);
  func->is_method=true;
  func->parent_type=enum_->type;
  ast_nodes_Variable *var = ast_nodes_Variable_new(enum_->type);
  var->sym=ast_scopes_Symbol_from_local_variable("this", var, span);
  std_vector_Vector__4_push(func->params, var);
  types_Type *typ = types_Type_new_resolved(types_BaseType_Function, span);
  typ->u.func=types_FunctionType_from_func(func);
  func->type=typ;
  std_map_Map__4_insert(enum_->type->methods, "dbg", func);
}

void passes_register_types_RegisterTypes_register_namespace(passes_register_types_RegisterTypes *this, ast_program_Namespace *ns) {
  passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(ns->structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      passes_register_types_RegisterTypes_register_struct(this, ns, struc);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->enums); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    ast_nodes_Enum *enum_ = std_vector_Iterator__13_cur(&__iter);
    {
      passes_register_types_RegisterTypes_register_enum(this, ns, enum_);
      passes_register_types_RegisterTypes_add_dbg_method_for_enum(this, enum_);
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      if (child->always_add_to_scope) {
        passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, child->sym, NULL);
      } 
      passes_register_types_RegisterTypes_register_namespace(this, child);
    }
  }
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

void passes_register_types_RegisterTypes_register_base_type(passes_register_types_RegisterTypes *this, types_BaseType base) {
  char *name = types_BaseType_str(base);
  ast_scopes_Symbol *sym = ast_scopes_Symbol_new(ast_scopes_SymbolType_TypeDef, NULL, name, name, name, std_span_Span_default());
  types_Type *typ = types_Type_new_resolved(base, std_span_Span_default());
  typ->sym=sym;
  sym->u.type_def=typ;
  std_map_Map__1_insert(this->o->program->global->scope->items, name, sym);
}

void passes_register_types_RegisterTypes_register_alias(passes_register_types_RegisterTypes *this, char *name, types_Type *orig) {
  ast_scopes_Symbol *sym = ast_scopes_Symbol_new(ast_scopes_SymbolType_TypeDef, NULL, name, name, name, std_span_Span_default());
  types_Type *alias = types_Type_new_resolved(types_BaseType_Alias, std_span_Span_default());
  alias->name=name;
  alias->u.ptr=orig;
  alias->sym=sym;
  sym->u.type_def=alias;
  std_map_Map__1_insert(this->o->program->global->scope->items, name, sym);
}

void passes_register_types_RegisterTypes_register_builtin_types(passes_register_types_RegisterTypes *this) {
  for (u32 i = ((u32)0); (i < ((u32)types_BaseType_NUM_BASE_TYPES)); i+=((u32)1)) {
    passes_register_types_RegisterTypes_register_base_type(this, ((types_BaseType)i));
  }
  {
    types_Type *base = ast_program_Program_get_base_type(this->o->program, types_BaseType_Void, std_span_Span_default());
    types_Type *typ = types_Type_new_resolved(types_BaseType_Pointer, std_span_Span_default());
    typ->u.ptr=base;
    passes_register_types_RegisterTypes_register_alias(this, "untyped_ptr", typ);
  }
  {
    types_Type *base = ast_program_Program_get_base_type(this->o->program, types_BaseType_Char, std_span_Span_default());
    types_Type *typ = types_Type_new_resolved(types_BaseType_Pointer, std_span_Span_default());
    typ->u.ptr=base;
    passes_register_types_RegisterTypes_register_alias(this, "str", typ);
  }
  this->o->error_type=types_Type_new_resolved(types_BaseType_Error, std_span_Span_default());
}

void passes_register_types_RegisterTypes_run(ast_program_Program *program) {
  passes_register_types_RegisterTypes pass = (passes_register_types_RegisterTypes){.o=passes_generic_pass_GenericPass_new(program)};
  passes_register_types_RegisterTypes_register_builtin_types(&pass);
  passes_register_types_RegisterTypes_register_namespace(&pass, program->global);
}

passes_generic_pass_GenericPass *passes_generic_pass_GenericPass_new(ast_program_Program *program) {
  passes_generic_pass_GenericPass *pass = ((passes_generic_pass_GenericPass *)calloc(((u32)1), ((u32)sizeof(passes_generic_pass_GenericPass))));
  pass->scope_stack=std_vector_Vector__8_new(((u32)16));
  pass->namespace_stack=std_vector_Vector__9_new(((u32)16));
  pass->program=program;
  return pass;
}

void passes_generic_pass_GenericPass_push_scope(passes_generic_pass_GenericPass *this, ast_scopes_Scope *scope) {
  std_vector_Vector__8_push(this->scope_stack, scope);
}

void passes_generic_pass_GenericPass_push_namespace(passes_generic_pass_GenericPass *this, ast_program_Namespace *ns) {
  std_vector_Vector__9_push(this->namespace_stack, ns);
}

ast_scopes_Scope *passes_generic_pass_GenericPass_pop_scope(passes_generic_pass_GenericPass *this) {
  if (this->scope_stack->size==((u32)0)) 
  return NULL;
  
  return ((ast_scopes_Scope *)std_vector_Vector__8_pop(this->scope_stack));
}

ast_program_Namespace *passes_generic_pass_GenericPass_pop_namespace(passes_generic_pass_GenericPass *this) {
  if (this->namespace_stack->size==((u32)0)) 
  return NULL;
  
  return ((ast_program_Namespace *)std_vector_Vector__9_pop(this->namespace_stack));
}

ast_scopes_Scope *passes_generic_pass_GenericPass_scope(passes_generic_pass_GenericPass *this) {
  if (this->scope_stack->size==((u32)0)) 
  return NULL;
  
  return std_vector_Vector__8_at(this->scope_stack, (this->scope_stack->size - ((u32)1)));
}

ast_program_Namespace *passes_generic_pass_GenericPass_ns(passes_generic_pass_GenericPass *this) {
  if (this->namespace_stack->size==((u32)0)) 
  return NULL;
  
  return std_vector_Vector__9_at(this->namespace_stack, (this->namespace_stack->size - ((u32)1)));
}

errors_Error *passes_generic_pass_GenericPass_error(passes_generic_pass_GenericPass *this, errors_Error *err) {
  std_vector_Vector__14_push(this->program->errors, err);
  return err;
}

void passes_generic_pass_GenericPass_insert_into_scope_checked(passes_generic_pass_GenericPass *this, ast_scopes_Symbol *item, char *name) {
  if (!((bool)name)) 
  name=item->name;
  
  ast_scopes_Symbol *prev = ast_scopes_Scope_lookup_local(passes_generic_pass_GenericPass_scope(this), name);
  if (((bool)prev)) {
    passes_generic_pass_GenericPass_error(this, errors_Error_new_hint(item->span, format_string("Name %s already exists in scope", name), prev->span, format_string("Previous use of %s", name)));
    return ;
  } 
  ast_scopes_Scope_insert(passes_generic_pass_GenericPass_scope(this), name, item);
}

ast_scopes_Symbol *passes_generic_pass_GenericPass_lookup_in_symbol(passes_generic_pass_GenericPass *this, ast_scopes_Symbol *sym, char *name, std_span_Span span, bool error) {
  return ({ ast_scopes_Symbol *__yield_0;
    switch (sym->type) {
      case ast_scopes_SymbolType_Namespace: {
        ast_program_Namespace *ns = sym->u.ns;
        ast_scopes_Symbol *res = ast_program_Namespace_find_importable_symbol(ns, name);
        if ((error && !((bool)res))) {
          passes_generic_pass_GenericPass_error(this, errors_Error_new_hint(span, format_string("Could not find symbol %s in namespace %s", name, sym->display), sym->span, format_string("Namespace was defined here")));
        } 
        __yield_0 = res;
      } break;
      case ast_scopes_SymbolType_Structure: {
        ast_nodes_Structure *struc = sym->u.struc;
        if (struc->is_templated) {
          passes_generic_pass_GenericPass_error(this, errors_Error_new_hint(span, format_string("Need to specify template specialization for %s", sym->display), sym->span, format_string("Template was defined here")));
        } 
        ast_nodes_Function *method = std_map_Map__4_get(struc->type->methods, name, NULL);
        if ((error && !((bool)method))) {
          passes_generic_pass_GenericPass_error(this, errors_Error_new(span, format_string("Could not find method %s in structure %s", name, sym->display)));
        } 
        if (!((bool)method)) 
        return NULL;
        
        __yield_0 = method->sym;
      } break;
      case ast_scopes_SymbolType_TypeDef: {
        types_Type *type_def = sym->u.type_def;
        ast_nodes_Function *method = std_map_Map__4_get(type_def->methods, name, NULL);
        if ((error && !((bool)method))) {
          passes_generic_pass_GenericPass_error(this, errors_Error_new(span, format_string("Could not find method %s in type %s", name, sym->display)));
        } 
        if (!((bool)method)) 
        return NULL;
        
        __yield_0 = method->sym;
      } break;
      case ast_scopes_SymbolType_Enum: {
        ast_nodes_Enum *enum_ = sym->u.enum_;
        ast_nodes_Variable *field = ast_nodes_Enum_get_field(enum_, name);
        if (((bool)field)) 
        return field->sym;
        
        ast_nodes_Function *method = std_map_Map__4_get(enum_->type->methods, name, NULL);
        if (((bool)method)) 
        return method->sym;
        
        if (error) {
          passes_generic_pass_GenericPass_error(this, errors_Error_new_hint(span, format_string("Could not find value/method %s in enum %s", name, sym->display), sym->span, format_string("Enum was defined here")));
        } 
        return NULL;
      } break;
      default: {
        passes_generic_pass_GenericPass_error(this, errors_Error_new_hint(span, format_string("Can't lookup a name inside a %s", ast_scopes_SymbolType_dbg(sym->type)), sym->span, format_string("%s was defined here", ast_scopes_SymbolType_dbg(sym->type))));
        return NULL;
      } break;
    }
;__yield_0; });
}

void passes_generic_pass_GenericPass_import_all_from_namespace(passes_generic_pass_GenericPass *this, ast_program_Namespace *ns) {
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(ns->functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
    {
      if (!func->is_method) {
        passes_generic_pass_GenericPass_insert_into_scope_checked(this, func->sym, NULL);
      } 
    }
  }
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(ns->structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      passes_generic_pass_GenericPass_insert_into_scope_checked(this, struc->sym, NULL);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->enums); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    ast_nodes_Enum *enum_ = std_vector_Iterator__13_cur(&__iter);
    {
      passes_generic_pass_GenericPass_insert_into_scope_checked(this, enum_->sym, NULL);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->variables); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      ast_nodes_Variable *var = node->u.var_decl.var;
      passes_generic_pass_GenericPass_insert_into_scope_checked(this, var->sym, NULL);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->constants); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      ast_nodes_Variable *var = node->u.var_decl.var;
      passes_generic_pass_GenericPass_insert_into_scope_checked(this, var->sym, NULL);
    }
  }
}

void passes_generic_pass_GenericPass_import_all_from_symbol(passes_generic_pass_GenericPass *this, ast_scopes_Symbol *sym) {
  switch (sym->type) {
    case ast_scopes_SymbolType_Namespace: {
      passes_generic_pass_GenericPass_import_all_from_namespace(this, sym->u.ns);
    } break;
    case ast_scopes_SymbolType_TypeDef: {
      for (std_map_ValueIterator__4 __iter = std_map_Map__4_iter_values(sym->u.type_def->methods); std_map_ValueIterator__4_has_value(&__iter); std_map_ValueIterator__4_next(&__iter)) {
        ast_nodes_Function *method = std_map_ValueIterator__4_cur(&__iter);
        {
          passes_generic_pass_GenericPass_insert_into_scope_checked(this, method->sym, NULL);
        }
      }
    } break;
    case ast_scopes_SymbolType_Structure: {
      for (std_map_ValueIterator__4 __iter = std_map_Map__4_iter_values(sym->u.struc->type->methods); std_map_ValueIterator__4_has_value(&__iter); std_map_ValueIterator__4_next(&__iter)) {
        ast_nodes_Function *method = std_map_ValueIterator__4_cur(&__iter);
        {
          passes_generic_pass_GenericPass_insert_into_scope_checked(this, method->sym, NULL);
        }
      }
    } break;
    case ast_scopes_SymbolType_Enum: {
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(sym->u.enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
        {
          passes_generic_pass_GenericPass_insert_into_scope_checked(this, field->sym, NULL);
        }
      }
    } break;
    default: {
      passes_generic_pass_GenericPass_error(this, errors_Error_new(sym->span, format_string("Can't imdo wildcard import from a %s", ast_scopes_SymbolType_dbg(sym->type))));
    } break;
  }
}

void passes_mod_run_typecheck_passes(ast_program_Program *program) {
  passes_register_types_RegisterTypes_run(program);
  passes_typechecker_TypeChecker_run(program);
  if (!std_vector_Vector__14_is_empty(program->errors)) 
  ast_program_Program_exit_with_errors(program);
  
}

char *passes_mod_run_codegen_passes(ast_program_Program *program) {
  passes_reorder_structs_ReorderStructs_run(program);
  char *code = passes_code_generator_CodeGenerator_run(program);
  if (!std_vector_Vector__14_is_empty(program->errors)) 
  ast_program_Program_exit_with_errors(program);
  
  return code;
}

void passes_code_generator_CodeGenerator_gen_indent(passes_code_generator_CodeGenerator *this) {
  for (u32 i = ((u32)0); (i < this->indent); i+=((u32)1)) {
    std_buffer_Buffer_puts(&this->out, "  ");
  }
}

void str_replace(char **this, char *other) {
  char *s = *this;
  free(s);
  *this=other;
}

errors_Error *passes_code_generator_CodeGenerator_error(passes_code_generator_CodeGenerator *this, errors_Error *err) {
  return passes_generic_pass_GenericPass_error(this->o, err);
}

ast_scopes_Scope *passes_code_generator_CodeGenerator_scope(passes_code_generator_CodeGenerator *this) {
  return passes_generic_pass_GenericPass_scope(this->o);
}

void passes_code_generator_CodeGenerator_gen_debug_info(passes_code_generator_CodeGenerator *this, std_span_Span span, bool force) {
  if ((!this->o->program->gen_debug_info && !force)) 
  return ;
  
  std_span_Location loc = span.start;
  std_buffer_Buffer_putsf(&this->out, format_string("\n#line %u \"%s\"\n", loc.line, loc.filename));
}

char *passes_code_generator_CodeGenerator_get_op(passes_code_generator_CodeGenerator *this, ast_nodes_ASTType type) {
  return ({ char *__yield_0;
    switch (type) {
      case ast_nodes_ASTType_And: {
        __yield_0 = "&&";
      } break;
      case ast_nodes_ASTType_Assignment: {
        __yield_0 = "=";
      } break;
      case ast_nodes_ASTType_BitwiseAnd: {
        __yield_0 = "&";
      } break;
      case ast_nodes_ASTType_BitwiseOr: {
        __yield_0 = "|";
      } break;
      case ast_nodes_ASTType_BitwiseXor: {
        __yield_0 = "^";
      } break;
      case ast_nodes_ASTType_Divide: {
        __yield_0 = "/";
      } break;
      case ast_nodes_ASTType_Equals: {
        __yield_0 = "==";
      } break;
      case ast_nodes_ASTType_GreaterThan: {
        __yield_0 = ">";
      } break;
      case ast_nodes_ASTType_GreaterThanEquals: {
        __yield_0 = ">=";
      } break;
      case ast_nodes_ASTType_LeftShift: {
        __yield_0 = "<<";
      } break;
      case ast_nodes_ASTType_LessThan: {
        __yield_0 = "<";
      } break;
      case ast_nodes_ASTType_LessThanEquals: {
        __yield_0 = "<=";
      } break;
      case ast_nodes_ASTType_Minus: {
        __yield_0 = "-";
      } break;
      case ast_nodes_ASTType_Modulus: {
        __yield_0 = "%";
      } break;
      case ast_nodes_ASTType_Multiply: {
        __yield_0 = "*";
      } break;
      case ast_nodes_ASTType_NotEquals: {
        __yield_0 = "!=";
      } break;
      case ast_nodes_ASTType_Or: {
        __yield_0 = "||";
      } break;
      case ast_nodes_ASTType_Plus: {
        __yield_0 = "+";
      } break;
      case ast_nodes_ASTType_PlusEquals: {
        __yield_0 = "+=";
      } break;
      case ast_nodes_ASTType_MinusEquals: {
        __yield_0 = "-=";
      } break;
      case ast_nodes_ASTType_MultiplyEquals: {
        __yield_0 = "*=";
      } break;
      case ast_nodes_ASTType_DivideEquals: {
        __yield_0 = "/=";
      } break;
      case ast_nodes_ASTType_RightShift: {
        __yield_0 = ">>";
      } break;
      case ast_nodes_ASTType_PreDecrement: {
        __yield_0 = "--";
      } break;
      case ast_nodes_ASTType_PreIncrement: {
        __yield_0 = "++";
      } break;
      case ast_nodes_ASTType_PostDecrement: {
        __yield_0 = "--";
      } break;
      case ast_nodes_ASTType_PostIncrement: {
        __yield_0 = "++";
      } break;
      default: std_panic(format_string("Unknown op type in get_op: %s", ast_nodes_ASTType_dbg(type))); break;
    }
;__yield_0; });
}

void passes_code_generator_CodeGenerator_gen_internal_print(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  ast_nodes_AST *callee = node->u.call.callee;
  bool newline_after = str_eq(callee->u.ident.name, "println");
  std_buffer_Buffer_puts(&this->out, "printf(");
  std_vector_Vector__16 *args = node->u.call.args;
  ast_nodes_Argument *first = std_vector_Vector__16_at(args, ((u32)0));
  if (first->expr->type==ast_nodes_ASTType_FormatStringLiteral) {
    passes_code_generator_CodeGenerator_gen_format_string_variadic(this, first->expr, newline_after);
    std_buffer_Buffer_puts(&this->out, ")");
    return ;
  } 
  for (u32 i = ((u32)0); (i < args->size); i+=((u32)1)) {
    if ((i > ((u32)0))) 
    std_buffer_Buffer_puts(&this->out, ", ");
    
    ast_nodes_Argument *arg = std_vector_Vector__16_at(args, i);
    passes_code_generator_CodeGenerator_gen_expression(this, arg->expr);
    if ((i==((u32)0) && newline_after)) 
    std_buffer_Buffer_puts(&this->out, "\"\\n\"");
    
  }
  std_buffer_Buffer_puts(&this->out, ")");
}

void passes_code_generator_CodeGenerator_gen_format_string_part(passes_code_generator_CodeGenerator *this, char *part) {
  u32 len = str_len(part);
  for (u32 i = ((u32)0); (i < len); i+=((u32)1)) {
    if (part[i]=='\\') {
      i+=((u32)1);
      switch (part[i]) {
        case '`':
        case '{':
        case '}': {
        } break;
        default: {
          std_buffer_Buffer_putc(&this->out, '\\');
        } break;
      }
    }  else     if (part[i]=='"') {
      std_buffer_Buffer_putc(&this->out, '\\');
    }  else     if (part[i]=='%') {
      std_buffer_Buffer_putc(&this->out, '%');
    } 
    
    
    std_buffer_Buffer_putc(&this->out, part[i]);
  }
}

void passes_code_generator_CodeGenerator_gen_format_string_variadic(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, bool newline_after) {
  std_vector_Vector__7 *parts = node->u.fmt_str.parts;
  std_vector_Vector__12 *exprs = node->u.fmt_str.exprs;
  std_vector_Vector__7 *specs = node->u.fmt_str.specs;
  std_buffer_Buffer_putc(&this->out, '"');
  for (u32 i = ((u32)0); (i < exprs->size); i+=((u32)1)) {
    char *part = std_vector_Vector__7_at(parts, i);
    ast_nodes_AST *expr = std_vector_Vector__12_at(exprs, i);
    passes_code_generator_CodeGenerator_gen_format_string_part(this, part);
    char *spec = std_vector_Vector__7_at(specs, i);
    if (((bool)spec)) {
      std_buffer_Buffer_puts(&this->out, "%");
      std_buffer_Buffer_puts(&this->out, spec);
      continue;
    } 
    types_Type *expr_type = types_Type_unaliased(expr->etype);
    switch (expr_type->base) {
      case types_BaseType_I8:
      case types_BaseType_I16:
      case types_BaseType_I32: {
        std_buffer_Buffer_puts(&this->out, "%d");
      } break;
      case types_BaseType_U8:
      case types_BaseType_U16:
      case types_BaseType_U32: {
        std_buffer_Buffer_puts(&this->out, "%u");
      } break;
      case types_BaseType_I64: {
        std_buffer_Buffer_puts(&this->out, "%lld");
      } break;
      case types_BaseType_U64: {
        std_buffer_Buffer_puts(&this->out, "%llu");
      } break;
      case types_BaseType_Bool: {
        std_buffer_Buffer_puts(&this->out, "%s");
      } break;
      case types_BaseType_F32:
      case types_BaseType_F64: {
        std_buffer_Buffer_puts(&this->out, "%f");
      } break;
      case types_BaseType_Char: {
        std_buffer_Buffer_puts(&this->out, "%c");
      } break;
      case types_BaseType_Pointer: {
        switch (expr_type->u.ptr->base) {
          case types_BaseType_Char: {
            std_buffer_Buffer_puts(&this->out, "%s");
          } break;
          default: {
            std_buffer_Buffer_puts(&this->out, "%p");
          } break;
        }
      } break;
      default: {
        passes_code_generator_CodeGenerator_error(this, errors_Error_new(expr->span, "Invalid type for format string"));
        std_buffer_Buffer_puts(&this->out, "%s");
      } break;
    }
  }
  char *part = std_vector_Vector__7_back(parts);
  passes_code_generator_CodeGenerator_gen_format_string_part(this, part);
  if (newline_after) 
  std_buffer_Buffer_puts(&this->out, "\\n");
  
  std_buffer_Buffer_putc(&this->out, '"');
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(exprs); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *expr = std_vector_Iterator__12_cur(&__iter);
    {
      std_buffer_Buffer_puts(&this->out, ", ");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
    }
  }
}

void passes_code_generator_CodeGenerator_gen_format_string(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  std_buffer_Buffer_puts(&this->out, "format_string(");
  passes_code_generator_CodeGenerator_gen_format_string_variadic(this, node, false);
  std_buffer_Buffer_puts(&this->out, ")");
}

void passes_code_generator_CodeGenerator_gen_yield_expression(passes_code_generator_CodeGenerator *this, ast_nodes_AST *expr) {
  char *yield_var = std_vector_Vector__7_back(this->yield_vars);
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, yield_var);
  std_buffer_Buffer_puts(&this->out, " = ");
  passes_code_generator_CodeGenerator_gen_expression(this, expr);
  std_buffer_Buffer_puts(&this->out, ";\n");
}

void passes_code_generator_CodeGenerator_gen_constant(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  ast_nodes_Variable *const_ = node->u.var_decl.var;
  if (!const_->sym->is_extern) {
    passes_code_generator_CodeGenerator_gen_indent(this);
    std_buffer_Buffer_puts(&this->out, "#define ");
    std_buffer_Buffer_puts(&this->out, const_->sym->out_name);
    std_buffer_Buffer_puts(&this->out, " (");
    passes_code_generator_CodeGenerator_gen_expression(this, node->u.var_decl.init);
    std_buffer_Buffer_puts(&this->out, ")\n");
  } 
}

void passes_code_generator_CodeGenerator_gen_constants(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns) {
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->constants); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *const_ = std_vector_Iterator__12_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_constant(this, const_);
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_constants(this, child);
    }
  }
}

void passes_code_generator_CodeGenerator_gen_global_variables(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns) {
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->variables); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      ast_nodes_Variable *var = node->u.var_decl.var;
      if (!var->sym->is_extern) {
        passes_code_generator_CodeGenerator_gen_var_declaration(this, node);
        std_buffer_Buffer_puts(&this->out, ";\n");
      } 
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_global_variables(this, child);
    }
  }
}

void passes_code_generator_CodeGenerator_gen_control_body(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, ast_nodes_AST *body) {
  if (body->type==ast_nodes_ASTType_Block) {
    passes_code_generator_CodeGenerator_gen_block(this, body, true);
    std_buffer_Buffer_puts(&this->out, " ");
  }  else {
    if ((body->type != ast_nodes_ASTType_If)) {
      std_buffer_Buffer_puts(&this->out, "\n");
    } 
    if ((((bool)node->etype) && (body->type != ast_nodes_ASTType_Yield))) {
      passes_code_generator_CodeGenerator_gen_yield_expression(this, body);
    }  else {
      passes_code_generator_CodeGenerator_gen_statement(this, body);
    } 
    passes_code_generator_CodeGenerator_gen_indent(this);
  } 
}

void passes_code_generator_CodeGenerator_gen_in_yield_context(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  char *yield_var = format_string("__yield_%u", this->yield_vars->size);
  std_vector_Vector__7_push(this->yield_vars, yield_var);
  types_Type *ret_type = node->etype;
  std_buffer_Buffer_puts(&this->out, "({ ");
  passes_code_generator_CodeGenerator_gen_type_and_name(this, ret_type, yield_var);
  std_buffer_Buffer_puts(&this->out, ";\n");
  if (node->type==ast_nodes_ASTType_Block) {
    passes_code_generator_CodeGenerator_gen_block(this, node, false);
  }  else {
    this->indent+=((u32)1);
    passes_code_generator_CodeGenerator_gen_statement(this, node);
    this->indent-=((u32)1);
  } 
  std_buffer_Buffer_puts(&this->out, ";");
  std_buffer_Buffer_puts(&this->out, yield_var);
  std_buffer_Buffer_puts(&this->out, "; })");
  std_vector_Vector__7_pop(this->yield_vars);
}

void passes_code_generator_CodeGenerator_gen_constructor(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, ast_nodes_Structure *struc) {
  std_buffer_Buffer_putsf(&this->out, format_string("(%s){", struc->sym->out_name));
  std_vector_Vector__4 *fields = struc->fields;
  std_vector_Vector__16 *args = node->u.call.args;
  for (u32 i = ((u32)0); (i < args->size); i+=((u32)1)) {
    if ((i != ((u32)0))) 
    std_buffer_Buffer_puts(&this->out, ", ");
    
    ast_nodes_Argument *arg = std_vector_Vector__16_at(args, i);
    ast_nodes_Variable *field = std_vector_Vector__4_at(fields, i);
    std_buffer_Buffer_putsf(&this->out, format_string(".%s=", field->sym->out_name));
    passes_code_generator_CodeGenerator_gen_expression(this, arg->expr);
  }
  std_buffer_Buffer_puts(&this->out, "}");
}

void passes_code_generator_CodeGenerator_gen_expression(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  switch (node->type) {
    case ast_nodes_ASTType_IntLiteral: {
      ast_nodes_NumLiteral *num_lit = &node->u.num_literal;
      if ((node->etype->base != types_BaseType_I32)) {
        std_buffer_Buffer_puts(&this->out, "((");
        passes_code_generator_CodeGenerator_gen_type(this, node->etype);
        std_buffer_Buffer_puts(&this->out, ")");
        std_buffer_Buffer_puts(&this->out, num_lit->text);
        std_buffer_Buffer_puts(&this->out, ")");
      }  else {
        std_buffer_Buffer_puts(&this->out, num_lit->text);
      } 
    } break;
    case ast_nodes_ASTType_FloatLiteral: {
      ast_nodes_NumLiteral *num_lit = &node->u.num_literal;
      std_buffer_Buffer_puts(&this->out, num_lit->text);
      if (node->etype->base==types_BaseType_F32) {
        std_buffer_Buffer_puts(&this->out, "f");
      } 
    } break;
    case ast_nodes_ASTType_FormatStringLiteral: {
      passes_code_generator_CodeGenerator_gen_format_string(this, node);
    } break;
    case ast_nodes_ASTType_StringLiteral: {
      char *str_lit = node->u.string_literal;
      std_buffer_Buffer_puts(&this->out, "\"");
      std_buffer_Buffer_puts(&this->out, str_lit);
      std_buffer_Buffer_puts(&this->out, "\"");
    } break;
    case ast_nodes_ASTType_CharLiteral: {
      char *char_lit = node->u.char_literal;
      std_buffer_Buffer_puts(&this->out, "'");
      std_buffer_Buffer_puts(&this->out, char_lit);
      std_buffer_Buffer_puts(&this->out, "'");
    } break;
    case ast_nodes_ASTType_If: {
      ast_nodes_AST *a = node->u.if_stmt.body;
      ast_nodes_AST *b = node->u.if_stmt.els;
      if (((a->type != ast_nodes_ASTType_Block) && (b->type != ast_nodes_ASTType_Block))) {
        std_buffer_Buffer_puts(&this->out, "(");
        passes_code_generator_CodeGenerator_gen_expression(this, node->u.if_stmt.cond);
        std_buffer_Buffer_puts(&this->out, " ? ");
        passes_code_generator_CodeGenerator_gen_expression(this, a);
        std_buffer_Buffer_puts(&this->out, " : ");
        passes_code_generator_CodeGenerator_gen_expression(this, b);
        std_buffer_Buffer_puts(&this->out, ")");
      }  else {
        passes_code_generator_CodeGenerator_gen_in_yield_context(this, node);
      } 
    } break;
    case ast_nodes_ASTType_Match: {
      passes_code_generator_CodeGenerator_gen_in_yield_context(this, node);
    } break;
    case ast_nodes_ASTType_Block: {
      passes_code_generator_CodeGenerator_gen_in_yield_context(this, node);
    } break;
    case ast_nodes_ASTType_Member: {
      if (((bool)node->resolved_symbol)) {
        ast_scopes_Symbol *sym = node->resolved_symbol;
        switch (sym->type) {
          case ast_scopes_SymbolType_Function: {
            std_buffer_Buffer_puts(&this->out, sym->out_name);
          } break;
          default: {
            passes_code_generator_CodeGenerator_error(this, errors_Error_new(node->span, format_string("Unhandled symbol type in CodeGenerator::gen_expression: %s", ast_scopes_SymbolType_dbg(sym->type))));
          } break;
        }
        return ;
      } 
      ast_nodes_AST *lhs = node->u.member.lhs;
      passes_code_generator_CodeGenerator_gen_expression(this, lhs);
      if (node->u.member.is_pointer) {
        std_buffer_Buffer_puts(&this->out, "->");
      }  else {
        std_buffer_Buffer_puts(&this->out, ".");
      } 
      std_buffer_Buffer_puts(&this->out, node->u.member.rhs_name);
    } break;
    case ast_nodes_ASTType_Identifier:
    case ast_nodes_ASTType_NSLookup:
    case ast_nodes_ASTType_Specialization: {
      ast_scopes_Symbol *sym = node->resolved_symbol;
      if (!((bool)sym)) {
        passes_code_generator_CodeGenerator_error(this, errors_Error_new(node->span, "Symbol not found in CodeGenerator::gen_expression"));
        return ;
      } 
      switch (sym->type) {
        case ast_scopes_SymbolType_Function:
        case ast_scopes_SymbolType_Variable:
        case ast_scopes_SymbolType_Constant: {
          std_buffer_Buffer_puts(&this->out, sym->out_name);
        } break;
        default: std_panic(format_string("Unhandled symbol type: %s", ast_scopes_SymbolType_dbg(sym->type))); break;
      }
    } break;
    case ast_nodes_ASTType_Call: {
      ast_nodes_AST *callee = node->u.call.callee;
      if ((callee->type==ast_nodes_ASTType_Identifier && (str_eq(callee->u.ident.name, "print") || str_eq(callee->u.ident.name, "println")))) {
        passes_code_generator_CodeGenerator_gen_internal_print(this, node);
        return ;
      } 
      ast_scopes_Symbol *sym = ast_nodes_AST_symbol(callee);
      if (((((bool)sym) && sym->type==ast_scopes_SymbolType_Structure) && node->u.call.is_constructor)) {
        passes_code_generator_CodeGenerator_gen_constructor(this, node, sym->u.struc);
        return ;
      } 
      passes_code_generator_CodeGenerator_gen_expression(this, callee);
      std_buffer_Buffer_puts(&this->out, "(");
      std_vector_Vector__16 *args = node->u.call.args;
      for (u32 i = ((u32)0); (i < args->size); i+=((u32)1)) {
        if ((i != ((u32)0))) 
        std_buffer_Buffer_puts(&this->out, ", ");
        
        ast_nodes_Argument *arg = std_vector_Vector__16_at(args, i);
        passes_code_generator_CodeGenerator_gen_expression(this, arg->expr);
      }
      std_buffer_Buffer_puts(&this->out, ")");
    } break;
    case ast_nodes_ASTType_BoolLiteral: {
      bool bool_lit = node->u.bool_literal;
      std_buffer_Buffer_puts(&this->out, (bool_lit ? "true" : "false"));
    } break;
    case ast_nodes_ASTType_Address: {
      ast_nodes_AST *expr = node->u.unary;
      std_buffer_Buffer_puts(&this->out, "&");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
    } break;
    case ast_nodes_ASTType_Dereference: {
      ast_nodes_AST *expr = node->u.unary;
      std_buffer_Buffer_puts(&this->out, "*");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
    } break;
    case ast_nodes_ASTType_Negate: {
      ast_nodes_AST *expr = node->u.unary;
      std_buffer_Buffer_puts(&this->out, "-");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
    } break;
    case ast_nodes_ASTType_BitwiseNot: {
      ast_nodes_AST *expr = node->u.unary;
      std_buffer_Buffer_puts(&this->out, "~");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
    } break;
    case ast_nodes_ASTType_Not: {
      ast_nodes_AST *expr = node->u.unary;
      std_buffer_Buffer_puts(&this->out, "!");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
    } break;
    case ast_nodes_ASTType_IsNotNull: {
      ast_nodes_AST *expr = node->u.unary;
      std_buffer_Buffer_puts(&this->out, "((bool)");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
      std_buffer_Buffer_puts(&this->out, ")");
    } break;
    case ast_nodes_ASTType_Cast: {
      ast_nodes_AST *expr = node->u.unary;
      types_Type *type = node->etype;
      std_buffer_Buffer_puts(&this->out, "((");
      std_buffer_Buffer_puts(&this->out, passes_code_generator_CodeGenerator_get_type_name_string(this, type, "", false));
      std_buffer_Buffer_puts(&this->out, ")");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
      std_buffer_Buffer_puts(&this->out, ")");
    } break;
    case ast_nodes_ASTType_SizeOf: {
      std_buffer_Buffer_puts(&this->out, "((u32)sizeof(");
      passes_code_generator_CodeGenerator_gen_type(this, node->u.size_of_type);
      std_buffer_Buffer_puts(&this->out, "))");
    } break;
    case ast_nodes_ASTType_Assert: {
      ast_nodes_AST *expr = node->u.assertion.expr;
      std_buffer_Buffer_puts(&this->out, "ae_assert(");
      passes_code_generator_CodeGenerator_gen_expression(this, expr);
      std_buffer_Buffer_puts(&this->out, ", ");
      {
        std_buffer_Buffer_puts(&this->out, "\"");
        std_buffer_Buffer_putsf(&this->out, std_span_Location_str(&expr->span.start));
        char *expr_str = ast_program_Program_get_source_text(this->o->program, expr->span);
        std_buffer_Buffer_putsf(&this->out, format_string(": Assertion failed: `%s`", expr_str));
        std_buffer_Buffer_puts(&this->out, "\"");
      }
      std_buffer_Buffer_puts(&this->out, ", ");
      if (((bool)node->u.assertion.msg)) {
        passes_code_generator_CodeGenerator_gen_expression(this, node->u.assertion.msg);
      }  else {
        std_buffer_Buffer_puts(&this->out, "NULL");
      } 
      std_buffer_Buffer_puts(&this->out, ")");
    } break;
    case ast_nodes_ASTType_Null: {
      std_buffer_Buffer_puts(&this->out, "NULL");
    } break;
    case ast_nodes_ASTType_And:
    case ast_nodes_ASTType_BitwiseAnd:
    case ast_nodes_ASTType_BitwiseOr:
    case ast_nodes_ASTType_BitwiseXor:
    case ast_nodes_ASTType_Divide:
    case ast_nodes_ASTType_GreaterThan:
    case ast_nodes_ASTType_GreaterThanEquals:
    case ast_nodes_ASTType_LeftShift:
    case ast_nodes_ASTType_LessThan:
    case ast_nodes_ASTType_LessThanEquals:
    case ast_nodes_ASTType_Minus:
    case ast_nodes_ASTType_Modulus:
    case ast_nodes_ASTType_Multiply:
    case ast_nodes_ASTType_NotEquals:
    case ast_nodes_ASTType_Or:
    case ast_nodes_ASTType_Plus:
    case ast_nodes_ASTType_RightShift: {
      ast_nodes_AST *lhs = node->u.binary.lhs;
      ast_nodes_AST *rhs = node->u.binary.rhs;
      std_buffer_Buffer_puts(&this->out, "(");
      passes_code_generator_CodeGenerator_gen_expression(this, lhs);
      std_buffer_Buffer_puts(&this->out, " ");
      std_buffer_Buffer_puts(&this->out, passes_code_generator_CodeGenerator_get_op(this, node->type));
      std_buffer_Buffer_puts(&this->out, " ");
      passes_code_generator_CodeGenerator_gen_expression(this, rhs);
      std_buffer_Buffer_puts(&this->out, ")");
    } break;
    case ast_nodes_ASTType_Index: {
      ast_nodes_AST *lhs = node->u.binary.lhs;
      ast_nodes_AST *rhs = node->u.binary.rhs;
      passes_code_generator_CodeGenerator_gen_expression(this, lhs);
      std_buffer_Buffer_puts(&this->out, "[");
      passes_code_generator_CodeGenerator_gen_expression(this, rhs);
      std_buffer_Buffer_puts(&this->out, "]");
    } break;
    case ast_nodes_ASTType_Equals:
    case ast_nodes_ASTType_Assignment:
    case ast_nodes_ASTType_PlusEquals:
    case ast_nodes_ASTType_MinusEquals:
    case ast_nodes_ASTType_DivideEquals:
    case ast_nodes_ASTType_MultiplyEquals: {
      passes_code_generator_CodeGenerator_gen_expression(this, node->u.binary.lhs);
      std_buffer_Buffer_puts(&this->out, passes_code_generator_CodeGenerator_get_op(this, node->type));
      passes_code_generator_CodeGenerator_gen_expression(this, node->u.binary.rhs);
    } break;
    case ast_nodes_ASTType_PreIncrement:
    case ast_nodes_ASTType_PreDecrement: {
      std_buffer_Buffer_puts(&this->out, passes_code_generator_CodeGenerator_get_op(this, node->type));
      passes_code_generator_CodeGenerator_gen_expression(this, node->u.unary);
    } break;
    case ast_nodes_ASTType_PostIncrement:
    case ast_nodes_ASTType_PostDecrement: {
      passes_code_generator_CodeGenerator_gen_expression(this, node->u.unary);
      std_buffer_Buffer_puts(&this->out, passes_code_generator_CodeGenerator_get_op(this, node->type));
    } break;
    default: {
      passes_code_generator_CodeGenerator_error(this, errors_Error_new(node->span, format_string("Unhandled expression type in CodeGenerator: %s", ast_nodes_ASTType_dbg(node->type))));
    } break;
  }
}

void passes_code_generator_CodeGenerator_gen_var_declaration(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  ast_nodes_Variable *var = node->u.var_decl.var;
  passes_code_generator_CodeGenerator_gen_type_and_name(this, var->type, var->sym->out_name);
  if (((bool)node->u.var_decl.init)) {
    std_buffer_Buffer_puts(&this->out, " = ");
    passes_code_generator_CodeGenerator_gen_expression(this, node->u.var_decl.init);
  } 
}

void passes_code_generator_CodeGenerator_gen_match_case_body(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, ast_nodes_AST *body) {
  if (body->type==ast_nodes_ASTType_Block) {
    std_buffer_Buffer_puts(&this->out, " ");
    passes_code_generator_CodeGenerator_gen_block(this, body, true);
  }  else   if ((body->type==ast_nodes_ASTType_Call && body->returns)) {
    std_buffer_Buffer_puts(&this->out, " ");
    passes_code_generator_CodeGenerator_gen_expression(this, body);
    std_buffer_Buffer_puts(&this->out, ";");
  }  else   if ((((bool)node->etype) && (body->type != ast_nodes_ASTType_Yield))) {
    std_buffer_Buffer_puts(&this->out, " {\n");
    this->indent+=((u32)1);
    passes_code_generator_CodeGenerator_gen_yield_expression(this, body);
    this->indent-=((u32)1);
    passes_code_generator_CodeGenerator_gen_indent(this);
    std_buffer_Buffer_puts(&this->out, "}");
  }  else {
    std_buffer_Buffer_puts(&this->out, " {\n");
    this->indent+=((u32)1);
    passes_code_generator_CodeGenerator_gen_statement(this, body);
    this->indent-=((u32)1);
    passes_code_generator_CodeGenerator_gen_indent(this);
    std_buffer_Buffer_puts(&this->out, "}");
  } 
  
  
}

void passes_code_generator_CodeGenerator_gen_match_string(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  ast_nodes_Match stmt = node->u.match_stmt;
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, "{\n");
  this->indent+=((u32)1);
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, "char *__match_str = ");
  passes_code_generator_CodeGenerator_gen_expression(this, stmt.expr);
  std_buffer_Buffer_puts(&this->out, ";\n");
  std_vector_Vector__18 *cases = stmt.cases;
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, "if (");
  for (u32 i = ((u32)0); (i < cases->size); i+=((u32)1)) {
    ast_nodes_MatchCase *_case = std_vector_Vector__18_at(cases, i);
    std_buffer_Buffer_puts(&this->out, "!strcmp(__match_str, ");
    passes_code_generator_CodeGenerator_gen_expression(this, _case->cond);
    std_buffer_Buffer_puts(&this->out, ")");
    if (((bool)_case->body)) {
      std_buffer_Buffer_puts(&this->out, ")");
      passes_code_generator_CodeGenerator_gen_match_case_body(this, node, _case->body);
      std_buffer_Buffer_puts(&this->out, " else ");
      if ((i != (cases->size - ((u32)1)))) {
        std_buffer_Buffer_puts(&this->out, "if (");
      } 
    }  else {
      std_buffer_Buffer_puts(&this->out, " || ");
    } 
  }
  if (((bool)stmt.defolt)) {
    passes_code_generator_CodeGenerator_gen_match_case_body(this, node, stmt.defolt);
  } 
  std_buffer_Buffer_puts(&this->out, "\n");
  this->indent-=((u32)1);
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, "}\n");
}

void passes_code_generator_CodeGenerator_gen_match(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  ast_nodes_Match stmt = node->u.match_stmt;
  if (types_Type_is_str(stmt.expr->etype)) {
    passes_code_generator_CodeGenerator_gen_match_string(this, node);
    return ;
  } 
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, "switch (");
  passes_code_generator_CodeGenerator_gen_expression(this, stmt.expr);
  std_buffer_Buffer_puts(&this->out, ") {\n");
  std_vector_Vector__18 *cases = stmt.cases;
  this->indent+=((u32)1);
  for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(cases); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
    ast_nodes_MatchCase *_case = std_vector_Iterator__18_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_puts(&this->out, "case ");
      passes_code_generator_CodeGenerator_gen_expression(this, _case->cond);
      std_buffer_Buffer_puts(&this->out, ":");
      if (((bool)_case->body)) {
        passes_code_generator_CodeGenerator_gen_match_case_body(this, node, _case->body);
        std_buffer_Buffer_puts(&this->out, " break;\n");
      }  else {
        std_buffer_Buffer_puts(&this->out, "\n");
      } 
    }
  }
  if (((bool)stmt.defolt)) {
    passes_code_generator_CodeGenerator_gen_indent(this);
    std_buffer_Buffer_puts(&this->out, "default:");
    passes_code_generator_CodeGenerator_gen_match_case_body(this, node, stmt.defolt);
    std_buffer_Buffer_puts(&this->out, " break;\n");
  } 
  this->indent-=((u32)1);
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, "}\n");
}

void passes_code_generator_CodeGenerator_gen_defers_upto(passes_code_generator_CodeGenerator *this, ast_scopes_Scope *end_scope) {
  bool first = true;
  for (ast_scopes_Scope *cur = passes_code_generator_CodeGenerator_scope(this); ((bool)cur); cur=cur->parent) {
    for (u32 i = ((u32)0); (i < cur->defers->size); i+=((u32)1)) {
      if (first) {
        first=false;
        passes_code_generator_CodeGenerator_gen_indent(this);
        std_buffer_Buffer_puts(&this->out, "/* defers */\n");
      } 
      u32 idx = ((cur->defers->size - i) - ((u32)1));
      ast_nodes_AST *stmt = std_vector_Vector__12_at(cur->defers, idx);
      passes_code_generator_CodeGenerator_gen_statement(this, stmt);
    }
    if (cur==end_scope) 
    break;
    
  }
}

void passes_code_generator_CodeGenerator_gen_statement(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node) {
  passes_code_generator_CodeGenerator_gen_debug_info(this, node->span, false);
  switch (node->type) {
    case ast_nodes_ASTType_Return: {
      ast_scopes_Scope *upto = passes_code_generator_CodeGenerator_scope(this);
      for (ast_scopes_Scope *cur = passes_code_generator_CodeGenerator_scope(this); (((bool)cur) && ((bool)cur->cur_func)); cur=cur->parent) {
        upto=cur;
      }
      passes_code_generator_CodeGenerator_gen_defers_upto(this, upto);
      passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_puts(&this->out, "return ");
      if (((bool)node->u.unary)) {
        passes_code_generator_CodeGenerator_gen_expression(this, node->u.unary);
      } 
      std_buffer_Buffer_puts(&this->out, ";\n");
    } break;
    case ast_nodes_ASTType_Yield: {
      passes_code_generator_CodeGenerator_gen_yield_expression(this, node->u.unary);
    } break;
    case ast_nodes_ASTType_Import: {
    } break;
    case ast_nodes_ASTType_Break:
    case ast_nodes_ASTType_Continue: {
      u32 loop_count = passes_code_generator_CodeGenerator_scope(this)->loop_count;
      ast_scopes_Scope *upto = passes_code_generator_CodeGenerator_scope(this);
      for (ast_scopes_Scope *cur = passes_code_generator_CodeGenerator_scope(this); (((bool)cur) && cur->loop_count==loop_count); cur=cur->parent) {
        upto=cur;
      }
      passes_code_generator_CodeGenerator_gen_defers_upto(this, upto);
      passes_code_generator_CodeGenerator_gen_indent(this);
      if (node->type==ast_nodes_ASTType_Break) {
        std_buffer_Buffer_puts(&this->out, "break;\n");
      }  else {
        std_buffer_Buffer_puts(&this->out, "continue;\n");
      } 
    } break;
    case ast_nodes_ASTType_VarDeclaration: {
      passes_code_generator_CodeGenerator_gen_indent(this);
      passes_code_generator_CodeGenerator_gen_var_declaration(this, node);
      std_buffer_Buffer_puts(&this->out, ";\n");
    } break;
    case ast_nodes_ASTType_Block: {
      passes_code_generator_CodeGenerator_gen_indent(this);
      passes_code_generator_CodeGenerator_gen_block(this, node, true);
      std_buffer_Buffer_puts(&this->out, "\n");
    } break;
    case ast_nodes_ASTType_Defer: {
      std_vector_Vector__12_push(passes_code_generator_CodeGenerator_scope(this)->defers, node->u.unary);
    } break;
    case ast_nodes_ASTType_If: {
      ast_nodes_AST *cond = node->u.if_stmt.cond;
      ast_nodes_AST *body = node->u.if_stmt.body;
      ast_nodes_AST *else_body = node->u.if_stmt.els;
      passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_puts(&this->out, "if (");
      passes_code_generator_CodeGenerator_gen_expression(this, cond);
      std_buffer_Buffer_puts(&this->out, ") ");
      passes_code_generator_CodeGenerator_gen_control_body(this, node, body);
      if (((bool)else_body)) {
        std_buffer_Buffer_puts(&this->out, " else ");
        passes_code_generator_CodeGenerator_gen_control_body(this, node, else_body);
        std_buffer_Buffer_puts(&this->out, "\n");
      }  else {
        std_buffer_Buffer_puts(&this->out, "\n");
      } 
    } break;
    case ast_nodes_ASTType_Match: {
      passes_code_generator_CodeGenerator_gen_match(this, node);
    } break;
    case ast_nodes_ASTType_While: {
      ast_nodes_AST *cond = node->u.loop.cond;
      ast_nodes_AST *body = node->u.loop.body;
      passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_puts(&this->out, "while (");
      passes_code_generator_CodeGenerator_gen_expression(this, cond);
      std_buffer_Buffer_puts(&this->out, ") ");
      passes_code_generator_CodeGenerator_gen_block(this, body, true);
      std_buffer_Buffer_puts(&this->out, "\n");
    } break;
    case ast_nodes_ASTType_For: {
      ast_nodes_AST *init = node->u.loop.init;
      ast_nodes_AST *cond = node->u.loop.cond;
      ast_nodes_AST *step = node->u.loop.step;
      ast_nodes_AST *body = node->u.loop.body;
      passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_puts(&this->out, "for (");
      if (((bool)init)) {
        if (init->type==ast_nodes_ASTType_VarDeclaration) {
          passes_code_generator_CodeGenerator_gen_var_declaration(this, init);
        }  else {
          passes_code_generator_CodeGenerator_gen_expression(this, init);
        } 
      } 
      std_buffer_Buffer_puts(&this->out, "; ");
      if (((bool)cond)) 
      passes_code_generator_CodeGenerator_gen_expression(this, cond);
      
      std_buffer_Buffer_puts(&this->out, "; ");
      if (((bool)step)) 
      passes_code_generator_CodeGenerator_gen_expression(this, step);
      
      std_buffer_Buffer_puts(&this->out, ") ");
      passes_code_generator_CodeGenerator_gen_block(this, body, true);
      std_buffer_Buffer_puts(&this->out, "\n");
    } break;
    default: {
      passes_code_generator_CodeGenerator_gen_indent(this);
      passes_code_generator_CodeGenerator_gen_expression(this, node);
      std_buffer_Buffer_puts(&this->out, ";\n");
    } break;
  }
}

void passes_code_generator_CodeGenerator_gen_block(passes_code_generator_CodeGenerator *this, ast_nodes_AST *node, bool with_braces) {
  if (with_braces) 
  std_buffer_Buffer_puts(&this->out, "{\n");
  
  ast_scopes_Scope *scope = node->u.block.scope;
  passes_generic_pass_GenericPass_push_scope(this->o, node->u.block.scope);
  std_vector_Vector__12 *statements = node->u.block.statements;
  this->indent+=((u32)1);
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(statements); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *statement = std_vector_Iterator__12_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_statement(this, statement);
    }
  }
  if (!node->returns) {
    passes_code_generator_CodeGenerator_gen_defers_upto(this, scope);
  } 
  this->indent-=((u32)1);
  passes_code_generator_CodeGenerator_gen_indent(this);
  if (with_braces) 
  std_buffer_Buffer_puts(&this->out, "}");
  
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

char *passes_code_generator_CodeGenerator_helper_gen_type(passes_code_generator_CodeGenerator *this, types_Type *top, types_Type *cur, char *acc, bool is_func_def) {
  switch (cur->base) {
    case types_BaseType_Void:
    case types_BaseType_Bool:
    case types_BaseType_Char:
    case types_BaseType_I8:
    case types_BaseType_I16:
    case types_BaseType_I32:
    case types_BaseType_I64:
    case types_BaseType_U8:
    case types_BaseType_U16:
    case types_BaseType_U32:
    case types_BaseType_U64:
    case types_BaseType_F32:
    case types_BaseType_F64: {
      str_replace(&acc, format_string("%s %s", types_BaseType_str(cur->base), acc));
    } break;
    case types_BaseType_Structure: {
      str_replace(&acc, format_string("%s %s", cur->u.struc->sym->out_name, acc));
    } break;
    case types_BaseType_Enum: {
      str_replace(&acc, format_string("%s %s", cur->u.enum_->sym->out_name, acc));
    } break;
    case types_BaseType_Alias: {
      acc=passes_code_generator_CodeGenerator_helper_gen_type(this, top, cur->u.ptr, acc, false);
    } break;
    case types_BaseType_Function: {
      std_buffer_Buffer args_str = std_buffer_Buffer_make(((u32)16));
      std_vector_Vector__4 *params = cur->u.func->params;
      if (params->size==((u32)0)) 
      std_buffer_Buffer_puts(&args_str, "void");
      
      for (u32 i = ((u32)0); (i < params->size); i+=((u32)1)) {
        if ((i != ((u32)0))) 
        std_buffer_Buffer_puts(&args_str, ", ");
        
        ast_nodes_Variable *var = std_vector_Vector__4_at(params, i);
        char *arg_str = passes_code_generator_CodeGenerator_get_type_name_string(this, var->type, var->sym->out_name, false);
        std_buffer_Buffer_putsf(&args_str, arg_str);
      }
      if ((is_func_def && cur==top)) {
        str_replace(&acc, format_string("%s(%s)", acc, std_buffer_Buffer_str(&args_str)));
      }  else {
        str_replace(&acc, format_string("(*%s)(%s)", acc, std_buffer_Buffer_str(&args_str)));
      } 
      free(args_str.data);
      acc=passes_code_generator_CodeGenerator_helper_gen_type(this, top, cur->u.func->return_type, acc, false);
    } break;
    case types_BaseType_Pointer: {
      bool needs_parens = (((bool)cur->u.ptr) && (cur->u.ptr->base==types_BaseType_Function || cur->u.ptr->base==types_BaseType_Array));
      if (needs_parens) {
        str_replace(&acc, format_string("(*%s)", acc));
      }  else {
        str_replace(&acc, format_string("*%s", acc));
      } 
      acc=passes_code_generator_CodeGenerator_helper_gen_type(this, top, cur->u.ptr, acc, false);
    } break;
    case types_BaseType_Array: {
      std_buffer_Buffer prev_buffer = this->out;
      this->out=std_buffer_Buffer_make(((u32)16));
      passes_code_generator_CodeGenerator_gen_expression(this, cur->u.arr->size_expr);
      str_replace(&acc, format_string("%s[%s]", acc, std_buffer_Buffer_str(&this->out)));
      free(this->out.data);
      this->out=prev_buffer;
      acc=passes_code_generator_CodeGenerator_helper_gen_type(this, top, cur->u.arr->elem_type, acc, false);
    } break;
    default: {
      passes_code_generator_CodeGenerator_error(this, errors_Error_new(cur->span, format_string("Unhandled type found in CodeGenerator::helper_gen_type: %s: %s", types_BaseType_dbg(cur->base), types_Type_str(cur))));
    } break;
  }
  return acc;
}

char *passes_code_generator_CodeGenerator_get_type_name_string(passes_code_generator_CodeGenerator *this, types_Type *type, char *name, bool is_func_def) {
  ae_assert((type != NULL), "compiler/passes/code_generator.oc:876:12: Assertion failed: `type != null`", NULL);
  char *final = passes_code_generator_CodeGenerator_helper_gen_type(this, type, type, strdup(name), is_func_def);
  str_strip_trailing_whitespace(final);
  return final;
}

void passes_code_generator_CodeGenerator_gen_type_and_name(passes_code_generator_CodeGenerator *this, types_Type *type, char *name) {
  std_buffer_Buffer_putsf(&this->out, passes_code_generator_CodeGenerator_get_type_name_string(this, type, name, false));
}

void passes_code_generator_CodeGenerator_gen_type(passes_code_generator_CodeGenerator *this, types_Type *type) {
  passes_code_generator_CodeGenerator_gen_type_and_name(this, type, "");
}

void passes_code_generator_CodeGenerator_gen_function(passes_code_generator_CodeGenerator *this, ast_nodes_Function *func) {
  if ((func->is_method && func->parent_type->base==types_BaseType_Structure)) {
    ast_nodes_Structure *struc = func->parent_type->u.struc;
    if (struc->is_templated) 
    return ;
    
  } 
  passes_code_generator_CodeGenerator_gen_debug_info(this, func->sym->span, false);
  passes_code_generator_CodeGenerator_gen_function_decl(this, func);
  std_buffer_Buffer_puts(&this->out, " ");
  passes_code_generator_CodeGenerator_gen_block(this, func->body, true);
  std_buffer_Buffer_puts(&this->out, "\n\n");
}

void passes_code_generator_CodeGenerator_gen_function_decl(passes_code_generator_CodeGenerator *this, ast_nodes_Function *func) {
  char *func_name = func->sym->out_name;
  char *s = passes_code_generator_CodeGenerator_get_type_name_string(this, func->type, func_name, true);
  std_buffer_Buffer_putsf(&this->out, s);
}

void passes_code_generator_CodeGenerator_gen_functions(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns) {
  std_vector_Vector__10 *functions = ns->functions;
  passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
    {
      if (!func->sym->is_extern) 
      passes_code_generator_CodeGenerator_gen_function(this, func);
      
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_functions(this, child);
    }
  }
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

void passes_code_generator_CodeGenerator_gen_function_decls(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns) {
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(ns->functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
    {
      if (func->sym->is_extern) 
      continue;
      
      if ((func->is_method && func->parent_type->base==types_BaseType_Structure)) {
        ast_nodes_Structure *struc = func->parent_type->u.struc;
        if (struc->is_templated) 
        continue;
        
      } 
      passes_code_generator_CodeGenerator_gen_function_decl(this, func);
      if (func->exits) 
      std_buffer_Buffer_puts(&this->out, " __attribute__((noreturn))");
      
      std_buffer_Buffer_puts(&this->out, ";\n");
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_function_decls(this, child);
    }
  }
}

void passes_code_generator_CodeGenerator_gen_enum_types(passes_code_generator_CodeGenerator *this, ast_program_Namespace *ns) {
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->enums); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    ast_nodes_Enum *enum_ = std_vector_Iterator__13_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_enum(this, enum_);
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_enum_types(this, child);
    }
  }
}

void passes_code_generator_CodeGenerator_gen_enum_dbg_method(passes_code_generator_CodeGenerator *this, ast_nodes_Enum *enum_) {
  ast_nodes_Function *dbg = std_map_Map__4_at(enum_->type->methods, "dbg");
  passes_code_generator_CodeGenerator_gen_function_decl(this, dbg);
  std_buffer_Buffer_puts(&this->out, " {\n");
  this->indent+=((u32)1);
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, "switch (this) {\n");
  this->indent+=((u32)1);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      passes_code_generator_CodeGenerator_gen_indent(this);
      std_buffer_Buffer_puts(&this->out, "case ");
      std_buffer_Buffer_puts(&this->out, field->sym->out_name);
      std_buffer_Buffer_putsf(&this->out, format_string(": return \"%s\";\n", field->sym->name));
    }
  }
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_putsf(&this->out, format_string("default: return \"<unknown>\";\n"));
  this->indent-=((u32)1);
  passes_code_generator_CodeGenerator_gen_indent(this);
  std_buffer_Buffer_puts(&this->out, "}\n");
  this->indent-=((u32)1);
  std_buffer_Buffer_puts(&this->out, "}\n\n");
}

void passes_code_generator_CodeGenerator_gen_enum(passes_code_generator_CodeGenerator *this, ast_nodes_Enum *enum_) {
  char *enum_name = enum_->sym->out_name;
  if (!enum_->sym->is_extern) {
    std_buffer_Buffer_putsf(&this->out, format_string("typedef enum %s {\n", enum_name));
    for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
      ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
      {
        std_buffer_Buffer_puts(&this->out, "  ");
        std_buffer_Buffer_puts(&this->out, field->sym->out_name);
        std_buffer_Buffer_puts(&this->out, ",\n");
      }
    }
    std_buffer_Buffer_putsf(&this->out, format_string("} %s;\n\n", enum_name));
  } 
  passes_code_generator_CodeGenerator_gen_enum_dbg_method(this, enum_);
}

void passes_code_generator_CodeGenerator_gen_struct(passes_code_generator_CodeGenerator *this, ast_nodes_Structure *struc) {
  char *struc_name = struc->sym->out_name;
  if (struc->is_union) {
    std_buffer_Buffer_putsf(&this->out, format_string("union %s {\n", struc_name));
  }  else {
    std_buffer_Buffer_putsf(&this->out, format_string("struct %s {\n", struc_name));
  } 
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      std_buffer_Buffer_puts(&this->out, "  ");
      passes_code_generator_CodeGenerator_gen_type_and_name(this, field->type, field->sym->out_name);
      std_buffer_Buffer_puts(&this->out, ";\n");
    }
  }
  std_buffer_Buffer_puts(&this->out, "};\n\n");
}

void passes_code_generator_CodeGenerator_gen_structs_and_typedefs(passes_code_generator_CodeGenerator *this) {
  std_vector_Vector__11 *structs = this->o->program->ordered_structs;
  if (structs->size==((u32)0)) 
  return ;
  
  std_buffer_Buffer_puts(&this->out, "/* Struct typedefs */\n");
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      if (struc->sym->is_extern) 
      continue;
      
      char *struc_name = struc->sym->out_name;
      if (struc->is_union) {
        std_buffer_Buffer_putsf(&this->out, format_string("typedef union %s %s;\n", struc_name, struc_name));
      }  else {
        std_buffer_Buffer_putsf(&this->out, format_string("typedef struct %s %s;\n", struc_name, struc_name));
      } 
    }
  }
  std_buffer_Buffer_puts(&this->out, "\n/* Struct definitions */\n");
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      if (struc->sym->is_extern) 
      continue;
      
      passes_code_generator_CodeGenerator_gen_struct(this, struc);
    }
  }
  std_buffer_Buffer_puts(&this->out, "\n");
}

char *passes_code_generator_CodeGenerator_generate(passes_code_generator_CodeGenerator *this) {
  for (std_vector_Iterator__7 __iter = std_vector_Vector__7_iter(this->o->program->c_includes); std_vector_Iterator__7_has_value(&__iter); std_vector_Iterator__7_next(&__iter)) {
    char *include = std_vector_Iterator__7_cur(&__iter);
    {
      std_buffer_Buffer_putsf(&this->out, format_string("#include \"%s\"\n", include));
    }
  }
  std_buffer_Buffer_puts(&this->out, "\n");
  for (std_map_Iterator__3 __iter = std_map_Map__3_iter(this->o->program->c_embeds); std_map_Iterator__3_has_value(&__iter); std_map_Iterator__3_next(&__iter)) {
    std_map_Node__3 *it = std_map_Iterator__3_cur(&__iter);
    {
      std_buffer_Buffer_putsf(&this->out, format_string("/* Embed: %s */\n", it->key));
      std_buffer_Buffer_puts(&this->out, it->value);
      std_buffer_Buffer_puts(&this->out, "\n\n");
    }
  }
  std_buffer_Buffer_puts(&this->out, "/* Enums */\n");
  passes_code_generator_CodeGenerator_gen_enum_types(this, this->o->program->global);
  std_buffer_Buffer_puts(&this->out, "/* Constants */\n");
  passes_code_generator_CodeGenerator_gen_constants(this, this->o->program->global);
  passes_code_generator_CodeGenerator_gen_structs_and_typedefs(this);
  passes_code_generator_CodeGenerator_gen_global_variables(this, this->o->program->global);
  std_buffer_Buffer_puts(&this->out, "/* function declarations */\n");
  passes_code_generator_CodeGenerator_gen_function_decls(this, this->o->program->global);
  std_buffer_Buffer_puts(&this->out, "/* function implementations */\n");
  passes_code_generator_CodeGenerator_gen_functions(this, this->o->program->global);
  return std_buffer_Buffer_str(&this->out);
}

passes_code_generator_CodeGenerator passes_code_generator_CodeGenerator_make(ast_program_Program *program) {
  return (passes_code_generator_CodeGenerator){.o=passes_generic_pass_GenericPass_new(program), .out=std_buffer_Buffer_make(((u32)16)), .indent=((u32)0), .yield_vars=std_vector_Vector__7_new(((u32)16))};
}

char *passes_code_generator_CodeGenerator_run(ast_program_Program *program) {
  passes_code_generator_CodeGenerator pass = passes_code_generator_CodeGenerator_make(program);
  return passes_code_generator_CodeGenerator_generate(&pass);
}

errors_Error *passes_typechecker_TypeChecker_error(passes_typechecker_TypeChecker *this, errors_Error *err) {
  return passes_generic_pass_GenericPass_error(this->o, err);
}

ast_scopes_Scope *passes_typechecker_TypeChecker_scope(passes_typechecker_TypeChecker *this) {
  return passes_generic_pass_GenericPass_scope(this->o);
}

types_Type *passes_typechecker_TypeChecker_get_base_type(passes_typechecker_TypeChecker *this, types_BaseType base, std_span_Span span) {
  return ast_program_Program_get_base_type(this->o->program, base, span);
}

types_Type *passes_typechecker_TypeChecker_get_type_by_name(passes_typechecker_TypeChecker *this, char *name, std_span_Span span) {
  return ast_program_Program_get_type_by_name(this->o->program, name, span);
}

types_Type *passes_typechecker_TypeChecker_resolve_type(passes_typechecker_TypeChecker *this, types_Type *old, bool allow_incomplete, bool error, bool resolve_templates) {
  bool p_a = allow_incomplete;
  bool p_e = error;
  bool p_r = resolve_templates;
  types_Type *resolved = old;
  switch (old->base) {
    case types_BaseType_Pointer:
    case types_BaseType_Alias: {
      types_Type *ptr = passes_typechecker_TypeChecker_resolve_type(this, old->u.ptr, p_a, p_e, p_r);
      if (!((bool)ptr)) 
      return NULL;
      
      resolved->u.ptr=ptr;
    } break;
    case types_BaseType_Function: {
      types_FunctionType *func = old->u.func;
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(func->params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        ast_nodes_Variable *var = std_vector_Iterator__4_cur(&__iter);
        {
          var->type=passes_typechecker_TypeChecker_resolve_type(this, var->type, p_a, p_e, p_r);
          if (!((bool)var->type)) 
          return NULL;
          
        }
      }
      func->return_type=passes_typechecker_TypeChecker_resolve_type(this, func->return_type, p_a, p_e, p_r);
      if (!((bool)func->return_type)) 
      return NULL;
      
    } break;
    case types_BaseType_Unresolved: {
      ast_nodes_AST *node = old->u.unresolved;
      ast_scopes_Symbol *res = passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node, p_e, NULL, p_r);
      if (((bool)res)) {
        switch (res->type) {
          case ast_scopes_SymbolType_TypeDef: {
            resolved=res->u.type_def;
          } break;
          case ast_scopes_SymbolType_Structure: {
            ast_nodes_Structure *struc = res->u.struc;
            if ((struc->is_templated && !allow_incomplete)) {
              if (error) {
                passes_typechecker_TypeChecker_error(this, errors_Error_new(old->span, format_string("Cannot use templated struct %s as a type", struc->sym->name)));
                return NULL;
              } 
              return resolved;
            } 
            resolved=res->u.struc->type;
            if ((node->type==ast_nodes_ASTType_Specialization && struc->is_templated)) {
              ae_assert(!resolve_templates, "compiler/passes/typechecker.oc:78:36: Assertion failed: `not resolve_templates`", "Should have been errored in resolve_scoped_identifier");
              types_Type *type = types_Type_new_resolved(types_BaseType_UnresolvedTemplate, node->span);
              type->u.unresolved_spec=types_UnresolvedTemplate_new(resolved, node->u.spec.template_args);
              resolved=type;
            } 
          } break;
          case ast_scopes_SymbolType_Enum: {
            resolved=res->u.enum_->type;
          } break;
          default: {
            if (error) {
              passes_typechecker_TypeChecker_error(this, errors_Error_new(res->span, format_string("Cannot use %s as a type", res->display)));
              resolved=NULL;
            } 
          } break;
        }
      }  else {
        if (error) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(old->u.unresolved->span, "Could not find type with this name"));
          resolved=NULL;
        } 
      } 
    } break;
    case types_BaseType_Array: {
      types_ArrayType *arr = old->u.arr;
      arr->elem_type=passes_typechecker_TypeChecker_resolve_type(this, arr->elem_type, p_a, p_e, p_r);
      if (!((bool)arr->elem_type)) 
      return NULL;
      
      types_Type *res = passes_typechecker_TypeChecker_check_expression(this, arr->size_expr, NULL);
      if (!((bool)res)) 
      return NULL;
      
    } break;
    case types_BaseType_Structure:
    case types_BaseType_Char:
    case types_BaseType_Bool:
    case types_BaseType_Void:
    case types_BaseType_I8:
    case types_BaseType_I16:
    case types_BaseType_I32:
    case types_BaseType_I64:
    case types_BaseType_U8:
    case types_BaseType_U16:
    case types_BaseType_U32:
    case types_BaseType_U64:
    case types_BaseType_F32:
    case types_BaseType_F64:
    case types_BaseType_Enum: {
    } break;
    default: {
      if (error) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(old->span, format_string("Unhandled type in resolve %s", types_BaseType_str(old->base))));
        resolved=NULL;
      } 
    } break;
  }
  return resolved;
}

void passes_typechecker_TypeChecker_resolve_templated_struct_methods(passes_typechecker_TypeChecker *this, ast_nodes_Structure *old, ast_nodes_Structure *cur) {
  types_Type *old_type = old->type;
  types_Type *cur_type = cur->type;
  std_map_Map__4 *old_methods = old_type->methods;
  std_map_Map__4 *cur_methods = cur_type->methods;
  ast_program_Namespace *parent_ns = old->sym->ns;
  for (std_map_Iterator__4 __iter = std_map_Map__4_iter(old_methods); std_map_Iterator__4_has_value(&__iter); std_map_Iterator__4_next(&__iter)) {
    std_map_Node__4 *iter = std_map_Iterator__4_cur(&__iter);
    {
      char *name = iter->key;
      ast_nodes_Function *method = iter->value;
      ast_nodes_Function *new_method = ast_program_Program_get_function_deep_copy(this->o->program, method, parent_ns);
      new_method->parent_type=cur_type;
      std_map_Map__4_insert(cur_methods, name, new_method);
      ast_scopes_Symbol_update_parent(new_method->sym, cur_type->sym);
      std_vector_Vector__10_push(parent_ns->functions, new_method);
      if (!method->is_static) {
        ast_nodes_Variable *this_param = std_vector_Vector__4_at(new_method->params, ((u32)0));
        if (this_param->type->base==types_BaseType_Pointer) {
          this_param->type->u.ptr=cur_type;
        }  else {
          this_param->type=cur_type;
        } 
      } 
      new_method->scope=passes_typechecker_TypeChecker_scope(this);
      passes_typechecker_TypeChecker_check_function_declaration(this, new_method);
    }
  }
  for (std_map_ValueIterator__4 __iter = std_map_Map__4_iter_values(cur_methods); std_map_ValueIterator__4_has_value(&__iter); std_map_ValueIterator__4_next(&__iter)) {
    ast_nodes_Function *method = std_map_ValueIterator__4_cur(&__iter);
    {
      std_vector_Vector__10_push(this->unchecked_functions, method);
    }
  }
}

ast_scopes_Symbol *passes_typechecker_TypeChecker_resolve_templated_struct(passes_typechecker_TypeChecker *this, ast_nodes_Structure *struc, ast_nodes_AST *node) {
  {
    ast_scopes_Symbol *found = ast_nodes_Structure_find_template_instance(struc, node->u.spec.template_args);
    if (((bool)found)) 
    return found;
    
  }
  ast_nodes_Structure *resolved_struc = ast_program_Program_get_struct_deep_copy(this->o->program, struc, struc->sym->ns);
  resolved_struc->is_templated=false;
  resolved_struc->template_params=NULL;
  resolved_struc->template_instances=NULL;
  ast_program_Namespace *parent_ns = struc->sym->ns;
  ast_scopes_Scope *scope = ast_scopes_Scope_new(parent_ns->scope);
  passes_generic_pass_GenericPass_push_scope(this->o, scope);
  std_vector_Vector__4 *template_params = struc->template_params;
  std_vector_Vector__3 *template_args = node->u.spec.template_args;
  if ((template_params->size != template_args->size)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Invalid number of template arguments for %s", struc->sym->name)));
    return NULL;
  } 
  std_buffer_Buffer new_display_name = std_buffer_Buffer_make(((u32)16));
  std_buffer_Buffer_puts(&new_display_name, struc->sym->name);
  std_buffer_Buffer_puts(&new_display_name, "<");
  for (u32 i = ((u32)0); (i < template_params->size); i+=((u32)1)) {
    ast_nodes_Variable *param = std_vector_Vector__4_at(template_params, i);
    types_Type *arg = std_vector_Vector__3_at(template_args, i);
    if ((i > ((u32)0))) 
    std_buffer_Buffer_puts(&new_display_name, ", ");
    
    std_buffer_Buffer_puts(&new_display_name, types_Type_str(arg));
    ast_scopes_Symbol *sym = ast_scopes_Symbol_new(ast_scopes_SymbolType_TypeDef, NULL, param->sym->name, param->sym->name, param->sym->name, param->sym->span);
    sym->u.type_def=arg;
    passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, sym, NULL);
  }
  std_buffer_Buffer_puts(&new_display_name, ">");
  char *new_out_name = format_string("%s__%u", struc->sym->name, struc->template_instances->size);
  ast_scopes_Symbol *sym = ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType_Structure, parent_ns, parent_ns->sym, new_out_name, struc->sym->span);
  sym->display=std_buffer_Buffer_str(&new_display_name);
  sym->u.struc=resolved_struc;
  resolved_struc->sym=sym;
  ast_nodes_TemplateInstance *instance = ast_nodes_TemplateInstance_new(template_args, struc->sym, sym);
  std_vector_Vector__15_push(struc->template_instances, instance);
  types_Type *typ = types_Type_new_resolved(types_BaseType_Structure, sym->span);
  typ->u.struc=resolved_struc;
  resolved_struc->type=typ;
  typ->sym=sym;
  typ->template_instance=instance;
  passes_typechecker_TypeChecker_resolve_struct(this, resolved_struc);
  passes_typechecker_TypeChecker_resolve_templated_struct_methods(this, struc, resolved_struc);
  passes_generic_pass_GenericPass_pop_scope(this->o);
  return sym;
}

ast_scopes_Symbol *passes_typechecker_TypeChecker_resolve_scoped_identifier(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool error, types_Type *hint, bool resolve_templates) {
  switch (node->type) {
    case ast_nodes_ASTType_NSLookup: {
      std_span_Span lhs_span = node->u.lookup.lhs->span;
      ast_scopes_Symbol *lhs = passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node->u.lookup.lhs, error, NULL, resolve_templates);
      if (!((bool)lhs)) 
      return NULL;
      
      char *name = node->u.lookup.rhs_name;
      ast_scopes_Symbol *res = passes_generic_pass_GenericPass_lookup_in_symbol(this->o, lhs, name, node->span, error);
      node->resolved_symbol=res;
      return res;
    } break;
    case ast_nodes_ASTType_Identifier: {
      char *name = node->u.ident.name;
      ast_scopes_Symbol *res = ((ast_scopes_Symbol *)NULL);
      if ((((bool)hint) && hint->base==types_BaseType_Enum)) {
        ast_nodes_Enum *enum_ = hint->u.enum_;
        ast_nodes_Variable *field = ((ast_nodes_Variable *)ast_nodes_Enum_get_field(enum_, name));
        if (((bool)field)) {
          res=field->sym;
        } 
      } 
      if (!((bool)res)) {
        res=ast_scopes_Scope_lookup_recursive(passes_typechecker_TypeChecker_scope(this), name);
      } 
      node->resolved_symbol=res;
      if ((error && !((bool)res))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Couldn't find this identifier"));
      } 
      return res;
    } break;
    case ast_nodes_ASTType_Specialization: {
      ast_scopes_Symbol *base = passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node->u.spec.base, error, hint, resolve_templates);
      if (!((bool)base)) 
      return NULL;
      
      std_vector_Vector__3 *args = node->u.spec.template_args;
      bool failed = false;
      for (u32 i = ((u32)0); (i < args->size); i+=((u32)1)) {
        types_Type *resolved_arg = passes_typechecker_TypeChecker_resolve_type(this, std_vector_Vector__3_at(args, i), true, error, resolve_templates);
        if (!((bool)resolved_arg)) {
          failed=true;
          continue;
        } 
        args->data[i]=resolved_arg;
      }
      if (failed) 
      return NULL;
      
      base=ast_scopes_Symbol_remove_alias(base);
      if (!resolve_templates) {
        node->resolved_symbol=base;
        return base;
      } 
      switch (base->type) {
        case ast_scopes_SymbolType_Structure: {
          ast_nodes_Structure *struc = base->u.struc;
          if (!struc->is_templated) {
            if (error) {
              passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Can only specialize a templated structure, got %s", base->display)));
            } 
            return NULL;
          } 
          ast_scopes_Symbol *res = passes_typechecker_TypeChecker_resolve_templated_struct(this, struc, node);
          node->resolved_symbol=res;
          return res;
        } break;
        default: {
          if (error) {
            passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Can only specialize a structure, got %s", base->display)));
          } 
          return NULL;
        } break;
      }
    } break;
    default: {
      if (error) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Don't know how to resolve node type %s", ast_nodes_ASTType_dbg(node->type))));
      } 
      return NULL;
    } break;
  }
}

void passes_typechecker_TypeChecker_check_block(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_expr, types_Type *hint) {
  ast_nodes_Block *block = &node->u.block;
  ast_scopes_Scope *scope = ast_scopes_Scope_new(passes_typechecker_TypeChecker_scope(this));
  scope->can_yield=(scope->can_yield || is_expr);
  block->scope=scope;
  passes_generic_pass_GenericPass_push_scope(this->o, block->scope);
  std_vector_Vector__12 *stmts = block->statements;
  std_span_Span yield_span = std_span_Span_default();
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(stmts); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *stmt = std_vector_Iterator__12_cur(&__iter);
    {
      passes_typechecker_TypeChecker_check_statement(this, stmt);
      if (stmt->returns) 
      node->returns=true;
      
      if ((stmt->type != ast_nodes_ASTType_Yield)) 
      continue;
      
      if (((bool)node->etype)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_hint(stmt->span, "Cannot yield multiple times in a block", yield_span, "Previously yielded here"));
      } 
      node->etype=stmt->etype;
      yield_span=stmt->span;
    }
  }
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

void passes_typechecker_TypeChecker_check_method_call(passes_typechecker_TypeChecker *this, ast_nodes_Function *method, ast_nodes_AST *node) {
  ast_nodes_AST *callee = node->u.call.callee;
  if (((callee->type != ast_nodes_ASTType_Member) && (callee->type != ast_nodes_ASTType_NSLookup))) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(callee->span, "Method call is not to a member, internal compiler error"));
    return ;
  } 
  if (node->u.call.added_method_arg) 
  return ;
  
  node->u.call.added_method_arg=true;
  if ((callee->type != ast_nodes_ASTType_Member)) 
  return ;
  
  if (method->params->size==((u32)0)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(callee->span, "Instance method should have `this` argument, internal error"));
    return ;
  } 
  types_Type *method_param = std_vector_Vector__4_at(method->params, ((u32)0))->type;
  if (!((bool)method_param)) 
  return ;
  
  ast_nodes_Member member = callee->u.member;
  ast_nodes_AST *first_arg = member.lhs;
  if ((member.is_pointer && (method_param->base != types_BaseType_Pointer))) {
    first_arg=ast_nodes_AST_new_unop(ast_nodes_ASTType_Dereference, first_arg->span, first_arg);
  }  else   if ((!member.is_pointer && method_param->base==types_BaseType_Pointer)) {
    first_arg=ast_nodes_AST_new_unop(ast_nodes_ASTType_Address, first_arg->span, first_arg);
  } 
  
  std_vector_Vector__16_push_front(node->u.call.args, ast_nodes_Argument_new(NULL, std_span_Span_default(), first_arg));
}

types_Type *passes_typechecker_TypeChecker_check_internal_print(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  std_vector_Vector__16 *args = node->u.call.args;
  if ((args->size < ((u32)1))) {
    passes_generic_pass_GenericPass_error(this->o, errors_Error_new(node->span, "Function requires at least one argument"));
  } 
  ast_nodes_Argument *first = std_vector_Vector__16_at(args, ((u32)0));
  types_Type *first_type = passes_typechecker_TypeChecker_check_expression(this, first->expr, NULL);
  if ((((bool)first_type) && !types_Type_is_str(first_type))) {
    passes_generic_pass_GenericPass_error(this->o, errors_Error_new(first->expr->span, format_string("First argument must be a string literal, got %s", types_Type_str(first_type))));
  } 
  for (std_vector_Iterator__16 __iter = std_vector_Vector__16_iter(args); std_vector_Iterator__16_has_value(&__iter); std_vector_Iterator__16_next(&__iter)) {
    ast_nodes_Argument *arg = std_vector_Iterator__16_cur(&__iter);
    {
      passes_typechecker_TypeChecker_check_expression(this, arg->expr, NULL);
      passes_typechecker_TypeChecker_call_dbg_on_enum_value(this, &arg->expr);
    }
  }
  return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Void, node->span);
}

types_Type *passes_typechecker_TypeChecker_check_constructor(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  node->u.call.is_constructor=true;
  ast_nodes_AST *callee = node->u.call.callee;
  ast_nodes_Structure *struc = callee->resolved_symbol->u.struc;
  std_vector_Vector__4 *params = struc->fields;
  passes_typechecker_TypeChecker_check_call_args(this, node, params);
  return struc->type;
}

void passes_typechecker_TypeChecker_check_call_args(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, std_vector_Vector__4 *params) {
  std_vector_Vector__16 *args = node->u.call.args;
  if ((params->size < args->size)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Too many arguments, expected %u but got %u", params->size, args->size)));
  } 
  for (u32 i = ((u32)0); (i < params->size); i+=((u32)1)) {
    ast_nodes_Variable *param = std_vector_Vector__4_at(params, i);
    if ((i < args->size)) {
      ast_nodes_Argument *arg = std_vector_Vector__16_at(args, i);
      if ((((bool)arg->label) && !str_eq(arg->label, param->sym->name))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(arg->label_span, format_string("Argument label '%s' does not match parameter name '%s'", arg->label, param->sym->name)));
      } 
      types_Type *arg_type = passes_typechecker_TypeChecker_check_expression(this, arg->expr, param->type);
      if ((!((bool)arg_type) || !((bool)param->type))) 
      continue;
      
      if (!types_Type_eq(arg_type, param->type)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(arg->expr->span, format_string("Argument %s has type %s but expected %s", param->sym->name, types_Type_str(arg_type), types_Type_str(param->type))));
      } 
    }  else     if (((bool)param->default_value)) {
      ast_nodes_Argument *new_arg = ast_nodes_Argument_new(NULL, std_span_Span_default(), param->default_value);
      std_vector_Vector__16_push(args, new_arg);
    }  else {
      passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Missing required argument %s", param->sym->name)));
    } 
    
  }
}

types_Type *passes_typechecker_TypeChecker_check_call(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  ast_nodes_AST *callee = node->u.call.callee;
  std_vector_Vector__16 *args = node->u.call.args;
  if (callee->type==ast_nodes_ASTType_Identifier) {
    callee->u.ident.is_function=false;
    char *name = callee->u.ident.name;
    if ((str_eq(name, "print") || str_eq(name, "println"))) {
      return passes_typechecker_TypeChecker_check_internal_print(this, node);
    } 
  } 
  types_Type *res = ({ types_Type *__yield_0;
    switch (callee->type) {
      case ast_nodes_ASTType_Member: {
        __yield_0 = passes_typechecker_TypeChecker_check_member(this, callee, true);
      } break;
      case ast_nodes_ASTType_Identifier:
      case ast_nodes_ASTType_NSLookup:
      case ast_nodes_ASTType_Specialization: {
        ast_scopes_Symbol *sym = passes_typechecker_TypeChecker_resolve_scoped_identifier(this, callee, true, NULL, true);
        if (!((bool)sym)) 
        return NULL;
        
        callee->resolved_symbol=sym;
        __yield_0 = ({ types_Type *__yield_1;
          switch (sym->type) {
            case ast_scopes_SymbolType_Structure: {
              return passes_typechecker_TypeChecker_check_constructor(this, node);
            } break;
            default: {
              __yield_1 = passes_typechecker_TypeChecker_check_expression(this, callee, NULL);
            } break;
          }
;__yield_1; });
      } break;
      default: {
        __yield_0 = passes_typechecker_TypeChecker_check_expression(this, callee, NULL);
      } break;
    }
;__yield_0; });
  if (!((bool)res)) 
  return NULL;
  
  if ((res->base != types_BaseType_Function)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(callee->span, format_string("Cannot call a non-function type: %s", types_Type_str(res))));
    return NULL;
  } 
  types_FunctionType *func = res->u.func;
  if ((((bool)func->orig) && func->orig->exits)) 
  node->returns=true;
  
  std_vector_Vector__4 *params = func->params;
  if (((((bool)func->orig) && func->orig->is_method) && !func->orig->is_static)) {
    passes_typechecker_TypeChecker_check_method_call(this, func->orig, node);
  } 
  passes_typechecker_TypeChecker_check_call_args(this, node, params);
  if (((bool)func->orig)) {
    node->u.call.is_function_pointer=false;
    node->u.call.func=func->orig;
  } 
  return func->return_type;
}

types_Type *passes_typechecker_TypeChecker_check_pointer_arith(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *_lhs, types_Type *_rhs) {
  if ((node->type==ast_nodes_ASTType_Plus || node->type==ast_nodes_ASTType_Minus)) {
    if ((_lhs->base==types_BaseType_Pointer && types_Type_is_integer(_rhs))) {
      return _lhs;
    } 
    if ((types_Type_is_integer(_lhs) && _rhs->base==types_BaseType_Pointer)) {
      return _rhs;
    } 
    if ((types_Type_eq(_lhs, _rhs) && _lhs->base==types_BaseType_Pointer)) {
      if (node->type==ast_nodes_ASTType_Minus) {
        return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_I64, node->span);
      } 
    } 
  } 
  passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Invalid pointer arithmetic"));
  return NULL;
}

types_Type *passes_typechecker_TypeChecker_check_binary_op(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *lhs, types_Type *rhs) {
  switch (node->type) {
    case ast_nodes_ASTType_Plus:
    case ast_nodes_ASTType_Minus:
    case ast_nodes_ASTType_Multiply:
    case ast_nodes_ASTType_Divide: {
      if ((lhs->base==types_BaseType_Pointer || rhs->base==types_BaseType_Pointer)) {
        return passes_typechecker_TypeChecker_check_pointer_arith(this, node, lhs, rhs);
      }  else       if ((!types_Type_is_numeric(lhs) || !types_Type_is_numeric(rhs))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operator requires numeric types", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
        return NULL;
      }  else       if (!types_Type_eq(lhs, rhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
        return NULL;
      }  else {
        return lhs;
      } 
      
      
    } break;
    case ast_nodes_ASTType_PlusEquals:
    case ast_nodes_ASTType_MinusEquals:
    case ast_nodes_ASTType_DivideEquals:
    case ast_nodes_ASTType_MultiplyEquals: {
      if ((!types_Type_is_numeric(lhs) || !types_Type_is_numeric(rhs))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operator requires numeric types", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
        return NULL;
      } 
      ast_nodes_AST *lhs_node = node->u.binary.lhs;
      if (!ast_nodes_AST_is_lvalue(lhs_node)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Left hand side of assignment must be assignable"));
        return NULL;
      } 
      if (!types_Type_eq(lhs, rhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
        return NULL;
      } 
      return lhs;
    } break;
    case ast_nodes_ASTType_LessThan:
    case ast_nodes_ASTType_LessThanEquals:
    case ast_nodes_ASTType_GreaterThan:
    case ast_nodes_ASTType_GreaterThanEquals: {
      if ((!types_Type_is_numeric_or_char(lhs) || !types_Type_is_numeric_or_char(rhs))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operator requires numeric or char types", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
      } 
      if (!types_Type_eq(lhs, rhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
      } 
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span);
    } break;
    case ast_nodes_ASTType_Equals:
    case ast_nodes_ASTType_NotEquals: {
      if (!types_Type_eq(lhs, rhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
      } 
      if (lhs->base==types_BaseType_Structure) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot compare structs directly"));
      } 
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span);
    } break;
    case ast_nodes_ASTType_And:
    case ast_nodes_ASTType_Or: {
      if ((!types_Type_eq(lhs, rhs) || (lhs->base != types_BaseType_Bool))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operands must be boolean", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
      } 
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span);
    } break;
    case ast_nodes_ASTType_Modulus:
    case ast_nodes_ASTType_BitwiseOr:
    case ast_nodes_ASTType_BitwiseAnd:
    case ast_nodes_ASTType_BitwiseXor:
    case ast_nodes_ASTType_LeftShift:
    case ast_nodes_ASTType_RightShift: {
      if ((!types_Type_is_integer(lhs) || !types_Type_is_integer(rhs))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operator requires integer types", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
      } 
      if (!types_Type_eq(lhs, rhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Operands must be of the same type", format_string("Got types '%s' and '%s'", types_Type_str(lhs), types_Type_str(rhs))));
      } 
      return lhs;
    } break;
    default: std_panic(format_string("Internal error: unhandled op in check_binary_op: %s", ast_nodes_ASTType_dbg(node->type))); break;
  }
}

types_Type *passes_typechecker_TypeChecker_check_format_string(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  std_vector_Vector__7 *parts = node->u.fmt_str.parts;
  std_vector_Vector__12 *exprs = node->u.fmt_str.exprs;
  if ((parts->size != (exprs->size + ((u32)1)))) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Number of format string parts does not match number of expressions"));
  } 
  for (u32 i = ((u32)0); (i < exprs->size); i+=((u32)1)) {
    ast_nodes_AST *expr = std_vector_Vector__12_at(exprs, i);
    types_Type *typ = passes_typechecker_TypeChecker_check_expression(this, expr, NULL);
    if (!((bool)typ)) 
    continue;
    
    typ=passes_typechecker_TypeChecker_call_dbg_on_enum_value(this, &expr);
    exprs->data[i]=expr;
    if (!((bool)typ)) 
    continue;
    
    switch (types_Type_unaliased(typ)->base) {
      case types_BaseType_Bool:
      case types_BaseType_Char:
      case types_BaseType_I8:
      case types_BaseType_I16:
      case types_BaseType_I32:
      case types_BaseType_I64:
      case types_BaseType_U8:
      case types_BaseType_U16:
      case types_BaseType_U32:
      case types_BaseType_U64:
      case types_BaseType_F32:
      case types_BaseType_F64:
      case types_BaseType_Pointer: {
      } break;
      default: {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(expr->span, format_string("Only strings / builtin types can be formatted, got type '%s'", types_Type_str(typ))));
      } break;
    }
  }
  return passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span);
}

types_Type *passes_typechecker_TypeChecker_check_member(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_being_called) {
  types_Type *lhs = passes_typechecker_TypeChecker_check_expression(this, node->u.member.lhs, NULL);
  if (!((bool)lhs)) 
  return NULL;
  
  bool is_pointer = false;
  if (lhs->base==types_BaseType_Pointer) {
    is_pointer=true;
    if (!((bool)lhs->u.ptr)) {
      passes_typechecker_TypeChecker_error(this, errors_Error_new(lhs->span, "Got unresolved type"));
      return NULL;
    } 
    lhs=lhs->u.ptr;
  } 
  if (lhs->base==types_BaseType_Structure) {
    ast_nodes_Structure *struc = lhs->u.struc;
    ast_nodes_Variable *field = ast_nodes_Structure_get_field(struc, node->u.member.rhs_name);
    if (((bool)field)) {
      node->u.member.is_pointer=is_pointer;
      return field->type;
    } 
  } 
  if (types_Type_can_have_methods(lhs)) {
    ast_nodes_Function *method = std_map_Map__4_get(lhs->methods, node->u.member.rhs_name, NULL);
    if (((bool)method)) {
      if (!is_being_called) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot access method without calling it"));
        return NULL;
      } 
      if (method->is_static) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot call static method as instance method"));
        return NULL;
      } 
      node->u.member.is_pointer=is_pointer;
      node->resolved_symbol=method->sym;
      return method->type;
    } 
  } 
  passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Type %s has no member named '%s'", types_Type_str(lhs), node->u.member.rhs_name)));
  return NULL;
}

types_Type *passes_typechecker_TypeChecker_check_expression(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *hint) {
  types_Type *typ = passes_typechecker_TypeChecker_check_expression_helper(this, node, hint);
  if (((bool)typ)) 
  typ=types_Type_decay_array(typ);
  
  node->etype=typ;
  return typ;
}

types_Type *passes_typechecker_TypeChecker_check_expression_helper(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *hint) {
  switch (node->type) {
    case ast_nodes_ASTType_IntLiteral: {
      if (((bool)node->u.num_literal.suffix)) {
        return passes_typechecker_TypeChecker_resolve_type(this, node->u.num_literal.suffix, false, true, true);
      } 
      if ((((bool)hint) && types_Type_is_integer(hint))) 
      return hint;
      
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_U32, node->span);
    } break;
    case ast_nodes_ASTType_FloatLiteral: {
      if (((bool)node->u.num_literal.suffix)) {
        return passes_typechecker_TypeChecker_resolve_type(this, node->u.num_literal.suffix, false, true, true);
      } 
      if ((((bool)hint) && types_Type_is_float(hint))) 
      return hint;
      
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_F32, node->span);
    } break;
    case ast_nodes_ASTType_StringLiteral: {
      return passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span);
    } break;
    case ast_nodes_ASTType_Null: {
      return passes_typechecker_TypeChecker_get_type_by_name(this, "untyped_ptr", node->span);
    } break;
    case ast_nodes_ASTType_Cast: {
      types_Type *typ = passes_typechecker_TypeChecker_check_expression(this, node->u.cast.lhs, NULL);
      if (!((bool)typ)) 
      return NULL;
      
      types_Type *target = passes_typechecker_TypeChecker_resolve_type(this, node->u.cast.to, false, true, true);
      if (!((bool)target)) 
      return NULL;
      
      return target;
    } break;
    case ast_nodes_ASTType_FormatStringLiteral: {
      return passes_typechecker_TypeChecker_check_format_string(this, node);
    } break;
    case ast_nodes_ASTType_CharLiteral: {
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Char, node->span);
    } break;
    case ast_nodes_ASTType_BoolLiteral: {
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span);
    } break;
    case ast_nodes_ASTType_Negate: {
      if (!((bool)hint)) {
        hint=passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_I32, node->span);
      } 
      types_Type *typ = passes_typechecker_TypeChecker_check_expression(this, node->u.unary, hint);
      if (!((bool)typ)) 
      return NULL;
      
      if (!types_Type_is_numeric(typ)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Cannot negate non-numeric type: %s", types_Type_str(typ))));
        return NULL;
      } 
      return typ;
    } break;
    case ast_nodes_ASTType_BitwiseNot: {
      types_Type *typ = passes_typechecker_TypeChecker_check_expression(this, node->u.unary, hint);
      if (!((bool)typ)) 
      return NULL;
      
      if (!types_Type_is_integer(typ)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Cannot do bitwise-not on non-integer type: %s", types_Type_str(typ))));
        return NULL;
      } 
      return typ;
    } break;
    case ast_nodes_ASTType_IsNotNull: {
      types_Type *typ = passes_typechecker_TypeChecker_check_expression(this, node->u.unary, NULL);
      if (!((bool)typ)) 
      return NULL;
      
      typ=types_Type_unaliased(typ);
      if ((typ->base != types_BaseType_Pointer)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Can only use ? on pointer types, got %s", types_Type_str(typ))));
      } 
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span);
    } break;
    case ast_nodes_ASTType_Not: {
      types_Type *typ = passes_typechecker_TypeChecker_check_expression(this, node->u.unary, passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span));
      if (!((bool)typ)) 
      return NULL;
      
      if ((typ->base != types_BaseType_Bool)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Cannot negate non-boolean type: %s", types_Type_str(typ))));
        return NULL;
      } 
      return typ;
    } break;
    case ast_nodes_ASTType_Address: {
      types_Type *typ = passes_typechecker_TypeChecker_check_expression(this, node->u.unary, NULL);
      if (!((bool)typ)) 
      return NULL;
      
      switch (typ->base) {
        case types_BaseType_Char: {
          return passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span);
        } break;
        case types_BaseType_Void: {
          return passes_typechecker_TypeChecker_get_type_by_name(this, "untyped_ptr", node->span);
        } break;
        default: {
          types_Type *ptr = types_Type_new_resolved(types_BaseType_Pointer, node->span);
          ptr->u.ptr=typ;
          return ptr;
        } break;
      }
    } break;
    case ast_nodes_ASTType_Dereference: {
      types_Type *typ = passes_typechecker_TypeChecker_check_expression(this, node->u.unary, NULL);
      if (!((bool)typ)) 
      return NULL;
      
      if ((typ->base != types_BaseType_Pointer)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Cannot dereference non-pointer type: %s", types_Type_str(typ))));
        return NULL;
      } 
      return typ->u.ptr;
    } break;
    case ast_nodes_ASTType_Member: {
      return passes_typechecker_TypeChecker_check_member(this, node, false);
    } break;
    case ast_nodes_ASTType_SizeOf: {
      types_Type *typ = passes_typechecker_TypeChecker_resolve_type(this, node->u.size_of_type, false, true, true);
      if (!((bool)typ)) 
      return NULL;
      
      node->u.size_of_type=typ;
      return passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_U32, node->span);
    } break;
    case ast_nodes_ASTType_If: {
      passes_typechecker_TypeChecker_check_if(this, node, true, hint);
      return node->etype;
    } break;
    case ast_nodes_ASTType_Block: {
      passes_typechecker_TypeChecker_check_block(this, node, true, hint);
      return node->etype;
    } break;
    case ast_nodes_ASTType_Match: {
      passes_typechecker_TypeChecker_check_match(this, node, true, hint);
      return node->etype;
    } break;
    case ast_nodes_ASTType_Index: {
      types_Type *lhs = passes_typechecker_TypeChecker_check_expression(this, node->u.binary.lhs, NULL);
      types_Type *rhs = passes_typechecker_TypeChecker_check_expression(this, node->u.binary.rhs, NULL);
      if ((!((bool)lhs) || !((bool)rhs))) 
      return NULL;
      
      if (!types_Type_is_integer(rhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Index must be an integer, got %s", types_Type_str(rhs))));
        return NULL;
      } 
      lhs=types_Type_unaliased(lhs);
      switch (lhs->base) {
        case types_BaseType_Array: {
          return lhs->u.arr->elem_type;
        } break;
        case types_BaseType_Pointer: {
          return lhs->u.ptr;
        } break;
        default: {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Cannot index type %s", types_Type_str(lhs))));
          return NULL;
        } break;
      }
    } break;
    case ast_nodes_ASTType_Identifier:
    case ast_nodes_ASTType_NSLookup:
    case ast_nodes_ASTType_Specialization: {
      ast_scopes_Symbol *item = passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node, true, hint, true);
      if (!((bool)item)) 
      return NULL;
      
      switch (item->type) {
        case ast_scopes_SymbolType_Function: {
          return item->u.func->type;
        } break;
        case ast_scopes_SymbolType_Variable:
        case ast_scopes_SymbolType_Constant: {
          return item->u.var->type;
        } break;
        case ast_scopes_SymbolType_Structure: {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot use a struct name as an expression"));
          return NULL;
        } break;
        case ast_scopes_SymbolType_Enum: {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot use an enum name as an expression"));
          return NULL;
        } break;
        case ast_scopes_SymbolType_Namespace: {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot use a namespace as an expression"));
          return NULL;
        } break;
        case ast_scopes_SymbolType_TypeDef: {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot use a type name as an expression"));
          return NULL;
        } break;
      }
    } break;
    case ast_nodes_ASTType_Call: {
      return passes_typechecker_TypeChecker_check_call(this, node);
    } break;
    case ast_nodes_ASTType_Plus:
    case ast_nodes_ASTType_Minus:
    case ast_nodes_ASTType_Multiply:
    case ast_nodes_ASTType_Divide:
    case ast_nodes_ASTType_LessThan:
    case ast_nodes_ASTType_LessThanEquals:
    case ast_nodes_ASTType_GreaterThan:
    case ast_nodes_ASTType_GreaterThanEquals:
    case ast_nodes_ASTType_Equals:
    case ast_nodes_ASTType_NotEquals:
    case ast_nodes_ASTType_PlusEquals:
    case ast_nodes_ASTType_MinusEquals:
    case ast_nodes_ASTType_MultiplyEquals:
    case ast_nodes_ASTType_DivideEquals:
    case ast_nodes_ASTType_And:
    case ast_nodes_ASTType_Or:
    case ast_nodes_ASTType_Modulus:
    case ast_nodes_ASTType_BitwiseOr:
    case ast_nodes_ASTType_BitwiseAnd:
    case ast_nodes_ASTType_BitwiseXor:
    case ast_nodes_ASTType_LeftShift:
    case ast_nodes_ASTType_RightShift: {
      types_Type *lhs = passes_typechecker_TypeChecker_check_expression(this, node->u.binary.lhs, NULL);
      types_Type *rhs = passes_typechecker_TypeChecker_check_expression(this, node->u.binary.rhs, lhs);
      if ((!((bool)lhs) || !((bool)rhs))) 
      return NULL;
      
      return passes_typechecker_TypeChecker_check_binary_op(this, node, types_Type_unaliased(lhs), types_Type_unaliased(rhs));
    } break;
    case ast_nodes_ASTType_Assignment: {
      types_Type *lhs = passes_typechecker_TypeChecker_check_expression(this, node->u.binary.lhs, NULL);
      types_Type *rhs = passes_typechecker_TypeChecker_check_expression(this, node->u.binary.rhs, lhs);
      if ((!((bool)lhs) || !((bool)rhs))) 
      return NULL;
      
      if (!ast_nodes_AST_is_lvalue(node->u.binary.lhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->u.binary.lhs->span, "Must be an l-value"));
      } 
      if (!types_Type_eq(lhs, rhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Variable type does not match assignment type, Expected type '%s', got '%s'", types_Type_str(lhs), types_Type_str(rhs))));
      } 
      return lhs;
    } break;
    case ast_nodes_ASTType_PreIncrement:
    case ast_nodes_ASTType_PostIncrement:
    case ast_nodes_ASTType_PreDecrement:
    case ast_nodes_ASTType_PostDecrement: {
      types_Type *lhs = passes_typechecker_TypeChecker_check_expression(this, node->u.unary, NULL);
      if (!((bool)lhs)) 
      return NULL;
      
      if (!types_Type_is_integer(lhs)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Cannot increment or decrement non-integer type: %s", types_Type_str(lhs))));
        return NULL;
      } 
      if (!ast_nodes_AST_is_lvalue(node->u.unary)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Can't perform %s on a non-lvalue", ast_nodes_ASTType_dbg(node->type))));
      } 
      return lhs;
    } break;
    default: {
      passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Invalid expression in TypeChecker::check_expression"));
      return this->o->error_type;
    } break;
  }
}

types_Type *passes_typechecker_TypeChecker_call_dbg_on_enum_value(passes_typechecker_TypeChecker *this, ast_nodes_AST **node_ptr) {
  ast_nodes_AST *node = *node_ptr;
  if (!((bool)node->etype)) 
  return NULL;
  
  if ((node->etype->base != types_BaseType_Enum)) 
  return node->etype;
  
  ast_nodes_AST *member = ast_nodes_AST_new(ast_nodes_ASTType_Member, node->span);
  member->u.member.lhs=node;
  member->u.member.rhs_name="dbg";
  member->u.member.rhs_span=node->span;
  ast_nodes_AST *call = ast_nodes_AST_new(ast_nodes_ASTType_Call, node->span);
  call->u.call.callee=member;
  call->u.call.args=std_vector_Vector__16_new(((u32)16));
  *node_ptr=call;
  return passes_typechecker_TypeChecker_check_expression(this, call, NULL);
}

void passes_typechecker_TypeChecker_check_match_for_enum(passes_typechecker_TypeChecker *this, ast_nodes_Enum *enum_, ast_nodes_AST *node, bool is_expr, types_Type *hint) {
  std_map_Map__5 *mapping = std_map_Map__5_new();
  std_vector_Vector__18 *cases = node->u.match_stmt.cases;
  node->returns=(cases->size > ((u32)0));
  for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(cases); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
    ast_nodes_MatchCase *_case = std_vector_Iterator__18_cur(&__iter);
    {
      ast_nodes_AST *cond = _case->cond;
      char *name;
      types_Type *cond_type = passes_typechecker_TypeChecker_check_expression(this, cond, enum_->type);
      if (!((bool)cond_type)) 
      continue;
      
      if (!types_Type_eq(cond_type, enum_->type)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_hint(cond->span, "Condition does not match expression type", node->u.match_stmt.expr->span, format_string("Match expression is of type '%s'", types_Type_str(enum_->type))));
      } 
      if ((cond_type->base != types_BaseType_Enum)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(cond->span, "Expected an enum value"));
        continue;
      } 
      name=cond->resolved_symbol->name;
      ast_nodes_MatchCase *prev = std_map_Map__5_get(mapping, name, NULL);
      if (((bool)prev)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_hint(cond->span, "Duplicate condition name in match", prev->cond->span, "This condition was previously used here"));
      } 
      std_map_Map__5_insert(mapping, name, _case);
      if (((bool)_case->body)) {
        passes_typechecker_TypeChecker_check_expression_statement(this, node, _case->body, is_expr, hint);
      } 
    }
  }
  ast_nodes_AST *defolt = node->u.match_stmt.defolt;
  if ((mapping->num_items != enum_->fields->size)) {
    if (!((bool)defolt)) {
      passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->span, "Match does not cover all cases", format_string("Only %u of %u cases are covered", mapping->num_items, enum_->fields->size)));
    }  else {
      passes_typechecker_TypeChecker_check_expression_statement(this, node, defolt, is_expr, hint);
    } 
  }  else {
    if (((bool)defolt)) {
      passes_typechecker_TypeChecker_error(this, errors_Error_new(node->u.match_stmt.defolt_span, "`else` case is not needed for this match"));
    } 
  } 
  if ((is_expr && !((bool)node->etype))) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Expression-match must yield a value"));
  } 
  /* defers */
  std_map_Map__5_free(mapping);
}

void passes_typechecker_TypeChecker_check_match(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_expr, types_Type *hint) {
  ast_nodes_AST *expr = node->u.match_stmt.expr;
  types_Type *expr_type = passes_typechecker_TypeChecker_check_expression(this, expr, NULL);
  if (!((bool)expr_type)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Match statement must have a valid expression"));
    return ;
  } 
  if (expr_type->base==types_BaseType_Enum) {
    ast_nodes_Enum *enum_ = expr_type->u.enum_;
    passes_typechecker_TypeChecker_check_match_for_enum(this, enum_, node, is_expr, hint);
    return ;
  } 
  if (((!types_Type_is_integer(expr_type) && !types_Type_is_str(expr_type)) && (expr_type->base != types_BaseType_Char))) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new_note(expr->span, "This type cannot be matched on", format_string("Expression type is '%s'", types_Type_str(expr_type))));
  } 
  std_vector_Vector__18 *cases = node->u.match_stmt.cases;
  node->returns=(cases->size > ((u32)0));
  for (std_vector_Iterator__18 __iter = std_vector_Vector__18_iter(cases); std_vector_Iterator__18_has_value(&__iter); std_vector_Iterator__18_next(&__iter)) {
    ast_nodes_MatchCase *_case = std_vector_Iterator__18_cur(&__iter);
    {
      types_Type *cond_type = passes_typechecker_TypeChecker_check_expression(this, _case->cond, expr_type);
      if (!((bool)cond_type)) 
      continue;
      
      if (!types_Type_eq(cond_type, expr_type)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new_hint(cond_type->span, "Condition does not match expression type", node->u.match_stmt.expr->span, format_string("Match expression is of type '%s'", types_Type_str(cond_type))));
      } 
      if ((((_case->cond->type != ast_nodes_ASTType_IntLiteral) && (_case->cond->type != ast_nodes_ASTType_CharLiteral)) && (_case->cond->type != ast_nodes_ASTType_StringLiteral))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(_case->cond->span, "Match condition must use only literals"));
      } 
      if (((bool)_case->body)) {
        passes_typechecker_TypeChecker_check_expression_statement(this, node, _case->body, is_expr, hint);
      } 
    }
  }
  ast_nodes_AST *defolt = node->u.match_stmt.defolt;
  if (!((bool)defolt)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "`else` case is missing"));
  }  else {
    passes_typechecker_TypeChecker_check_expression_statement(this, node, defolt, is_expr, hint);
  } 
  if (((is_expr && !((bool)node->etype)) && !node->returns)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Expression-match must yield a value"));
  } 
}

void passes_typechecker_TypeChecker_check_if(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_expr, types_Type *hint) {
  types_Type *cond_type = passes_typechecker_TypeChecker_check_expression(this, node->u.if_stmt.cond, passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span));
  if ((((bool)cond_type) && (cond_type->base != types_BaseType_Bool))) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new_note(node->u.if_stmt.cond->span, "Condition must be a boolean", format_string("Got type '%s'", types_Type_str(cond_type))));
  } 
  ast_nodes_AST *body = node->u.if_stmt.body;
  node->returns=true;
  passes_typechecker_TypeChecker_check_expression_statement(this, node, body, is_expr, hint);
  if (((bool)node->u.if_stmt.els)) {
    ast_nodes_AST *else_stmt = node->u.if_stmt.els;
    passes_typechecker_TypeChecker_check_expression_statement(this, node, else_stmt, is_expr, hint);
  }  else   if (is_expr) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "If expressions must have an else branch"));
  }  else {
    node->returns=false;
  } 
  
}

void passes_typechecker_TypeChecker_check_expression_statement(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, ast_nodes_AST *body, bool is_expr, types_Type *hint) {
  switch (body->type) {
    case ast_nodes_ASTType_Match: {
      passes_typechecker_TypeChecker_check_match(this, body, is_expr, hint);
    } break;
    case ast_nodes_ASTType_If: {
      passes_typechecker_TypeChecker_check_if(this, body, is_expr, hint);
    } break;
    case ast_nodes_ASTType_Block: {
      passes_typechecker_TypeChecker_check_block(this, body, is_expr, hint);
    } break;
    default: {
      if (is_expr) {
        passes_typechecker_TypeChecker_check_expression(this, body, hint);
      }  else {
        passes_typechecker_TypeChecker_check_statement(this, body);
      } 
    } break;
  }
  node->returns=(node->returns && body->returns);
  if (!is_expr) 
  return ;
  
  types_Type *ret = body->etype;
  if (body->returns) {
  }  else   if (!((bool)ret)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(body->span, format_string("Must yield a value in this branch, body type is %s", ast_nodes_ASTType_dbg(body->type))));
  }  else   if (!((bool)node->etype)) {
    node->etype=ret;
  }  else   if (!types_Type_eq(node->etype, ret)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new_note(body->span, "Yield type of branch doesn't match previous branches", format_string("Expected type '%s', got '%s'", types_Type_str(node->etype), types_Type_str(ret))));
  } 
  
  
  
}

void passes_typechecker_TypeChecker_check_while(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  ast_scopes_Scope *scope = ast_scopes_Scope_new(passes_typechecker_TypeChecker_scope(this));
  scope->loop_count+=((u32)1);
  passes_generic_pass_GenericPass_push_scope(this->o, scope);
  ast_nodes_AST *cond = node->u.loop.cond;
  ast_nodes_AST *body = node->u.loop.body;
  types_Type *cond_type = passes_typechecker_TypeChecker_check_expression(this, cond, passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span));
  if ((((bool)cond_type) && (cond_type->base != types_BaseType_Bool))) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new_note(cond->span, "Condition must be a boolean", format_string("Got type '%s'", types_Type_str(cond_type))));
  } 
  passes_typechecker_TypeChecker_check_statement(this, body);
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

void passes_typechecker_TypeChecker_check_for(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  ast_scopes_Scope *scope = ast_scopes_Scope_new(passes_typechecker_TypeChecker_scope(this));
  scope->loop_count+=((u32)1);
  passes_generic_pass_GenericPass_push_scope(this->o, scope);
  ast_nodes_AST *init = node->u.loop.init;
  ast_nodes_AST *cond = node->u.loop.cond;
  ast_nodes_AST *step = node->u.loop.step;
  ast_nodes_AST *body = node->u.loop.body;
  if (((bool)init)) 
  passes_typechecker_TypeChecker_check_statement(this, init);
  
  if (((bool)cond)) {
    types_Type *cond_type = passes_typechecker_TypeChecker_check_expression(this, cond, passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span));
    if ((((bool)cond_type) && (cond_type->base != types_BaseType_Bool))) {
      passes_typechecker_TypeChecker_error(this, errors_Error_new_note(cond->span, "Condition must be a boolean", format_string("Got type '%s'", types_Type_str(cond_type))));
    } 
  } 
  if (((bool)step)) 
  passes_typechecker_TypeChecker_check_expression(this, step, NULL);
  
  passes_typechecker_TypeChecker_check_statement(this, body);
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

void passes_typechecker_TypeChecker_check_statement(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  switch (node->type) {
    case ast_nodes_ASTType_Return: {
      ast_nodes_Function *cur_func = passes_typechecker_TypeChecker_scope(this)->cur_func;
      if (!((bool)cur_func)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot return from outside a function"));
        return ;
      } 
      types_Type *expected = cur_func->return_type;
      if (expected->base==types_BaseType_Void) {
        if (((bool)node->u.unary)) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot return a value from a void function"));
        } 
      }  else       if (((bool)node->u.unary)) {
        types_Type *res = passes_typechecker_TypeChecker_check_expression(this, node->u.unary, expected);
        if ((((bool)res) && !types_Type_eq(res, expected))) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Return type %s does not match function return type %s", types_Type_str(res), types_Type_str(expected))));
        } 
      }  else {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Expected a return value for non-void function"));
      } 
      
      node->returns=true;
    } break;
    case ast_nodes_ASTType_Assert: {
      ast_nodes_AST *expr = node->u.assertion.expr;
      types_Type *expr_typ = passes_typechecker_TypeChecker_check_expression(this, expr, passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span));
      if ((((bool)expr_typ) && (expr_typ->base != types_BaseType_Bool))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Can only assert boolean types, got %s", types_Type_str(expr_typ))));
      } 
      if (((bool)node->u.assertion.msg)) {
        types_Type *msg_typ = passes_typechecker_TypeChecker_check_expression(this, node->u.assertion.msg, NULL);
        if ((((bool)msg_typ) && (msg_typ != passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span)))) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Can only assert strings, got %s", types_Type_str(msg_typ))));
        } 
      } 
      if ((expr->type==ast_nodes_ASTType_BoolLiteral && expr->u.bool_literal==false)) {
        node->returns=true;
      } 
    } break;
    case ast_nodes_ASTType_Defer: {
      passes_typechecker_TypeChecker_check_statement(this, node->u.unary);
    } break;
    case ast_nodes_ASTType_Yield: {
      if (!passes_typechecker_TypeChecker_scope(this)->can_yield) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot yield here"));
      } 
      node->etype=passes_typechecker_TypeChecker_check_expression(this, node->u.unary, NULL);
    } break;
    case ast_nodes_ASTType_Import: {
      passes_typechecker_TypeChecker_handle_import_statement(this, node);
    } break;
    case ast_nodes_ASTType_Break:
    case ast_nodes_ASTType_Continue: {
      if (passes_typechecker_TypeChecker_scope(this)->loop_count==((u32)0)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("%s statement outside of loop", ast_nodes_ASTType_dbg(node->type))));
      } 
    } break;
    case ast_nodes_ASTType_If: {
      passes_typechecker_TypeChecker_check_if(this, node, false, NULL);
    } break;
    case ast_nodes_ASTType_While: {
      passes_typechecker_TypeChecker_check_while(this, node);
    } break;
    case ast_nodes_ASTType_For: {
      passes_typechecker_TypeChecker_check_for(this, node);
    } break;
    case ast_nodes_ASTType_Block: {
      passes_typechecker_TypeChecker_check_block(this, node, false, NULL);
    } break;
    case ast_nodes_ASTType_Match: {
      passes_typechecker_TypeChecker_check_match(this, node, false, NULL);
    } break;
    case ast_nodes_ASTType_VarDeclaration: {
      ast_nodes_Variable *var = node->u.var_decl.var;
      ast_scopes_Symbol *res = ast_scopes_Scope_lookup_local(passes_typechecker_TypeChecker_scope(this), var->sym->name);
      if (((bool)res)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Variable %s already exists in this scope", var->sym->name)));
        return ;
      } 
      ast_scopes_Symbol *sym = var->sym;
      ast_scopes_Scope_insert(passes_typechecker_TypeChecker_scope(this), var->sym->name, sym);
      if (((bool)var->type)) {
        var->type=passes_typechecker_TypeChecker_resolve_type(this, var->type, false, true, true);
        if (!((bool)var->type)) 
        return ;
        
      } 
      ast_nodes_AST *init = node->u.var_decl.init;
      if (((bool)init)) {
        types_Type *res = passes_typechecker_TypeChecker_check_expression(this, init, var->type);
        if (!((bool)res)) 
        return ;
        
        if (((((bool)res) && ((bool)var->type)) && !types_Type_eq(res, var->type))) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(init->span, format_string("Variable %s has type %s but initializer has type %s", var->sym->name, types_Type_str(var->type), types_Type_str(res))));
        }  else         if (!((bool)var->type)) {
          var->type=res;
        } 
        
      }  else       if (!((bool)var->type)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Variable %s has no type and no initializer", var->sym->name)));
      } 
      
    } break;
    default: {
      passes_typechecker_TypeChecker_check_expression(this, node, NULL);
    } break;
  }
}

void passes_typechecker_TypeChecker_check_function(passes_typechecker_TypeChecker *this, ast_nodes_Function *func) {
  func->sym->comment=passes_typechecker_TypeChecker_resolve_doc_links(this, func->sym->comment, func->span);
  if ((func->is_method && func->parent_type->base==types_BaseType_Structure)) {
    ast_nodes_Structure *struc = func->parent_type->u.struc;
    if (struc->is_templated) 
    return ;
    
  } 
  if (func->checked) 
  return ;
  
  func->checked=true;
  ast_scopes_Scope *new_scope = ast_scopes_Scope_new(func->scope);
  std_vector_Vector__4 *params = func->params;
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
    {
      ast_nodes_AST *default_expr = param->default_value;
      if (((bool)default_expr)) {
        types_Type *default_type = passes_typechecker_TypeChecker_check_expression(this, default_expr, param->type);
        if ((((bool)default_type) && !types_Type_eq(default_type, param->type))) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(default_expr->span, format_string("Default argument has type %s but expected %s", types_Type_str(default_type), types_Type_str(param->type))));
        } 
      } 
      std_map_Map__1_insert(new_scope->items, param->sym->name, param->sym);
    }
  }
  new_scope->cur_func=func;
  if (func->sym->is_extern) 
  return ;
  
  passes_generic_pass_GenericPass_push_scope(this->o, new_scope);
  passes_typechecker_TypeChecker_check_statement(this, func->body);
  if (((!func->body->returns && (func->return_type->base != types_BaseType_Void)) && !str_eq(func->sym->name, "main"))) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(func->sym->span, "Function does not always return"));
  } 
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

void passes_typechecker_TypeChecker_handle_namespace_imports(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns) {
  passes_generic_pass_GenericPass_push_namespace(this->o, ns);
  passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->imports); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *import_ = std_vector_Iterator__12_cur(&__iter);
    {
      passes_typechecker_TypeChecker_handle_import_statement(this, import_);
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_typechecker_TypeChecker_handle_namespace_imports(this, child);
    }
  }
  passes_generic_pass_GenericPass_pop_scope(this->o);
  passes_generic_pass_GenericPass_pop_namespace(this->o);
}

void passes_typechecker_TypeChecker_check_global_variable(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  ast_nodes_AST *init = node->u.var_decl.init;
  if (!((bool)init)) 
  return ;
  
  ast_nodes_Variable *var = node->u.var_decl.var;
  var->sym->comment=passes_typechecker_TypeChecker_resolve_doc_links(this, var->sym->comment, node->span);
  types_Type *init_type = passes_typechecker_TypeChecker_check_expression(this, init, var->type);
  if ((!((bool)init_type) || !((bool)var->type))) 
  return ;
  
  if (!types_Type_eq(init_type, var->type)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(init->span, format_string("Variable %s has type %s but initializer has type %s", var->sym->name, types_Type_str(var->type), types_Type_str(init_type))));
  } 
}

void passes_typechecker_TypeChecker_check_namespace(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns) {
  passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  passes_generic_pass_GenericPass_push_namespace(this->o, ns);
  ns->sym->comment=passes_typechecker_TypeChecker_resolve_doc_links(this, ns->sym->comment, ns->span);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(ns->functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
    {
      passes_typechecker_TypeChecker_check_function(this, func);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->constants); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      ast_nodes_AST *init = node->u.var_decl.init;
      if (((bool)init)) {
        passes_typechecker_TypeChecker_check_const_expression(this, init, NULL);
      }  else       if (!node->u.var_decl.var->sym->is_extern) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Constant must have an initializer"));
      } 
      
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->enums); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    ast_nodes_Enum *enum_ = std_vector_Iterator__13_cur(&__iter);
    {
      enum_->sym->comment=passes_typechecker_TypeChecker_resolve_doc_links(this, enum_->sym->comment, enum_->span);
      for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(enum_->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
        ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
        {
          field->sym->comment=passes_typechecker_TypeChecker_resolve_doc_links(this, field->sym->comment, field->sym->span);
        }
      }
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->variables); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      passes_typechecker_TypeChecker_check_global_variable(this, node);
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_typechecker_TypeChecker_check_namespace(this, child);
    }
  }
  passes_generic_pass_GenericPass_pop_namespace(this->o);
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

char *passes_typechecker_TypeChecker_resolve_doc_links(passes_typechecker_TypeChecker *this, char *doc, std_span_Span obj_span) {
  if (!((bool)doc)) 
  return NULL;
  
  std_buffer_Buffer buffer = std_buffer_Buffer_make(((u32)16));
  u32 prev = ((u32)0);
  u32 doc_len = str_len(doc);
  for (u32 i = ((u32)0); (i < (doc_len - ((u32)1))); i++) {
    if (doc[i]=='\\') {
      i++;
    } 
    if ((doc[i]==doc[(i + ((u32)1))] && doc[(i + ((u32)1))]=='{')) {
      std_buffer_Buffer_putsf(&buffer, str_substring(doc, prev, (i - prev)));
      prev=(i + ((u32)2));
      i++;
    }  else     if ((doc[i]==doc[(i + ((u32)1))] && doc[(i + ((u32)1))]=='}')) {
      char *part = str_substring(doc, prev, (i - prev));
      lexer_Lexer lexer = lexer_Lexer_make(part, NULL);
      std_span_Span span = (std_span_Span){.start=obj_span.start, .end=obj_span.start};
      std_vector_Vector__6 *tokens = lexer_Lexer_lex(&lexer);
      if ((lexer.errors->size > ((u32)0))) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(span, format_string("Invalid documentation link: '%s'", part)));
        return doc;
      } 
      parser_Parser parser = parser_Parser_make(this->o->program, passes_generic_pass_GenericPass_ns(this->o));
      parser.tokens=tokens;
      parser.curr=((u32)0);
      ast_nodes_AST *ident = parser_Parser_parse_scoped_identifier(&parser);
      if (!((bool)ident)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(span, format_string("Invalid documentation link: '%s'", part)));
        return doc;
      } 
      ast_scopes_Symbol *sym = passes_typechecker_TypeChecker_resolve_scoped_identifier(this, ident, false, NULL, true);
      if (!((bool)sym)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(span, format_string("Invalid documentation link: '%s'", part)));
        return doc;
      } 
      char *linked_part = ({ char *__yield_0;
        switch (sym->type) {
          case ast_scopes_SymbolType_Structure: {
            __yield_0 = format_string("%x", sym->u.struc->type);
          } break;
          case ast_scopes_SymbolType_Function: {
            __yield_0 = format_string("%x", sym->u.func);
          } break;
          case ast_scopes_SymbolType_Enum: {
            __yield_0 = format_string("%x", sym->u.enum_->type);
          } break;
          case ast_scopes_SymbolType_TypeDef: {
            __yield_0 = format_string("%x", sym->u.type_def);
          } break;
          case ast_scopes_SymbolType_Variable: {
            __yield_0 = format_string("%x", sym->u.var);
          } break;
          case ast_scopes_SymbolType_Constant: {
            __yield_0 = format_string("%x", sym->u.var);
          } break;
          case ast_scopes_SymbolType_Namespace: {
            __yield_0 = format_string("%x", sym->u.ns);
          } break;
        }
;__yield_0; });
      std_buffer_Buffer_puts(&buffer, "{{");
      std_buffer_Buffer_putsf(&buffer, linked_part);
      std_buffer_Buffer_puts(&buffer, "}}");
      free(part);
      prev=(i + ((u32)2));
      i++;
    } 
    
  }
  std_buffer_Buffer_putsf(&buffer, str_substring(doc, prev, doc_len));
  return std_buffer_Buffer_str(&buffer);
}

types_Type *passes_typechecker_TypeChecker_check_const_expression(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, types_Type *hint) {
  types_Type *typ = ({ types_Type *__yield_0;
    switch (node->type) {
      case ast_nodes_ASTType_Identifier:
      case ast_nodes_ASTType_NSLookup: {
        ast_scopes_Symbol *sym = passes_typechecker_TypeChecker_resolve_scoped_identifier(this, node, true, hint, true);
        if (!((bool)sym)) 
        return NULL;
        
        if ((sym->type != ast_scopes_SymbolType_Constant)) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new_hint(node->span, "Cannot use a non-constant value in a constant expression", sym->span, "Value was defined here"));
          return NULL;
        } 
        __yield_0 = sym->u.var->type;
      } break;
      case ast_nodes_ASTType_IntLiteral: {
        __yield_0 = passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_I32, node->span);
      } break;
      case ast_nodes_ASTType_FloatLiteral: {
        __yield_0 = passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_F32, node->span);
      } break;
      case ast_nodes_ASTType_BoolLiteral: {
        __yield_0 = passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Bool, node->span);
      } break;
      case ast_nodes_ASTType_CharLiteral: {
        __yield_0 = passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Char, node->span);
      } break;
      case ast_nodes_ASTType_StringLiteral: {
        __yield_0 = passes_typechecker_TypeChecker_get_type_by_name(this, "str", node->span);
      } break;
      case ast_nodes_ASTType_Plus:
      case ast_nodes_ASTType_Minus:
      case ast_nodes_ASTType_Multiply:
      case ast_nodes_ASTType_Divide:
      case ast_nodes_ASTType_LessThan:
      case ast_nodes_ASTType_LessThanEquals:
      case ast_nodes_ASTType_GreaterThan:
      case ast_nodes_ASTType_GreaterThanEquals:
      case ast_nodes_ASTType_Equals:
      case ast_nodes_ASTType_NotEquals:
      case ast_nodes_ASTType_And:
      case ast_nodes_ASTType_Or:
      case ast_nodes_ASTType_Modulus:
      case ast_nodes_ASTType_BitwiseNot:
      case ast_nodes_ASTType_BitwiseOr:
      case ast_nodes_ASTType_BitwiseAnd:
      case ast_nodes_ASTType_BitwiseXor:
      case ast_nodes_ASTType_LeftShift:
      case ast_nodes_ASTType_RightShift: {
        types_Type *lhs = passes_typechecker_TypeChecker_check_const_expression(this, node->u.binary.lhs, NULL);
        types_Type *rhs = passes_typechecker_TypeChecker_check_const_expression(this, node->u.binary.rhs, NULL);
        if ((!((bool)lhs) || !((bool)rhs))) 
        return NULL;
        
        if ((lhs->base==types_BaseType_Pointer || rhs->base==types_BaseType_Pointer)) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot do pointer arithmetic in constant expressions"));
        } 
        __yield_0 = passes_typechecker_TypeChecker_check_binary_op(this, node, types_Type_unaliased(lhs), types_Type_unaliased(rhs));
      } break;
      case ast_nodes_ASTType_Negate: {
        if (!((bool)hint)) {
          hint=passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_I32, node->span);
        } 
        types_Type *typ = passes_typechecker_TypeChecker_check_const_expression(this, node->u.unary, hint);
        if (!((bool)typ)) 
        return NULL;
        
        if (!types_Type_is_numeric(typ)) {
          passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("Cannot negate non-numeric type: %s", types_Type_str(typ))));
          return NULL;
        } 
        return typ;
      } break;
      default: {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Unsupported operator in constant expression"));
        return NULL;
      } break;
    }
;__yield_0; });
  node->etype=typ;
  return typ;
}

void passes_typechecker_TypeChecker_handle_import_path_base(passes_typechecker_TypeChecker *this, std_vector_Vector__5 *parts, ast_scopes_Symbol *base, char *alias, i32 start_idx) {
  for (u32 i = ((u32)start_idx); (i < parts->size); i+=((u32)1)) {
    ast_nodes_ImportPart *part = std_vector_Vector__5_at(parts, i);
    switch (part->type) {
      case ast_nodes_ImportPartType_Wildcard: {
        passes_generic_pass_GenericPass_import_all_from_symbol(this->o, base);
        return ;
      } break;
      case ast_nodes_ImportPartType_Multiple: {
        std_vector_Vector__17 *paths = part->u.paths;
        for (u32 j = ((u32)0); (j < paths->size); j+=((u32)1)) {
          std_vector_Vector__5 *path = std_vector_Vector__17_at(paths, j);
          passes_typechecker_TypeChecker_handle_import_path_base(this, path, base, alias, 0);
        }
        return ;
      } break;
      case ast_nodes_ImportPartType_Single: {
      } break;
    }
    char *name = part->u.single.name;
    if (str_eq(name, "this")) {
      if (((i + ((u32)1)) != parts->size)) {
        passes_typechecker_TypeChecker_error(this, errors_Error_new(part->span, "`this` can only be used as the last part of an import path"));
        return ;
      } 
      if (((bool)part->u.single.alias)) 
      alias=part->u.single.alias;
      
      break;
    } 
    ast_scopes_Symbol *new_base = passes_generic_pass_GenericPass_lookup_in_symbol(this->o, base, name, part->u.single.alias_span, false);
    if (!((bool)new_base)) {
      passes_typechecker_TypeChecker_error(this, errors_Error_new(part->span, format_string("Invalid import, namespace %s does not exist", name)));
      return ;
    } 
    base=new_base;
    alias=part->u.single.alias;
    if (!((bool)alias)) 
    alias=name;
    
  }
  passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, base, alias);
}

void passes_typechecker_TypeChecker_handle_import_statement(passes_typechecker_TypeChecker *this, ast_nodes_AST *node) {
  ast_nodes_Import path = node->u.import_path;
  ast_nodes_ImportPart *part = std_vector_Vector__5_at(path.parts, ((u32)0));
  if ((part->type != ast_nodes_ImportPartType_Single)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(part->span, "Invalid import, first part must be a single identifier"));
    return ;
  } 
  char *name = part->u.single.name;
  char *alias = part->u.single.alias;
  if (!((bool)alias)) 
  alias=name;
  
  ast_scopes_Symbol *base = ({ ast_scopes_Symbol *__yield_0;
    switch (path.type) {
      case ast_nodes_ImportType_GlobalNamespace: {
        __yield_0 = ast_program_Namespace_find_importable_symbol(this->o->program->global, name);
      } break;
      case ast_nodes_ImportType_ProjectNamespace: {
        __yield_0 = ast_program_Namespace_find_importable_symbol(this->o->program->project_root, name);
      } break;
      case ast_nodes_ImportType_ParentNamespace: {
        ast_program_Namespace *cur = passes_generic_pass_GenericPass_ns(this->o);
        for (u32 i = ((u32)0); (i < path.parent_count); i+=((u32)1)) {
          if (!((bool)cur->parent)) {
            errors_Error_panic(passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, "Cannot import from parent of root namespace")));
            return ;
          } 
          cur=cur->parent;
        }
        __yield_0 = ast_program_Namespace_find_importable_symbol(cur, name);
      } break;
      case ast_nodes_ImportType_CurrentScope: {
        __yield_0 = ast_scopes_Scope_lookup_recursive(passes_generic_pass_GenericPass_scope(this->o), name);
      } break;
    }
;__yield_0; });
  if (!((bool)base)) {
    passes_typechecker_TypeChecker_error(this, errors_Error_new(part->span, format_string("Couldn't import %s", name)));
    return ;
  } 
  passes_typechecker_TypeChecker_handle_import_path_base(this, path.parts, base, alias, 1);
}

void passes_typechecker_TypeChecker_pre_check_function(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns, ast_nodes_Function *func) {
  if (func->is_method) {
    std_span_Span parent_span = func->parent_type->span;
    types_Type *parent_type = passes_typechecker_TypeChecker_resolve_type(this, func->parent_type, true, true, true);
    if (!((bool)parent_type)) {
      passes_generic_pass_GenericPass_error(this->o, errors_Error_new(parent_span, "Could not find this type"));
      return ;
    } 
    if (!types_Type_can_have_methods(parent_type)) {
      passes_generic_pass_GenericPass_error(this->o, errors_Error_new(parent_span, "This type cannot have methods"));
      return ;
    } 
    switch (parent_type->base) {
      case types_BaseType_Structure: {
        ast_nodes_Structure *struc = parent_type->u.struc;
        ast_nodes_Variable *name = ast_nodes_Structure_get_field(struc, func->sym->name);
        if (((bool)name)) {
          passes_generic_pass_GenericPass_error(this->o, errors_Error_new_hint(func->sym->span, "Field with this name already exists", name->sym->span, "Previous definition here"));
        } 
      } break;
      case types_BaseType_Enum: {
        ast_nodes_Enum *enum_ = parent_type->u.enum_;
        ast_nodes_Variable *name = ast_nodes_Enum_get_field(enum_, func->sym->name);
        if (((bool)name)) {
          passes_generic_pass_GenericPass_error(this->o, errors_Error_new_hint(func->sym->span, "Enum value with this name already exists", name->sym->span, "Previous definition here"));
        } 
      } break;
      default: {
      } break;
    }
    ast_nodes_Function *res = std_map_Map__4_get(parent_type->methods, func->sym->name, NULL);
    if (((bool)res)) {
      passes_generic_pass_GenericPass_error(this->o, errors_Error_new_hint(func->sym->span, "Method with this name already exists", res->sym->span, "Previous definition here"));
      return ;
    } 
    ast_scopes_Symbol_update_parent(func->sym, parent_type->sym);
    std_map_Map__4_insert(parent_type->methods, func->sym->name, func);
    func->parent_type=parent_type;
  }  else {
    ast_scopes_Symbol *item = func->sym;
    passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, item, NULL);
  } 
  func->scope=passes_typechecker_TypeChecker_scope(this);
}

void passes_typechecker_TypeChecker_loosely_resolve_templated_struct(passes_typechecker_TypeChecker *this, ast_nodes_Structure *struc) {
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      types_Type *res = passes_typechecker_TypeChecker_resolve_type(this, field->type, true, false, false);
      field->type=res;
    }
  }
}

void passes_typechecker_TypeChecker_resolve_struct(passes_typechecker_TypeChecker *this, ast_nodes_Structure *struc) {
  std_vector_Vector__4 *fields = struc->fields;
  struc->sym->comment=passes_typechecker_TypeChecker_resolve_doc_links(this, struc->sym->comment, struc->span);
  if (struc->is_templated) {
    passes_typechecker_TypeChecker_loosely_resolve_templated_struct(this, struc);
    return ;
  } 
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      types_Type *res = passes_typechecker_TypeChecker_resolve_type(this, field->type, false, true, true);
      field->sym->comment=passes_typechecker_TypeChecker_resolve_doc_links(this, field->sym->comment, field->sym->span);
      if (!((bool)res)) {
        passes_generic_pass_GenericPass_error(this->o, errors_Error_new(field->sym->span, "Couldn't resolve type"));
      }  else {
        field->type=res;
      } 
    }
  }
}

void passes_typechecker_TypeChecker_check_function_declaration(passes_typechecker_TypeChecker *this, ast_nodes_Function *func) {
  bool allow_incomplete = false;
  bool error = true;
  bool resolve_templates = true;
  if ((func->is_method && func->parent_type->base==types_BaseType_Structure)) {
    ast_nodes_Structure *struc = func->parent_type->u.struc;
    if (struc->is_templated) {
      allow_incomplete=true;
      error=false;
      resolve_templates=false;
    } 
  } 
  func->return_type=passes_typechecker_TypeChecker_resolve_type(this, func->return_type, allow_incomplete, error, resolve_templates);
  if (!((bool)func->return_type)) {
    func->return_type=passes_typechecker_TypeChecker_get_base_type(this, types_BaseType_Void, func->sym->span);
  } 
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(func->params); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *param = std_vector_Iterator__4_cur(&__iter);
    {
      param->type=passes_typechecker_TypeChecker_resolve_type(this, param->type, allow_incomplete, error, resolve_templates);
    }
  }
  types_Type *typ = types_Type_new_resolved(types_BaseType_Function, func->sym->span);
  typ->u.func=types_FunctionType_from_func(func);
  func->type=typ;
}

void passes_typechecker_TypeChecker_check_post_import(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns) {
  passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  passes_generic_pass_GenericPass_push_namespace(this->o, ns);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(ns->functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
    {
      passes_typechecker_TypeChecker_check_function_declaration(this, func);
    }
  }
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(ns->structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      passes_typechecker_TypeChecker_resolve_struct(this, struc);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->variables); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      passes_typechecker_TypeChecker_pre_check_globals(this, node, false);
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_typechecker_TypeChecker_check_post_import(this, child);
    }
  }
  passes_generic_pass_GenericPass_pop_scope(this->o);
  passes_generic_pass_GenericPass_pop_namespace(this->o);
}

void passes_typechecker_TypeChecker_pre_check_globals(passes_typechecker_TypeChecker *this, ast_nodes_AST *node, bool is_const) {
  ast_nodes_Variable *var = node->u.var_decl.var;
  if (!((bool)var->type)) {
    char *c = (is_const ? "Constant" : "Global variable");
    passes_typechecker_TypeChecker_error(this, errors_Error_new(node->span, format_string("%s must have a type", c)));
    return ;
  } 
  var->type=passes_typechecker_TypeChecker_resolve_type(this, var->type, false, true, true);
  if (!((bool)var->type)) 
  return ;
  
  passes_generic_pass_GenericPass_insert_into_scope_checked(this->o, var->sym, NULL);
}

void passes_typechecker_TypeChecker_pre_check_constants(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns) {
  passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->constants); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      passes_typechecker_TypeChecker_pre_check_globals(this, node, true);
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_typechecker_TypeChecker_pre_check_constants(this, child);
    }
  }
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

void passes_typechecker_TypeChecker_pre_check_namespace(passes_typechecker_TypeChecker *this, ast_program_Namespace *ns) {
  passes_generic_pass_GenericPass_push_scope(this->o, ns->scope);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(ns->functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
    {
      passes_typechecker_TypeChecker_pre_check_function(this, ns, func);
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_typechecker_TypeChecker_pre_check_namespace(this, child);
    }
  }
  passes_generic_pass_GenericPass_pop_scope(this->o);
}

void passes_typechecker_TypeChecker_run(ast_program_Program *program) {
  passes_typechecker_TypeChecker pass = (passes_typechecker_TypeChecker){.o=passes_generic_pass_GenericPass_new(program), .unchecked_functions=std_vector_Vector__10_new(((u32)16))};
  passes_typechecker_TypeChecker_pre_check_constants(&pass, program->global);
  passes_typechecker_TypeChecker_pre_check_namespace(&pass, program->global);
  passes_typechecker_TypeChecker_handle_namespace_imports(&pass, program->global);
  passes_typechecker_TypeChecker_check_post_import(&pass, program->global);
  passes_typechecker_TypeChecker_check_namespace(&pass, program->global);
  while ((pass.unchecked_functions->size > ((u32)0))) {
    ast_nodes_Function *func = ((ast_nodes_Function *)std_vector_Vector__10_pop(pass.unchecked_functions));
    passes_typechecker_TypeChecker_check_function(&pass, func);
  }
}

void passes_namespace_dump_NamespaceDump_print_indent(passes_namespace_dump_NamespaceDump *this) {
  for (u32 i = ((u32)0); (i < this->indent); i+=((u32)1)) {
    printf("  ");
  }
}

void passes_namespace_dump_NamespaceDump_print_namespace(passes_namespace_dump_NamespaceDump *this, ast_program_Namespace *ns) {
  passes_namespace_dump_NamespaceDump_print_indent(this);
  if (str_len(ns->sym->display)==((u32)0)) {
    printf("(ns) <root>""\n");
  }  else {
    printf("(ns) %s\n", ns->sym->display);
  } 
  this->indent+=((u32)1);
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(ns->functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
    {
      passes_namespace_dump_NamespaceDump_print_indent(this);
      printf("(func) %s\n", func->sym->display);
    }
  }
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(ns->structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      passes_namespace_dump_NamespaceDump_print_indent(this);
      printf("(struct) %s\n", struc->sym->display);
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(ns->enums); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    ast_nodes_Enum *enum_ = std_vector_Iterator__13_cur(&__iter);
    {
      passes_namespace_dump_NamespaceDump_print_indent(this);
      printf("(enum) %s\n", enum_->sym->display);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->variables); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      ast_nodes_Variable *var = node->u.var_decl.var;
      passes_namespace_dump_NamespaceDump_print_indent(this);
      printf("(var) %s\n", var->sym->display);
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(ns->constants); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      ast_nodes_Variable *var = node->u.var_decl.var;
      passes_namespace_dump_NamespaceDump_print_indent(this);
      printf("(const) %s\n", var->sym->display);
    }
  }
  this->indent-=((u32)1);
}

void passes_namespace_dump_NamespaceDump_run(ast_program_Program *program) {
  passes_namespace_dump_NamespaceDump pass = (passes_namespace_dump_NamespaceDump){.indent=((u32)0)};
  printf("---------------------- Namespace Dump ----------------------""\n");
  for (ast_program_NSIterator __iter = ast_program_Program_iter_namespaces(program); ast_program_NSIterator_has_value(&__iter); ast_program_NSIterator_next(&__iter)) {
    ast_program_Namespace *ns = ast_program_NSIterator_cur(&__iter);
    {
      passes_namespace_dump_NamespaceDump_print_namespace(&pass, ns);
    }
  }
  printf("------------------------------------------------------------""\n");
}

passes_reorder_structs_ReorderStructs *passes_reorder_structs_ReorderStructs_new(ast_program_Program *program) {
  passes_reorder_structs_ReorderStructs *pass = ((passes_reorder_structs_ReorderStructs *)calloc(((u32)1), ((u32)sizeof(passes_reorder_structs_ReorderStructs))));
  *pass=(passes_reorder_structs_ReorderStructs){.o=passes_generic_pass_GenericPass_new(program), .all_structs=std_vector_Vector__11_new(((u32)16)), .done=std_map_Map__0_new()};
  return pass;
}

void passes_reorder_structs_ReorderStructs_free(passes_reorder_structs_ReorderStructs *this) {
  std_vector_Vector__11_free(this->all_structs);
  std_map_Map__0_free(this->done);
  free(this->o);
  free(this);
}

void passes_reorder_structs_ReorderStructs_collect_all_structs(passes_reorder_structs_ReorderStructs *this, ast_program_Namespace *ns) {
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(ns->structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      if (struc->is_templated) {
        for (std_vector_Iterator__15 __iter = std_vector_Vector__15_iter(struc->template_instances); std_vector_Iterator__15_has_value(&__iter); std_vector_Iterator__15_next(&__iter)) {
          ast_nodes_TemplateInstance *instance = std_vector_Iterator__15_cur(&__iter);
          {
            ast_scopes_Symbol *sym = instance->resolved;
            ae_assert(sym->type==ast_scopes_SymbolType_Structure, "compiler/passes/reorder_structs.oc:42:24: Assertion failed: `sym.type == Structure`", NULL);
            std_vector_Vector__11_push(this->all_structs, sym->u.struc);
          }
        }
      }  else {
        std_vector_Vector__11_push(this->all_structs, struc);
      } 
    }
  }
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(ns->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *child = std_map_ValueIterator__2_cur(&__iter);
    {
      passes_reorder_structs_ReorderStructs_collect_all_structs(this, child);
    }
  }
}

void passes_reorder_structs_ReorderStructs_dfs(passes_reorder_structs_ReorderStructs *this, ast_nodes_Structure *struc) {
  if (((bool)std_map_Map__0_get(this->done, struc->sym->out_name, NULL))) 
  return ;
  
  std_map_Map__0_insert(this->done, struc->sym->out_name, struc);
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(struc->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      if ((((bool)field->type) && field->type->base==types_BaseType_Structure)) {
        ast_nodes_Structure *other = field->type->u.struc;
        passes_reorder_structs_ReorderStructs_dfs(this, other);
      } 
    }
  }
  std_vector_Vector__11_push(this->o->program->ordered_structs, struc);
}

void passes_reorder_structs_ReorderStructs_reorder_structs(passes_reorder_structs_ReorderStructs *this) {
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(this->all_structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      passes_reorder_structs_ReorderStructs_dfs(this, struc);
    }
  }
}

void passes_reorder_structs_ReorderStructs_run(ast_program_Program *program) {
  passes_reorder_structs_ReorderStructs *pass = passes_reorder_structs_ReorderStructs_new(program);
  passes_reorder_structs_ReorderStructs_collect_all_structs(pass, program->global);
  passes_reorder_structs_ReorderStructs_reorder_structs(pass);
  passes_reorder_structs_ReorderStructs_free(pass);
}

ast_scopes_Symbol *ast_scopes_Symbol_new(ast_scopes_SymbolType type, ast_program_Namespace *ns, char *name, char *display, char *out_name, std_span_Span span) {
  ast_scopes_Symbol *item = ((ast_scopes_Symbol *)calloc(((u32)1), ((u32)sizeof(ast_scopes_Symbol))));
  item->name=name;
  item->display=display;
  item->out_name=out_name;
  item->span=span;
  item->type=type;
  item->ns=ns;
  return item;
}

char *ast_scopes_Symbol_join_display(char *a, char *b) {
  return (str_len(a)==((u32)0) ? b : format_string("%s::%s", a, b));
}

char *ast_scopes_Symbol_join_out_name(char *a, char *b) {
  return (str_len(a)==((u32)0) ? b : format_string("%s_%s", a, b));
}

ast_scopes_Symbol *ast_scopes_Symbol_new_with_parent(ast_scopes_SymbolType type, ast_program_Namespace *ns, ast_scopes_Symbol *parent, char *name, std_span_Span span) {
  char *display = ast_scopes_Symbol_join_display(parent->display, name);
  char *out_name = ast_scopes_Symbol_join_out_name(parent->out_name, name);
  return ast_scopes_Symbol_new(type, ns, name, display, out_name, span);
}

void ast_scopes_Symbol_update_parent(ast_scopes_Symbol *this, ast_scopes_Symbol *parent) {
  this->display=ast_scopes_Symbol_join_display(parent->display, this->name);
  if (!this->is_extern) {
    this->out_name=ast_scopes_Symbol_join_out_name(parent->out_name, this->name);
  } 
}

ast_scopes_Symbol *ast_scopes_Symbol_from_local_variable(char *name, ast_nodes_Variable *var, std_span_Span span) {
  ast_scopes_Symbol *item = ast_scopes_Symbol_new(ast_scopes_SymbolType_Variable, NULL, name, name, name, span);
  item->u.var=var;
  return item;
}

ast_scopes_Symbol *ast_scopes_Symbol_remove_alias(ast_scopes_Symbol *this) {
  return ({ ast_scopes_Symbol *__yield_0;
    switch (this->type) {
      case ast_scopes_SymbolType_TypeDef: {
        if ((this->u.type_def->sym != this)) {
          return ast_scopes_Symbol_remove_alias(this->u.type_def->sym);
        } 
        return this;
      } break;
      default: {
        __yield_0 = this;
      } break;
    }
;__yield_0; });
}

ast_scopes_Scope *ast_scopes_Scope_new(ast_scopes_Scope *parent) {
  ast_scopes_Scope *scope = ((ast_scopes_Scope *)calloc(((u32)1), ((u32)sizeof(ast_scopes_Scope))));
  scope->items=std_map_Map__1_new();
  scope->defers=std_vector_Vector__12_new(((u32)16));
  if (((bool)parent)) {
    scope->loop_count=parent->loop_count;
    scope->can_yield=parent->can_yield;
    scope->cur_func=parent->cur_func;
    scope->parent=parent;
  } 
  return scope;
}

ast_scopes_Symbol *ast_scopes_Scope_lookup_recursive(ast_scopes_Scope *this, char *name) {
  ast_scopes_Symbol *item = std_map_Map__1_get(this->items, name, NULL);
  if (((bool)item)) 
  return item;
  
  if (((bool)this->parent)) 
  return ast_scopes_Scope_lookup_recursive(this->parent, name);
  
  return NULL;
}

ast_scopes_Symbol *ast_scopes_Scope_lookup_local(ast_scopes_Scope *this, char *name) {
  return std_map_Map__1_get(this->items, name, NULL);
}

void ast_scopes_Scope_insert(ast_scopes_Scope *this, char *name, ast_scopes_Symbol *symbol) {
  std_map_Map__1_insert(this->items, name, symbol);
}

ast_program_Namespace *ast_program_Namespace_new(ast_program_Namespace *parent, char *path) {
  ast_program_Namespace *ns = ((ast_program_Namespace *)calloc(((u32)1), ((u32)sizeof(ast_program_Namespace))));
  ns->parent=parent;
  ast_scopes_Scope *parent_scope = (((bool)parent) ? parent->scope : NULL);
  ns->scope=ast_scopes_Scope_new(parent_scope);
  ns->functions=std_vector_Vector__10_new(((u32)16));
  ns->structs=std_vector_Vector__11_new(((u32)16));
  ns->enums=std_vector_Vector__13_new(((u32)16));
  ns->constants=std_vector_Vector__12_new(((u32)16));
  ns->variables=std_vector_Vector__12_new(((u32)16));
  ns->namespaces=std_map_Map__2_new();
  ns->imports=std_vector_Vector__12_new(((u32)16));
  ns->path=path;
  ns->is_a_file=false;
  return ns;
}

ast_scopes_Symbol *ast_program_Namespace_find_importable_symbol(ast_program_Namespace *this, char *name) {
  ast_program_Namespace *item = std_map_Map__2_get(this->namespaces, name, NULL);
  if (((bool)item)) 
  return item->sym;
  
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(this->constants); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      ast_nodes_Variable *var = node->u.var_decl.var;
      if (str_eq(var->sym->name, name)) 
      return var->sym;
      
    }
  }
  for (std_vector_Iterator__12 __iter = std_vector_Vector__12_iter(this->variables); std_vector_Iterator__12_has_value(&__iter); std_vector_Iterator__12_next(&__iter)) {
    ast_nodes_AST *node = std_vector_Iterator__12_cur(&__iter);
    {
      ast_nodes_Variable *var = node->u.var_decl.var;
      if (str_eq(var->sym->name, name)) 
      return var->sym;
      
    }
  }
  for (std_vector_Iterator__10 __iter = std_vector_Vector__10_iter(this->functions); std_vector_Iterator__10_has_value(&__iter); std_vector_Iterator__10_next(&__iter)) {
    ast_nodes_Function *func = std_vector_Iterator__10_cur(&__iter);
    {
      if (func->is_method) 
      continue;
      
      if (str_eq(func->sym->name, name)) 
      return func->sym;
      
    }
  }
  for (std_vector_Iterator__11 __iter = std_vector_Vector__11_iter(this->structs); std_vector_Iterator__11_has_value(&__iter); std_vector_Iterator__11_next(&__iter)) {
    ast_nodes_Structure *struc = std_vector_Iterator__11_cur(&__iter);
    {
      if (str_eq(struc->sym->name, name)) 
      return struc->sym;
      
    }
  }
  for (std_vector_Iterator__13 __iter = std_vector_Vector__13_iter(this->enums); std_vector_Iterator__13_has_value(&__iter); std_vector_Iterator__13_next(&__iter)) {
    ast_nodes_Enum *enum_ = std_vector_Iterator__13_cur(&__iter);
    {
      if (str_eq(enum_->sym->name, name)) 
      return enum_->sym;
      
    }
  }
  return NULL;
}

ast_program_Program *ast_program_Program_new(void) {
  ast_program_Program *prog = ((ast_program_Program *)calloc(((u32)1), ((u32)sizeof(ast_program_Program))));
  prog->global=ast_program_Namespace_new(NULL, "");
  prog->ordered_structs=std_vector_Vector__11_new(((u32)16));
  prog->global->sym=ast_scopes_Symbol_new(ast_scopes_SymbolType_Namespace, prog->global, "", "", "", std_span_Span_default());
  prog->global->sym->u.ns=prog->global;
  prog->error_level=((u32)1);
  prog->errors=std_vector_Vector__14_new(((u32)16));
  prog->c_includes=std_vector_Vector__7_new(((u32)16));
  prog->c_flags=std_vector_Vector__7_new(((u32)16));
  prog->c_embeds=std_map_Map__3_new();
  prog->sources=std_map_Map__3_new();
  return prog;
}

void ast_program_Program_exit_with_errors(ast_program_Program *this) {
  errors_display_error_messages(this->errors, this->error_level);
  exit(1);
}

char *ast_program_Program_get_source_text(ast_program_Program *this, std_span_Span span) {
  std_span_Location start = span.start;
  std_span_Location end = span.end;
  char *contents = ((char *)std_map_Map__3_get(this->sources, start.filename, NULL));
  if (!((bool)contents)) 
  return NULL;
  
  u32 len = (end.index - start.index);
  return str_substring(contents, start.index, len);
}

errors_Error *ast_program_Program_error(ast_program_Program *this, errors_Error *err) {
  std_vector_Vector__14_push(this->errors, err);
  return err;
}

ast_nodes_Function *ast_program_Program_get_function_deep_copy(ast_program_Program *this, ast_nodes_Function *old, ast_program_Namespace *ns) {
  char *func_text = ast_program_Program_get_source_text(this, old->span);
  if (!((bool)func_text)) 
  return NULL;
  
  lexer_Lexer lexer = lexer_Lexer_make(func_text, NULL);
  lexer.loc=old->span.start;
  std_vector_Vector__6 *tokens = lexer_Lexer_lex(&lexer);
  parser_Parser parser = parser_Parser_make(this, ns);
  parser.tokens=tokens;
  parser.curr=((u32)0);
  ast_nodes_Function *func = parser_Parser_parse_function(&parser);
  return func;
}

ast_nodes_Structure *ast_program_Program_get_struct_deep_copy(ast_program_Program *this, ast_nodes_Structure *old, ast_program_Namespace *ns) {
  char *text = ast_program_Program_get_source_text(this, old->span);
  if (!((bool)text)) 
  return NULL;
  
  lexer_Lexer lexer = lexer_Lexer_make(text, NULL);
  lexer.loc=old->span.start;
  std_vector_Vector__6 *tokens = lexer_Lexer_lex(&lexer);
  parser_Parser parser = parser_Parser_make(this, ns);
  parser.tokens=tokens;
  parser.curr=((u32)0);
  ast_nodes_Structure *struc = parser_Parser_parse_struct(&parser);
  return struc;
}

types_Type *ast_program_Program_get_base_type(ast_program_Program *this, types_BaseType base, std_span_Span span) {
  ast_scopes_Symbol *sym = ast_scopes_Scope_lookup_local(this->global->scope, types_BaseType_str(base));
  if ((((bool)sym) && sym->type==ast_scopes_SymbolType_TypeDef)) {
    return sym->u.type_def;
  }  else {
    ast_program_Program_error(this, errors_Error_new(span, format_string("Internal compiler error, couldn't find base type %s", types_BaseType_dbg(base))));
    return NULL;
  } 
}

types_Type *ast_program_Program_get_type_by_name(ast_program_Program *this, char *name, std_span_Span span) {
  ast_scopes_Symbol *sym = ast_scopes_Scope_lookup_local(this->global->scope, name);
  if ((((bool)sym) && sym->type==ast_scopes_SymbolType_TypeDef)) {
    return sym->u.type_def;
  }  else {
    ast_program_Program_error(this, errors_Error_new(span, format_string("Internal compiler error, couldn't find type %s", name)));
    return NULL;
  } 
}

ast_program_NSIterator ast_program_Program_iter_namespaces(ast_program_Program *this) {
  ast_program_NSIterator iter = (ast_program_NSIterator){.stack=std_vector_Vector__9_new(((u32)16)), .curr=NULL};
  std_vector_Vector__9_push(iter.stack, this->global);
  ast_program_NSIterator_next(&iter);
  return iter;
}

bool ast_program_NSIterator_has_value(ast_program_NSIterator *this) {
  return (((bool)this->stack) && !std_vector_Vector__9_is_empty(this->stack));
}

void ast_program_NSIterator_next(ast_program_NSIterator *this) {
  ae_assert(!std_vector_Vector__9_is_empty(this->stack), "compiler/ast/program.oc:238:12: Assertion failed: `not .stack.is_empty()`", NULL);
  this->curr=std_vector_Vector__9_pop(this->stack);
  for (std_map_ValueIterator__2 __iter = std_map_Map__2_iter_values(this->curr->namespaces); std_map_ValueIterator__2_has_value(&__iter); std_map_ValueIterator__2_next(&__iter)) {
    ast_program_Namespace *ns = std_map_ValueIterator__2_cur(&__iter);
    {
      std_vector_Vector__9_push(this->stack, ns);
    }
  }
  if (std_vector_Vector__9_is_empty(this->stack)) {
    this->curr=NULL;
    std_vector_Vector__9_free(this->stack);
    this->stack=NULL;
  } 
}

ast_program_Namespace *ast_program_NSIterator_cur(ast_program_NSIterator *this) {
  return this->curr;
}

ast_nodes_ASTType ast_nodes_ASTType_from_token(tokens_TokenType type) {
  return ({ ast_nodes_ASTType __yield_0;
    switch (type) {
      case tokens_TokenType_Ampersand: {
        __yield_0 = ast_nodes_ASTType_BitwiseAnd;
      } break;
      case tokens_TokenType_And: {
        __yield_0 = ast_nodes_ASTType_And;
      } break;
      case tokens_TokenType_Caret: {
        __yield_0 = ast_nodes_ASTType_BitwiseXor;
      } break;
      case tokens_TokenType_EqualEquals: {
        __yield_0 = ast_nodes_ASTType_Equals;
      } break;
      case tokens_TokenType_Equals: {
        __yield_0 = ast_nodes_ASTType_Assignment;
      } break;
      case tokens_TokenType_GreaterThan: {
        __yield_0 = ast_nodes_ASTType_GreaterThan;
      } break;
      case tokens_TokenType_GreaterThanEquals: {
        __yield_0 = ast_nodes_ASTType_GreaterThanEquals;
      } break;
      case tokens_TokenType_LessThan: {
        __yield_0 = ast_nodes_ASTType_LessThan;
      } break;
      case tokens_TokenType_LessThanEquals: {
        __yield_0 = ast_nodes_ASTType_LessThanEquals;
      } break;
      case tokens_TokenType_Line: {
        __yield_0 = ast_nodes_ASTType_BitwiseOr;
      } break;
      case tokens_TokenType_Minus: {
        __yield_0 = ast_nodes_ASTType_Minus;
      } break;
      case tokens_TokenType_MinusEquals: {
        __yield_0 = ast_nodes_ASTType_MinusEquals;
      } break;
      case tokens_TokenType_NotEquals: {
        __yield_0 = ast_nodes_ASTType_NotEquals;
      } break;
      case tokens_TokenType_Or: {
        __yield_0 = ast_nodes_ASTType_Or;
      } break;
      case tokens_TokenType_Percent: {
        __yield_0 = ast_nodes_ASTType_Modulus;
      } break;
      case tokens_TokenType_Plus: {
        __yield_0 = ast_nodes_ASTType_Plus;
      } break;
      case tokens_TokenType_PlusEquals: {
        __yield_0 = ast_nodes_ASTType_PlusEquals;
      } break;
      case tokens_TokenType_Slash: {
        __yield_0 = ast_nodes_ASTType_Divide;
      } break;
      case tokens_TokenType_SlashEquals: {
        __yield_0 = ast_nodes_ASTType_DivideEquals;
      } break;
      case tokens_TokenType_Star: {
        __yield_0 = ast_nodes_ASTType_Multiply;
      } break;
      case tokens_TokenType_StarEquals: {
        __yield_0 = ast_nodes_ASTType_MultiplyEquals;
      } break;
      case tokens_TokenType_Tilde: {
        __yield_0 = ast_nodes_ASTType_BitwiseNot;
      } break;
      default: std_panic(format_string("Unhandled token type in ASTType::from_token: %s", tokens_TokenType_str(type))); break;
    }
;__yield_0; });
}

ast_nodes_Variable *ast_nodes_Variable_new(types_Type *type) {
  ast_nodes_Variable *var = ((ast_nodes_Variable *)calloc(((u32)1), ((u32)sizeof(ast_nodes_Variable))));
  var->type=type;
  return var;
}

ast_nodes_TemplateInstance *ast_nodes_TemplateInstance_new(std_vector_Vector__3 *params, ast_scopes_Symbol *parent, ast_scopes_Symbol *resolved) {
  ast_nodes_TemplateInstance *instance = ((ast_nodes_TemplateInstance *)calloc(((u32)1), ((u32)sizeof(ast_nodes_TemplateInstance))));
  instance->params=params;
  instance->parent=parent;
  instance->resolved=resolved;
  return instance;
}

ast_nodes_Structure *ast_nodes_Structure_new(void) {
  ast_nodes_Structure *struc = ((ast_nodes_Structure *)calloc(((u32)1), ((u32)sizeof(ast_nodes_Structure))));
  struc->fields=std_vector_Vector__4_new(((u32)16));
  return struc;
}

ast_nodes_Variable *ast_nodes_Structure_get_field(ast_nodes_Structure *this, char *name) {
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(this->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      if (str_eq(field->sym->name, name)) {
        return field;
      } 
    }
  }
  return NULL;
}

ast_scopes_Symbol *ast_nodes_Structure_find_template_instance(ast_nodes_Structure *this, std_vector_Vector__3 *template_args) {
  std_vector_Vector__3 *a = template_args;
  for (std_vector_Iterator__15 __iter = std_vector_Vector__15_iter(this->template_instances); std_vector_Iterator__15_has_value(&__iter); std_vector_Iterator__15_next(&__iter)) {
    ast_nodes_TemplateInstance *instance = std_vector_Iterator__15_cur(&__iter);
    {
      std_vector_Vector__3 *b = instance->params;
      if ((a->size != b->size)) 
      continue;
      
      bool all_ok = true;
      for (u32 j = ((u32)0); (j < a->size); j+=((u32)1)) {
        types_Type *a_type = std_vector_Vector__3_at(a, j);
        types_Type *b_type = std_vector_Vector__3_at(b, j);
        if (!types_Type_eq(a_type, b_type)) {
          all_ok=false;
          break;
        } 
      }
      if (all_ok) 
      return instance->resolved;
      
    }
  }
  return NULL;
}

ast_nodes_Enum *ast_nodes_Enum_new(void) {
  ast_nodes_Enum *enum_ = ((ast_nodes_Enum *)calloc(((u32)1), ((u32)sizeof(ast_nodes_Enum))));
  enum_->fields=std_vector_Vector__4_new(((u32)16));
  return enum_;
}

ast_nodes_Variable *ast_nodes_Enum_get_field(ast_nodes_Enum *this, char *name) {
  for (std_vector_Iterator__4 __iter = std_vector_Vector__4_iter(this->fields); std_vector_Iterator__4_has_value(&__iter); std_vector_Iterator__4_next(&__iter)) {
    ast_nodes_Variable *field = std_vector_Iterator__4_cur(&__iter);
    {
      if (str_eq(field->sym->name, name)) 
      return field;
      
    }
  }
  return NULL;
}

ast_nodes_Function *ast_nodes_Function_new(void) {
  ast_nodes_Function *func = ((ast_nodes_Function *)calloc(((u32)1), ((u32)sizeof(ast_nodes_Function))));
  func->params=std_vector_Vector__4_new(((u32)16));
  return func;
}

ast_nodes_Argument *ast_nodes_Argument_new(char *label, std_span_Span label_span, ast_nodes_AST *expr) {
  ast_nodes_Argument *arg = ((ast_nodes_Argument *)calloc(((u32)1), ((u32)sizeof(ast_nodes_Argument))));
  arg->expr=expr;
  arg->label=label;
  arg->label_span=label_span;
  return arg;
}

ast_nodes_ImportPart *ast_nodes_ImportPart_new(ast_nodes_ImportPartType type, std_span_Span span) {
  ast_nodes_ImportPart *part = ((ast_nodes_ImportPart *)calloc(((u32)1), ((u32)sizeof(ast_nodes_ImportPart))));
  part->type=type;
  part->span=span;
  return part;
}

ast_nodes_MatchCase *ast_nodes_MatchCase_new(ast_nodes_AST *cond, ast_nodes_AST *body) {
  ast_nodes_MatchCase *_case = ((ast_nodes_MatchCase *)calloc(((u32)1), ((u32)sizeof(ast_nodes_MatchCase))));
  _case->cond=cond;
  _case->body=body;
  return _case;
}

ast_nodes_AST *ast_nodes_AST_new(ast_nodes_ASTType type, std_span_Span span) {
  ast_nodes_AST *ast = ((ast_nodes_AST *)calloc(((u32)1), ((u32)sizeof(ast_nodes_AST))));
  ast->type=type;
  ast->span=span;
  return ast;
}

ast_nodes_AST *ast_nodes_AST_new_unop(ast_nodes_ASTType type, std_span_Span span, ast_nodes_AST *expr) {
  ast_nodes_AST *ast = ast_nodes_AST_new(type, span);
  ast->u.unary=expr;
  return ast;
}

ast_nodes_AST *ast_nodes_AST_new_binop(ast_nodes_ASTType type, ast_nodes_AST *lhs, ast_nodes_AST *rhs) {
  std_span_Span span = std_span_Span_join(lhs->span, rhs->span);
  ast_nodes_AST *ast = ast_nodes_AST_new(type, span);
  ast->u.binary.lhs=lhs;
  ast->u.binary.rhs=rhs;
  return ast;
}

bool ast_nodes_AST_is_identifier(ast_nodes_AST *this) {
  return ({ bool __yield_0;
    switch (this->type) {
      case ast_nodes_ASTType_Identifier: {
        __yield_0 = true;
      } break;
      case ast_nodes_ASTType_NSLookup: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

ast_scopes_Symbol *ast_nodes_AST_symbol(ast_nodes_AST *this) {
  return this->resolved_symbol;
}

bool ast_nodes_AST_is_lvalue(ast_nodes_AST *this) {
  return ({ bool __yield_0;
    switch (this->type) {
      case ast_nodes_ASTType_Identifier: {
        __yield_0 = !this->u.ident.is_function;
      } break;
      case ast_nodes_ASTType_Member: {
        __yield_0 = true;
      } break;
      case ast_nodes_ASTType_Dereference: {
        __yield_0 = true;
      } break;
      case ast_nodes_ASTType_Index: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

bool lexer_is_hex_digit(char c) {
  if (char_is_digit(c)) 
  return true;
  
  if ((('a' <= c) && (c <= 'f'))) 
  return true;
  
  if ((('A' <= c) && (c <= 'F'))) 
  return true;
  
  return false;
}

lexer_Lexer lexer_Lexer_make(char *source, char *filename) {
  std_span_Location start_loc = (std_span_Location){.filename=filename, .line=((u32)1), .col=((u32)1), .index=((u32)0)};
  return (lexer_Lexer){.source=source, .source_len=str_len(source), .i=((u32)0), .loc=start_loc, .seen_newline=false, .tokens=std_vector_Vector__6_new(((u32)16)), .errors=std_vector_Vector__14_new(((u32)16)), .in_comment=false, .comment=std_buffer_Buffer_make(((u32)16)), .comment_start=start_loc};
}

void lexer_Lexer_push(lexer_Lexer *this, tokens_Token *token) {
  token->seen_newline=this->seen_newline;
  if ((this->comment.size > ((u32)0))) {
    token->comment=std_buffer_Buffer_new_str(&this->comment);
    token->comment_loc=this->comment_start;
  } 
  std_buffer_Buffer_clear(&this->comment);
  std_vector_Vector__6_push(this->tokens, token);
  this->seen_newline=false;
  this->in_comment=false;
}

void lexer_Lexer_push_type(lexer_Lexer *this, tokens_TokenType type, u32 len) {
  std_span_Location start_loc = this->loc;
  for (u32 i = ((u32)0); (i < len); i+=((u32)1)) {
    lexer_Lexer_inc(this);
  }
  lexer_Lexer_push(this, tokens_Token_from_type(type, (std_span_Span){.start=start_loc, .end=this->loc}));
}

char lexer_Lexer_cur(lexer_Lexer *this) {
  return this->source[this->i];
}

void lexer_Lexer_inc(lexer_Lexer *this) {
  switch (lexer_Lexer_cur(this)) {
    case '\n': {
      this->loc.line+=((u32)1);
      this->loc.col=((u32)1);
      this->seen_newline=true;
    } break;
    default: {
      this->loc.col+=((u32)1);
    } break;
  }
  this->i+=((u32)1);
  this->loc.index+=((u32)1);
}

char lexer_Lexer_peek(lexer_Lexer *this, i32 offset) {
  if (lexer_Lexer_cur(this)=='\0') {
    return lexer_Lexer_cur(this);
  } 
  return this->source[(this->i + ((u32)1))];
}

void lexer_Lexer_lex_char_literal(lexer_Lexer *this) {
  std_span_Location start_loc = this->loc;
  u32 start = (this->i + ((u32)1));
  lexer_Lexer_inc(this);
  if (lexer_Lexer_cur(this)=='\\') {
    lexer_Lexer_inc(this);
  } 
  lexer_Lexer_inc(this);
  if ((lexer_Lexer_cur(this) != '\'')) {
    std_vector_Vector__14_push(this->errors, errors_Error_new((std_span_Span){.start=this->loc, .end=this->loc}, "Expected ' after character literal"));
  } 
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  lexer_Lexer_inc(this);
  lexer_Lexer_push(this, tokens_Token_new(tokens_TokenType_CharLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text));
}

void lexer_Lexer_lex_string_literal(lexer_Lexer *this, bool has_seen_f) {
  std_span_Location start_loc = this->loc;
  char end_char = lexer_Lexer_cur(this);
  u32 start = (this->i + ((u32)1));
  lexer_Lexer_inc(this);
  while (((this->i < this->source_len) && (lexer_Lexer_cur(this) != end_char))) {
    if (lexer_Lexer_cur(this)=='\\') {
      lexer_Lexer_inc(this);
    } 
    lexer_Lexer_inc(this);
  }
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  lexer_Lexer_inc(this);
  if ((this->i >= this->source_len)) {
    std_vector_Vector__14_push(this->errors, errors_Error_new((std_span_Span){.start=this->loc, .end=this->loc}, "Unterminated string literal"));
  } 
  if ((end_char=='`' || has_seen_f)) {
    lexer_Lexer_push(this, tokens_Token_new(tokens_TokenType_FormatStringLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text));
  }  else {
    lexer_Lexer_push(this, tokens_Token_new(tokens_TokenType_StringLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text));
  } 
}

tokens_Token *lexer_Lexer_lex_int_literal_different_base(lexer_Lexer *this) {
  std_span_Location start_loc = this->loc;
  u32 start = this->i;
  lexer_Lexer_inc(this);
  switch (lexer_Lexer_cur(this)) {
    case 'x': {
      lexer_Lexer_inc(this);
      while (((this->i < this->source_len) && lexer_is_hex_digit(lexer_Lexer_cur(this)))) {
        lexer_Lexer_inc(this);
      }
    } break;
    case 'b': {
      lexer_Lexer_inc(this);
      while ((((this->i < this->source_len) && lexer_Lexer_cur(this)=='0') || lexer_Lexer_cur(this)=='1')) {
        lexer_Lexer_inc(this);
      }
    } break;
    default: {
      ae_assert(false, "compiler/lexer.oc:158:24: Assertion failed: `false`", "Invalid base for int literal");
    } break;
  }
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  return tokens_Token_new(tokens_TokenType_IntLiteral, (std_span_Span){.start=start_loc, .end=this->loc}, text);
}

tokens_Token *lexer_Lexer_lex_numeric_literal_helper(lexer_Lexer *this) {
  std_span_Location start_loc = this->loc;
  if (lexer_Lexer_cur(this)=='0') {
    switch (lexer_Lexer_peek(this, 1)) {
      case 'x':
      case 'b': {
        return lexer_Lexer_lex_int_literal_different_base(this);
      } break;
      default: {
      } break;
    }
  } 
  u32 start = this->i;
  tokens_TokenType token_type;
  while (char_is_digit(lexer_Lexer_cur(this))) {
    lexer_Lexer_inc(this);
  }
  if (lexer_Lexer_cur(this)=='.') {
    lexer_Lexer_inc(this);
    while (char_is_digit(lexer_Lexer_cur(this))) {
      lexer_Lexer_inc(this);
    }
    token_type=tokens_TokenType_FloatLiteral;
  }  else {
    token_type=tokens_TokenType_IntLiteral;
  } 
  u32 len = (this->i - start);
  char *text = str_substring(this->source, start, len);
  return tokens_Token_new(token_type, (std_span_Span){.start=start_loc, .end=this->loc}, text);
}

void lexer_Lexer_lex_numeric_literal(lexer_Lexer *this) {
  tokens_Token *token = lexer_Lexer_lex_numeric_literal_helper(this);
  if (((lexer_Lexer_cur(this)=='u' || lexer_Lexer_cur(this)=='i') || lexer_Lexer_cur(this)=='f')) {
    std_span_Location start_loc = this->loc;
    u32 start = this->i;
    lexer_Lexer_inc(this);
    while (((this->i < this->source_len) && char_is_digit(lexer_Lexer_cur(this)))) {
      lexer_Lexer_inc(this);
    }
    u32 len = (this->i - start);
    char *suffix = str_substring(this->source, start, len);
    token->suffix=tokens_Token_from_ident(suffix, (std_span_Span){.start=start_loc, .end=this->loc});
  } 
  lexer_Lexer_push(this, token);
}

void lexer_Lexer_lex_comment(lexer_Lexer *this) {
  while (lexer_Lexer_cur(this)=='/') {
    lexer_Lexer_inc(this);
  }
  bool save_comment = false;
  if ((lexer_Lexer_cur(this)=='*' || lexer_Lexer_cur(this)=='.')) {
    lexer_Lexer_inc(this);
    save_comment=true;
  } 
  if ((lexer_Lexer_cur(this)==' ' || lexer_Lexer_cur(this)=='\t')) {
    lexer_Lexer_inc(this);
  } 
  while (((this->i < this->source_len) && (lexer_Lexer_cur(this) != '\n'))) {
    if (save_comment) 
    std_buffer_Buffer_putc(&this->comment, lexer_Lexer_cur(this));
    
    lexer_Lexer_inc(this);
  }
  if (save_comment) 
  std_buffer_Buffer_putc(&this->comment, '\n');
  
}

std_vector_Vector__6 *lexer_Lexer_lex(lexer_Lexer *this) {
  while ((this->i < this->source_len)) {
    char c = lexer_Lexer_cur(this);
    switch (c) {
      case ' ':
      case '\t':
      case '\v':
      case '\r':
      case '\b':
      case '\n': {
        lexer_Lexer_inc(this);
      } break;
      case ';': {
        lexer_Lexer_push_type(this, tokens_TokenType_Semicolon, ((u32)1));
      } break;
      case ',': {
        lexer_Lexer_push_type(this, tokens_TokenType_Comma, ((u32)1));
      } break;
      case '.': {
        lexer_Lexer_push_type(this, tokens_TokenType_Dot, ((u32)1));
      } break;
      case '(': {
        lexer_Lexer_push_type(this, tokens_TokenType_OpenParen, ((u32)1));
      } break;
      case ')': {
        lexer_Lexer_push_type(this, tokens_TokenType_CloseParen, ((u32)1));
      } break;
      case '[': {
        lexer_Lexer_push_type(this, tokens_TokenType_OpenSquare, ((u32)1));
      } break;
      case ']': {
        lexer_Lexer_push_type(this, tokens_TokenType_CloseSquare, ((u32)1));
      } break;
      case '{': {
        lexer_Lexer_push_type(this, tokens_TokenType_OpenCurly, ((u32)1));
      } break;
      case '}': {
        lexer_Lexer_push_type(this, tokens_TokenType_CloseCurly, ((u32)1));
      } break;
      case '@': {
        lexer_Lexer_push_type(this, tokens_TokenType_AtSign, ((u32)1));
      } break;
      case '%': {
        lexer_Lexer_push_type(this, tokens_TokenType_Percent, ((u32)1));
      } break;
      case '^': {
        lexer_Lexer_push_type(this, tokens_TokenType_Caret, ((u32)1));
      } break;
      case '&': {
        lexer_Lexer_push_type(this, tokens_TokenType_Ampersand, ((u32)1));
      } break;
      case '|': {
        lexer_Lexer_push_type(this, tokens_TokenType_Line, ((u32)1));
      } break;
      case '?': {
        lexer_Lexer_push_type(this, tokens_TokenType_Question, ((u32)1));
      } break;
      case '~': {
        lexer_Lexer_push_type(this, tokens_TokenType_Tilde, ((u32)1));
      } break;
      case '!': {
        switch (lexer_Lexer_peek(this, 1)) {
          case '=': {
            lexer_Lexer_push_type(this, tokens_TokenType_NotEquals, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_Exclamation, ((u32)1));
          } break;
        }
      } break;
      case ':': {
        switch (lexer_Lexer_peek(this, 1)) {
          case ':': {
            lexer_Lexer_push_type(this, tokens_TokenType_ColonColon, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_Colon, ((u32)1));
          } break;
        }
      } break;
      case '=': {
        switch (lexer_Lexer_peek(this, 1)) {
          case '=': {
            lexer_Lexer_push_type(this, tokens_TokenType_EqualEquals, ((u32)2));
          } break;
          case '>': {
            lexer_Lexer_push_type(this, tokens_TokenType_FatArrow, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_Equals, ((u32)1));
          } break;
        }
      } break;
      case '*': {
        switch (lexer_Lexer_peek(this, 1)) {
          case '=': {
            lexer_Lexer_push_type(this, tokens_TokenType_StarEquals, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_Star, ((u32)1));
          } break;
        }
      } break;
      case '+': {
        switch (lexer_Lexer_peek(this, 1)) {
          case '=': {
            lexer_Lexer_push_type(this, tokens_TokenType_PlusEquals, ((u32)2));
          } break;
          case '+': {
            lexer_Lexer_push_type(this, tokens_TokenType_PlusPlus, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_Plus, ((u32)1));
          } break;
        }
      } break;
      case '-': {
        switch (lexer_Lexer_peek(this, 1)) {
          case '=': {
            lexer_Lexer_push_type(this, tokens_TokenType_MinusEquals, ((u32)2));
          } break;
          case '-': {
            lexer_Lexer_push_type(this, tokens_TokenType_MinusMinus, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_Minus, ((u32)1));
          } break;
        }
      } break;
      case '<': {
        switch (lexer_Lexer_peek(this, 1)) {
          case '=': {
            lexer_Lexer_push_type(this, tokens_TokenType_LessThanEquals, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_LessThan, ((u32)1));
          } break;
        }
      } break;
      case '>': {
        switch (lexer_Lexer_peek(this, 1)) {
          case '=': {
            lexer_Lexer_push_type(this, tokens_TokenType_GreaterThanEquals, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_GreaterThan, ((u32)1));
          } break;
        }
      } break;
      case '/': {
        switch (lexer_Lexer_peek(this, 1)) {
          case '/': {
            lexer_Lexer_lex_comment(this);
          } break;
          case '=': {
            lexer_Lexer_push_type(this, tokens_TokenType_SlashEquals, ((u32)2));
          } break;
          default: {
            lexer_Lexer_push_type(this, tokens_TokenType_Slash, ((u32)1));
          } break;
        }
      } break;
      case '\'': {
        lexer_Lexer_lex_char_literal(this);
      } break;
      case '"':
      case '`': {
        lexer_Lexer_lex_string_literal(this, false);
      } break;
      default: {
        std_span_Location start_loc = this->loc;
        if ((c=='f' && lexer_Lexer_peek(this, 1)=='"')) {
          lexer_Lexer_inc(this);
          lexer_Lexer_lex_string_literal(this, true);
        }  else         if (char_is_digit(c)) {
          lexer_Lexer_lex_numeric_literal(this);
        }  else         if ((char_is_alpha(c) || c=='_')) {
          u32 start = this->i;
          while ((char_is_alnum(lexer_Lexer_cur(this)) || lexer_Lexer_cur(this)=='_')) {
            lexer_Lexer_inc(this);
          }
          u32 len = (this->i - start);
          char *text = str_substring(this->source, start, len);
          lexer_Lexer_push(this, tokens_Token_from_ident(text, (std_span_Span){.start=start_loc, .end=this->loc}));
        }  else {
          std_vector_Vector__14_push(this->errors, errors_Error_new((std_span_Span){.start=this->loc, .end=this->loc}, format_string("Unrecognized char in lexer: '%c'", c)));
          lexer_Lexer_inc(this);
        } 
        
        
      } break;
    }
  }
  this->seen_newline=true;
  lexer_Lexer_push_type(this, tokens_TokenType_EOF, ((u32)0));
  return this->tokens;
}

tokens_Token *tokens_Token_new(tokens_TokenType type, std_span_Span span, char *text) {
  tokens_Token *tok = ((tokens_Token *)calloc(((u32)1), ((u32)sizeof(tokens_Token))));
  *tok=(tokens_Token){.type=type, .span=span, .text=text, .suffix=NULL, .seen_newline=false, .comment=NULL, .comment_loc=std_span_Location_default()};
  return tok;
}

tokens_Token *tokens_Token_from_type(tokens_TokenType type, std_span_Span span) {
  return tokens_Token_new(type, span, "");
}

tokens_Token *tokens_Token_from_ident(char *text, std_span_Span span) {
  tokens_TokenType type = tokens_TokenType_from_text(text);
  return tokens_Token_new(type, span, text);
}

char *tokens_Token_str(tokens_Token *this) {
  return format_string("%s: %s", std_span_Span_str(this->span), tokens_TokenType_str(this->type));
}

bool tokens_Token_is_word(tokens_Token this) {
  return ({ bool __yield_0;
    switch (this.type) {
      case tokens_TokenType_Identifier: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = (((u64)this.type) > ((u64)tokens_TokenType_BEGIN_KEYWORDS));
      } break;
    }
;__yield_0; });
}

tokens_TokenType tokens_TokenType_from_text(char *text) {
  return ({ tokens_TokenType __yield_0;
    {
      char *__match_str = text;
      if (!strcmp(__match_str, "and")) {
        __yield_0 = tokens_TokenType_And;
      } else if (!strcmp(__match_str, "as")) {
        __yield_0 = tokens_TokenType_As;
      } else if (!strcmp(__match_str, "assert")) {
        __yield_0 = tokens_TokenType_Assert;
      } else if (!strcmp(__match_str, "break")) {
        __yield_0 = tokens_TokenType_Break;
      } else if (!strcmp(__match_str, "const")) {
        __yield_0 = tokens_TokenType_Const;
      } else if (!strcmp(__match_str, "continue")) {
        __yield_0 = tokens_TokenType_Continue;
      } else if (!strcmp(__match_str, "def")) {
        __yield_0 = tokens_TokenType_Def;
      } else if (!strcmp(__match_str, "defer")) {
        __yield_0 = tokens_TokenType_Defer;
      } else if (!strcmp(__match_str, "else")) {
        __yield_0 = tokens_TokenType_Else;
      } else if (!strcmp(__match_str, "enum")) {
        __yield_0 = tokens_TokenType_Enum;
      } else if (!strcmp(__match_str, "extern")) {
        __yield_0 = tokens_TokenType_Extern;
      } else if (!strcmp(__match_str, "false")) {
        __yield_0 = tokens_TokenType_False;
      } else if (!strcmp(__match_str, "for")) {
        __yield_0 = tokens_TokenType_For;
      } else if (!strcmp(__match_str, "fn")) {
        __yield_0 = tokens_TokenType_Fn;
      } else if (!strcmp(__match_str, "if")) {
        __yield_0 = tokens_TokenType_If;
      } else if (!strcmp(__match_str, "let")) {
        __yield_0 = tokens_TokenType_Let;
      } else if (!strcmp(__match_str, "match")) {
        __yield_0 = tokens_TokenType_Match;
      } else if (!strcmp(__match_str, "namespace")) {
        __yield_0 = tokens_TokenType_Namespace;
      } else if (!strcmp(__match_str, "not")) {
        __yield_0 = tokens_TokenType_Not;
      } else if (!strcmp(__match_str, "null")) {
        __yield_0 = tokens_TokenType_Null;
      } else if (!strcmp(__match_str, "or")) {
        __yield_0 = tokens_TokenType_Or;
      } else if (!strcmp(__match_str, "return")) {
        __yield_0 = tokens_TokenType_Return;
      } else if (!strcmp(__match_str, "sizeof")) {
        __yield_0 = tokens_TokenType_SizeOf;
      } else if (!strcmp(__match_str, "struct")) {
        __yield_0 = tokens_TokenType_Struct;
      } else if (!strcmp(__match_str, "true")) {
        __yield_0 = tokens_TokenType_True;
      } else if (!strcmp(__match_str, "then")) {
        __yield_0 = tokens_TokenType_Then;
      } else if (!strcmp(__match_str, "union")) {
        __yield_0 = tokens_TokenType_Union;
      } else if (!strcmp(__match_str, "import")) {
        __yield_0 = tokens_TokenType_Import;
      } else if (!strcmp(__match_str, "void")) {
        __yield_0 = tokens_TokenType_Void;
      } else if (!strcmp(__match_str, "yield")) {
        __yield_0 = tokens_TokenType_Yield;
      } else if (!strcmp(__match_str, "while")) {
        __yield_0 = tokens_TokenType_While;
      } else  {
        __yield_0 = tokens_TokenType_Identifier;
      }
    }
;__yield_0; });
}

char *tokens_TokenType_str(tokens_TokenType this) {
  return ({ char *__yield_0;
    switch (this) {
      case tokens_TokenType_And: {
        __yield_0 = "and";
      } break;
      case tokens_TokenType_As: {
        __yield_0 = "as";
      } break;
      case tokens_TokenType_Assert: {
        __yield_0 = "assert";
      } break;
      case tokens_TokenType_Break: {
        __yield_0 = "break";
      } break;
      case tokens_TokenType_Const: {
        __yield_0 = "const";
      } break;
      case tokens_TokenType_Continue: {
        __yield_0 = "continue";
      } break;
      case tokens_TokenType_Def: {
        __yield_0 = "def";
      } break;
      case tokens_TokenType_Defer: {
        __yield_0 = "defer";
      } break;
      case tokens_TokenType_Else: {
        __yield_0 = "else";
      } break;
      case tokens_TokenType_Enum: {
        __yield_0 = "enum";
      } break;
      case tokens_TokenType_Extern: {
        __yield_0 = "extern";
      } break;
      case tokens_TokenType_False: {
        __yield_0 = "false";
      } break;
      case tokens_TokenType_For: {
        __yield_0 = "for";
      } break;
      case tokens_TokenType_Fn: {
        __yield_0 = "fn";
      } break;
      case tokens_TokenType_If: {
        __yield_0 = "if";
      } break;
      case tokens_TokenType_Let: {
        __yield_0 = "let";
      } break;
      case tokens_TokenType_Match: {
        __yield_0 = "match";
      } break;
      case tokens_TokenType_Namespace: {
        __yield_0 = "namespace";
      } break;
      case tokens_TokenType_Not: {
        __yield_0 = "not";
      } break;
      case tokens_TokenType_Null: {
        __yield_0 = "null";
      } break;
      case tokens_TokenType_Or: {
        __yield_0 = "or";
      } break;
      case tokens_TokenType_Return: {
        __yield_0 = "return";
      } break;
      case tokens_TokenType_SizeOf: {
        __yield_0 = "sizeof";
      } break;
      case tokens_TokenType_Struct: {
        __yield_0 = "struct";
      } break;
      case tokens_TokenType_True: {
        __yield_0 = "true";
      } break;
      case tokens_TokenType_Then: {
        __yield_0 = "then";
      } break;
      case tokens_TokenType_Union: {
        __yield_0 = "union";
      } break;
      case tokens_TokenType_Import: {
        __yield_0 = "import";
      } break;
      case tokens_TokenType_Void: {
        __yield_0 = "void";
      } break;
      case tokens_TokenType_Yield: {
        __yield_0 = "yield";
      } break;
      case tokens_TokenType_While: {
        __yield_0 = "while";
      } break;
      default: {
        __yield_0 = tokens_TokenType_dbg(this);
      } break;
    }
;__yield_0; });
}

char *errors_ErrorType_str(errors_ErrorType this) {
  return ({ char *__yield_0;
    switch (this) {
      case errors_ErrorType_Standard: {
        __yield_0 = "Standard";
      } break;
      case errors_ErrorType_WithNote: {
        __yield_0 = "WithNote";
      } break;
      case errors_ErrorType_WithHint: {
        __yield_0 = "WithHint";
      } break;
    }
;__yield_0; });
}

char *errors_MessageType_to_color(errors_MessageType this) {
  return ({ char *__yield_0;
    switch (this) {
      case errors_MessageType_Error: {
        __yield_0 = "\x1b[31m";
      } break;
      case errors_MessageType_Warning: {
        __yield_0 = "\x1b[33m";
      } break;
      case errors_MessageType_Note: {
        __yield_0 = "\x1b[32m";
      } break;
    }
;__yield_0; });
}

char *errors_MessageType_str(errors_MessageType this) {
  return ({ char *__yield_0;
    switch (this) {
      case errors_MessageType_Error: {
        __yield_0 = "Error";
      } break;
      case errors_MessageType_Warning: {
        __yield_0 = "Warning";
      } break;
      case errors_MessageType_Note: {
        __yield_0 = "Note";
      } break;
    }
;__yield_0; });
}

void errors_display_line(void) {
  printf("--------------------------------------------------------------------------------""\n");
}

void errors_display_message(errors_MessageType type, std_span_Span span, char *msg) {
  errors_display_line();
  printf("%s: %s: %s""\n", std_span_Location_str(&span.start), errors_MessageType_str(type), msg);
  errors_display_line();
}

void errors_display_message_span(errors_MessageType type, std_span_Span span, char *msg) {
  char *color = errors_MessageType_to_color(type);
  char *reset = "\x1b[0m";
  errors_display_message(type, span, msg);
  char *filename = span.start.filename;
  if (!FILE_exists(filename)) 
  return ;
  
  FILE *file = FILE_open(filename, "r");
  char *contents = FILE_slurp(file);
  u32 around_offset = ((u32)1);
  u32 min_line = u32_max((span.start.line - around_offset), ((u32)1));
  u32 max_line = (span.end.line + around_offset);
  char *lines = contents;
  char *cur = strsep(&lines, "\n");
  u32 line_no = ((u32)1);
  while ((((bool)cur) && (line_no <= max_line))) {
    if (((line_no >= min_line) && (line_no <= max_line))) {
      printf("%4d | ", line_no);
      if (line_no==span.start.line) {
        u32 start_col = (span.start.col - ((u32)1));
        u32 end_col = (span.end.col - ((u32)1));
        if ((span.end.line != span.start.line)) {
          end_col=str_len(cur);
        } 
        for (u32 i = ((u32)0); (i < start_col); i+=((u32)1)) {
          printf("%c", cur[i]);
        }
        printf("%s", color);
        for (u32 i = start_col; (i < end_col); i+=((u32)1)) {
          printf("%c", cur[i]);
        }
        printf("%s%s""\n", reset, &cur[end_col]);
        printf("%*s%s^ %s%s""\n", (start_col + ((u32)7)), "", color, msg, reset);
      }  else {
        printf("%s""\n", cur);
      } 
    } 
    line_no+=((u32)1);
    cur=strsep(&lines, "\n");
  }
  /* defers */
  free(contents);
  fclose(file);
}

void errors_Error_display(errors_Error *this) {
  switch (this->type) {
    case errors_ErrorType_Standard: {
      errors_display_message_span(errors_MessageType_Error, this->span1, this->msg1);
      errors_display_line();
    } break;
    case errors_ErrorType_WithNote: {
      errors_display_message_span(errors_MessageType_Error, this->span1, this->msg1);
      errors_display_message(errors_MessageType_Note, this->span1, this->msg2);
    } break;
    case errors_ErrorType_WithHint: {
      errors_display_message_span(errors_MessageType_Error, this->span1, this->msg1);
      errors_display_message_span(errors_MessageType_Note, this->span2, this->msg2);
      errors_display_line();
    } break;
  }
}

void errors_Error_panic(errors_Error *this) {
  errors_Error_display(this);
  exit(1);
}

errors_Error *errors_Error_new(std_span_Span span, char *msg) {
  errors_Error *err = ((errors_Error *)calloc(((u32)1), ((u32)sizeof(errors_Error))));
  err->type=errors_ErrorType_Standard;
  err->msg1=msg;
  err->span1=span;
  return err;
}

errors_Error *errors_Error_new_note(std_span_Span span, char *msg, char *note) {
  errors_Error *err = ((errors_Error *)calloc(((u32)1), ((u32)sizeof(errors_Error))));
  err->type=errors_ErrorType_WithNote;
  err->msg1=msg;
  err->span1=span;
  err->msg2=note;
  return err;
}

errors_Error *errors_Error_new_hint(std_span_Span span, char *msg, std_span_Span span2, char *hint) {
  errors_Error *err = ((errors_Error *)calloc(((u32)1), ((u32)sizeof(errors_Error))));
  err->type=errors_ErrorType_WithHint;
  err->msg1=msg;
  err->span1=span;
  err->msg2=hint;
  err->span2=span2;
  return err;
}

void errors_display_error_messages(std_vector_Vector__14 *errors, u32 detail_level) {
  char *num_errors_env = getenv("OCEN_NUM_ERRORS");
  u32 max_num_errors = (((bool)num_errors_env) ? str_to_u32(num_errors_env) : ((u32)10));
  u32 num_errors = u32_min(errors->size, max_num_errors);
  bool first = true;
  for (u32 i = ((u32)0); (i < num_errors); i+=((u32)1)) {
    errors_Error *err = std_vector_Vector__14_at(errors, ((num_errors - i) - ((u32)1)));
    switch (detail_level) {
      case ((u32)0): {
        printf("%s: %s""\n", std_span_Location_str(&err->span1.start), err->msg1);
      } break;
      case ((u32)1): {
        errors_display_message_span(errors_MessageType_Error, err->span1, err->msg1);
      } break;
      case ((u32)2): {
        if (first) 
        printf("""\n");
        
        first=false;
        errors_Error_display(err);
      } break;
      default: std_panic("invalid detail level"); break;
    }
  }
}

char *types_BaseType_str(types_BaseType this) {
  return ({ char *__yield_0;
    switch (this) {
      case types_BaseType_Char: {
        __yield_0 = "char";
      } break;
      case types_BaseType_Bool: {
        __yield_0 = "bool";
      } break;
      case types_BaseType_Void: {
        __yield_0 = "void";
      } break;
      case types_BaseType_I8: {
        __yield_0 = "i8";
      } break;
      case types_BaseType_I16: {
        __yield_0 = "i16";
      } break;
      case types_BaseType_I32: {
        __yield_0 = "i32";
      } break;
      case types_BaseType_I64: {
        __yield_0 = "i64";
      } break;
      case types_BaseType_U8: {
        __yield_0 = "u8";
      } break;
      case types_BaseType_U16: {
        __yield_0 = "u16";
      } break;
      case types_BaseType_U32: {
        __yield_0 = "u32";
      } break;
      case types_BaseType_U64: {
        __yield_0 = "u64";
      } break;
      case types_BaseType_F32: {
        __yield_0 = "f32";
      } break;
      case types_BaseType_F64: {
        __yield_0 = "f64";
      } break;
      default: {
        __yield_0 = types_BaseType_dbg(this);
      } break;
    }
;__yield_0; });
}

types_FunctionType *types_FunctionType_from_func(ast_nodes_Function *func) {
  types_FunctionType *type = ((types_FunctionType *)calloc(((u32)1), ((u32)sizeof(types_FunctionType))));
  type->orig=func;
  type->return_type=func->return_type;
  type->params=func->params;
  return type;
}

types_FunctionType *types_FunctionType_from_args(std_vector_Vector__4 *params, types_Type *return_type) {
  types_FunctionType *type = ((types_FunctionType *)calloc(((u32)1), ((u32)sizeof(types_FunctionType))));
  type->return_type=return_type;
  type->params=params;
  return type;
}

types_ArrayType *types_ArrayType_new(types_Type *elem_type, ast_nodes_AST *size_expr) {
  types_ArrayType *type = ((types_ArrayType *)calloc(((u32)1), ((u32)sizeof(types_ArrayType))));
  type->elem_type=elem_type;
  type->size_expr=size_expr;
  return type;
}

types_UnresolvedTemplate *types_UnresolvedTemplate_new(types_Type *base, std_vector_Vector__3 *args) {
  types_UnresolvedTemplate *spec = ((types_UnresolvedTemplate *)calloc(((u32)1), ((u32)sizeof(types_UnresolvedTemplate))));
  spec->base=base;
  spec->args=args;
  return spec;
}

types_Type *types_Type_new_resolved(types_BaseType base, std_span_Span span) {
  types_Type *type = ((types_Type *)calloc(((u32)1), ((u32)sizeof(types_Type))));
  type->base=base;
  type->span=span;
  type->name=types_BaseType_str(base);
  type->methods=std_map_Map__4_new();
  return type;
}

types_Type *types_Type_new_unresolved(char *name, std_span_Span span) {
  types_Type *type = types_Type_new_resolved(types_BaseType_Unresolved, span);
  type->name=name;
  return type;
}

types_Type *types_Type_new_unresolved_base(types_BaseType base, std_span_Span span) {
  ast_nodes_AST *ident = ast_nodes_AST_new(ast_nodes_ASTType_Identifier, span);
  ident->u.ident.name=types_BaseType_str(base);
  types_Type *type = types_Type_new_resolved(types_BaseType_Unresolved, span);
  type->u.unresolved=ident;
  return type;
}

bool types_Type_is_integer(types_Type *this) {
  return ({ bool __yield_0;
    switch (this->base) {
      case types_BaseType_I8:
      case types_BaseType_I16:
      case types_BaseType_I32:
      case types_BaseType_I64:
      case types_BaseType_U8:
      case types_BaseType_U16:
      case types_BaseType_U32:
      case types_BaseType_U64: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

bool types_Type_is_float(types_Type *this) {
  return (this->base==types_BaseType_F32 || this->base==types_BaseType_F64);
}

bool types_Type_is_numeric(types_Type *this) {
  return ({ bool __yield_0;
    switch (this->base) {
      case types_BaseType_I8:
      case types_BaseType_I16:
      case types_BaseType_I32:
      case types_BaseType_I64:
      case types_BaseType_U8:
      case types_BaseType_U16:
      case types_BaseType_U32:
      case types_BaseType_U64:
      case types_BaseType_F32:
      case types_BaseType_F64: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

bool types_Type_is_numeric_or_char(types_Type *this) {
  return (types_Type_is_numeric(this) || this->base==types_BaseType_Char);
}

bool types_Type_can_have_methods(types_Type *this) {
  return ({ bool __yield_0;
    switch (this->base) {
      case types_BaseType_Char:
      case types_BaseType_Bool:
      case types_BaseType_Void:
      case types_BaseType_I8:
      case types_BaseType_I16:
      case types_BaseType_I32:
      case types_BaseType_I64:
      case types_BaseType_U8:
      case types_BaseType_U16:
      case types_BaseType_U32:
      case types_BaseType_U64:
      case types_BaseType_F32:
      case types_BaseType_F64:
      case types_BaseType_Structure:
      case types_BaseType_Enum:
      case types_BaseType_Alias: {
        __yield_0 = true;
      } break;
      default: {
        __yield_0 = false;
      } break;
    }
;__yield_0; });
}

bool types_Type_eq(types_Type *this, types_Type *other) {
  if ((this==NULL && other==NULL)) 
  return true;
  
  if ((this==NULL || other==NULL)) 
  return false;
  
  if (this->base==types_BaseType_Alias) 
  return types_Type_eq(this->u.ptr, other);
  
  if (other->base==types_BaseType_Alias) 
  return types_Type_eq(this, other->u.ptr);
  
  if ((this->base != other->base)) 
  return false;
  
  switch (this->base) {
    case types_BaseType_Error:
    case types_BaseType_Unresolved: {
      return false;
    } break;
    case types_BaseType_Function: {
      types_FunctionType *af = this->u.func;
      types_FunctionType *bf = other->u.func;
      if (!types_Type_eq(af->return_type, bf->return_type)) 
      return false;
      
      if ((af->params->size != bf->params->size)) 
      return false;
      
      for (u32 i = ((u32)0); (i < af->params->size); i+=((u32)1)) {
        ast_nodes_Variable *a = std_vector_Vector__4_at(af->params, i);
        ast_nodes_Variable *b = std_vector_Vector__4_at(bf->params, i);
        if (!types_Type_eq(a->type, b->type)) 
        return false;
        
      }
      return true;
    } break;
    case types_BaseType_Pointer: {
      if ((this->u.ptr->base==types_BaseType_Void || other->u.ptr->base==types_BaseType_Void)) {
        return true;
      } 
      return types_Type_eq(this->u.ptr, other->u.ptr);
    } break;
    case types_BaseType_Structure: {
      return this->u.struc==other->u.struc;
    } break;
    case types_BaseType_Enum: {
      return this->u.enum_==other->u.enum_;
    } break;
    default: {
      return true;
    } break;
  }
}

bool types_Type_is_str(types_Type *this) {
  return (this->base==types_BaseType_Alias && str_eq(this->sym->out_name, "str"));
}

types_Type *types_Type_unaliased(types_Type *this) {
  if ((this->base != types_BaseType_Alias)) 
  return this;
  
  return types_Type_unaliased(this->u.ptr);
}

types_Type *types_Type_decay_array(types_Type *this) {
  if ((this->base != types_BaseType_Array)) 
  return this;
  
  types_Type *res = types_Type_new_resolved(types_BaseType_Pointer, this->span);
  res->u.ptr=this->u.arr->elem_type;
  return res;
}

char *types_Type_str(types_Type *this) {
  return ({ char *__yield_0;
    switch (this->base) {
      case types_BaseType_Pointer: {
        __yield_0 = format_string("&%s", types_Type_str(this->u.ptr));
      } break;
      case types_BaseType_Function: {
        __yield_0 = "<function>";
      } break;
      case types_BaseType_Structure: {
        __yield_0 = this->u.struc->sym->display;
      } break;
      case types_BaseType_Enum: {
        __yield_0 = this->u.enum_->sym->display;
      } break;
      case types_BaseType_Alias: {
        __yield_0 = format_string("%s", this->name);
      } break;
      default: {
        __yield_0 = types_BaseType_str(this->base);
      } break;
    }
;__yield_0; });
}

void usage(i32 code) {
  printf("--------------------------------------------------------""\n");
  printf("Usage: ./ocen [options] <file>""\n");
  printf("Options:""\n");
  printf("    -o path        Output executable (default: ./out)""\n");
  printf("    -c path        Output C code (default: {out}.c)""\n");
  printf("    -e0            Minimal one-line errors""\n");
  printf("    -e1            Error messages with source code (default)""\n");
  printf("    -e2            Error messages with source / hints""\n");
  printf("    -s             Silent mode (no debug output)""\n");
  printf("    -n             Don't compile C code (default: false)""\n");
  printf("    -d             Emit debug information (default: false)""\n");
  printf("    --docs path    Output documentation JSON (default: none)""\n");
  printf("                   (Default: working directory)""\n");
  printf("--------------------------------------------------------""\n");
  exit(code);
}

void save_and_compile_code(ast_program_Program *program, char *code) {
  if (!((bool)c_path)) {
    c_path=format_string("%s.c", exec_path);
  } 
  FILE *out_file = FILE_open(c_path, "w");
  FILE_puts(out_file, code);
  fclose(out_file);
  if (!compile_c) 
  return ;
  
  char *cmdbuf = ((char *)calloc(((u32)1), ((u32)1024)));
  strcat(cmdbuf, format_string("gcc -o %s %s", exec_path, c_path));
  for (std_vector_Iterator__7 __iter = std_vector_Vector__7_iter(program->c_flags); std_vector_Iterator__7_has_value(&__iter); std_vector_Iterator__7_next(&__iter)) {
    char *flag = std_vector_Iterator__7_cur(&__iter);
    {
      strcat(cmdbuf, " ");
      strcat(cmdbuf, flag);
    }
  }
  if (debug) {
    strcat(cmdbuf, " -ggdb3");
  } 
  if (!silent) {
    printf("[+] %s""\n", cmdbuf);
  } 
  i32 exit_code = system(cmdbuf);
  if ((exit_code != 0)) {
    printf("[-] Compilation failed""\n");
    exit(1);
  } 
}

i32 main(i32 argc, char **argv) {
  for (u32 i = ((u32)1); (i < ((u32)argc)); i+=((u32)1)) {
    {
      char *__match_str = argv[i];
      if (!strcmp(__match_str, "-h")) {
        usage(0);
      } else if (!strcmp(__match_str, "-s")) {
        silent=true;
      } else if (!strcmp(__match_str, "-d")) {
        debug=true;
      } else if (!strcmp(__match_str, "-n")) {
        compile_c=false;
      } else if (!strcmp(__match_str, "-o")) {
        i+=((u32)1);
        exec_path=argv[i];
      } else if (!strcmp(__match_str, "-c")) {
        i+=((u32)1);
        c_path=argv[i];
      } else if (!strcmp(__match_str, "-e0")) {
        error_level=((u32)0);
      } else if (!strcmp(__match_str, "-e1")) {
        error_level=((u32)1);
      } else if (!strcmp(__match_str, "-e2")) {
        error_level=((u32)2);
      } else if (!strcmp(__match_str, "--docs")) {
        i+=((u32)1);
        docs_path=argv[i];
      } else  {
        if (argv[i][((u32)0)]=='-') {
          printf("Unknown option: %s""\n", argv[i]);
          usage(1);
        }  else         if (!((bool)filename)) {
          filename=argv[i];
        }  else {
          printf("Unknown option/argument: '%s'""\n", argv[i]);
          usage(1);
        } 
        
      }
    }
  }
  if (!((bool)filename)) {
    printf("No file specified""\n");
    usage(1);
  } 
  ast_program_Program *program = ast_program_Program_new();
  program->error_level=error_level;
  program->gen_debug_info=debug;
  parser_Parser_parse_toplevel(program, filename);
  if (!std_vector_Vector__14_is_empty(program->errors)) 
  ast_program_Program_exit_with_errors(program);
  
  passes_mod_run_typecheck_passes(program);
  if (((bool)docs_path)) {
    docgen_generate_doc_json(program, docs_path);
  }  else {
    char *code = passes_mod_run_codegen_passes(program);
    save_and_compile_code(program, code);
  } 
}

