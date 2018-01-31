# Hexcodes.py
# MIT LICENSE 2016
# Shaun Harker 

def isPowerOfTwo (n):
  return (n & (n-1) == 0) and (n != 0)

def isAdjacentHexcode(hexcode1, hexcode2):
  return int(hexcode1, 16) < int(hexcode2, 16) and isPowerOfTwo( int(hexcode1, 16) ^ int(hexcode2, 16) )

def essential(hexcode,n,m):
  i = int(hexcode, 16)
  binning = [sum ([ 1 if i & 2**k else 0 for k in range(j*m,(j+1)*m)]) for j in range(0,2**n)]
  if not 0 in binning:
    return False 
  if not m in binning:
    return False
  N = 2**n 
  for d in range(0,n):
    bit = 2**d 
    flag = False
    for x in range(0, N):
      if binning[x] != binning[x ^ bit]:
        flag = True
        break
    if not flag:
      return False
  return True
