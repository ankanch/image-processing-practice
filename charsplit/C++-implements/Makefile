test: TextExtractorTest.cpp
	@echo off
	@echo make test program:
	g++ -o testapp.exe .\TextExtractorTest.cpp -Werror=return-type
	@echo done.

template: build_template.cpp
	@echo off
	@echo make templates building program:
	g++ -o template_builder.exe .\build_template.cpp  -Werror=return-type
	@echo done.

clean: 
	del .\cache\* /s
	del template_builder.exe testapp.exe log.txt

