/*Napraviti konkurentni program koji stvara nit iz koje:
pita korisnika za visinu u centimetrima i
ispisuje: â€œVasa visina je <uneta_visina> cm.â€

Testirati program tako sto ce se nit prevesti iz stanja
joinable prvo operacijom join a nakon toga detach.
*/


#include <iostream>
#include <thread>
using namespace std;

void f() {
    cout << "unesite vasu visinu u cm" << endl;
    double visina;
    cin >> visina;
    cout << "vasa visina je " << visina << " cm " << endl;
}

int main()
{
    thread nit(f);
    nit.join();
    //nit.detach();
    return 0;
}
