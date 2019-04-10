@echo off

gcc -ggdb -Wall -std=c11 -O3 -Iinclude src/cl_win32.c -c -o build/crud_lite.o
windres -i src/cl_resource.rc -o build/resource.o
gcc -Llib build/crud_lite.o build/resource.o -o crud_lite.exe -luser32 -lgdi32 -lcomctl32 -lsqlite3