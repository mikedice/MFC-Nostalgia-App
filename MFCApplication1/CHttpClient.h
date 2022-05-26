#pragma once
#include <WinInet.h>

typedef void AppCallback(BYTE* responseContent, DWORD_PTR appContext);

// This class encapsulates the funcitonality of an HTTP client used to fetch
// data from a REST endpoint. The HTTP operations are async using WinInet.
// The complete HTTP response is returned to the client in their AppCallback 
// as a null terminated string
class CHttpClient
{
public:
    CHttpClient();
    ~CHttpClient();

    void OpenUrl(LPCTSTR lpszUrl, AppCallback *callback, DWORD_PTR appContext);

    void Close();
private:
    enum HttpClientStatus {
        None,
        OpeningUrl,
        ReadingFile,
        Completed,
        Error
    };

    static void CALLBACK InetCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
        LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);

    void ProcessRequestComplete(LPINTERNET_ASYNC_RESULT pInternetAsyncResult);

    BYTE* lpbReadBuffer;
    int currentReadIndex;
    HttpClientStatus status;
    HINTERNET inetHandle;
    HINTERNET inetHttpUrlHandle;
    DWORD_PTR appContext;
    AppCallback* appCallback;

    // Read the content length from the HTTP response
    bool GetContentLength(HINTERNET httpResponse, DWORD* pLength);

    // Read the content type from the HTTP response
    bool GetContentType(HINTERNET httpResponse, LPTSTR* contentType);

    bool ReadContent(HINTERNET httpResponse, DWORD contentLength, LPTSTR contentType);

    // Fails the request by closing the internet handled and setting the status to Error
    void FailRequest(HINTERNET httpResponse, LPTSTR message);
};

