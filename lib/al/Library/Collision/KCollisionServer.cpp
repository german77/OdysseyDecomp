#include "Library/Collision/KCollisionServer.h"

#include "Library/Math/MathUtil.h"
#include "Library/Yaml/ByamlIter.h"

namespace al {

template <typename T>
const T* getData(const KCPrismHeader* header, u32 offset) {
    return reinterpret_cast<const T*>(uintptr_t(header) + offset);
}

void KCollisionServer::initKCollisionServer(void* data, const void* attributeData) {}

void KCollisionServer::setData(void* data) {}

const KCPrismHeader& KCollisionServer::getInnerKcl(s32 index) const {}

u32 KCollisionServer::getNumInnerKcl() const {}

const KCPrismHeader* KCollisionServer::getV1Header(s32 index) const {}

bool KCollisionServer::calcFarthestVertexDistance() {}

s32 KCollisionServer::getTriangleNum(const KCPrismHeader* header) const {}

const KCPrismData& KCollisionServer::getPrismData(u32 index, const KCPrismHeader* header) const {
    return getData<KCPrismData>(header, header->prismOffset)[(s32)index];
}

bool KCollisionServer::isNearParallelNormal(const KCPrismData* data,
                                            const KCPrismHeader* header) const {}

bool KCollisionServer::isNanPrism(const KCPrismData* data, const KCPrismHeader* header) const {
    if (sead::Mathf::isNan(data->length))
        return true;

    sead::Vector3f normalFace = getFaceNormal(data, header);
    sead::Vector3f normalA = getEdgeNormal1(data, header);
    sead::Vector3f normalB = getEdgeNormal2(data, header);
    sead::Vector3f normalC = getEdgeNormal3(data, header);
    sead::Vector3f pos = getVertexData(data->vertexIndex, header);

    sead::Vector3f firstResult;
    calcPosLocal(&firstResult, data, 1, header);

    if (firstResult.isNan() || normalFace.isNan() || normalA.isNan() || normalB.isNan() ||
        normalC.isNan() || pos.isNan())
        return true;

    sead::Vector3f secondResult;
    calcPosLocal(&secondResult, data, 2, header);
    return secondResult.isNan();
}

void calcPos(sead::Vector3f* out, const KCPrismData* data, const sead::Vector3f& position,
             const sead::Vector3f& a, const sead::Vector3f& b, const sead::Vector3f& c) {
    sead::Vector3f v21 = a.cross(b);
    f32 dot = v21.dot(c);

    f32 factor = data->length / sead::Mathf::max(dot, sead::Mathf::epsilon());
    out->setScaleAdd(factor, v21, position);
}

void KCollisionServer::calcPosLocal(sead::Vector3f* out, const KCPrismData* data, s32 location,
                                    const KCPrismHeader* header) const {
    switch (location) {
    case 0: {
        sead::Vector3f position = getVertexData(data->vertexIndex, header);
        *out = position;
        return;
    }
    case 1: {
        calcPos(out, data, getVertexData(data->vertexIndex, header), getFaceNormal(data, header),
                getEdgeNormal2(data, header), getEdgeNormal3(data, header));
        return;
    }
    case 2: {
        calcPos(out, data, getVertexData(data->vertexIndex, header), getEdgeNormal1(data, header),
                getFaceNormal(data, header), getEdgeNormal3(data, header));
        return;
    }
    default:
        *out = {0.0f, 0.0f, 0.0f};
        return;
    }
}

void KCollisionServer::getMinMax(sead::Vector3f* min, sead::Vector3f* max) const {}

void KCollisionServer::getAreaSpaceSize(sead::Vector3f* size, const KCPrismHeader* header) const {}

void KCollisionServer::getAreaSpaceSize(s32* sizeX, s32* sizeY, s32* sizeZ,
                                        const KCPrismHeader* header) const {}

void KCollisionServer::getAreaSpaceSize(sead::Vector3u* size, const KCPrismHeader* header) const {}

const KCPrismData* KCollisionServer::checkPoint(sead::Vector3f*, f32, f32*) {}

const u8* KCollisionServer::searchBlock(s32* widthShift, const sead::Vector3u& block,
                                        const KCPrismHeader* header) const {}

s32 KCollisionServer::checkSphere(const sead::Vector3f*, f32, f32, u32,
                                  sead::FixedRingBuffer<KCHitInfo, 512>* hits) {}

bool KCollisionServer::outCheckAndCalcArea(sead::Vector3u* blockMin, sead::Vector3u* blockMax,
                                           const sead::Vector3f& posMin,
                                           const sead::Vector3f& posMax,
                                           const KCPrismHeader* header) const {}

bool KCollisionServer::KCHitSphere(const KCPrismData* data, const KCPrismHeader* header,
                                   const sead::Vector3f*, f32, f32, f32*, u8*) {}

const KCPrismData* KCollisionServer::checkArrow(const sead::Vector3f&, const sead::Vector3f&,
                                                sead::FixedRingBuffer<KCHitInfo, 512>* hits, u32*,
                                                u32) const {}

void KCollisionServer::objectSpaceToAreaOffsetSpaceV3f(sead::Vector3f* areaOff,
                                                       const sead::Vector3f& objSpace,
                                                       const KCPrismHeader* header) const {}

bool KCollisionServer::isInsideMinMaxInAreaOffsetSpace(const sead::Vector3u& block,
                                                       const KCPrismHeader* header) const {}

bool KCollisionServer::KCHitArrow(const KCPrismData* data, const KCPrismHeader* header,
                                  const sead::Vector3f&, const sead::Vector3f&, f32*, u8*) const {}

s32 KCollisionServer::checkSphereForPlayer(const sead::Vector3f*, f32, f32, u32,
                                           sead::FixedRingBuffer<KCHitInfo, 512>*) {}

bool KCollisionServer::KCHitSphereForPlayer(const KCPrismData* data, const KCPrismHeader* header,
                                            const sead::Vector3f*, f32, f32, f32*, u8*) {}

s32 KCollisionServer::checkDisk(const sead::Vector3f*, f32, f32, const sead::Vector3f&, f32, u32,
                                sead::FixedRingBuffer<KCHitInfo, 512>*) {}

bool KCHitDisk(const KCPrismData* data, const KCPrismHeader* header, const sead::Vector3f*, f32,
               f32, f32, const sead::Vector3f&, f32*, u8*) {}

void KCollisionServer::searchPrism(
    sead::Vector3f* pos, f32 searchRadius,
    sead::IDelegate2<const KCPrismData*, const KCPrismHeader*>& callback) {}

void KCollisionServer::searchPrismMinMax(
    const sead::Vector3f& min, const sead::Vector3f& max,
    sead::IDelegate2<const KCPrismData*, const KCPrismHeader*>& callback) {}

void KCollisionServer::searchPrismArrow(
    const sead::Vector3f& pos1, const sead::Vector3f& pos2,
    sead::IDelegate2<const KCPrismData*, const KCPrismHeader*>& callback) {}

void KCollisionServer::searchPrismDisk(
    const sead::Vector3f&, const sead::Vector3f&, f32, f32,
    sead::IDelegate2<const KCPrismData*, const KCPrismHeader*>& callback) {}

bool KCollisionServer::isParallelNormal(const KCPrismData* data,
                                        const KCPrismHeader* header) const {}

const sead::Vector3f& KCollisionServer::getFaceNormal(const KCPrismData* data,
                                                      const KCPrismHeader* header) const {
    return getNormal(data->faceNormalIndex, header);
}

const sead::Vector3f& KCollisionServer::getEdgeNormal1(const KCPrismData* data,
                                                       const KCPrismHeader* header) const {
    return getNormal(data->edgeNormalIndex[0], header);
}

const sead::Vector3f& KCollisionServer::getEdgeNormal2(const KCPrismData* data,
                                                       const KCPrismHeader* header) const {
    return getNormal(data->edgeNormalIndex[1], header);
}

const sead::Vector3f& KCollisionServer::getEdgeNormal3(const KCPrismData* data,
                                                       const KCPrismHeader* header) const {
    return getNormal(data->edgeNormalIndex[2], header);
}

bool KCollisionServer::KCHitDisc(const KCPrismData* data, const KCPrismHeader* header,
                                 const sead::Vector3f&, const sead::Vector3f&, f32, f32,
                                 sead::Vector3f*, f32*) {}

s32 KCollisionServer::toIndex(const KCPrismData* data, const KCPrismHeader* header) const {}

const sead::Vector3f& KCollisionServer::getNormal(u32 index, const KCPrismHeader* header) const {
    return getData<sead::Vector3f>(header, header->normalsOffset)[(s32)index];
}

void KCollisionServer::calXvec(const sead::Vector3f* a, const sead::Vector3f* b,
                               sead::Vector3f* result) {
    result->x = a->z * b->y - a->y * b->z;
    result->y = a->x * b->z - a->z * b->x;
    result->z = a->y * b->x - a->x * b->y;
}

const sead::Vector3f& KCollisionServer::getVertexData(u32 index,
                                                      const KCPrismHeader* header) const {
    return getData<sead::Vector3f>(header, header->vertexOffset)[(s32)index];
}

u32 KCollisionServer::getVertexNum(const KCPrismHeader* header) const {
    return 0xAAAAAAAB * ((uintptr_t(header->normalsOffset) - header->vertexOffset) >> 2);
}

s32 KCollisionServer::getNormalNum(const KCPrismHeader* header) const {}

s32 KCollisionServer::getAttributeElementNum() const {
    return 0;
}

bool KCollisionServer::getAttributes(ByamlIter* destIter, u32 triIndex,
                                     const KCPrismHeader* header) const {}

bool KCollisionServer::getAttributes(ByamlIter* destIter, const KCPrismData* data) const {
    return mAttributeIter->tryGetIterByIndex(destIter, data->collisionType);
}

void KCollisionServer::objectSpaceToAreaOffsetSpace(sead::Vector3u* areaOffSpace,
                                                    const sead::Vector3f& objSpace,
                                                    const KCPrismHeader* header) const {}

void KCollisionServer::areaOffsetSpaceToObjectSpace(sead::Vector3f* objSpace,
                                                    const sead::Vector3u& areaOffSpace,
                                                    const KCPrismHeader* header) const {}

bool KCollisionServer::doBoxCheck(const sead::Vector3f*, const sead::Vector3f*, sead::Vector3u*,
                                  sead::Vector3u*, const KCPrismHeader* header) {}

s32 KCollisionServer::calcAreaBlockOffset(const sead::Vector3u& block,
                                          const KCPrismHeader* header) const {}

s32 KCollisionServer::calcChildBlockOffset(const sead::Vector3u& block, s32 shift) {}

u32 KCollisionServer::getBlockData(const u32* data, u32 offset) {
    return *reinterpret_cast<const u32*>(reinterpret_cast<const char*>(data) + offset);
}

}  // namespace al
