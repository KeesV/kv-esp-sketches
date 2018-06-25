#include "state.h"

State::State() {
    this->station1Watering = false;
    this->station2Watering = false;
    this->station3Watering = false;
    this->station4Watering = false;
    this->station5Watering = false;
    this->station6Watering = false;
    this->updated = false;
}

bool State::GetStationStatus(int stationNumber) {
    switch (stationNumber) {
        case 1:
            return this->station1Watering;
        break;
        case 2:
            return this->station1Watering;
        break;
        case 3:
            return this->station1Watering;
        break;
        case 4:
            return this->station1Watering;
        break;
        case 5:
            return this->station1Watering;
        break;
        case 6:
            return this->station1Watering;
        break;
    }
}

void State::SetStationStatus(int stationNumber, bool status) {
    switch (stationNumber) {
        case 1:
            if(status != this->station1Watering) {
                this->station1Watering = status;
                this->updated = true;
            }
        break;
        case 2:
            if(status != this->station2Watering) {
                this->station2Watering = status;
                this->updated = true;
            }
        break;
        case 3:
            if(status != this->station3Watering) {
                this->station3Watering = status;
                this->updated = true;
            }
        break;
        case 4:
            if(status != this->station4Watering) {
                this->station4Watering = status;
                this->updated = true;
            }
        break;
        case 5:
            if(status != this->station5Watering) {
                this->station5Watering = status;
                this->updated = true;
            }
        break;
        case 6:
            if(status != this->station6Watering) {
                this->station6Watering = status;
                this->updated = true;
            }
        break;
    }
}

void State::UpdateProcessed() {
    this->updated = false;
}