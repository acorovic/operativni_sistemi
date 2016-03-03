#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main()
{
    cout<<"unesite linije"<<endl;
    vector<string> linije;
    string linija;
    while(getline(cin,linija))
        linije.push_back(linija);
    cout<<endl<<"ispis"<<endl;
    for(auto it : linije)
        cout<<it<<endl;
    cout<<"ispis naopako"<<endl;
    //vector<string>::reverse_iterator it;
    for(auto it = linije.rbegin(); it != linije.rend(); it++)
        cout<<*it<<endl;

    return 0;
}
