import sys

argv_count = len(sys.argv)

if argv_count <= 1:
	print("no args ,exit")
	exit()
elif argv_count in (2,):
    with open(sys.argv[1]) as f:
        data = f.read().replace("\n",",").replace("\r","")
        #data = data.split("\n")
        #data = [ "\"" + x + "\"" for x in data ]
        #call_str = "wordlist.push_back(@WORD);\n"
        #data = [ call_str.replace("@WORD",x) for x in data ]
        #data = "".join(data)
        with open("wordslist.h","w") as ff:
            raw_data = """
                #include<string>
                static std::string wordliststr = "%s";
                """ % data
            ff.write(raw_data)
