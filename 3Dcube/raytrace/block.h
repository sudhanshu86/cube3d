/*
    block.h

*/

#ifndef __DDC_BLOCK_H
#define __DDC_BLOCK_H

#include "imager.h"

namespace Imager
{

    class ConcreteBlock: public SetDifference
    {
    public:
        ConcreteBlock(const Vector& _center, const Optics& _optics)
            : SetDifference(Vector(), CreateLargeCuboid(_optics), CreateSmallCuboidUnion(_optics))
        {
            Move(_center);
        }

    private:
        static SolidObject* CreateLargeCuboid(const Optics& _optics)
        {
            Cuboid* cuboid = new Cuboid(8.0, 16.0, 8.0);
            cuboid->SetUniformOptics(_optics);
            return cuboid;
        }

        static SolidObject* CreateSmallCuboidUnion(const Optics& _optics)
        {

            Cuboid *topCuboid    = new Cuboid(6.0, 6.5, 8.01);     
            Cuboid *bottomCuboid = new Cuboid(6.0, 6.5, 8.01);

            topCuboid->SetUniformOptics(_optics);
            bottomCuboid->SetUniformOptics(_optics);

            topCuboid->Move   (0.0, +7.5, 0.0);
            bottomCuboid->Move(0.0, -7.5, 0.0);

            return new SetUnion(Vector(), topCuboid, bottomCuboid);
        }
    };
}

#endif 
