#include "AST.h"
#include "paracl.h"
#include "ast_representation.h"

#include <memory>
#include <fstream>

int main()
{
    std::unique_ptr<FlexLexer> lexer = std::make_unique<yyFlexLexer>();
    yy::DriverPCL driver(lexer.release());
    yy::ast_representation_t astr;
    driver.parse(&astr);

    std::ofstream asts("./AST_dump"), sts("./ST_dump");
    yy::astr_dumper dumper(&asts, &sts); dumper(astr);
    astr.execute();
    return 0;
}