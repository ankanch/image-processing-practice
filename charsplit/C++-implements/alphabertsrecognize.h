#include<iostream>
#include<cmath>
#include"lib/Eigen/Dense"
#include"lib/Eigen/Core"
#include"lib/Eigen/SVD"
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
    JacobiSVD<MatrixXd> svd( image, ComputeThinU | ComputeThinV);
    // get matrix U ,sigma, V transpose
    MatrixXd u = svd.matrixU();
    MatrixXd s = svd.singularValues().asDiagonal();
    MatrixXd vt = svd.matrixV().transpose();

    // select PCA vectore
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
std::string predictAlphberts(MATRIX mat,const int width,const int height){
    //convert to Eigen matrix
    MatrixXd x = to_EigenMatrixXd(mat,width,height);
    
    //get PCA vector
    PcaVector pca = getPca(x,2);

    // loop computing cos with the template data
    
    // return data
    return "";
}