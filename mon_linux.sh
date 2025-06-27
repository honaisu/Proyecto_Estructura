#!/bin/bash
set -e

echo "[🐈] Compilando THE MON PROJECT..."

gcc *.c headers/code/*.c headers/TDAs/*.c -o main -lncurses -lm && ./main