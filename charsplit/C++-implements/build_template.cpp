#include"TextExtractor.h"
#include<iostream>
#include<dirent.h>
#include<string>

using namespace std;


/* this function is used to build template for alphberts recognize */
int main(int argc,char**argv){
    if(argc == 2){
                DIR *dir;
                struct dirent *ent;
                string result = "";
                if ((dir = opendir (argv[1])) != NULL) {
                    /* print all the files and directories within directory */
                    int i =0;
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
        
                            //convert image to Matrix and delete redundent empty line
                            IMAGE image =  to_Martix(rgb_image,a);

        
                            cout<<"To grayscale...\t";
                            ImagePack imagex = to_grayScale(image,a);
                            cout<<"normalizing...\t";
                            imagex.image = set_lessThan2Value(imagex.image,imagex.properties,0,180);   //50
                            imagex.image = set_largeThan2Value(imagex.image,imagex.properties,1,180); //185

                            //perform count image projection match
                            cout<<"scale to fit...\t";
                            std::cout << "\ninput size=\th:"<<imagex.properties.height<<"\tw:"<<imagex.properties.width<<std::endl;
                            imagex.image = delteEmptyline(imagex.image,imagex.properties);
                            std::cout << "\noutput size=\th:"<<imagex.properties.height<<"\tw:"<<imagex.properties.width<<std::endl;
                            cout<<"saving...\t";
                            ImagePack2D d2line = depixelize(imagex.image,imagex.properties);
                            //klog("saving");
                            save_string( numpylize( nullptr ,d2line.properties, d2line.image ) ,"cache/template_scale2fit_"+ to_string(++i) + ".txt");                    

                            // scale image to the same size
                            cout<<"get projection and width-height ration features ...\t";
                            Features f =  feature_extractor_projectionmatch(imagex.image,imagex.properties,to_string(fname[0]));
                            cout<<"converting to string ...\n";
                            std::string strx = feature2string(f);
                            result += strx + "#";

        
                        }else{
                            printf ("Skip %s \n", ent->d_name);
                            cout<<fname<<",\t"<< fname.find(".jpg") <<endl;
                        }
                    }
                    closedir (dir);
                    cout<<"saving..."<<endl;
                    fstream fs;
                    fs.open("template.h",ios_base::out);
                    string head = "#include<string>\r\nstd::string pcadata = \" " + result + "\";";
                    fs<<head;
                    fs.close();
                    cout<<"result length:\t"<<result.length()<<endl;
                    
                } else {
                /* could not open directory */
                perror ("");
                return EXIT_FAILURE;
                }
            }
}