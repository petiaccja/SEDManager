#pragma once

#include "State.hpp"


namespace sedmgr {

namespace mock {
    std::vector<std::shared_ptr<SecurityProvider>> GetMockPreconfig();
}

} // namespace sedmgr