#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

vector<int> min_n(const vector<int>& v,	int n) {
    vector<int> temp = v;
    sort(temp.begin(), temp.end());

    vector<int> pov;
    for(int i = 0; i < n; i++)
        pov.push_back(temp[i]);

    return pov;
}
int main()
{
    int niz[] = {188,12,1,6,22,14};
    vector<int> a(niz, niz + sizeof(niz)/sizeof(int));

    vector<int> b = min_n(a,3);
    for(auto it : b)
        cout<<it<<endl;
    return 0;
}
