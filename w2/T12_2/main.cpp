/*Napraviti konkurentni program koji pita korisnika koliko niti da
stvori, a zatim stvara zadati broj niti. Pri instanciranju nit dobija
redni broj pod kojim je stvorena. Svaka nit ispisuje svoj redni broj.
*/

#include <iostream>
#include <thread>

using namespace std;

void f(int i) {
    cout << "moj redni broj je " << i << endl;
}

int main()
{
    cout << "unesite koliko niti zelite da stvorite" << endl;
    int br_niti;
    cin >> br_niti;

    thread *niz_niti = new thread[br_niti];

    for(int i = 0; i < br_niti; i++)
        niz_niti[i] = thread(f,i);

    for(int i = 0; i < br_niti; i++)
        niz_niti[i].join();

    delete[] niz_niti;
    return 0;
}
