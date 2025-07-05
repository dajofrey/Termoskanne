#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

glslang_path=${SCRIPT_DIR}/../external/glslang-main
if [ -d "$glslang_path" ]; then
    echo "$glslang_path already exists!"
else
    cd ${SCRIPT_DIR}/../external
    wget -O tmp.zip https://github.com/KhronosGroup/glslang/archive/main.zip
    unzip -o tmp.zip
    rm tmp.zip
    cd glslang-main
    cmake -DENABLE_OPT=0 . && make
fi

cd ${SCRIPT_DIR}/../external/glslang-main/StandAlone

./glslangValidator -V -x -o ../../../src/lib/tk-terminal/Common/Data/GLSL/TextSDF.vert.inc ../../../build/glsl/TextSDF.vert
./glslangValidator -V -x -o ../../../src/lib/tk-terminal/Common/Data/GLSL/TextSDF.frag.inc ../../../build/glsl/TextSDF.frag
./glslangValidator -V -x -o ../../../src/lib/tk-terminal/Common/Data/GLSL/Background.vert.inc ../../../build/glsl/Background.vert
./glslangValidator -V -x -o ../../../src/lib/tk-terminal/Common/Data/GLSL/Background.frag.inc ../../../build/glsl/Background.frag
