#include "Token.hpp"


namespace sedmgr {

eTag GetTagForData(size_t dataLength) {
    if (dataLength <= 15) {
        return eTag::SHORT_ATOM;
    }
    else if (dataLength <= 2047) {
        return eTag::MEDIUM_ATOM;
    }
    else if (dataLength <= 16'777'215) {
        return eTag::LONG_ATOM;
    }
    else {
        throw std::invalid_argument("data length too large to fit into any atom");
    }
}

} // namespace sedmgr