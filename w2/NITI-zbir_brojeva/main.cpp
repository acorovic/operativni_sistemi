/* Napraviti program koji kreira jednu nit kojoj se prosledjuju
dva cela broja a i b. U okviru niti sabrati brojeve i ispisati
njihov zbir*/

#include <iostream>
#include <thread>

using namespace std;

void f(int a, int b) {
    cout << "zbir je " << a+b << endl;
}

int main()
{
    int a = 5, b = 7;
    thread nit(f,a,b);
    nit.join();
    return 0;
}
