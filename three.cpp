#include <iostream>
#include <cmath>    
#include <numeric>  
using namespace std;

// Проверка, является ли число value приближённо рациональным
bool is_rational(double value, int max_denominator, int& numerator_out, int& denominator_out) {
    const double EPS = 1e-9; // допустимая погрешность
    for (int denom = 1; denom <= max_denominator; ++denom) {
        int numer = round(value * denom); // предполагаемый числитель
        double approx = static_cast<double>(numer) / denom; // приближённая дробь
        if (fabs(value - approx) < EPS) {
            // Сокращаем дробь
            int g = gcd(numer, denom);
            numerator_out = numer / g;
            denominator_out = denom / g;
            return true; // нашли подходящую рациональную аппроксимацию
        }
    }
    return false; 
}

int main() {
    int a, b;
    cout << "Введите a и b: ";
    cin >> a >> b;

    // Проверка сходимости ряда
    if (b <= 1) {
        cout << "infinity" << endl;
        return 0;
    }

    double sum = 0.0;
    double term;
    const int MAX_N = 1000;     // максимум слагаемых
    const double EPS = 1e-10;   // порог остановки

    // Вычисляем сумму ряда
    for (int n = 1; n <= MAX_N; ++n) {
        term = pow(n, a) / pow(b, n); 
        sum += term;
        if (term < EPS) break; // если добавка стала мала — останавливаемся
    }

    // Пытаемся выразить сумму как рациональную дробь
    int numer, denom;
    if (is_rational(sum, 100000, numer, denom)) {
        cout << "Сумма: " << numer << "/" << denom << endl;
    } else {
        cout << "irrational" << endl;
    }

    return 0;
}
