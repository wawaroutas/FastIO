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

template<int buffer_size>
class IO {
 public:
  IO();
  ~IO();
  IO(const IO&) = delete;
  IO& operator=(const IO&) = delete;
  IO(IO&&) = delete;
  IO& operator=(IO&&) = delete;

  // template<typename T, bool> T read(void);
  // template<typename T> void read(T&);
  // template<typename T, typename... Args> void read(T&, Args&...);

  // template<typename T, bool> void write(T);
  // template<typename T, typename... Args> void write(T, Args...);
 private:
  char buffer_[buffer_size] {};
  char* pos_ { buffer_ };
};

template<int buffer_size>
IO<buffer_size>::IO() {
// #if defined(unix) || defined(__unix__) || defined(__unix)
  read(0, buffer_, buffer_size);
// #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
//   _read(0, buffer_, buffer_size);
// #else
  // setvbuf(stdin, buffer_, _IOLBF, buffer_size);
  // setvbuf(stdout, buffer_, _IONBF, buffer_size);
  // fread(buffer_, sizeof(char), buffer_size, stdin);
// #endif
}

template<int buffer_size>
IO<buffer_size>::~IO() {
// #if defined(unix) || defined(__unix__) || defined(__unix)
  write(1, buffer_, buffer_size);
// #elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
//   _write(1, buffer_, buffer_size);
// #else
  // fwrite(buffer_, sizeof(char), buffer_size, stdout);
// #endif
}

} // namespace fast


int main() {
  fast::IO<20000> io;
}