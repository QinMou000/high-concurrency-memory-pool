#include "ObjectPool.h"

int main()
{
    TestObjectPool();
    cout << SizeClass::RoundUp(2) << endl;
    cout << SizeClass::RoundUp(5) << endl;
    cout << SizeClass::RoundUp(9) << endl;
    cout << SizeClass::RoundUp(129) << endl;
    cout << SizeClass::RoundUp(1025) << endl;


    cout << SizeClass::Index(2) << endl;
    cout << SizeClass::Index(5) << endl;
    cout << SizeClass::Index(9) << endl;
    cout << SizeClass::Index(129) << endl;
    cout << SizeClass::Index(1025) << endl;

    return 0;
}