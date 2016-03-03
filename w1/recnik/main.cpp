#include <iostream>
#include <map>
#include <string>
using namespace std;

int main()
{
    map<string,string> recnik;
    recnik["white"] = "belo";
    recnik["black"] = "crno";
    recnik["gold"] = "zlatno";

    cout<<"unesite rec za prevod"<<endl;

    string unos;
    cin>>unos;
        if(recnik[unos] !=  "")
            cout<< "prevod je "<<recnik[unos]<<endl;
        else
            cout<< "nema prevoda" <<endl;

    return 0;
}
