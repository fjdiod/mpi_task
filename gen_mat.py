import numpy as np
N = 64
np.random.seed(13)
mat = np.random.randint(10, size=(N, N))
v = np.random.randint(10, size=N)
print(mat.dot(v))

file1 = open('mat2', 'w')
file2 = open('vect2', 'w')
print(mat)
print(v)
file1.write(str(N) + '\n')
file2.write(str(N) + '\n')
str1 = ''
str2 = ''
for i in range(N):
    for j in range(N):
        str1 += str(mat[i][j]) + " "
    str2 += str(v[i]) + " "
file1.write(str1)
file2.write(str2)
