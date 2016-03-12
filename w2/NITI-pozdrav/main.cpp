/* Napraviti program koji kreira jednu nit i
u okviru niti ispisuje proizvoljnu recenicu*/



#include <iostream>
#include <thread>
#include <string>
using namespace std;

void f() {
    cout << "pozdrav iz niti" << endl;
}
int main()
{
    thread nit(f);
    nit.join();
    return 0;
}
