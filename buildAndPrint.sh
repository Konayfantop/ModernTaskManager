#!/usr/bin/bash

declare -A utilityBasedOnInput
utilityBasedOnInput["shared_debug"]="cmake -DCMAKE_BUILD_TYPE=Debug ."
utilityBasedOnInput["shared_release"]="cmake -DCMAKE_BUILD_TYPE=Release ."
utilityBasedOnInput["test"]=compileAndRunUnittests
utilityBasedOnInput["unittest"]=compileAndRunUnittests
utilityBasedOnInput["regression"]="make VERBOSE=1; ./out"

function compileAndRunUnittests () {
    echo -e "\e[${GREEN}m [INFO]: Initializing and running unit-tests \e[${RESET}m"
    cmake -DBUILD_TESTING=ON ..
    make VERBOSE=1

    ctest --verbose
}

# TODO: Check if there is a clever way to exclude test and regression in the same time or debug and release in the same time
function determineCompileCommand () {
    for arg in "$@"; do
        if [ -n "${utilityBasedOnInput["$arg"]}" ]; then
            eval "${utilityBasedOnInput["$arg"]}"
        else
            echo -e "\e[${RED}m [ERROR]: The flag given doesn't exist. Type help for insight. Ending... \e[${RESET}m"
            exit;
        fi
    done
}

GREEN='0;32' # for normal print
YELLOW='0;33' # for warnings
RED='0;31' # for errors

echo -e "\e[${GREEN}m [INFO]: Building start for ModernTaskManager \e[${RESET}m"

if [ -d "build" ]; then
    echo -e "\e[${YELLOW}m [WARNING]: Old build dir detected with old compilation symbols. Removing and Recreating... \e[${RESET}m"
    rm -rf build
else
    echo -e "\e[${GREEN}m [INFO]: There is no old build dir. Creating... \e[${RESET}m"
fi

mkdir build && cd build

# for exporting compilation flags used to parse by Clangd (for staging and indexing)
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

if [ "$#" -gt 2 ]; then
    echo -e "\e[${RED}m [ERROR]: For the moment no more than 2 commands are supported. Type help for insight. Ending... \e[${reset}m"
    exit
elif [ "$#" -eq 0 ]; then
    determineCompileCommand "shared_debug" "out"
else
    echo -e "\e[${GREEN}m [INFO]: XYN_2 \e[${RESET}m"
    determineCompileCommand "$@"
fi

# little bit of info during compilation never hurt anyone

# Always index new entries by removing the old .json compilation flags in case a new .hpp is introduced
echo -e "\e[${GREEN}m [INFO]: Setting-up the brand new compile_commands.json as a result of the compilation process ! -> Clangd <- \e[${RESET}m"
cp compile_commands.json ../compile_commands.json # TODO_SOFT : Maybe we can compare the files instead of doing a needless copy all the time
echo -e "\e[${YELLOW}m [INFO]: YOU MIGHT NEED TO RESTART YOUR WINDOW ! -> Clangd <- \e[${RESET}m"
