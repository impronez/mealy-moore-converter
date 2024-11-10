#pragma once
#ifndef MEALY_AUTOMATA_H
#define MEALY_AUTOMATA_H

#include <fstream>
#include <list>
#include <utility>
#include <vector>

#include "IAutomata.h"

using inputSymbol = std::string;
using MealyTransitionTable = std::list<std::pair<inputSymbol, std::vector<Transition>>>;
using MealyStates = std::vector<std::string>;

class MealyAutomata final : public IAutomata
{
public:
    MealyAutomata(MealyStates states, MealyTransitionTable table)
        : m_states(std::move(states)),
        m_transitionTable(std::move(table))
    {}

    void ExportToCsv(const std::string &filename) const override
    {
        std::ofstream output(filename);
        if (!output.is_open())
        {
            const std::string message = "Could not open file " + filename + " for writing";
            throw std::invalid_argument(message);
        }

        for (const auto& state: m_states)
        {
            output << ';' << state;
        }
        output << std::endl;

        for (const auto& [inputSymbol, transitions] : m_transitionTable)
        {
            output << inputSymbol;

            for (const Transition& transition : transitions)
            {
                output << ';' << transition.nextState << '/' << transition.outputSymbol;
            }

            output << std::endl;
        }
    }

    [[nodiscard]] MealyTransitionTable GetTransitionTable() const
    {
        return m_transitionTable;
    }

    [[nodiscard]] MealyStates GetStates() const
    {
        return m_states;
    }

    std::vector<std::string> GetInputSymbols()
    {
        std::vector<std::string> inputSymbols;

        for (auto& it: m_transitionTable)
        {
            inputSymbols.emplace_back(it.first);
        }

        return inputSymbols;
    }

private:
    MealyStates m_states;
    MealyTransitionTable m_transitionTable;
};

#endif