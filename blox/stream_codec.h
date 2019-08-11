#pragma once

#include <blox/bijective/function.h>
#include <sstream>
#include <string>

namespace blox {

template <typename T>
class stream_codec : public bijective::function<T, std::string_view> {
 public:
  using base_type = bijective::function<T, std::string_view>;
  stream_codec();
};

template <typename T>
stream_codec<T>::stream_codec()
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

}  // namespace blox
