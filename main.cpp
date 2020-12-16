#include <string>
#include <iostream>
#include <vector>

#include "Argument.h"


struct Config
{
   int rows = 0;
   int cols = 0;
   std::string filename = "";
   bool verbose = false;
} config;




int main(int argc, char* argv[])
{
    {
    
        using namespace ArgumentParser;

        Parser parser("Transpose the rows of a matrix");

        parser.addArgument(&config.rows, "-r", "--rows") << help("The rows in the matrix");
        parser.addArgument(&config.cols, "-c", "--cols") << help("The columns of the matrix");
        parser.addArgument(&config.verbose, "-v", "--verbose");
        parser.addArgument(&config.filename, "src") << help("The csv file of ths source matrix");

        parser.parse(argc, argv);
    }

    std::cout << "Rows: " << config.rows << std::endl;
    std::cout << "Cols: " << config.cols << std::endl;
    std::cout << "Filename: " << config.filename << std::endl;
    std::cout << "Verbose: " << config.verbose << std::endl;

    return 0;
}
