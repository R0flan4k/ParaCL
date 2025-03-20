#include "AST.h"
#include "AST_dumper.h"
#include "paracl.h"
#include "ast_representation.h"
#include "lexer.h"
#include "driver_exceptions.h"

#include <memory>
#include <fstream>

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Input file expected.\n";
            return 1;
        }
        std::string ifile_name(argv[1]);
        std::ifstream file_stream(ifile_name);
        if (file_stream.fail())
        {
            std::cerr << "File " << ifile_name << " is not exhisting.\n";
            return 1;
        }
        
        yy::LexerPCL lexer(&file_stream);
        yy::DriverPCL driver(&lexer, ifile_name);
        yy::ast_representation_t astr;
        driver.parse(&astr);

#ifndef NDEBUG
        std::ofstream asts("./AST_dump"), sts("./ST_dump");
        yy::astr_dumper dumper(&asts, &sts); 
        dumper(astr);
#endif
        astr.execute();
    }
    catch (const ExceptsPCL::compilation_error& ce)
    {
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    
    return 0;
}