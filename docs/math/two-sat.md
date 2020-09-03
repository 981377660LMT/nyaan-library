## 2-SAT

2-SATを$\mathrm{O}(N + M)$($N$は論理変数の個数、$M$は節の個数)で計算するライブラリ。

#### 概要

SAT(充足可能性問題,satisfiability problem)とは、論理変数$x_1,x_2,\ldots x_n$からなる論理式が与えられたときに、変数に真か偽かを割り当てることで式全体を真にできるかを判定する問題である。

SATは一般にはNP困難であることが知られているが、連言標準形に直したときに節内の変数の数が2以下であるものを2-SATと呼び、多項式時間で解くことが出来る。

#### 使い方

- `TwoSAT(n)`　: $n$頂点のSATを初期化する。
- `add_cond(s, fs, t, ft)` : $v(i,0)=x_i,v(i,1)=\lnot x_i$としたとき、$(v(x_s,s) \wedge v(x_t,t))$を条件に加える。
- `run()` : 2-SATを解く。条件を満たす組み合わせが存在するときは各変数の値が格納された配列を、存在しないときは空の配列を返す。