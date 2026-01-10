#pragma once

namespace al {
class LiveActor;
class PlacementInfo;

class PlacementClippingExpander {
public:
    PlacementClippingExpander();
    void init(LiveActor*, const PlacementInfo& info);

private:
    char filler[0x20];
};

static_assert(sizeof(PlacementClippingExpander) == 0x20);
}  // namespace al
