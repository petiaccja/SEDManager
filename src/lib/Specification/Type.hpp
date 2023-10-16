#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>


class Type {
public:
    struct Storage {
        virtual ~Storage() {}
    };

    virtual ~Type() {}

protected:
    Type(std::shared_ptr<Storage> storage) : m_storage(std::move(storage)) {}

    template <class Out, class In>
    friend Out type_cast(const In& in);

    template <class In>
    friend uint64_t type_uid(const In& in);

    std::shared_ptr<Storage> m_storage;
};


struct TypeIdentifier {
    virtual ~TypeIdentifier() {}

    struct Storage {
        virtual uint64_t Id() const { return 0; }
    };
};


template <class BaseType, uint64_t Identifier>
class IdentifiedType : public BaseType {
public:
    struct Storage : BaseType::Storage, TypeIdentifier::Storage {
        using BaseType::Storage::Storage;
        constexpr uint64_t Id() const override { return Identifier; }
    };

    template <class... Args>
        requires std::is_constructible_v<typename BaseType::Storage, Args...>
    IdentifiedType(Args&&... args) : BaseType(std::make_shared<Storage>(std::forward<Args>(args)...)) {}


protected:
    using BaseType::BaseType;
};


template <class Out, class In>
Out type_cast(const In& in) {
    const auto outStorage = std::dynamic_pointer_cast<typename Out::Storage>(in.m_storage);
    if (outStorage) {
        return Out(outStorage);
    }
    throw std::bad_cast();
}


template <class Type>
uint64_t type_uid(const Type& in) {
    const auto idStorage = std::dynamic_pointer_cast<typename TypeIdentifier::Storage>(in.m_storage);
    if (idStorage) {
        return idStorage->Id();
    }
    throw std::bad_cast();
}


//------------------------------------------------------------------------------
// Base types
//------------------------------------------------------------------------------

class IntegerType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(size_t width, bool signedness) : width(width), signedness(signedness) {}
        size_t width;
        bool signedness;
    };

    IntegerType(size_t width, bool signedness) : Type(std::make_shared<Storage>(width, signedness)) {}

    size_t Width() const { return std::dynamic_pointer_cast<Storage>(m_storage)->width; }
    bool Signedness() const { return std::dynamic_pointer_cast<Storage>(m_storage)->signedness; }

protected:
    using Type::Type;
};


class BytesType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(size_t length, bool fixed) : length(length), fixed(fixed) {}
        size_t length;
        bool fixed;
    };

    BytesType(size_t length, bool fixed) : Type(std::make_shared<Storage>(length, fixed)) {}

protected:
    using Type::Type;
};


//------------------------------------------------------------------------------
// Concrete base types
//------------------------------------------------------------------------------

class UnsignedIntType : public IntegerType {
public:
    struct Storage : IntegerType::Storage {
        Storage(size_t width) : IntegerType::Storage(width, false) {}
    };

    UnsignedIntType(size_t width) : IntegerType(std::make_shared<Storage>(width)) {}

protected:
    using IntegerType::IntegerType;
};


class SignedIntType : public IntegerType {
public:
    struct Storage : IntegerType::Storage {
        Storage(size_t width) : IntegerType::Storage(width, true) {}
    };

    SignedIntType(size_t width) : IntegerType(std::make_shared<Storage>(width)) {}

protected:
    using IntegerType::IntegerType;
};


class CappedBytesType : public BytesType {
public:
    struct Storage : BytesType::Storage {
        Storage(size_t maxLength) : BytesType::Storage(maxLength, false) {}
    };

    CappedBytesType(size_t maxLength) : BytesType(std::make_shared<Storage>(maxLength)) {}

protected:
    using BytesType::BytesType;
};


class FixedBytesType : public BytesType {
public:
    struct Storage : BytesType::Storage {
        Storage(size_t length) : BytesType::Storage(length, true) {}
    };

    FixedBytesType(size_t length) : BytesType(std::make_shared<Storage>(length)) {}

protected:
    using BytesType::BytesType;
};


//------------------------------------------------------------------------------
// Compound types
//------------------------------------------------------------------------------

class EnumerationType : public UnsignedIntType {
public:
    struct Storage : UnsignedIntType::Storage {
        using UnsignedIntType::Storage::Storage;
        Storage(std::vector<std::pair<uint64_t, uint64_t>> ranges) : UnsignedIntType::Storage(GetLength(ranges)), ranges(ranges) {}
        std::vector<std::pair<uint64_t, uint64_t>> ranges;

        static size_t GetLength(std::span<const std::pair<uint64_t, uint64_t>> ranges) {
            // TODO: select the integer size that can represent all ranges.
            return 4;
        }
    };

    EnumerationType(std::vector<std::pair<uint64_t, uint64_t>> ranges) : UnsignedIntType(std::make_shared<Storage>(std::move(ranges))) {}
    std::span<const std::pair<uint64_t, uint64_t>> Ranges() const { return std::dynamic_pointer_cast<Storage>(m_storage)->ranges; }

protected:
    using UnsignedIntType::UnsignedIntType;
};


class AlternativeType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(std::vector<Type> types) : types(std::move(types)) {}
        std::vector<Type> types;
    };

    AlternativeType(std::vector<Type> types) : Type(std::make_shared<Storage>(std::move(types))) {}
    std::span<const Type> Types() const { return std::dynamic_pointer_cast<Storage>(m_storage)->types; }

protected:
    using Type::Type;
};


class ListType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(Type elementType) : elementType(std::move(elementType)) {}
        Type elementType;
    };

    ListType(Type elementType) : Type(std::make_shared<Storage>(std::move(elementType))) {}
    Type ElementType() const { return std::dynamic_pointer_cast<Storage>(m_storage)->elementType; }

protected:
    using Type::Type;
};


//------------------------------------------------------------------------------
// Identified types
//------------------------------------------------------------------------------

struct UInteger1 final : IdentifiedType<UnsignedIntType, 0x0000'0005'0000'0210> {
    UInteger1() : IdentifiedType(1) {}
};

struct UInteger2 final : IdentifiedType<UnsignedIntType, 0> {
    UInteger2() : IdentifiedType(2) {}
};

struct UInteger4 final : IdentifiedType<UnsignedIntType, 0> {
    UInteger4() : IdentifiedType(4) {}
};

struct UInteger8 final : IdentifiedType<UnsignedIntType, 0> {
    UInteger8() : IdentifiedType(8) {}
};

struct AnyUInteger final : IdentifiedType<AlternativeType, 0> {
    AnyUInteger() : IdentifiedType(std::vector<Type>{ UInteger1(), UInteger2(), UInteger4(), UInteger8() }) {}
};
