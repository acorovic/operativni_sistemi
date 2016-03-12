/*Napraviti konkrentni program koji izraÄunava
sumu svih elemenata vektora, koristeÄ‡i funkciju f():

f(ci begin, ci end, double& zbir) pri cemu je
ci typedef vector<double>::const_iterator

Podeliti racunanje sume na 2 dela tako da prvu polovinu vektora
sumira prva nit, a drugu polovinu druga nit.

Napomena: ovakva optimizacija sumiranja je znacajna kada se radi
na dvojezgarnom procesoru za vektore velike duzine.
*/

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

typedef vector<double>::const_iterator ci;

void f(ci a_begin, ci a_end, double &zbir) {
    for( ; a_begin != a_end; a_begin++)
        zbir += *a_begin;
}
int main()
{
    vector<double> a = {1,2,3,4,5,6,7};
    double zbir = 0;
    thread t1(f, a.begin(), a.begin() + a.size()/2, ref(zbir));
    thread t2(f, a.begin() + a.size()/2, a.end(), ref(zbir));
    t1.join();
    t2.join();
    cout << "zbir je " << zbir << endl;
    return 0;
}
