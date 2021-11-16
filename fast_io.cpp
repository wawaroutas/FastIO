#include <ctype.h>     // isspace()
#include <stdint.h>    // int64_t
#include <stdio.h>     // getchar(), putchar(), setvbuf(), _IOLBF

#include <limits>      // std::numeric_limits::digits10
#include <string>      // std::string
#include <type_traits> // std::enable_if, std::is_integral, std::is_signed,
                       // std::is_unsigned, std::is_same, std::remove_cv,
                       // std::integral_constant

// #if defined(unix) || defined(__unix__) || defined(__unix)
// #include <unistd.h>    // read(), write()
// #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
// #include <io.h>        // _read(), _write()
// #endif


#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
#pragma GCC optimize("Ofast")
#pragma GCC target("fma,sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,tune=native")
#pragma GCC optimize("unroll-loops")
#elif defined(__clang__)
#pragma clang loop unroll(enable)
#endif

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

// template<int in_buffer_size, int out_buffer_size>
// class IO {
//  public:
//   IO() {
//   #if defined(unix) || defined(__unix__) || defined(__unix)
//   #define COSTUM_INPUT
//     ::read(0, in_buffer_, in_buffer_size);
//   #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
//   #define COSTUM_INPUT
//     _read(0, in_buffer_, in_buffer_size);
//   #else
//     // setvbuf(stdin, in_buffer_, _IOLBF, in_buffer_size);
//     // setvbuf(stdout, out_buffer_, _IOLBF, out_buffer_size);
//   #endif
//   }
//   ~IO() {
//   #if defined(unix) || defined(__unix__) || defined(__unix)
//   #define COSTUM_OUTPUT
//     ::write(1, out_buffer_, write_pos_ - out_buffer_);
//   #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
//   #define COSTUM_OUTPUT
//     _write(1, out_buffer_, write_pos_ - out_buffer_);
//   #else
//     //
//   #endif
//   }
//   IO(const IO&) = delete;
//   IO& operator=(const IO&) = delete;
//   IO(IO&&) = delete;
//   IO& operator=(IO&&) = delete;

//   friend int getchar();
//   friend void putchar(int);
//  private:
//   static char in_buffer_[in_buffer_size];
//   static char out_buffer_[out_buffer_size];
//   static char* read_pos_;
//   static char* write_pos_;
// };

// template<int in_buffer_size, int out_buffer_size>
// char IO<in_buffer_size, out_buffer_size>::in_buffer_[in_buffer_size] {};

// template<int in_buffer_size, int out_buffer_size>
// char IO<in_buffer_size, out_buffer_size>::out_buffer_[out_buffer_size] {};

// template<int in_buffer_size, int out_buffer_size>
// char* IO<in_buffer_size, out_buffer_size>::read_pos_  { in_buffer_ };

// template<int in_buffer_size, int out_buffer_size>
// char* IO<in_buffer_size, out_buffer_size>::write_pos_ { out_buffer_ };

inline int getchar() {
// #ifdef COSTUM_INPUT
//   return *read_pos_++;
// #elif defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
  return getchar_unlocked();
#elif defined(_CRT_DISABLE_PERFCRIT_LOCKS)
  return _getchar_nolock();
#else
  return ::getchar();
#endif
}

inline void putchar(int c) {
// #ifdef COSTUM_OUTPUT
//   *write_pos_++ = c;
// #elif defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
  putchar_unlocked(c);
#elif defined(_CRT_DISABLE_PERFCRIT_LOCKS)
  _putchar_nolock(c);
#else
  ::putchar(c);
#endif  
}

// Input
template<typename int_t, type::enable_if_is_int_type<int_t> = true>
inline int_t read() {
  int_t x { 0 };
  int c { getchar() };
  bool minus { c == '-'};
  while (!helper::is_number(c)) { c = getchar(); }
  for ( ; helper::is_number(c); c = getchar()) { x = x * 10 + c - '0'; }
  return minus ? -x : x;
}

template<typename uint_t, type::enable_if_is_uint_type<uint_t> = true>
inline uint_t read() {
  uint_t x { 0u };
  int c { getchar() };
  while (!helper::is_number(c)) { c = getchar(); }
  for ( ; helper::is_number(c); c = getchar()) { x = x * 10 + c - '0'; }
  return x;
}

template<typename string_t, type::enable_if_is_string_type<string_t> = true>
inline string_t read() {
  string_t s;
  int c { getchar() };
  while (isspace(c)) { c = getchar(); }
  for ( ; !isspace(c); c = getchar()) { s.push_back(c); }
  return s;
}

template<typename char_t, type::enable_if_is_char_type<char_t> = true>
inline char_t read() {
  return getchar();
}

template<typename T>
inline void read(T &x) {
  x = read<T>();
}

template<typename T, typename... Args>
inline void read(T &x, Args &... args) {
  read(x);
  read(args...);
}

// Output
template<typename int_t, type::enable_if_is_int_type<int_t> = true>
inline void write(int_t x) {
  if (x < 0) {
    putchar('-');
    x = -x;
  }
  char buffer[std::numeric_limits<int_t>::digits10 + 1], *p;
  for (p = buffer; x; x /= 10) { *p++ = '0' + x % 10; }
  while (p != buffer) { putchar(*--p); }
}

template<typename uint_t, type::enable_if_is_uint_type<uint_t> = true>
inline void write(uint_t x) {
  char buffer[std::numeric_limits<uint_t>::digits10 + 1], *p;
  for (p = buffer; x; x /= 10) { *p++ = '0' + x % 10; }
  while (p != buffer) { putchar(*--p); }
}

template<typename string_t, type::enable_if_is_string_type<string_t> = true>
inline void write(const string_t &s) {
  for (typename string_t::const_iterator it = s.begin(); it != s.end(); ++it) {
    putchar(*it); 
  }
}

template<typename char_t_pointer, 
         type::enable_if_is_char_pointer_type<char_t_pointer> = true>
inline void write(char_t_pointer s) {
  while (*s) { putchar(*s++); }
}

template<typename char_t, type::enable_if_is_char_type<char_t> = true>
inline void write(char_t c) {
  putchar(c);
}

template<typename T, typename... Args>
inline void write(T x, Args... args) {
  write(x);
  write(args...);
}

} // namespace fast


int main() {
  int a, b, c;
  fast::read(a, b, c);
  fast::write(a, ' ', b, ' ', c);
}
