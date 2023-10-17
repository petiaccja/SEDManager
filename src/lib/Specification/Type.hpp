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


template <class Out, class In>
bool type_isa(const In& in) {
    return nullptr != std::dynamic_pointer_cast<typename Out::Storage>(in.m_storage);
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
        Storage(std::vector<std::pair<uint16_t, uint16_t>> ranges) : UnsignedIntType::Storage(2), ranges(std::move(ranges)) {}
        std::vector<std::pair<uint16_t, uint16_t>> ranges;
    };

    EnumerationType(std::vector<std::pair<uint16_t, uint16_t>> ranges) : UnsignedIntType(std::make_shared<Storage>(std::move(ranges))) {}
    EnumerationType(std::pair<uint16_t, uint16_t> range) : EnumerationType(std::vector{ range }) {}
    EnumerationType(uint16_t lower, uint16_t upper) : EnumerationType(std::pair{ lower, upper }) {}
    std::span<const std::pair<uint16_t, uint16_t>> Ranges() const { return std::dynamic_pointer_cast<Storage>(m_storage)->ranges; }

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
    template <std::convertible_to<Type>... Types>
    AlternativeType(Types&&... types) : AlternativeType(std::vector<Type>{ std::forward<Types>(types)... }) {}
    std::span<const Type> Types() const { return std::dynamic_pointer_cast<Storage>(m_storage)->types; }

protected:
    using Type::Type;
};


class ListType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(const Type& elementType) : elementType(elementType) {}
        Type elementType;
    };

    ListType(const Type& elementType) : Type(std::make_shared<Storage>(elementType)) {}
    Type ElementType() const { return std::dynamic_pointer_cast<Storage>(m_storage)->elementType; }

protected:
    using Type::Type;
};


class StructType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(std::vector<Type> elementTypes) : elementTypes(std::move(elementTypes)) {}
        std::vector<Type> elementTypes;
    };

    StructType(std::vector<Type> elementTypes) : Type(std::make_shared<Storage>(std::move(elementTypes))) {}
    template <std::convertible_to<Type>... Types>
    StructType(Types&&... types) : StructType(std::vector<Type>{ std::forward<Types>(types)... }) {}
    std::span<const Type> ElementTypes() const { return std::dynamic_pointer_cast<Storage>(m_storage)->elementTypes; }

protected:
    using Type::Type;
};


class SetType : public UnsignedIntType {
public:
    struct Storage : UnsignedIntType::Storage {
        using UnsignedIntType::Storage::Storage;
        Storage(std::vector<std::pair<uint16_t, uint16_t>> ranges) : UnsignedIntType::Storage(2), ranges(std::move(ranges)) {}
        std::vector<std::pair<uint16_t, uint16_t>> ranges;
    };

    SetType(std::vector<std::pair<uint16_t, uint16_t>> ranges) : UnsignedIntType(std::make_shared<Storage>(std::move(ranges))) {}
    SetType(std::pair<uint16_t, uint16_t> range) : SetType(std::vector{ range }) {}
    SetType(uint16_t lower, uint16_t upper) : SetType(std::pair{ lower, upper }) {}
    std::span<const std::pair<uint16_t, uint16_t>> Ranges() const { return std::dynamic_pointer_cast<Storage>(m_storage)->ranges; }

protected:
    using UnsignedIntType::UnsignedIntType;
};


//------------------------------------------------------------------------------
// Reference types
//------------------------------------------------------------------------------

class RestrictedByteReferenceType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(std::vector<uint64_t> tables) : tables(std::move(tables)) {}
        std::vector<uint64_t> tables;
    };

    RestrictedByteReferenceType(std::vector<uint64_t> tables) : Type(std::make_shared<Storage>(std::move(tables))) {}
    RestrictedByteReferenceType(uint64_t table) : RestrictedByteReferenceType(std::vector{ table }) {}

protected:
    using Type::Type;
};


class RestrictedObjectReferenceType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(std::vector<uint64_t> tables) : tables(std::move(tables)) {}
        std::vector<uint64_t> tables;
    };

    RestrictedObjectReferenceType(std::vector<uint64_t> tables) : Type(std::make_shared<Storage>(std::move(tables))) {}
    RestrictedObjectReferenceType(uint64_t table) : RestrictedObjectReferenceType(std::vector{ table }) {}

protected:
    using Type::Type;
};


class GeneralByteReferenceType : public Type {
public:
    struct Storage : Type::Storage {};

    GeneralByteReferenceType() : Type(std::make_shared<Storage>()) {}

protected:
    using Type::Type;
};


class GeneralObjectReferenceType : public Type {
public:
    struct Storage : Type::Storage {};

    GeneralObjectReferenceType() : Type(std::make_shared<Storage>()) {}

protected:
    using Type::Type;
};


class GeneralTableReferenceType : public Type {
public:
    struct Storage : Type::Storage {};

    GeneralTableReferenceType() : Type(std::make_shared<Storage>()) {}

protected:
    using Type::Type;
};


class GeneralByteTableReferenceType : public Type {
public:
    struct Storage : Type::Storage {};

    GeneralByteTableReferenceType() : Type(std::make_shared<Storage>()) {}

protected:
    using Type::Type;
};


class GeneralObjectTableReferenceType : public Type {
public:
    struct Storage : Type::Storage {};

    GeneralObjectTableReferenceType() : Type(std::make_shared<Storage>()) {}

protected:
    using Type::Type;
};



//------------------------------------------------------------------------------
// Named types
//------------------------------------------------------------------------------

class NameValueUintegerType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(uint16_t name, const Type& valueType) : name(name), valueType(valueType) {}
        uint16_t name;
        Type valueType;
    };

    NameValueUintegerType(uint16_t name, const Type& valueType) : Type(std::make_shared<Storage>(name, valueType)) {}
    uint16_t NameType() const { return std::dynamic_pointer_cast<Storage>(m_storage)->name; }
    Type ValueType() const { return std::dynamic_pointer_cast<Storage>(m_storage)->valueType; }

protected:
    using Type::Type;
};