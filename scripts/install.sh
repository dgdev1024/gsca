#!/bin/bash

premake5 gmake
make config=release $@
mkdir -p /usr/local/include/GSCA
cp ./projects/GSCA/*.h /usr/local/include/GSCA
cp ./build/bin/Release/libgsca.a /usr/local/lib
cp ./build/bin/Release/gscab /usr/local/bin
cp ./build/bin/Release/gscap /usr/local/bin
