#include "Table.hpp"


namespace sedmgr {

Table::Table(Uid table, TableDesc desc, std::shared_ptr<Session> session)
    : m_table(table), m_desc(std::move(desc)), m_session(session) {
}


Uid Table::First() const {
    if (m_session) {
        if (m_desc.singleRow) {
            return *m_desc.singleRow;
        }
        return m_session->base.Next(m_table, {}).value_or(0);
    }
    return 0;
}

} // namespace sedmgr