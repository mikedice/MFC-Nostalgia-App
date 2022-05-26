#include "pch.h"
#include "CWeatherSensorData.h"

CWeatherSensorData::CWeatherSensorData()
{
    this->pressureValue = 0.0;
    this->tempFValue = 0.0;
    this->timestamp = NULL;
}

CWeatherSensorData::~CWeatherSensorData() {
    this->pressureValue = 0.0;
    this->tempFValue = 0.0;
    if (this->timestamp != NULL) {
        delete[] this->timestamp;
        this->timestamp == NULL;
    }
}

CWeatherSensorData::CWeatherSensorData(double pressureValue, double tempFValue, char* timeStamp)
{
    this->pressureValue = pressureValue;
    this->tempFValue = tempFValue;
    if (timeStamp != NULL) {
        size_t len = (strlen(timeStamp)+1) * sizeof (char);
        this->timestamp = new char[len];
        strcpy_s(this->timestamp, len, timeStamp);
    }
}

CWeatherSensorData::CWeatherSensorData(const CWeatherSensorData& other)
{
    this->pressureValue = other.pressureValue;
    this->tempFValue = other.tempFValue;
    if (other.timestamp != NULL) {
        size_t len = (strlen(other.timestamp) + 1) * sizeof(char);
        this->timestamp = new char[len + 1];
        strcpy_s(this->timestamp, len, other.timestamp);
    }
}

double CWeatherSensorData::GetPressure()
{
    return this->pressureValue;
}

double CWeatherSensorData::GetTempInFarenheight() 
{
    return this->tempFValue;
}

const char* CWeatherSensorData::GetTimestamp() 
{
    return this->timestamp;
}

/*static*/ bool CWeatherSensorData::ParseTimestamp(unsigned long long timestamp, char* buffer, size_t bufferLen)
{
    tm tmBuff;
    memset(&tmBuff, 0, sizeof(tm));

    // JavaScript timestamps are milliseconds since the Unix epoch
    // And the C runtime libraries operate on seconds since the Unix epoch
    // so conversion by divide by 1000 is required.
    __time64_t test = timestamp / 1000;
    errno_t err = _localtime64_s(&tmBuff, &test);
    if (err > 0) {
        strerror_s(buffer, bufferLen, err);
        return false;
    }
    
    memset(buffer, 0, bufferLen);
    err = asctime_s(buffer, bufferLen, &tmBuff);
    if (err > 0) {
        strerror_s(buffer, bufferLen, err);
        return false;
    }

    return true;
}