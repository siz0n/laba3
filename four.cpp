#include <iostream>
#include <vector>
#include <deque>
using namespace std;

int main() {
    int n, m;
    cin >> n >> m;
    vector<int> A(n);
    for (int &x : A) 
    cin >> x;

    deque<long long> dq;
    dq.push_back(0); // Базовый случай: dp[n] = 0

    for (int i = n - 1; i >= 0; --i) {
        long long best = -1e18; // Минимальное значение для инициализации
    long long sum = 0;
    int max_k = min(m, n - i); // Сколько чисел можно взять на текущем шаге     
        // Используем только первые max_k элементов из дека
        for (int k = 1; k <= max_k; ++k) {
            sum += A[i + k - 1]; // Накопление суммы взятых чисел
            if (k - 1 < dq.size()) {
                best = max(best, sum - dq[k - 1]); // Оптимальный выбор
            }
        }
        // Добавляем текущий best в начало дека
        dq.push_front(best);
        // Удерживаем размер дека не больше m
        if (dq.size() > m) dq.pop_back();
    }

    cout << (dq.front() > 0 ? 1 : 0) << endl;
    return 0;
}