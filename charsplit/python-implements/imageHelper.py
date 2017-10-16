import numpy as np
import cv2
from matplotlib import pyplot as plt

def expandToSquare(img):
    """
    Expand an image to a square one.
    
    `img` should be a numpy array. 

    The length and width will be the max value of the raw image width or height.  
    """
    maxp = max(img.shape)
    minp = min(img.shape)
    need = maxp - minp
    cc = np.zeros([maxp,need])
    xi = None
    if maxp == img.shape[0]:
        xi = np.concatenate((img,cc),axis=1)
    else:
        xi = np.concatenate((img,cc),axis=0)
    return xi

def normalizeImage(img,threshold=0):
    """
    Normalize an image by its mean.
    
    `img` should be an numpy array. 

    pixel value less than `threshold` will be set to zero.
    """
    img -= np.mean(img)
    img[img<=threshold] = 0
    return img


def scaleSquareImage(img,size):
    """
    Scale an square image to a specified size, with the size of  size*size. 
    """
    #  keepaspect ratio
    r = size / img.shape[1]
    dim = (size, int(img.shape[0] * r))
    
    # perform the actual resizing of the image and show it
    resized = cv2.resize(img, dim, interpolation = cv2.INTER_AREA)
    return resized

def grayScale(img):
    """
    transfer an image into grayscale.
    """
    img[img<0]=0
    return  cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

def pixelize(img,keneral=4):
    sizex = keneral*keneral
    img = grayScale(img)
    #fig = plt.figure()
    #fig.add_subplot(1,2,1)
    #plt.imshow(img)
    print(img.shape)
    tb = []
    rowbase = 0
    colbase = 0
    colcount = img.shape[1]
    rowcount = img.shape[0]
    loopscount = int(img.size/sizex)
    for i in range(0,loopscount):
        unit = img[rowbase:rowbase+keneral,colbase:colbase+keneral]
        if colbase+keneral != colcount:
            colbase+=keneral
        else:
            colbase = 0
            rowbase+=keneral
        tb.append(max(unit.reshape(sizex)))
    blurimg = (np.asarray(tb)).reshape( int(img.shape[0]/keneral),int(img.shape[1]/keneral))
    print(blurimg.shape)
    #fig.add_subplot(1,2,2)
    #plt.imshow(blurimg)
    #plt.show()
    return blurimg

def deleteBackground(img,threshold=60):
    """
    Delete background of an picture , as well as pixels with value under mode value + threshold.
    """
    print(img.shape)
    reimg = img.reshape( img.size,1)
    u, indices = np.unique(reimg, return_inverse=True)
    mode =  u[np.argmax(np.bincount(indices))]
    reimg[reimg > mode - threshold] = 0
    return reimg.reshape(img.shape)

def extractText(img,des):
    """
    This function will extract all text from a grayscale image. And save it to the `des` path.
    """
    try:
        img = grayScale(img)
    except Exception as e:
        print("already gray scale image")
    # scan on y axis first to extract row out
    img[img > 200] = 255    # normailze
    y_nzl = np.asarray([ len(row)-row.tolist().count(255) for row in img ])
    y_nzl[y_nzl > 0] = 1
    # split row now
    row_boundary_list = []
    for i,y in enumerate(y_nzl[:-1]):
        if (y == 0 and y_nzl[i+1] == 1):
            row_boundary_list.append(i+1)
        elif (y == 1 and y_nzl[i+1] == 0):
            row_boundary_list.append(i)
    row_image = []
    for i in range(0,len(row_boundary_list),2):
        row_image.append(img[row_boundary_list[i]:row_boundary_list[i+1],:])
    plt.imshow(row_image[0])
    # start extract alphaberts now
    alphaberts = []
    for row in row_image:
        col_image = []
        col_boundary_list = []
        x_nzl = np.asarray([ len(col)-col.tolist().count(255) for col in row.T ])
        x_nzl[x_nzl > 0] = 1
        plt.plot(x_nzl)
        #plt.show()
        start = end = 0
        for i,x in enumerate(x_nzl[:-1]):
            if x==0 and x_nzl[i+1] == 1:
                start = i
            if x == 1 and x_nzl[i+1] == 0:
                end = i+1
                alp = row[:,start:end]
                start = end
                col_image.append(alp)
                #plt.imshow(alp,cmap='gray')
                #plt.show()
        alphaberts.extend(col_image)

    plt.show()

    for i,img in enumerate(alphaberts):
        plt.imsave(des+str(i)+".jpg",img)
    return alphaberts

def delete_background(img):
    try:
        img = grayScale(img)
    except Exception as e:
        print("already gray scale image")
    # scan on y axis first to extract row out
    PIXEL_JUMP = 10
    row_jump_count = []
    for row in img:
        px = row.copy()
        px[px>0] -= px[0]
        jump = [ x for x in px if x>PIXEL_JUMP ]
        row_jump_count.append(jump)
    




if __name__ == "__main__":
    IMG_PATH = "./data/test.jpg"
    IMG_PATH = "./data/girl.jpg"
    testimg = plt.imread(IMG_PATH)
    #plt.show()
    #extractText(testimg,"./data/testresult/")
    fig = plt.figure()
    fig.add_subplot(2,3,1)
    plt.title(" raw image ")
    plt.imshow( testimg )
    fig.add_subplot(2,3,2)
    plt.title(" kernal size = 2 ")
    plt.imshow( pixelize(testimg,2) )
    fig.add_subplot(2,3,3)
    plt.title(" kernal size = 4 ")
    plt.imshow( pixelize(testimg,4) )
    fig.add_subplot(2,3,4)
    plt.title(" kernal size = 8 ")
    plt.imshow( pixelize(testimg,8) )
    fig.add_subplot(2,3,5)
    plt.title(" kernal size = 16 ")
    plt.imshow( pixelize(testimg,16) )
    fig.add_subplot(2,3,6)
    plt.title(" kernal size = 20 ")
    plt.imshow( pixelize(testimg,20) )
    plt.show()
    