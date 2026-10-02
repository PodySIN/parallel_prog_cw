#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <iostream>

struct Fd {
  int fd;

  explicit Fd(int f):
    fd(f)
  {}

  ~Fd()
  {
    if (fd >= 0)
      ::close(fd);
  }

  Fd(const Fd&) = delete;
  Fd& operator=(const Fd&) = delete;

  int get() const
  {
    return fd;
  }

  void close()
  {
    if (fd >= 0) {
      ::close(fd);
      fd = -1;
    }
  }
};

size_t send(int& err, int wr, const char* b, size_t k)
{
  size_t r = 0;
  while (r < k) {
    err = write(wr, b + r, k - r);
    if (err < 0)
      break;
    r += err;
  }
  return r;
}

int main()
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << errno << "\n";
    return 1;
  }

  int pps[2] = {};
  if (pipe(pps) != 0) {
    std::cerr << errno << "\n";
    return 1;
  }

  pid_t pid;
  {
    Fd rd(pps[0]);
    Fd wr(pps[1]);

    pid = fork();
    if (pid < 0) {
      std::cerr << errno << "\n";
      return 1;
    }

    if (!pid) {
      wr.close();
      char p[100] = {};
      sprintf(p, "%d", rd.get());
      execl("./child", "child", p, NULL);
      std::cerr << errno << "\n";
      return 1;
    }

    int err = 0;
    send(err, wr.get(), line.data(), line.size());
    if (err < 0) {
      std::cerr << errno << "\n";
      return 1;
    }
  }

  if (waitpid(pid, 0, 0) != pid) {
    std::cerr << errno << "\n";
    return 1;
  }
  return 0;
}
