#pragma once


#include <Data/NativeTypes.hpp>
#include <Specification/Module.hpp>
#include <TrustedPeripheral/Session.hpp>

#include <cstdint>
#include <memory>


namespace sedmgr {

template <bool Mutable>
class TableRow {
    friend class TableRow<!Mutable>;

public:
    class const_cell_reference {
    public:
        const_cell_reference(Uid object, uint32_t column, std::shared_ptr<Session> session)
            : m_row(object), m_column(column), m_session(session) {}
        Value operator*() const { return m_session->base.Get(m_row, m_column); }

    protected:
        Uid m_row = 0;
        uint32_t m_column = 0;
        std::shared_ptr<Session> m_session = nullptr;
    };

    class mutable_cell_reference : const_cell_reference {
    public:
        using const_cell_reference::const_cell_reference;
        using const_cell_reference::operator*;
        mutable_cell_reference& operator=(const Value& value) {
            this->m_session->base.Set(this->m_row, this->m_column, value);
            return *this;
        }
    };

    template <bool Mutable_>
    using cell_reference = std::conditional_t<Mutable_, mutable_cell_reference, const_cell_reference>;

    template <bool Mutable_>
    class column_iterator {
    public:
        using value_type = cell_reference<Mutable_>;
        using difference_type = ptrdiff_t;

    public:
        column_iterator() {}
        column_iterator(Uid object, uint32_t column, std::shared_ptr<Session> session)
            : m_object(object), m_column(column), m_session(session) {}

        value_type operator*() const { return value_type(m_object, m_column, m_session); }
        value_type operator[](difference_type offset) const { return *(*this + offset); }

        column_iterator& operator++() { return *this += 1; }
        column_iterator operator++(int) {
            auto copy = *this;
            ++*this;
            return copy;
        }
        column_iterator& operator--() { return *this -= 1; }
        column_iterator operator--(int) {
            auto copy = *this;
            --*this;
            return copy;
        }
        column_iterator& operator+=(difference_type offset) { return m_column += offset, *this; }
        column_iterator& operator-=(difference_type offset) { return operator+=(-offset); }
        column_iterator operator+(difference_type offset) const { return column_iterator(*this) += offset; }
        friend column_iterator operator+(difference_type offset, const column_iterator& it) { return it + offset; }
        column_iterator operator-(difference_type offset) const { return column_iterator(*this) -= offset; }
        difference_type operator-(const column_iterator& rhs) const { return m_column - rhs.m_column; }

        auto operator<=>(const column_iterator& rhs) const noexcept { return m_column <=> rhs.m_column; }
        bool operator==(const column_iterator& rhs) const noexcept { return m_column == rhs.m_column; }
        bool operator!=(const column_iterator& rhs) const noexcept { return m_column != rhs.m_column; }

    private:
        Uid m_object = 0;
        difference_type m_column = 0;
        std::shared_ptr<Session> m_session = nullptr;
    };

    using iterator = column_iterator<true>;
    using const_iterator = column_iterator<false>;

    static_assert(std::input_iterator<iterator>);
    static_assert(std::input_iterator<const_iterator>);

public:
    TableRow(Uid object, TableDesc desc, std::shared_ptr<Session> session)
        : m_row(object), m_desc(std::move(desc)), m_session(session) {}

    template <bool MutableOther>
        requires(!Mutable && MutableOther)
    TableRow(const TableRow<MutableOther>& other)
        : m_row(other.m_row), m_desc(other.m_desc), m_session(other.m_session) {}

    iterator begin() { return iterator{ m_row, 0, m_session }; }
    iterator end() { return iterator{ m_row, m_desc.columns.size(), m_session }; }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    const_iterator cbegin() const { return const_iterator{ m_row, 0, m_session }; }
    const_iterator cend() const { return const_iterator{ m_row, m_desc.columns.size(), m_session }; }

    cell_reference<Mutable> operator[](size_t index) { return begin()[index]; }
    cell_reference<false> operator[](size_t index) const { return begin()[index]; }

    size_t size() const { return m_desc.columns.size(); }

    std::span<const ColumnDesc> GetDesc() const { return m_desc.columns; }

    Uid Id() const { return m_row; }

private:
    Uid m_row = 0;
    TableDesc m_desc = {};
    std::shared_ptr<Session> m_session = nullptr;
};


using Object = TableRow<true>;

} // namespace sedmgr