#include <iostream>
#include <fstream>
#include <string>
using namespace std;
struct sinhvien {
    int mssv;
    string hoten;
    string nganh;
    double dtb;
};
sinhvien a[12];
int main() {
    ofstream fileOut("sinhvien.txt");
    int n;
    cin >> n;
    for (int i = 0; i<n;i++) {
        cin >> a[i].mssv;
        cin.ignore();
        getline(cin,a[i].hoten);
        getline(cin,a[i].nganh);
        cin >> a[i].dtb;
    }
    for(int i = 0; i< n;i++) {
        for (int j = i;j<n;j++) {
            if(a[i].dtb < a[j].dtb) {
                sinhvien tmp = a[i];
                a[i] = a[j];
                a[j] = tmp;
            }
        }
    }
    if (fileOut.is_open()) {
        fileOut<<n<<'\n';
        for (int i = 0; i< n;i++) {
            cout << a[i].mssv << " " << " " << a[i].hoten<< " " << a[i].nganh << " " << a[i].dtb << '\n';
        }
    }
}
