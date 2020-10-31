#!/bin/bash
g++ -std=c++17 ../code/fastvc.cpp -o ../code/fastvc.out
for fn in ./*.in; do
    echo $($1 < "$fn" | head -n 1) $($2 < "$fn" | head -n 1)
done
