#pragma once

#include "script/parser/core/expr_parser.hpp"
#include "script/parser/core/stmt_parser.hpp"
#include "script/parser/expr/grouping.hpp"
#include "script/parser/expr/ident_call.hpp"
#include "script/parser/expr/kw_expr.hpp"
#include "script/parser/expr/list_lit.hpp"
#include "script/parser/expr/literals.hpp"
#include "script/parser/expr/prefix_unary.hpp"
#include "script/parser/stmt/for.hpp"
#include "script/parser/stmt/if.hpp"
#include "script/parser/stmt/let.hpp"
#include "script/parser/stmt/return.hpp"
#include "script/parser/stmt/while.hpp"

namespace valkey::script {

using ActiveExprParser = ExprParser<PrefixUnaryParser, IntLitParser, DblLitParser, StrLitParser,
                                    IdentOrCallParser, ScriptCallParser, GroupingParser,
                                    ListLitParser, KwExprParser>;

using ActiveStmtParser =
    StmtParser<ActiveExprParser, LetParser, IfParser, WhileParser, ForParser, ReturnParser>;

}  // namespace valkey::script
