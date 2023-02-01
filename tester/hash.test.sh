#!/bin/bash
echo "[testing hash module]"
./runtest.sh "thash 10" "build and use a small hash table"
./runtest.sh "thash 100" "build and use a medium sized hash table"
./runtest.sh "thash 1000" "build and use a large sized hash table"
echo "[done]"
