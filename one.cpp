
 // -4 4 
#include<iostream>
#include<cmath>
#include<iomanip>
using namespace std;

double funcs(double x) {
    if (x <= 0) {
        return sqrt(4 - (x + 2) * (x + 2));
    } else if (x >= 0 && x < 0.5) {
        return 0;
    } else if (x >= 0.5 && x < 2) {
        return log(x) / x;
    } else {
        return 1;
    }
}
int main() {
	setlocale(LC_ALL, "RU");
	double x_start, x_end, dx;
	cout << "Введите x начальное:" << endl;
	cin >> x_start;
	cout << "Введите x конечное" << endl;
	cin >> x_end;
	while (x_end <= x_start) {
		cout << "x начальное не может быть меньше x конечного" << endl;
		cout << "Введите коррректное значение x конечного:";
			cin >> x_end;
	}
	cout << "Ввелите шаг:" << endl;
	cin >> dx;
	while (dx <= 0) {
		cout << "Шаг не может быть отрицательным или равен нулю" << endl;
		cout << "Введите корректное значение шага:";
		cin >> dx;
	}
	cout << fixed << setprecision(4);
	cout << "   X\t\tf(X)" << endl;
	for (double x = x_start; x <= x_end; x+=dx) {
		double y = funcs(x);
		cout << setw(8) << x << "\t" << setw(8) << y << endl;
	}
	return 0;
}