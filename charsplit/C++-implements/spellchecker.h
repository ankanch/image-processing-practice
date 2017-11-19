#include<string>
#include<vector>
#include<fstream>
#include<algorithm>
#include<iostream>
#include"wordslist.h"

typedef std::vector<std::string> WORDLIST;
typedef std::vector<std::string>::iterator WLITERATOR;

/*****************************************************************************\
***                                                                        ***\   
***                         Simple Spell Checker                           ***\
***                 This can help to check words correction                ***\
***                    by Kanch at http://akakanch.com                     ***\
***                         kanchisme@gmail.com                            ***\
***                                                                        ***\
***                                                                        ***\
****************************************************************************\*/
static WORDLIST wordlist;
static std::string alphberts = "abcdefghijklmnopqrstuvwxyz";

/* this function will load word list from external file to the memeroy */
const WORDLIST loadWords(const std::string path){
    WORDLIST wl;
    std::ifstream infile(path.c_str());
    std::string line;
    while (std::getline(infile, line))
    {
        std::string word = line.substr(0,line.find("\n"));
        wl.push_back(word);
    }
    return wl;
}


/* this function will load word list from string */
const WORDLIST loadWords(){
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(wordliststr);
   while (std::getline(tokenStream, token, ','))
   {
      tokens.push_back(token);
   }
   return tokens;
}

/* return a list of word with 1 alphberts differs from original word */
inline const WORDLIST edit1(const std::string word){
    WORDLIST wl;
    std::string newword;
    //generate alphberts replace
    for(int j=0;j<word.length();++j){
        for(int i=0;i<alphberts.length();++i){
            std::string ww = word;
            wl.push_back( ww.replace(j,1,1,alphberts[i]) );
        }
    }
    // generate delete one alphberts
    for(int i=0;i<word.length();++i){
        std::string ww = word;
        wl.push_back( ww.erase(i,1) );
    }    
    return wl;
}

inline const WORDLIST edit2(const std::string word){
    WORDLIST wl;
    std::string newword;
    //generate two alphberts replace
    for(int j=0;j<word.length();++j){
        for(int k=0;k<word.length();++k){
            if(j == k){
                break;
            }
            for(int i=0;i<alphberts.length();++i){
                for(int l=0;l<alphberts.length();++l){
                    std::string ww = word;
                    ww.replace(j,1,1,alphberts[i]).replace(k,1,1,alphberts[l]); 
                    wl.push_back(ww);
                }
            }
        }
    }
    return wl;
}

/* this function try to find if a given word is valid */
inline const bool find(std::string word){
    WLITERATOR it = std::find(wordlist.begin(),wordlist.end(),word);
    if( it == wordlist.end() ){
        return false;
    }
    return true;
}

inline WLITERATOR chooseSmallOne(const WLITERATOR&a,const WLITERATOR&b,const WLITERATOR&zero){
    if( std::distance(zero,a  ) <= std::distance(zero,b) ){
        return a;
    }
    return b;
}

/* this function suggest a word based on given string */
const std::string suggest(std::string str){
    if(find(str)){
        return str;
    }
    WORDLIST wl1 = edit1(str);
    WORDLIST wl2 = edit2(str);

    WLITERATOR it;
    WLITERATOR minit = wordlist.end();
    //search in wl1
    for(WLITERATOR i=wl1.begin();i<wl1.end();++i){
        it = std::find(wordlist.begin(), wordlist.end(), *i);    
        if( it != wordlist.end() ){
            minit = chooseSmallOne(it,minit,wordlist.begin());
        }
    }
    if( minit != wordlist.end() ){
        return *minit;
    }

    //search in wl2
    minit = wordlist.end();
    for(WLITERATOR i=wl2.begin();i<wl2.end();++i){
        it = std::find(wordlist.begin(), wordlist.end(), *i);    
        if( it != wordlist.end() ){
            minit = chooseSmallOne(it,minit,wordlist.begin());
        }
    }
    if( minit != wordlist.end() ){
        return *minit;
    }

    //no suggest
    return str;
}

//

