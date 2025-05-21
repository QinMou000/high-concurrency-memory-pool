#include "ObjectPool.h"

int main()
{
    TestObjectPool();
    cout << SizeClass::RoundUp(2) << endl;
    cout << SizeClass::RoundUp(5) << endl;
    cout << SizeClass::RoundUp(9) << endl;
    cout << SizeClass::RoundUp(129) << endl;
    cout << SizeClass::RoundUp(1025) << endl;

    return 0;
}