#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

// Функция: моделирует остывание кофе
void cofe(double Tk, double Tsr, double r, int n, vector<double>& t, vector<double>& T) {
    for (int i = 0; i <= n; ++i) {  // от 0 до n минут включительно
        t.push_back(i);  // время
        double temp = Tsr + (Tk - Tsr) * exp(-r * i);  // формула остывания
        T.push_back(temp);
    }
}

// Функция: находит коэффициенты аппроксимирующей прямой
void agroh(const vector<double>& x, const vector<double>& y, double& a, double& b) {
    int n = x.size();
    double sum_x = 0, sum_y = 0, sum_x2 = 0, sum_xy = 0;

    for (int i = 0; i < n; i++) {
        sum_x += x[i];
        sum_y += y[i];
        sum_x2 += x[i] * x[i];
        sum_xy += x[i] * y[i];
    }

    a = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    b = (sum_y - a * sum_x) / n;
}

// Функция: считает коэффициент корреляции Пирсона
double korgel(const vector<double>& x, const vector<double>& y) {
    int n = x.size();
    double avg_x = 0, avg_y = 0;

    for (int i = 0; i < n; i++) {
        avg_x += x[i];
        avg_y += y[i];
    }
    avg_x /= n;
    avg_y /= n;

    double numerator = 0, denom_x = 0, denom_y = 0;
    for (int i = 0; i < n; i++) {
        numerator += (x[i] - avg_x) * (y[i] - avg_y);
        denom_x += (x[i] - avg_x) * (x[i] - avg_x);
        denom_y += (y[i] - avg_y) * (y[i] - avg_y);
    }

    return numerator / sqrt(denom_x * denom_y);
}

int main() {
    double Tk, Tsr, r;
    int n;

    // Ввод данных
    cout << "Введите температуру окружающей среды (Tsr): ";
    cin >> Tsr;
    cout << "Введите начальную температуру кофе (Tk): ";
    cin >> Tk;
    cout << "Введите коэффициент остывания (r): ";
    cin >> r;
    cout << "Введите продолжительность наблюдения в минутах (n): ";
    cin >> n;

    vector<double> t, T;

    // Вычисление температур
    cofe(Tk, Tsr, r, n, t, T);

    // Аппроксимация
    double a, b;
    agroh(t, T, a, b);

    // Корреляция
    double r_corr = korgel(t, T);

    // Вывод результатов
    cout << fixed << setprecision(3);
    cout << "\nТаблица значений:\n";
    cout << "Время (мин)\tТемпература (°C)\n";
    for (size_t i = 0; i < t.size(); ++i) {
        cout << t[i] << "\t\t" << T[i] << "\n";
    }

    cout << "\nАппроксимирующая прямая:\n";
    cout << "y = " << a << " * x + " << b << endl;

    cout << "Коэффициент корреляции: r = " << r_corr << endl;

    return 0;
}
