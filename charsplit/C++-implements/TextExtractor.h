#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image_write.h"
#include<string>
#include<iostream>
#include<fstream>
#include <sstream>

#define ALLOW_DEBUG_MSG true

/*************************************************************************\
*                                                                        *\   
*                           Text Extractor                               *\
*           This can help to to split alphaberts from an image.          *\
*                    by Kanch at http://akakanch.com                     *\
*                         kanchisme@gmail.com                            *\
*                                                                        *\
*                                                                        *\
*************************************************************************\/

/* Basic types */
typedef uint8_t BASE;
typedef BASE*** MATRIX;
typedef BASE** ROW;
typedef BASE** COLUMN;
typedef BASE* PIXEL;
typedef BASE CHANNEL;
typedef MATRIX  IMAGE;

/* struct stores image basic information */
struct ImageData{
    int width;
    int height;
    int channel;
};

/* struct stores image matrix and its basic information */
struct ImagePack{
    IMAGE image;
    ImageData properties;
};

/* ==============================================Helper Function============================================ */

/*Transfer anyvalue to string, for compatiable of Android app.
source:https://stackoverflow.com/questions/22774009/android-ndk-stdto-string-support*/
template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

/* used to display debug message, 
you can turn it off by setting macro ALLOW_DEBUG_MSG to 0 */
template <typename T>
void klog(T msg,const bool newline=true){
    if(ALLOW_DEBUG_MSG){
        std::cout<<">>>>>"<<msg;
        if(newline){
            std::cout<<std::endl;
        }
    }
}

/* Calucate average of a given array (row vector) */
BASE avg(const PIXEL p,const uint8_t channels){
    int sum = 0;
    for(int i=0;i<channels;i++){
        sum+=p[i];
    }
    return sum/channels;
}
/* sum up an array */
long sum(const PIXEL p,const uint8_t channels){
    long sum = 0;
    for(int i=0;i<channels;i++){
        sum+=p[i];
    }
    return sum;
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

/* perform mean normailize to pictures */
const IMAGE normalize(IMAGE image,const ImageData& id){
    long sumx = 0;
    for(int i=0;i<id.height;i++){
        for(int j=0;j<id.width;j++){
            sumx += sum( image[i][j],id.channel );
        }
    }
    BASE avgv = BASE(sumx/(id.height*id.width));
    for(int i =0;i<id.height;i++){
        for(int j=0;j<id.width;j++){
            try{
                image[i][j][0] -= avgv;
            }catch(...){
                klog(i,false);
                klog(j,false);
            }
        }
    }
    return image;
}

/* this function can set all pixels which values large than a specified value to 1 */
const IMAGE set_toOne(IMAGE image,const ImageData& id,const uint8_t threshold=0){
    for(int i=0;i<id.height;i++){
        for(int j=0;i<id.width;j++){
            try{
                klog(i,false);
                klog(j,false);
                if( image[i][j][0] >= threshold ){
                        image[i][j][0] = 1;
                }
            }catch(...){
                klog(i,false);
                klog(j,false);
            }
        }
    }
    return image;
}

/* Tranfera row vector into a matrix */
const IMAGE to_Martix(uint8_t* img,const ImageData & id){
    
    MATRIX mat = new COLUMN[id.height];

    for(int i= 0;i<id.height;i++){
        ROW row = new PIXEL[id.width];
        for(int j=0;j<id.width;j++){
            PIXEL pixel = new CHANNEL[id.channel];
            for(int c=0;c<id.channel;c++){
                pixel[c] = int(img[ i*id.width + j +c ]);
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
    klog("normalizing...");
    grayscaleimgpack.image = normalize(grayscaleimgpack.image,grayscaleimgpack.properties);
    klog("set to 1...");
    grayscaleimgpack.image = set_toOne(grayscaleimgpack.image,grayscaleimgpack.properties);
    klog("save normailed to file");
    save2File(grayscaleimgpack.image ,grayscaleimgpack.properties,"normalized.txt");
    klog("start scanning on y...");

    klog("start scanning on x...");

    klog("split sentence...");


    klog("split alphaberts...");
    
    
}