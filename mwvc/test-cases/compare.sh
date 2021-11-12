#!/bin/bash
for fn in ./*.in; do
    f=0
    s=0
    f=$($1 < "$fn" | head -n 1)
    s=$($2 < "$fn" | head -n 1)
    echo -n "$fn $f $s " && bc <<<"scale=2; $f / $s"
done
