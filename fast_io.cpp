#if defined(unix) || defined(__unix__) || defined(__unix)
#include <unistd.h> // read(), write()
#define READ(buff, len) read(0, buff, len)
#define WRITE(buff, len) write(1, buff, len)
#elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#include <io.h>     // _read(), _write()
#define READ(buff, len) _read(0, buff, len)
#define WRITE(buff, len) _write(1, buff, len)
#else
#include <stdio.h>  // fread(), fwrite(), stdin, stdout
#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
#define READ(buff, len) fread_unlocked(buff, sizeof(char), len, stdin)
#define WRITE(buff, len) fwrite_unlocked(buff, sizeof(char), len, stdout)
#elif defined(_CRT_DISABLE_PERFCRIT_LOCKS)
#define READ(buff, len) _fread_nolock(buff, sizeof(char), len, stdin)
#define WRITE(buff, len) _fwrite_nolock(buff, sizeof(char), len, stdout)
#elif
#define READ(buff, len) fread(buff, sizeof(char), len, stdin)
#define WRITE(buff, len) fwrite(buff, sizeof(char), len, stdout)
#endif
#endif


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

  // template<typename T, bool> T read();
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
  READ(buffer_, buffer_size);
}

template<int buffer_size>
IO<buffer_size>::~IO() {
  WRITE(buffer_, buffer_size);
}

} // namespace fast


int main() {
  fast::IO<1024> io;
}