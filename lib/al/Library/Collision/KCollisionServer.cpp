#include "Library/Collision/KCollisionServer.h"

#include "Library/Math/MathUtil.h"
#include "Library/Yaml/ByamlIter.h"

namespace al {

template <typename T>
const T* getData(const KCPrismHeader* header, u32 offset) {
    return reinterpret_cast<const T*>(uintptr_t(header) + offset);
}

template <typename T>
T* getData(void* data, u32 offset) {
    return reinterpret_cast<T*>(uintptr_t(data) + offset);
}

KCollisionServer::KCollisionServer() = default;

void KCollisionServer::initKCollisionServer(void* data, const void* attributeData) {
    setData(data);
    if (attributeData != nullptr)
        mAttributeIter = new ByamlIter((u8*)attributeData);
}

u32 getOctreeOffset(u8* data) {
    return *getData<u32>(data, 0x4);
}

u32 getModelListOffset(u8* data) {
    return *getData<u32>(data, 0x8);
}

u32 getCoordShiftX(u8* data) {
    return *getData<u32>(data, 0x28);
}

u32 getCoordShiftY(u8* data) {
    return *getData<u32>(data, 0x2C);
}

u32 getCoordShiftZ(u8* data) {
    return *getData<u32>(data, 0x30);
}

void KCollisionServer::setData(void* data) {
    mData = (u8*)data;
    mModelsOffset = getData<u32>(mData, getModelListOffset(mData));
    mOctreeData = getData<OctreeData>(mData, getOctreeOffset(mData));

    mCoordShift.x = getCoordShiftX(mData) - 1;
    mCoordShift.y = getCoordShiftY(mData) - 1;
    mCoordShift.z = getCoordShiftZ(mData) - 1;
    unkShiftedByCoordShift.x = -1 << getCoordShiftX(mData);
    unkShiftedByCoordShift.y = -1 << getCoordShiftY(mData);
    unkShiftedByCoordShift.z = -1 << getCoordShiftZ(mData);

    mModelsData.allocBuffer(getNumInnerKcl(), nullptr);
    for (s32 i = 0; i < getNumInnerKcl(); i++)
        mModelsData.pushBack(getInnerKcl(i));
}

KCPrismHeader* KCollisionServer::getInnerKcl(s32 index) const {
    return reinterpret_cast<KCPrismHeader*>(mData + mModelsOffset[index]);
}

s32 KCollisionServer::getNumInnerKcl() const {
    return *getData<u32>(mData, 0xC);
}

const KCPrismHeader* KCollisionServer::getV1Header(s32 index) const {
    return mModelsData[index];
}

bool KCollisionServer::calcFarthestVertexDistance() {
    f32 maxSquaredLength = 0.0f;
    bool isFirstModel = true;
    for (int i = 0; i < getNumInnerKcl(); i++) {
        const KCPrismHeader* header = getV1Header(i);
        s32 numTriangles = getTriangleNum(header);
        for (int j = 0; j < numTriangles; j++) {
            const KCPrismData& data = getPrismData(j, header);
            if (isNearParallelNormal(&data, header)) {
                const_cast<KCPrismData&>(data).length = -sead::Mathf::abs(data.length);
                break;
            }
            if (isNanPrism(&data, header)) {
                f32 v16 = sead::Mathf::abs(data.length);
                if (!isnan(data.length))
                    const_cast<KCPrismData&>(data).length = 0.0f;
                else
                    const_cast<KCPrismData&>(data).length = -v16;
                break;
            }

            sead::Vector3f pos;
            calcPosLocal(&pos, &data, 0, header);
            if (pos.squaredLength() > maxSquaredLength)
                maxSquaredLength = pos.squaredLength();
            calcPosLocal(&pos, &data, 1, header);
            if (pos.squaredLength() > maxSquaredLength)
                maxSquaredLength = pos.squaredLength();
            calcPosLocal(&pos, &data, 2, header);
            if (pos.squaredLength() > maxSquaredLength)
                maxSquaredLength = pos.squaredLength();
        }
        isFirstModel = false;
    }
    mFarthestVertexDistance = sead::Mathf::sqrt(maxSquaredLength);
    return isFirstModel;
}

s32 KCollisionServer::getTriangleNum(const KCPrismHeader* header) const {
    return (header->octreeOffset - (u64)header->prismOffset) / sizeof(KCPrismData);
}

const KCPrismData& KCollisionServer::getPrismData(u32 index, const KCPrismHeader* header) const {
    return getData<KCPrismData>(header, header->prismOffset)[(s32)index];
}

bool KCollisionServer::isNearParallelNormal(const KCPrismData* data,
                                            const KCPrismHeader* header) const {
    sead::Vector3f edge1 = getEdgeNormal1(data, header);
    sead::Vector3f edge2 = getEdgeNormal2(data, header);
    sead::Vector3f edge3 = getEdgeNormal3(data, header);
    return isParallelDirection(edge1, edge2, 0.01f) || isParallelDirection(edge1, edge3, 0.01f) ||
           isParallelDirection(edge2, edge3, 0.01f);
}

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
    return (sead::Vector3f*)header->normalsOffset - (sead::Vector3f*)header->vertexOffset;
}

u32 getPrismNum(const KCPrismHeader* header) {
    return ((u64)header->octreeOffset - (u64)header->prismOffset )/ sizeof(KCPrismData);
}

u32 KCollisionServer::getNormalNum(const KCPrismHeader* header) const {
    return 4 * getPrismNum(header);
}

s32 KCollisionServer::getAttributeElementNum() const {
    return 0;
}

bool KCollisionServer::getAttributes(ByamlIter* iter, u32 prismIndex,
                                     const KCPrismHeader* header) const {
    return mAttributeIter->tryGetIterByIndex(iter, getPrismData(prismIndex, header).collisionType);
}

bool KCollisionServer::getAttributes(ByamlIter* iter, const KCPrismData* data) const {
    return mAttributeIter->tryGetIterByIndex(iter, data->collisionType);
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
