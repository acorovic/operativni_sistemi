/*
Definisati funkciju: void increment(int& a);
koja poveÄ‡ava (za jedan) vrednost argumenta.

Napraviti program koji:
a) poziva funkciju increment()
b) stvara nit od funkcije increment()
*/

#include <thread>
#include <iostream>

using namespace std;

void increment(int &a) {
    ++a;
}

int main() {
    int a = 0;
    int b = 0;

    increment(a);

    thread nit(increment,ref(b));
    nit.join();

    cout << "a=" << a << endl;
    cout << "b=" << b << endl;

}
