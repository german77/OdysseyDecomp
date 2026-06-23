#pragma once

#include <container/seadPtrArray.h>
#include <container/seadRingBuffer.h>
#include <math/seadVector.h>
#include <prim/seadDelegate.h>

namespace al {
class ByamlIter;

struct KCPrismHeader {
    u32 vertexOffset;
    u32 normalsOffset;
    u32 prismOffset;
    u32 octreeOffset;
    f32 thickness;
    sead::Vector3f octreeOrigin;
    sead::Vector3u widthMask;
    sead::Vector3u widthShift;
    f32 hitboxRadiusCap;
};

struct KCPrismData {
    f32 length;
    u16 vertexIndex;
    u16 faceNormalIndex;
    u16 edgeNormalIndex[3];
    u16 collisionType;
    u32 triIndex;
};

static_assert(sizeof(KCPrismData) == 0x14);

struct KCHitInfo {
    const KCPrismHeader* header;
    const KCPrismData* data;
    f32 _10;
    u8 _14;  // collision location, enum
};

struct ModelListData {
    char _0;
};

struct OctreeData {
    char _0;
};

class KCollisionServer {
public:
    KCollisionServer();

    void initKCollisionServer(void* data, const void* attributeData);
    void setData(void* data);
    KCPrismHeader* getInnerKcl(s32 index) const;
    s32 getNumInnerKcl() const;
    const KCPrismHeader* getV1Header(s32 index) const;
    bool calcFarthestVertexDistance();
    s32 getTriangleNum(const KCPrismHeader* header) const;
    const KCPrismData& getPrismData(u32 index, const KCPrismHeader* header) const;
    bool isNearParallelNormal(const KCPrismData* data, const KCPrismHeader* header) const;
    bool isNanPrism(const KCPrismData* data, const KCPrismHeader* header) const;
    void calcPosLocal(sead::Vector3f* pos, const KCPrismData* data, s32 location,
                      const KCPrismHeader* header) const;
    void getMinMax(sead::Vector3f* min, sead::Vector3f* max) const;
    void getAreaSpaceSize(sead::Vector3f* size, const KCPrismHeader* header) const;
    void getAreaSpaceSize(s32* sizeX, s32* sizeY, s32* sizeZ, const KCPrismHeader* header) const;
    void getAreaSpaceSize(sead::Vector3u* size, const KCPrismHeader* header) const;
    const KCPrismData* checkPoint(sead::Vector3f*, f32, f32*);  // TODO unknown parameter usage
    const u8* searchBlock(s32* widthShift, const sead::Vector3u& block,
                          const KCPrismHeader* header) const;
    s32 checkSphere(const sead::Vector3f*, f32, f32, u32,
                    sead::FixedRingBuffer<KCHitInfo, 512>* hits);  // TODO unknown parameter usage
    bool outCheckAndCalcArea(sead::Vector3u* blockMin, sead::Vector3u* blockMax,
                             const sead::Vector3f& posMin, const sead::Vector3f& posMax,
                             const KCPrismHeader* header) const;
    bool KCHitSphere(const KCPrismData* data, const KCPrismHeader* header, const sead::Vector3f*,
                     f32, f32, f32*, u8*);  // TODO unknown parameter usage
    const KCPrismData* checkArrow(const sead::Vector3f&, const sead::Vector3f&,
                                  sead::FixedRingBuffer<KCHitInfo, 512>* hits, u32*,
                                  u32) const;  // TODO unknown parameter usage
    void objectSpaceToAreaOffsetSpaceV3f(sead::Vector3f* areaOff, const sead::Vector3f& objSpace,
                                         const KCPrismHeader* header) const;
    bool isInsideMinMaxInAreaOffsetSpace(const sead::Vector3u& block,
                                         const KCPrismHeader* header) const;
    bool KCHitArrow(const KCPrismData* data, const KCPrismHeader* header, const sead::Vector3f&,
                    const sead::Vector3f&, f32*, u8*) const;  // TODO unknown parameter usage
    s32
    checkSphereForPlayer(const sead::Vector3f*, f32, f32, u32,
                         sead::FixedRingBuffer<KCHitInfo, 512>*);  // TODO unknown parameter usage
    bool KCHitSphereForPlayer(const KCPrismData* data, const KCPrismHeader* header,
                              const sead::Vector3f*, f32, f32, f32*,
                              u8*);  // TODO unknown parameter usage
    s32 checkDisk(const sead::Vector3f*, f32, f32, const sead::Vector3f&, f32, u32,
                  sead::FixedRingBuffer<KCHitInfo, 512>*);  // TODO unknown parameter usage
    bool KCHitDisk(const KCPrismData* data, const KCPrismHeader* header, const sead::Vector3f*, f32,
                   f32, f32, const sead::Vector3f&, f32*, u8*);  // TODO unknown parameter usage
    void searchPrism(sead::Vector3f* pos, f32 searchRadius,
                     sead::IDelegate2<const KCPrismData*, const KCPrismHeader*>& callback);
    void searchPrismMinMax(const sead::Vector3f& min, const sead::Vector3f& max,
                           sead::IDelegate2<const KCPrismData*, const KCPrismHeader*>& callback);
    void searchPrismArrow(const sead::Vector3f& pos1, const sead::Vector3f& pos2,
                          sead::IDelegate2<const KCPrismData*, const KCPrismHeader*>& callback);
    void searchPrismDisk(const sead::Vector3f&, const sead::Vector3f&, f32, f32,
                         sead::IDelegate2<const KCPrismData*, const KCPrismHeader*>&
                             callback);  // TODO unknown parameter usage
    bool isParallelNormal(const KCPrismData* data, const KCPrismHeader* header) const;
    const sead::Vector3f& getFaceNormal(const KCPrismData* data, const KCPrismHeader* header) const;
    const sead::Vector3f& getEdgeNormal1(const KCPrismData* data,
                                         const KCPrismHeader* header) const;
    const sead::Vector3f& getEdgeNormal2(const KCPrismData* data,
                                         const KCPrismHeader* header) const;
    const sead::Vector3f& getEdgeNormal3(const KCPrismData* data,
                                         const KCPrismHeader* header) const;
    bool KCHitDisc(const KCPrismData* data, const KCPrismHeader* header, const sead::Vector3f&,
                   const sead::Vector3f&, f32, f32, sead::Vector3f*,
                   f32*);  // TODO unknown parameter usage
    s32 toIndex(const KCPrismData* data, const KCPrismHeader* header) const;
    const sead::Vector3f& getNormal(u32 index, const KCPrismHeader* header) const;

    static void calXvec(const sead::Vector3f* a, const sead::Vector3f* b, sead::Vector3f* result);

    const sead::Vector3f& getVertexData(u32 index, const KCPrismHeader* header) const;
    u32 getVertexNum(const KCPrismHeader* header) const;
    u32 getNormalNum(const KCPrismHeader* header) const;
    s32 getAttributeElementNum() const;
    bool getAttributes(ByamlIter* destIter, u32 triIndex, const KCPrismHeader* header) const;
    bool getAttributes(ByamlIter* destIter, const KCPrismData* data) const;
    void objectSpaceToAreaOffsetSpace(sead::Vector3u* areaOffSpace, const sead::Vector3f& objSpace,
                                      const KCPrismHeader* header) const;
    void areaOffsetSpaceToObjectSpace(sead::Vector3f* objSpace, const sead::Vector3u& areaOffSpace,
                                      const KCPrismHeader* header) const;
    bool doBoxCheck(const sead::Vector3f*, const sead::Vector3f*, sead::Vector3u*, sead::Vector3u*,
                    const KCPrismHeader* header);  // TODO unknown parameter usage
    s32 calcAreaBlockOffset(const sead::Vector3u& block, const KCPrismHeader* header) const;

    static s32 calcChildBlockOffset(const sead::Vector3u& block, s32 shift);
    static u32 getBlockData(const u32* data, u32 offset);

private:
    sead::PtrArray<KCPrismHeader> mModelsData;
    u8* mData = nullptr;
    ByamlIter* mAttributeIter = nullptr;
    u32* mModelsOffset = nullptr;
    OctreeData* mOctreeData = nullptr;
    sead::Vector3u mCoordShift = {0, 0, 0};
    sead::Vector3u unkShiftedByCoordShift = {0, 0, 0};
    f32 mFarthestVertexDistance = 1.0f;
};

static_assert(sizeof(KCollisionServer) == 0x50);

}  // namespace al
