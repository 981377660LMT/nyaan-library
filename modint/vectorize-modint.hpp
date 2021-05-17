#pragma once

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2")

using m256 = __m256i;
struct alignas(32) mmint {
  m256 x;
  static m256 R, mR, M0, M1, M2, N2;

  mmint() : x() {}
  inline mmint(const m256& _x) : x(_x) {}
  inline mmint(int a) : x(_mm256_set1_epi32(a)) {}
  inline mmint(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7)
      : x(_mm256_set_epi32(a7, a6, a5, a4, a3, a2, a1, a0)) {}
  inline operator m256&() { return x; }
  inline operator const m256&() const { return x; }
  inline int& operator[](int i) { return *(reinterpret_cast<int*>(&x) + i); }
  inline const int& operator[](int i) const {
    return *(reinterpret_cast<const int*>(&x) + i);
  }

  friend ostream& operator<<(ostream& os, const mmint& m) {
    unsigned r = R[0], mod = M1[0];
    auto reduce1 = [&](const uint64_t& b) {
      unsigned res = (b + uint64_t(unsigned(b) * unsigned(-r)) * mod) >> 32;
      return res >= mod ? res - mod : res;
    };
    for (int i = 0; i < 8; i++) {
      os << reduce1(m[i]) << (i == 7 ? "" : " ");
    }
    return os;
  }

  template <typename mint>
  static void set_mod() {
    R = _mm256_set1_epi32(mint::r);
    M0 = _mm256_setzero_si256();
    M1 = _mm256_set1_epi32(mint::get_mod());
    M2 = _mm256_set1_epi32(mint::get_mod() * 2);
    N2 = _mm256_set1_epi32(mint::n2);
    mR = _mm256_sub_epi32(M0, R);
  }

  static inline mmint reduce(const mmint& prod02, const mmint& prod13) {
    m256 unpalo = _mm256_unpacklo_epi32(prod02, prod13);
    m256 unpahi = _mm256_unpackhi_epi32(prod02, prod13);
    m256 prodlo = _mm256_unpacklo_epi64(unpalo, unpahi);
    m256 prodhi = _mm256_unpackhi_epi64(unpalo, unpahi);
    m256 hiplm1 = _mm256_add_epi32(prodhi, M1);
    m256 lomulr = _mm256_mullo_epi32(prodlo, R);
    m256 a13 = _mm256_shuffle_epi32(lomulr, 0xF5);
    m256 prod02_ = _mm256_mul_epu32(lomulr, M1);
    m256 prod13_ = _mm256_mul_epu32(a13, M1);
    m256 unpalo_ = _mm256_unpacklo_epi32(prod02_, prod13_);
    m256 unpahi_ = _mm256_unpackhi_epi32(prod02_, prod13_);
    m256 prod = _mm256_unpackhi_epi64(unpalo_, unpahi_);
    return _mm256_sub_epi32(hiplm1, prod);
  }

  static inline mmint itom(const mmint& A) { return A * N2; }

  static inline mmint mtoi(const mmint& A) {
    m256 lomulr = _mm256_mullo_epi32(A, mR);
    m256 a13 = _mm256_shuffle_epi32(lomulr, 0xF5);
    m256 prod02_ = _mm256_mul_epu32(lomulr, M1);
    m256 prod13_ = _mm256_mul_epu32(a13, M1);
    m256 unpalo_ = _mm256_unpacklo_epi32(prod02_, prod13_);
    m256 unpahi_ = _mm256_unpackhi_epi32(prod02_, prod13_);
    m256 prod = _mm256_unpackhi_epi64(unpalo_, unpahi_);
    m256 cmpA = _mm256_cmpgt_epi32(A, M0);
    m256 addA = _mm256_and_si256(cmpA, mmint(1));
    return _mm256_add_epi32(prod, addA);
  }

  friend inline mmint operator+(const mmint& A, const mmint& B) {
    m256 apb = _mm256_add_epi32(A, B);
    m256 ret = _mm256_sub_epi32(apb, M2);
    m256 cmp = _mm256_cmpgt_epi32(M0, ret);
    m256 add = _mm256_and_si256(cmp, M2);
    return _mm256_add_epi32(add, ret);
  }

  friend inline mmint operator-(const mmint& A, const mmint& B) {
    m256 ret = _mm256_sub_epi32(A, B);
    m256 cmp = _mm256_cmpgt_epi32(M0, ret);
    m256 add = _mm256_and_si256(cmp, M2);
    return _mm256_add_epi32(add, ret);
  }

  friend inline mmint operator*(const mmint& A, const mmint& B) {
    m256 a13 = _mm256_shuffle_epi32(A, 0xF5);
    m256 b13 = _mm256_shuffle_epi32(B, 0xF5);
    m256 prod02 = _mm256_mul_epu32(A, B);
    m256 prod13 = _mm256_mul_epu32(a13, b13);
    return reduce(prod02, prod13);
  }

  inline mmint& operator+=(const mmint& A) { return (*this) = (*this) + A; }
  inline mmint& operator-=(const mmint& A) { return (*this) = (*this) - A; }
  inline mmint& operator*=(const mmint& A) { return (*this) = (*this) * A; }

  bool operator==(const mmint& A) {
    m256 sub = _mm256_sub_epi32(x, A.x);
    return _mm256_testz_si256(sub, sub) == 1;
  }
  bool operator!=(const mmint& A) { return !((*this) == A); }
};
__attribute__((aligned(32))) __m256i mmint::R, mmint::mR;
__attribute__((aligned(32))) __m256i mmint::M0, mmint::M1, mmint::M2, mmint::N2;

/**
 * @brief vectorize modint
 */
