#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include<string>
#include<iostream>
#include<fstream>
#include <sstream>
#include<cmath>
#include <vector>
#include <algorithm>
#include"template.h"
#include"spellchecker.h"


// for debug
#ifndef ALLOW_DEBUG_MSG                 // set to show debug message (in console)
    #define ALLOW_DEBUG_MSG             false
#endif

#ifndef ALLOW_DEBUG_FILE_STORAGE        // set to save every image generated (including segmentation cache)
    #define ALLOW_DEBUG_FILE_STORAGE    false
#endif

static int debug_alp_count = 0;

/*****************************************************************************\
***                                                                        ***\   
***                    Text Extractor and Recognition                      ***\
***           This can help to to split alphaberts from an image.          ***\
***                    by Kanch at http://akakanch.com                     ***\
***                         kanchisme@gmail.com                            ***\
***                                                                        ***\
***                                                                        ***\
****************************************************************************\*/

/* ==============================================Types Defination============================================ */
#ifndef FEATURE_DEFENDED
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

    /* structures to stores Features */
    struct Features{
        std::vector<int> x_sum;
        std::vector<int> y_sum;
        std::vector<int> ninesampling;      // for nine sampling
        int circle;                         // record if there is a circle in the image
        int width;
        int height;
        double wh_ratio;
        std::string label;
    };


    typedef std::vector<ImagePack> LISTIMAGEPACK;
    typedef std::vector<ImagePack2D> LISTIMAGEPACK2D;
    typedef std::vector<LISTIMAGEPACK> DLISTIMAGEPACK;
    typedef std::vector<ImagePack>::iterator LISTIMAGEPACKITERATOR;
    typedef std::vector<ImagePack2D>::iterator LISTIMAGEPACK2DITERATOR;
    typedef std::vector<LISTIMAGEPACK>::iterator DLISTIMAGEPACKITERATOR;
#endif

void save_string(const std::string data,const std::string path);

/* ==============================================Helper Function============================================ */

/*Transfer anyvalue to string, for compatiable of Android app.
source:https://stackoverflow.com/questions/22774009/android-ndk-stdto-string-support*/
template <typename T>
std::string to_string(T value){
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

void pause(bool pause=true){
    while(pause){
        ;
    }
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

template <typename T>
void printVector(std::vector<T> v,std::string name=""){
    if(ALLOW_DEBUG_MSG){
        std::cout<<"Vector "<<name<<" : ";
        for(int i=0;i<v.size();++i){
            std::cout<<v[i]<<",";
        }
        std::cout<<std::endl;
    }
}

/* Calucate average of a given array (row vector) */
BASE avg(const PIXEL p,const uint8_t channels){
    int sum = 0;
    for(int i=0;i<channels;++i){
        sum+=p[i];
    }
    return sum/channels;
}
/* sum up an array */
long sum(const PIXEL p,const uint8_t channels){
    long sum = 0;
    for(int i=0;i<channels;++i){
        sum+=p[i];
    }
    return sum;
}



/* save image to file */
void save2File(const IMAGE image,const ImageData &id,const std::string des="image.txt"){
    std::string data = "";
    for(int i=0;i<id.height;++i){
        std::string rowstr = "";
        for(int j=0;j<id.width;++j){
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
    if(ALLOW_DEBUG_MSG){
        std::cout<<"[\n";
        if(id.channel > 0){
            MATRIX mat = mat3d;
            for(int i =0;i<id.height;++i){
                std::cout<<"[";
                for(int j=0;j<id.width;++j){
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
            for(int i =0;i<id.height;++i){
                std::cout<<"[";
                for(int j=0;j<id.width;++j){
                    std::cout<< int(mat[i][j]) <<",";
                }
                std::cout<<"\b] "<<std::endl;
            }  
        }
        std::cout<<"]"<<std::endl;
    }
}

/* perform mean normailize to pictures */
const IMAGE normalize(IMAGE image,const ImageData& id,BASE* mean){
    long long sumx = 0;
    for(int i=0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
            sumx += sum( image[i][j],id.channel );
        }
    }
    BASE avgv = BASE(sumx/(id.height*id.width));
    *mean = avgv;
    for(int i =0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
            image[i][j][0] -= avgv;
        }
    }
    return image;
}

/* this function can set all pixels which values large than a specified value to 1 */
const IMAGE set_largeThan2Value(IMAGE image,const ImageData& id,const uint8_t value,const int threshold){
    for(int i=0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
                if( int(image[i][j][0]) >= threshold ){
                        image[i][j][0] = value;
                }
        }
    }
    return image;
}
/* this function can set all pixels which values less than a specified value to 1 */
const IMAGE set_lessThan2Value(IMAGE image,const ImageData& id,const uint8_t value,const int threshold){
    for(int i=0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
                if( int(image[i][j][0]) <= threshold ){
                        image[i][j][0] = value;
                }
        }
    }
    return image;
}

/* this function will enhancing the image (maxiumize the gray pixel value) */
/* this function should be use only after the normailization and background deleted */
/* for one channel image */
const IMAGE enhanceImage(IMAGE image,const ImageData& id){
    for(int i=0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
            if(int(image[i][j][0])>0){
                image[i][j][0] = 255;
            }
        }
    }
    return image;
}

const IMAGE reverseImageBit(IMAGE image,const ImageData& id){
    for(int i=0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
            if(int(image[i][j][0]) == 0){
                image[i][j][0] = 1;
            }else{
                image[i][j][0] = 0;
            }
        }
    }
    return image;
}

/* Tranfera row vector into a matrix */
const IMAGE to_Martix(uint8_t* img,const ImageData & id){
    
    MATRIX mat = new COLUMN[id.height];

    for(int i= 0;i<id.height;++i){
        ROW row = new PIXEL[id.width];
        for(int j=0;j<id.width;++j){
            PIXEL pixel = new CHANNEL[id.channel];
            for(int c=0;c<id.channel;c++){
                pixel[c] = int(img[ 3*(i*id.width + j) +c ]);
            }
            row[j] = pixel;
        }
        mat[i] = row;
    }
    return mat;
}

/* set background to 1, text to 0 */
const IMAGE Thresholding(IMAGE& img,const ImageData& id){
    std::string hist = "";
    //first count 0 to 255 pixel value count
    std::vector<int> pixelcount(256);
    for(int i=0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
            ++pixelcount[ img[i][j][0] ];
        }
    }
    for(auto&a : pixelcount){
        hist += to_string(a);
        hist += ",";
    }
    save_string( "[" + hist.substr(0,hist.length() -1 ) + "]" ,"cache/hist.txt");
    // start split backgroud color and forecolor
    int minvalue = 10;
    for(std::vector<int>::iterator it=pixelcount.begin()+1;it<pixelcount.end();++it){
        if(*it >= pixelcount[0]){
            minvalue = std::distance(pixelcount.begin(),it)/5*4;
            break;
        }
    }
    // set background to 1, data to 1
    img = set_lessThan2Value(img,id,1,minvalue);  
    img = set_largeThan2Value(img,id,0,minvalue); 

    return img;
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
    int colbase = 0;
    for(int i=0;i<id.height;++i){
        if( i >= row_start && i < row_end){
            ROW row = new PIXEL[width];
            int base = 0;
            for(int j=0;j<id.width;++j){
                if( j >=col_start && j < col_end  ){
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
    for(int i=0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
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

    for(int i=0;i<id.height;++i){
        ROW row = new PIXEL[id.width];
        for(int j=0;j<id.width;++j){
            int buf =  int(image[i][j][0]*0.11 + image[i][j][1]*0.59 + image[i][j][2]*0.3 );
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
    for(int i=0;i<id.height;++i){
        ROW2D row = new BASE[id.width];
        for(int j=0;j<id.width;++j){
            row[j] = image[i][j][0];
        }
        buf[i] = row;
    }
    i2d.image = buf;
    return i2d;
}

/* make an matrix avaliable to show as image in python */
const std::string numpylize(const IMAGE mat3d,const ImageData& id,const IMAGE2D mat2d=nullptr){
    std::string numpylizedstring = "";
    if(ALLOW_DEBUG_FILE_STORAGE){
        if(id.channel > 0){
            MATRIX mat = mat3d;
            for(int i =0;i<id.height;++i){
                numpylizedstring += "[";
                for(int j=0;j<id.width;++j){
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
            for(int i =0;i<id.height;++i){
                numpylizedstring += "[";
                for(int j=0;j<id.width;++j){
                    numpylizedstring += to_string(int(mat[i][j])) + ",";
                }
                numpylizedstring.pop_back();
                numpylizedstring += "],";
            }  
        }
        numpylizedstring.pop_back();
    }
    numpylizedstring += "]";
    return "[" + numpylizedstring;
}

/* save string to file */
void save_string(const std::string data,const std::string path){
    if(ALLOW_DEBUG_FILE_STORAGE){
        std::fstream fs;
        fs.open(path.c_str(),std::ios_base::out);
        fs<<data;
        fs.close();
    }
}

/* type cast: from C++ native int to uint8_t */
uint8_t * cast2uint8_t(const int * src,const int len){
    uint8_t * des = new uint8_t[len];
    for(int i=0;i<len;++i){
        des[i] = src[i];
    }
    return des;
}


std::vector<std::string> split_string(const std::string& data,const std::string&sym){
    std::vector<std::string> result;
    int s=0;
    std::string subresult = "";
    for(int i=0;i<data.length();++i){
        if( data[i] == sym[0]  ){
            bool found = true;
            for(int j=1;j<sym.length();++j){
                if( sym[j] != data[i+j] ){
                    found = false;
                    break;
                }
            }
            if(found){
                subresult = data.substr(s,i-s);
                result.push_back(subresult);
                i += sym.length();
                s = i;
            }
        }
    }
    if(s != data.length()){
        result.push_back( data.substr(s,data.length()-s) );
    }
    return result;
}


/* strinfy the result list */
/* format: [array of image data]@width@height<!> */
/* <!> this symbol is used to split differnt images */
const std::string strinfy(DLISTIMAGEPACK dpack,int&sum){
    std::string result = "";
    for(int i=0;i<dpack.size();++i){    // extract line container
        LISTIMAGEPACK linecontainer = dpack[i];
        for(int j=0;j<linecontainer.size();++j){     // extract alpberts image on every line
            // we're processing ImagePack here
            std::string buf = "[";
            std::string properties = to_string(linecontainer[j].properties.width) + "@"
                                    + to_string(linecontainer[j].properties.height) + "<!>";
            //start processing evert channel's pixel now,only work for 1 channel image
            // that is grayscale image
            //int mmm=0;
            if( linecontainer[j].properties.channel ==1 ){
                for(int z=0;z<linecontainer[j].properties.height;z++){
                    for(int l=0;l<linecontainer[j].properties.width;l++){
                        buf += to_string(int(linecontainer[j].image[z][l][0])) + ",";
                    }
                }
                buf = buf.substr(0,buf.length() - 1) + "]@";
                buf += properties;
                result += buf;
                sum++;
            }
        }
    }
    return result;
}

/* delete empty line of an image */
const IMAGE delteEmptyline(IMAGE image,ImageData&id,const int del_target=1){
    int countx = 0;
    int del_line=0;
    // scan on top
    for(int i=0;i<id.height;++i){
        for(int j=0;j<id.width;++j){
            if( int(image[i][j][0]) == del_target ){
                ++countx;
            }
        }
        if(countx != id.width){
            break;
        }else{
            ++del_line;
            countx = 0;
        }
    }
    image = sliceSubMatrix3D(image,id,del_line,id.height,-1,-1);
    id.height -= del_line;

    // scan on bottom
    countx=0;
    del_line=0;
    for(int i=id.height-1;i>=0;i--){
        for(int j=0;j<id.width;++j){
            if( int(image[i][j][0]) == del_target ){
                ++countx;
            }
        }
        if(countx != id.width){
            break;
        }else{
            ++del_line;
            countx = 0;
        }
    }
    image = sliceSubMatrix3D(image,id,0,id.height-del_line,-1,-1);
    id.height -= del_line;

    // scan on the left
    countx=0;
    del_line=0;
    for(int i=0;i<id.width;++i){
        for(int j=0;j<id.height;++j){
            if( int(image[j][i][0]) == del_target ){
                ++countx;
            }
        }
        if(countx != id.height){
            break;
        }else{
            ++del_line;
            countx = 0;
        }
    }
    image = sliceSubMatrix3D(image,id,-1,-1,del_line,id.width);
    id.width -= del_line;

    // scan on the right
    countx=0;
    del_line=0;
    for(int i=id.width-1;i>=0;++i){
        for(int j=0;j<id.height;++j){
            if( int(image[j][i][0]) == del_target ){
                ++countx;
            }
        }
        if(countx != id.height){
            break;
        }else{
            ++del_line;
            countx = 0;
        }
    }
    image = sliceSubMatrix3D(image,id,-1,-1,0,id.width-del_line);
    id.width -= del_line;

    return image;
}

/* image feature extractor: 投影区块匹配  */
Features feature_extractor_projectionmatch(IMAGE img,ImageData&id,std::string label){
    Features feature;
    std::vector<int> xsum(id.width);
    std::vector<int> ysum;
    for(int i=0;i<id.height;++i){
        int ycount = 0;
        for(int j=0;j<id.width;++j){
            if( int(img[i][j][0]) == 0 ){ // 0 for data , 1 for background
                ++ycount;
                ++(xsum[j]);
            }
        }
        ysum.push_back(ycount);
    }
    feature.x_sum = xsum;
    feature.y_sum = ysum;
    feature.width = id.width;
    feature.height = id.height;
    feature.wh_ratio = double(id.width)/id.height;
    feature.label = label;
    return feature;
}


/* image feature extractor: nine special points sampling   */
Features feature_extractor_9Sampling(IMAGE img,ImageData&id,Features& fea){
    //we're going to select 9 special point to use as our third feature
    // these 9 points are top-left, top-center, top-right, also with mid-height and bottom
    // then, we're going to arrange them in the order of scaning
    fea.ninesampling.push_back( img[ 0 ][ 0 ][0] );                                // top-left
    fea.ninesampling.push_back( img[ 0 ][ int(id.width/2) ][0] );                  // top-center
    fea.ninesampling.push_back( img[ 0 ][ id.width-1 ][0] );                       //top-right
    fea.ninesampling.push_back( img[ int(id.height/2) ][ 0 ][0] );                  //mid-l
    fea.ninesampling.push_back( img[ int(id.height/2) ][ int(id.width/2) ][0] );   //mid-c
    fea.ninesampling.push_back( img[ int(id.height/2) ][ id.width-1 ][0] );        //mid-r
    fea.ninesampling.push_back( img[ id.height-1 ][ 0 ][0] );                      //bottom-l
    fea.ninesampling.push_back( img[ id.height-1 ][ int(id.width/2) ][0] );        //bottom-c
    fea.ninesampling.push_back( img[ id.height-1 ][ id.width-1 ][0] );              //bottom-r

    return fea;
}


/* get the weight feature of the picture */
void feature_circle(IMAGE img,ImageData&id,Features&fea){
    // scan from left to right
    int lcount = 0;
    for(int i=0;i<id.height;++i){
        int bufx = 0;
        bool nof = false;
        for(int j=0;j<id.width;++j){
            if(img[i][j][0] == 0 && !nof){
                ++bufx;
                nof = true;
            }else if(img[i][j][0]== 1 && nof){
                nof = false;
            }
        }
        if(bufx >= 2){
            ++lcount;
        }
    }

    //scan from top to bottom
    int vcount = 0;
    for(int i=0;i<id.width;++i){
        int bufx = 0;
        bool nof = false;
        for(int j=0;j<id.height;++j){
            if(img[j][i][0] == 0 && !nof){
                ++bufx;
                nof = true;
            }else if(img[j][i][0]== 1 && nof){
                nof = false;
            }
        }
        if(bufx >= 2){
            ++vcount;
        }
    }

    //make sure the point exist
    if(lcount > 0 && vcount > 0){
        fea.circle = 1;
        return;
    }
    fea.circle = 0;
}

/* compute cosine */
const double cosine(std::vector<int> v1,std::vector<int> v2){
    int sum = 0;
    int sumv1=0,sumv2=0;
    // reverse value (1<>0) in v1 and v2
    for(int i=0;i<v1.size();++i){
        v1[i] = std::abs(--v1[i]);
        v2[i] = std::abs(--v2[i]);
    }
    for(int i=0;i<v1.size();++i){
        sum += v1[i]*v2[i];
        sumv1 += v1[i];
        sumv2 += v2[i];
    }
    return sum*1.0/(sqrt(sumv1)*sqrt(sumv2));
} 


/* parse feature structure to string */
/* feature format: x1,x2@y1,y2@scale@height,width@label@9points#next_feature_section@xweight,vertial_ratio */
std::string feature2string(Features f){
    std::string buf = "";
    for(int i=0;i<f.x_sum.size();++i){
        buf += to_string(f.x_sum[i]) + ",";
    }
    buf = buf.substr(0,buf.length()-1) + "@";
    for(int i=0;i<f.y_sum.size();++i){
        buf += to_string(f.y_sum[i]) + ",";
    }
    buf = buf.substr(0,buf.length()-1) + "@";
    buf += to_string(f.wh_ratio) + "@";
    buf += to_string(f.height) + ",";
    buf += to_string(f.width) + "@";
    buf += f.label + "@";
    for(int i=0;i<f.ninesampling.size();++i){
        buf += to_string(f.ninesampling[i]) + ",";
    }
    buf = buf.substr(0,buf.length()-1) + "@";
    buf += to_string(f.circle) ;
    
    return buf;
}

#ifndef FEATURE_DEFENDED
/* converting string to features,s = x1,x2@y1,y2@scale@height,width@label */
Features string2feature(std::string s){
    Features f;
    // get x_sum_str,y_sum_str,tatio_str,height_str,width_str,label
    std::vector<std::string> dataframe = split_string(s,"@"); 
    //parse x_sum
    std::vector<std::string> values = split_string(dataframe[0],",");
    std::vector<int> xsum;
    for(int i=0;i<values.size();++i){
        xsum.push_back( atoi( values[i].c_str() ) );
    }
    //parse y_sum
    values = split_string(dataframe[1],",");
    std::vector<int> ysum;
    for(int i=0;i<values.size();++i){
        ysum.push_back( atoi( values[i].c_str() ) );
    }
    f.x_sum = xsum;
    f.y_sum = ysum;
    //parse ratio
    f.wh_ratio = atof(dataframe[2].c_str());
    // parse width and height
    std::vector<std::string> hw = split_string(dataframe[3],",");
    f.height = atoi(hw[0].c_str());
    f.width = atoi(hw[1].c_str());
    f.label = dataframe[4];
    // set 9 special feature
    values = split_string(dataframe[5],",");
    std::vector<int> nsps;
    for(int i=0;i<values.size();++i){
        nsps.push_back(atoi(values[i].c_str()));
    }
    f.ninesampling = nsps;
    // set ratio feature
    f.circle = atoi(dataframe[6].c_str());

    return f;
}
#endif

/* function below is used for prediction */

const std::vector<Features> parseTemplateData(std::string data){
    std::vector<Features> templatelist;
    std::vector<std::string> datalist = split_string(data,"#");
    for(int i=0;i<datalist.size();++i){
        templatelist.push_back( string2feature(datalist[i]) );
    }
    return templatelist;
}


/* minus two feature vectors with the consideration of different font size */
const std::vector<double> minusWithScale(const std::vector<int> x,const std::vector<int> templatex,const double payoff){
    std::vector<double> result;
    // compress longer vector to align with the shorter one
    if( x.size() >= templatex.size() ){ 
        // compress x
        const double mapping_size = x.size()/templatex.size();
        for(int i=0;i<templatex.size();++i){
            int base = int(i*mapping_size);
            int upper = int(base+mapping_size);
            int sumx = 0;
            for(int j=base;j<upper;++j){
                sumx += x[j];
            }
            sumx /= (upper-base);
            result.push_back( sumx*payoff - templatex[i] );
        }
    }else{
        // compress template
        const double mapping_size =templatex.size()/x.size();
        for(int i=0;i<x.size();++i){
            int base = int(i*mapping_size);
            int upper = int(base+mapping_size);
            int sumx = 0;
            for(int j=base;j<int(base+mapping_size);++j){
                sumx += templatex[j];
            }
            sumx /= (upper-base);
            result.push_back( sumx*payoff - x[i] );
        }
    }
    printVector(x,"x");
    printVector(templatex,"templatex");
    printVector(result,"minusWithScale");
    return result;
}

/*  */
const double meanSquaredError(const std::vector<double> con,const double csum){
    double error = 0.0;
    std::string logx="";
    for(int i=0;i<con.size();++i){
        double xx =  con[i]/csum;
        error += (xx*xx);
    }
    return error/con.size();
}

const double payoff(const int a,const int b){
    return ((a>=b?b:a)*1.0) /  ((a>=b?a:b)*1.0) ;
}


/* compute similarity based on width and height ratio*/ 
const double error_feature_WHR(const Features img,const Features temp){
    return std::abs( img.wh_ratio - temp.wh_ratio );
}

/* compute similarity */
inline const double similarity(const Features img,const Features temp){
    std::vector<double> x_diff = minusWithScale( img.x_sum , temp.x_sum , payoff( temp.height,img.height ) );
    std::vector<double> y_diff = minusWithScale( img.y_sum , temp.y_sum , payoff( temp.width,img.width ) );
    double x_error = meanSquaredError( x_diff , temp.height<=img.height?img.height:temp.height );
    double y_error = meanSquaredError( y_diff , temp.width<=img.width?img.width:temp.width );
    const double efwhr = error_feature_WHR(img,temp);
    const double simNSPS = cosine(temp.ninesampling,img.ninesampling);
    //std::cout<<"\n\t\tx_err="<<x_error<<"\ty_err="<<y_error<<"\tcos="<<simNSPS<<"\twh_ratio="<<efwhr<<"\n";
    return  0.65*(x_error + y_error) + 0.12*(1-simNSPS) + 0.23*efwhr ;
    //return  0.65*(x_error + y_error) + 0.12*(1-simNSPS) + 0.13*efwhr + 0.1*std::abs(img.circle-temp.circle);
}

/* predict which alphaberts it is  */
std::string predictAlphberts(ImagePack img,const std::vector<Features> &templatedata){
    // get feature
    Features f = feature_extractor_projectionmatch(img.image,img.properties,"");
    f = feature_extractor_9Sampling(img.image,img.properties,f);
    //feature_circle(img.image,img.properties,f);

    double minv = std::numeric_limits<double>::max();            // stores max similarity
    std::string current="*";    // stores char which matches to the max similarity
    // loop computing cos with the template data
    for(auto& temp: templatedata){
        double x = similarity(f,temp);
        if( x <= minv ){
            minv =x;
            current = temp.label;
        }
    }
    
    // return data
    return current;
}

void exportImage(ImagePack & imp2d,std::string filename){
    ImagePack2D d2line = depixelize(imp2d.image,imp2d.properties);
    save_string( numpylize( nullptr ,d2line.properties, d2line.image ) ,"cache/" + filename);
}

/* ==============================================Extract text fucntion here================================== */

DLISTIMAGEPACK extractText(uint8_t* img,const ImageData & id){
    const IMAGE image  = to_Martix(img,id);
    save_string( numpylize(image,id) ,"cache/raw_image.txt");
    ImagePack grayscaleimgpack = to_grayScale(image,id);
    //Thresholding(image,id);
    ImagePack2D a = depixelize(grayscaleimgpack.image ,grayscaleimgpack.properties);
    save_string(numpylize(nullptr,a.properties,a.image),"cache/grayscale.txt");
    /* 1 for background, 0 for data */
    grayscaleimgpack.image = set_lessThan2Value(grayscaleimgpack.image,grayscaleimgpack.properties,1,180);   //50
    grayscaleimgpack.image = set_largeThan2Value(grayscaleimgpack.image,grayscaleimgpack.properties,0,180); //185
    a = depixelize(grayscaleimgpack.image ,grayscaleimgpack.properties);
    save_string(numpylize(nullptr,a.properties,a.image),"cache/thresholding.txt");
    int *ones_on_y = new int[grayscaleimgpack.properties.height];
    for(int i=0;i<grayscaleimgpack.properties.height;++i){
        int buf = 0;
        for(int j=0;j<grayscaleimgpack.properties.width;++j){
            if(grayscaleimgpack.image[i][j][0] > 0){
               ++buf;
            }
        }
        ones_on_y[i] = buf;
    }
    LIST sentence_boundary;
    int start,end;
    for(int i=0;i<grayscaleimgpack.properties.height-1;++i){
        if( ( ones_on_y[i] == 0) && (ones_on_y[i+1] > 0) ){ //top boundary of sentence
            start = i;
        }else if((ones_on_y[i] > 0) && (ones_on_y[i+1] == 0)){   // bottom boundary of sentence
            end = i+1;
            sentence_boundary.push_back(start);
            sentence_boundary.push_back(end);
        }
    }
    int linecount = sentence_boundary.size()/2;
    LISTIMAGEPACK sentences;
    for(int i=0;i<linecount;++i){
        ImagePack im;
        im.image = sliceSubMatrix3D(grayscaleimgpack.image,grayscaleimgpack.properties,
                                    sentence_boundary[2*i],sentence_boundary[2*i+1],-1,-1);
        im.properties.width = grayscaleimgpack.properties.width;
        im.properties.height = sentence_boundary[2*i+1] - sentence_boundary[2*i];
        im.properties.channel = 1;
        sentences.push_back(im);
        ImagePack2D d2line = depixelize(im.image,im.properties);
        //save_string( numpylize( nullptr ,d2line.properties, d2line.image ) ,"cache/line" + to_string(i+1) + ".txt");
    }
    DLISTIMAGEPACK alphaberts;
    for(int i =0;i<sentences.size();++i){
        LISTIMAGEPACK alpha;
        ImagePack sen = sentences[i];
        LIST zeros_on_sentence;
        for(int j=0;j<sen.properties.width;++j ){
            int buf=0;
            for(int c=0;c<sen.properties.height;c++){
                if(sen.image[c][j][0] > 0){
                    ++buf;
                }
            }
            zeros_on_sentence.push_back(buf);
        }
        start = end = 0;
        for(int j=0;j<zeros_on_sentence.size();++j){
            if( ( zeros_on_sentence[j] == 0) && (zeros_on_sentence[j+1] > 0) ){ //left boundary of alphaberts
                start = j;
            }else if((zeros_on_sentence[j] > 0) && (zeros_on_sentence[j+1] == 0)){   // right boundary of alplaberts
                end = j+1;
                IMAGE imx = sliceSubMatrix3D(sen.image,sen.properties,-1,-1,start,end);
                ImagePack imp = {imx,{end-start,sen.properties.height,1}};
                imp.image = reverseImageBit(imp.image,imp.properties);

                ImagePack2D d2line = depixelize(imp.image,imp.properties);
                //save_string( numpylize( nullptr ,d2line.properties, d2line.image ) ,"cache/alp_"+ to_string(i+1) + to_string(j+1) + ".txt");
                alpha.push_back(imp);
            }
        }
        alphaberts.push_back(alpha);
    }

    //delete resource
    delete[] ones_on_y;
    deleteMatrix(image,id);
    deleteMatrix(grayscaleimgpack.image,grayscaleimgpack.properties);
    return alphaberts;
}

/* this function will extract all words out of the picture */
/* words extracted can be used in extractText for future recognize */
DLISTIMAGEPACK extractWord(uint8_t* img,const ImageData & id){
    const IMAGE image = to_Martix(img,id);
    save_string( numpylize(image,id) ,"cache/raw_image.txt");
    ImagePack grayscaleimgpack = to_grayScale(image,id);
    /* 1 for background, 0 for data */
    Thresholding(grayscaleimgpack.image,grayscaleimgpack.properties);
    //grayscaleimgpack.image = set_lessThan2Value(grayscaleimgpack.image,grayscaleimgpack.properties,1,180);  
    //grayscaleimgpack.image = set_largeThan2Value(grayscaleimgpack.image,grayscaleimgpack.properties,0,180); 
    exportImage(grayscaleimgpack,"thresholding_img.txt" );
    int *ones_on_y = new int[grayscaleimgpack.properties.height];
    for(int i=0;i<grayscaleimgpack.properties.height;++i){
        int buf = 0;
        for(int j=0;j<grayscaleimgpack.properties.width;++j){
            if(grayscaleimgpack.image[i][j][0] > 0){
               ++buf;
            }
        }
        ones_on_y[i] = buf;
    }
    LIST sentence_boundary;
    int start,end;
    for(int i=0;i<grayscaleimgpack.properties.height-1;++i){
        if( ( ones_on_y[i] == 0) && (ones_on_y[i+1] > 0) ){ //top boundary of sentence
            start = i;
        }else if((ones_on_y[i] > 0) && (ones_on_y[i+1] == 0)){   // bottom boundary of sentence
            end = i+1;
            sentence_boundary.push_back(start);
            sentence_boundary.push_back(end);
        }
    }
    int linecount = sentence_boundary.size()/2;
    LISTIMAGEPACK sentences;
    for(int i=0;i<linecount;++i){
        ImagePack im;
        im.image = sliceSubMatrix3D(grayscaleimgpack.image,grayscaleimgpack.properties,
                                    sentence_boundary[2*i],sentence_boundary[2*i+1],-1,-1);
        im.properties.width = grayscaleimgpack.properties.width;
        im.properties.height = sentence_boundary[2*i+1] - sentence_boundary[2*i];
        im.properties.channel = 1;
        sentences.push_back(im);
        ImagePack2D d2line = depixelize(im.image,im.properties);
        //save_string( numpylize( nullptr ,d2line.properties, d2line.image ) ,"cache/line" + to_string(i+1) + ".txt");
    }
    DLISTIMAGEPACK words;
    for(int i =0;i<sentences.size();++i){
        LISTIMAGEPACK alpha;
        ImagePack sen = sentences[i];
        LIST zeros_on_sentence;
        for(int j=0;j<sen.properties.width;++j ){
            int buf=0;
            for(int c=0;c<sen.properties.height;c++){
                if(sen.image[c][j][0] > 0){
                    ++buf;
                }
            }
            zeros_on_sentence.push_back(buf);
        }
        // get avereage space length
        //get left and right boundary first
        int left_line_boundary = 0;
        for(int j=0;j<zeros_on_sentence.size();--j){
            if(zeros_on_sentence[j] != 0){
                break;  
            }
            ++left_line_boundary;
        }
        int right_line_boundary = 0;
        for(int j=zeros_on_sentence.size()-1;j >= 0;--j){
            if(zeros_on_sentence[j] != 0){
                break;  
            }
            ++right_line_boundary;
        }
        int average = 0;
        int count_space = 0;
        int lenxxx = 0;
        bool loopzero = false;
        for(int j=left_line_boundary;j<zeros_on_sentence.size()-right_line_boundary;++j){
            if(zeros_on_sentence[j] == 0){
                if(!loopzero){
                    start = j;
                    lenxxx = 1;
                    loopzero = true;
                    ++count_space;
                }else{
                    ++lenxxx;
                }
            }else{
                if(loopzero){
                    loopzero = false;
                    average += lenxxx;
                }
            }
        }
        average = (average/count_space) + 2;        // change this number if it works badly
        //std::cout<<"average space length="<<average<<std::endl;
        // start extract words
        start = end = 0;
        bool no_split = true;
        for(int j=0;j<zeros_on_sentence.size();++j){
            if( ( zeros_on_sentence[j] == 0) && (zeros_on_sentence[j+1] > 0) ){ //left boundary of words
                if(no_split){
                    start = j;
                }
            }else if((zeros_on_sentence[j] > 0) && (zeros_on_sentence[j+1] == 0)){   // right boundary of alplaberts
                end = j+1;
                no_split = true;
                int ending_checking_pos = ((j+average)>(zeros_on_sentence.size()))?(zeros_on_sentence.size()):(j+average);
                for(int k=j+1;k<ending_checking_pos;k++){
                    if(zeros_on_sentence[k] > 0){
                        no_split = false;
                        break;
                    }
                }
                if(no_split){
                    IMAGE imx = sliceSubMatrix3D(sen.image,sen.properties,-1,-1,start,end);
                    ImagePack imp = {imx,{end-start,sen.properties.height,1}};
                    //imp.image = reverseImageBit(imp.image,imp.properties);
                    alpha.push_back(imp);
                    no_split = true;
                }
            }
        }
        words.push_back(alpha);
    }

    //delete resource
    delete[] ones_on_y;
    deleteMatrix(image,id);
    deleteMatrix(grayscaleimgpack.image,grayscaleimgpack.properties);
    return words;
}

DLISTIMAGEPACK extractTextFromWord(const IMAGE word,const ImageData & id){
    DLISTIMAGEPACK alphaberts;
    LISTIMAGEPACK alpha;
    ImagePack sen = ImagePack{word,id};
    exportImage(sen,"extractTextFromWord_raw.txt" );
    LIST zeros_on_sentence;
    for(int j=0;j<sen.properties.width;++j ){
        int buf=0;
        for(int c=0;c<sen.properties.height;c++){
            if(sen.image[c][j][0] > 0){
                ++buf;
            }
        }
        zeros_on_sentence.push_back(buf);
    }
    int start=0, end = 0;
    bool beg_found = false;
    for(int j=0;j<zeros_on_sentence.size();++j){
        if( ( zeros_on_sentence[j] > 0 ) && (!beg_found)  ){ //left boundary of alphaberts
            start = j;
            beg_found = true;
        }else if(((zeros_on_sentence[j] == 0) || (j==zeros_on_sentence.size()-1)) && beg_found){   // right boundary of alplaberts
            end = j;
            IMAGE imx = sliceSubMatrix3D(sen.image,sen.properties,-1,-1,start,end);
            ImagePack imp = {imx,{end-start,sen.properties.height,1}};
            imp.image = reverseImageBit(imp.image,imp.properties);
            beg_found = false;
            //printMatrix(imp.image,imp.properties);
            //exportImage(imp,"alp_alp_from_wordsExtracted_"+ to_string(debug_alp_count++) + ".txt");
            alpha.push_back(imp);
        }
    }
    alphaberts.push_back(alpha);
    return alphaberts;
}

/* ==============================================Recognize text fucntion here================================== */
/* recognize a single image */
std::string recognize(DLISTIMAGEPACK& data){
    //start recognize
    std::string result = "";
    const std::vector<Features> templatedata =  parseTemplateData(pcadata);
    for(DLISTIMAGEPACKITERATOR i=data.begin();i<data.end();++i){
        LISTIMAGEPACK alpha = *i;
        for(LISTIMAGEPACKITERATOR j=alpha.begin();j<alpha.end();++j){
            //delete empty line
            (*j).image = delteEmptyline((*j).image,(*j).properties);
            ImagePack2D d2line = depixelize((*j).image,(*j).properties);
            std::string re = predictAlphberts(*j,templatedata);
            result += re;
            //printMatrix((*j).image,(*j).properties);
        }
    }
    return result;
}

/* recognize a data with the original format, like words and space */
/* input must be words list */
std::string recognizeWithFormat(const DLISTIMAGEPACK& data){
    std::string result = "";
    initSpellChecker();
    for(auto& line: data){
        for(auto& wordimg: line){
            DLISTIMAGEPACK word = extractTextFromWord(wordimg.image,wordimg.properties);
            std::string wordx =  recognize(word);
            wordx = suggest(wordx);// correct word here
            result += wordx + " ";
        }
        result += "\r\n";
    }
    return result;
}