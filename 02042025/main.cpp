#include <iostream>
#include <algorithm>

using namespace std;

class PS {
    int tuso, mauso;

public:
    PS(int t = 0, int m = 1) : tuso(t), mauso(m) { rutgon(); }

    void nhap() {
        cout << "Nhập tử số: ";
        cin >> tuso;
        cout << "Nhập mẫu số: ";
        cin >> mauso;
        if (mauso == 0) {
            cout << "Lỗi: Mẫu số không thể bằng 0!" << endl;
            exit(1);
        }
        rutgon();
    }

    void in() { cout << tuso << '/' << mauso << endl; }

    PS operator+(PS a) {
        return PS(tuso * a.mauso + a.tuso * mauso, mauso * a.mauso);
    }

    PS operator-(PS a) {
        return PS(tuso * a.mauso - a.tuso * mauso, mauso * a.mauso);
    }

    PS operator*(PS a) {
        return PS(tuso * a.tuso, mauso * a.mauso);
    }

    PS operator/(PS a) {
        return PS(tuso * a.mauso, mauso * a.tuso);
    }

    bool operator<(PS a) { return tuso * a.mauso < a.tuso * mauso; }
    bool operator<=(PS a) { return tuso * a.mauso <= a.tuso * mauso; }
    bool operator>(PS a) { return tuso * a.mauso > a.tuso * mauso; }
    bool operator>=(PS a) { return tuso * a.mauso >= a.tuso * mauso; }
    bool operator==(PS a) { return tuso * a.mauso == a.tuso * mauso; }
    bool operator!=(PS a) { return tuso * a.mauso != a.tuso * mauso; }

    PS operator++() { // Tiền tố ++a
        tuso += mauso;
        return *this;
    }

    PS operator++(int) { // Hậu tố a++
        PS temp = *this;
        tuso += mauso;
        return temp;
    }

    PS operator--() { // Tiền tố --a
        tuso -= mauso;
        return *this;
    }

    PS operator--(int) { // Hậu tố a--
        PS temp = *this;
        tuso -= mauso;
        return temp;
    }

private:
    void rutgon() {
        int gcd = __gcd(abs(tuso), abs(mauso));
        tuso /= gcd;
        mauso /= gcd;
        if (mauso < 0) {
            tuso = -tuso;
            mauso = -mauso;
        }
    }
};

int main() {
    PS a, b, c;
    cout << "Nhập phân số a:" << endl;
    a.nhap();
    cout << "Nhập phân số b:" << endl;
    b.nhap();
    cout << "Nhập phân số c:" << endl;
    c.nhap();

    cout << "Phân số a: "; a.in();
    cout << "Phân số b: "; b.in();
    cout << "Phân số c: "; c.in();

    PS max_ps = (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
    cout << "Phân số lớn nhất: "; max_ps.in();

    cout << "Tổng a + b: "; (a + b).in();
    cout << "Hiệu a - b: "; (a - b).in();
    cout << "Hiệu a - c: "; (a - c).in();
    cout << "Tích a * b: "; (a * b).in();
    cout << "Thương a / b: "; (a / b).in();

    return 0;
}
