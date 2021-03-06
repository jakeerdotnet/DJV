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

#include <djvAV/PNG.h>

#include <djvCore/FileIO.h>
#include <djvCore/FileSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PNG
            {
                class Read::File
                {
                public:
                    File()
                    {
                        memset(&pngError, 0, sizeof(ErrorStruct));
                        png = png_create_read_struct(
                            PNG_LIBPNG_VER_STRING,
                            &pngError,
                            djvPngError,
                            djvPngWarning);
                    }

                    File(File&& other) noexcept :
                        f(other.f),
                        png(other.png),
                        pngInfo(other.pngInfo),
                        pngInfoEnd(other.pngInfoEnd),
                        pngError(std::move(other.pngError))
                    {}

                    ~File()
                    {
                        if (f)
                        {
                            fclose(f);
                            f = nullptr;
                        }
                        if (png || pngInfo || pngInfoEnd)
                        {
                            png_destroy_read_struct(
                                png        ? &png        : nullptr,
                                pngInfo    ? &pngInfo    : nullptr,
                                pngInfoEnd ? &pngInfoEnd : nullptr);
                            png        = nullptr;
                            pngInfo    = nullptr;
                            pngInfoEnd = nullptr;
                        }
                    }

                    File& operator = (File&& other) noexcept
                    {
                        if (this != &other)
                        {
                            f = other.f;
                            png = other.png;
                            pngInfo = other.pngInfo;
                            pngInfoEnd = other.pngInfoEnd;
                            pngError = std::move(other.pngError);
                        }
                        return *this;
                    }

                    bool open(const std::string& fileName)
                    {
                        f = FileSystem::fopen(fileName.c_str(), "rb");
                        return f;
                    }

                    FILE *      f          = nullptr;
                    png_structp png        = nullptr;
                    png_infop   pngInfo    = nullptr;
                    png_infop   pngInfoEnd = nullptr;
                    ErrorStruct pngError;
                };

                Read::Read()
                {}

                Read::~Read()
                {
                    _finish();
                }

                std::shared_ptr<Read> Read::create(
                    const FileSystem::FileInfo& fileInfo,
                    const ReadOptions& readOptions,
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, readOptions, resourceSystem, logSystem);
                    return out;
                }

                namespace
                {
                    bool pngOpen(
                        FILE*       f,
                        png_structp png,
                        png_infop *  pngInfo,
                        png_infop *  pngInfoEnd)
                    {
                        if (setjmp(png_jmpbuf(png)))
                        {
                            return false;
                        }
                        *pngInfo = png_create_info_struct(png);
                        if (!*pngInfo)
                        {
                            return false;
                        }
                        *pngInfoEnd = png_create_info_struct(png);
                        if (!*pngInfoEnd)
                        {
                            return false;
                        }
                        uint8_t tmp[8];
                        if (fread(tmp, 8, 1, f) != 1)
                        {
                            return false;
                        }
                        if (png_sig_cmp(tmp, 0, 8))
                        {
                            return false;
                        }
                        png_init_io(png, f);
                        png_set_sig_bytes(png, 8);
                        png_read_info(png, *pngInfo);
                        if (png_get_interlace_type(png, *pngInfo) != PNG_INTERLACE_NONE)
                        {
                            return false;
                        }
                        png_set_expand(png);
                        //png_set_gray_1_2_4_to_8(png);
                        png_set_palette_to_rgb(png);
                        png_set_tRNS_to_alpha(png);
                        return true;
                    }

                } // namespace

                Info Read::_readInfo(const std::string & fileName)
                {
                    File f;
                    return _open(fileName, f);
                }

                namespace
                {
                    bool pngScanline(png_structp png, uint8_t * out)
                    {
                        if (setjmp(png_jmpbuf(png)))
                        {
                            return false;
                        }
                        png_read_row(png, out, 0);
                        return true;
                    }

                    bool pngEnd(png_structp png, png_infop pngInfo)
                    {
                        if (setjmp(png_jmpbuf(png)))
                        {
                            return false;
                        }
                        png_read_end(png, pngInfo);
                        return true;
                    }

                } // namespace

                std::shared_ptr<Image::Image> Read::_readImage(const std::string & fileName)
                {
                    std::shared_ptr<Image::Image> out;
                    File f;
                    const auto info = _open(fileName, f);
                    out = Image::Image::create(info.video[0].info);

                    out->setPluginName(pluginName);
                    for (uint16_t y = 0; y < info.video[0].info.size.h; ++y)
                    {
                        if (!pngScanline(f.png, out->getData(y)))
                        {
                            throw FileSystem::Error(f.pngError.msg);
                        }
                    }
                    pngEnd(f.png, f.pngInfoEnd);
                    return out;
                }

                Info Read::_open(const std::string & fileName, File & f)
                {
                    if (!f.png)
                    {
                        throw FileSystem::Error(f.pngError.msg);
                    }
                    if (!f.open(fileName))
                    {
                        throw FileSystem::Error(DJV_TEXT("The file cannot be opened."));
                    }
                    if (!pngOpen(f.f, f.png, &f.pngInfo, &f.pngInfoEnd))
                    {
                        throw FileSystem::Error(f.pngError.msg);
                    }

                    int channels = png_get_channels(f.png, f.pngInfo);
                    if (png_get_color_type(f.png, f.pngInfo) == PNG_COLOR_TYPE_PALETTE)
                    {
                        channels = 3;
                    }
                    if (png_get_valid(f.png, f.pngInfo, PNG_INFO_tRNS))
                    {
                        ++channels;
                    }
                    int bitDepth = png_get_bit_depth(f.png, f.pngInfo);
                    if (bitDepth < 8)
                    {
                        bitDepth = 8;
                    }
                    Image::Type imageType = Image::getIntType(channels, bitDepth);
                    if (Image::Type::None == imageType)
                    {
                        throw FileSystem::Error("Unsupported image type.");
                    }
                    auto info = Image::Info(png_get_image_width(f.png, f.pngInfo), png_get_image_height(f.png, f.pngInfo), imageType);

                    if (bitDepth >= 16 && Memory::Endian::LSB == Memory::getEndian())
                    {
                        png_set_swap(f.png);
                    }

                    return Info(fileName, VideoInfo(info, _speed, _sequence));
                }

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

