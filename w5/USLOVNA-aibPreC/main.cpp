/*
	Napraviti konkurentni program sa tri niti.
	Nit a ispisuje: Ja sam nit a.
	Nit b ispisuje: Ja sam nit b.
	Nit c ispisuje: Ja sam nit c.

	Obezbediti da se niti a i b, uvek izvrse pre niti c.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class Koordinator{
    private:
        mutex m;
        condition_variable ispisanoAiB;
        int ispisano;
    public:
        Koordinator() : ispisano(0) {}
        void ispis() {
            unique_lock<mutex> lock(m);
            ispisano++;
            if(ispisano == 2)
                ispisanoAiB.notify_one();
        }
        void cekajIspis() {
            unique_lock<mutex> lock(m);
            while(ispisano != 2)
                ispisanoAiB.wait(lock);
        }
};

mutex terminal;
Koordinator k;

void nitA() {
    {
    unique_lock<mutex> lockT(terminal);
    cout << "ja sam nit A" << endl;}
    k.ispis();
}

void nitB() {
    {
    unique_lock<mutex> lockT(terminal);
    cout << "ja sam nit B" << endl;}
    k.ispis();
}

void nitC() {
    k.cekajIspis();
    unique_lock<mutex> lockT(terminal);
    cout << "ja sam nit C" << endl;
}

int main()
{
    thread b(nitB);
    thread c(nitC);
    thread a(nitA);
    c.join();
    b.join();
    a.join();
    return 0;
}
