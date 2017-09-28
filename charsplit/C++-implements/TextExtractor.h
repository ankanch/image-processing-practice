#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image_write.h"
#include<string>
#include<iostream>
#include<fstream>
#include <sstream>

#define ALLOW_DEBUG_MSG true

typedef uint8_t BASE;
typedef BASE*** MATRIX;
typedef BASE** ROW;
typedef BASE** COLUMN;
typedef BASE* PIXEL;
typedef BASE CHANNEL;
typedef MATRIX  IMAGE;

struct ImageData{
    int width;
    int height;
    int channel;
};

struct ImagePack{
    IMAGE image;
    ImageData properties;
};

/* ==============================================Helper Function============================================ */

/*https://stackoverflow.com/questions/22774009/android-ndk-stdto-string-support*/
template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}
template <typename T>
void klog(T msg,const bool newline=true){
    if(ALLOW_DEBUG_MSG){
        std::cout<<">>>>>"<<msg;
        if(newline){
            std::cout<<std::endl;
        }
    }
}

BASE avg(const PIXEL p,const uint8_t channels){
    BASE sum = 0;
    for(int i=0;i<channels;i++){
        sum+=p[i];
    }
    return sum/channels;
}

/* save image to file */
void save2File(const IMAGE image,const ImageData &id,const std::string des="image.txt"){
    std::string data = "";
    for(int i=0;i<id.height;i++){
        std::string rowstr = "";
        for(int j=0;j<id.width;j++){
            std::string pixel = "";
            for(int c=0;c<id.channel;c++){
                pixel += to_string(int(image[i][j][c])) + ",";
            }
            pixel = "[" + pixel.substr(0,pixel.length()-1) + "]";
            rowstr += pixel + ",";
        }
        rowstr = "[" + rowstr.substr(0,rowstr.length()-1) + "]";
        data += rowstr + "\r\n";
    }
    data = "[" + data.substr(0,data.length()-1) + "]";
    std::fstream fs;
    fs.open(des.c_str(),std::ios_base::out);
    fs<<data;
    fs.close();
}

/* Tranfera row vector into a matrix */
const IMAGE to_Martix(uint8_t* img,const ImageData & id){
    
    MATRIX mat = new COLUMN[id.height];

    for(int i= 0;i<id.height;i++){
        ROW row = new PIXEL[id.width];
        for(int j=0;j<id.width;j++){
            PIXEL pixel = new CHANNEL[id.channel];
            for(int c=0;c<id.channel;c++){
                pixel[c] = int(img[ i*id.width + j*id.width + c ]);
            }
            row[j] = pixel;
        }
        mat[i] = row;
    }
    return mat;
}

/* Grayscale an image */
/* only 1 color channel will be left after this function */
const ImagePack to_grayScale(const IMAGE image,const ImageData& id){
    IMAGE grayscale = new COLUMN[id.height];

    for(int i=0;i<id.height;i++){
        ROW row = new PIXEL[id.width];
        for(int j=0;j<id.width;j++){
            PIXEL p = new CHANNEL[1]{ avg( image[i][j] , id.channel ) };
            row[j] = p;
        }
        grayscale[i] = row;
    }
    ImagePack result = {grayscale,{id.width,id.height,1}};
    return result;
}


/* ==============================================Extract text fucntion here================================== */

void extractText(uint8_t* img,const ImageData & id){
    klog("converting to matrix...");
    const IMAGE image = to_Martix(img,id);
    klog("to grayscale image...");
    ImagePack grayscaleimgpack = to_grayScale(image,id);
    save2File(grayscaleimgpack.image,grayscaleimgpack.properties,"image2.txt");
    
    
}