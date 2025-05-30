#pragma once

namespace nn::g3d {
class ModelObj;
}  // namespace nn::g3d

namespace al {
class InitResourceDataAnim;
class Resource;

struct AnimPlayerInitInfo {
    Resource* animRes;
    nn::g3d::ModelObj* modelObj;
    Resource* modelRes;
    InitResourceDataAnim* animResData;
};
}  // namespace al
