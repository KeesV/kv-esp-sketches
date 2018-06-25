#ifndef __STATE_H__
#define __STATE_H__

class State {
    private:
    bool station1Watering;
    bool station2Watering;
    bool station3Watering;
    bool station4Watering;
    bool station5Watering;
    bool station6Watering;
    bool updated;

    public:
    State();
    bool GetStationStatus(int stationNumber);
    void SetStationStatus(int stationNumber, bool state);
    void UpdateProcessed();
};

#endif