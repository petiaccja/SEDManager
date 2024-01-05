#include "Method.hpp"

#include <Archive/Conversion.hpp>
#include <Error/Exception.hpp>


namespace sedmgr {

namespace impl {

    std::vector<std::pair<intptr_t, const Value&>> LabelOptionalArgs(std::span<const Value> streams) {
        using Item = std::pair<intptr_t, const Value&>;
        std::vector<Item> labels;
        std::ranges::transform(streams, std::back_inserter(labels), [](const Value& stream) {
            if (stream.Is<Named>()) {
                const auto& named = stream.Get<Named>();
                if (!named.name.IsInteger()) {
                    throw std::invalid_argument("expected an integer as argument label");
                }
                return Item{ named.name.Get<intptr_t>(), named.value };
            }
            return Item{ intptr_t(-1), stream };
        });
        return labels;
    }

} // namespace impl

} // namespace sedmgr