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

#include <djvAV/Pixel.h>

#include <djvCore/ISystem.h>
#include <djvCore/UID.h>
#include <djvCore/Vector.h>

#include <future>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;

        } // namespace FileSystem
    } // namespace Core

    namespace AV
    {
        namespace IO
        {
            class Info;

        } // namespace IO

        namespace Image
        {
            class Size;
            class Info;
            class Convert;
            class Image;
            
        } // namespace Image
            
        //! This class provides a thumbnail error.
        class ThumbnailError : public std::runtime_error
        {
        public:
            explicit ThumbnailError(const std::string&);
        };
        
        //! This class provides a system for generating thumbnail images from files.
        class ThumbnailSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(ThumbnailSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ThumbnailSystem();

        public:
            virtual ~ThumbnailSystem();

            //! Create a new thumbnail system.
            //! Throws:
            //! - ThumbnailError
            static std::shared_ptr<ThumbnailSystem> create(const std::shared_ptr<Core::Context>&);

            //! This structure provides information about a file.
            struct InfoFuture
            {
                InfoFuture();
                InfoFuture(std::future<IO::Info> &, Core::UID);
                std::future<IO::Info> future;
                Core::UID uid = 0;
            };
            
            //! Get information about a file.
            InfoFuture getInfo(const Core::FileSystem::FileInfo&);

            //! Cancel information about a file.
            void cancelInfo(Core::UID);

            //! This structure provides a thumbnail image for a file.
            struct ImageFuture
            {
                ImageFuture();
                ImageFuture(std::future<std::shared_ptr<Image::Image> > &, Core::UID);
                std::future<std::shared_ptr<Image::Image> > future;
                Core::UID uid = 0;
            };

            //! Get a thumbnail image for the given file.
            ImageFuture getImage(
                const Core::FileSystem::FileInfo& path,
                const Image::Size&                size,
                Image::Type                       type = Image::Type::None);

            //! Cancel a thumbnail image.
            void cancelImage(Core::UID);

            //! Get the infromation cache percentage used.
            float getInfoCachePercentage() const;

            //! Get the image cache percentage used.
            float getImageCachePercentage() const;

            //! Clear the cache.
            void clearCache();

        private:
            void _handleInfoRequests();
            void _handleImageRequests(const std::shared_ptr<Image::Convert> &);

            DJV_PRIVATE();
        };

    } // namespace AV
} // namespace djv
