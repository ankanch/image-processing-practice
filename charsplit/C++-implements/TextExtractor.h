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

#define ALLOW_DEBUG_MSG true

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
typedef std::vector<ImagePack> LISTIMAGEPACK;

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
void printMatrix(const MATRIX mat,const ImageData&id){
    std::cout<<"[";
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
    std::cout<<"]"<<std::endl;
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
const IMAGE set_largeThan2Value(IMAGE image,const ImageData& id,const uint8_t value,const int threshold){
    for(int i=0;i<id.height;i++){
        for(int j=0;j<id.width;j++){
                if( int(image[i][j][0]) > threshold ){
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
    save2File(grayscaleimgpack.image ,grayscaleimgpack.properties,"grayscale.txt");
    klog("set to 1...");
    grayscaleimgpack.image = set_lessThan2Value(grayscaleimgpack.image,grayscaleimgpack.properties,1,200);
    grayscaleimgpack.image = set_largeThan2Value(grayscaleimgpack.image,grayscaleimgpack.properties,0,200);
    klog("save normailed to file");
    save2File(grayscaleimgpack.image ,grayscaleimgpack.properties,"normalized.txt");
    klog("start scanning on y...");
    int *ones_on_y = new int[grayscaleimgpack.properties.height];
    for(int i=0;i<grayscaleimgpack.properties.height;i++){
        int buf = 0;
        for(int j=0;j<grayscaleimgpack.properties.width;j++){
            buf += grayscaleimgpack.image[i][j][0];
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
                                    sentence_boundary[i],sentence_boundary[2*i+1],-1,-1);
        im.properties.width = grayscaleimgpack.properties.width;
        im.properties.height = sentence_boundary[2*i+1] - sentence_boundary[i];
        im.properties.channel = 1;
        sentences.push_back(im);
        klog("line split.");
        //printMatrix(im.image,im.properties);
    }
    klog("start scanning on x...");
    klog("split line alphaberts...");


    klog("split alphaberts...");
    
    
    klog("deleting resource...");
    delete[] ones_on_y;
    deleteMatrix(image,id);
    deleteMatrix(grayscaleimgpack.image,grayscaleimgpack.properties);
}