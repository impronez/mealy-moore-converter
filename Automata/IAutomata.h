#pragma once
#include <memory>
#include <string>

struct Transition
{
    Transition(const std::string& nextState, const std::string& outputSymbol)
        : nextState(nextState),
        outputSymbol(outputSymbol)
    {}

    std::string nextState;
    std::string outputSymbol;

    bool operator<(const Transition& other) const
    {
        if (nextState != other.nextState)
        {
            return nextState < other.nextState;
        }
        return outputSymbol < other.outputSymbol;
    }
};

class IAutomata
{
public:
    virtual void ExportToCsv(const std::string& filename) const = 0;

    virtual ~IAutomata() = default;
};
