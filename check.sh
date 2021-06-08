#!/bin/bash

function pause(){
   read -p "$*"
}


cppcheck --enable=all --language=c++ task.cpp --suppress=missingInclude

pause 'Press [Enter] key to continue...'

cpplint task.cpp

pause 'Press [Enter] key to continue...'