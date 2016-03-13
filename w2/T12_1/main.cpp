/*Sabrati korespodentne elemente vektora a i b, a zbirove smestiti na
odgovarajuce pozicije vektora c. Obezbediti da svako sabiranje
obavlja posebna nit. */

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

typedef vector<double>::const_iterator ci;

void f(double a, double b, double &c) {
    c = a + b;
    cout << c << endl;
}

int main()
{
    vector<double> a = {1,2,3,4,5,6,7};
    vector<double> b = {1,2,3,4,5,6,7};
    vector<double> c(a.size());

    thread niz_niti[a.size()];

    for(int i = 0; i < a.size(); i++)
        niz_niti[i] = thread(f, a[i], b[i], ref(c[i]));

    double zbir = 0;

    for(int i = 0; i < a.size(); i++)
        niz_niti[i].join();

    for(int i = 0; i < c.size(); i++)
        zbir += c[i];

    cout << "zbir je " << zbir << endl;

    return 0;
}
