#ifndef JGTL_RANGE_H_INCLUDED
#define JGTL_RANGE_H_INCLUDED

#include <iostream>
#include <algorithm>
#include <cmath>

namespace JGTL
{
    template<class T>
    class RangeSet;

    template<class T>
    class Range
    {
    public:
        friend class RangeSet<T>;
        T low,high;
        bool nullRange;

        Range()
                :
                low(0),
                high(0),
                nullRange(false)
        {}

        Range(const T &a,const T &b,bool forceNullRange=false)
        {
            low = min(a,b);
            high = max(a,b);
            nullRange = forceNullRange;
        }

        Range<T> operator -(T delta)
        {
            return Range<T>(low-delta,high-delta,nullRange);
        }

        void set(const T &a,const T &b)
        {
            low = min(a,b);
            high = max(a,b);
            nullRange = false;
        }

        T getLow() const
        {
            return low;
        }

        T getHigh() const
        {
            return high;
        }

        bool isOverlappingMultipoint(const Range<T> &other) const
        {
            if(high<=other.getLow())
                return false;
            else if(low>=other.getHigh())
                return false;
            else
                return true;
        }

        bool isOverlapping(const Range<T> &other) const
        {
            if(high<other.getLow())
                return false;
            else if(low>other.getHigh())
                return false;
            else
                return true;
        }

        inline bool isNull() const
        {
            return nullRange;
        }

        T getOverlapSize(const Range<T> &other) const
        {
            if(!isOverlapping(other))
            {
                return 0;
            }
            else
            {
                return min(high,other.high)-max(low,other.low);
            }
        }

        T getProjectionDistance(const Range<T> &other) const
        {
            //Two ways to move.  Either the high == other.low or low == other.high.
            //Pick the smallest

            if(!isOverlapping(other))
            {
                return 0;
            }
            else
            {
                if(fabs(high-other.low)<fabs(low-other.high))
                {
                    return other.low-high;
                }
                else
                {
                    return other.high-low;
                }
            }
        }

        Range<T> getOverlap(const Range<T> &other) const
        {
            if(!isOverlapping(other))
            {
                return Range<T>(0,0,true);
            }
            else
            {
                Range<T> newRange;

                newRange.low = max(low,other.low);
                newRange.high = min(high,other.high);

                return newRange;
            }
        }

        inline bool containsPoint(T pt)
        {
            return low<=pt && high>=pt;
        }

        void display()
        {
		std::cout << '[' << low << "," << high << "]";
        }

    };

    template<class T>
    inline std::ostream& operator<< ( std::ostream& stream, const Range<T>& r )
    {
	    stream << r.low << ' ' << r.high;

	    return stream;
    }

    template<class T>
    inline std::istream& operator>> ( std::istream& stream, Range<T>& r )
    {
	    stream >> r.low >> r.high;

	    return stream;
    }
}

#endif
