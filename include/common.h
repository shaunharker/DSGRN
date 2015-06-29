/// common.h
/// Shaun Harker
/// 2015-05-31
#ifndef DSGRN_COMMON_H
#define DSGRN_COMMON_H

/// Common includes

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <utility>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>

#include "boost/serialization/serialization.hpp"
#include "boost/serialization/base_object.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/shared_ptr.hpp"
#include "boost/serialization/unordered_map.hpp"

#include "boost/functional/hash.hpp"

#include "Tools/SHA256.h"
#include "Tools/json.hpp"
using json = nlohmann::json;

#endif
