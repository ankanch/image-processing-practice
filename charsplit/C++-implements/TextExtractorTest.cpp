#include<iostream>
#include<string>
#include"TextExtractor.h"


using namespace std;

void save_stringT(const std::string data,const std::string path){
        std::fstream fs;
        fs.open(path.c_str(),std::ios_base::out);
        fs<<data;
        fs.close();
}

int main(int argc,char**argv){
    cout<<">>Reading image"<<endl;
    int width, height, bpp;
    
    uint8_t* rgb_image = stbi_load("data/template.jpg", &width, &height, &bpp, 3);

    ImageData a = {width,height,bpp};
    klog(width);
    klog(height);
    klog(bpp);
    
    // extract single alphaberts image
    DLISTIMAGEPACK p = extractText(rgb_image,a);
    int sum = 0;
    string ss = strinfy(p,sum);
    save_stringT(ss,"dta.txt");
    cout<<">>width:"<<width<<"\theight:"<<height<<"\tbpp:"<<bpp<<"\tsplit_sum="<<sum<<endl;
    stbi_image_free(rgb_image);

    // recognize
    string result = recognize(p);
    cout<<"result:\t"<<result<<endl;

    
    return 0;
}