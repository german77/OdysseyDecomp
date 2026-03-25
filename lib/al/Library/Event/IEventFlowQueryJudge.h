#pragma once

namespace al {

class IEventFlowQueryJudge {
    virtual const char* judgeQuery(const char* query) const = 0;
};

}  // namespace al
