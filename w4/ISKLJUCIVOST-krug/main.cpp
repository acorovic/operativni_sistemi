/*
Napraviti konkurentni program koji odreÄ‘uje najbliÅ¾u taÄku kruÅ¾nici (iz velikog vektora taÄaka - npr 100000000 elemenata).
Program realizovati uz pomoÄ‡ 2 klase:

1) Klasa Closest_point modelira najbliÅ¾u taÄku kruÅ¾nici. Interfejs klase sadrÅ¾i sledeÄ‡e metode:

class Closest_point {
    void test_and_set(const Point& p, double d);
    friend ostream& operator<<(ostream& os, Closest_point& cp);
};

Metoda test and set treba da proveri da li je prosleÄ‘ena taÄka p bliÅ¾a od trenutno najbliÅ¾e taÄke i da ako jeste
 postavi parametre
nove najbliÅ¾e taÄke. Operator sluÅ¾i za ispis koordinata najbliÅ¾e taÄke na ekran.

2) Klasa Circle modeluje kruznicu. Interfejs klase sadrÅ¾i sledeÄ‡e metode:

class Circle {
   double circumference_distance(const Point& t);
   void find_closest(VPci p_begin, VPci p_end, Closest_point& cpoint);
};

Metoda circumference_distance treba da izraÄuna udaljenost taÄke od kruÅ¾nice po formuli datoj na slajdovima.
Operator sluÅ¾i za prolazak kroz vektor taÄaka i poreÄ‘enje pojedinaÄnih taÄaka vektora sa trenutno najbliÅ¾om taÄkom
(globalnom taÄkom). Dato poreÄ‘enje se realizuje pozivanjem metode test_and_set na objektu najbliÅ¾e taÄke (cpoint).

Kreirati jedan globalni objekat najbliÅ¾e taÄke.

Kreirati 3 niti, pri cemu tela niti predstavlja data funkcija f
void f(Circle& c, VPci p_begin, VPci p_end, Closest_point& cpoint) {
    c.find_closest(p_begin, p_end, cpoint);
}
Svakoj niti se prosledjuje referenca na kruznicu koja se posmatra,1/3 vektora taÄaka kao i referenca na
 globalni objekat najbliÅ¾e taÄke.
Nakon zavrÅ¡etka niti ispisati koodrdinate najbliÅ¾e taÄke.

Napomena: konstruktore klasa Closest_point i Circle realizovati samostalno.
Kreirati veliki vektor taÄaka (npr. 10000 elemenata) sa sluÄajnim vrednostima koordinata taÄaka (ne predalekim od kruÅ¾nice).
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cmath>

using namespace std;

struct Point{
    double x;
    double y;
};

typedef vector<Point>::const_iterator VPci;

class Closest_point {
    private:
        Point p0;
        double distance;
        mutex m;
        bool prazan;
    public:
        Closest_point() {
            distance = 0;
            prazan = true;
        }
        void test_and_set(const Point& p, double d);
        friend ostream& operator<<(ostream& os, Closest_point& cp) {
            os << "x = " << cp.p0.x << " y = " << cp.p0.y << endl;
            return os;
        }
};

void Closest_point::test_and_set(const Point& p, double d) {
    unique_lock<mutex> lock(m);
    if(prazan) {
        p0 = p;
        distance = d;
        prazan = false;
        return;
    }
    if(d < distance) {
        distance = d;
        p0 = p;
    }
}

class Circle {
    private:
        Point p0;
        double r;
    public:
        Circle(Point c, double rr) {
            p0 = c;
            r = rr;
        }
        double circumference_distance(const Point& t) {
            return abs(sqrt(pow(p0.x-t.x, 2) + pow(p0.y-t.y, 2)) - r);
        }
        void find_closest(VPci p_begin, VPci p_end, Closest_point& cpoint);
};

void Circle::find_closest(VPci p_begin, VPci p_end, Closest_point& cpoint) {
    for(; p_begin != p_end; p_begin++)
        cpoint.test_and_set(*p_begin, circumference_distance(*p_begin));
}



void f(Circle& c, VPci p_begin, VPci p_end, Closest_point& cpoint) {
    c.find_closest(p_begin, p_end, cpoint);
}

const int brTacaka = 1000;
const int brNiti = 3;

int main()
{
    srand(time(NULL));
    Point centarKruga;
    centarKruga.x = 0;
    centarKruga.y = 0;
    double radius = 3;
    double random_x;
    double random_y;
    Circle krug(centarKruga, radius);
    vector<Point> vektorTacaka;
    for(int i = 0; i < brTacaka; i++) {
        random_x = rand()%(20*(int)radius) + radius/2;
        random_y = rand()%(20*(int)radius) + radius/2;
        vektorTacaka.push_back({random_x, random_y});
    }
    Closest_point cpoint;
    f(krug, vektorTacaka.begin(), vektorTacaka.end(), ref(cpoint));
    cout << cpoint;

    Closest_point cpoint1;
    thread niti[brNiti];
    VPci pocetak = vektorTacaka.begin();
    VPci kraj = pocetak + brTacaka/brNiti;
    for(int i = 0; i < brNiti - 1; i++) {
        niti = thread(f, krug, pocetak, kraj, ref(cpoint1));
        pocetak = kraj;
        kraj += brTacaka/brNiti;
    }
    niti[brNiti] = thread(f,krug, kraj, vektorTacaka.end(), ref(cpoint));
    for(int i = 0; i < brNiti; i++)
        niti[i].join();
    cout << cpoint1;
    return 0;
}
