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

typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<double> vd;
typedef vector<vi> vvi;
typedef vector<bool> vb;
typedef vector<vb> vvb;
typedef long long ll;

vb cover;
vi W;
vd score;
ll cost = 0;
clock_t start = clock();
uset<int> banlist;

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
}

int main() {
    fast();
    srand(time(NULL));
    int N, E;
    cin >> N >> E;
    W.assign(N, -1);
    score.assign(N, -1);
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
        if(sz(curr) == 1) { // Rule 1 N(v) = {u}
            if(W[i] >= W[curr[0]]) { // W(v) > W(u)
                if(!cover[curr[0]]) {
                    banlist.insert(curr[0]);
                    add(curr[0]);
                } 
            }
        } else if(sz(curr) == 2) { // N(v) = {n1, n2}
            twoNeighbours.pb(i);
            if(AM[curr[0]][curr[1]]) { // Rule 2 v, n1, n2 are a clique
                if(W[i] >= (W[curr[0]] + W[curr[1]])) { // W(v) > W(n1) + W(n2)
                    add(curr[0]);
                    add(curr[1]);
                    banlist.insert(curr[0]);
                    banlist.insert(curr[1]);
                }
            }
            if(sz(AL[curr[0]]) == 2) { // Rule 3 Start
                if(AM[curr[0]][curr[1]]) {  // N(n1) = {v, n2}
                    if(W[i] >= W[curr[0]]) { // We only need either v or n1
                        add(curr[0]);
                        banlist.insert(curr[0]);
                    } else {
                        add(i);
                        banlist.insert(i);
                    }
                }
            }
            if(sz(AL[curr[1]]) == 2) {
                if(AM[curr[0]][curr[1]]) { // N(n2) = {v, n1}
                    if(W[i] >= W[curr[1]]) { // We only need either v or n2
                        add(curr[1]);
                        banlist.insert(curr[1]);
                    } else {
                        add(i);
                        banlist.insert(i);
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
                add(AL[u][0]);
                add(AL[u][1]);
                banlist.insert(AL[u][0]);
                banlist.insert(AL[u][1]);
            }
        }
    }
    rep(i, 0, N-1) {
        rep(j, i+1, N) { // only half of AM since symmetrical.
            if(AM[i][j] && !cover[i] && !cover[j]) {
                add(i); add(j); // 2 approx
            }
        }
    }

    ll best = cost;
    vb snapshot = cover;

    // Naive
    vb naiveCover(N, false);
    rep(i, 0, N-1) {
        rep(j, i, N) {
            if(AM[i][j]) {
                if(W[i] < W[j]) {
                    naiveCover[i] = true;
                } else if(W[i] > W[j]) {
                    naiveCover[j] = true;
                } else if(i < j) {
                    naiveCover[i] = true;
                } else {
                    naiveCover[j] = true;
                }
            }
        }
    }
    ll naiveCost = 0;
    rep(i, 0, N) {
        if(naiveCover[i]) naiveCost += W[i];
    }
    if(naiveCost < best) {
            snapshot = cover = naiveCover;
            best = cost = naiveCost;
        }
    // Naive end

    // Score
    rep(i, 0, N) {
        ll sum = W[i];
        rep(j, 0, N) {
            if(AM[i][j]) sum += W[j];
        }
        score[i] = 1 - (W[i] / sum);
    }

    int rmvCount = 3;

    while(((float(clock() - start) /  CLOCKS_PER_SEC) < 1.97)) {
        // Choose vertices to remove
        uset<int> toRemoveSet;
        rep(i, 0, rmvCount) {
            double maxscore = -1;
            int toRemove = -1;
            rep(i, 0, N) {
                if(cover[i] && score[i] > maxscore) {
                    if((toRemoveSet.count(i) == 0) && (banlist.count(i) == 0)) {
                        maxscore = score[i];
                        toRemove = i;
                    }
                }
            }
            toRemoveSet.insert(toRemove);
        }
        cout << sz(toRemoveSet) << endl;
        trav(i, toRemoveSet) {
            //cout << "Removing " << i << endl; 
            remove(i);
        }
        // Choose vertices to add
        trav(i, toRemoveSet) {
            trav(j, AL[i]) {
                add(j);
                //if(!cover[i]) cout << "Adding " << i << endl;
            }
        }

        // Clean up and compare
        toRemoveSet.clear();
        cout << "New cost after iteration is " << cost << endl;
        if(cost < best) {
            snapshot = cover;
            best = cost;
        }
    }


    cout << best << endl;
    rep(i, 0, N) {
        if(snapshot[i]) {
            cout << i << " ";
        }
    }
    cout << endl;
    //cout << float(clock() - start ) /  CLOCKS_PER_SEC;

}
