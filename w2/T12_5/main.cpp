/*Napraviti konkurentni program koji pravi srpsko engleski recnik iz
englesko srpskog recnika. Recnik ima proizvoljan broj reci (bitno je
da bude vise od jedne). Posao treba obaviti u jednoj niti. Ispisati
englesko srpski i srpsko engleski recnik na kraju programa.
Napomena: ispis prevedenih reci ne mora biti u redosledu
unesenih reci prilikom formiranja recnika, ali prevod mora biti
tacan.*/

#include <iostream>
#include <thread>
#include <map>

using namespace std;

void f(map<string,string> &es, map<string,string> &se) {
    map<string,string>::iterator it = es.begin();
    for( ; it != es.end(); it++) {
        se[it->second] = it->first;
        }
}

int main()
{
    string srpski[] = {"crveno", "zuto", "crno", "belo", "plavo"};
    string engleski[] = {"red", "yellow", "black", "white", "blue"};
    map<string,string> engleskoSrpski;
    map<string,string> srpskoEngleski;

    for(int i = 0; i < 5; i++)
        engleskoSrpski[engleski[i]] = srpski[i];

    thread nit(f,ref(engleskoSrpski),ref(srpskoEngleski));
    nit.join();

    cout << "englesko-srpski " << endl;
    map<string,string>::const_iterator it;
    for(it = engleskoSrpski.begin(); it != engleskoSrpski.end(); it++)
        cout << it->first << " " << it->second << endl;

    cout << "srpsko-engleski " << endl;
    for(it = srpskoEngleski.begin(); it != srpskoEngleski.end(); it++)
        cout << it->first << " " << it->second << endl;
    return 0;
}
