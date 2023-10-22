#!/bin/bash
read prop

echo $prop | grep -E "^[A-Z][a-zA-Z0-9 ,]*(\.){1}$" | grep -v -E "si[ ]*," | grep -v -E "n[pb]"