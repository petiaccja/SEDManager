#pragma once

#include <StorageDevice/NvmeDevice.hpp>
#include <TrustedPeripheral/Session.hpp>
#include <TrustedPeripheral/SessionManager.hpp>
#include <TrustedPeripheral/TrustedPeripheral.hpp>


struct NamedObject {
    Uid uid = 0;
    std::optional<std::string> name = std::nullopt;
};


template <bool Mutable>
class TableRow {
public:
    template <bool Mutable_>
    class cell_reference {
    public:
        cell_reference(Uid object, uint32_t column, std::shared_ptr<Session> session)
            : m_row(object), m_column(column), m_session(session) {}
        Value operator*() const { return m_session->base.Get(m_row, m_column); }
        cell_reference& operator=(const Value& value) { return m_session->base.Set(m_row, m_column, value), *this; }

    private:
        Uid m_row = 0;
        uint32_t m_column = 0;
        std::shared_ptr<Session> m_session = nullptr;
    };


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
    TableRow(Uid object, size_t numColumns, std::shared_ptr<Session> session)
        : m_row(object), m_numColumns(numColumns), m_session(session) {}

    iterator begin() { return iterator{ m_row, 0, m_session }; }
    iterator end() { return iterator{ m_row, m_numColumns, m_session }; }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    const_iterator cbegin() const { return const_iterator{ m_row, 0, m_session }; }
    const_iterator cend() const { return const_iterator{ m_row, m_numColumns, m_session }; }

    cell_reference<Mutable> operator[](size_t index) { return begin()[index]; }
    cell_reference<false> operator[](size_t index) const { return begin()[index]; }

    Uid Id() const { return m_row; }

private:
    Uid m_row = 0;
    size_t m_numColumns = 0;
    std::shared_ptr<Session> m_session = nullptr;
};


using Object = TableRow<true>;


class Table {
public:
    template <bool Mutable>
    class row_iterator {
    public:
        using value_type = TableRow<Mutable>;
        using difference_type = ptrdiff_t;

    public:
        row_iterator() {}
        row_iterator(Uid table, Uid row, size_t numColumns, std::shared_ptr<Session> session)
            : m_table(table), m_row(row), m_numColumns(numColumns), m_session(session) {}

        value_type operator*() const { return value_type{ m_row, m_numColumns, m_session }; }
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
        size_t m_numColumns = 0;
        std::shared_ptr<Session> m_session = nullptr;
    };

    using iterator = row_iterator<true>;
    using const_iterator = row_iterator<false>;

    static_assert(std::input_iterator<iterator>);
    static_assert(std::input_iterator<const_iterator>);

public:
    Table() {}
    Table(Uid table, std::shared_ptr<Session> session);

    iterator begin() { return iterator{ m_table, First(), m_numColumns, m_session }; }
    iterator end() { return iterator{ m_table, 0, m_numColumns, m_session }; }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
    const_iterator cbegin() const { return const_iterator{ m_table, First(), m_numColumns, m_session }; }
    const_iterator cend() const { return const_iterator{ m_table, 0, m_numColumns, m_session }; }

    uint32_t NumColumns() const { return m_numColumns; }

private:
    Uid First() const;

private:
    Uid m_table = 0;
    size_t m_numColumns = 0;
    std::shared_ptr<Session> m_session = nullptr;
};


class App {
public:
    App(std::string_view device);

    const TPerDesc& GetCapabilities() const;
    std::unordered_map<std::string, uint32_t> GetProperties();

    std::vector<NamedObject> GetSecurityProviders();
    std::vector<NamedObject> GetAuthorities();
    std::vector<NamedObject> GetTables();
    Table GetTable(Uid table);
    Object GetObject(Uid table, Uid object);

    void Start(Uid securityProvider);
    void Authenticate(Uid authority, std::optional<std::span<const std::byte>> password = {});
    void End();

    void StackReset();
    void Reset();
    void Revert(std::span<const std::byte> psidPassword);

private:
    std::optional<std::string> GetNameFromTable(Uid uid, std::optional<uint32_t> column = {});
    std::vector<NamedObject> GetNamedRows(Session& session, const Table& table, uint32_t nameColumn);

private:
    std::shared_ptr<NvmeDevice> m_device;
    std::shared_ptr<TrustedPeripheral> m_tper;
    std::shared_ptr<SessionManager> m_sessionManager;
    std::shared_ptr<Session> m_session;
    TPerDesc m_capabilities;
};