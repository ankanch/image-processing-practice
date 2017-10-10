#ifndef DIGITSFINDER_TEXTEXTRACTOR_H
#define DIGITSFINDER_TEXTEXTRACTOR_H

#endif //DIGITSFINDER_TEXTEXTRACTOR_H
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image_write.h"
#include<string>
#include<iostream>
#include<fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#define ALLOW_DEBUG_MSG false
#define ALLOW_DEBUG_FILE_STORAGE false

/*****************************************************************************\
***                                                                        ***\   
***                          Text Extractor                                ***\
***           This can help to to split alphaberts from an image.          ***\
***                    by Kanch at http://akakanch.com                     ***\
***                         kanchisme@gmail.com                            ***\
***                                                                        ***\
***                                                                        ***\
******************************************************************************\

/* Basic types */
typedef uint8_t BASE;
typedef BASE*** MATRIX;
typedef BASE** ROW;
typedef BASE** COLUMN;
typedef BASE* PIXEL;
typedef BASE* ALPHABERTS;
typedef BASE CHANNEL;
typedef MATRIX  IMAGE;
typedef std::vector<int> LIST;

typedef ROW IMAGE2D;
typedef PIXEL ROW2D;

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
struct ImagePack2D{
    IMAGE2D image;
    ImageData properties;
};
typedef std::vector<ImagePack> LISTIMAGEPACK;
typedef std::vector<ImagePack2D> LISTIMAGEPACK2D;
typedef std::vector<LISTIMAGEPACK> DLISTIMAGEPACK;

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

/* print Matrix to console */
/* can print pixelize and depixelize */
void printMatrix(const MATRIX mat3d,const ImageData&id,const IMAGE2D mat2d=nullptr){
    if(ALLOW_DEBUG_FILE_STORAGE){
        std::cout<<"[";
        if(id.channel > 0){
            MATRIX mat = mat3d;
            for(int i =0;i<id.height;i++){
                std::cout<<"[";
                for(int j=0;j<id.width;j++){
                    std::cout<<"[";
                    for(int c=0;c<id.channel;c++){
                        std::cout<< int(mat[i][j][c]) <<",";
                    }
                    std::cout<<"\b] " ;
                }
                std::cout<<"\b] "<<std::endl;
            }
        }else{
            IMAGE2D mat = mat2d;
            for(int i =0;i<id.height;i++){
                std::cout<<"[";
                for(int j=0;j<id.width;j++){
                    std::cout<< int(mat[i][j]) <<",";
                }
                std::cout<<"\b] "<<std::endl;
            }  
        }
        std::cout<<"]"<<std::endl;
    }
}

/* perform mean normailize to pictures */
const IMAGE normalize(IMAGE image,const ImageData& id){
    long long sumx = 0;
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
const IMAGE set_largeThan2Value(IMAGE image,const ImageData& id,const uint8_t value,const int threshold){
    for(int i=0;i<id.height;i++){
        for(int j=0;j<id.width;j++){
                if( int(image[i][j][0]) >= threshold ){
                        image[i][j][0] = value;
                }
        }
    }
    return image;
}
/* this function can set all pixels which values less than a specified value to 1 */
const IMAGE set_lessThan2Value(IMAGE image,const ImageData& id,const uint8_t value,const int threshold){
    for(int i=0;i<id.height;i++){
        for(int j=0;j<id.width;j++){
                if( int(image[i][j][0]) <= threshold ){
                        image[i][j][0] = value;
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

/* get a slice of an matrix, like a[:,:] in Python */
/* return `nullptr` if invailed data dectected */
/* double nagative value in row or column stands for all select all rows and columns  */
const MATRIX sliceSubMatrix3D(MATRIX mat,const ImageData&id,const int rs=0,const int re=0,const int cs=0,const int ce=0){
    int row_start,row_end,col_start,col_end;
    row_start = rs;
    row_end = re;
    col_start = cs;
    col_end = ce;

    if( rs > re || cs > ce || ( (rs>=0)^(re>=0) ) || ( (cs>=0)^(ce>=0) ) ){
        return nullptr;
    }
    if( row_end < 0 && row_start < 0 ){     // all rows
        row_start = 0;
        row_end = id.height;
    }
    if( col_end < 0 && col_start < 0 ){     // all columns
        col_start = 0;
        col_end = id.width;
    }
    int height = row_end - row_start;
    int width = col_end - col_start;
    MATRIX submatrix = new COLUMN[height];
    //klog("height:" + to_string(height)+ "\twidth:" + to_string(width));
    int colbase = 0;
    for(int i=0;i<id.height;i++){
        if( i >= row_start && i < row_end){
            //klog("in row " + to_string(i));
            ROW row = new PIXEL[width];
            int base = 0;
            for(int j=0;j<id.width;j++){
                if( j >=col_start && j < col_end  ){
                    //klog("col-"+to_string(j));
                    PIXEL pixel = new BASE[id.channel];
                    for(int c=0;c<id.channel;c++){
                        pixel[c] = mat[i][j][c];
                    }
                    row[base] = pixel;
                    base++;
                }
            }
            submatrix[colbase] = row;
            colbase++;
        }
    }
    return submatrix;
}

/* Delete resource taken up by matrix */
void deleteMatrix(MATRIX mat,const ImageData& id){
    for(int i=0;i<id.height;i++){
        for(int j=0;j<id.width;j++){
            delete[] mat[i][j];
        }
        delete[] mat[i];
    }
    delete[] mat;
}

/* Grayscale an image */
/* only 1 color channel will be left after this function */
const ImagePack to_grayScale(const IMAGE image,const ImageData& id){
    IMAGE grayscale = new COLUMN[id.height];

    for(int i=0;i<id.height;i++){
        ROW row = new PIXEL[id.width];
        for(int j=0;j<id.width;j++){
            long buf =  (image[i][j][0]*299 + image[i][j][1]*587 + image[i][j][2]*114 + 500) / 1000;
            PIXEL p = new CHANNEL[1]{ BASE(buf) };
            row[j] = p;
        }
        grayscale[i] = row;
    }
    ImagePack result = {grayscale,{id.width,id.height,1}};
    return result;
}
/* make pixel in image from a array to single BASE variable */
const ImagePack2D depixelize(const IMAGE image,const ImageData& id){
    ImagePack2D i2d;
    i2d.properties.width = id.width;
    i2d.properties.height = id.height;
    i2d.properties.channel = 0;
    if(id.channel != 1){
        i2d.image = nullptr;
        return i2d;
    }
    IMAGE2D buf = new ROW2D[id.height];
    for(int i=0;i<id.height;i++){
        ROW2D row = new BASE[id.width];
        for(int j=0;j<id.width;j++){
            row[j] = image[i][j][0];
            //klog(int(row[j]),false);
        }
        buf[i] = row;
    }
    i2d.image = buf;
    return i2d;
}

/* make an matrix avaliable to show as image in python */
const std::string numpylize(const IMAGE mat3d,const ImageData& id,const IMAGE2D mat2d=nullptr){
    klog("numpylizing...");
    std::string numpylizedstring = "";
    if(id.channel > 0){
        MATRIX mat = mat3d;
        for(int i =0;i<id.height;i++){
            numpylizedstring += "[";
            for(int j=0;j<id.width;j++){
                numpylizedstring += "[";
                for(int c=0;c<id.channel;c++){
                    numpylizedstring += to_string(int(mat[i][j][c])) + ",";
                }
                numpylizedstring.pop_back();
                numpylizedstring += "],";
            }
            numpylizedstring.pop_back();
            numpylizedstring += "],";
        }
    }else{
        IMAGE2D mat = mat2d;
        for(int i =0;i<id.height;i++){
            numpylizedstring += "[";
            for(int j=0;j<id.width;j++){
                numpylizedstring += to_string(int(mat[i][j])) + ",";
            }
            numpylizedstring.pop_back();
            numpylizedstring += "],";
        }  
    }
    numpylizedstring.pop_back();
    numpylizedstring += "]";
    return "[" + numpylizedstring;
}

/* save string to file */
void save_string(const std::string data,const std::string path){
    if(ALLOW_DEBUG_FILE_STORAGE){
        klog("saving string...",false);
        std::fstream fs;
        fs.open(path.c_str(),std::ios_base::out);
        fs<<data;
        fs.close();
        klog(".done.");
    }
}

/* type cast: from C++ native int to uint8_t */
uint8_t * cast2uint8_t(const int * src,const int len){
    uint8_t * des = new uint8_t[len];
    for(int i=0;i<len;i++){
        des[i] = src[i];
    }
    return des;
}

/* strinfy the result list */
/* format: [array of image data]@width@height<!> */
/* <!> this symbol is used to split differnt images */
const std::string strinfy(DLISTIMAGEPACK dpack,int&sum){
    std::string result = "";
    for(int i=0;i<dpack.size();i++){    // extract line container
        LISTIMAGEPACK linecontainer = dpack[i];
        for(int j=0;j<linecontainer.size();j++){     // extract alpberts image on every line
            // we're processing ImagePack here
            std::string buf = "[";
            std::string properties = to_string(linecontainer[j].properties.width) + "@"
                                    + to_string(linecontainer[j].properties.height) + "<!>";
            //start processing evert channel's pixel now
            for(int z=0;z<linecontainer[j].properties.height;z++){
                for(int l=0;l<linecontainer[j].properties.width;l++){
                    buf += to_string(int(linecontainer[j].image[z][l][0]));
                    buf += "," ;
                }
            }
            buf += buf.substr(0,buf.length() - 1) + "]@";
            buf += properties;
            result += buf;
            sum++;
        }
    }
    return result;
}

/* ==============================================Extract text fucntion here================================== */

DLISTIMAGEPACK extractText(uint8_t* img,const ImageData & id){
    klog("converting to matrix...");
    const IMAGE image = to_Martix(img,id);
    klog("saving matrix...");
    save_string( numpylize(image,id) ,"cache/raw_image.txt");
    klog("to grayscale image...");
    ImagePack grayscaleimgpack = to_grayScale(image,id);
    ImagePack2D a = depixelize(grayscaleimgpack.image ,grayscaleimgpack.properties);
    save_string(numpylize(nullptr,a.properties,a.image),"cache/numpylizestr.txt");
    klog("set to 1...");
    grayscaleimgpack.image = normalize(grayscaleimgpack.image,grayscaleimgpack.properties);
    grayscaleimgpack.image = set_lessThan2Value(grayscaleimgpack.image,grayscaleimgpack.properties,0,50);
    grayscaleimgpack.image = set_largeThan2Value(grayscaleimgpack.image,grayscaleimgpack.properties,0,185);
    klog("save normailzed picture...");
    a = depixelize(grayscaleimgpack.image ,grayscaleimgpack.properties);
    save_string(numpylize(nullptr,a.properties,a.image),"cache/normalized.txt");
    klog("save normailed to file");
    klog("start scanning on y...");
    int *ones_on_y = new int[grayscaleimgpack.properties.height];
    for(int i=0;i<grayscaleimgpack.properties.height;i++){
        int buf = 0;
        for(int j=0;j<grayscaleimgpack.properties.width;j++){
            if(grayscaleimgpack.image[i][j][0] > 0){
               ++buf;
            }
        }
        ones_on_y[i] = buf;
    }
    klog("split line...");
    LIST sentence_boundary;
    int start,end;
    for(int i=0;i<grayscaleimgpack.properties.height-1;i++){
        //klog("ones_on_y="+to_string(ones_on_y[i])+"\t@"+to_string(i));
        if( ( ones_on_y[i] == 0) && (ones_on_y[i+1] > 0) ){ //top boundary of sentence
            start = i;
        }else if((ones_on_y[i] > 0) && (ones_on_y[i+1] == 0)){   // bottom boundary of sentence
            end = i+1;
            sentence_boundary.push_back(start);
            sentence_boundary.push_back(end);
        }
    }
    int linecount = sentence_boundary.size()/2;
    klog("linecout="+to_string(linecount) + "\tsb.size()="+to_string(sentence_boundary.size()));
    LISTIMAGEPACK sentences;
    for(int i=0;i<linecount;i++){
        ImagePack im;
        im.image = sliceSubMatrix3D(grayscaleimgpack.image,grayscaleimgpack.properties,
                                    sentence_boundary[2*i],sentence_boundary[2*i+1],-1,-1);
        im.properties.width = grayscaleimgpack.properties.width;
        im.properties.height = sentence_boundary[2*i+1] - sentence_boundary[2*i];
        im.properties.channel = 1;
        sentences.push_back(im);
        ImagePack2D d2line = depixelize(im.image,im.properties);
        save_string( numpylize( nullptr ,d2line.properties, d2line.image ) ,"cache/line" + to_string(i+1) + ".txt");
    }
    klog("Sentences list size=" + to_string(sentences.size()) + "\tstart scanning on x...");
    DLISTIMAGEPACK alphaberts;
    for(int i =0;i<sentences.size();i++){
        LISTIMAGEPACK alpha;
        klog("scan on sentence... on line " + to_string(i+1));
        ImagePack sen = sentences[i];
        LIST zeros_on_sentence;
        for(int j=0;j<sen.properties.width;j++ ){
            int buf=0;
            for(int c=0;c<sen.properties.height;c++){
                if(sen.image[c][j][0] > 0){
                    ++buf;
                }
            }
            zeros_on_sentence.push_back(buf);
        }
        klog("split line alphaberts....");
        start = end = 0;
        klog("\tsb.size()="+to_string(zeros_on_sentence.size()));
        for(int j=0;j<zeros_on_sentence.size();j++){
            if( ( zeros_on_sentence[j] == 0) && (zeros_on_sentence[j+1] > 0) ){ //left boundary of alphaberts
                start = j;
            }else if((zeros_on_sentence[j] > 0) && (zeros_on_sentence[j+1] == 0)){   // right boundary of alplaberts
                end = j+1;
                //klog("scling... start:" + to_string(start) + "\tend:" + to_string(end) );
                IMAGE imx = sliceSubMatrix3D(sen.image,sen.properties,-1,-1,start,end);
                ImagePack imp = {imx,{end-start,sen.properties.height,1}};
                //printMatrix(imp.image,imp.properties);
                //klog("depixelize");
                ImagePack2D d2line = depixelize(imp.image,imp.properties);
                //klog("saving");
                save_string( numpylize( nullptr ,d2line.properties, d2line.image ) ,"cache/alp_"+ to_string(i+1) + to_string(j+1) + ".txt");
                alpha.push_back(imp);
            }
        }
        alphaberts.push_back(alpha);
    }

    //delete resource
    klog("deleting resource...");
    delete[] ones_on_y;
    deleteMatrix(image,id);
    deleteMatrix(grayscaleimgpack.image,grayscaleimgpack.properties);

    return alphaberts;
}