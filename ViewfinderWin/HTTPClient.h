/*
	Class to handle HTTP GET requests for webpages and images.
	If an HTML resource is specified (RES_HTML) the client will download the source and save it in the SOURCE variable.
	If an image is specified (RES_IMG) the client will download the image data and save it as the FILE_NAME specified.
*/


#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <fstream>
#include <sstream>
#include <string>



#define RES_HTML 0
#define RES_IMG 1

class HTTPClient {
	std::ofstream imgFile;
	std::wstringstream buffer;
	std::wstring SERVER, RES, SOURCE;
	std::string FILE_NAME;
	int checkSourceErrors();
public:
	int download(const int TYPE);
	int setParams(const std::wstring &SERVER, const std::wstring &RES);
	void setDownName(const std::string &name) { FILE_NAME = name; }
	std::wstring getSource() { return SOURCE; }
	std::wstring getServer() { return SERVER; }
	std::wstring getRes() { return RES; }
};

#endif