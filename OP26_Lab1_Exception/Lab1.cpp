#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <limits>

using namespace std;

/* ===================== —“–” “”–» ===================== */

struct Node {
    double x;
    double y;
};

/* ===================== «◊»“”¬¿ÕÕﬂ “¿ ≤Õ“≈–œŒÀﬂ÷≤ﬂ ===================== */

bool loadTable(const string& file, vector<Node>& table) {
    ifstream fin(file);
    if (!fin) return false;

    Node n;
    while (fin >> n.x >> n.y) {
        table.push_back(n);
    }
    fin.close();
    return !table.empty();
}

double interpolate(const vector<Node>& t, double x) {
    for (size_t i = 0; i + 1 < t.size(); i++) {
        if (t[i].x <= x && x <= t[i + 1].x) {
            return t[i].y +
                (t[i + 1].y - t[i].y) * (x - t[i].x) /
                (t[i + 1].x - t[i].x);
        }
    }
    return 0.0;
}

/* ===================== ¿À√Œ–»“Ã 2 ===================== */

double U1(double x) {
    return atan(asin(sin(3 * x)));
}

double T1(double x) {
    return atan(acos(sin(2 * x)));
}

double Qqn1(double x, double y, double z) {
    return x / U1(x) + y * T1(y) - U1(z) * T1(z);
}

double Qnk1(double x, double y) {
    return 1.15 * Qqn1(x, y, x + y)
        - 0.95 * Qqn1(y, x, x - y);
}

double Qnk_alg2(double x, double y) {
    double a = Qnk1(x, y);
    double b = Qnk1(y, x);
    return x * a + y * b - 0.05 * a * b;
}

/* ===================== ¿À√Œ–»“Ã 1 ===================== */

double Qkn(double x, double y,
    const vector<Node>& Utab,
    const vector<Node>& Ttab) {
    double U = interpolate(Utab, x);
    double T = interpolate(Ttab, y);
    if (U == 0) return 0;
    return x / U + y * T;
}

double Qnk(double x, double y,
    const vector<Node>& Utab,
    const vector<Node>& Ttab,
    bool useAlg2) {
    if (useAlg2) return Qnk_alg2(x, y);
    return Qkn(x, y, Utab, Ttab) + x * Qkn(y, x, Utab, Ttab);
}

/* ===================== Rsv ===================== */

double Rsv(double x, double y, double z,
    const vector<Node>& Utab,
    const vector<Node>& Ttab,
    bool useAlg2) {

    if (z > x && z > y)
        return -Qnk(y, z, Utab, Ttab, useAlg2);
    if (y > x && y > z)
        return  Qnk(x, z, Utab, Ttab, useAlg2);
    if (x > y && x > z)
        return  Qnk(z, y, Utab, Ttab, useAlg2);

    return 0;
}

/* ===================== func(x,y,z) ===================== */

double func_xyz(double x, double y, double z,
    const vector<Node>& Utab,
    const vector<Node>& Ttab,
    bool useAlg2) {

    return Rsv(x, y, z, Utab, Ttab, useAlg2)
        + 0.5 * Rsv(y, z, x, Utab, Ttab, useAlg2)
        * Rsv(z, x, y, Utab, Ttab, useAlg2);
}

/* ===================== “≈ —“ ===================== */

double GetFromText(const string& text) {
    ifstream fin("dat3.dat");
    if (!fin) return 0;

    string key;
    double val;
    while (fin >> key >> val) {
        if (key == text)
            return val;
    }
    return 0;
}

double Min4(double a, double b, double c, double d) {
    return min(min(a, b), min(c, d));
}

double Ktext(double x, double y, double z, const string& text) {
    double t = GetFromText(text);
    if (text.empty())
        return Min4(x, y, z, x - z);
    if (t <= 0)
        return Min4(x - y, x - z, y - z, z);
    return t;
}

/* ===================== M-‘”Õ ÷≤Ø ===================== */

double Y(double x) {
    if (100 - x * x < 0) return 0;
    if (x * x - 100 < 1) return 1;
    return log((x * x) / 100.0);
}

double Ysm(double x, double y) {
    return Y(x) * y + 0.7 * Y(y);
}

double Tsm(double x, double y) {
    if (y - 4 * x < 0) return 0;
    return log(pow(x, 5) - 3 * pow(x, 2) + 4 * y * y)
        * pow(Ysm(y, x), 2);
}

double Mts(double x, double y) {
    return x * Tsm(x, y) - y * Tsm(x, x);
}

double Mtl(double x, double y, double z) {
    return x * Mts(x, y) + z * Mts(z, y);
}

/* ===================== func_regr ===================== */

double func_regr(double r, double m, double k) {
    return 10 * k * r - m * r;
}

/* ===================== MAIN ===================== */

int main() {
    double x, y, z;
    string text;

    cin >> x >> y >> z >> text;

    vector<Node> Utab, Ttab;
    bool okU = loadTable("dat1.dat", Utab);
    bool okT = loadTable("dat2.dat", Ttab);

    bool useAlg2 = (!okU || !okT || x <= 5 || x <= 10);

    double r = func_xyz(x, y, z, Utab, Ttab, useAlg2);
    double m = Mtl(x, y, z);
    double k = Ktext(x, y, z, text);

    double result = func_regr(r, m, k);

    cout << "Result = " << result << endl;
    return 0;
}
