#!/bin/sh

while inotifywait --event modify --event move --event create --event delete --event delete_self ../src/* ../include/* ../sample/* ../CMakeLists.txt; do
    make -j $(nproc) -k
done
