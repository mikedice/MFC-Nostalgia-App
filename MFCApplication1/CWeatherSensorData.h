#pragma once


// This model class represents a reading from a weather sensor. It holds the 
// weather sensor's barometric pressure reading, temperature in Farenheight
// reading and the timestamp of the reading. The timestamp is a string that has
// been created to represent the sensor's reading timestamp in the UI's local
// time zone.
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

