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

int rc_size = 0;
int usp = 0;
int N, E;

// for edges
vii e; //edge list
vi ew; // edge weights
vi us; // uncovered stack
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
vi optimal_vertices;
vvb AM; // Adjacency Matrix
vvi AL; // Adjacency List

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
            // cout << "removing " << v << " makes uncovered edge " << v << " - " << u << endl; 
            score[u] += ew[ei];
            cc[u] = 1;
            us[usp] = ei;
            us_index[ei] = usp++;
            // cout << "added " << e[ei].first << " - " << e[ei].second << " to uncov stack " << endl;
        }
    }

}

void get_optimal_vertices() {
    // Applying `Constructing a Vertex Cover with Reductions` in https://www.aaai.org/ocs/index.php/AAAI/AAAI18/paper/viewFile/16127/16491
    vi twoNeighbours;
    rep(i, 0, N) { 
        vi curr = AL[i];
        if(sz(curr) == 1) { // Rule 1 N(v) = {u}
            if(W[i] >= W[curr[0]]) { // W(v) > W(u)
                optimal_vertices.pb(curr[0]);
            }
        } else if(sz(curr) == 2) { // N(v) = {n1, n2}
            twoNeighbours.pb(i);
            if(AM[curr[0]][curr[1]]) { // Rule 2 v, n1, n2 are a clique
                if(W[i] >= (W[curr[0]] + W[curr[1]])) { // W(v) > W(n1) + W(n2)
                    optimal_vertices.pb(curr[0]);
                    optimal_vertices.pb(curr[1]);
                }
            }
            if(sz(AL[curr[0]]) == 2) { // Rule 3 Start
                if(AM[curr[0]][curr[1]]) {  // N(n1) = {v, n2}
                    if(W[i] >= W[curr[0]]) { // We only need either v or n1
                        optimal_vertices.pb(curr[0]);
                    } else {
                        optimal_vertices.pb(i);
                    }
                }
            }
            if(sz(AL[curr[1]]) == 2) {
                if(AM[curr[0]][curr[1]]) { // N(n2) = {v, n1}
                    if(W[i] >= W[curr[1]]) { // We only need either v or n2
                        optimal_vertices.pb(curr[1]);
                    } else {
                        optimal_vertices.pb(i);
                    }
                }
            } // Rule 3 End
        }
    }
    int x = sz(twoNeighbours);
    rep(i, 0, x-1) { // Rule 4
        rep(j, i+1, x) {
            int u = twoNeighbours[i];
            int v = twoNeighbours[j];
            if(AL[u] == AL[v] && // / N(u) = N(v) ={n1, n2}
            ((W[u] + W[v]) >= (W[AL[u][0]] + W[AL[u][1]])) && // W(u) + W(v) >= W(n1) + W(n2)
            (!AM[AL[u][0]][AL[u][1]])) { // {n1, n2} !∈ E
                optimal_vertices.pb(AL[u][0]);
                optimal_vertices.pb(AL[u][1]);
            }
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

int remove1() {

    //IDEA: remove all with score 0

    int br = rc[0];
    double brs = (double)W[br] / (double)abs(score[br]);

    if(score[brs] != 0) {
        rep(i, 1, rc_size) {
            int v = rc[i];
            if(score[v] == 0) break;
            double s = (double)W[v] / (double)abs(score[v]);
            if(s > brs) {
                brs = s;
                br = v;
            }
        }
    } 
    remove(br);
    return br;
}

int remove2(int num) {
    int br = rc[rand() % rc_size];
    double brs = (double)W[br] / (double)abs(score[br]);
    rep(i, 0, num) {
        int v = rc[rand() % rc_size];
        double s = (double)W[v] / (double)abs(score[v]);
        if(tl[v] == 1 || s < brs) continue;
        else if(s > brs) {
            br = v;
            brs = s;
        } else {
            if(t[v] < t[br]) {
                br = v;
                brs = s;
            }
        }
    }
    return br;
}

int chooseAdd(int r1, int r2 = 0) {
    double bs = 0.0;
    int a = 0;

    rep(i, 0, deg[r1]) {
        int ei = el[r1][i];
        ii edge = e[ei];
        int u = edge.first == r1 ? edge.second : edge.first;
        if(cover[u] || cc[u] == 0) continue;
        double s = (double)score[u] / (double)W[u];
        if(s > bs) {
            bs = s;
            a = u;
        } else if(s == bs) {
            if(t[u] < t[a]) {
                a = u;
            }
        }
    }
    if(cc[r1] == 1 && !cover[r1]) {
        double s = (double)score[r1] / (double)W[r1];
        if(s > bs) {
            bs = s;
            a = r1;
        } else if(s == bs) {
            if(t[r1] < t[a]) {
                a = r1;
            }
        }
    }

    if(r2 != 0) {
        rep(i, 0, deg[r2]) {
            int ei = el[r2][i];
            ii edge = e[ei];
            int u = edge.first == r2 ? edge.second : edge.first;
            if(cover[u] || cc[u] == 0) continue;
            double s = (double)score[u] / (double)W[u];
            if(s > bs) {
                bs = s;
                a = u;
            } else if(s == bs) {
                if(t[u] < t[a]) {
                    a = u;
                }
            }
        }
        if(cc[r2] == 1 && !cover[r2]) {
            double s = (double)score[r2] / (double)W[r2];
            if(s > bs) {
                bs = s;
                a = r2;
            } else if(s == bs) {
                if(t[r2] < t[a]) {
                    a = r2;
                }
            }
        }
    }
    return a;
}

void add(int v) {
    cover[v] = true;
    score[v] = -score[v];
    cost += W[v];

    rc[rc_size] = v;
    rc_index[v] = rc_size++;

    rep(i, 0, deg[v]) {
        int ei = el[v][i];
        ii edge = e[ei];
        int u = edge.first == v ? edge.second : edge.first;
        if(cover[u]) {
            score[u] += ew[ei];
            // cout << i << " out of " << deg[v] << " alr covered" << endl;
        } else {
            // cout << " uncovered edge found! " << u << endl;
            score[u] -= ew[ei];
            cc[u] = 1;
            int lucei = us[--usp];
            int idx = us_index[ei];
            us[idx] = lucei;
            us_index[lucei] = idx;
        }
        // cout << i << " out of " << deg[v] <<  endl;
    }
}

void update_ew() {
    rep(i, 0, usp) {
        int ei = us[i];
        ew[ei] += 1;
        ii edge = e[ei];
        score[edge.first] += 1;
        score[edge.second] += 1;
        cc[edge.first] = 1;
        cc[edge.second] = 1;
    }
}

int main() {
    fast();
    srand(time(NULL));
    cin >> N >> E;
    cout << "here 6" << endl;
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
    cout << "here 5" << endl;
    AM.assign(N, vb(N, false)); // We store both Adjacency Matrix
    AL.assign(N, vi()); // and Adjacency list
    cout << "here 4" << endl;
    int ei = 0;
    rep(i, 0, E) {
        int v, u;
        cin >> v >> u;
        cout << v << " - " << u << endl;
        if(!AM[v][u] && !AM[u][v]) { 
            cout << "not seen before" << endl;
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
            cout << "seen before" << endl;
        }
    }

    cout << "took input" <<  endl;
    E = ei;

    cout << "took input" <<  endl;

    ew.assign(E, 1);
    us_index.assign(E, -1);
    us.assign(E, -1);
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

    cout << "here 3" << endl;
    get_optimal_vertices();
    cout << "here 2" << endl;
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
            trav(i, optimal_vertices) {
                cover[i] = true;
                cost += W[i];
            }
            cout << "here " << endl;
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
        return 0;
    }

    cout << best_cost << endl;
    rep(i, 0, N) {
        if(best_cover[i]) {
            cout << i << " ";
        }
    }
    cout << endl;

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

    cout << best_cost << endl;
    rep(i, 0, N) {
        if(best_cover[i]) {
            cout << i << " ";
        }
    }
    cout << endl;

    int av, r1, r2 = 0;
    int step = 1;

    while(true) {
        update_best_cover();  
        if((float(clock() - start) /  CLOCKS_PER_SEC) > 1.97) {
            cout << "time elapsed" << endl;
            break;
        }
        r1 = remove1();
        cout << "uncovered edges left after removing " << r1 << endl;
        rep(i, 0, usp) {
            int ei = us[i];
            ii edge = e[ei];
            cout << edge.first << " - " << edge.second << endl;
        }
        r2 = remove2(50);

        remove(r2);
        cout << "uncovered edges left after removing " << r2 << endl;
        rep(i, 0, usp) {
            int ei = us[i];
            ii edge = e[ei];
            cout << edge.first << " - " << edge.second << endl;
        }

        t[r2] = step;


        tl.assign(N, 0);
        cout << r1 << " and " << r2 << " were removed" << endl;
        while(usp > 0) {
            cout << "uncovered edges left" << endl;
            rep(i, 0, usp) {
                int ei = us[i];
                ii edge = e[ei];
                cout << edge.first << " - " << edge.second << endl;
            }
            int toAdd = chooseAdd(r1, r2);
            cout << " adding back " << toAdd <<  endl;
            add(toAdd);
            update_ew();
            tl[toAdd] = 1;
            t[toAdd] = step;
        }
        cout << "uncovered edges left after all additions" << endl;
        rep(i, 0, usp) {
            int ei = us[i];
            ii edge = e[ei];
            cout << edge.first << " - " << edge.second << endl;
        }
        rep(i, 0, rc_size) {
            int v = rc[i];
            if(cover[v] && score[v] == 0) {
                remove(v);
                i--;
                // cout << "removed " << i << " with degree " << deg[i] << endl;
            }
        }
        cout << "uncovered edges left after removing redundancies" << endl;
        rep(i, 0, usp) {
            int ei = us[i];
            ii edge = e[ei];
            cout << edge.first << " - " << edge.second << endl;
        }
        cout << step << " iteration(s) done " << endl;
        step++;
    }

    // cout << "made it here 10" << endl;
    cout << best_cost << endl;
    rep(i, 0, N) {
        if(best_cover[i]) {
            cout << i << " ";
        }
    }
    cout << endl;
    // cout << float(clock() - start ) /  CLOCKS_PER_SEC;
}
