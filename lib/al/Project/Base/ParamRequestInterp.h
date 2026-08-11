#pragma once

#include <basis/seadTypes.h>

namespace al {
class ParameterObj;

class IUseRequestParam {
public:
    virtual const char* getParamName() const = 0;
    virtual ParameterObj* getParamObj() = 0;
    virtual const ParameterObj* getParamObj() const = 0;
    virtual bool isEqual(const IUseRequestParam& requestParam) const;
    virtual void copy(const IUseRequestParam& requestParam);
    virtual void copyInterp(const IUseRequestParam& requestParamStart,
                            const IUseRequestParam& requestParamEnd, f32 rate);
};

static_assert(sizeof(IUseRequestParam) == 0x8);

class ParamRequestInterp {
public:
    ParamRequestInterp();

    void calcRate() const;
    void clearRequest();
    void endInit();
    bool isRequested();
    bool requestParam(s32, s32, const al::IUseRequestParam&);
    void updateInterp();

    IUseRequestParam* getCurrentParam() const { return mCurrentParam; }

    template <class T>
    void initialize() {
        mCurrentParam = new T();
        _18 = new T();
        _20 = new T();
        _30 = new T();
    }

private:
    void requestParamDirect_(s32, const al::IUseRequestParam&);

    s32 _0;
    s32 _4;
    s32 _8;
    bool _c;
    bool _d;
    bool _e;
    IUseRequestParam* mCurrentParam;
    IUseRequestParam* _18;
    IUseRequestParam* _20;
    s32 _28;
    IUseRequestParam* _30;
};

static_assert(sizeof(ParamRequestInterp) == 0x38);

}  // namespace al
