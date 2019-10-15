/*
    solid.cpp

    Contains common code for base class SolidObject.
*/

#include "imager.h"

namespace Imager
{
    bool SolidObject::Contains(const Vector& point) const
    {

        if (isFullyEnclosed)
        {

            const Vector direction(0.0, 0.0, 1.0);

            enclosureList.clear();
            AppendAllIntersections(point, direction, enclosureList);

            int enterCount = 0;     
            int exitCount  = 0;     

            IntersectionList::const_iterator iter = enclosureList.begin();
            IntersectionList::const_iterator end  = enclosureList.end();
            for (; iter != end; ++iter)
            {
                const Intersection& intersection = *iter;
 
                const double dotprod = DotProduct(
                    direction, 
                    intersection.surfaceNormal);
  
                if (dotprod > EPSILON)
                {
                    ++exitCount;
                }
                else if (dotprod < -EPSILON)
                {
                    ++enterCount;
                }
                else
                {

                    throw ImagerException("Ambiguous transition.");
                }
            }


            switch (exitCount - enterCount)
            {
            case 0:
                return false;   

            case 1:
                return true;    

            default:

                throw ImagerException("Cannot determine containment.");
            }
        }
        else
        {

            return false;
        }
    }
}
