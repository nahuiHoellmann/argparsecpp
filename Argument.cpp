/*
Index of this file:
[SECTION] Helper functions
[SECTION] ParserException / ErrorHandling
[SECTION] Argument
[SECTION] _TypedArgument specializations
[SECTION] Parser
*/

#include "Argument.h"

#include <iostream>
#include <cstring>
#include <iomanip>

#define HELP_INDENTATION 4
#define HELP_IDENTIFIER_WIDTH 20

namespace ArgumentParser {


//-------------------------------------------------------------------------
// [SECTION] Helper functions
//-------------------------------------------------------------------------

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


//-------------------------------------------------------------------------
// [SECTION] ParserException  / ErrorHandling
//-------------------------------------------------------------------------

#define HandleError(s) \
    { \
      std::stringstream ss; \
      ss  << s; \
      throw ParserException(ss.str()); \
    }

ParserException::ParserException(std::string d):description(d){}

const char * ParserException::what () const throw ()
{
    return description.c_str();
}



//-------------------------------------------------------------------------
// [SECTION] Argument
//-------------------------------------------------------------------------

Argument::Argument(std::string firstIdentifier, std::string secondIdentifier, int requiredPositions):
    firstIdentifier(firstIdentifier),
    secondIdentifier(secondIdentifier),
    hasTwoIdentifiers(secondIdentifier != ""),
    requiredPositions(requiredPositions)
{}


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


void Argument::setHelp(std::string h)
{
    help = h;
}


void Argument::print(std::ostream& o)
{
    if(positional)
    {
        o << firstIdentifier;
    }
    else
    {
        o << "[" << (hasTwoIdentifiers?secondIdentifier:firstIdentifier) << "]";
    }
}


void Argument::printHelp(std::ostream& o)
{
    o << std::setw(HELP_INDENTATION) << "";

    std::string identfier;

    o << std::setw(HELP_IDENTIFIER_WIDTH) << std::setiosflags(std::ios::left);

    if(hasTwoIdentifiers)
    {
        std::stringstream ss;
        ss << firstIdentifier << "," << secondIdentifier;
        o << ss.str();
        
    }
    else
    {
        o << firstIdentifier;
    }
    
    o << std::resetiosflags(std::ios::left) << help << std::endl;
}

std::ostream& operator<<(std::ostream& o, Argument &arg)
{
    arg.print(o);
    return o;
}


Argument& operator<<(Argument &arg, help h)
{
    arg.setHelp(h);
    return arg;
}


//-------------------------------------------------------------------------
// [SECTION] _TypedArgument specializations
//-------------------------------------------------------------------------

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


//-------------------------------------------------------------------------
// [SECTION] Parser
//-------------------------------------------------------------------------

Parser::Parser(std::string description):description(description)
{
    addArgument(&shouldPrintHelp, "-h", "--help") << help("Print this help and exit");
}

void Parser::printUsage(std::ostream& o)
{
    o << "Usage: " << programName << " [-h] ";

    for(auto a = options.begin() + 1; a != options.end(); a++)
    {   
        o << **a << " ";
    }

    for(auto a: positionalArgs)
    {
        o << *a << " ";
    }
    o << std::endl;
}

void Parser::parse(int argc, char* argv[])
{   
    try
    {
        programName = argv[0];

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

            if(shouldPrintHelp) printHelp(std::cout);
        }

        for(auto a: positionalArgs)
        {
            parsed = a -> consume(argc, argv);
            argc -= parsed;
            argv += parsed;
        }
    }
    catch (ParserException& e)
    {
        printUsage(std::cerr);
        std::cerr << programName << ": error";

        if(argc > 0)
        {
            std::cerr << " at " << argv[0];
        } 
        std::cerr << ": " << e.description << std::endl;
        exit(-1);
    }
}

void Parser::printHelp(std::ostream& o)
{
    printUsage(o);

    if(description != "")
    {
        o << std::endl << description << std::endl;
    }

    o << std::endl << "Positional arguments:" << std::endl;

    for(auto a: positionalArgs)
    {
        a -> printHelp(o);
    }

     o << std::endl << "Options:" << std::endl;

    for(auto a: options)
    {
        a -> printHelp(o);
    }

    exit(0);
}

} // namespace ArgumetParser