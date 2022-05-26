#pragma once


// This model class represents a single reading from a weather sensor. It holds the 
// weather sensor's barometric pressure reading in inches of mercury, 
// temperature in Farenheight and the timestamp of the reading as recorded by the sensor.
// The timestamp is a string that was created by converting the sensor's reading timestamp
// to the user's local time.
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

    // Helper to conver the sensor's timestamp to the user's local time.
    static bool ParseTimestamp(unsigned long long timestamp, char* buffer, size_t bufferLen);

private:
    float pressureValue;
    float tempFValue;
    char* timestamp;
};

