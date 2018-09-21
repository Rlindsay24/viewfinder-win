#pragma comment(lib, "winhttp.lib")

#include "HTTPClient.h"
#include <iostream>
#include <windows.h>
#include <winhttp.h>
using namespace std;

int HTTPClient::setParams(const wstring &s, const wstring &r)
{
	//TODO: Validation on parameter input
	SERVER = s;
	RES = r;
	return 0;
}

int HTTPClient::checkSourceErrors()
{
	if (SOURCE.substr(0, 5) == L"Error") {
		if (SOURCE.substr(7, 14) == L"Host not found")
			return 1;
		if (SOURCE.substr(7, 9) == L"DNS error")
			return 2;
		if (SOURCE.substr(18, 17) == L"Invalid subreddit")
			return 3;
		return -1;
	}
	else {
		return 0;
	}
}

int HTTPClient::download(const int TYPE)
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;
	BOOL  bResults = FALSE;
	HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;

	// Create handle and send request
	hSession = WinHttpOpen(L"ViewfinderWin", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession)
		hConnect = WinHttpConnect(hSession, SERVER.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", RES.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	// End the request
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);


	if (TYPE == RES_IMG) {
		imgFile.open(FILE_NAME, ofstream::binary);
	}
	// Keep checking for data until there is nothing left.
	if (bResults)
	{
		do
		{
			// Check for available data.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				printf("Error %u in WinHttpQueryDataAvailable.\n",
					GetLastError());

			// Allocate space for the buffer.
			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				dwSize = 0;
			}
			else
			{
				// Read the data.
				ZeroMemory(pszOutBuffer, dwSize + 1);

				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
					dwSize, &dwDownloaded))
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				else {
					// Write buffer to file or memory depending on TYPE
					if (TYPE == RES_IMG) {
						imgFile.write(pszOutBuffer, dwSize);
					}
					else if (TYPE == RES_HTML) {
						buffer << pszOutBuffer;
					}
				}


				// Free the memory allocated to the buffer.
				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}
	// Report any errors.
	if (!bResults)
		printf("Error %d has occurred.\n", GetLastError());

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
	if (TYPE == RES_IMG) {
		imgFile.close();
	}
	else if (TYPE == RES_HTML) {
		SOURCE = buffer.str();
	}
	return 0;
}
