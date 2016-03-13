/*
Napisati program koji proverava koje se od zadatih tacaka nalaze u/na kruuznici, a koje su van kruznice.
Tacke se nalaze u vektoru points:
    struct Point { double x, y; };
    vector<Point> points;
Definisati klasu:
    class Circle {
        ...
    public:
        Circle(const Point& t, const double r_);
        void check(const vector<Point> p, vector<bool>& belong);
    };
Objekt tipa Circle, za svaku tacku iz vektora, izracunava da li se ona nalazi:
u krugu (ili na kruznici) - korespodentni element vektora belong dobija vrednost true ili
van kruga - korespodentni element vektora belong dobija vrednost false.

Stvoriti nit koja poziva funkciju check nad objektom klase Circle.
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
using namespace std;

struct Point {double x, y;};

class Circle{
    private:
        Point center;
        double r;
        bool in(Point point) {
            return pow((point.x - center.x),2) + pow((point.y - center.y),2) <= pow(r,2);
        }
    public:
        Circle(const Point c, const double r) {
            center = c;
            this->r = r;
        }

        void getInfo() {
            cout << "center = " << center.x << " " << center.y << " r " << r << endl;
        }

        void check(const vector<Point> p, vector<bool>& belong) {
            for(int i = 0; i < p.size(); i++)
                belong[i] = in(p[i]);
        }
};

void f(Circle k, vector<Point> tacke, vector<bool>& belong) {
    k.check(tacke,belong);
}

int main()
{
    Circle krug({0,0}, 3);
    krug.getInfo();
    vector<Point> tacke = {{0,0}, {1,1}, {0,3}, {3,5}, {8,2}};
    vector<bool> belong(tacke.size());
    thread t(f,krug,tacke,ref(belong));
    t.join();
    for(int i = 0; i < belong.size(); i++)
        cout << tacke[i].x << " " << tacke[i].y << " " << belong[i] << endl;

    return 0;
}
