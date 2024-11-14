##
# \file build.sh
# \author Isaiah Lateer
# 
# Used to build the project.
# 

#!/bin/bash

directory=$(dirname "$0")

pushd $directory

mkdir -p bin

source_files=$(find . -type f -name "*.c")

flags="-Wall -Werror -DNDEBUG -Isrc -o bin/noise_generator.exe"

gcc $source_files $flags

popd
