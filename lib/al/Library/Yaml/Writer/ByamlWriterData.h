#pragma once

#include <basis/seadTypes.h>
#include <container/seadTList.h>

namespace sead {
class WriteStream;
}

namespace al {

class ByamlWriterData {
public:
    virtual ~ByamlWriterData() = default;

    virtual void makeIndex() {}

    virtual u32 calcPackSize() const { return 4; }

    virtual u8 getTypeCode() const { return 0; }

    virtual bool isContainer() const { return false; }

    virtual void write(sead::WriteStream*) const;

    virtual void print(s32 recursionDepth) const {}

    void printIndent(s32) const;
};

class ByamlWriterBool : public ByamlWriterData {
public:
    ByamlWriterBool(bool value);
    u8 getTypeCode() const override;
    void write(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

private:
    bool mValue;
};

class ByamlWriterInt : public ByamlWriterData {
public:
    ByamlWriterInt(s32 value);
    u8 getTypeCode() const override;
    void write(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

private:
    s32 mValue;
};

class ByamlWriterFloat : public ByamlWriterData {
public:
    ByamlWriterFloat(f32 value);
    u8 getTypeCode() const override;
    void write(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

private:
    f32 mValue;
};

class ByamlWriterUInt : public ByamlWriterData {
public:
    ByamlWriterUInt(u32 value);
    u8 getTypeCode() const override;
    void write(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

private:
    u32 mValue;
};

class ByamlWriterNull : public ByamlWriterData {
public:
    ByamlWriterNull();
    u8 getTypeCode() const override;
    void write(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;
};

class ByamlWriterStringTable;

class ByamlWriterString : public ByamlWriterData {
public:
    ByamlWriterString(const char* string, ByamlWriterStringTable* stringTable);
    u8 getTypeCode() const override;
    void write(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

private:
    const char* mString;
    ByamlWriterStringTable* mStringTable;
};

class ByamlWriterBigDataList;

class ByamlWriterBigData : public ByamlWriterData {
public:
    ByamlWriterBigData(ByamlWriterBigDataList* list);
    ~ByamlWriterBigData() = default;
    void write(sead::WriteStream* stream) const override;

    virtual u32 calcBigDataSize() const { return 8; }

    virtual void writeBigData(sead::WriteStream*) const {}

    void setOffset(s32 offset) { mOffset = offset; }

private:
    ByamlWriterBigDataList* mList;
    s32 mOffset = 0;
};

class ByamlWriterInt64 : public ByamlWriterBigData {
public:
    ByamlWriterInt64(s64 value, ByamlWriterBigDataList* list);
    ~ByamlWriterInt64() = default;
    u8 getTypeCode() const override;
    void writeBigData(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

private:
    s64 mValue;
};

class ByamlWriterUInt64 : public ByamlWriterBigData {
public:
    ByamlWriterUInt64(u64 value, ByamlWriterBigDataList* list);
    ~ByamlWriterUInt64() = default;
    u8 getTypeCode() const override;
    void writeBigData(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

private:
    u64 mValue;
};

class ByamlWriterDouble : public ByamlWriterBigData {
public:
    ByamlWriterDouble(f64 value, ByamlWriterBigDataList* list);
    ~ByamlWriterDouble() = default;
    u8 getTypeCode() const override;
    void writeBigData(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

private:
    f64 mValue;
};

class ByamlWriterHash;
class ByamlWriterArray;
class ByamlWriterStringTable;

class ByamlWriterContainer : public ByamlWriterData {
public:
    bool isContainer() const override { return true; }

    virtual void addBool(const char*, bool) {}

    virtual void addInt(const char*, s32) {}

    virtual void addUInt(const char*, u32) {}

    virtual void addFloat(const char*, f32) {}

    virtual void addInt64(const char*, s64, ByamlWriterBigDataList*) {}

    virtual void addUInt64(const char*, u64, ByamlWriterBigDataList*) {}

    virtual void addDouble(const char*, f64, ByamlWriterBigDataList*) {}

    virtual void addString(const char*, const char*) {}

    virtual void addHash(const char*, ByamlWriterHash*) {}

    virtual void addArray(const char*, ByamlWriterArray*) {}

    virtual void addNull(const char*) {}

    virtual void addBool(bool) {}

    virtual void addInt(s32) {}

    virtual void addUInt(u32) {}

    virtual void addFloat(f32) {}

    virtual void addInt64(s64, ByamlWriterBigDataList*) {}

    virtual void addUInt64(u64, ByamlWriterBigDataList*) {}

    virtual void addDouble(f64, ByamlWriterBigDataList*) {}

    virtual void addString(const char*) {}

    virtual void addHash(ByamlWriterHash*) {}

    virtual void addArray(ByamlWriterArray*) {}

    virtual void addNull() {}

    virtual void writeContainer(sead::WriteStream*) const {}

    virtual bool isHash() const { return false; }

    virtual bool isArray() const { return false; }

    virtual void deleteData() {}

    s32 getOffset() const { return mOffset; }

    void setOffset(s32 offset) { mOffset = offset; }

private:
    s32 mOffset = 0;
};

class ByamlWriterArray : public ByamlWriterContainer {
public:
    ByamlWriterArray(ByamlWriterStringTable* stringTable);
    ~ByamlWriterArray();

    void deleteData() override;
    u32 calcPackSize() const override;

    void addData(ByamlWriterData* data);
    void addBool(bool value) override;
    void addInt(s32 value) override;
    void addUInt(u32 value) override;
    void addFloat(f32 value) override;
    void addInt64(s64 value, ByamlWriterBigDataList* list) override;
    void addUInt64(u64 value, ByamlWriterBigDataList* list) override;
    void addDouble(f64 value, ByamlWriterBigDataList* list) override;
    void addString(const char* value) override;
    void addHash(ByamlWriterHash* hash) override;
    void addArray(ByamlWriterArray* array) override;
    void addNull() override;

    u8 getTypeCode() const override;
    void writeContainer(sead::WriteStream* stream) const override;
    void write(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;

    bool isArray() const override { return true; }

private:
    sead::TList<ByamlWriterData*> mList;
    ByamlWriterStringTable* mStringTable;
};

static_assert(sizeof(ByamlWriterArray) == 0x30);

class ByamlWriterHashPair : public sead::ListNode {
public:
    ByamlWriterHashPair(const char* key, ByamlWriterData* value);

    const char* getKey() { return mKey; }

    ByamlWriterData* getValue() { return mValue; }

private:
    void* mSelfReference = this;
    void* unk = nullptr;
    const char* mKey;
    ByamlWriterData* mValue;
};

static_assert(sizeof(ByamlWriterHashPair) == 0x30);

class ByamlWriterHash : public ByamlWriterContainer {
public:
    ByamlWriterHash(ByamlWriterStringTable* stringTable1, ByamlWriterStringTable* stringTable2);
    ~ByamlWriterHash();

    void deleteData() override;  // TODO implementation missing
    u32 calcPackSize() const override;

    void addData(const char*, ByamlWriterData*);  // TODO implementation missing
    void addBool(const char* key, bool value) override;
    void addInt(const char* key, s32 value) override;
    void addUInt(const char* key, u32 value) override;
    void addFloat(const char* key, f32 value) override;
    void addInt64(const char* key, s64 value, ByamlWriterBigDataList* list) override;
    void addUInt64(const char* key, u64 value, ByamlWriterBigDataList* list) override;
    void addDouble(const char* key, f64 value, ByamlWriterBigDataList* list) override;
    void addString(const char* key, const char* value) override;
    void addHash(const char* key, ByamlWriterHash* value) override;
    void addArray(const char* key, ByamlWriterArray* value) override;
    void addNull(const char* key) override;

    u8 getTypeCode() const override;
    void writeContainer(sead::WriteStream*) const override;  // TODO implementation missing
    void write(sead::WriteStream* stream) const override;
    void print(s32 recursionDepth) const override;  // TODO implementation missing

    bool isHash() const override { return true; }

private:
    sead::TList<ByamlWriterHashPair> mList;  // TODO not really... it's something different here.
    ByamlWriterStringTable* mStringTable1;
    ByamlWriterStringTable* mStringTable2;
};

static_assert(sizeof(ByamlWriterHash) == 0x38);

}  // namespace al
