import numpy as np
import sys
from matplotlib import pyplot as plt

if len(sys.argv) <= 1:
	print("no args ,exit")
	exit()
print("reading numpylized data from ",sys.argv[1])
with open(sys.argv[1]) as ff:
	data = ff.read().replace("\n","").replace("\t","")
	t = eval(data)
	print("done reading data.")
	img = np.asarray(t)
	print(img)
	print("showing image...")
	plt.imshow(img,cmap='gray')
	plt.show()
	print("done.")