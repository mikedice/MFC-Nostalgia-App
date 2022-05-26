#include "pch.h"
#include "CHttpClient.h"

CHttpClient::CHttpClient()
{
    this->inetHandle = NULL;
    this->inetHttpUrlHandle = NULL;
    this->status = None;
    this->lpbReadBuffer = NULL;
    this->currentReadIndex = 0;
}

CHttpClient::~CHttpClient()
{
    if (this->inetHandle != NULL) {
        InternetCloseHandle(this->inetHandle);
        this->inetHandle = NULL;
    }

    if (this->inetHttpUrlHandle != NULL) {
        InternetCloseHandle(this->inetHttpUrlHandle);
        this->inetHttpUrlHandle = NULL;
    }
}

void CHttpClient::OpenUrl(LPCTSTR lpszUrl, AppCallback *appCallback, DWORD_PTR appContext)
{
    this->appCallback = appCallback;
    this->appContext = appContext;
    // Everything this class does is async
    if (this->inetHandle == NULL) {
        this->inetHandle = InternetOpen(
            _T("CPP internet client"),
            INTERNET_OPEN_TYPE_DIRECT,
            NULL,
            NULL,
            INTERNET_FLAG_ASYNC
        );
    }

    if (this->inetHandle == NULL) {
        CString* pMessage = new CString();
        pMessage->Format(_T("Error opening internet handle %x"), ::GetLastError());
        THROW(pMessage);
    }

    if (INTERNET_INVALID_STATUS_CALLBACK == InternetSetStatusCallback(this->inetHandle, CHttpClient::InetCallback)) {
        CString* pMessage = new CString();
        pMessage->Format(_T("Error settings status callback %x"), ::GetLastError());
        THROW(pMessage);
    }

    this->status = OpeningUrl;
    this->inetHttpUrlHandle = InternetOpenUrl(this->inetHandle, lpszUrl, NULL, 0, 0, (DWORD_PTR)this);
    // Go Async
}

void CHttpClient::ProcessRequestComplete(LPINTERNET_ASYNC_RESULT pInternetAsyncResult)
{
    if (pInternetAsyncResult->dwError != ERROR_SUCCESS) {
        TRACE(_T("ProcessRequestComplete unsuccessful %d\n"), pInternetAsyncResult->dwError);
        this->Close();
    }

    if (this->status == OpeningUrl) {
        // This handle is how you get at the HTTP response
        HINTERNET httpResponse = (HINTERNET)pInternetAsyncResult->dwResult;

        // Requires a content-length header in the response
        DWORD contentLength = 0;
        if (!this->GetContentLength(httpResponse, &contentLength)) {
            this->FailRequest(httpResponse, _T("content-length response header is required and was not supplied\n"));
            return;
        }
        
        // Content-type header is requried by this class.
        LPTSTR contentType;
        if (!this->GetContentType(httpResponse, &contentType)) {
            this->FailRequest(httpResponse, _T("content-type response header is required and was not supplied\n"));
            return;
        }

        this->ReadContent(httpResponse, contentLength, contentType);
    }
}

bool CHttpClient::ReadContent(HINTERNET httpResponse, DWORD contentLength, LPTSTR contentType)
{
    if (contentLength > 0) {
        this->lpbReadBuffer = new BYTE[contentLength+1]; // plus one byte for the null terminator
        this->currentReadIndex = 0;
    }
    
    DWORD dwRead = 0;
    BOOL bResult = InternetReadFile(httpResponse, this->lpbReadBuffer, contentLength, &dwRead);
    this->status = Completed;

    this->lpbReadBuffer[contentLength] = '\0';
    TRACE(_T("Read result %d\n"), bResult);
    this->appCallback(this->lpbReadBuffer, this->appContext);

    return true;
}

void CHttpClient::Close() {

}


void CALLBACK CHttpClient::InetCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    InternetCookieHistory cookieHistory;
    //PREQUEST_CONTEXT ReqContext = (PREQUEST_CONTEXT)dwContext;
    CHttpClient* pHttpClient = (CHttpClient*)dwContext;

    UNREFERENCED_PARAMETER(dwStatusInformationLength);

    fprintf(stderr, "Callback Received for Handle %p \t", hInternet);

    switch (dwInternetStatus)
    {
    case INTERNET_STATUS_COOKIE_SENT:
        fprintf(stderr, "Status: Cookie found and will be sent with request\n");
        break;

    case INTERNET_STATUS_COOKIE_RECEIVED:
        fprintf(stderr, "Status: Cookie Received\n");
        break;

    case INTERNET_STATUS_COOKIE_HISTORY:

        fprintf(stderr, "Status: Cookie History\n");

        //ASYNC_ASSERT(lpvStatusInformation);
        //ASYNC_ASSERT(dwStatusInformationLength == sizeof(InternetCookieHistory));

        cookieHistory = *((InternetCookieHistory*)lpvStatusInformation);

        if (cookieHistory.fAccepted)
        {
            fprintf(stderr, "Cookie Accepted\n");
        }
        if (cookieHistory.fLeashed)
        {
            fprintf(stderr, "Cookie Leashed\n");
        }
        if (cookieHistory.fDowngraded)
        {
            fprintf(stderr, "Cookie Downgraded\n");
        }
        if (cookieHistory.fRejected)
        {
            fprintf(stderr, "Cookie Rejected\n");
        }


        break;

    case INTERNET_STATUS_CLOSING_CONNECTION:
        fprintf(stderr, "Status: Closing Connection\n");
        break;

    case INTERNET_STATUS_CONNECTED_TO_SERVER:
        fprintf(stderr, "Status: Connected to Server\n");
        break;

    case INTERNET_STATUS_CONNECTING_TO_SERVER:
        fprintf(stderr, "Status: Connecting to Server\n");
        break;

    case INTERNET_STATUS_CONNECTION_CLOSED:
        fprintf(stderr, "Status: Connection Closed\n");
        break;

    case INTERNET_STATUS_HANDLE_CLOSING:
        fprintf(stderr, "Status: Handle Closing\n");

        //
        // Signal the cleanup routine that it is
        // safe to cleanup the request context
        //

        //ASYNC_ASSERT(ReqContext);
        //SetEvent(ReqContext->CleanUpEvent);

        break;

    case INTERNET_STATUS_HANDLE_CREATED:
        //ASYNC_ASSERT(lpvStatusInformation);
        fprintf(stderr,
            "Handle %x created\n",
            ((LPINTERNET_ASYNC_RESULT)lpvStatusInformation)->dwResult);

        break;

    case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
        fprintf(stderr, "Status: Intermediate response\n");
        break;

    case INTERNET_STATUS_RECEIVING_RESPONSE:
        fprintf(stderr, "Status: Receiving Response\n");
        break;

    case INTERNET_STATUS_RESPONSE_RECEIVED:
        //ASYNC_ASSERT(lpvStatusInformation);
        //ASYNC_ASSERT(dwStatusInformationLength == sizeof(DWORD));

        fprintf(stderr, "Status: Response Received (%d Bytes)\n", *((LPDWORD)lpvStatusInformation));

        break;

    case INTERNET_STATUS_REDIRECT:
        fprintf(stderr, "Status: Redirect\n");
        break;

    case INTERNET_STATUS_REQUEST_COMPLETE:
        fprintf(stderr, "Status: Request complete\n");

        //ASYNC_ASSERT(lpvStatusInformation);

        pHttpClient->ProcessRequestComplete((LPINTERNET_ASYNC_RESULT)lpvStatusInformation);
        break;

    case INTERNET_STATUS_REQUEST_SENT:
        //ASYNC_ASSERT(lpvStatusInformation);
        //ASYNC_ASSERT(dwStatusInformationLength == sizeof(DWORD));

        fprintf(stderr, "Status: Request sent (%d Bytes)\n", *((LPDWORD)lpvStatusInformation));
        break;

    case INTERNET_STATUS_DETECTING_PROXY:
        fprintf(stderr, "Status: Detecting Proxy\n");
        break;

    case INTERNET_STATUS_RESOLVING_NAME:
        fprintf(stderr, "Status: Resolving Name\n");
        break;

    case INTERNET_STATUS_NAME_RESOLVED:
        fprintf(stderr, "Status: Name Resolved\n");
        break;

    case INTERNET_STATUS_SENDING_REQUEST:
        fprintf(stderr, "Status: Sending request\n");
        break;

    case INTERNET_STATUS_STATE_CHANGE:
        fprintf(stderr, "Status: State Change\n");
        break;

    case INTERNET_STATUS_P3P_HEADER:
        fprintf(stderr, "Status: Received P3P header\n");
        break;

    default:
        fprintf(stderr, "Status: Unknown (%d)\n", dwInternetStatus);
        break;
    }

    return;
}


// Read the content length from the HTTP response. Also
bool CHttpClient::GetContentLength(HINTERNET httpResponse, DWORD* length)
{
    DWORD contentLength = 0;
    DWORD contentLengthSize = sizeof(DWORD);
    BOOL infoFound = HttpQueryInfo(httpResponse, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentLength, &contentLengthSize, 0);
    if (infoFound && contentLength > 0) {
        *length = contentLength;
        return true;
    }
    return false;
}

void CHttpClient::FailRequest(HINTERNET httpResponse, LPTSTR message) {
    InternetCloseHandle(httpResponse);
    this->status = Error;
    TRACE(_T("Failing request with error %s\n"), message);
}

bool CHttpClient::GetContentType(HINTERNET httpResponse, LPTSTR* contentType)
{
    // Requires a content-type header in the response
    LPTSTR outputBuffer = NULL;
    DWORD bufferSize = 0;

    BOOL infoFound = HttpQueryInfo(httpResponse, HTTP_QUERY_CONTENT_TYPE, outputBuffer, &bufferSize, 0);

    if (!infoFound) {
        if (::GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
            return false;
        }
        
        if (bufferSize > 0) {
            outputBuffer = new TCHAR[bufferSize];
            infoFound = HttpQueryInfo(httpResponse, HTTP_QUERY_CONTENT_TYPE, outputBuffer, &bufferSize, 0);
            contentType = &outputBuffer;
            return true;
        }
    }
    return false;
}
