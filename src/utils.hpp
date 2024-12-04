#pragma once
#include <sstream>
#include <string>
#include <vector>

namespace idl2xml {

template <typename T> std::string join(const std::vector<T>& v, const std::string& sep) {
  std::stringstream sstm;
  for (auto it = v.cbegin(); it != v.cend(); ++it) {
    if (it != v.cbegin()) {
      sstm << sep;
    } else {
      // pass
    }
    sstm << *it;
  }
  return sstm.str();
}
}  // namespace idl2xml