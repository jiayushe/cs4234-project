#include <bits/stdc++.h>

using namespace std;

#define EPS 1e-6

inline double square(double x) {
    return x * x;
}

struct point {
    double x, y;
    point(double _x = 0, double _y = 0) {
        x = _x;
        y = _y;
    }
    double dist(point p) {
        return sqrt(square(x - p.x) + square(y - p.y));
    }
    point update(double dx, double dy) {
        return point(x + dx, y + dy);
    }
};

int n;
vector<point> points;

double totalDist(point p) {
    double res = 0;
    for(auto q : points) {
        res += q.dist(p);
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    
    cin>>n;
    points.resize(n);
    for(int i = 0; i < n; i++) {
        cin>>points[i].x>>points[i].y;
    }
    
    point currP = points[0];
    double currDist = totalDist(currP);
    double step = 10;
    while(step > EPS) {
        int flag = 0;
        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                if(i == 0 and j == 0) continue;
                point nextP = currP.update(step * i, step * j);
                double nextDist = totalDist(nextP);
                if(nextDist < currDist) {
                    currDist = nextDist;
                    currP = nextP;
                    flag = 1;
                }
            }
        }
        if(!flag) step /= 2;
    }
    
    cout<<fixed<<setprecision(6)<<currDist<<endl;
    
    return 0;
}
