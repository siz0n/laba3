#include <iostream>
using namespace std;

// Ручная реализация НОД (алгоритм Евклида)
int gcd(int a, int b) {
    while (b != 0) {
        int r = a % b;
        a = b;
        b = r;
    }
    return a;
}

// Проверка на расходимость
bool diverges(int a, int b) {
    return b <= 1;
}

// Проверка на рациональность
bool isRational(int a) {
    return a == 1;
}

// Сокращение дроби
pair<int, int> reduce(int num, int den) {
    int g = gcd(num, den);
    return {num / g, den / g};
}

int main() {
    int a, b;
    cin >> a >> b;

    if (diverges(a, b)) {
        cout << "infinity\n";
    } else if (!isRational(a)) {
        cout << "irrational\n";
    } else {
        // Формула суммы для a = 1: S = b / (b - 1)^2
        int num = b;
        int den = (b - 1) * (b - 1);
        auto [n, d] = reduce(num, den);
        cout << n << "/" << d << "\n";
    }

    return 0;
}
