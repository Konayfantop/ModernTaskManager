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
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make VERBOSE=1
./out

echo -e "\e[${GREEN}m [INFO]: Setting-up the brand new compile_commands.json as a result of the compilation process ! -> Clangd <- \e[${RESET}m"
cp compile_commands.json ../compile_commands.json # TODO_SOFT : Maybe we can compare the files instead of doing a needless copy all the time
echo -e "\e[${YELLOW}m [INFO]: YOU MIGHT NEED TO RESTART YOUR WINDOW ! -> Clangd <- \e[${RESET}m"
