import numpy as np
import sys
from matplotlib import pyplot as plt

argv_count = len(sys.argv)

if argv_count <= 1:
	print("no args ,exit")
	exit()
else:
	print("reading numpylized data from ",sys.argv[1])
	with open(sys.argv[1]) as ff:
		data = ff.read().replace("\n","").replace("\t","")
		t = eval(data)
		print("done reading data.")
		img = np.asarray(t)
		print(img,img.shape)
		if argv_count == 2:
			print("showing grayscale image...")
			plt.imshow(img,cmap='gray')
		elif argv_count == 3:
			print("showing RGB image...")
			img = img.astype(np.float32)
			plt.imshow(img)
		plt.show()
		print("done.")