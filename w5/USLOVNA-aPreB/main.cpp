/*
	Napraviti konkurentni program sa dve niti.
	Nit a ispisuje: Ja sam nit a.
	Nit b ispisuje: Ja sam nit b.
	Obezbediti da se uvek izvrsi prvo nit a.
*/

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace std;

class Koordinator{
    private:
        mutex m;
        bool ispisanoA;
        condition_variable ispisanA;
    public:
        Koordinator() {
            ispisanoA = false;
        }
        void prviZavrsio() {
            unique_lock<mutex> lock(m);
            ispisanoA = true;
            ispisanA.notify_one();
        }
        void drugiCeka() {
            unique_lock<mutex> lock(m);
            while(!ispisanoA)
                ispisanA.wait(lock);
        }
};

Koordinator k;

void nitA() {
    cout << "ja sam nit a" << endl;
    k.prviZavrsio();
}

void nitB() {
    k.drugiCeka();
    cout << "ja sam nit b" << endl;
}

int main()
{
    thread b(nitB);
    thread a(nitA);
    a.join();
    b.join();
    return 0;
}
