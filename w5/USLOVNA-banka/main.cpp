// Napraviti konkurentni program koji modeluje kreditno poslovanje banke.
// Banka odobrava kredite u dinarima i u evrima.

// Klijent trazi kredit pozivanjem operacije uzmi_kredit(),
// kojoj prosledjuje svotu koju zeli da pozajmi od banke i valutu u kojoj zeli da pozajmi.
// Klijent neko vreme koristi pozajmljena sredstva, pa ih vrati banci
// pozivanjem operacije vrati_kredit().

// Banka inicijalno poseduje odredjene svote dinara i evra
// na dva razlicita racuna, koje pozajmljuje.
// Banka odobrava kredite dok ima sredstava.
// Kada vise nema sredstava, banka ceka da klijenti vrate
// pretodno odobrene kredite pre nego sto odobri sledeci kredit.
// Banka odobrava kredite u proizvoljnom redosledu.

// Banka tezi tome da klijent ciji je zahtev moguce ispunitini
// (postoje sredstva) ne ceka na kredit.

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>

using namespace std;
using namespace chrono;

enum Valuta {dinar, euro};

mutex term_m;

class Banka{
    private:
        double eRacun, dinRacun;
        mutex m;
        condition_variable vratiE, vratiDin;
    public:
        Banka(double a, double b) {
            eRacun = a;
            dinRacun = b;
        }
        double getStanje() {return eRacun + dinRacun;}
        double uzmiKredit(double svota, Valuta val) {
            unique_lock<mutex> lock(m);
            if(val == dinar) {
                while(dinRacun < svota)
                    vratiDin.wait(lock);
                dinRacun -= svota;
                return dinRacun;
            }
            if(val == euro) {
                while(eRacun < svota)
                    vratiE.wait(lock);
                eRacun -= svota;
                return eRacun;
            }
        }
        void vratiKredit(double svota, Valuta val) {
            unique_lock<mutex> lock(m);
            if(val == dinar) {
                dinRacun += svota;
                vratiDin.notify_all();
            }
            if(val == euro) {
                eRacun += svota;
                vratiE.notify_all();
            }
        }
};

string vratiValutu(Valuta val) {
    if(val == dinar)
        return "dinar";
    else
        return "euro";
}

void klijent(Banka &b, double svota, Valuta val, int rbr) {
    unique_lock<mutex> lock(term_m);
    cout << "Klijent " << rbr << " trazi kredit od " << svota << vratiValutu(val) << endl;
    lock.unlock();
    b.uzmiKredit(svota, val);
    lock.lock();
    cout << "Klijent " << rbr << " je uzeo kredit " << endl;
    lock.unlock();
    this_thread::sleep_for(seconds(1));
    b.vratiKredit(svota, val);
    lock.lock();
    cout << "Klijent " << rbr << " je vratio kredit " << endl;


}

const int DIN_KLIJENATA = 10;
const int EVR_KLIJENATA = 10;

int main()
{
    Banka b(30,30);
    thread t[DIN_KLIJENATA];
   thread t1[EVR_KLIJENATA];

   vector<int> d_kolicine = {20, 5, 7, 13, 14, 4, 30, 6, 9};
   vector<int> e_kolicine = {10, 10, 5, 20, 6, 7, 15, 3, 12};

   for(int i = 0; i < DIN_KLIJENATA; i++)
      t[i] = thread(klijent, ref(b), d_kolicine[i], dinar, i);
   for(int i = 0; i < EVR_KLIJENATA; i++)
      t1[i] = thread(klijent, ref(b), e_kolicine[i], euro,
                     DIN_KLIJENATA + i);
   for(int i = 0; i < DIN_KLIJENATA; i++)
      t[i].join();
   for(int i = 0; i < EVR_KLIJENATA; i++)
      t1[i].join();
    cout << b.getStanje();
    return 0;
}
