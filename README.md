
# Matrix Vector Multiplication (MPI)

This program computes the product of a square matrix and a vector in C using the Open MPI library. To accomplish this task, it uses collective operations to distribute the load amongst nodes. It also limits the amount of processes used to the dimension of the matrix and vector, excluding unnecessary processes.


## Run Locally

The program uses redirection to read from the input.csv file. You can alter this file as you wish, making sure you comply with the following:

* The matrix is a square matrix (N x N)
* The vector and matrix share the same dimension (rules of matrix multiplication)
* The number of processes you use to execute the program is a power of two
* The matrix/vector dimension is also a power of two

Compile

```bash
  mpicc mpi-mult.c -o mpi-mult 
```

Execute

```bash
  mpirun --oversubscribe -np 4 ./mpi-mult < input.csv
```


## Sample input file

* The first line of the .csv file must be the matrix/vector dimension
* The second line is the matrix in a single line separated by spaces. So,  if you wanted to represent the matrix 

```c
  |1,2|
  |3,4|
  
```
You would write it as:

```c
  1 2 3 4 
```
* The third line is the vector in a single line separate by spaces. So, for the vector:

```c
  |1|
  |2|
```
You write:

```c
  1 2 
```

An example of the input.csv for a 2x2 matrix and a 2x1 vector, with a dimension of 2 would look like:

```python
2 //this is the dimension
1 2 3 4  //this is the matrix
1 2  //this is the vector
```
