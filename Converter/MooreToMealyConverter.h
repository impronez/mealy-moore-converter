#pragma once
#include <memory>

#include "../Automata/MealyAutomata.h"
#include "../Automata/MooreAutomata.h"

using MooreStatesInfoToStates = std::map<MooreStatesInfo, std::string>;

class MooreToMealyConverter
{
public:
    static constexpr char STATE_CHAR = 'F';
    static constexpr size_t FIRST_STATE_INDEX = 0;

    explicit MooreToMealyConverter(std::unique_ptr<MooreAutomata> moore)
        : m_moore(std::move(moore))
    {}

    [[nodiscard]] std::unique_ptr<MealyAutomata> GetMealyAutomata() const
    {
        std::vector<std::string> inputSymbols = m_moore->GetInputSymbols();
        std::vector<std::string> outputSymbols = m_moore->GetOutputSymbols();

        MooreStatesInfo mooreStatesInfo = m_moore->GetStatesInfo();
        MooreTransitionTable mooreTransitionTable = m_moore->GetTransitionTable();

        auto mealyStates = GetMealyStates(mooreStatesInfo);
        auto mealyTransitionTable = GetMealyTransitionTable(mooreStatesInfo, mooreTransitionTable);

        return std::make_unique<MealyAutomata>(mealyStates, mealyTransitionTable);
    }

private:
    static MealyStates GetMealyStates(const MooreStatesInfo& statesInfo)
    {
        MealyStates mealyStates;
        for (const auto& info : statesInfo)
        {
            mealyStates.emplace_back(info.first);
        }

        return mealyStates;
    }

    static MealyTransitionTable GetMealyTransitionTable(const MooreStatesInfo& statesInfo,
        const MooreTransitionTable& mooreTransitionTable)
    {
        MealyTransitionTable mealyTransitionTable;

        auto stateToOutputSymbolMap = GetStateToOutputSymbolMap(statesInfo);

        for (auto& mooreRow : mooreTransitionTable)
        {
            std::pair<inputSymbol, std::vector<Transition>> mealyRow;
            mealyRow.first = mooreRow.first;

            std::vector<Transition>& mealyTransitions = mealyRow.second;
            for (std::string state : mooreRow.second)
            {
                std::string outputSymbol = stateToOutputSymbolMap[state];
                state[0] = STATE_CHAR;

                Transition transition(state, outputSymbol);

                mealyTransitions.emplace_back(transition);
            }
            mealyTransitionTable.emplace_back(std::move(mealyRow));
        }

        return mealyTransitionTable;
    }

    static std::map<State, OutputSymbol> GetStateToOutputSymbolMap(const MooreStatesInfo& statesInfo)
    {
        std::map<State, OutputSymbol> stateToOutputSymbolMap;
        for (const auto& info : statesInfo)
        {
            stateToOutputSymbolMap[info.first] = info.second;
        }

        return stateToOutputSymbolMap;
    }

    std::unique_ptr<MooreAutomata> m_moore;
};
