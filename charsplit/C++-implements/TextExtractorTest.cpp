#include<iostream>
#include<string>
#define ALLOW_DEBUG_FILE_STORAGE           true
#define ALLOW_DEBUG_MSG                    false
#include"TextExtractor.h"

#define TEST_WORD_SPLIT true
#define TEST_ALPHABERTS_SPLIT true
#define TEST_AW_CONBINATION true

using namespace std;

void save_stringT(const std::string data,const std::string path){
        std::fstream fs;
        fs.open(path.c_str(),std::ios_base::out);
        fs<<data;
        fs.close();
}

int main(int argc,char**argv){
    cout<<">>Reading image"<<endl;
    int width, height, bpp;
    
    DLISTIMAGEPACK p_words;
    if(TEST_WORD_SPLIT){                                    //PASS
        cout<<("Test for words segmentation...")<<endl;
        uint8_t* rgb_image_word_test = stbi_load("data/test2.jpg", &width, &height, &bpp, 3);
        ImageData b = {width,height,bpp};
        p_words = extractWord(rgb_image_word_test,b);
        klog("saving result..");
        int word_count = 0;
        for(int i=0;i<p_words.size();i++){
            LISTIMAGEPACK line = p_words[i];
            for(int j=0;j<line.size();j++){
                ImagePack imp = line[j];
                ++word_count;
                exportImage(imp,"alp_wordsExtracted_"+ to_string(i+1) + to_string(j+1) + ".txt");
            }
        }
        cout<<"word segmentation couts:"<<word_count<<endl;
        stbi_image_free(rgb_image_word_test);
        cout<<("done.")<<endl;
    }


    if(TEST_AW_CONBINATION){                                //FAILED
        cout<<"\n\nTEST_AW_CONBINATION result="<<recognizeWithFormat(p_words)<<endl;
    }
    
    if(TEST_ALPHABERTS_SPLIT){                             //PASS
        cout<<("Test for alphaberts segmentation...")<<endl;
        uint8_t* rgb_image = stbi_load("data/test2.jpg", &width, &height, &bpp, 3);
        ImageData a = {width,height,bpp};
        // extract single alphaberts image
        DLISTIMAGEPACK p = extractText(rgb_image,a);
        int sum = 0;
        string ss = strinfy(p,sum);
        cout<<">>width:"<<width<<"\theight:"<<height<<"\tbpp:"<<bpp<<"\tsplit_sum="<<sum<<endl;
        stbi_image_free(rgb_image);
        // recognize
        string result = recognize(p);
        cout<<"result:\t"<<result<<endl;
    }

    return 0;
}