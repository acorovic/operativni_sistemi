/*
Napraviti konkurentni program koji u funkciji visina pita korisnika koliko je visok.
Nakon toga korisnik unosi svoju visinu. Na kraju u funkciji se ispisuje uneta visina.

Kreirati 2 niti od date funkcije. Ispratiti ispis.
*/

#include <iostream>
#include <thread>

using namespace std;

void f() {
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
