#include <iostream>

#include "ArgumentsParser.h"
#include "AutomataController.h"
#include "Converter/MealyToMooreConverter.h"
#include "Converter/MealyToMooreConverter.h"
#include "Converter/MooreToMealyConverter.h"

void MealyToMooreConversion(Args& args)
{
    auto mealy = MealyController::GetMealyAutomataFromCsvFile(args.inputFilename);

    MealyToMooreConverter converter((std::move(mealy)));
    auto moore = converter.GetMooreAutomata();

    moore->ExportToCsv(args.outputFilename);
}

void MooreToMealyConversion(Args& args)
{
    auto moore = MooreController::GetMooreAutomataFromCsvFile(args.inputFilename);

    MooreToMealyConverter converter(std::move(moore));
    auto mealy = converter.GetMealyAutomata();

    mealy->ExportToCsv(args.outputFilename);
}

int main(const int argc, char** argv)
{
    try
    {
        switch (auto args = ParseArgs(argc, argv); args.operation)
        {
            case Operation::MealyToMoore:
                MealyToMooreConversion(args);
                break;
            case Operation::MooreToMealy:
                MooreToMealyConversion(args);
                break;
            default: break;
        }

        std::cout << "Converted!\n";
    }
    catch (const std::exception& err)
    {
        std::cout << err.what() << std::endl << "Not converted!\n";

        return -1;
    }
    return 0;
}
