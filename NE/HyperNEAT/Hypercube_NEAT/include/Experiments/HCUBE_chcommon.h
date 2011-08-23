#ifndef HCUBE_CHCOMMON_H_INCLUDED
#define HCUBE_CHCOMMON_H_INCLUDED

// types
class coor
{
public:
    int x;   // file
    int y;   // row

    coor(int _x,int _y)
            :
            x(_x),
            y(_y)
    {}

    coor()
            :
            x(0),
            y(0)
    {}
};

namespace HCUBE
{

    typedef struct
    {
        int jumps;   // how many jumps are there in this move?
        int newpiece;   // what type of piece appears on to
        int oldpiece;   // what disappears on from
        coor from, to;  // coordinates of the squares in 8x8 notation!
        coor path[12];  // intermediate path coordinates
        coor del[12];   // squares whose pieces are deleted
        int delpiece[12];  // what is on these squares
    }
    CBmove;

    /*----------> structure definitions  */
    struct move2
    {
        short n;
        int m[12];
    };
}

#endif // HCUBE_CHCOMMON_H_INCLUDED
