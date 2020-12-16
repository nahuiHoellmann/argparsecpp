/*
Index of this file:
[SECTION] ParserException / ErrorHandling
[SECTION] Argument
[SECTION] _TypedArgument
[SECTION] _TypedArgument specializations
[SECTION] Parser
*/


#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace ArgumentParser {


//-------------------------------------------------------------------------
// [SECTION] ParserException
//-------------------------------------------------------------------------

struct ParserException : public std::exception
{   
    std::string description;
    ParserException(std::string d);
    const char * what () const throw ();
};


//-------------------------------------------------------------------------
// [SECTION] Argument
//-------------------------------------------------------------------------

class Argument
{
private:
    bool hasTwoIdentifiers = false;
    bool found = false;
    bool positional = false;
    std::string firstIdentifier;
    std::string secondIdentifier;
    std::string help = "";
protected:
    int requiredPositions = 1;

    Argument(std::string firstIdentifier, std::string secondIdentifier, int requiredPositions);

public:
    int consume(int argc, char* argv[]);
    void setPositional(bool);
    void setHelp(std::string);
    virtual int parse(int argc, char* argv[]) = 0;
    void print(std::ostream& o);
    void printHelp(std::ostream& o);
};

typedef std::string help;

Argument& operator<<(Argument &arg, help h);
std::ostream& operator<<(std::ostream& o, Argument &arg);


//-------------------------------------------------------------------------
// [SECTION] _TypedArgument
//-------------------------------------------------------------------------

template <typename T>
class _TypedArgument:
    public Argument
{
    T* storeTo;
public:
    _TypedArgument(T* storeTo, std::string firstIdentifier, std::string secondIdentifier="");
    virtual int parse(int argc, char* argv[]) override;
};


template <typename T>
_TypedArgument<T>::_TypedArgument(T* storeTo, std::string firstIdentifier, std::string secondIdentifier)
    :Argument(firstIdentifier, secondIdentifier, typeid(T) == typeid(bool)?0:1), storeTo(storeTo){};


//-------------------------------------------------------------------------
// [SECTION] _TypedArgument specializations
//-------------------------------------------------------------------------

template<> int _TypedArgument<bool>::parse(int argc, char* argv[]);
template<> int _TypedArgument<int>::parse(int argc, char* argv[]);
template<> int _TypedArgument<std::string>::parse(int argc, char* argv[]);


//-------------------------------------------------------------------------
// [SECTION] Parser
//-------------------------------------------------------------------------

class Parser
{
    std::vector<Argument*> options;
    std::vector<Argument*> positionalArgs;
    std::string programName;

    std::string description;

    bool shouldPrintHelp = false;

public:

    Parser(std::string description = "");

    template <typename T>
    Argument& addArgument(T* storeTo, std::string firstIdentifier, std::string secondIdentifier = "")
    {
        bool positional = !(firstIdentifier.at(0) == '-');
        
        if(secondIdentifier != "")
        {
            if(positional || (!positional && !(secondIdentifier.at(0) == '-' && secondIdentifier.at(1) == '-')))
            {
                std::stringstream ss;
                ss  << "Invalid second identifier "  << firstIdentifier;
                throw ParserException(ss.str());
            }
        }

        Argument* arg = static_cast<Argument*>(new _TypedArgument<T>(storeTo, firstIdentifier, secondIdentifier));
        arg -> setPositional(positional);

        if(positional) positionalArgs.push_back(arg);
        else options.push_back(arg);

        return *arg;
    }

    void printUsage(std::ostream& o);
    
    void parse(int argc, char* argv[]);

    void printHelp(std::ostream& o);
};

} // namespace ArgumentParser