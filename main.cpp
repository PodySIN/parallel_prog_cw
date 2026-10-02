#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <iostream>

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
  int pps[2] = {}, err = pipe(pps);
  if (err) {
    std::cerr << errno << "\n";
    return 1;
  }
  int rd = pps[0], wr = pps[1];

  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << errno << "\n";
    return 1;
  }

  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << errno << "\n";
    return 1;
  }
  if (!pid) {
    err = close(wr);
    if (err) {
      std::cerr << errno << "\n";
      return 1;
    }
    char p[100] = {};
    err = sprintf(p, "%d", rd);
    if (!err) {
      std::cerr << errno << "\n";
      return 1;
    }
    execl("./child", "child", p, NULL);
    std::cerr << errno << "\n";
    return 1;
  }

  err = close(rd);
  if (err) {
    std::cerr << errno << "\n";
    return 1;
  }

  send(err, wr, line.data(), line.size());
  if (err < 0) {
    std::cerr << errno << "\n";
    return 1;
  }

  err = close(wr);
  if (err) {
    std::cerr << errno << "\n";
    return 1;
  }
  err = waitpid(pid, 0, 0);
  if (err != pid) {
    std::cerr << errno << "\n";
    return 1;
  }
  return 0;
}
