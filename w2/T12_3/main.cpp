/*Napraviti konkurentni program koji stvara jednu nit. Nit ima 2
parametra. Jedan je referenca na ulaznu listu a drugi referenca na
izlaznu. Nit treba da elemente ulazne liste prebaci u izlaznu tako da
stoje u obrnutom redosledu. Ispisati izgled izlazne liste nakon rada
niti.*/

#include <iostream>
#include <thread>
#include <list>

using namespace std;

typedef list<double>::const_iterator ci;

void f(list<double> &listaIN, list<double> &listaOUT) {
    ci a_begin = listaIN.begin();
    ci a_end = listaIN.end();
    for( ; a_begin != a_end; a_begin++)
        listaOUT.push_front(*a_begin);

}
int main()
{
    list<double> a = {1,2,3,4};
    list<double> b;
    thread nit(f,ref(a),ref(b));
    nit.join();

    for(ci it = b.begin(); it != b.end(); it++)
        cout << *it << endl;
    return 0;
}
