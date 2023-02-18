#include <vector>

class Potato{
public:
    int hops;
    char trace[513];
    int count;
    Potato(int h):hops(h),count(0){}
    Potato():hops(0),count(0){}
};