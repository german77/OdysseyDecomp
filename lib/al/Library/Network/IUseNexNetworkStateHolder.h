#pragma once

namespace al {

class NexNetworkStateHolder;

class IUseNexNetworkStateHolder {
public:
    virtual ~IUseNexNetworkStateHolder() = 0;
    virtual NexNetworkStateHolder* getNexNetworkStateHolder() const = 0;
};
}  // namespace al
