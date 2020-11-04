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
vi cc; //configuration change
vi tabu;
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

void add(int v) {
    if(!cover[v]) {
        cost += W[v];
        cover[v] = true;
    }
    score[v] = -score[v];
    rep(i, 0, deg[v]) {
        int ei = el[v][i];
        ii edge = e[ei];
        int u = edge.first == v ? edge.second : edge.first;
        if(cover[u]) {
            score[u] += ew[ei];
        } else {
            score[u] -= ew[ei];
        }
    }


}

void remove(int v) {
    if(cover[v]) cost -= W[v];
    cover[v] = false;
    score[v] = -score[v];

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
        }
    }
}

void remove_redundant() {
    rep(i, 0, N) {
        if(cover[i] && score[i] == 0) {
            remove(i);
            // cout << "removed " << i << " with degree " << deg[i] << endl;
        }
    }
}

int choose_min_loss_vertex() { // Choose v in C such that score(v) is the highest negative number
    double min_loss = -DBL_MAX;
    int to_remove = -1;
    rep(i, 0, N) {
        if(cover[i]) {
            double ascore = score[i] / W[i];
            if(ascore > min_loss) {
                to_remove = i;
                min_loss = ascore;
            }
        }
    }
    if(to_remove == -1) {
        cout << "Choose Min Loss Error" << endl;
        exit(0);
    }
    return to_remove;
}

int choose_bms_vertex() { // 50 Random candidates not in tabu, picking the candidate with the highest negative number
    vi candidates;
    rep(i, 0, N) {
        if(cover[i] && !tabu[i]) candidates.pb(i);
    }

    shuffle(candidates.begin(), candidates.end(), default_random_engine(seed));
    double worst_score = -DBL_MAX;
    int to_remove = -1;
    rep(i, 0, min(sz(candidates), 50)) {
        double ascore = score[candidates[i]] / W[candidates[i]];
        if(ascore > worst_score) {
            worst_score = ascore;
            to_remove = candidates[i];
        }
    }
    if(to_remove == -1) {
        cout << "Choose BMS Error" << endl;
        exit(0);
    }
    return to_remove;

}

vi get_uncovered(int w, int u) {
    vi edge_indices;
    trav(i, el[w]) {
        int a = e[i].first;
        int b = e[i].second;
        if(!cover[a] && !cover[b]) edge_indices.pb(i);
    }
    trav(i, el[u]) {
        int a = e[i].first;
        int b = e[i].second;
        if(!cover[a] && !cover[b]) edge_indices.pb(i);
    }
    return edge_indices;
}

int choose_max_gain_vertex() {
    double max_gain = 0;
    int to_add = -1;
    rep(i, 0, N) {
        if(!cover[i]) {
            double ascore = score[i] / W[i]; 
            if(ascore > max_gain) {
                to_add = i;
                max_gain = ascore;
            }
        }
    }

    // In case no candidates
    if(to_add == -1) {
        cout << "Choose max gain error" << endl;
        exit(0);
    }

    return to_add;
}

void update_cc_remove(int v) {
    cc[v] = 0;
    trav(i, AL[v]) {
        cc[i] = 1;
    }
}

void update_cc_add(int v) {
    trav(i, AL[v]) {
        cc[i] = 1;
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

void update_best_cover() {
    if(check_cover() == 1 && cost < best_cost) {
        best_cost = cost;
        best_cover = cover;
    }
}

int main() {
    //fast();
    srand(time(NULL));
    cin >> N >> E;
    W.assign(N, -1);
    score.assign(N, 0);
    deg.assign(N, 0);
    t.assign(N, 0);
    el.assign(N, vi());
    tabu.assign(N, 0);

    trav(i, W) cin >> i;
    AM.assign(N, vb(N, false)); // We store both Adjacency Matrix
    AL.assign(N, vi()); // and Adjacency List
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
        }
    }
    E = ei;
    ew.assign(E, 1);
    cover.assign(N, false);
    best_cover.assign(N, false);
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
    get_optimal_vertices();
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
            trav(i, optimal_vertices) add(i);
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

    // if(check_best_cover() == 0) {
    //     cout << "wrong soln here 1" << endl;
    //     return 0;
    // }

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


    remove_redundant();

    update_best_cover();

    // if(check_best_cover() == 0) {
    //     cout << "wrong soln here 2" << endl;
    //     return 0;
    // }

    cc.assign(N, 1);

    int ctr = 0;
    
    while(((float(clock() - start) /  CLOCKS_PER_SEC) < 1.90)) {
        // Choose vertices to remove
        int w = choose_min_loss_vertex();
        remove(w);
        
        update_cc_remove(w);
        int u = choose_bms_vertex();
        remove(u);
        update_cc_remove(u);
        tabu.assign(N, 0);
        // Generate Uncovered edges 

        vi uncovered_edges = get_uncovered(u, w);
        // cout << "We removed vertices " << w << " and " << u << endl; 
        
        // Choose vertices to add
        while(sz(uncovered_edges) > 0) {
            int v = choose_max_gain_vertex();
            add(v);
            vi new_ue;
            // Update edges after adding
            trav(i, uncovered_edges) {
                ii edge = e[i];
                if(v != edge.first && v != edge.second) {
                    new_ue.pb(i);
                }
            }
            uncovered_edges = new_ue;
            tabu[v] = 1;
            update_cc_add(v);
            trav(i, uncovered_edges) {
                ew[i]++;
                ii edge = e[i];
                score[edge.first]++;
                score[edge.second]++;
                cc[edge.first] = 1;
                cc[edge.second] = 1;
            }
            // cout << "We added vertice  " << v << endl;
        }
        
        // Clean up and compare
        // cout << "At iteration " << ctr << " cost is " << cost << endl; 
        remove_redundant();
        update_best_cover();
        ctr++;
    }
    

    // if(check_best_cover() == 0) {
    //     cout << "wrong soln here 3" << endl;
    //     return 0;
    // }

    cout << best_cost << endl;
    rep(i, 0, N) {
        if(best_cover[i]) {
            cout << i << " ";
        }
    }
    cout << endl;
    //cout << float(clock() - start ) /  CLOCKS_PER_SEC;
        

}
