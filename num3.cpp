#include <iostream>
#include <vector>

using namespace std;

enum Operation {
    ADD,
    SUB,
    MUL,
    XOR
};

int operate(int a, int b, Operation op, int m) {
    switch (op) {
        case ADD:
            return (a + b) % m;
        case SUB:
            return (a - b + m) % m;
        case MUL:
            return (a * b) % m;
        case XOR:
            return (a ^ b) % m;
        default:
            return 0;
    }
}

vector<int> delayedFibonacci(int j, int k, const vector<int>& initial, int m, Operation op, int length) {
    vector<int> S = initial;

    for (int n = initial.size(); n < length; ++n) {
        int val = operate(S[n - j], S[n - k], op, m);
        S.push_back(val);
    }

    return S;
}

Operation readOperation() {
    cout << "Выберите операцию (введите номер):\n";
    cout << "0 - сложение (+)\n1 - вычитание (-)\n2 - умножение (*)\n3 - XOR (^)\n";
    int op;
    cin >> op;
    if (op < 0 || op > 3) {
        cout << "Неверный ввод, по умолчанию выбрано сложение.\n";
        return ADD;
    }
    return static_cast<Operation>(op);
}

int main() {
    int j, k, m, length;
    cout << "Введите j (0 < j < k): ";
    cin >> j;
    cout << "Введите k (k > j): ";
    cin >> k;
    if (j <= 0 || k <= j) {
        cout << "Ошибка: должно выполняться 0 < j < k.\n";
        return 1;
    }
    cout << "Введите модуль m (>0): ";
    cin >> m;
    if (m <= 0) {
        cout << "Ошибка: m должно быть положительным.\n";
        return 1;
    }
    cout << "Введите длину последовательности (>= k+2): ";
    cin >> length;
    if (length < k + 2) {
        cout << "Ошибка: длина должна быть не меньше k+2.\n";
        return 1;
    }

    vector<int> initial(k);
    cout << "Введите " << k << " начальных значений (каждое от 0 до " << m-1 << "):\n";
    for (int i = 0; i < k; ++i) {
        cin >> initial[i];
        if (initial[i] < 0 || initial[i] >= m) {
            cout << "Ошибка: начальные значения должны быть в диапазоне [0, " << m-1 << "]\n";
            return 1;
        }
    }

    Operation op = readOperation();

    auto sequence = delayedFibonacci(j, k, initial, m, op, length);

    cout << "Сгенерированная последовательность (начиная с n = k+1):\n";
    for (int i = k; i < (int)sequence.size(); ++i){
        cout << sequence[i] << " ";
    }
    cout << endl;

    return 0;
}
