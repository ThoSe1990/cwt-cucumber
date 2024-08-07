#pragma once

#include "defines.hpp"
#include "context.hpp"
#include "asserts.hpp"
#include "get_args.hpp"
#include "options.hpp"

#include "ast/test_results.hpp"

namespace cuke
{

[[nodiscard]] cuke::results::test_status entry_point(int argc,
                                                     const char* argv[]);

}  // namespace cuke
