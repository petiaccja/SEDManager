#include "FlatBinaryArchive.hpp"


FlatBinaryInputArchive::FlatBinaryInputArchive(std::istream& stream)
    : cereal::InputArchive<FlatBinaryInputArchive>(this),
      m_stream(stream) {}


FlatBinaryOutputArchive::FlatBinaryOutputArchive(std::ostream& stream)
    : cereal::OutputArchive<FlatBinaryOutputArchive>(this),
      m_stream(stream) {}