#include<string>
#include<vector>
#include<fstream>
#include<algorithm>
#include <sstream>
#include<iostream>
#include"wordslist.h"

#ifndef SPELL_CHECKER
#define SPELL_CHECKER
typedef std::vector<std::string> WORDLIST;
typedef std::vector<std::string>::iterator WLITERATOR;
#endif


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
static bool inited = false;

/* this function will load word list from external file to the memeroy */
const WORDLIST loadWords(const std::string& path){
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

/* this function try to find if a given word is valid */
inline const bool find(const std::string& word){
    if( (std::find(wordlist.begin(),wordlist.end(),word)) == wordlist.end() ){
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


/* return a list of word with 1 alphberts differs from original word */
inline const WORDLIST edit1(const std::string& word){
    WORDLIST wl;
    std::string newword;
    std::string ww = "";
    //generate alphberts replace
    for(int j=0;j<word.length();++j){
        for(auto&a : alphberts){
            ww = word;
            wl.push_back( ww.replace(j,1,1,a) );
        }
    }
    // generate delete one alphberts
    for(int i=0;i<word.length();++i){
        ww = word;
        wl.push_back( ww.erase(i,1) );
    }    
    return wl;
}

inline const std::string edit2(const std::string& word){
    //WORDLIST wl;
    std::string newword;
    std::string ww = "";
    WLITERATOR it,minit = wordlist.end();
    //generate two alphberts replace
    for(int j=0;j<word.length();++j){
        for(int k=0;k<word.length();++k){
            if(j == k){
                break;
            }
            for( auto&a : alphberts){
                for( auto&b : alphberts){
                    ww = word;
                    ww.replace(j,1,1,a).replace(k,1,1,b); 
                    it = std::find(wordlist.begin(), wordlist.end(), ww); 
                    if( it != wordlist.end() ){
                        minit = chooseSmallOne(it,minit,wordlist.begin());
                    }
                    //wl.push_back(ww);
                }
            }
        }
    }
    return *minit;
}

/* this function suggest a word based on given string */
inline const std::string suggest(const std::string& str){
    if(find(str)){
        return str;
    }
    WORDLIST wl1 = edit1(str);

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

    //return  edit2(str);

    //no suggest
    return str;
}

//
void initSpellChecker(){
    if(!inited){
        //wordlist = loadWords("8wfeq_list.txt");
        wordlist = loadWords();
    }
}
