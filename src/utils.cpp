#include "utils.hpp"

namespace idl2xml {

std::vector<std::string_view> split(const std::string_view s, const std::string& sep) {
  std::vector<std::string_view> result;
  size_t start = 0;
  size_t end = s.find(sep);
  while (end != std::string_view::npos) {
    result.push_back(s.substr(start, end - start));
    start = end + sep.size();
    end = s.find(sep, start);
  }
  result.push_back(s.substr(start, s.size() - start));
  return result;
}

}  // namespace idl2xml