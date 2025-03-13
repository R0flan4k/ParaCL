#include "AST.h"
#include "paracl.h"
#include "ast_representation.h"

#include <memory>
#include <fstream>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Input file expected.\n";
        return 1;
    }
    std::string ifile_name(argv[1]);
    std::ifstream stream(ifile_name);
    if (stream.fail())
    {
        std::cerr << "File " << ifile_name << " is not exhisting.\n";
        return 1;
    }

    try
    {
        std::unique_ptr<FlexLexer> lexer = std::make_unique<yyFlexLexer>(&stream);
        yy::DriverPCL driver(lexer.release());
        yy::ast_representation_t astr;
        driver.parse(&astr);

        std::ofstream asts("./AST_dump"), sts("./ST_dump");
        yy::astr_dumper dumper(&asts, &sts); dumper(astr);
        astr.execute();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    
    return 0;
}