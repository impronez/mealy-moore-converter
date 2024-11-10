#pragma once

#ifndef MOORE_AUTOMATA_H
#define MOORE_AUTOMATA_H

#include <fstream>
#include <list>
#include <vector>

#include "IAutomata.h"

using InputSymbol = std::string;
using OutputSymbol = std::string;
using State = std::string;
using MooreTransitionTable = std::list<std::pair<InputSymbol, std::vector<State>>>;
using MooreStatesInfo = std::vector<std::pair<State, OutputSymbol>>;

class MooreAutomata final : public IAutomata
{
public:
    MooreAutomata(
        std::vector<InputSymbol>&& inputSymbols,
        MooreStatesInfo&& statesInfo,
        MooreTransitionTable&& transitionTable
    )
        : m_inputSymbols(std::move(inputSymbols)),
        m_statesInfo(std::move(statesInfo)),
        m_transitionTable(std::move(transitionTable))
    {}

    void ExportToCsv(const std::string &filename) const override
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open the file for writing.");
        }

        std::string statesStr, outputSymbolsStr;
        for (const auto& info : m_statesInfo)
        {
            outputSymbolsStr += ';' + info.second;
            statesStr += ';' + info.first;
        }
        outputSymbolsStr += '\n';
        statesStr += '\n';

        file << outputSymbolsStr;
        file << statesStr;

        for (const auto& input : m_inputSymbols)
        {
            file << input;

            for (const auto& transitions: m_transitionTable)
            {
                if (transitions.first == input)
                {
                    for (const auto& transition : transitions.second)
                    {
                        file << ";" << transition;
                    }
                    file << "\n";
                }
            }
        }

        file.close();
    }

    [[nodiscard]] std::vector<std::string> GetInputSymbols() const
    {
        return m_inputSymbols;
    }

    [[nodiscard]] std::vector<std::string> GetOutputSymbols() const
    {
        return m_outputSymbols;
    }

    [[nodiscard]] MooreStatesInfo GetStatesInfo() const
    {
        return m_statesInfo;
    }

    [[nodiscard]] MooreTransitionTable GetTransitionTable() const
    {
        return m_transitionTable;
    }

private:
    std::vector<std::string> m_inputSymbols;
    std::vector<std::string> m_outputSymbols;
    MooreStatesInfo m_statesInfo;
    MooreTransitionTable m_transitionTable;
};

#endif