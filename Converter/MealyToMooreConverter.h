#pragma once

#include <algorithm>
#include <map>
#include <queue>
#include <set>

#include "../Automata/MealyAutomata.h"

class MealyToMooreConverter
{
public:
    static constexpr char STATE_CHAR = 'q';
    static constexpr size_t FIRST_STATE_INDEX = 0;

    explicit MealyToMooreConverter(std::unique_ptr<MealyAutomata> mealy)
        : m_mealy(std::move(mealy))
    {}

    [[nodiscard]] std::unique_ptr<MooreAutomata> GetMooreAutomata() const
    {
        auto mealyTransitionTable = m_mealy->GetTransitionTable();
        auto mealyStates = m_mealy->GetStates();
        auto inputSymbols = m_mealy->GetInputSymbols();

        auto transitions = GetUniquePossibleTransitions(mealyTransitionTable, mealyStates);
        auto transitionTable = GetNewStateNamesFromTransitions(transitions);

        MooreStatesInfo mooreStateInfo;
        for (auto& it: transitionTable)
        {
            mooreStateInfo.emplace_back(it.second, it.first.outputSymbol);
        }

        std::sort(mooreStateInfo.begin(), mooreStateInfo.end(), [](const std::pair<std::string, std::string>& a,
            const std::pair<std::string, std::string>& b) {
                return a.first < b.first;
        });

        MooreTransitionTable mooreTransitionTable;
        for (auto& row: mealyTransitionTable)
        {
            std::string inputSymbol = row.first;
            std::vector<State> states;

            for (auto& it: transitions)
            {
                std::string state = it.nextState;

                auto stateIter = std::find(mealyStates.begin(), mealyStates.end(), state);
                size_t index = std::distance(mealyStates.begin(), stateIter);

                auto transition = row.second[index];

                const std::string& transitionNewName = transitionTable.at(transition);

                states.emplace_back(transitionNewName);
            }
            mooreTransitionTable.emplace_back(inputSymbol, states);
        }

        return std::make_unique<MooreAutomata>(
            std::move(inputSymbols),
            std::move(mooreStateInfo),
            std::move(mooreTransitionTable));
    }
private:
    static std::map<Transition, std::string> GetNewStateNamesFromTransitions(std::vector<Transition>& transitions)
    {
        std::map<Transition, std::string> transitionToNewStateNames;

        for (unsigned index = FIRST_STATE_INDEX; auto& transition: transitions)
        {
            transitionToNewStateNames[transition] = STATE_CHAR + std::to_string(index++);
        }

        return transitionToNewStateNames;
    }

    static std::vector<std::string> GetAllPossibleState(MealyTransitionTable& transitionTable, MealyStates& mealyStates)
    {
        std::vector<std::string> possibleStates = { mealyStates.front() };
        std::set<std::string> possibleStatesSet = { mealyStates.front() };
        size_t possibleStatesIndex = 0;

        while (possibleStatesIndex < possibleStates.size())
        {
            std::string sourceState = possibleStates[possibleStatesIndex];
            size_t index = GetIndexOfStringInVector(mealyStates, possibleStates.at(possibleStatesIndex++));

            for (auto& it: transitionTable)
            {
                std::string state = it.second[index].nextState;
                if (!possibleStatesSet.contains(state))
                {
                    possibleStatesSet.insert(state);
                    possibleStates.push_back(state);
                }
            }
        }

        return possibleStates;
    }

    static std::vector<Transition> GetUniquePossibleTransitions(MealyTransitionTable& transitionTable, MealyStates& mealyStates)
    {
        auto uniqueTransitions = GetUniqueTransitions(transitionTable);

        std::vector<std::string> possibleStates = GetAllPossibleState(transitionTable, mealyStates);

        std::vector<Transition> possibleTransitions;

        for (auto& state: possibleStates)
        {
            for (bool isFound = false; auto& transition: uniqueTransitions)
            {
                if (transition.nextState == state)
                {
                    isFound = true;
                    possibleTransitions.push_back(transition);
                }
                if (isFound && transition.nextState != state)
                {
                    break;
                }
            }
        }

        return possibleTransitions;
    }

    static size_t GetIndexOfStringInVector(std::vector<std::string>& states, std::string& state)
    {
        auto it = std::find(states.begin(), states.end(), state);

        if (it != states.end())
        {
            return std::distance(states.begin(), it);
        }

        throw std::range_error("Invalid state");
    }

    static std::set<Transition> GetUniqueTransitions(MealyTransitionTable& transitionTable)
    {
        std::set<Transition> uniqueTransitions;
        for (auto& row: transitionTable)
        {
            for (auto& transition: row.second)
            {
                uniqueTransitions.emplace(transition);
            }
        }

        return uniqueTransitions;
    }

    std::unique_ptr<MealyAutomata> m_mealy;
};
