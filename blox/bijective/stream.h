#pragma once

#include <blox/bijective/function.h>
#include <sstream>
#include <string>

namespace blox {

namespace bijective {

template <typename T>
class stream : public bijective::function<T, std::string_view> {
 public:
  using base_type = bijective::function<T, std::string_view>;
  stream();
};

template <typename T>
stream<T>::stream()
    : base_type(
          [](const T& input) {
            static std::string buffer;
            std::ostringstream os;
            os << input;
            buffer = os.str();
            return std::string_view(buffer);
          },
          [](std::string_view input) {
            static std::string buffer;
            buffer = input;
            T output;
            std::istringstream is{buffer};
            is >> output;
            return output;
          }) {}

}  // namespace bijective
}  // namespace blox
