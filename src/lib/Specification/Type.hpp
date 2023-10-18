#pragma once

#include <cassert>
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

    template <class T>
    typename T::Storage& GetStorage() {
        const auto storage = std::dynamic_pointer_cast<typename T::Storage>(m_storage);
        assert(storage);
        return *storage;
    }

    template <class T>
    const typename T::Storage& GetStorage() const {
        const auto storage = std::dynamic_pointer_cast<const typename T::Storage>(m_storage);
        assert(storage);
        return *storage;
    }

    template <class Out, class In>
    friend Out type_cast(const In& in);

    template <class Out, class In>
    friend bool type_isa(const In& in);

    template <class In>
    friend uint64_t type_uid(const In& in);

private:
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
        template <class... Args>
            requires std::is_constructible_v<typename BaseType::Storage, Args...>
        Storage(Args&&... args) : BaseType::Storage(std::forward<Args>(args)...) {}

        constexpr uint64_t Id() const override { return Identifier; }
    };

    template <class... Args>
        requires std::is_constructible_v<Storage, Args...>
    IdentifiedType(Args&&... args) : BaseType(std::make_shared<Storage>(std::forward<Args>(args)...)) {}
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

    size_t Width() const { return GetStorage<IntegerType>().width; }
    bool Signedness() const { return GetStorage<IntegerType>().signedness; }

    explicit IntegerType(std::shared_ptr<Storage> s) : Type(std::move(s)) {}
};


class BytesType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(size_t length, bool fixed) : length(length), fixed(fixed) {}
        size_t length;
        bool fixed;
    };

    BytesType(size_t length, bool fixed) : Type(std::make_shared<Storage>(length, fixed)) {}

    size_t Length() const { return GetStorage<BytesType>().length; }
    bool Fixed() const { return GetStorage<BytesType>().fixed; }

    explicit BytesType(std::shared_ptr<Storage> s) : Type(std::move(s)) {}
};


//------------------------------------------------------------------------------
// Concrete base types
//------------------------------------------------------------------------------

class UnsignedIntType : public IntegerType {
public:
    struct Storage : IntegerType::Storage {
        Storage(size_t width) : IntegerType::Storage(width, false) {}
    };

    explicit UnsignedIntType(size_t width) : IntegerType(std::make_shared<Storage>(width)) {}

    explicit UnsignedIntType(std::shared_ptr<Storage> s) : IntegerType(std::move(s)) {}
};


class SignedIntType : public IntegerType {
public:
    struct Storage : IntegerType::Storage {
        Storage(size_t width) : IntegerType::Storage(width, true) {}
    };

    explicit SignedIntType(size_t width) : IntegerType(std::make_shared<Storage>(width)) {}

    explicit SignedIntType(std::shared_ptr<Storage> s) : IntegerType(std::move(s)) {}
};


class CappedBytesType : public BytesType {
public:
    struct Storage : BytesType::Storage {
        Storage(size_t maxLength) : BytesType::Storage(maxLength, false) {}
    };

    explicit CappedBytesType(size_t maxLength) : BytesType(std::make_shared<Storage>(maxLength)) {}

    explicit CappedBytesType(std::shared_ptr<Storage> s) : BytesType(std::move(s)) {}
};


class FixedBytesType : public BytesType {
public:
    struct Storage : BytesType::Storage {
        Storage(size_t length) : BytesType::Storage(length, true) {}
    };

    explicit FixedBytesType(size_t length) : BytesType(std::make_shared<Storage>(length)) {}

    explicit FixedBytesType(std::shared_ptr<Storage> s) : BytesType(std::move(s)) {}
};


//------------------------------------------------------------------------------
// Compound types
//------------------------------------------------------------------------------

class EnumerationType : public UnsignedIntType {
public:
    struct Storage : UnsignedIntType::Storage {
        Storage(std::vector<std::pair<uint16_t, uint16_t>> ranges) : UnsignedIntType::Storage(2), ranges(std::move(ranges)) {}
        Storage(std::pair<uint16_t, uint16_t> range) : Storage(std::vector{ range }) {}
        Storage(uint16_t lower, uint16_t upper) : Storage(std::pair{ lower, upper }) {}
        std::vector<std::pair<uint16_t, uint16_t>> ranges;
    };

    explicit EnumerationType(std::vector<std::pair<uint16_t, uint16_t>> ranges) : UnsignedIntType(std::make_shared<Storage>(std::move(ranges))) {}
    explicit EnumerationType(std::pair<uint16_t, uint16_t> range) : EnumerationType(std::vector{ range }) {}
    EnumerationType(uint16_t lower, uint16_t upper) : EnumerationType(std::pair{ lower, upper }) {}
    std::span<const std::pair<uint16_t, uint16_t>> Ranges() const { return GetStorage<EnumerationType>().ranges; }

    explicit EnumerationType(std::shared_ptr<Storage> s) : UnsignedIntType(std::move(s)) {}
};


class AlternativeType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(std::vector<Type> types) : types(std::move(types)) {}
        template <std::convertible_to<Type>... Types>
        Storage(Types&&... types) : Storage(std::vector<Type>{ std::forward<Types>(types)... }) {}
        std::vector<Type> types;
    };

    explicit AlternativeType(std::vector<Type> types) : Type(std::make_shared<Storage>(std::move(types))) {}
    template <std::convertible_to<Type>... Types>
    explicit AlternativeType(Types&&... types) : AlternativeType(std::vector<Type>{ std::forward<Types>(types)... }) {}
    std::span<const Type> Types() const { return GetStorage<AlternativeType>().types; }

    explicit AlternativeType(std::shared_ptr<Storage> s) : Type(std::move(s)) {}
};


class ListType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(const Type& elementType) : elementType(elementType) {}
        Type elementType;
    };

    explicit ListType(const Type& elementType) : Type(std::make_shared<Storage>(elementType)) {}
    Type ElementType() const { return GetStorage<ListType>().elementType; }

    explicit ListType(std::shared_ptr<Storage> s) : Type(std::move(s)) {}
};


class StructType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(std::vector<Type> elementTypes) : elementTypes(std::move(elementTypes)) {}
        template <std::convertible_to<Type>... Types>
        Storage(Types&&... types) : Storage(std::vector<Type>{ std::forward<Types>(types)... }) {}
        std::vector<Type> elementTypes;
    };

    explicit StructType(std::vector<Type> elementTypes) : Type(std::make_shared<Storage>(std::move(elementTypes))) {}
    template <std::convertible_to<Type>... Types>
    explicit StructType(Types&&... types) : StructType(std::vector<Type>{ std::forward<Types>(types)... }) {}
    std::span<const Type> ElementTypes() const { return GetStorage<StructType>().elementTypes; }

    explicit StructType(std::shared_ptr<Storage> s) : Type(std::move(s)) {}
};


class SetType : public ListType {
public:
    struct Storage : ListType::Storage {
        using ListType::Storage::Storage;
        Storage(std::vector<std::pair<uint16_t, uint16_t>> ranges) : ListType::Storage(UnsignedIntType(2)), ranges(std::move(ranges)) {}
        explicit Storage(std::pair<uint16_t, uint16_t> range) : Storage(std::vector{ range }) {}
        Storage(uint16_t lower, uint16_t upper) : Storage(std::pair{ lower, upper }) {}
        std::vector<std::pair<uint16_t, uint16_t>> ranges;
    };

    explicit SetType(std::vector<std::pair<uint16_t, uint16_t>> ranges) : ListType(std::make_shared<Storage>(std::move(ranges))) {}
    explicit SetType(std::pair<uint16_t, uint16_t> range) : SetType(std::vector{ range }) {}
    SetType(uint16_t lower, uint16_t upper) : SetType(std::pair{ lower, upper }) {}
    std::span<const std::pair<uint16_t, uint16_t>> Ranges() const { return GetStorage<SetType>().ranges; }

    explicit SetType(std::shared_ptr<Storage> s) : ListType(std::move(s)) {}
};


//------------------------------------------------------------------------------
// Reference types
//------------------------------------------------------------------------------


class RestrictedReferenceType : public Type {
public:
    struct Storage : Type::Storage {
        Storage(std::vector<uint64_t> tables) : tables(std::move(tables)) {}
        Storage(uint64_t table) : Storage(std::vector{ table }) {}
        std::vector<uint64_t> tables;
    };

    explicit RestrictedReferenceType(std::vector<uint64_t> tables) : Type(std::make_shared<Storage>(std::move(tables))) {}
    explicit RestrictedReferenceType(uint64_t table) : RestrictedReferenceType(std::vector{ table }) {}

    std::span<const uint64_t> Tables() const { return GetStorage<RestrictedReferenceType>().tables; }

    explicit RestrictedReferenceType(std::shared_ptr<Storage> s) : Type(std::move(s)) {}
};


class RestrictedByteReferenceType : public RestrictedReferenceType {
public:
    struct Storage : RestrictedReferenceType::Storage {
        using RestrictedReferenceType::Storage::Storage;
    };

    explicit RestrictedByteReferenceType(std::vector<uint64_t> tables) : RestrictedReferenceType(std::make_shared<Storage>(std::move(tables))) {}
    explicit RestrictedByteReferenceType(uint64_t table) : RestrictedByteReferenceType(std::vector{ table }) {}

    explicit RestrictedByteReferenceType(std::shared_ptr<Storage> s) : RestrictedReferenceType(std::move(s)) {}
};


class RestrictedObjectReferenceType : public RestrictedReferenceType {
public:
    struct Storage : RestrictedReferenceType::Storage {
        using RestrictedReferenceType::Storage::Storage;
    };

    explicit RestrictedObjectReferenceType(std::vector<uint64_t> tables) : RestrictedReferenceType(std::make_shared<Storage>(std::move(tables))) {}
    explicit RestrictedObjectReferenceType(uint64_t table) : RestrictedObjectReferenceType(std::vector{ table }) {}

    explicit RestrictedObjectReferenceType(std::shared_ptr<Storage> s) : RestrictedReferenceType(std::move(s)) {}
};


class GeneralReferenceType : public Type {
public:
    struct Storage : Type::Storage {};

    GeneralReferenceType() : Type(std::make_shared<Storage>()) {}

    explicit GeneralReferenceType(std::shared_ptr<Storage> s) : Type(std::move(s)) {}
};

class GeneralByteReferenceType : public GeneralReferenceType {
public:
    struct Storage : GeneralReferenceType::Storage {};

    GeneralByteReferenceType() : GeneralReferenceType(std::make_shared<Storage>()) {}

    explicit GeneralByteReferenceType(std::shared_ptr<Storage> s) : GeneralReferenceType(std::move(s)) {}
};


class GeneralObjectReferenceType : public GeneralReferenceType {
public:
    struct Storage : GeneralReferenceType::Storage {};

    GeneralObjectReferenceType() : GeneralReferenceType(std::make_shared<Storage>()) {}

    explicit GeneralObjectReferenceType(std::shared_ptr<Storage> s) : GeneralReferenceType(std::move(s)) {}
};


class GeneralTableReferenceType : public GeneralReferenceType {
public:
    struct Storage : GeneralReferenceType::Storage {};

    GeneralTableReferenceType() : GeneralReferenceType(std::make_shared<Storage>()) {}

    explicit GeneralTableReferenceType(std::shared_ptr<Storage> s) : GeneralReferenceType(std::move(s)) {}
};


class GeneralByteTableReferenceType : public GeneralTableReferenceType {
public:
    struct Storage : GeneralTableReferenceType::Storage {};

    GeneralByteTableReferenceType() : GeneralTableReferenceType(std::make_shared<Storage>()) {}

    explicit GeneralByteTableReferenceType(std::shared_ptr<Storage> s) : GeneralTableReferenceType(std::move(s)) {}
};


class GeneralObjectTableReferenceType : public GeneralTableReferenceType {
public:
    struct Storage : GeneralTableReferenceType::Storage {};

    GeneralObjectTableReferenceType() : GeneralTableReferenceType(std::make_shared<Storage>()) {}

    explicit GeneralObjectTableReferenceType(std::shared_ptr<Storage> s) : GeneralTableReferenceType(std::move(s)) {}
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
    uint16_t NameType() const { return GetStorage<NameValueUintegerType>().name; }
    Type ValueType() const { return GetStorage<NameValueUintegerType>().valueType; }

    explicit NameValueUintegerType(std::shared_ptr<Storage> s) : Type(std::move(s)) {}
};