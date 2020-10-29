#!/bin/bash
for fn in ./*.in; do
    echo $($1 < "$fn" | head -n 1) $($2 < "$fn" | head -n 1)
done
