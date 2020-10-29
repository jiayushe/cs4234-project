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

typedef vector<int> vi;
typedef vector<vi> vvi;
typedef vector<bool> vb;
typedef vector<vb> vvb;
typedef long long ll;

vb cover;
vi W;
ll cost = 0;

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

int main() {
    fast();
    int N, E;
    cin >> N >> E;
    W.assign(N, -1);
    trav(i, W) cin >> i;
    vvb AM(N, vb(N, false)); // We store both Adjacency Matrix
    vvi AL(N, vi()); // and Adjacency List
    rep(_, 0, E) {
        int v, u;
        cin >> v >> u;
        AL[v].pb(u);
        AL[u].pb(v);
        AM[v][u] = true;
        AM[u][v] = true;
    }
    cover.assign(N, false);
    // Applying `Constructing a Vertex Cover with Reductions` in https://www.aaai.org/ocs/index.php/AAAI/AAAI18/paper/viewFile/16127/16491
    vi twoNeighbours;
    rep(i, 0, N) { 
        vi curr = AL[i];
        if(sz(curr) == 1) { // Rule 1
            if(W[i] >= W[curr[0]]) { 
                if(!cover[curr[0]]) {
                    add(curr[0]);
                } 
            }
        } else if(sz(curr) == 2) {
            twoNeighbours.pb(i);
            if(AM[curr[0]][curr[1]]) {
                if(W[i] >= (W[curr[0]] + W[curr[1]])) { // Rule 2
                    add(curr[0]);
                    add(curr[1]);
                }
            }
            if(sz(AL[curr[0]]) == 2) {
                if(AM[i][curr[0]] && AM[curr[0]][curr[1]]) { // Rule 3 Start
                    if(W[i] >= W[curr[0]]) {
                        add(curr[0]);
                    } else {
                        add(i);
                    }
                }
            }
            if(sz(AL[curr[1]]) == 2) {
                if(AM[i][curr[1]] && AM[curr[0]][curr[1]]) { 
                    if(W[i] >= W[curr[1]]) {
                        add(curr[1]);
                    } else {
                        add(i);
                    }
                }
            } // Rule 3 End
        }
    }
    int x = sz(twoNeighbours);
    rep(i, 0, x-1) { // Rule 4
        rep(j, i+1, x) {
            if(AL[i] == AL[j] && ((W[i] + W[j]) >= (W[AL[i][0]] + W[AL[i][1]]))) {
                add(AL[i][0]);
                add(AL[i][1]);
            }
        }
    }
    rep(i, 0, N) {
        if(cover[i]) cout << i << " is in cover." << endl;
    }

}
