#!/bin/sh
# Compile, Run and extract trace from matmul kernel

export PIN_ROOT=/home/mdl/azk6085/CSE530/pin-3.18-98332-gaebd7b1e6-gcc-linux/
size=${1:-'10'}
sparsity=${2:-'50'}
input_file=${3:-'input_matrix.in'}
build_type=${4:-'normal'}

python utils/random_matrix_generator.py --n $size --dump $input_file --sparsity $sparsity

if [[ $build_type = "clean" ]];
then
    echo 'clean build'
    rm -rf bin/
    rm -rf traces/
else
    echo 'recursive build'
fi

mkdir -p bin/
mkdir -p traces/

echo Compiling Matmul_boostlib
g++ -Wall src/matmul_boostlib.cpp -o bin/matmul_boostlib.o    
#g++ -std=c++98 -Wall -O3 -g src/matmul_boostlib.cpp -o bin/matmul_boostlib.o -pedantic

echo Compiling Matmul_strassenalgo
g++ -Wall src/matmul_strassenalgo.cpp -o bin/matmul_strassenalgo.o    
#g++ -std=c++98 -Wall -O3 -g src/matmul_strassenalgo.cpp -o bin/matmul_strassenalgo.o -pedantic

echo Compiling Matmul_ikjalgo
g++ -Wall src/matmul_ikjalgo.cpp -o bin/matmul_ikjalgo.o    
#g++ -std=c++98 -Wall -O3 -g src/matmul_ikjalgo.cpp -o bin/matmul_ikjalgo.o -pedantic

echo Compiling Matmul_ijkalgo
g++ -Wall src/matmul_ijkalgo.cpp -o bin/matmul_ijkalgo.o    
#g++ -std=c++98 -Wall -O3 -g src/matmul_ijkalgo.cpp -o bin/matmul_ijkalgo.o -pedantic

echo Compiling Matmul_csr
g++ -Wall src/matmul_csr.cpp src/csr.h -o bin/matmul_csr.o
#g++ -std=c++98 -Wall -O3 -g src/matmul_csr.cpp src/csr.h -o bin/matmul_csr.o -pedantic

echo Compiling Matmul_smash
g++ -Wall src/matmul_smash.cpp src/smash.h -o bin/matmul_smash.o
#g++ -std=c++98 -Wall -O3 -g src/matmul_smash.cpp src/smash.h -o bin/matmul_csr.o -pedantic

for entry in bin/*.o
do
  f=$(echo "${entry##*/}");
  kernelname=$(echo $f| cut  -d'.' -f 1);
  filename="${kernelname}_traces.out"
  if [[ $kernelname = "matmul_csr" ]];
  then
	echo "Passing matrix in csr fmt"
  	echo "Running $kernelname on $input_file"
	csrA="csrA_${input_file}"
	csrB="csrB_${input_file}"
  	time $PIN_ROOT/pin -t $PIN_ROOT/source/tools/ManualExamples/obj-intel64/pinatrace.so -- $entry $csrA $csrB
  elif [[ $kernelname = "matmul_smash" ]];
  then 	
	echo "Passing matrix in csr fmt"  
  	echo "Running $kernelname on $input_file"
	csrA="csrA_${input_file}"
	csrB="csrB_${input_file}"
  	time $PIN_ROOT/pin -t $PIN_ROOT/source/tools/ManualExamples/obj-intel64/pinatrace.so -- $entry -f 2 -s 2 -t 2 -i $csrA -k $csrB
  else 
	echo "Passing matrix in dense fmt"
  	echo "Running $kernelname on $input_file"
  	time $PIN_ROOT/pin -t $PIN_ROOT/source/tools/ManualExamples/obj-intel64/pinatrace.so -- $entry --input_file $input_file
  fi
  head pinatrace.out  
  mv pinatrace.out traces/$filename  
done

source run_simulator.sh /home/mdl/azk6085/CSE530/CachePerformanceOnMatMul/traces
