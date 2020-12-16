#include <cstdarg>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>

namespace ArgumentParser {

int isInt(const char* s){
    for(int i = 0; i < strlen(s); i++)
    {
        if(!isdigit(s[i]))
        {
           return 0;
        }
    }
    return 1;
}

typedef int (*StringParser)(void*, int argc, char* argv[]);

int parseString(void* storeTo, int argc, char* argv[])
{
    *(std::string*)storeTo = argv[0];
    return 1;
}

int parseInt(void* storeTo, int argc, char* argv[])
{   
    if(!isInt(argv[0]))
    {
        std::cout << argv[0] << " is not a valid int" << std::endl;
    }
    if(!isInt(argv[0]))
    {
        std::cout << argv[0] << " is not a valid int" << std::endl;
    }
    
    *(int*)storeTo = atoi(argv[0]);
    return 1;
}

int parseBool(void* storeTo, int argc, char* argv[])
{
    *(bool*)storeTo = true;
    return 0;
}



struct Argument
{
    bool hasTwoIdentifiers = false;
    bool found = false;
    bool positional = false;
    std::string firstIdentifier;
    std::string secondIdentifier;
    std::string help;
    int requiredPositions;
    StringParser parseValue;

    Argument(void* p):storeTo(p){};

    int consume(int argc, char* argv[])
    { 
        if(!positional)
        {
            if(argc != 0 && (firstIdentifier == argv[0] || (hasTwoIdentifiers && secondIdentifier == argv[0])))
            {
                if(requiredPositions + 1 <= argc)
                {
                    return 1 + parseValue(storeTo, argc - 1, &argv[1]);
                }
                else
                {
                     std::cout << "Missing argument for " << "firstIdentifier";
                }
            }
            return 0;
        }
        else if(requiredPositions <= argc)
        {
            return parseValue(storeTo, argc, argv);
        }
        else
        {
            std::cout << "Missing argument for " << "firstIdentifier";
        }
        return 0;
    }

    template <typename T>
    static Argument* create_new_argument(T* storeTo)
    {

        Argument* arg = new Argument((void*) storeTo);

        if (typeid(T) == typeid(int))
        {
            arg -> requiredPositions = 1;
            arg -> parseValue = parseInt;
        }
        else if (typeid(T) == typeid(bool))
        {
            arg -> requiredPositions = 0;
            arg -> parseValue = parseBool;
        }
        else if (typeid(T) == typeid(std::string))
        {
            arg -> requiredPositions = 1;
            arg -> parseValue = parseString;
        }

        return arg;
    }
};


class Parser
{
    std::vector<Argument*> options;
    std::vector<Argument*> positionalArgs;
public:
    template <typename T>
    Argument& addArgument(T* store, ...)
    {
        Argument* arg = Argument::create_new_argument(store);

        int n = 0;

        va_list args;
        va_start(args, store);
        while(true)
        {
            std::string a = va_arg(args, char*);
            if(a.at(0) == '-' && a.at(1) != '-' && n == 0)
            {
                //std::cout << "short arg: " << a << std::endl;
                arg -> firstIdentifier = a;
                n++;
                continue;
            }
            else if (a.at(0) == '-' && a.at(1) == '-' && n < 2)
            {
                //std::cout << "long arg: " << a << std::endl;
                if(n == 0)
                {
                    arg -> firstIdentifier = a;
                }
                else
                {
                    arg -> secondIdentifier = a;
                    arg -> hasTwoIdentifiers = true;
                }
                
                n++;
            }
            else if (n == 0)
            {
                //std::cout << "positional arg: " << a << std::endl;
                arg -> positional = true;
                arg -> firstIdentifier = a;
                n++;
            }
            break;
        }
        va_end(args);

        if(arg -> positional)
        {
            positionalArgs.push_back(arg);
        }
        else
        {
            options.push_back(arg);
        }
        

        return (*arg);
    }

    void parse(int argc, char* argv[])
    {   

        argc -= 1;
        argv += 1;

        int parsed;

        while(true)
        {
            parsed = 0;
            for(auto a: options)
            {
                parsed = a -> consume(argc, argv);
                if(parsed != 0) break;
            }
            if(parsed == 0) break;

            argc -= parsed;
            argv = &argv[parsed];
        }

        for(auto a: positionalArgs)
        {
            parsed = a -> consume(argc, argv);
            argc -= parsed;
            argv = &argv[parsed];
        }
    }
};

typedef std::string help;

Argument& operator<<(Argument &arg, help h)
{
    //td::cout << "Added Help '" << h << "' to: " << arg.firstIdentifier << std::endl;
    arg.help = h;
    return arg;
}

}


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

// "..."


// parser.add_argument("filename",help="xy")
// parser.add_argument("square", type=int)
// parser.add_argument("-o", "--optional")
// parser.add_argument("-b", "--boolean", action="store_true")
// parser.add_argument("-v", "--verbosity", choices=[1,2,3])
// parser.add_arguument("rest", nargs='*')

// args = parser.parse_args()

// print(args.filename)
