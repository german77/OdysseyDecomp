#pragma once

namespace al {
class ValidatorBase {
public:
    virtual bool validate() = 0;
    virtual ValidatorBase* fnc_8() const = 0;
};


enum class ResultFlag :s32{ 
    False=0,
    True=1<<0,
    NotFound=1<<1,
    InvalidState=1<<2,
    All=InvalidState|NotFound|True,
};

inline ResultFlag operator|(ResultFlag a, ResultFlag b)
{ return static_cast<ResultFlag>(static_cast<s32>(a) | static_cast<s32>(b)); }

inline ResultFlag operator&(ResultFlag a, ResultFlag b)
{ return static_cast<ResultFlag>(static_cast<s32>(a) & static_cast<s32>(b)); } 

// Removes optimizations for proper match
inline ResultFlag blackBox(ResultFlag value) {
    __asm__("" : "+r"(value));
    return value;
}

inline bool evaluate(ResultFlag flag){
    return (blackBox(flag|ResultFlag::InvalidState) & ResultFlag::All) != ResultFlag::InvalidState;
}

}  // namespace al
