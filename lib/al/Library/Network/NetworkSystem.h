#pragma once

namespace al {
class RankingDirector;
class DataStoreDirector;

class NetworkSystem {
public:
    DataStoreDirector* getDataStoreDirector() const { return mDataStoreDirector; }

    RankingDirector* getRankingDirector() const { return mRankingDirector; }

private:
    unsigned char _padding[0x30];
    DataStoreDirector* mDataStoreDirector;
    RankingDirector* mRankingDirector;
};
}  // namespace al
