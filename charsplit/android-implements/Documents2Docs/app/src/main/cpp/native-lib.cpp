#include <jni.h>
#include <string>
#include"TextExtractor.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_akakanch_documents2docs_documents2docs_RecentScreened_processImageJNI(
        JNIEnv *env,
        jobject /* this */,
        jintArray ipcdata /* image data as 1D array */,
        jint width /* image width */,
        jint height /* image height */,
        jint channels /* image channels */,
        jint returnimg /* return type: 1:alphberts,3:words,2:recognized */) {

    // get data and convert it
    jboolean iscopy = false;
    int * buf = (int*)env->GetIntArrayElements(ipcdata,&iscopy);
    jint length = width*height*channels;
    uint8_t * image = cast2uint8_t(buf,int(length));

    // start split alphaberts
    ImageData id;
    id.channel = channels;
    id.width = width;
    id.height = height;
    DLISTIMAGEPACK alphaberts = extractText(image,id);
    std::string imagesting = "";

    int typecode = (int)returnimg;
    if(typecode == 1) {
        int sum = 0;
        imagesting = strinfy(alphaberts, sum);
    }else if(typecode == 2){
        imagesting = recognize(alphaberts);
    }else if(typecode == 3){
        int sum = 0;
        DLISTIMAGEPACK words = extractWord(image,id);
        imagesting = strinfy(words, sum);
    }

    // just count "<!>" in result str to know how many sub image we get
    return env->NewStringUTF(imagesting.c_str());
}
