#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    int n, m;
    cin >> n >> m;
    vector<int> A(n);
    for (int &x : A) cin >> x;

    vector<long long> prefix(n + 1, 0);
    for (int i = 0; i < n; ++i)
        prefix[i + 1] = prefix[i] + A[i];

    vector<long long> dp(n + 1, 0);  // dp[i] — макс. разница от i до конца

    for (int i = n - 1; i >= 0; --i) {
        long long best = -1e18; // заменили LLONG_MIN
        for (int k = 1; k <= m && i + k <= n; ++k) {
            long long sum = prefix[i + k] - prefix[i];
            best = max(best, sum - dp[i + k]);
        }
        dp[i] = best;
    }
    
    cout << (dp[0] > 0 ? 1 : 0) << endl;
    return 0;
}
