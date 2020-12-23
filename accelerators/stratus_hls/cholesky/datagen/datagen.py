import numpy as np
n = input("Enter Number of Rows in the Matrix : ")
K = np.random.randint(5, size=(n, n))
A = np.random.rand(n, n)
B=A.transpose()
C = np.dot(A, B)
D= np.identity(n) 
E= n*D
F = np.add(C,E)
np.savetxt('input.txt',F,fmt='%.6f')
L = np.linalg.cholesky(F)
np.savetxt('output.txt',L,fmt='%.8f')
