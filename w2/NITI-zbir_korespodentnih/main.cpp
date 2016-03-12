/*
Napraviti konkurentni program  (koristeÄ‡i funkciju f())
koji sabira korespodentne elemente kontejnera  a i b,
a rezultat smeÅ¡ta u vektor zbir.

typedef vector<double>::const_iterator ci;
void f(ci a_begin, ci a_end, ci b_begin, vector<double>::iterator sum_begin);

Program optimizovati za procesor sa 2 jezgra.
Napomene:
Podrazumeva se da su kontejneri a i b iste duÅ¾ine.
Sabiranje korespodentnih elemenata je sabiranje matrica (vektora)
*/

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

typedef vector<double>::const_iterator ci;

void f(ci a_begin, ci a_end, ci b_begin, vector<double>::iterator sum_begin) {
    for( ; a_begin != a_end; a_begin++, b_begin++, sum_begin++)
        *sum_begin = *a_begin + *b_begin;
}

int main()
{
    vector<double> a = {1,2,3,4,5,6,7};
    vector<double> b = {1,2,3,4,5,6,7};
    vector<double> sum(a.size());

    thread t1(f, a.begin(), a.begin() + a.size()/2, b.begin(), sum.begin());
    thread t2(f, a.begin() + a.size()/2, a.end(), b.begin() + b.size()/2, sum.begin() + sum.size()/2);
    t1.join();
    t2.join();

    for(ci it = sum.begin(); it != sum.end(); it++)
        cout << *it << endl;



    return 0;
}
