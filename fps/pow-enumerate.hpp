#pragma once

#include <cassert>
#include <vector>
using namespace std;

#include "formal-power-series.hpp"

// [x^n] f(x)^i g(x) を i=0,1,...,m で列挙
// n = (f の次数) - 1
template <typename mint>
FormalPowerSeries<mint> pow_enumerate(FormalPowerSeries<mint> f,
                                      FormalPowerSeries<mint> g = {1},
                                      int m = -1) {
  using fps = FormalPowerSeries<mint>;
  int n = f.size() - 1, k = 1;
  g.resize(n + 1);
  if (m == -1) m = n;
  int h = 1;
  while (h < n + 1) h *= 2;
  fps P((n + 1) * k), Q((n + 1) * k), nP, nQ, buf;
  for (int i = 0; i <= n; i++) P[i * k + 0] = g[i];
  for (int i = 0; i <= n; i++) Q[i * k + 0] = -f[i];
  while (n) {
    nP.clear(), nQ.clear();
    for (int i = 0; i <= n; i++) {
      buf.assign(2 * k, 0);
      copy(begin(P) + i * k, begin(P) + (i + 1) * k, begin(buf));
      buf.ntt();
      copy(begin(buf), end(buf), back_inserter(nP));

      buf.assign(2 * k, 0);
      copy(begin(Q) + i * k, begin(Q) + (i + 1) * k, begin(buf));
      if (i == 0) buf[k] += 1;
      buf.ntt();
      copy(begin(buf), end(buf), back_inserter(nQ));
    }
    nP.resize(2 * h * 2 * k);
    nQ.resize(2 * h * 2 * k);
    fps p(2 * h), q(2 * h);
    for (int j = 0; j < 2 * k; j++) {
      p.assign(2 * h, 0);
      q.assign(2 * h, 0);
      for (int i = 0; i < h; i++) {
        p[i] = nP[i * 2 * k + j], q[i] = nQ[i * 2 * k + j];
      }
      p.ntt(), q.ntt();
      for (int i = 0; i < 2 * h; i += 2) swap(q[i], q[i + 1]);
      for (int i = 0; i < 2 * h; i++) p[i] *= q[i];
      q.resize(h);
      for (int i = 0; i < h; i++) q[i] = q[i * 2] * q[i * 2 + 1];
      p.intt(), q.intt();
      for (int i = 0; i < 2 * h; i++) nP[i * 2 * k + j] = p[i];
      for (int i = 0; i < h; i++) nQ[i * 2 * k + j] = q[i];
    }
    for (int i = 0; i <= n / 2; i++) {
      copy(begin(nP) + (i * 2 + n % 2) * 2 * k,
           begin(nP) + (i * 2 + n % 2 + 1) * 2 * k, begin(buf));
      buf.intt();
      copy(begin(buf), end(buf), begin(nP) + i * 2 * k);

      copy(begin(nQ) + i * 2 * k, begin(nQ) + (i + 1) * 2 * k, begin(buf));
      buf.intt();
      if (i == 0) buf[0] -= 1;
      copy(begin(buf), end(buf), begin(nQ) + i * 2 * k);
    }
    nP.resize((n / 2 + 1) * 2 * k);
    nQ.resize((n / 2 + 1) * 2 * k);
    swap(P, nP), swap(Q, nQ);
    n /= 2, h /= 2, k *= 2;
  }

  fps S{begin(P), begin(P) + k};
  fps T{begin(Q), begin(Q) + k};
  return (S.rev() * (T + (fps{1} << k)).rev().inv(m + 1)).pre(m + 1);
}

/*
// 別バージョン
// [x^n] f(x)^i g(x) を i=0,1,...,m で列挙
// n = (f の次数) - 1
FormalPowerSeries<mint> pow_enumerate(FormalPowerSeries<mint> f,
                                      FormalPowerSeries<mint> g = {1},
                                      int m = -1) {
  using fps = FormalPowerSeries<mint>;
  int n = f.size() - 1, k = 1;
  g.resize(n + 1);
  if (m == -1) m = n;
  int h = 1;
  while (h < n + 1) h *= 2;
  fps P(h * k), Q(h * k), nP(4 * h * k), nQ(4 * h * k), nR(2 * h * k);
  for (int i = 0; i <= n; i++) P[i] = g[i], Q[i] = -f[i];
  while (n) {
    nP.assign(4 * h * k, 0);
    nQ.assign(4 * h * k, 0);
    for (int i = 0; i < k; i++) {
      copy(begin(P) + i * h, begin(P) + i * h + n + 1, begin(nP) + i * 2 * h);
      copy(begin(Q) + i * h, begin(Q) + i * h + n + 1, begin(nQ) + i * 2 * h);
    }
    nQ[k * 2 * h] += 1;
    nP.ntt(), nQ.ntt();
    for (int i = 0; i < 4 * h * k; i += 2) swap(nQ[i], nQ[i + 1]);
    for (int i = 0; i < 4 * h * k; i++) nP[i] *= nQ[i];
    for (int i = 0; i < 2 * h * k; i++) nR[i] = nQ[i * 2] * nQ[i * 2 + 1];
    nP.intt(), nR.intt();
    nR[0] -= 1;
    P.assign(h * k, 0), Q.assign(h * k, 0);
    for (int i = 0; i < 2 * k; i++) {
      for (int j = 0; j <= n / 2; j++) {
        P[i * h / 2 + j] = nP[i * 2 * h + j * 2 + n % 2];
        Q[i * h / 2 + j] = nR[i * h + j];
      }
    }
    n /= 2, h /= 2, k *= 2;
  }
  fps S{begin(P), begin(P) + k}, T{begin(Q), begin(Q) + k};
  T.push_back(1);
  return (S.rev() * T.rev().inv(m + 1)).pre(m + 1);
}
*/

/**
 * @brief pow 列挙
 */
