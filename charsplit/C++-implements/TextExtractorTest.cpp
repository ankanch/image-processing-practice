#include<iostream>
#include<string>
#include"TextExtractor.h"

using namespace std;





int main(int argc,char**argv){
    cout<<">>Reading image"<<endl;
    int width, height, bpp;
    
    uint8_t* rgb_image = stbi_load("data/test.jpg", &width, &height, &bpp, 3);
    //uint8_t* rgb_image = stbi_load("test.jpg", &width, &height, &bpp, 3);
    ImageData a = {width,height,bpp};
    klog(width);
    klog(height);
    klog(bpp);
    IMAGE image =  to_Martix(rgb_image,a);
    klog("saving to image.txt...");
    save2File( image,a);
    extractText(rgb_image,a);
    cout<<">>width:"<<width<<"\theight:"<<height<<"\tbpp:"<<bpp<<endl;
    stbi_image_free(rgb_image);

    //stbi_write_jpg("image2.jpg", width, height, 3, rgb_image, width*3);
    
    return 0;
}