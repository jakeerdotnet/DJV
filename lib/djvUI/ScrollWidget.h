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

#include <djvUI/Margin.h>
#include <djvUI/Widget.h>

#include <djvCore/Range.h>
#include <djvCore/Timer.h>

namespace djv
{
    namespace UI
    {
        //! This enumeration provides the scroll directions.
        enum class ScrollType
        {
            Both,
            Horizontal,
            Vertical,

            Count,
            First = Both
        };
        DJV_ENUM_HELPERS(ScrollType);

        //! This class provides a scroll widget.
        //!
        //! \todo Add a minimum scroll bar handle size.
        class ScrollWidget : public Widget
        {
            DJV_NON_COPYABLE(ScrollWidget);

        protected:
            void _init(ScrollType, const std::shared_ptr<Core::Context>&);
            ScrollWidget();

        public:
            virtual ~ScrollWidget();

            static std::shared_ptr<ScrollWidget> create(ScrollType, const std::shared_ptr<Core::Context>&);

            ScrollType getScrollType() const;
            void setScrollType(ScrollType);

            const glm::vec2 & getScrollPos() const;
            void setScrollPos(const glm::vec2 &);
            void moveToBegin();
            void moveToEnd();
            void movePageUp();
            void movePageDown();
            void moveUp();
            void moveDown();
            void moveLeft();
            void moveRight();

            bool hasAutoHideScrollBars() const;
            void setAutoHideScrollBars(bool);

            void setBorder(bool);

            MetricsRole getMinimumSizeRole() const;
            MetricsRole getScrollBarSizeRole() const;
            void setMinimumSizeRole(MetricsRole);
            void setScrollBarSizeRole(MetricsRole);

            void addChild(const std::shared_ptr<IObject> &) override;
            void removeChild(const std::shared_ptr<IObject> &) override;
            void clearChildren() override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;
            void _clipEvent(Core::Event::Clip &) override;
            void _keyPressEvent(Core::Event::KeyPress&) override;
            void _scrollEvent(Core::Event::Scroll &) override;

            bool _eventFilter(const std::shared_ptr<IObject> &, Core::Event::Event &) override;

        private:
            void _updateScrollBars(const glm::vec2 &);
            void _addPointerSample(const glm::vec2 &);
            glm::vec2 _getPointerAverage() const;

            DJV_PRIVATE();
        };

    } // namespace UI
    
    DJV_ENUM_SERIALIZE_HELPERS(UI::ScrollType);
    DJV_ENUM_SERIALIZE_HELPERS(UI::ScrollType);

} // namespace djv
