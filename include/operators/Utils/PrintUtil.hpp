#include <string>
#include <type_traits>
#include <absl/strings/str_join.h>

namespace operators {

// Formatter that converts to string by calling the name attribute,
// works for pointes as well as references
class NameFormatter {
  template <class T>
  void dispatch(std::string* out, const T* const t, std::true_type) const {
    return absl::AlphaNumFormatter()(out, t->name());
  }
  template <class T>
  void dispatch(std::string* out, const T& t, std::false_type) const {
    return absl::AlphaNumFormatter()(out, t.name());
  }

 public:
  template <class T>
  void operator()(std::string* out, const T& t) const {
    return dispatch(out, t, std::is_pointer<T>());
  }
};

// Formatter that converts to string by calling the ToString attribute,
// works for pointes as well as references
class ToStringFormatter {
  template <class T>
  void dispatch(std::string* out, const T* const t, std::true_type) const {
    return absl::AlphaNumFormatter()(out, t->to_string());
  }
  template <class T>
  void dispatch(std::string* out, const T& t, std::false_type) const {
    return absl::AlphaNumFormatter()(out, t.to_string());
  }

 public:
  template <class T>
  void operator()(std::string* out, const T& t) const {
    return dispatch(out, t, std::is_pointer<T>());
  }
};

template <class F>
struct ApplyFormatter {
  ApplyFormatter(F f) : f(std::move(f)) {}
  F f;
  template <class T>
  void operator() (std::string * out, const T& t) const {
    return absl::AlphaNumFormatter()(out, f(t));
  }
};

}