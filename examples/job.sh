#!/bin/bash

#SBATCH --output  $mama_path
#SBATCH --error testJobErr.out
#SBATCH --nodes=2

echo $mama_path
echo $mia_data
srun /home/noahr/YGM_SSSP_Tinker/build/examples/sssp 30 3 $mia_data
srun sleep 20