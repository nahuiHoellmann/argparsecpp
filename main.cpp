#include <string>
#include <iostream>
#include <vector>

#include "Argument.h"

namespace ArgumentParser {


class Parser
{
    std::vector<Argument*> options;
    std::vector<Argument*> positionalArgs;
public:
    template <typename T>
    Argument& addArgument(T* storeTo, std::string firstIdentifier, std::string secondIdentifier = "")
    {
        bool positional = !(firstIdentifier.at(0) == '-');
        
        if(secondIdentifier != "")
        {
            if(positional || (!positional && !(secondIdentifier.at(0) == '-' && secondIdentifier.at(1) == '-')))
            {
                HandleError("Invalid second identifier "  << firstIdentifier);
            }
        }

        Argument* arg = static_cast<Argument*>(new _TypedArgument<T>(storeTo, firstIdentifier, secondIdentifier));
        arg -> setPositional(positional);

        if(positional) positionalArgs.push_back(arg);
        else options.push_back(arg);

        return *arg;
    }

    void parse(int argc, char* argv[])
    {   

        argc -= 1;
        argv += 1;

        int parsed = 0;

        while(argc != 0 && argv[0][0] == '-')
        {
            for(auto a: options)
            {
                parsed = a -> consume(argc, argv);
                if(parsed != 0) break;
            }
            if(!parsed)
            {
                HandleError("Invalid Option " << argv[0]);
            }

            argc -= parsed;
            argv += parsed;
        }

        for(auto a: positionalArgs)
        {
            parsed = a -> consume(argc, argv);
            argc -= parsed;
            argv += parsed;
        }
    }
};

} // namespace ArgumetParser


struct Config
{
   int rows = 0;
   int cols = 0;
   std::string filename = "";
   std::string destination = "";
   bool verbose = false;
} config;




int main(int argc, char* argv[])
{
    {
    
        using namespace ArgumentParser;

        Parser parser;

        parser.addArgument(&config.rows, "-r", "--rows") << help("The rows in the matrix");
        parser.addArgument(&config.cols, "-c", "--cols");
        parser.addArgument(&config.verbose, "-v", "--verbose");
        parser.addArgument(&config.filename, "src") << help("The file to copy");
        parser.addArgument(&config.destination, "dest");

        parser.parse(argc, argv);
    }

    std::cout << "Rows: " << config.rows << std::endl;
    std::cout << "Cols: " << config.cols << std::endl;
    std::cout << "Filename: " << config.filename << std::endl;
    std::cout << "Verbose: " << config.verbose << std::endl;

    return 0;
}
