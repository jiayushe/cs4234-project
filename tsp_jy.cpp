#include <bits/stdc++.h>

using namespace std;

#define INF 1<<30
#define INFLL 1LL<<61
#define EPS 1e-6
#define LSOne(S) ((S) & -(S))
#define bit(x, y) ((x>>y) & 1)
#define sz(x) (int)(x.size())
#define ll long long
#define ld long double
#define vi vector<int>
#define vvi vector<vi>
#define vd vector<double>
#define vvd vector<vd>
#define vll vector<ll>
#define vvll vector<vll>
#define pii pair<int, int>
#define pdi pair<double, int>
#define pid pair<int, double>
#define pdd pair<double, double>
#define pll pair<ll, ll>
#define umap unordered_map
#define uset unordered_set
#define sp ' '
#define endl '\n'
#define fi first
#define se second
#define pb push_back
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define uni(x) (x).erase(unique(all(x)), (x).end())
#define rep(i, n) for (ll i = 0; i < (ll)(n); i++)
#define rep1(i, n) for (ll i = 1; i <= (ll)(n); i++)
#define loop(i, l, r) for (ll i = (l); i <= (ll)(r); i++)
#define rloop(i, l, r) for (ll i = (l); i >= (ll)(r); i--)
#define debug(x) cerr<<#x<<" -> "<<x<<'\n'

inline double square(double x) { return x * x; }

inline ll getMax(vll arr) {
    ll maxx = -INF;
    for(auto i : arr) maxx = max(maxx, i);
    return maxx;
}

struct point {
    double x, y;
    point(double _x = 0, double _y = 0) { x = _x; y = _y; }
    int dist(point p) {
        return round(sqrt(square(x - p.x) + square(y - p.y)));
    }
};

int n, k = 20;
ll minD = INFLL;
vector<point> points;
vvll d;
vvi adj;
vector<vector<pair<ll, vi>>> memo;

pair<ll, vi> dp(int u, int mask) {
    if(mask == 0) return {d[u][0], vi(1, u)};
    pair<ll, vi> &ans = memo[u][mask];
    if(ans.fi != -1) return ans;
    ans = {INFLL, vi()};
    int m = mask;
    while(m) {
        int two_pow_v = LSOne(m);
        int v = __builtin_ctz(two_pow_v) + 1;
        pair<ll, vi> nx = dp(v, mask^two_pow_v);
        if(d[u][v] + nx.fi < ans.fi) {
            ans = nx;
            ans.fi += d[u][v];
        }
        m -= two_pow_v;
    }
    ans.se.pb(u);
    return ans;
}

vi greedy() {
    vi res(n), used(n);
    used[0] = 1;
    ll dist = 0;
    for(int i = 1; i < n; i++) {
        int prev = res[i - 1], best = -1;
        for(int nx = 1; nx < n; nx++) {
            if(used[nx]) continue;
            if(best == -1 or d[prev][nx] < d[prev][best]) best = nx;
        }
        res[i] = best, used[best] = 1;
        dist += d[prev][best];
    }
    dist += d[res.back()][0];
    debug(dist);
    return res;
}

inline ll totalDist(vi& tour) {
    ll res = 0;
    for(int i = 0; i < n; i++) {
        res += d[tour[i]][tour[(i + 1) % n]];
    }
    return res;
}

inline void reverse(vi& tour, vi& pos, int start, int end) {
    int numSwaps = ((start <= end ? 0 : n) + 1 + end - start) / 2;
    int i = start, j = end;
    for(int c = 0; c < numSwaps; c++) {
        swap(tour[i], tour[j]);
        pos[tour[i]] = i, pos[tour[j]] = j;
        i = (i + 1) % n, j = (j + n - 1) % n;
    }
}

random_device rd;
default_random_engine rng(rd());

inline vi perturb(vi& tour) {
    vi newTour;
    uniform_int_distribution<int> rdm(1, n / 4);
    int A = rdm(rng);
    int B = A + rdm(rng);
    int C = B + rdm(rng);
    copy(tour.begin(), tour.begin() + A, back_inserter(newTour));
    copy(tour.begin() + C, tour.end(), back_inserter(newTour));
    copy(tour.begin() + B, tour.begin() + C, back_inserter(newTour));
    copy(tour.begin() + A, tour.begin() + B, back_inserter(newTour));
    return newTour;
}

void twoOpt(vi& tour, vi& pos, ll& maxD) {
    int p, q, r, s, pi, qi, ri, si;
    bool opt = false;
    while(!opt) {
        opt = true;
        for(pi = 0; pi < n; pi++) {
            qi = (pi + 1) % n;
            p = tour[pi], q = tour[qi];
            for(int i = 0; i < k; i++) {
                ri = pos[adj[p][i]];
                si = (ri + 1) % n;
                r = tour[ri], s = tour[si];
                if(p == s or q == r) continue;
                if(d[p][r] + minD > d[p][q] + maxD) break;
                if(d[p][r] + d[q][s] < d[p][q] + d[r][s]) {
                    reverse(tour, pos, qi, ri);
                    maxD = getMax(vll{maxD, d[p][r], d[q][s]});
                    opt = false;
                    break;
                }
            }
        }
    }
}

void threeOpt(vi& tour, vi& pos, ll& maxD, const chrono::time_point<chrono::high_resolution_clock>& deadline) {
    int p, q, r, s, t, u, pi, qi, ri, si, ti, ui;
    bool opt = false;
    while(!opt) {
        opt = true;
        bool curOpt = true;
        for(pi = 0; pi < n; pi++) { // iterate all pq
            curOpt = true;
            qi = (pi + 1) % n;
            p = tour[pi], q = tour[qi];
            if(chrono::high_resolution_clock::now() > deadline) return;
            for(int i = 0; curOpt && i < k; i++) { // iterate all rs
                si = pos[adj[p][i]];
                ri = (si + n - 1) % n;
                r = tour[ri], s = tour[si];
                if(p == r || q == r) continue;
                if(d[p][s] + 2 * minD > d[p][q] + 2 * maxD) break;
                if(d[p][s] + 2 * minD > d[p][q] + d[r][s] + maxD) continue;
                for (int j = 0; curOpt && j < k; j++) { // iterate all tu
                    ui = pos[adj[p][j]];
                    ti = (ui + n - 1) % n;
                    t= tour[ti], u = tour[ui];
                    if (u == s || t == s || u == r || t == p || t == q) continue;
                    if (d[p][s] + d[q][u] + minD > d[p][q] + d[r][s] + maxD) break;
                    // reorder pqrstu using ABCDEF
                    int A, B, C, D, E, F, Ai, Bi, Ci, Di, Ei, Fi;
                    E = t, Ei = ti, F = u, Fi = ui;
                    if ((ri < pi && pi < ti) || (ti < ri && ri < pi) || (pi < ti && ti < ri)) {
                        A = r, Ai = ri, B = s, Bi = si, C = p, Ci = pi, D = q, Di = qi;
                    } else {
                        A = p, Ai = pi, B = q, Bi = qi, C = r, Ci = ri, D = s, Di = si;
                    }
                    ll currDist = d[A][B] + d[C][D] + d[E][F];
                    if(d[D][A] + d[F][B] + d[C][E] < currDist) { // DA, FB, CE
                        reverse(tour, pos, Fi, Ai), reverse(tour, pos, Di, Ei);
                        maxD = getMax(vll{maxD, d[D][A], d[F][B], d[C][E]});
                        curOpt = false;
                    } else if(d[B][D] + d[E][A] + d[F][C] < currDist) { // BD, EA, FC
                        reverse(tour, pos, Fi, Ai), reverse(tour, pos, Bi, Ci);
                        maxD = getMax(vll{maxD, d[B][D], d[E][A], d[F][C]});
                        curOpt = false;
                    } else if(d[A][C] + d[B][E] + d[D][F] < currDist) { // AC, BE, DF
                        reverse(tour, pos, Bi, Ci), reverse(tour, pos, Di, Ei);
                        maxD = getMax(vll{maxD, d[A][C], d[B][E], d[D][F]});
                        curOpt = false;
                    } else if(d[B][E] + d[D][A] + d[F][C] < currDist) { // BE, DA, FC
                        reverse(tour, pos, Ai, Fi), reverse(tour, pos, Bi, Ci), reverse(tour, pos, Di, Ei);
                        maxD = getMax(vll{maxD, d[B][E], d[D][A], d[F][C]});
                        curOpt = false;
                    }
                }
            }
            opt &= curOpt;
        }
    }
}

vi approx(const chrono::time_point<chrono::high_resolution_clock>& deadline) {
    vi tour = greedy(), pos(n);
    ll maxD = 0;
    for(int i = 0; i < n; i++) {
        pos[tour[i]] = i;
        maxD = max(maxD, d[tour[i]][tour[(i + 1) % n]]);
    }
    twoOpt(tour, pos, maxD);
    threeOpt(tour, pos, maxD, deadline);
    chrono::milliseconds totalTime(0), averageTime(0);
    vi bestTour = tour;
    ll bestDist = totalDist(tour);
    for(int i = 0; (chrono::high_resolution_clock::now() + max(chrono::milliseconds(50), 2 * averageTime)) < deadline; i++) {
        debug(bestDist);
        auto start = chrono::high_resolution_clock::now();
        tour = perturb(tour);
        maxD = 0;
        for(int j = 0; j < n; j++) {
            pos[tour[j]] = j;
            maxD = max(maxD, d[tour[j]][tour[(j + 1) % n]]);
        }
        twoOpt(tour, pos, maxD);
        threeOpt(tour, pos, maxD, deadline);
        ll currDist = totalDist(tour);
        if(currDist < bestDist) {
            bestTour = tour;
            bestDist = currDist;
        }
        totalTime += chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
        averageTime = totalTime / (i + 1);
    }
    debug(bestDist);
    return bestTour;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);
    
    // initialize
    cin>>n;
    points.resize(n);
    for(int i = 0; i < n; i++) {
        cin>>points[i].x>>points[i].y;
    }
    d.assign(n, vll(n));
    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            d[i][j] = d[j][i] = points[i].dist(points[j]);
            minD = min(minD, d[i][j]);
        }
    }
    k = min(k, n - 1);
    adj.assign(n, vi(k));
    for(int i = 0; i < n; i++) {
        vi temp;
        for(int j = 0; j < n; j++) {
            if(i != j) temp.pb(j);
        }
        partial_sort(
            temp.begin(), temp.begin() + k, temp.end(), [&](int p, int q) { return d[i][p] < d[i][q]; }
        );
        for(int j = 0; j < k; j++) {
            adj[i][j] = temp[j];
        }
    }

    // compute
    if(n < 20) {
        memo.assign(n, vector<pair<ll, vi>>(1<<(n - 1), {-1, vi()}));
        pair<ll, vi> res = dp(0, (1<<(n - 1)) - 1);
        reverse(all(res.se));
        for(auto i : res.se) cout<<i<<endl;
    } else {
        vi res = approx(chrono::high_resolution_clock::now() + chrono::milliseconds(1900));
        for(auto i : res) cout<<i<<endl;
    }
    
    return 0;
}
