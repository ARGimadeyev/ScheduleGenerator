// #pragma GCC optimize("Ofast, unroll-loops")
#include <bits/stdc++.h>
#include "lib/scheduler.h"

using namespace std;

using ll = long long;
using ld = long double;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
uniform_int_distribution<> random_class(0, TOTAL_CLASSES - 1);
uniform_int_distribution<> random_lesson(0, TOTAL_LESSONS - 1);

ld rnd() {
    return ld(rng()) / (UINT_MAX);
}

constexpr double COOLING_RATE = 0.98;
constexpr double MIN_COOL = 1e-2;

int main() {
    cout << "Hello!\n";
    cout << "If you want to improve the schedule you have already created (without intersections), enter: 1" << endl;
    Schedule now;
    int num;
    cin >> num;
    if (num == 1) {
        now.read();
    } else {
        now.readNew();
    }
    now.recnt();
    Schedule best = now;

    ld T = 1'000;

    for (; T >= MIN_COOL; T *= COOLING_RATE) {
        Schedule nw = now;

        int Class = random_class(rng);
        nw.swp(Class, random_lesson(rng), random_lesson(rng));

        nw.recnt();
        ll delta = nw.cost - now.cost;
        if (delta < 0 || rnd() < exp(-delta / T)) {
            now = nw;
            if (now.cost < best.cost) {
                best = now;
            }
        }
        if (now.cost - best.cost > 1e10) now = best;
        cerr << T << ' ' << best.cost << ' ' << now.cost << '\n';
    }
    cout << best.cost << endl;
    best.print();
    return 0;
}
