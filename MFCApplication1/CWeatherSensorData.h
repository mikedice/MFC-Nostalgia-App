#pragma once


class CWeatherSensorData
{
public:
    CWeatherSensorData();
    ~CWeatherSensorData();
    CWeatherSensorData(double pressureValue, double tempFValue, char* timeStamp);
    CWeatherSensorData(const CWeatherSensorData& other);

    double GetPressure();
    double GetTempInFarenheight();
    const char* GetTimestamp();

    static bool ParseTimestamp(unsigned long long timestamp, char* buffer, size_t bufferLen);

private:
    float pressureValue;
    float tempFValue;
    char* timestamp;
};

