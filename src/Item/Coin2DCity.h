#pragma once

#include "Library/LiveActor/LiveActor.h"

#include "Util/IUseDimension.h"

class Coin2DCity : public al::LiveActor, public IUseDimension {
public:
    Coin2DCity(const char* name, Coin2DCityDirector* director);

    void init(const al::ActorInitInfo& initInfo) override;
    void control() override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    ActorDimensionKeeper* getActorDimensionKeeper() const override;
    

startLight()
endLight()
exeWait()
exeLight()
exeGot()
exeGotWait()

private:
    char _110[0x18];
};
