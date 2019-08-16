import sys
import random

def mkLine(n):
    j = 0
    l = ""
    while j < n:
        b0 = str(random.randint(0,1))
        l += b0
        j += 1
    return l

def mkMatrix(m, n):
    i = 0
    M = ""
    while i < m:
        M += mkLine(n)
        M += "\n"
        i += 1
    return M
        
# print("Enter the number of rows: "[:-1])
# m = int(input())
# print("Enter the number of columns: "[:-1])
# n = int(input())
# print(M[:-1])

args = sys.argv
m = int(args[1])
n = int(args[2])
M = mkMatrix(m,n)
print(M[:-1])
