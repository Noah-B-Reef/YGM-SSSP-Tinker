#!/bin/bash

#SBATCH --output testJob.out
#SBATCH --error testJobErr.out
#SBATCH --nodes=2

srun sssp_test 11 3
srun sleep 30

