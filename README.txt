Hello!

-The master branch contains the implemented solution with some features.
-The parallelDL branch contains the solution with a parallel download implementation but the program crashes.
	//Didn't have the time to debug it, this was a fast solution - ideally a derived Thread class would be better

-I did the assingment using the cURL library. 
-If you want to compile my code you will have to install the library.
	-I have provided the .zip file containing the latest version of libcurl
	-Working library install instructions for Visual Studio 2015 on a 64bit Windows 10 system are:
		
	A. Build libcurl static library
		Download the latest curl generic source from: https://curl.haxx.se/download.html //provided in the repository
		Extract the source to a local directory (e.g. C:\libcurl)
		Open a command prompt
		"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat" To initialize your VC environment variables (adjust your VS 2013 installation directory as needed)
		cd C:\libcurl\winbuild
		nmake /f Makefile.vc mode=static VC=12
		The build should appear in C:\libcurl\builds\libcurl-vc12-x86-release-static-ipv6-sspi-winssl
	B. Link Against libcurl in Visual Studio
		In Visual Studio, right click your project in Solution Explorer, then click "Properties"
		Configuration Properties > C/C++ > General > Additional Include Directories: add C:\libcurl\builds\libcurl-vc12-x86-release-static-ipv6-sspi-winssl\include
		Configuration Properties > C/C++ > Preprocessor > Preprocessor Definitions: add CURL_STATICLIB
		Configuration Properties > Linker > General > Additional Library Directories: add C:\libcurl\builds\libcurl-vc12-x86-release-static-ipv6-sspi-winssl\lib
		Configuration Properties > Linker > Input > Additional Dependencies: add libcurl_a.lib
