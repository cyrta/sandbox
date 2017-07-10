

all: binarytrees regexredux revcomp fannkuchredux mandelbrot nbody fasta pidigits

pidigits:

fasta:

nbody:
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -mfpmath=sse -msse3 nbody.c -o nbody -lm

spectralnorm:
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -fopenmp -mfpmath=sse -msse2 spectralnorm.c -o spectralnorm -lm

mandelbrot:
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -mno-fma -fno-finite-math-only -mfpmath=sse -msse2 -fopenmp mandelbrot.gcc-6.c -o mandelbrot

fannkuchredux:
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -std=c99 -fopenmp fannkuchredux.c -o fannkuchredux
	i
revcomp:
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -funroll-loops -fopenmp revcomp.c -o revcomp

regexredux:
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -fopenmp regexredux.c -o regexredux -lpcre

binarytrees:
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -fopenmp -D_FILE_OFFSET_BITS=64 -I/usr/include/apr-1 binary-trees.c -o binary-trees  -lapr-1 -lgomp -lm 
