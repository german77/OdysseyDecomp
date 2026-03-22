#pragma once

namespace al {
class EffectSystem;
class NatureDirector;
}  // namespace al

namespace alEffectEnvUtil {
void setNatureDirector(al::EffectSystem* effectSystem, al::NatureDirector* natureDirector);
void resetNatureDirector(al::EffectSystem* effectSystem);
}  // namespace alEffectEnvUtil
