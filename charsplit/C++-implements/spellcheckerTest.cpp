#include"spellchecker.h"


int main(int argc,char**argv){
    std::cout<<"--loading data...\n";
    initSpellChecker();
    std::cout<<"--words count="<<wordlist.size()<<std::endl;

    std::cout<<"--Test for find(string):\n";
    std::cout<<"\tfind('kanch')= "<<find("kanch")<<std::endl;
    std::cout<<"\tfind('is')= "<<find("is")<<std::endl;
    std::cout<<"\tfind('algorithm')= "<<find("algorithm")<<std::endl;
    std::cout<<"\tfind('this')= "<<find("this")<<std::endl;
    std::cout<<"\tfind('awesome')= "<<find("awesome")<<std::endl;

    std::cout<<std::endl<<"--Test for edit1('file'):\n";
    std::cout<<"\tedit1 suggest for file="<< edit1("file")<<std::endl;

    std::cout<<std::endl<<"--Test for edit2('file'):\n";
    ///std::cout<<"\tedit2 suggest for file="<<edit2("file")<<std::endl;

    std::cout<<std::endl<<"--Test for suggest(string):\n";
    std::cout<<"\tsuggest(segmentatiop)="<<suggest("segmentatiop")<<"\t\texpect: "<<"segmentation"<<std::endl;
    std::cout<<"\tsuggest(algopithmg)="<<suggest("algopithmg")<<"\t\texpect: "<<"algorithms"<<std::endl;
    std::cout<<"\tsuggest(valjdate)="<<suggest("valjdate")<<"\t\texpect: "<<"validate"<<std::endl;
    std::cout<<"\tsuggest(test)="<<suggest("test")<<"\t\texpect: "<<"test"<<std::endl;
    std::cout<<"\tsuggest(thlg)="<<suggest("thlg")<<"\t\texpect: "<<"this"<<std::endl;
    std::cout<<"\tsuggest(wopkg)="<<suggest("wopkg")<<"\t\texpect: "<<"works"<<std::endl;
    std::cout<<"\tsuggest(ig)="<<suggest("ig")<<"\t\texpect: "<<"is"<<std::endl;
    std::cout<<"\tsuggest(lg)="<<suggest("lg")<<"\t\texpect: "<<"it"<<std::endl;
    std::cout<<"\tsuggest(we)="<<suggest("we")<<"\t\texpect: "<<"we"<<std::endl;

    return 0;
}