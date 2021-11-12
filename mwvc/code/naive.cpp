#include <iostream>
#include <vector>
#include <bitset>
using namespace std;

const int MAX_N = 4000;

int w[MAX_N];
int adj[MAX_N][MAX_N];
bool u[MAX_N];

int main() {
    int n, e;
    cin >> n;
    cin >> e;

    // cout << n << " " <<  e << endl;
    vector<pair<int, int> > edges;
    vector<vector<int> > al(n);

    for(int i = 0; i < n; i++) {
        cin >> w[i];
    }
    // cout << w[0] << endl;    
    for(int i = 0; i < e; i++) {
        int a, b;
        cin >> a;
        cin >> b;
        adj[a][b] = 1;
        adj[b][a] = 1;
        edges.push_back(make_pair(a, b));
        al[a].push_back(b);
        al[b].push_back(a);
    }

    for(auto& edge : edges) {
        int a = edge.first;
        int b = edge.second;
        if(!u[a] && !u[b]) {
            if(w[a] < w[b]) {
                u[a] = true;
            } else if(w[a] > w[b]) {
                u[b] = true;
            } else {
                if(a < b && al[a].size() > 1) {
                    u[a] = true;
                } else {
                    u[b] = true;
                }
            }
        }
    }

    long long tw = 0;
    string vc = "";
    for(int i = 0; i < n; i++) {
        if(u[i]) {
            tw += w[i];
            vc += to_string(i) + " ";
        }
    }
    cout << tw << endl;
    cout << vc << endl;
    return 0;
}