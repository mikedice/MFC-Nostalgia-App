#include "pch.h"
#include "SensorRepository.h"
#include <afxinet.h>
#include "CHttpClient.h"
#include "nlohmann/include/single_include/nlohmann/json.hpp"
using json = nlohmann::json;

SensorRepository::SensorRepository()
{
    TRACE("SensorRepository::SensorRepository no initializer\n");
    this->pHttpClient = NULL;
    this->loadCallback = NULL;
    this->loadCallbackContext = 0;
}

SensorRepository::~SensorRepository()
{
    TRACE("SensorRepository::~SensorRepository\n");
    this->FreeHttpClient();
}

void SensorRepository::FreeHttpClient() {
    if (this->pHttpClient != NULL)
    {
        delete this->pHttpClient;
        this->pHttpClient = NULL;
    }
}

void SensorRepository::NotifyLoadComplete() {
    if (this->loadCallback != NULL) {
        this->loadCallback(this->loadCallbackContext);
    }
    this->loadCallback = NULL;
    this->loadCallbackContext = 0;
}

void SensorRepository::LoadData(LoadDataCompleteCallback callback, DWORD_PTR context)
{
    this->loadCallback = callback;
    this->loadCallbackContext = context;

    TRACE("SensorRepository::LoadData\n");

    try {
        this->pHttpClient = new CHttpClient();
        this->pHttpClient->OpenUrl(_T("http://mikedice.net/device"), [](BYTE* responseContent, DWORD_PTR appContext) {
                TRACE("SensorRepository received data %s\n", responseContent);
                // It is too bad the lambda can't give me access to the this pointer
                // which I would like it to be a SensorRepository*
                SensorRepository* pRepository = (SensorRepository*)appContext;

                auto jsonData = json::parse(responseContent);
                double temperatureF = (double)jsonData["farenheight"];
                double pressure = (double)jsonData["pressure"];
                unsigned long long ts = static_cast<unsigned long long>(jsonData["messageTimestamp"]);
                
                // A bunch of yuck to convert the JavaScript timestamp we got from the web service
                // into a time string converted for local time.
                const size_t bufferSize = 256;
                char timeString[bufferSize];
                
                CWeatherSensorData::ParseTimestamp(ts, timeString, bufferSize);
                pRepository->data.Add(CWeatherSensorData(pressure, temperatureF, timeString));
                
                // Clean up at end of Http request processing
                pRepository->FreeHttpClient();

                // Notify async caller that we are done loading data
                pRepository->NotifyLoadComplete();

            }, (DWORD_PTR)this);
    }
    catch (CInternetException* pExep)
    {
        TRACE("SensorRepository::LoadData Exception\n");
    }
}

CWeatherSensorData SensorRepository::GetData()
{
    TRACE("SensorRepository::GetData\n");
    return this->data[0];
}


void SensorRepository::ClearData()
{
    this->data.RemoveAll();
}
