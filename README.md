# parallel-computing-lab
Parallel computing course lab project code.

## Python Code
* `analysis.ipynb` Jupyter notebook for the analysis and plotting of the results.
* `convert.py` Helper script to convert .tif images into .rgb files containing raw pixel information.
* `exec.py` Helper script to create `mpirun` commands using metadata of the images to run the C program.
* `metadata.py` Helper script to extract metadata from image files (name and resolution).

### Requirements
Scripts `convert.py` and `metadata.py` require the Pillow library to run. Conversion script expects input images in `./img_tif/` and will output `.rgb` images in `./img_rgb/`.

### Usage
```Shell
# Pre-Req: Download .tif images in ./img_tif/
python3 convert.py
# Generate metadata
python3 metadata.py
# Generation execution commands
python3 exec.py
```

## C Code
* `pixelate.c` Main program that implements the MPI algorithm.
* `pix_kernel.c / pix_kernel.h` Source files for the pixelate kernel implementation. 

### Requirements
The MPI algorithm requires a working OpenMPI installation to build and run and a compatible C compiler.

### Usage
Compile with: ``mpicc -o pixelate pixelate.c pix_kernel.c``

Run algorithm on the [heic0601a Hubble Orion Nebula image](https://esahubble.org/images/heic0601a/), converted from tif to rgb using `convert.py`. The algorithm is run using 32 processors and a kernel window of 32x32px:
```Shell
mpirun -np 32 ./pixelate img_rgb/heic0601a.rgb 18000 18000 3 32 32
```

Example `job32.slurm` file:

```Shell
#!/bin/bash
#SBATCH -J pixelate32
#SBATCH -o output_%j.txt
#SBATCH -e errors_%j.txt
#SBATCH --mail-user <email>
#SBATCH --mail-type all
#SBATCH -t 00:10:00
#SBATCH -n 32
#SBATCH -p allgroups
#SBATCH --mem 8G

cd $SLURM_SUBMIT_DIR

mpicc -o pixelate pixelate.c pix_kernel.c
echo "Run 1"
mpirun -np $SLURM_NTASKS ./pixelate img_rgb/heic0601a.rgb 18000 18000 3 32 32
echo "Run 2"
mpirun -np $SLURM_NTASKS ./pixelate img_rgb/heic0601a.rgb 18000 18000 3 32 32
echo "Run 3"
mpirun -np $SLURM_NTASKS ./pixelate img_rgb/heic0601a.rgb 18000 18000 3 32 32
echo "Run 4"
mpirun -np $SLURM_NTASKS ./pixelate img_rgb/heic0601a.rgb 18000 18000 3 32 32
echo "Run 5"
mpirun -np $SLURM_NTASKS ./pixelate img_rgb/heic0601a.rgb 18000 18000 3 32 32
```
Submit using ``sbatch job32.slurm``
