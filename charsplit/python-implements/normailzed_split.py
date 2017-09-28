from matplotlib import pyplot as plt
import imageHelper
import numpy as np
import cv2

rawimg = cv2.imread("./test.jpg")
#rawimg = rawimg - int(np.mean(rawimg))
#rawimg[rawimg < 50] = 0
fig = plt.figure()
fig.add_subplot(2,3,1)
plt.title("raw image")
plt.imshow(rawimg)

fig.add_subplot(2,3,2)
plt.title("grey scale image")
grayscaleimg = cv2.cvtColor(rawimg,cv2.COLOR_BGR2GRAY)
grayscaleimg = grayscaleimg - int(np.mean(grayscaleimg))
grayscaleimg[grayscaleimg < 50] = 0
plt.imshow(grayscaleimg,cmap='gray')


# counting non-zero value by row , axis y
row_nz = []
for row in grayscaleimg.tolist():
    row_nz.append(len(row) - row.count(0))
fig.add_subplot(2,3,3)
plt.title("non-zero values on y (by row)")
plt.plot(row_nz)


# counting non-zero value by column, x axis
col_nz = []
for col in grayscaleimg.T.tolist():
    col_nz.append(len(col) - col.count(0))
fig.add_subplot(2,3,4)
plt.title("non-zero values on y (by col)")
plt.plot(col_nz)


##### start split
# first find upper and lower boundary of y (row)
fig.add_subplot(2,3,5)
plt.title("y boudary deleted")
upper_y = 0
for i,x in enumerate(row_nz):
    if x != 0:
        upper_y = i
        break
lower_y = 0
for i,x in enumerate(row_nz[::-1]):
    if x!=0:
        lower_y = len(row_nz) - i
        break
sliced_y_img = grayscaleimg[upper_y:lower_y,:]
sliced_y_img[sliced_y_img < 50] = 0
plt.imshow(sliced_y_img)

# show histgram
"""
fig.add_subplot(2,3,6)
plt.title("hist")
plt.hist(sliced_y_img)
"""

# then we find left and right boundary of every digital (x, on column)
column_boundary_list = []
record = False
for i,x in enumerate(col_nz[:-1]):
    if (col_nz[i] == 0 and col_nz[i+1] != 0) or col_nz[i] != 0 and col_nz[i+1] == 0:
        column_boundary_list.append(i+1)
img_list = []
xl = [ column_boundary_list[i:i+2] for i in range(0,len(column_boundary_list),2) ]
for x in xl:
    img_list.append( sliced_y_img[:,x[0]:x[1]] )
# del invalid image
img_list = [ x for x in img_list if x.shape[1] > 5 ]
# show image
fig = plt.figure()
for i,img in enumerate(img_list):
    fig.add_subplot(3,4,i+1)
    plt.imshow(img)
    # expand to max-size
    xi = imageHelper.expandToSquare(img)
    xi = imageHelper.scaleSquareImage(xi,28)
    plt.imsave("./result/zc%s.jpg"%i, xi ,cmap='gray')
    plt.imsave("./result/%s.jpg"%i, img )
plt.show()
    

