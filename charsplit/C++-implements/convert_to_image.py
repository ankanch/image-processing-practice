import numpy as np
import os
import sys
from matplotlib import pyplot as plt

argv_count = len(sys.argv)

if argv_count <= 1:
	print("no args ,exit")
	exit()
else:
    img_des = sys.argv[1] + "alphaberts"
    try:    
        os.mkdir(img_des)
    except:
        pass
    imglist = os.listdir(sys.argv[1])
    for image in imglist:
        if "alp_" in image or "template_sc" in image:
            print("processing",image)
            with open(sys.argv[1] + image,"r") as ff:
                data = ff.read().replace("\n","").replace("\t","")
                t = eval(data)
                ss = []
                for row in t:
                    ss.append( [ 1 if x==0 else 0 for x in row ] )
                t = ss
                img = np.asarray(t)
                plt.imsave(img_des + "/" + image[:image.find(".")] + ".jpg" ,img,cmap="gray")
        else:
            print("skip",image)