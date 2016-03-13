/*Napraviti konkurentni program koji pronalazi element najblizi 0 iz
zadatog niza brojeva. Posao podeliti tako da ga izvrsavaju 3 niti.
Duzina niza brojeva treba da je deljiva sa 3.*/

#include <iostream>
#include <thread>
#include <cmath>
#define BR_NITI 3

using namespace std;

void f(int *pocetak, int n, int &najmanji) {
    najmanji = *pocetak;
    for(int i = 1; i < n; i++) {
        if(abs(*(pocetak + i)) < abs(najmanji) )
            najmanji = *(pocetak + i);
    }
    cout << najmanji << endl;
}

int main()
{
    int niz[] = {-2,4,5,7,6,8,9,11,-15};
    thread niz_niti[BR_NITI];
    int minimalni[BR_NITI];
    int n = (sizeof(niz)/sizeof(int)) / BR_NITI;
    for(int i = 0; i < BR_NITI; i++)
        niz_niti[i] = thread(f,niz + i*n, n, ref(minimalni[i]));

    for(int i = 0; i < BR_NITI; i++)
        niz_niti[i].join();

    int najbliz;
    thread najblizi(f,minimalni,BR_NITI,ref(najbliz));
    najblizi.join();
    cout << "najblizi 0 je " << najbliz << endl;
    return 0;
}
