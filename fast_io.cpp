#include <ctype.h>     // isspace()
#include <stdint.h>    // int64_t

#include <limits>      // std::numeric_limits::digits10
#include <string>      // std::string
#include <type_traits> // std::enable_if, std::is_integral, std::is_signed,
                       // std::is_unsigned, std::is_same, std::remove_cv,
                       // std::integral_constant

// #if defined(unix) || defined(__unix__) || defined(__unix)
#include <unistd.h> // read(), write()
// #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
// #include <io.h>     // _read(), _write()
// #else
// #include <stdio.h>  // fread(), fwrite(), stdin, stdout, setvbuf(), _IOFBF
// #if defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
// #define fread fread_unlocked
// #define fwrite fwrite_unlocked
// #elif defined(_CRT_DISABLE_PERFCRIT_LOCKS)
// #define fread _fread_nolock
// #define fwrite _fwrite_nolock
// #endif
// #endif


namespace fast {

// Type Metafunctions
namespace type {

// Type categories
template<typename T, typename char_t = typename std::decay<T>::type>
struct is_char_type : std::integral_constant<bool,
  std::is_same<char_t, char>::value ||
  std::is_same<char_t, wchar_t>::value ||
#if __cplusplus >= 202002L
  std::is_same<char_t, char8_t>::value ||
#endif
  std::is_same<char_t, char16_t>::value ||
  std::is_same<char_t, char32_t>::value>
{};

template<typename int_t>
struct is_int_type : std::integral_constant<bool,
  std::is_integral<int_t>::value && 
  std::is_signed<int_t>::value &&
  !is_char_type<int_t>::value>
{};

template<typename uint_t>
struct is_uint_type : std::integral_constant<bool,
  std::is_integral<uint_t>::value && 
  std::is_unsigned<uint_t>::value &&
  !is_char_type<uint_t>::value>
{};

template<typename string_t>
struct is_string_type : std::integral_constant<bool,
  std::is_same<string_t, std::string>::value ||
  std::is_same<string_t, std::wstring>::value ||
#if __cplusplus >= 202002L
  std::is_same<string_t, std::u8string>::value ||
#endif
  std::is_same<string_t, std::u16string>::value ||
  std::is_same<string_t, std::u32string>::value>
{};

template<typename T, typename char_t_pointer = typename std::decay<T>::type>
struct is_char_pointer_type : std::integral_constant<bool,
  std::is_same<char_t_pointer, char*>::value ||
  std::is_same<char_t_pointer, const char*>::value ||
  std::is_same<char_t_pointer, wchar_t*>::value ||
  std::is_same<char_t_pointer, const wchar_t*>::value ||
#if __cplusplus >= 202002L
  std::is_same<char_t_pointer, char8_t*>::value ||
  std::is_same<char_t_pointer, const char8_t*>::value ||
#endif
  std::is_same<char_t_pointer, char16_t*>::value ||
  std::is_same<char_t_pointer, const char16_t*>::value ||
  std::is_same<char_t_pointer, char32_t*>::value ||
  std::is_same<char_t_pointer, const char32_t*>::value>
{};

// Enable if certain type
template<typename T>
using enable_if_is_char_type = 
  typename std::enable_if<is_char_type<T>::value, bool>::type;

template<typename T>
using enable_if_is_int_type = 
  typename std::enable_if<is_int_type<T>::value, bool>::type;

template<typename T>
using enable_if_is_uint_type = 
  typename std::enable_if<is_uint_type<T>::value, bool>::type;

template<typename T>
using enable_if_is_string_type = 
  typename std::enable_if<is_string_type<T>::value, bool>::type;

template<typename T>
using enable_if_is_char_pointer_type = 
  typename std::enable_if<is_char_pointer_type<T>::value, bool>::type;

} // namespace type

namespace helper {

inline bool is_number(char c) {
  return c >= '0' && c <= '9';
}

} // namespace helper

template<int buffer_size>
class IO {
 public:
  IO() {
  // #if defined(unix) || defined(__unix__) || defined(__unix)
    ::read(0, buffer_, buffer_size);
  // #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
  //   _read(0, buffer_, buffer_size);
  // #else
    // setvbuf(stdin, buffer_, _IOLBF, buffer_size);
    // setvbuf(stdout, buffer_, _IONBF, buffer_size);
  // #endif
  }
  ~IO() {
  // #if defined(unix) || defined(__unix__) || defined(__unix)
    ::write(1, buffer_, buffer_size);
  // #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
  //   _write(1, buffer_, pos_ - buffer_);
  // #else

  // #endif
  }
  IO(const IO&) = delete;
  IO& operator=(const IO&) = delete;
  IO(IO&&) = delete;
  IO& operator=(IO&&) = delete;

  // Input Access
  template<typename int_t, type::enable_if_is_int_type<int_t> = true>
  inline int_t read() {
    int_t x { 0 };
    bool minus { *pos_ == '-' };
    while (!helper::is_number(*pos_)) { ++pos_; }
    for (; helper::is_number(*pos_); ++pos_) {
      x = (x << 3) + (x << 1) + (*pos_ - '0');
    }
    return minus ? -x : x;
  }

  template<typename uint_t, type::enable_if_is_uint_type<uint_t> = true>
  inline uint_t read() {
    uint_t x { 0u };
    while (!helper::is_number(*pos_)) { ++pos_; }
    for (; helper::is_number(*pos_); ++pos_) {
      x = (x << 3) + (x << 1) + (*pos_ - '0');
    }
    return x;
  }

  template<typename char_t, type::enable_if_is_char_type<char_t> = true>
  inline char_t read() {
    return *pos_++;
  }

  template<typename string_t, type::enable_if_is_string_type<string_t> = true>
  inline string_t read() {
    string_t s;
    while (isspace(*pos_)) { pos_++; }
    for (; !isspace(*pos_); ++pos_) { s.push_back(*pos_); }
    return s;
  }

  template<typename T>
  inline void read(T& x) {
    x = read<T>();
  }

  template<typename T, typename... Args>
  inline void read(T& x, Args&... args) {
    read(x);
    read(args...);
  }

  // Output Access
  template<typename int_t, type::enable_if_is_int_type<int_t> = true>
  inline void write(int_t x) {
    if (x < 0) {
      *pos_++ = '-';
      x = -x;
    }
    char buffer[std::numeric_limits<int_t>::digits10 + 1], *p { buffer };
    for (; x; x /= 10) { *p++ = '0' + (x % 10); }
    while (p != buffer) { *pos_++ = *--p; }
  }

  template<typename uint_t, type::enable_if_is_uint_type<uint_t> = true>
  inline void write(uint_t x) {
    char buffer[std::numeric_limits<uint_t>::digits10 + 1], *p { buffer };
    for (; x; x /= 10) { *p++ = '0' + (x % 10); }
    while (p != buffer) { *pos_++ = *--p; }
  }

  template<typename char_t, type::enable_if_is_char_type<char_t> = true>
  inline void write(char_t x) {
    *pos_++ = x;
  }

  template<typename string_t, type::enable_if_is_string_type<string_t> = true>
  inline void write(const string_t& s) {
    for (const auto c : s) { *pos_++ = c; }
  }

  template<typename char_t_pointer,
           type::enable_if_is_char_pointer_type<char_t_pointer> = true>
  inline void write(const char_t_pointer s) {
    while (*s) { *pos_++ = *s++; }
  }

  template<typename T, typename... Args>
  inline void write(T x, Args... args) {
    write(x);
    write(args...);
  }

 private:
  char buffer_[buffer_size] {};
  char* pos_ { buffer_ };
};

} // namespace fast


int main() {
  fast::IO<20000> io;
}