#pragma once

#include <numeric>
using namespace std;

#include "../math-fast/gcd.hpp"
#include "bigint.hpp"

namespace GCDforBigintImpl {

bigint bigint_pow(bigint a, long long k) {
  bigint res{1};
  while (k) {
    if (k & 1) res = res * a;
    k >>= 1;
    if (k) a = a * a;
  }
  return res;
}

// a = 2^x 5^y の形で表現する
pair<int, int> shrink(int a) {
  assert(a > 0);
  int x = __builtin_ctz(a);
  a >>= x;
  int y = a == 1        ? 0
          : a == 5      ? 1
          : a == 25     ? 2
          : a == 125    ? 3
          : a == 625    ? 4
          : a == 3125   ? 5
          : a == 15625  ? 6
          : a == 78125  ? 7
          : a == 390625 ? 8
                        : 9;
  return {x, y};
}

pair<int, int> shrink(bigint& a) {
  assert(a.neg == false);
  if (a.dat.empty()) return {0, 0};
  pair<int, int> res{0, 0};
  while (true) {
    int g = gcd(bigint::D, a.dat[0]);
    if (g == 1) break;
    if (g != bigint::D) a *= bigint::D / g;
    a.dat.erase(begin(a.dat));
    auto s = shrink(g);
    res.first += s.first, res.second += s.second;
  }
  return res;
}

template <bool FAST = false>
bigint gcd_d_ary(bigint a, bigint b) {
  a.neg = b.neg = false;
  if constexpr (FAST) {
    if (max<int>(a.dat.size(), b.dat.size()) <= 4) {
      return __int128_t(BinaryGCDImpl::binary_gcd128(a.to_i128(), b.to_i128()));
    }
  }
  if (a.dat.empty()) return b;
  if (b.dat.empty()) return a;
  pair<int, int> s = shrink(a), t = shrink(b);
  if (a < b) swap(a, b);
  while (true) {
    if (b.dat.empty()) break;
    if constexpr (FAST) {
      if ((int)a.dat.size() <= 4) break;
    }
    a = a - b;
    if (!a.dat.empty()) {
      while (true) {
        int g = gcd<int>(a.dat[0], bigint::D);
        if (g == 1) break;
        if (g != bigint::D) a *= bigint::D / g;
        a.dat.erase(begin(a.dat));
      }
    }
    if (a < b) swap(a, b);
  }
  assert(a >= b);
  bigint g;
  if constexpr (FAST) {
    if (b.dat.empty()) {
      g = a;
    } else {
      g = __int128_t(BinaryGCDImpl::binary_gcd128(a.to_i128(), b.to_i128()));
    }
  } else {
    g = a;
  }
  int e2 = min(s.first, t.first);
  int e5 = min(s.second, t.second);
  if (e2) g *= bigint_pow(bigint{2}, e2);
  if (e5) g *= bigint_pow(bigint{5}, e5);
  return g;
}
}  // namespace GCDforBigintImpl

MultiPrecisionInteger gcd(const MultiPrecisionInteger& a,
                          const MultiPrecisionInteger& b) {
  return GCDforBigintImpl::gcd_d_ary<true>(a, b);
}
