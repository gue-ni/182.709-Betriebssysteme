import math
import cmath

#re = [complex(0, 0), complex(0, 0)]
#ro = [complex(0, 0), complex(2, 0)]

re = [complex(0, 0), complex(1.4, 0)]
ro = [complex(0, 0), complex(1.4, 0)]

#re = [complex(0.7, 0)]
#ro = [complex(-0.7, 0)]

n = len(re) + len(ro)
r = [complex(i, i) for i in range(0, n)]

print("re:")
print(re)
print("ro:")
print(ro)
PI = math.pi


for k in range(0, int(n/2)):
	x = -((2*PI)/n) * k
	exp = complex( math.cos(x), math.sin(x) ) 
	r[k] = re[k] + exp * ro[k]	
	r[k+int(n/2)] = re[k] - exp * ro[k]

print("result:")
print(r)
