/*
Napraviti konkurentni program koji u funkciji visina pita korisnika koliko je visok.
Nakon toga korisnik unosi svoju visinu. Na kraju u funkciji se ispisuje uneta visina.

SpreÄiti Å¡tetno preplitanje na terminalu koriÅ¡Ä‡enjem klase mutex.

Kreirati 2 niti od date funkcije. Ispratiti ispis.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex term_m;

void f() {
    unique_lock<mutex> lock(term_m);
    cout << "koliko ste visoki" << endl;
    int n;
    cin >> n;
    cout << "visoki ste " << n << endl;
}

int main()
{
    thread nit1(f), nit2(f);
    nit1.join();
    nit2.join();
    return 0;
}
