#define ALLOW_DEBUG_FILE_STORAGE           true
#define ALLOW_DEBUG_MSG                    true
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
                            printf (">>||Reading %s ...\n", ent->d_name);
                            // read image
                            int width=0,height=0,bpp=0;
                            uint8_t* rgb_image = stbi_load( (string(argv[1])  + string(ent->d_name)).c_str(), &width, &height, &bpp, 3);
                            ImageData a = {width,height,bpp};
        
                            //convert image to Matrix and delete redundent empty line
                            IMAGE image =  to_Martix(rgb_image,a);

        
                            cout<<"\t|To grayscale...\t";
                            ImagePack imagex = to_grayScale(image,a);
                            cout<<"normalizing...\t";
                            imagex.image = set_lessThan2Value(imagex.image,imagex.properties,0,180);   //50
                            imagex.image = set_largeThan2Value(imagex.image,imagex.properties,1,180); //185

                            //perform count image projection match
                            cout<<"scale to fit...\t";
                            //imagex.image = reverseImageBit(imagex.image,imagex.properties); // reverse, 0 for data, 1 for background
                            std::cout << "\n\t|input size h:"<<imagex.properties.height<<", w:"<<imagex.properties.width<<"\t";
                            imagex.image = delteEmptyline(imagex.image,imagex.properties,0);
                            std::cout << "\t,output size h:"<<imagex.properties.height<<", w:"<<imagex.properties.width<<"\t";
                            imagex.image = reverseImageBit(imagex.image,imagex.properties); // reverse, 0 for data, 1 for background
                            cout<<"\tsaving...\t";
                            ImagePack2D d2line = depixelize(imagex.image,imagex.properties);
                            //klog("saving");
                            save_string( numpylize( nullptr ,d2line.properties, d2line.image ) ,"cache/template_scale2fit_"+ to_string(++i) + ".txt");                    


                            // scale image to the same size
                            cout<<"\t|get projection and width-height ration features ...\t";

                            //extract feature
                            Features f =  feature_extractor_projectionmatch(imagex.image,imagex.properties,to_string(fname[0]));
                            f = feature_extractor_9Sampling(imagex.image,imagex.properties,f);
                            f = feature_ratio(imagex.image,imagex.properties,f);
                            cout<<"converting to string ...\n";
                            std::string strx = feature2string(f);
                            klog(strx);
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
            }else if(argc == 3){
                // second parameter is the template file
                // third parameter is the temp split alpheberts output directory
                cout<<">>Reading template image"<<endl;
                int width, height, bpp;
                
                uint8_t* rgb_image = stbi_load(argv[1], &width, &height, &bpp, 3);
            
                ImageData a = {width,height,bpp};
                string out_dir_prefix = string(argv[2]);
                
                // extract single alphaberts image
                DLISTIMAGEPACK p = extractText(rgb_image,a);
                int sum = 0;
                string ss = strinfy(p,sum);
                cout<<">>width:"<<width<<"\theight:"<<height<<"\tbpp:"<<bpp<<"\tsplit_sum="<<sum<<endl;
                cout<<"saving..."<<endl;
                for(int i=0;i<p.size();i++){
                    LISTIMAGEPACK line = p[i];
                    for(int j=0;j<line.size();j++){
                        ImagePack2D d2line = depixelize(line[j].image,line[j].properties);
                        string data = numpylize( nullptr ,d2line.properties, d2line.image );
                        string ddir = out_dir_prefix  + "alp_"+ to_string(i+1) + to_string(j+1) + ".txt";
                        save_string(  data, ddir);
                    }
                }
                cout<<"done."<<endl;
                stbi_image_free(rgb_image);


            }
}