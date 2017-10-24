#include<iostream>
#include<cmath>
#include<vector>
#include<fstream>
#include"lib/Eigen/Dense"
#include"lib/Eigen/Core"
#include"lib/Eigen/SVD"

#define ABR_H
#define FEATURE_DEFENDED


using namespace Eigen;
/*****************************************************************************\
***                                                                        ***\   
***                      Recognize Alphaberts Related                      ***\
***                    by Kanch at http://akakanch.com                     ***\
***                         kanchisme@gmail.com                            ***\
***                                                                        ***\
***                                                                        ***\
******************************************************************************/
typedef uint8_t BASE;
typedef BASE*** MATRIX;
typedef BASE CHANNEL;
typedef Matrix<double, Dynamic, 1> PcaVector;

/* structures to stores Features */
struct Features{
    std::vector<int> x_sum;
    std::vector<int> y_sum;
    std::vector<int> line_sum;
    int width;
    int height;
    double wh_ratio;
    std::string label;
};

template <typename T>
void alog(T a){
    std::cout << ">>>"<<a<<std::endl;
}

std::vector<std::string> split_string(const std::string& data,const std::string&sym){
    std::vector<std::string> result;
    int s=0;
    std::string subresult = "";
    for(int i=0;i<data.length();i++){
        if( data[i] == sym[0]  ){
            bool found = true;
            for(int j=1;j<sym.length();j++){
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

PcaVector paseToPCAvector(const std::vector<std::string>& data){
    PcaVector vec(data.size(),1);
    for(int i=0;i<data.size();i++){
        vec(i,0) = std::atof(data[i].c_str());
    }
    return vec;
}

const std::vector<std::pair<PcaVector,std::string>> loadTemplateData(std::string fpath="template.data"){
    std::vector<std::pair<PcaVector,std::string>> pcalist;
    //read
    std::string data;
    std::fstream fs;
    fs.open(fpath.c_str(),std::ios_base::in);
    fs>>data;
    fs.close();
    //parse
    // format:  value,value,...,...#charvalue@....
    //          vector1#charvalue1@vector2#charvalue2@...
    std::vector<std::string> datalist = split_string(data,"@");
    for(int i=0;i<datalist.size();i++){
        //alog(datalist[i]);
        std::vector<std::string> datax = split_string(datalist[i],"#");
        std::vector<std::string> values = split_string(datax[0],",");
        PcaVector v = paseToPCAvector(values);
        std::string rv = datax[1];
        pcalist.push_back(std::pair<PcaVector,std::string>(v,rv));
    }

    return pcalist;
}
/* converting string to features,s = x1,x2@y1,y2@scale@height,width@label */
Features string2feature(std::string s){
    Features f;
    // get x_sum_str,y_sum_str,ration_str,height_str,width_str,label
    std::vector<std::string> dataframe = split_string(s,"@"); 
    //parse x_sum
    std::vector<std::string> values = split_string(dataframe[0],",");
    std::vector<int> xsum;
    for(int i=0;i<values.size();i++){
        xsum.push_back(atoi(values[i].c_str()));
    }
    //parse y_sum
    values = split_string(dataframe[1],",");
    std::vector<int> ysum;
    for(int i=0;i<values.size();i++){
        ysum.push_back(atoi(values[i].c_str()));
    }
    f.x_sum = xsum;
    f.y_sum = ysum;
    //parse ratio
    f.wh_ratio = atof(dataframe[2].c_str());
    // parse width and height
    f.height = atoi(dataframe[3].c_str());
    f.width = atoi(dataframe[4].c_str());
    f.label = dataframe[5];

    return f;
}

const std::vector<Features> parseTemplateData(std::string data){
    std::vector<Features> templatelist;
    std::vector<std::string> datalist = split_string(data,"#");
    for(int i=0;i<datalist.size();i++){
        templatelist.push_back( string2feature(datalist[i]) );
    }
    return templatelist;
}

void test_eigen(){
    MatrixXd C;
    C.setRandom(27,18);
    JacobiSVD<MatrixXd> svd( C, ComputeThinU | ComputeThinV);
    MatrixXd Cp = svd.matrixU() * svd.singularValues().asDiagonal() * svd.matrixV().transpose();
    MatrixXd diff = Cp - C;
    std::cout << "diff:\n" << diff.array().abs().sum() << "\n";
    std::cout << "matrix:\n" << C  << "\n";
    std::cout << "SingluarValues:\n" << svd.singularValues().data() << "\n";
}

/* cast native matrix MATRIX to Eigen Matrix type */
MatrixXd  to_EigenMatrixXd(MATRIX mat,const int width,const int height){
    MatrixXd result(height,width);
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            result(i,j) = mat[i][j][0];
        }
    }
    return result;
}

/* get top k PCA vectors */
PcaVector getPca(MatrixXd image,const int k){
    //std::cout << ">>>conputing SVD..." << std::endl;
    JacobiSVD<MatrixXd> svd( image, ComputeThinU | ComputeThinV);
    // get matrix U ,sigma, V transpose
    //std::cout << ">>>get U,sigma,Vt..." << std::endl;
    MatrixXd u = svd.matrixU();
    MatrixXd s = svd.singularValues().asDiagonal();
    MatrixXd vt = svd.matrixV().transpose();

    //std::cout<<">>>Matrix U:\t"<<u.cols()<<" Cols\t"<<u.rows()<<" Rows"<<std::endl;
    //std::cout<<">>>Matrix U:\t"<<vt.cols()<<" Cols\t"<<vt.rows()<<" Rows"<<std::endl;

    // select PCA vector
    PcaVector pcavec( u.rows()*k + vt.cols()*k ,1);
    int cur = 0;
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < u.rows(); j++) {
            pcavec(cur + i * u.rows() + j,0) = u(j,i);
        }
    }
    cur += u.rows() * k;
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < vt.cols(); j++) {
            pcavec(cur + i * vt.cols() + j,0) = vt(i, j);
        }
    }

    return pcavec;
}

/* compute cosine of an n*1 vector */
double cosine(PcaVector v1, PcaVector v2) {
    double ans = 0;

    int n = v1.rows();
    if(v2.rows() < v1.rows()){
        n = v2.rows();
    }
    double a = 0;
    for (int i = 0; i < n; i++) {
        a += v1(i,0) * v2(i,0);
    }
    double b = 0;
    for (int i = 0; i < n; i++) {
        b += v1(i,0) * v1(i,0);
    }
    b = std::sqrt(b);
    double c = 0;
    for (int i = 0; i < n; i++) {
        c += v2(i,0) * v2(i,0);
    }
    c = std::sqrt(c);
    ans = a / b / c;

    return ans;
}

/* predict which alphaberts it is  */
std::string predictAlphberts(MATRIX mat,const int width,const int height,
                            const std::vector<std::pair<PcaVector,std::string>>& pcalist){
    //convert to Eigen matrix
    MatrixXd x = to_EigenMatrixXd(mat,width,height);

    //normalize size
    x.conservativeResize(25,25);
    
    //get PCA vector
    PcaVector pca = getPca(x,12);

    // loop computing cos with the template data
    double maxv = 0;
    std::string current="*";
    for(int i=0;i<pcalist.size();i++){
        double x = cosine(pca,pcalist[i].first);
        if(x > maxv){
            maxv =x;
            current = pcalist[i].second;
        }
    }
    
    // return data
    return current;
}