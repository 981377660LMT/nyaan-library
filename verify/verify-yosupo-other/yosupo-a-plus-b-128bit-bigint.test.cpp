#define PROBLEM "https://judge.yosupo.jp/problem/many_aplusb_128bit"
//
#include "../../template/template.hpp"
//
#include "../../math/bigint.hpp"
using namespace Nyaan;

void q() {
  bigint a, b;
  in(a, b);
  out(a + b);
}

void Nyaan::solve() {
  int t = 1;
  in(t);
  while (t--) q();
}
