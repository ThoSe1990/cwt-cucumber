#pragma once

#include "chunk.hpp"

namespace cwt::details
{

std::size_t chunk::size() const noexcept { return m_constants.size(); }

}  // namespace cwt::details
