#pragma once

#include <string>


#define HandleError(s) std::cout << s << std::endl; exit(-1);


namespace ArgumentParser {

class Argument
{
private:
    bool hasTwoIdentifiers = false;
    bool found = false;
    bool positional = false;
    std::string firstIdentifier;
    std::string secondIdentifier;
    std::string help;
protected:
    int requiredPositions = 1;

    Argument(std::string firstIdentifier, std::string secondIdentifier, int requiredPositions);

public:
    int consume(int argc, char* argv[]);
    void setPositional(bool);
    void setHelp(std::string);
    virtual int parse(int argc, char* argv[]) = 0;
};

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



template<> int _TypedArgument<bool>::parse(int argc, char* argv[]);
template<> int _TypedArgument<int>::parse(int argc, char* argv[]);
template<> int _TypedArgument<std::string>::parse(int argc, char* argv[]);

typedef std::string help;

Argument& operator<<(Argument &arg, help h);

} // namespace ArgumentParser