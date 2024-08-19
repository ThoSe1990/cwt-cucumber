#pragma once

#include "defines.hpp"  // NOLINT
#include "asserts.hpp"  // NOLINT
#include "context.hpp"  // NOLINT
#include "get_args.hpp" // NOLINT 

#include "test_results.hpp"

namespace cuke
{

[[nodiscard]] cuke::results::test_status entry_point(int argc,
                                                     const char* argv[]);

}  // namespace cuke
