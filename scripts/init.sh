#!/usr/bin/env sh

echo "Running 'scalar_product' benchmark..."
python3 scripts/sp-benchmark.py

echo "Running 'matrix_product' benchmark..."
python3 scripts/mp-benchmark.py

echo "Running 'simulation' benchmark..."
python3 scripts/sim-benchmark.py