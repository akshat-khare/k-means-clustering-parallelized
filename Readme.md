# Compilation
```
g++ -o seq main_sequential.c lab1_io.c lab1_sequential.c -fopenmp  
g++ -o pthread main_pthread.c lab1_pthread.c lab1_io.c -fopenmp -pthread  
g++ -o omp main_omp.c lab1_omp.c lab1_io.c -fopenmp  
```
Use `seq` , `pthread`, `omp` binaries obtained for executing sequential, pthread and omp implementation respectively.  
Random initialization is on by default.
