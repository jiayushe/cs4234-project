// Trying out simulated annealing in conjunction with 2-opt, haven't added timer and tested
#include <bits/stdc++.h>

using namespace std; 

int N; 

typedef long long ll;  

int computeTourDistance(const vector<int>& dist) {
    ll sum = 0; 
    int j = 1; 
    for (int i = 0; i < N; i++, j++) {
        sum += dist[tour[i]][tour[j % N]]; 
    }
    return sum; 
}

vector<int> greedy(const vector<vector<ll>>& dist) {
    vector<int> tour(N);
    vector<bool> used(N); 
    tour[0] = 0; 
    used[0] = true;
    for (int i = 1; i < N; i++) {
        best = -1;
        for (int j = 0; j < N; j++) {
            if (!used[j] && (best == -1 || dist[tour[i - 1]][j] < dist[tour[i - 1]][best])) best = j; 
        }
        tour[i] = best; 
        used[best] = true;
    }
    return tour; 
}

void reversePath(vector<int>& tour, int start, int end, vector<int>& index) {
    int i = start; 
    int j = end; 
    int length = end - start; 
    int num = 0; 
    if (end >= start) {
        num = length; 
    } else {
        num = (end + N) - start; 
    }
    num = (num + 1)/2
    for (int k; k < num; k++) {
        swap(tour[i], tour[j]);
        index[tour[j]] = i;
        index[tour[i]] = j;   
        i = (i + 1) % N; 
        j = ((j + N) - 1) % N; 
    }
}

void twoExchange(const vector<vector<ll>>& dist, vector<int>& tour, const vector<int>& neighbour, vector<int>& index, int K) {
    int c1, c2, c3, c4; 
    int i, j, m, n; // indices
    bool foundLocalMin = false; 
    while (!foundLocalMin) {
        foundLocalMin = true; 
        int j = 1; 
        for (i = 0; i < N; i++, j++) {
            c1 = tour[i]; 
            c2 = tour[j % N];  
            for (int k = 0; k < K; k++) {
               m = index[neighbour[c1][k]]; 
               n = m + 1; 
               if (m != i and n != j) {
                   c3 = tour[m]; 
                   c4 = tour[n % N]; 
               } else {
                   continue; 
               } 
           }
           // prune
           if (dist[c1][c2] <= dist[c3][c4]) {
               continue; 
           }

           if (dist[c1][c2] + dist[c3][c4] < dist[c2][c3] + dist[c1][c4]) {
               reversePath(tour, c2, c3, index);
               foundLocalMin = false;  
               break; 
           }
        }
    }
}

void anneal(const vector<vector<ll>>& dist, const vector<vector<int>>& neighbour, vector<int>& tour, vector<int>& index) {

    double temperature = 100.0; // start from relatively low temperature to prevent being trapped in an unproductive region
    double delta = 0; 
    double threshold = 0.0001; 
    double coolingFactor = 0.995; 

    srand(time(NULL)); 

    ll curDistance = computeTourDistance(dist); 

    while (temperature > threshold) {
        next = twoExchange(dist, tour, index, neighbour, K); 

        newDistance = computeTourDistance(next); 

        delta = newDistance - curDistance; 

        if ((exp(-delta/temperature) > rand() % 100 + 1) && delta > 0 || delta < 0) {
            for (int i = 0; i < N; i++) tour[i] = next[i];
            curDistance = newDistance;  
        }
        temperature = temperature * coolingFactor; 
    } 
}

int main() {
    cin >> N; 
    
    vector<double> x(N);
    vector<double> y(N);

    vector<vector<ll>> dist; // direct distances between any two cities

    vector<int> tour;

    for (int i = 0; i < N; i++) {
        cin >> x[i] >> y[i]; 
    }

    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) dist[i][j] = dist[j][i] = round(sqrt(pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2))); 
    }

    tour = greedy(dist); 

    
    vector<int> index(N); // city index in the tour vector
    for (int i = 0; i < N; i++) {
        index[tour[i]] = i; 
    }
    

    // k-nearest neighbour, using max_k = 15  
    int K = min(N - 1, 15); 
    vector<vector<int>> neighbour(N, K);
    vector<int> temp(N - 1);

    for (int i = 0; i < N; i++) {
        int l = 0; 
        for (int j = 0; j < N - 1; j++) {
            if (i == j) temp[j] = ++l; 
            else temp[j] = l; 
            l++; 
        }
        sort(temp.begin(), temp.end(), [&](int j, int k) {
                return dist[i][j] < dist[i][k];
            }); 
        copy(temp.begin(), temp.begin() + K, neighbour[i]); 
    }

    anneal(dist, neighbour, tour, index, K); 

    for (auto c : tour) {
        cout << c << endl; 
    }

    return 0; 
}