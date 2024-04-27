#include <iostream>
#include <string>
#include "fwd_list.h"
using namespace std;

struct Point {
    int x = 0;
    int y = 0;
};

ostream& operator << (ostream& os, const Point& p) {
    os << "("s << p.x << ", "s << p.y << ")"s;
    return os;
}

int main() {

    FwdList<Point> mylist = { {1, 2}, {3, 4}, {5, 6}, {7, 8} };
    cout << mylist << endl;

    mylist.pop_front();
    mylist.pop_back();
    cout << mylist << endl;

    mylist.push_front({ -1, -2 });
    mylist.push_back({ -7, -8 });
    cout << mylist << endl;

    const auto it = mylist.insert_after(++mylist.begin(), Point({ 10, 20 }));
    cout << mylist << endl;

    mylist.erase_after(it);
    cout << mylist << endl;

	return 0;
}