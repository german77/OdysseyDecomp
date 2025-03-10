#pragma once

#include "Library/Layout/LayoutActor.h"

struct MapData;

class MapTerrainLayout : public al::LayoutActor {
public:
    MapTerrainLayout(const char* name);

    bool tryChangePrintWorld(s32 worldId);
    f32 getPaneSize() const;

    MapData* getMapData() const { return mMapData; }

private:
    MapData* mMapData = nullptr;
};
