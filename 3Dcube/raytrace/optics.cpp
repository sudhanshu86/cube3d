/*
    optics.cpp

    Member functions for class Optics, which describes the optical properties
    of a point on the surface of a solid object.
*/

#include <cmath>
#include "imager.h"

namespace Imager
{
    void Optics::ValidateReflectionColor(const Color& color) const
    {

        if (color.red < 0.0 || color.red > 1.0)
        {
            throw ImagerException("Invalid red color component.");
        }

        if (color.green < 0.0 || color.green > 1.0)
        {
            throw ImagerException("Invalid green color component.");
        }

        if (color.blue < 0.0 || color.blue > 1.0)
        {
            throw ImagerException("Invalid blue color component.");
        }
    }

    void Optics::SetMatteColor(const Color& _matteColor)
    {
        ValidateReflectionColor(_matteColor);
        matteColor = _matteColor;
    }

    void Optics::SetGlossColor(const Color& _glossColor)
    {
        ValidateReflectionColor(_glossColor);
        glossColor = _glossColor;
    }

    void Optics::SetMatteGlossBalance(
        double glossFactor,
        const Color& rawMatteColor,
        const Color& rawGlossColor)
    {

        ValidateReflectionColor(rawMatteColor);
        ValidateReflectionColor(rawGlossColor);

        // glossFactor must be in the range 0..1.
        if (glossFactor < 0.0 || glossFactor > 1.0)
        {
            throw ImagerException("Gloss factor must be in the range 0..1");
        }
        
        SetMatteColor((1.0 - glossFactor) * rawMatteColor);
        SetGlossColor(glossFactor * rawGlossColor);
    }

    void Optics::SetOpacity(double _opacity)
    {
        if (_opacity < 0.0 || _opacity > 1.0)
        {
            throw ImagerException("Invalid opacity.");
        }
        opacity = _opacity;
    }
}
