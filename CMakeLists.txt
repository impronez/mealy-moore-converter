cmake_minimum_required(VERSION 3.29)
project(mealy_moore_converter)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

add_executable(mealy_moore_converter main.cpp
        ArgumentsParser.h
        AutomataController.h
        Automata/IAutomata.h
        Automata/MealyAutomata.h
        Automata/MooreAutomata.h
        Converter/MealyToMooreConverter.h
        Converter/MooreToMealyConverter.h)
