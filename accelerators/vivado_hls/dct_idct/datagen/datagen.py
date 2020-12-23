#!/usr/bin/python

import sys

import numpy as np

m = int(sys.argv[1])  ##rows
n = int(sys.argv[2])  ##cols
K = np.random.randint(255, size=(m, n))


np.savetxt('input.txt',K,fmt='%d')
for i in range(0,m):
  for j in range(0,n):
	index = i*n+j
	print("input_matrix[" + str(index) + "] = " + str(K[i][j]) + ";")	
