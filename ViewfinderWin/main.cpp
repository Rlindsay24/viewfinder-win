/*
*   Viewfinder 1.0
*   -------------------------
*
*   Created by Richard Lindsay, Released 21/09/18
*
*/

// TODO: Options file allowing easy change of subreddit
// TODO: Use regular expressions to find image resolution
// TODO: Reg error handling



#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include "HTTPClient.h"
#include <windows.h>
using namespace std;

wstring findURL(const wstring& body, size_t startpos, long long imgRank);
int setWallpaper(const char* path);


int main()
{
	const wstring SUBREDDIT = L"/r/earthporn/";
	const wstring WEBPAGE = L"old.reddit.com";
	const string DOWN_LOCATION = "wallpaper.bmp";

	HTTPClient httpHandle = HTTPClient();
	httpHandle.setDownName(DOWN_LOCATION);
	if (httpHandle.setParams(WEBPAGE, SUBREDDIT) != 0) {
		cout << "Error: Webpage not valid" << endl;
	}

	int code;
	code = httpHandle.download(RES_HTML);
	if (code != 0) {
		cout << "Error when retrieving webpage" << endl;
		switch (code) {
			case 1: cout << "Host not found" << endl;
			case 2: cout << "DNS error: Viewfinder cannot connect to the DNS server. Make sure you are connected to a network with internet access." << endl;
			case 3: cout << "Error: The specified subreddit is invalid.\n" << "Please reconfigure or select from a reccomended subreddit." << endl;
			case -1: cout << "An unknown error occured" << endl;
		}
		cout << "Viewfinder will attemppt to load the last image used." << endl;
		code = setWallpaper("wallpaper.bmp");
		if (code == 1) {
			return 0;
		}
		else {
			cout << "Wallpaper could not be set - could not locate the file, or insuficcient permissions." << endl;
			return -1;
		}
	}

	int n = 0;
    bool match = false;
    long long rankNum = 1;
	int startpos = 0, endpos = 0;
    wstring imageURL = L"";
    while (rankNum <= 25) {
        cout << rankNum << endl;
        imageURL = findURL(httpHandle.getSource(), startpos, rankNum);
		size_t pos = imageURL.find(L"/");
		httpHandle.setParams(imageURL.substr(0, pos), imageURL.substr(pos));	// Partition URL into server and resource and set the parameters
        code = httpHandle.download(RES_IMG);
        if (code == 1) {
            ++rankNum;      // If image could not be downloaded try again with next image
            continue;
        } else {
            code = setWallpaper("wallpaper.bmp");
			if (code == 1)
				break;
			else
				cout << "The wallpaper could not be set. Continuing..." << endl;
        }
        ++rankNum;
    }
    return 0;
}

wstring findURL(const wstring& body, size_t startpos, long long imgRank)	
{
    bool match = false;
    wstring searchTerm = L"", imgURL = L"";
    while  (match == false) {
        startpos = body.find(searchTerm, startpos);    // Only searches after last position
        searchTerm = L"data-url=\"";
        startpos = body.find(searchTerm, startpos) + 10;    // Finds the next instance of 'data-url=' (This should be the link to the image)
        searchTerm = L"\"";
        size_t endpos = body.find(searchTerm, startpos);      // Finds the next ", indicating the end of the URL
        imgURL = body.substr(startpos, (endpos-startpos));
        match = true;
    }
    // Append .jpg if it does not have a common extension, and does not end with / (Useful for imgur links)
    if (imgURL.substr(imgURL.size()-4, imgURL.size()-1) != L".jpg" &&
        imgURL.substr(imgURL.size()-4, imgURL.size()-1) != L".png" &&
        imgURL.substr(imgURL.size()-4, imgURL.size()-1) != L".bmp" &&
        *(imgURL.end()-1) != '/')
        imgURL = imgURL + L".jpg";
    return imgURL.substr(8);	// Crop out 'https://' from start of url and return
}

int setWallpaper(const char* path)
{
	// Set registry to 2 (Stretch)
	HKEY key;
	RegOpenKey(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop\\"), &key);
	RegSetValueEx(key, TEXT("WallpaperStyle"), 0, REG_SZ, (LPBYTE)"2", strlen("2") * sizeof(char));
	RegCloseKey(key);
	return SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)path, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
}