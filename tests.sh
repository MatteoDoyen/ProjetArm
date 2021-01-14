#!/bin/bash

make

echo -e "\n--- START memory_test :"
./memory_test
echo "--- END memory_test"

echo -e "\n--- START registers_test :"
./registers_test
echo "--- END registers_test"

echo -e "\n--- START load_store_test :"
./load_store_test
echo "--- END load_store_test"