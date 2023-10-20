#pragma once

#include "Object.hpp"

#include <Data/NativeTypes.hpp>
#include <Specification/Module.hpp>
#include <TrustedPeripheral/Session.hpp>

#include <memory>



class Table {
public:
    template <bool Mutable>
    class row_iterator {
    public:
        using value_type = TableRow<Mutable>;
        using difference_type = ptrdiff_t;

    public:
        row_iterator() {}
        row_iterator(Uid table, Uid row, TableDesc desc, std::shared_ptr<Session> session)
            : m_table(table), m_row(row), m_desc(std::move(desc)), m_session(session) {}

        value_type operator*() const { return value_type{ m_row, m_desc, m_session }; }
        row_iterator& operator++() { return m_row = Next(), *this; }
        row_iterator operator++(int) {
            auto copy = *this;
            ++*this;
            return copy;
        }

        bool operator==(const row_iterator& rhs) const { return m_row == rhs.m_row; }
        bool operator!=(const row_iterator& rhs) const { return m_row != rhs.m_row; }

    private:
        Uid Next() const;

        Uid m_table = 0;
        Uid m_row = 0;
        TableDesc m_desc = {};
        std::shared_ptr<Session> m_session = nullptr;
    };

    using iterator = row_iterator<true>;
    using const_iterator = row_iterator<false>;

    static_assert(std::input_iterator<iterator>);
    static_assert(std::input_iterator<const_iterator>);

public:
    Table() {}
    Table(Uid table, TableDesc desc, std::shared_ptr<Session> session);

    iterator begin() { return iterator{ m_table, First(), m_desc, m_session }; }
    iterator end() { return iterator{ m_table, 0, m_desc, m_session }; }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    const_iterator cbegin() const { return const_iterator{ m_table, First(), m_desc, m_session }; }
    const_iterator cend() const { return const_iterator{ m_table, 0, m_desc, m_session }; }

    const TableDesc& GetDesc() const { return m_desc; }

private:
    Uid First() const;

private:
    Uid m_table = 0;
    TableDesc m_desc = {};
    std::shared_ptr<Session> m_session = nullptr;
};


template <bool Mutable>
Uid Table::row_iterator<Mutable>::Next() const {
    if (m_session) {
        if (m_desc.singleRow) {
            return 0;
        }
    }
    return m_session->base.Next(m_table, m_row).value_or(0);
}