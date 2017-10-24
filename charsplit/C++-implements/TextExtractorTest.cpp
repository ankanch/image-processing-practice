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
    IMAGE image =  to_Martix(rgb_image,a);
    IMAGE subimage = sliceSubMatrix3D(image,a,50,60,90,100);
    ImageData b = {10,10,1};
    ImagePack2D i2d = depixelize(subimage,b);
    printMatrix(nullptr,i2d.properties,i2d.image);
    string npstr = numpylize(nullptr,i2d.properties,i2d.image);
    save_string(npstr,"cache/numpylizestr.txt");
    
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

    //test_eigen();
    //Image x = to_EigenMatrixXd(subimage,b);
    //cout<<x<<endl;
    
    return 0;
}