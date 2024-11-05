#pragma once
#include <stdexcept>
#include <string>

const std::string MEALY_TO_MOORE = "mealy-to-moore";
const std::string MOORE_TO_MEALY = "moore-to-mealy";

enum class Operation
{
    MealyToMoore,
    MooreToMealy
};

struct Args
{
    Operation operation;
    std::string inputFilename;
    std::string outputFilename;
};

inline Args ParseArgs(const int argc, char** argv)
{
    if (argc != 4)
    {
        throw std::invalid_argument("Invalid number of arguments. Must be: <operation> <inputFilename> <outputFilename>");
    }

    Operation operation;

    if (argv[1] == MEALY_TO_MOORE)
    {
        operation = Operation::MealyToMoore;
    }
    else if (argv[1] == MOORE_TO_MEALY)
    {
        operation = Operation::MooreToMealy;
    }
    else
    {
        throw std::invalid_argument("Invalid operation");
    }

    return { operation, argv[2], argv[3] };
}
