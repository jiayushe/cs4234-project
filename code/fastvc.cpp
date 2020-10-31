#include <bits/stdc++.h>
using namespace std; 

#define endl '\n'
#define umap unordered_map
#define uset unordered_set
#define eb emplace_back
#define pb push_back
#define LSOne(x) (x & (-x))
#define all(x) x.begin(), x.end()
#define rall(x) x.rbegin(), x.rend()
#define sz(x) (int) x.size()
#define rep(i, a, b) for (int i = a; i < b; ++i)
#define trav(a, x) for (auto& a : x)

typedef long long ll;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<double> vd;
typedef vector<vi> vvi;
typedef vector<bool> vb;
typedef vector<vb> vvb;
typedef vector<ii> vii;
typedef vector<ll> vl;

int rc_size;
int usp;
int N, E;

// for edges
vii e; //edge list
vi ew; // edge weights
vii us; // uncovered stack
vi us_index; //mapping from edge number to index in us
//for vertices  
vi W;
vd score;
vvi el; // index of edges
vl t; //timestamp
vi deg; //degree
vi rc; //removal candidates
vi rc_index; // mapping from vertex number to index in rc
vi cc; //confidence change
vi tl; //tabu list
vb cover;
vb best_cover;

ll cost = 0;
ll best_cost = 0;
clock_t start = clock();
uset<int> banlist;
int seed = 0;

void fast() {
    cin.tie(0);
    cout.tie(0);
    ios::sync_with_stdio(0);
}

void add(int v) {
    if(!cover[v]) {
        cost += W[v];
        cover[v] = true;
    }
}

void remove(int v) {
    if(cover[v]) cost -= W[v];
    cover[v] = false;
    score[v] = -score[v];
    cc[v] = 0;

    int rci = rc_index[v];
    int lrc = rc[--rc_size];
    rc[rci] = lrc;
    rc_index[lrc] = rci;
    rc_index[v] = 0;


    // cout << "removing " << v << " with degree " << deg[v] << endl;
    rep(i, 0, deg[v]) {
        int ei = el[v][i];
        ii edge = e[ei];
        int u = edge.first == v ? edge.second : edge.first;
        if(cover[u]) {
            score[u] -= ew[ei];
            // cout << "safe to remove " << v << endl;
        } else {
            // cout << "shouldn't happen " << u << " - " << v << endl;
            score[u] += ew[ei];
            cc[u] = 1;
            us.pb(edge);
            us_index[ei] = usp++;
        }
    }

}

void reset_rc() {
    // Reset remove candidates
    int rci = 0;
    rep(i, 0, N) {
        if(cover[i]) {
            rc_index[i] = rci;
            rc[rci++] = i;
        }
    }
    rc_size = rci;
}

void update_best_cover() {
    if(cost < best_cost) {
        best_cost = cost;
        best_cover = cover;
    }
}

int check_best_cover() {
    rep(i, 0, E) {
        ii edge = e[i];
        int v = edge.first;
        int u = edge.second;
        if(!(best_cover[v] || best_cover[u])) {
            return 0;
        }
    }
    return 1;
}

int check_cover() {
    rep(i, 0, E) {
        ii edge = e[i];
        int v = edge.first;
        int u = edge.second;
        if(!(cover[v] || cover[u])) {
            return 0;
        }
    }
    return 1;
}

int main() {
    fast();
    srand(time(NULL));
    cin >> N >> E;
    W.assign(N, -1);
    score.assign(N, 0);
    deg.assign(N, 0);
    tl.assign(N, 0);
    cc.assign(N, 1);
    t.assign(N, 0);
    el.assign(N, vi()); 
    rc.assign(N, -1);
    rc_index.assign(N, -1);

    trav(i, W) cin >> i;
    vvb AM(N, vb(N, false)); // We store both Adjacency Matrix
    vvi AL(N, vi()); // and Adjacency List
    int kc = 0;
    int ei = 0;
    rep(i, 0, E) {
        int v, u;
        cin >> v >> u;
        if(!AM[v][u] && !AM[u][v]) { 
            e.pb(make_pair(v, u));
            el[v].pb(ei);
            el[u].pb(ei++);
            AL[v].pb(u);
            AL[u].pb(v);
            AM[v][u] = true;
            AM[u][v] = true;
            deg[v]++;
            deg[u]++;
        } else {
            kc++;
        }
    }
    E -= kc;
    ew.assign(E, 1);
    us_index.assign(E, -1);
    cover.assign(N, false);
    best_cover.assign(N, false);

    usp = 0;
    rep(i, 0, E) {
        ii edge = e[i];
        int v = edge.first;
        int u = edge.second;
        if(!(cover[u] || cover[v])) {
            double vs = (double)deg[v] / W[v];
            double us = (double)deg[u] / W[u];
            if(vs > us) {
                cover[v] = true;
                cost += W[v];
            } else {
                cover[u] = true;
                cost += W[u];
            }
        }
    }
    best_cover = cover;
    best_cost = cost;

    int times = 50;
    vi b(E/1024 + 1);
    rep(i, 0, E/1024 + 1) {
        b[i] = i;
    }

    rep(i, 0, times) {
        cover.assign(N, false);
        cost = 0;
        shuffle(b.begin(), b.end(), default_random_engine(seed));
        trav(block, b) {
            int beg = block * 1024;
            int end = min(E, beg + 1024);
            int bs = end - beg + 1;
            vi index(bs);
            rep(j, 0, bs) {
                index[j] = j + beg;
            }
            while(bs > 0) {
                int ri = rand() % bs;
                ii edge = e[index[ri]];
                int v = edge.first;
                int u = edge.second;
                swap(index[ri], index[--bs]);
                if(!(cover[u] || cover[v])) {
                    double vs = (double)deg[v] / W[v];
                    double us = (double)deg[u] / W[u];
                    if(vs > us) {
                        cover[v] = true;
                        cost += W[v];
                    } else {
                        cover[u] = true;
                        cost += W[u];
                    }
                }
            }
        }
        update_best_cover();
    }
    cover = best_cover;
    cost = best_cost;

    if(check_best_cover() == 0) {
        cout << "wrong soln here 1" << endl;
        return 0;
    }

    rep(i, 0, E) {
        ii edge = e[i];
        int v = edge.first;
        int u = edge.second;
        // cout << u << " - " << v << endl;
        if(cover[u] && !cover[v]) {
            // cout << " covered by " << u << endl;
            score[u] -= ew[i];
        } else if(cover[v] && !cover[u]) {
            // cout << " covered by " << v << endl; 
            score[v] -= ew[i];
        }
    }

    reset_rc();

    rep(i, 0, N) {
        if(cover[i] && score[i] == 0) {
            remove(i);
            // cout << "removed " << i << " with degree " << deg[i] << endl;
        }
    }

    update_best_cover();

    if(check_best_cover() == 0) {
        cout << "wrong soln here 2" << endl;
        return 0;
    }


    cout << best_cost << endl;
    rep(i, 0, N) {
        if(best_cover[i]) {
            cout << i << " ";
        }
    }
    cout << endl;
    //cout << float(clock() - start ) /  CLOCKS_PER_SEC;
        

}
