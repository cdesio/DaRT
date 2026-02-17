#!/bin/bash

../../simulation/build/rbe -mac ../sim1.in -out sim1 -sugar sugarPos_4x4_300nm.bin -histone histonePositions_4x4_300nm.bin -seed 1

../../simulation/build/rbe -mac ../sim2.in -out sim2 -sugar sugarPos_4x4_300nm.bin -histone histonePositions_4x4_300nm.bin -seed 1

../../simulation/build/rbe -mac ../sim3.in -out sim3 -sugar sugarPos_4x4_300nm.bin -histone histonePositions_4x4_300nm.bin -seed 1

conda activate clustering
python ../clustering.py
