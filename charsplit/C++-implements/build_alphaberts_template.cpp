#include"alphabertsrecognize.h"
#include"TextExtractor.h"
#include<iostream>
#include<dirent.h>
#include<string>

using namespace std;
/*****************************************************************************\
***                                                                        ***\   
***             Build Alphaberts template for later Recognize              ***\
***                    by Kanch at http://akakanch.com                     ***\
***                         kanchisme@gmail.com                            ***\
***                                                                        ***\
***                                                                        ***\
******************************************************************************/


int main(int argc,char**argv){
    if(argc == 2){

        DIR *dir;
        struct dirent *ent;
        std::string result = "";
        if ((dir = opendir (argv[1])) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                //generate template for every alpgaberts
                string fname = string(ent->d_name);
                size_t found =  fname.find(".jpg");
                if( found != string::npos   ){
                    printf ("Reading %s ...\n", ent->d_name);
                    // read image
                    int width=0,height=0,bpp=0;
                    uint8_t* rgb_image = stbi_load( (string(argv[1])  + string(ent->d_name)).c_str(), &width, &height, &bpp, 3);
                    ImageData a = {width,height,bpp};

                    //convert image to Eigen Matrix
                    std::cout<<"Converting to MATRIX...\t";
                    IMAGE image =  to_Martix(rgb_image,a);
                    std::cout<<"\tdone.\t"<<a.height<<" rows,"<<a.width<<" columns"<<std::endl;

                    std::cout<<"To grayscale...\t";
                    ImagePack imagex = to_grayScale(image,a);
                    std::cout<<"\tdone.\t"<<imagex.properties.height
                            <<" rows,"<<imagex.properties.width<<" columns"<<std::endl;

                    std::cout<<"normailze...\t";
                    uint8_t meanx = 0;
                    imagex.image = normalize(imagex.image,imagex.properties,&meanx);
                    imagex.image = set_lessThan2Value(imagex.image,imagex.properties,0,30);   //50
                    imagex.image = set_largeThan2Value(imagex.image,imagex.properties,0,meanx-30); //185
                    std::cout<<"\tdone.\t"<<std::endl;

                    std::cout<<"Converting to EigenMatrix...\t";
                    MatrixXd x = to_EigenMatrixXd(imagex.image,imagex.properties.width,imagex.properties.height);
                    std::cout<<"\tdone.\t"<<x.rows()<<" rows,"<<x.cols()<<" columns"<<std::endl;
            
                    //get PCA vector
                    std::cout<<"Computing SVD to get PCA vectors..."<<std::endl;
                    PcaVector pca = getPca(x,2);

                    std::cout<<"converting to string..."<<std::endl;
                    std::string colvecstr = "";
                    for(int i=0;i<pca.rows();i++){
                        colvecstr += to_string(pca(i,0)) + ",";
                    }
                    std::string symbol = to_string(fname[0]);
                    if(fname[1] == 'u'){
                        symbol = char(symbol[0]-32);
                    }
                    result += colvecstr.substr(0,colvecstr.length()-1) + "#" + symbol + "@";

                }else{
                    printf ("Skip %s \n", ent->d_name);
                    std::cout<<fname<<",\t"<< fname.find(".jpg") <<std::endl;
                }
            }
            closedir (dir);
            std::cout<<"saving..."<<std::endl;
            std::fstream fs;
            fs.open("template.data",std::ios_base::out);
            fs<<result;
            fs.close();
            std::cout<<"result length:\t"<<result.length()<<std::endl;
            
        } else {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
        }
    }

    // Test loading data
    std::cout<<"test for loading..."<<std::endl;
    const std::vector<std::pair<PcaVector,std::string>>& pcalist = loadTemplateData();
    std::cout<<"pcalist size="<<pcalist.size()<<std::endl;

    return 0;
}

