//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvAV/AV.h>

#include <memory>

namespace djv
{
    namespace AV
    {
        namespace Geom
        {
            struct TriangleMesh;

            //! This class provides the interface for shapes.
            class IShape
            {
            public:
                virtual ~IShape() = 0;

                virtual void triangulate(TriangleMesh &) const = 0;
            };

            //! This class provides a square shape.
            class Square : public IShape
            {
            public:
                explicit Square(float radius = .5F);

                float getRadius() const { return _radius; }
                void setRadius(float);

                void triangulate(TriangleMesh &) const override;

            private:
                float _radius = .5F;
            };

            //! This class provides a circle shape.
            class Circle : public IShape
            {
            public:
                Circle(float radius = .5F, size_t resolution = 10);

                float getRadius() const { return _radius; }
                size_t getResolution() const { return _resolution; }
                void setRadius(float);
                void setResolution(size_t);

                void triangulate(TriangleMesh &) const override;

            private:
                float _radius = .5F;
                size_t _resolution = 10;
            };

            //! This class provides a cube shape.
            class Cube : public IShape
            {
            public:
                explicit Cube(float radius = .5F);

                float getRadius() const { return _radius; }
                void setRadius(float);

                void triangulate(TriangleMesh &) const override;

            private:
                float _radius = .5F;
            };

            //! This class provides a sphere shape.
            class Sphere : public IShape
            {
            public:
                typedef std::pair<size_t, size_t> Resolution;
                typedef std::pair<float, float> Span;

                Sphere(float radius = .5F, const Resolution & = Resolution(10, 10));

                float getRadius() const { return _radius; }
                const Resolution & getResolution() const { return _resolution; }
                const Span & getUSpan() const { return _uSpan; }
                const Span & getVSpan() const { return _vSpan; }
                bool hasTextureSpan() const { return _textureSpan; }
                void setRadius(float);
                void setResolution(const Resolution &);
                void setUSpan(const Span &);
                void setVSpan(const Span &);
                void setTextureSpan(bool);

                void triangulate(TriangleMesh &) const override;

            private:
                float _radius = .5F;
                Resolution _resolution;
                Span _uSpan = Span(0.F, 1.F);
                Span _vSpan = Span(0.F, 1.F);
                bool _textureSpan = true;
            };

            //! This class provides a cylinder shape.
            class Cylinder : public IShape
            {
            public:
                typedef std::pair<float, float> Span;

                Cylinder(float radius = .5F, float length = 1.F, size_t resolution = 100);

                float getRadius() const { return _radius; }
                float getLength() const { return _length; }
                bool isCapped() const { return _capped; }
                size_t getResolution() const { return _resolution; }
                const Span & getSpan() const { return _span; }
                bool hasTextureSpan() const { return _textureSpan; }
                void setRadius(float);
                void setLength(float);
                void setCapped(bool);
                void setResolution(size_t);
                void setSpan(const Span &);
                void setTextureSpan(bool);

                void triangulate(TriangleMesh &) const override;

            private:
                float _radius = .5F;
                float _length = 1.F;
                bool _capped = false;
                size_t _resolution = 10;
                Span _span = Span(0.F, 1.F);
                bool _textureSpan = true;
            };

        } // namespace Geom
    } // namespace AV
} // namespace djv

