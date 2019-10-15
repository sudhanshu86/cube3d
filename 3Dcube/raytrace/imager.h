/*
    imager.h
    
*/

#ifndef __DDC_IMAGER_H
#define __DDC_IMAGER_H

#include <vector>
#include <cmath>
#include "algebra.h"

namespace Imager
{
    const double PI = 3.141592653589793238462643383279502884;

    const double EPSILON = 1.0e-6;      

    inline double RadiansFromDegrees(double degrees)
    {
        return degrees * (PI / 180.0);
    }

    class SolidObject;
    class ImageBuffer;

    class ImagerException
    {
    public:
        explicit ImagerException(const char *_message)
            : message(_message)
        {
        }

        const char *GetMessage() const { return message; }

    private:
        const char * const message;
    };

    class AmbiguousIntersectionException
    {
    };


    class Vector
    {
    public:
        double x;
        double y;
        double z;

        Vector()
            : x(0.0)
            , y(0.0)
            , z(0.0)
        {
        }

        Vector(double _x, double _y, double _z)
            : x(_x)
            , y(_y)
            , z(_z)
        {
        }

        const double MagnitudeSquared() const
        {
            return (x*x) + (y*y) + (z*z);
        }

        const double Magnitude() const
        {
            return sqrt(MagnitudeSquared());
        }

        const Vector UnitVector() const
        {
            const double mag = Magnitude();
            return Vector(x/mag, y/mag, z/mag);
        }

        Vector& operator *= (const double factor)
        {
            x *= factor;
            y *= factor;
            z *= factor;
            return *this;
        }

        Vector& operator += (const Vector& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
    };


    inline Vector operator + (const Vector &a, const Vector &b)
    {
        return Vector(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    inline Vector operator - (const Vector &a, const Vector &b)
    {
        return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    inline Vector operator - (const Vector& a)
    {
        return Vector(-a.x, -a.y, -a.z);
    }

    inline double DotProduct (const Vector& a, const Vector& b) 
    {
        return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
    }

    inline Vector CrossProduct (const Vector& a, const Vector& b)
    {
        return Vector(
            (a.y * b.z) - (a.z * b.y), 
            (a.z * b.x) - (a.x * b.z), 
            (a.x * b.y) - (a.y * b.x));
    }

    inline Vector operator * (double s, const Vector& v)
    {
        return Vector(s*v.x, s*v.y, s*v.z);
    }

    inline Vector operator / (const Vector& v, double s)
    {
        return Vector(v.x/s, v.y/s, v.z/s);
    }

    struct Color
    {
        double  red;
        double  green;
        double  blue;

        Color(double _red, double _green, double _blue, double _luminosity = 1.0)
            : red  (_luminosity * _red)
            , green(_luminosity * _green)
            , blue (_luminosity * _blue)
        {
        }

        Color()
            : red(0.0)
            , green(0.0)
            , blue(0.0)
        {
        }

        Color& operator += (const Color& other)
        {
            red   += other.red;
            green += other.green;
            blue  += other.blue;
            return *this;
        }

        Color& operator *= (const Color& other)
        {
            red   *= other.red;
            green *= other.green;
            blue  *= other.blue;
            return *this;
        }

        Color& operator *= (double factor)
        {
            red   *= factor;
            green *= factor;
            blue  *= factor;
            return *this;
        }

        Color& operator /= (double denom)
        {
            red   /= denom;
            green /= denom;
            blue  /= denom;
            return *this;
        }

        void Validate() const
        {
            if ((red < 0.0) || (green < 0.0) || (blue < 0.0))
            {
                throw ImagerException("Negative color values not allowed.");
            }
        }
    };

    inline Color operator * (const Color& aColor, const Color& bColor)
    {
        return Color(
            aColor.red   * bColor.red,
            aColor.green * bColor.green,
            aColor.blue  * bColor.blue);
    }

    inline Color operator * (double scalar, const Color &color)
    {
        return Color(
            scalar * color.red, 
            scalar * color.green, 
            scalar * color.blue);
    }

    inline Color operator + (const Color& a, const Color& b)
    {
        return Color(
            a.red   + b.red,
            a.green + b.green,
            a.blue  + b.blue);
    }

    const double REFRACTION_VACUUM   = 1.0000;
    const double REFRACTION_GLASS    = 1.5500;

    const double REFRACTION_MINIMUM  = 1.0000;
    const double REFRACTION_MAXIMUM  = 9.0000;

    inline void ValidateRefraction(double refraction)
    {
        if (refraction < REFRACTION_MINIMUM || 
            refraction > REFRACTION_MAXIMUM)
        {
            throw ImagerException("Invalid refractive index.");
        }
    }

    class Optics
    {
    public:
        Optics()
            : matteColor(Color(1.0, 1.0, 1.0))
            , glossColor(Color(0.0, 0.0, 0.0))
            , opacity(1.0)
        {
        }

        explicit Optics(
            Color _matteColor, 
            Color _glossColor  = Color(0.0, 0.0, 0.0),
            double _opacity    = 1.0)
        {
            SetMatteColor(_matteColor);
            SetGlossColor(_glossColor);
            SetOpacity(_opacity);
        }

        void SetMatteGlossBalance(
            double glossFactor,     // 0..1: balance between matte and gloss
            const Color& rawMatteColor,
            const Color& rawGlossColor);

        void SetMatteColor(const Color& _matteColor);
        void SetGlossColor(const Color& _glossColor);
        void SetOpacity(double _opacity);

        const Color& GetMatteColor() const { return matteColor; }
        const Color& GetGlossColor() const { return glossColor; }
        const double GetOpacity()    const { return opacity;    }

    protected:
        void ValidateReflectionColor(const Color& color) const;

    private:
        Color   matteColor;     // color, intensity of scattered reflection
        Color   glossColor;     // color, intensity of mirror reflection
        double  opacity;        // fraction 0..1 of reflected light
    };

    struct Intersection
    {

        double distanceSquared;

        Vector point;

        Vector surfaceNormal;

        const SolidObject* solid;

        const void* context;

        const char* tag;

        Intersection()
            : distanceSquared(1.0e+20)  
            , point()
            , surfaceNormal()
            , solid(NULL)
            , context(NULL)
            , tag(NULL)
        {
        }
    };

    typedef std::vector<Intersection> IntersectionList;

    int PickClosestIntersection(
        const IntersectionList& list, 
        Intersection& intersection);



    class Taggable       
    {
    public:
        Taggable(std::string _tag = "")
            : tag(_tag)
        {
        }

        void SetTag(std::string _tag)
        {
            tag = _tag;
        }

        std::string GetTag() const
        {
            return tag;
        }

    private:
        std::string tag;
    };



    class SolidObject: public Taggable
    {
    public:
        SolidObject(const Vector& _center = Vector(), bool _isFullyEnclosed = true)
            : center(_center)
            , refractiveIndex(REFRACTION_GLASS)
            , isFullyEnclosed(_isFullyEnclosed)
        {
        }

        virtual ~SolidObject()
        {
        }

        virtual void AppendAllIntersections(
            const Vector& vantage, 
            const Vector& direction, 
            IntersectionList& intersectionList) const = 0;

        int FindClosestIntersection(
            const Vector& vantage, 
            const Vector& direction, 
            Intersection &intersection) const
        {
            cachedIntersectionList.clear();
            AppendAllIntersections(vantage, direction, cachedIntersectionList);
            return PickClosestIntersection(cachedIntersectionList, intersection);
        }

        virtual bool Contains(const Vector& point) const;

        virtual Optics SurfaceOptics(
            const Vector& surfacePoint,
            const void *context) const
        {
            return uniformOptics;
        }

        double GetRefractiveIndex() const
        {
            return refractiveIndex;
        }

        virtual SolidObject& RotateX(double angleInDegrees) = 0;
        virtual SolidObject& RotateY(double angleInDegrees) = 0;
        virtual SolidObject& RotateZ(double angleInDegrees) = 0;

        virtual SolidObject& Translate(double dx, double dy, double dz)
        {
            center.x += dx;
            center.y += dy;
            center.z += dz;
            return *this;
        }

        SolidObject& Move(double cx, double cy, double cz)
        {
            Translate(cx - center.x, cy - center.y, cz - center.z);
            return *this;
        }

        SolidObject& Move(const Vector& newCenter)
        {
            Move(newCenter.x, newCenter.y, newCenter.z);
            return *this;
        }

        const Vector& Center() const { return center; }

        void SetUniformOptics(const Optics& optics)
        {
            uniformOptics = optics;
        }

        void SetMatteGlossBalance(
            double glossFactor,
            const Color& rawMatteColor,
            const Color& rawGlossColor)
        {
            uniformOptics.SetMatteGlossBalance(
                glossFactor, 
                rawMatteColor,
                rawGlossColor);
        }

        void SetFullMatte(const Color& matteColor)
        {
            uniformOptics.SetMatteGlossBalance(
                0.0,        // glossFactor=0 indicates full matte reflection
                matteColor,
                Color(0.0, 0.0, 0.0));  
        }

        void SetOpacity(const double opacity)
        {
            uniformOptics.SetOpacity(opacity);
        }

        void SetRefraction(const double refraction)
        {
            ValidateRefraction(refraction);
            refractiveIndex = refraction;
        }

    protected:
        const Optics& GetUniformOptics() const
        {
            return uniformOptics;
        }

    private:
        Vector center;  
        Optics uniformOptics;

        double refractiveIndex;

        const bool isFullyEnclosed;

        mutable IntersectionList cachedIntersectionList;
        mutable IntersectionList enclosureList;
    };


    class SolidObject_BinaryOperator: public SolidObject
    {
    public:

        SolidObject_BinaryOperator(
            const Vector& _center, 
            SolidObject* _left, 
            SolidObject* _right)
                : SolidObject(_center)
                , left(_left)
                , right(_right)
        {
        }

        virtual ~SolidObject_BinaryOperator()
        {
            delete left;
            left = NULL;

            delete right;
            right = NULL;
        }


        virtual SolidObject& RotateX(double angleInDegrees);
        virtual SolidObject& RotateY(double angleInDegrees);
        virtual SolidObject& RotateZ(double angleInDegrees);

        virtual SolidObject& Translate(double dx, double dy, double dz);

    protected:
        SolidObject& Left()  const { return *left;  }
        SolidObject& Right() const { return *right; }

        void NestedRotateX(
            SolidObject &nested, 
            double angleInDegrees, 
            double a, 
            double b);

        void NestedRotateY(
            SolidObject &nested, 
            double angleInDegrees, 
            double a, 
            double b);

        void NestedRotateZ(
            SolidObject &nested, 
            double angleInDegrees, 
            double a, 
            double b);

        mutable IntersectionList tempIntersectionList;

    private:
        SolidObject* left;
        SolidObject* right;
    };


    class SetUnion: public SolidObject_BinaryOperator
    {
    public:
        SetUnion(const Vector& _center, SolidObject* _left, SolidObject* _right)
            : SolidObject_BinaryOperator(_center, _left, _right)
        {
            SetTag("SetUnion");
        }

        virtual void AppendAllIntersections(
            const Vector& vantage, 
            const Vector& direction, 
            IntersectionList& intersectionList) const;

        virtual bool Contains(const Vector& point) const
        {

            return Left().Contains(point) || Right().Contains(point);
        }
    };


    class SetIntersection: public SolidObject_BinaryOperator
    {
    public:
        SetIntersection(
            const Vector& _center, 
            SolidObject* _left, 
            SolidObject* _right)
                : SolidObject_BinaryOperator(_center, _left, _right)
        {
            SetTag("SetIntersection");
        }

        virtual void AppendAllIntersections(
            const Vector& vantage, 
            const Vector& direction, 
            IntersectionList& intersectionList) const;

        virtual bool Contains(const Vector& point) const
        {

            return Left().Contains(point) && Right().Contains(point);
        }

    private:
        void AppendOverlappingIntersections(
            const Vector& vantage,
            const Vector& direction,
            const SolidObject& aSolid, 
            const SolidObject& bSolid, 
            IntersectionList& intersectionList) const;

        bool HasOverlappingIntersection(
            const Vector& vantage,
            const Vector& direction,
            const SolidObject& aSolid,
            const SolidObject& bSolid) const;
    };

    class SetComplement: public SolidObject
    {
    public:
        explicit SetComplement(SolidObject* _other)
            : SolidObject(_other->Center())
            , other(_other)
        {
            SetTag("SetComplement");
        }

        virtual ~SetComplement()
        {
            delete other;
            other = NULL;
        }

        virtual bool Contains(const Vector& point) const
        {

            return !other->Contains(point);
        }

        virtual void AppendAllIntersections(
            const Vector& vantage, 
            const Vector& direction, 
            IntersectionList& intersectionList) const;

        virtual SolidObject& Translate(double dx, double dy, double dz)
        {
            SolidObject::Translate(dx, dy, dz);
            other->Translate(dx, dy, dz);
            return *this;
        }

        virtual SolidObject& RotateX(double angleInDegrees)
        {
            other->RotateX(angleInDegrees);
            return *this;
        }

        virtual SolidObject& RotateY(double angleInDegrees)
        {
            other->RotateY(angleInDegrees);
            return *this;
        }

        virtual SolidObject& RotateZ(double angleInDegrees)
        {
            other->RotateZ(angleInDegrees);
            return *this;
        }

    private:
        SolidObject* other;
    };

    class SetDifference: public SetIntersection
    {
    public:
        SetDifference(
            const Vector& _center, 
            SolidObject* _left, 
            SolidObject* _right)
                : SetIntersection(_center, _left, new SetComplement(_right))
        {
            SetTag("SetDifference");
        }
    };


    class SolidObject_Reorientable: public SolidObject
    {
    public:
        explicit SolidObject_Reorientable(const Vector& _center = Vector())
            : SolidObject(_center)
            , rDir(1.0, 0.0, 0.0)
            , sDir(0.0, 1.0, 0.0)
            , tDir(0.0, 0.0, 1.0)
            , xDir(1.0, 0.0, 0.0)
            , yDir(0.0, 1.0, 0.0)
            , zDir(0.0, 0.0, 1.0)
        {
        }

        virtual void AppendAllIntersections(
            const Vector& vantage, 
            const Vector& direction, 
            IntersectionList& intersectionList) const;

        virtual SolidObject& RotateX(double angleInDegrees);
        virtual SolidObject& RotateY(double angleInDegrees);
        virtual SolidObject& RotateZ(double angleInDegrees);

        virtual bool Contains(const Vector& point) const
        {
            return ObjectSpace_Contains(ObjectPointFromCameraPoint(point));
        }

        virtual Optics SurfaceOptics(
            const Vector& surfacePoint,
            const void *context) const
        {
            return ObjectSpace_SurfaceOptics(
                ObjectPointFromCameraPoint(surfacePoint),
                context);
        }

    protected:

        virtual void ObjectSpace_AppendAllIntersections(
            const Vector& vantage, 
            const Vector& direction, 
            IntersectionList& intersectionList) const = 0;

        virtual bool ObjectSpace_Contains(const Vector& point) const = 0;

        virtual Optics ObjectSpace_SurfaceOptics(
            const Vector& surfacePoint,
            const void *context) const
        {
            return GetUniformOptics();
        }

        Vector ObjectDirFromCameraDir(const Vector& cameraDir) const
        {
            return Vector(
                DotProduct(cameraDir,rDir), 
                DotProduct(cameraDir,sDir), 
                DotProduct(cameraDir,tDir));
        }

        Vector ObjectPointFromCameraPoint(const Vector &cameraPoint) const
        {
            return ObjectDirFromCameraDir(cameraPoint - Center());
        }

        Vector CameraDirFromObjectDir(const Vector& objectDir) const
        {
            return Vector(
                DotProduct(objectDir,xDir), 
                DotProduct(objectDir,yDir), 
                DotProduct(objectDir,zDir));
        }

        Vector CameraPointFromObjectPoint(const Vector& objectPoint) const
        {
            return Center() + CameraDirFromObjectDir(objectPoint);
        }

        void UpdateInverseRotation()
        {


            xDir = Vector(rDir.x, sDir.x, tDir.x);
            yDir = Vector(rDir.y, sDir.y, tDir.y);
            zDir = Vector(rDir.z, sDir.z, tDir.z);
        }

    private:

        Vector  rDir;
        Vector  sDir;
        Vector  tDir;

        Vector  xDir;
        Vector  yDir;
        Vector  zDir;
    };


    class Cuboid: public SolidObject_Reorientable
    {
    public:
        Cuboid(double _a, double _b, double _c)
            : SolidObject_Reorientable()
            , a(_a)
            , b(_b)
            , c(_c)
        {
            SetTag("Cuboid");
        }

    protected:
        virtual void ObjectSpace_AppendAllIntersections(
            const Vector& vantage, 
            const Vector& direction, 
            IntersectionList& intersectionList) const;

        virtual bool ObjectSpace_Contains(const Vector& point) const
        {
            return 
                (fabs(point.x) <= a + EPSILON) &&
                (fabs(point.y) <= b + EPSILON) &&
                (fabs(point.z) <= c + EPSILON);
        }

    private:
        const double  a;   // half of the width:  faces at r = -a and r = +a.
        const double  b;   // half of the length: faces at s = -b and s = +b.
        const double  c;   // half of the height: faces at t = -c and t = +c.
    };

    struct LightSource: public Taggable
    {
        LightSource(const Vector& _location, const Color& _color, std::string _tag = "")
            : Taggable(_tag)
            , location(_location)
            , color(_color)
        {
        }

        Vector  location;
        Color   color;
    };


    class Scene
    {
    public:
        explicit Scene(const Color& _backgroundColor = Color())
            : backgroundColor(_backgroundColor)
            , ambientRefraction(REFRACTION_VACUUM)
            , activeDebugPoint(NULL)
        {
        }

        virtual ~Scene()
        {
            ClearSolidObjectList();
        }

        SolidObject& AddSolidObject(SolidObject* solidObject)
        {
            solidObjectList.push_back(solidObject);
            return *solidObject;
        }

        void AddLightSource(const LightSource &lightSource)
        {
            lightSourceList.push_back(lightSource);
        }


        void SaveImage(
            const char *outPngFileName, 
            size_t pixelsWide, 
            size_t pixelsHigh, 
            double zoom, 
            size_t antiAliasFactor) const;


        void SetAmbientRefraction(double refraction)
        {
            ValidateRefraction(refraction);
            ambientRefraction = refraction;
        }

        void AddDebugPoint(int iPixel, int jPixel)
        {
            debugPointList.push_back(DebugPoint(iPixel, jPixel));
        }

    private:
        void ClearSolidObjectList();

        int FindClosestIntersection(
            const Vector& vantage, 
            const Vector& direction, 
            Intersection& intersection) const;

        bool HasClearLineOfSight(
            const Vector& point1, 
            const Vector& point2) const;

        Color TraceRay(
            const Vector& vantage,
            const Vector& direction,
            double refractiveIndex,
            Color rayIntensity,
            int recursionDepth) const;

        Color CalculateLighting(
            const Intersection& intersection, 
            const Vector& direction, 
            double refractiveIndex,
            Color rayIntensity,
            int recursionDepth) const;

        Color CalculateMatte(const Intersection& intersection) const;

        Color CalculateReflection(
            const Intersection& intersection, 
            const Vector& incidentDir, 
            double refractiveIndex,
            Color rayIntensity,
            int recursionDepth) const;

        Color CalculateRefraction(
            const Intersection& intersection, 
            const Vector& direction, 
            double sourceRefractiveIndex,
            Color rayIntensity,
            int recursionDepth,
            double& outReflectionFactor) const;

        const SolidObject* PrimaryContainer(const Vector& point) const;

        double PolarizedReflection(
            double n1,              
            double n2,              
            double cos_a1,          
            double cos_a2) const;   

        void ResolveAmbiguousPixel(ImageBuffer& buffer, size_t i, size_t j) const;

        static unsigned char ConvertPixelValue(
            double colorComponent, 
            double maxColorValue)
        {
            int pixelValue = 
                static_cast<int> (255.0 * colorComponent / maxColorValue);

            if (pixelValue < 0)
            {
                pixelValue = 0;
            }
            else if (pixelValue > 255)
            {
                pixelValue = 255;
            }

            return static_cast<unsigned char>(pixelValue);
        }


        Color backgroundColor;                  


        typedef std::vector<SolidObject*> SolidObjectList;
        typedef std::vector<LightSource> LightSourceList;

        struct PixelCoordinates
        {
            size_t i;
            size_t j;

            PixelCoordinates(size_t _i, size_t _j)
                : i(_i)
                , j(_j)
            {
            }
        };
        typedef std::vector<PixelCoordinates> PixelList;

        SolidObjectList solidObjectList;

        LightSourceList lightSourceList;

        double ambientRefraction;

        mutable IntersectionList cachedIntersectionList;

        struct DebugPoint
        {
            int     iPixel;
            int     jPixel;

            DebugPoint(int _iPixel, int _jPixel)
                : iPixel(_iPixel)
                , jPixel(_jPixel)
            {
            }
        };
        typedef std::vector<DebugPoint> DebugPointList;
        DebugPointList debugPointList;
        mutable const DebugPoint* activeDebugPoint;
    };

    struct PixelData
    {
        Color   color;
        bool    isAmbiguous;

        PixelData()
            : color()
            , isAmbiguous(false)
        {
        }
    };


    class ImageBuffer
    {
    public:
        ImageBuffer (
            size_t _pixelsWide, 
            size_t _pixelsHigh, 
            const Color &backgroundColor)
                : pixelsWide(_pixelsWide)
                , pixelsHigh(_pixelsHigh)
                , numPixels(_pixelsWide * _pixelsHigh)
        {
            array = new PixelData[numPixels];
        }

        virtual ~ImageBuffer()
        {
            delete[] array;
            array = NULL;
            pixelsWide = pixelsHigh = numPixels = 0;
        }


        PixelData& Pixel(size_t i, size_t j) const
        {
            if ((i < pixelsWide) && (j < pixelsHigh))
            {
                return array[(j * pixelsWide) + i];
            }
            else
            {
                throw ImagerException("Pixel coordinate(s) out of bounds");
            }
        }

        size_t GetPixelsWide() const
        {
            return pixelsWide;
        }

        size_t GetPixelsHigh() const
        {
            return pixelsHigh;
        }


        double MaxColorValue() const
        {
            double max = 0.0;
            for (size_t i=0; i < numPixels; ++i) 
            {
                array[i].color.Validate();
                if (array[i].color.red > max)
                {
                    max = array[i].color.red;
                }
                if (array[i].color.green > max)
                {
                    max = array[i].color.green;
                }
                if (array[i].color.blue > max)
                {
                    max = array[i].color.blue;
                }
            }
            if (max == 0.0)
            {

                max = 1.0;
            }
            return max;
        }

    private:        
        size_t  pixelsWide;     
        size_t  pixelsHigh;     
        size_t  numPixels;      
        PixelData*  array;      
    };

    std::ostream& operator<< (std::ostream&, const Color&);
    std::ostream& operator<< (std::ostream&, const Vector&);
    std::ostream& operator<< (std::ostream&, const Intersection&);
    void Indent(std::ostream&, int depth);
}

#endif 
