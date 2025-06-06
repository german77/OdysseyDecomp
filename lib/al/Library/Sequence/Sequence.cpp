#include "Library/Sequence/Sequence.h"

namespace al {

Sequence::Sequence(const char* name) : NerveExecutor(name), mName(name) {}

void Sequence::init(const SequenceInitInfo& initInfo) {}

void Sequence::kill() {
    mIsAlive = false;
}

bool Sequence::isDisposable() const {
    return true;
}

}  // namespace al
