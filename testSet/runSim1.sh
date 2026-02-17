#!/bin/bash --login
#SBATCH --job-name=testSet
#SBATCH --output=testSet.out.%J
#SBATCH --error=testSet.err.%J
#SBATCH --time=0-02:00
#SBATCH --nodes=1
#SBATCH -p short
#SBATCH --ntasks-per-node=16
#SBATCH --mem 2GB
#SBATCH --account=IFAC023282

module add lang/gcc/9.1.0
module add apps/geant/4.11.1

time ../../simulation/build/rbe -mac ../sim1_bp.in -out sim1 -sugar sugarPos_4x4_300nm.bin -histone histonePositions_4x4_300nm.bin -seed 1

module load apps/root/6.26.00

conda activate clustering
python ../../Clustering/run.py --filename sim1.root --output sim1.csv --sugar sugarPos_4x4_300nm.bin


