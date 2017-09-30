#include<iostream>
#include<string>
#include"TextExtractor.h"

using namespace std;


int main(int argc,char**argv){
    cout<<">>Reading image"<<endl;
    int width, height, bpp;
    
    uint8_t* rgb_image = stbi_load("data/test.jpg", &width, &height, &bpp, 1);

    ImageData a = {width,height,bpp};
    klog(width);
    klog(height);
    klog(bpp);
    IMAGE image =  to_Martix(rgb_image,a);
    IMAGE subimage = sliceSubMatrix3D(image,a,50,60,90,100);
    ImageData b = {10,10,1};
    ImagePack2D i2d = depixelize(subimage,b);
    printMatrix(nullptr,i2d.properties,i2d.image);
    string npstr = numpylize(nullptr,i2d.properties,i2d.image);
    save_string(npstr,"cache/numpylizestr.txt");
    extractText(rgb_image,a);
    cout<<">>width:"<<width<<"\theight:"<<height<<"\tbpp:"<<bpp<<endl;
    stbi_image_free(rgb_image);

    //stbi_write_jpg("image2.jpg", width, height, 3, rgb_image, width*3);
    
    return 0;
}