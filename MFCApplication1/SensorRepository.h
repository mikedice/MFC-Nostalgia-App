#pragma once
#include "CWeatherSensorData.h"
#include <atlcoll.h>
#include "CHttpClient.h"

typedef void LoadDataCompleteCallback(DWORD_PTR appContext);

// This repository holds an array of weather sensor readings. The readings
// are fetched from a REST endpoint by the repository's LoadData method.
// The LoadData method, when called will clear the current readings in the
// repository and fetch a new set of readings from the REST endpoint. The
// REST data fetching is done using async WinInet (old school!)
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

