#pragma once

#include <algorithm>
#include <map>
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
        auto transitions = GetUniqueConsistentTransitions(mealyTransitionTable, mealyStates);
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
        for (auto& it: mealyTransitionTable)
        {
            std::string inputSymbol = it.first;
            std::vector<State> states;

            for (auto& iter: transitions)
            {
                std::string state = iter.nextState;

                auto stateIter = std::find(mealyStates.begin(), mealyStates.end(), state);
                size_t index = std::distance(mealyStates.begin(), stateIter);

                auto transition = it.second[index];
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

    static size_t GetCountOfDuplicateTransitions(size_t stateIndex, std::vector<Transition>& transitions, MealyStates& states)
    {
        const std::string& stateToDuplicate = states.at(stateIndex);

        size_t count = 0;
        bool isFound = false;
        for (const auto& transition : transitions)
        {
            if (transition.nextState == stateToDuplicate)
            {
                if (!isFound)
                {
                    isFound = true;
                }

                count++;
            }
            if (isFound && transition.nextState != stateToDuplicate)
            {
                break;
            }
        }

        return count;
    }

    static std::vector<Transition> GetUniqueConsistentTransitions(MealyTransitionTable& transitionTable, MealyStates& initStates)
    {
        auto uniqueTransitions = GetUniqueTransitions(transitionTable);
        std::set<Transition> addedTransitions = {};
        std::vector states = { initStates.front() };
        std::set statesList = {initStates.front()};
        size_t stateIndex = 0;

        // Проход по столбцам
        while (stateIndex < states.size())
        {
            for (auto& it: transitionTable)
            {
                std::string state = it.second[stateIndex].nextState;
                if (!statesList.contains(state))
                {
                    states.emplace_back(state);
                    statesList.insert(state);
                }
            }

            ++stateIndex;
        }

        std::vector<Transition> validTransitions = {};

        for (auto& state: states)
        {
            bool isFound = false;
            for (const auto& it: uniqueTransitions)
            {
                if (state == it.nextState)
                {
                    isFound = true;
                    validTransitions.emplace_back(it);
                }
                else if (state != it.nextState && isFound)
                {
                    break;
                }
            }
        }

        return validTransitions;
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
