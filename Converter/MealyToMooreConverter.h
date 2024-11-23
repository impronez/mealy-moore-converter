#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <set>

#include "../Automata/MealyAutomata.h"
#include "../Automata/MooreAutomata.h"

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

        ClearImpossibleStates(mealyTransitionTable, mealyStates);

        auto uniqueTransitions = GetUniqueTransitions(mealyTransitionTable, mealyStates);

        auto stateToTransitions = GetStateToTransitionsMap(uniqueTransitions);

        auto transitionToNewStateName = GetNewStateNamesFromTransitions(uniqueTransitions, mealyStates, stateToTransitions);

        auto mooreStatesInfo = GetMooreStatesInfo(transitionToNewStateName);

        MooreTransitionTable mooreTransitionTable;
        for (auto& row: mealyTransitionTable)
        {
            std::string inputSymbol = row.first;
            std::vector<State> states;
            for (unsigned index = 0; auto& it: row.second)
            {
                const std::string& state = mealyStates[index++];
                std::string newState = transitionToNewStateName[it];

                auto transitionsCount = GetTransitionsCountWithEqualState(state, stateToTransitions);

                states.insert(states.end(), transitionsCount, newState);
            }
            mooreTransitionTable.emplace_back(inputSymbol, states);
        }

        return std::make_unique<MooreAutomata>(
            std::move(inputSymbols),
            std::move(mooreStatesInfo),
            std::move(mooreTransitionTable));
    }

private:
    static unsigned GetTransitionsCountWithEqualState(const std::string& state,
        std::map<State, std::set<Transition>>& stateToTransitions)
    {
        return stateToTransitions[state].size();
    }

    static MooreStatesInfo GetMooreStatesInfo(std::map<Transition, std::string>& transitionToNewStateName)
    {
        MooreStatesInfo mooreStateInfo;
        for (auto& it: transitionToNewStateName)
        {
            mooreStateInfo.emplace_back(it.second, it.first.outputSymbol);
        }

        std::sort(mooreStateInfo.begin(), mooreStateInfo.end(), SortStringFromIndexesComp);

        return mooreStateInfo;
    }

    static std::map<State, std::set<Transition>> GetStateToTransitionsMap(std::vector<Transition>& transitions)
    {
        std::map<State, std::set<Transition>> stateToTransitions;
        for (auto& transition: transitions)
        {
            if (!stateToTransitions.contains(transition.nextState))
            {
                stateToTransitions[transition.nextState] = {};
            }

            if (!stateToTransitions[transition.nextState].contains(transition))
            {
                stateToTransitions[transition.nextState].emplace(transition);
            }
        }

        return stateToTransitions;
    }

    static std::map<Transition, std::string> GetNewStateNamesFromTransitions(std::vector<Transition>& transitions,
        MealyStates& states, std::map<State, std::set<Transition>>& stateToTransitions)
    {
        std::map<Transition, std::string> transitionToNewStateNames;
        for (unsigned index = FIRST_STATE_INDEX; auto& state: states)
        {
            for (auto& transition: stateToTransitions[state])
            {
                transitionToNewStateNames[transition] = STATE_CHAR + std::to_string(index++);
            }
        }

        return transitionToNewStateNames;
    }

    static std::vector<Transition> GetUniqueTransitions(MealyTransitionTable& transitionTable, MealyStates& states)
    {
        std::set<State> statesInTransitions;
        std::set<Transition> uniqueTransitions;
        std::vector<Transition> uniqueTransitionVector;

        for (auto& row: transitionTable)
        {
            for (auto& transition: row.second)
            {
                if (!uniqueTransitions.contains(transition))
                {
                    uniqueTransitions.emplace(transition);
                    uniqueTransitionVector.emplace_back(transition);
                    statesInTransitions.emplace(transition.nextState);
                }
            }
        }

        for (auto& it: states)
        {
            if (!statesInTransitions.contains(it))
            {
                uniqueTransitionVector.emplace_back(it, "");
                uniqueTransitions.emplace(it, "");
            }
        }

        return uniqueTransitionVector;
    }

    static bool SortStringFromIndexesComp(const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b)
    {
        int numA = std::stoi(a.first.substr(1));
        int numB = std::stoi(b.first.substr(1));

        return numA < numB;
    }

    static void ClearImpossibleStates(MealyTransitionTable& table, MealyStates& states)
    {
        std::map<std::string, unsigned> statesIndexes;
        for (unsigned index = 0; auto& state: states)
        {
            statesIndexes[state] = index++;
        }

        auto possibleStatesIndexes = GetPossibleStateIndexes(table, statesIndexes);
        if (possibleStatesIndexes.empty())
        {
            return;
        }

        std::vector<std::string> possibleStates;
        for (auto it: possibleStatesIndexes)
        {
            possibleStates.emplace_back(states.at(it));
        }

        MealyTransitionTable updatedTable;
        for (auto& row: table)
        {
            std::string inputSymbol = row.first;
            std::vector<Transition> rowStates;

            for (auto it: possibleStatesIndexes)
            {
                rowStates.emplace_back(row.second.at(it));
            }

            updatedTable.emplace_back(inputSymbol, rowStates);
        }

        states = std::move(possibleStates);
        table = std::move(updatedTable);
    }

    static std::set<unsigned> GetPossibleStateIndexes(const MealyTransitionTable& table, std::map<std::string, unsigned> statesIndexes)
    {
        std::vector<unsigned> possibleStateIndexesVector { 0 };
        std::set<unsigned> possibleStateIndexesSet { 0 };
        size_t possibleStateIndex = 0;

        while (possibleStateIndex < possibleStateIndexesVector.size())
        {
            unsigned index = possibleStateIndexesVector[possibleStateIndex++];

            for (auto& row: table)
            {
                std::string nextState = row.second[index].nextState;
                unsigned nextStateIndex = statesIndexes[nextState];

                if (!possibleStateIndexesSet.contains(nextStateIndex))
                {
                    possibleStateIndexesVector.push_back(nextStateIndex);
                    possibleStateIndexesSet.emplace(nextStateIndex);
                }
            }
        }

        return possibleStateIndexesSet;
    }

    std::unique_ptr<MealyAutomata> m_mealy;
};
