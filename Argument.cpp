#include "Argument.h"

#include <iostream>
#include <cstring>


namespace ArgumentParser {

static int isInt(const char* s)
{
    for(int i = 0; i < strlen(s); i++)
    {
        if(!isdigit(s[i]))
        {
           return 0;
        }
    }
    return 1;
}

int Argument::consume(int argc, char* argv[])
{ 
    if(!positional)
    {
        if(argc != 0 && (firstIdentifier == argv[0] || (hasTwoIdentifiers && secondIdentifier == argv[0])))
        {
            if(requiredPositions + 1 <= argc)
            {
                return 1 + parse(argc - 1, &argv[1]);
            }
            else
            {
                    HandleError("Missing argument for: " << firstIdentifier);
            }
        }
        return 0;
    }
    else if(requiredPositions <= argc)
    {
        return parse(argc, argv);
    }
    else
    {
         HandleError("Missing argument for: " << firstIdentifier);
    }
    return 0;
}

void Argument::setPositional(bool b)
{
    positional = b;
}

Argument::Argument(std::string firstIdentifier, std::string secondIdentifier, int requiredPositions):
    firstIdentifier(firstIdentifier),
    secondIdentifier(secondIdentifier),
    hasTwoIdentifiers(secondIdentifier != ""),
    requiredPositions(requiredPositions)
{}

template<> int _TypedArgument<int>::parse(int argc, char* argv[])
{
    if(!isInt(argv[0]))
    {
        HandleError(argv[0] << " is not a valid int");
    }
    
    *storeTo = atoi(argv[0]);
    return 1;
}


template<> int _TypedArgument<bool>::parse(int argc, char* argv[])
{
    *storeTo = true;
    return 0;
}

template<> int _TypedArgument<std::string>::parse(int argc, char* argv[])
{
    *storeTo = argv[0];
    return 1;
}

Argument& operator<<(Argument &arg, help h)
{
    //td::cout << "Added Help '" << h << "' to: " << arg.firstIdentifier << std::endl;
    arg.setHelp(h);
    return arg;
}

void Argument::setHelp(std::string h)
{
    help = h;
}

} // namespace ArgumetParser