#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <iostream>

size_t recv(int& err, int rd, char* b, size_t k)
{
  size_t r = 0;
  while (r < k) {
    err = read(rd, b + r, k - r);
    if (err < 0)
      break;
    if (err == 0)
      break;
    r += err;
  }
  return r;
}

bool read_all(int rd, std::string& out)
{
  char buf[4096];
  int err = 0;
  while (true) {
    size_t n = recv(err, rd, buf, sizeof(buf));
    if (n > 0)
      out.append(buf, n);
    if (err < 0) {
      std::cerr << errno << "\n";
      return false;
    }
    if (n < sizeof(buf))
      break;
  }
  return true;
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << errno << "\n";
    return 1;
  }
  int rd = std::atoi(argv[1]);
  if (rd == 0) {
    std::cerr << errno << "\n";
    return 1;
  }

  std::string msg;
  if (!read_all(rd, msg))
    return 1;

  int err = close(rd);
  if (err) {
    std::cerr << errno << "\n";
    return 1;
  }

  std::cout << msg << "\n";
  if (!std::cout) {
    std::cerr << errno << "\n";
    return 1;
  }
  return 0;
}
