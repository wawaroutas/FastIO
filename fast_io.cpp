#include <stdio.h>  // fread(), fwrite()
#include <unistd.h> // read(), write()


namespace fast {

#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE)
#define fread fread_unlocked
#define fwrite fwrite_unlocked
#elif defined(_CRT_DISABLE_PERFCRIT_LOCKS) 
#define fread _fread_nolock
#define fwrite _fwrite_nolock
#endif

template<int buffer_size>
class IO {
 public:
  IO();
  ~IO();
  IO(const IO&) = delete;
  IO& operator=(const IO&) = delete;
  IO(IO&&) = delete;
  IO& operator=(IO&&) = delete;
 private:
  char buffer_[buffer_size] {};
  char* pos_ { buffer_ };
};

#ifdef fread
#undef fread
#endif
#ifdef fwrite
#undef fwrite
#endif

} // namespace fast


int main() {

}