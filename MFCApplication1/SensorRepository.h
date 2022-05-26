#pragma once
#include "CWeatherSensorData.h"
#include <atlcoll.h>
#include "CHttpClient.h"

typedef void LoadDataCompleteCallback(DWORD_PTR appContext);

class SensorRepository
{
public:
    SensorRepository();
    ~SensorRepository();
    void LoadData(LoadDataCompleteCallback, DWORD_PTR appContext); 

    CWeatherSensorData GetData();
private:
    LoadDataCompleteCallback* loadCallback;
    DWORD_PTR loadCallbackContext;

    void FreeHttpClient();
    void NotifyLoadComplete();
    CHttpClient* pHttpClient;
    CAtlArray<CWeatherSensorData> data;
public:
    void ClearData();
};

