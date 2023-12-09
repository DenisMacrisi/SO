#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Script-ul nu s-a utilizat corespunzator: Mod de utilizare $0 <caracter> "
    exit 1
fi

caracter=$1
contor=0

while IFS= read -r prop; do
    res=$(echo "$prop" | grep -E "^[A-Z][a-zA-Z0-9 ,]*$caracter*[\.!?]{1}$" | grep -v -E "si[ ]*," | grep -v -E "n[pb]")
    if [ -n "$res" ]; then
        ((contor++))
    fi
done

echo "Numar: $contor"
