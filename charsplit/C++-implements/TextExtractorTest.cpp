#include<iostream>
#include<string>
#include"TextExtractor.h"

using namespace std;





int main(int argc,char**argv){
    cout<<">>Reading image"<<endl;
    int width, height, bpp;
    
    uint8_t* rgb_image = stbi_load("test.jpg", &width, &height, &bpp, 3);
    ImageData a = {width,height,bpp};
    klog("saving to image.txt...");
    save2File( to_Martix(rgb_image,a) ,a);
    extractText(rgb_image,a);
    cout<<">>width:"<<width<<"\theight:"<<height<<"\tbpp:"<<bpp<<endl;
    stbi_image_free(rgb_image);

    //stbi_write_jpg("image2.jpg", width, height, 3, rgb_image, width*3);
    
    return 0;
}