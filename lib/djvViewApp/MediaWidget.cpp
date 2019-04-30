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

#include <djvViewApp/MediaWidget.h>

#include <djvViewApp/ImageView.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/TimelineSlider.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/GridLayout.h>
#include <djvUI/Label.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/AVSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MediaWidget::Private
        {
            std::shared_ptr<Media> media;
            Time::Speed speed;
            Time::Timestamp duration = 0;
            Time::Timestamp currentTime = 0;
            AV::TimeUnits timeUnits = AV::TimeUnits::First;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> playbackActionGroup;
            std::shared_ptr<ImageView> imageView;
            //std::shared_ptr<UI::FloatEdit> speedEdit;
            std::shared_ptr<UI::ToolButton> speedButton;
            std::shared_ptr<UI::Label> currentTimeLabel;
            std::shared_ptr<UI::Label> durationLabel;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::PopupWidget> audioPopupWidget;
            std::shared_ptr<UI::BasicFloatSlider> volumeSlider;
            std::shared_ptr<UI::ToolButton> muteButton;
            std::shared_ptr<UI::StackLayout> layout;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > speedObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > durationObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver2;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<float> > volumeObserver;
            std::shared_ptr<ValueObserver<bool> > muteObserver;
            std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
        };
        
        void MediaWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MediaWidget");

            p.actions["Forward"] = UI::Action::create();
            p.actions["Forward"]->setIcon("djvIconPlaybackForward");
            p.actions["Reverse"] = UI::Action::create();
            p.actions["Reverse"]->setIcon("djvIconPlaybackReverse");
            p.playbackActionGroup = UI::ActionGroup::create(UI::ButtonType::Exclusive);
            p.playbackActionGroup->addAction(p.actions["Forward"]);
            p.playbackActionGroup->addAction(p.actions["Reverse"]);

            p.actions["InPoint"] = UI::Action::create();
            p.actions["InPoint"]->setIcon("djvIconFrameStart");
            p.actions["PrevFrame"] = UI::Action::create();
            p.actions["PrevFrame"]->setIcon("djvIconFramePrev");
            p.actions["NextFrame"] = UI::Action::create();
            p.actions["NextFrame"]->setIcon("djvIconFrameNext");
            p.actions["OutPoint"] = UI::Action::create();
            p.actions["OutPoint"]->setIcon("djvIconFrameEnd");

            p.imageView = ImageView::create(context);

            //p.speedEdit = UI::FloatEdit::create(context);
            //p.speedEdit->setRange(FloatRange(0.f, 1000.f));
            p.speedButton = UI::ToolButton::create(context);
            p.speedButton->setFontSizeRole(UI::MetricsRole::FontSmall);

            p.currentTimeLabel = UI::Label::create(context);
            p.currentTimeLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.currentTimeLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.durationLabel = UI::Label::create(context);
            p.durationLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.durationLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.timelineSlider = TimelineSlider::create(context);

            p.volumeSlider = UI::BasicFloatSlider::create(UI::Orientation::Horizontal, context);
            p.volumeSlider->setMargin(UI::MetricsRole::MarginSmall);
            p.muteButton = UI::ToolButton::create(context);
            p.muteButton->setIcon("djvIconClose");
            p.muteButton->setButtonType(UI::ButtonType::Toggle);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.volumeSlider);
            hLayout->addChild(p.muteButton);
            p.audioPopupWidget = UI::PopupWidget::create(context);
            p.audioPopupWidget->setIcon("djvIconAudio");
            p.audioPopupWidget->addChild(hLayout);

            auto toolbar = UI::ToolBar::create(context);
            toolbar->addAction(p.actions["Reverse"]);
            toolbar->addAction(p.actions["Forward"]);
            toolbar->addAction(p.actions["InPoint"]);
            toolbar->addAction(p.actions["PrevFrame"]);
            toolbar->addAction(p.actions["NextFrame"]);
            toolbar->addAction(p.actions["OutPoint"]);

            auto playbackLayout = UI::GridLayout::create(context);
            playbackLayout->setSpacing(UI::MetricsRole::None);
            playbackLayout->setBackgroundRole(UI::ColorRole::Overlay);
            playbackLayout->addChild(toolbar);
            playbackLayout->setGridPos(toolbar, 0, 0);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->addChild(p.timelineSlider);
            hLayout->setStretch(p.timelineSlider, UI::RowStretch::Expand);
            hLayout->addChild(p.audioPopupWidget);
            playbackLayout->addChild(hLayout);
            playbackLayout->setGridPos(hLayout, 1, 0);
            playbackLayout->setStretch(hLayout, UI::GridStretch::Horizontal);
            playbackLayout->addChild(p.speedButton);
            playbackLayout->setGridPos(p.speedButton, 0, 1);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->addChild(p.currentTimeLabel);
            hLayout->addExpander();
            hLayout->addChild(p.durationLabel);
            playbackLayout->addChild(hLayout);
            playbackLayout->setGridPos(hLayout, 1, 1);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addExpander();
            vLayout->addChild(playbackLayout);

            p.layout = UI::StackLayout::create(context);
            p.layout->addChild(p.imageView);
            p.layout->addChild(vLayout);
            addChild(p.layout);

            _widgetUpdate();
            _speedUpdate();

            auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
            p.playbackActionGroup->setExclusiveCallback(
                [weak](int index)
            {
                if (auto widget = weak.lock())
                {
                    if (auto media = widget->_p->media)
                    {
                        Playback playback = Playback::Stop;
                        switch (index)
                        {
                        case 0: playback = Playback::Forward; break;
                        case 1: playback = Playback::Reverse; break;
                        }
                        media->setPlayback(playback);
                    }
                }
            });

            /*p.speedEdit->setValueCallback(
                [weak](float value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->media)
                    {
                        widget->_p->media->setSpeed(Time::Speed::floatToSpeed(value));
                    }
                }
            });*/

            p.volumeSlider->setValueCallback(
                [weak](float value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->media)
                    {
                        widget->_p->media->setVolume(value);
                    }
                }
            });

            p.muteButton->setCheckedCallback(
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->media)
                    {
                        widget->_p->media->setMute(value);
                    }
                }
            });

            p.currentTimeObserver = ValueObserver<Time::Timestamp>::create(
                p.timelineSlider->observeCurrentTime(),
                [weak](Time::Timestamp value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->media)
                    {
                        widget->_p->media->setCurrentTime(value);
                    }
                }
            });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = ValueObserver<AV::TimeUnits>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::TimeUnits value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->timeUnits = value;
                    widget->_widgetUpdate();
                }
            });
        }

        MediaWidget::MediaWidget() :
            _p(new Private)
        {}

        MediaWidget::~MediaWidget()
        {}

        std::shared_ptr<MediaWidget> MediaWidget::create(Context * context)
        {
            auto out = std::shared_ptr<MediaWidget>(new MediaWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<Media> & MediaWidget::getMedia() const
        {
            return _p->media;
        }

        void MediaWidget::setMedia(const std::shared_ptr<Media> & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.media)
                return;
            p.media = value;
            p.imageView->setMedia(value);
            _widgetUpdate();
            _speedUpdate();
            if (p.media)
            {
                auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
                p.infoObserver = ValueObserver<AV::IO::Info>::create(
                    p.media->observeInfo(),
                    [weak](const AV::IO::Info & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value.video.size() ? value.video[0].speed : Time::Speed();
                        widget->_widgetUpdate();
                    }
                });
                p.speedObserver = ValueObserver<Time::Speed>::create(
                    p.media->observeSpeed(),
                    [weak](const Time::Speed& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value;
                        widget->_widgetUpdate();
                        widget->_speedUpdate();
                    }
                });
                p.durationObserver = ValueObserver<Time::Timestamp>::create(
                    p.media->observeDuration(),
                    [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->duration = value;
                        widget->_widgetUpdate();
                    }
                });
                p.currentTimeObserver2 = ValueObserver<Time::Timestamp>::create(
                    p.media->observeCurrentTime(),
                    [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentTime = value;
                        widget->_widgetUpdate();
                    }
                });
                p.playbackObserver = ValueObserver<Playback>::create(
                    p.media->observePlayback(),
                    [weak](Playback value)
                {
                    if (auto widget = weak.lock())
                    {
                        int index = -1;
                        switch (value)
                        {
                        case Playback::Forward: index = 0; break;
                        case Playback::Reverse: index = 1; break;
                        default: break;
                        }
                        widget->_p->playbackActionGroup->setChecked(index);
                    }
                });
                p.volumeObserver = ValueObserver<float>::create(
                    p.media->observeVolume(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->volumeSlider->setValue(value);
                    }
                });
                p.muteObserver = ValueObserver<bool>::create(
                    p.media->observeMute(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->muteButton->setChecked(value);
                    }
                });
            }
            else
            {
                p.speed = Time::Speed();
                p.duration = 0;
                p.currentTime = 0;
                p.infoObserver.reset();
                p.speedObserver.reset();
                p.durationObserver.reset();
                p.currentTimeObserver2.reset();
                p.playbackObserver.reset();
                p.volumeObserver.reset();
                p.muteObserver.reset();
                _widgetUpdate();
                _speedUpdate();
            }
            p.timelineSlider->setMedia(p.media);
        }

        void MediaWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MediaWidget::_layoutEvent(Event::Layout &)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void MediaWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Forward"]->setEnabled(p.media.get());
            p.actions["Reverse"]->setEnabled(p.media.get());
            p.actions["InPoint"]->setEnabled(p.media.get());
            p.actions["PrevFrame"]->setEnabled(p.media.get());
            p.actions["NextFrame"]->setEnabled(p.media.get());
            p.actions["OutPoint"]->setEnabled(p.media.get());

            auto playback = Playback::Stop;
            if (p.media)
            {
                playback = p.media->observePlayback()->get();
            }
            switch (playback)
            {
            case Playback::Stop:    p.playbackActionGroup->setChecked(-1); break;
            case Playback::Forward: p.playbackActionGroup->setChecked( 0); break;
            case Playback::Reverse: p.playbackActionGroup->setChecked( 1); break;
            default: break;
            }

            auto avSystem = getContext()->getSystemT<AV::AVSystem>();
            p.currentTimeLabel->setText(avSystem->getLabel(p.currentTime, p.speed));
            p.currentTimeLabel->setEnabled(p.media.get());
            p.durationLabel->setText(avSystem->getLabel(p.duration, p.speed));
            p.durationLabel->setEnabled(p.media.get());

            p.timelineSlider->setEnabled(p.media.get());

            p.audioPopupWidget->setEnabled(p.media.get());
        }

        void MediaWidget::_speedUpdate()
        {
            DJV_PRIVATE_PTR();
            //p.speedEdit->setValue(Time::Speed::speedToFloat(p.speed));
            {
                std::stringstream ss;
                ss.precision(2);
                ss << DJV_TEXT("FPS") << ": " << std::fixed << Time::Speed::speedToFloat(p.speed);
                p.speedButton->setText(ss.str());
            }
        }

        void MediaWidget::_localeEvent(Event::Locale&)
        {
            DJV_PRIVATE_PTR();
            p.actions["Forward"]->setTooltip(_getText(DJV_TEXT("Forward tooltip")));
            p.actions["Reverse"]->setTooltip(_getText(DJV_TEXT("Reverse tooltip")));
            p.actions["InPoint"]->setTooltip(_getText(DJV_TEXT("Go to in point tooltip")));
            p.actions["NextFrame"]->setTooltip(_getText(DJV_TEXT("Next frame tooltip")));
            p.actions["PrevFrame"]->setTooltip(_getText(DJV_TEXT("Previous frame tooltip")));
            p.actions["OutPoint"]->setTooltip(_getText(DJV_TEXT("Go to out point tooltip")));

            p.currentTimeLabel->setTooltip(_getText(DJV_TEXT("Current time tooltip")));
            p.durationLabel->setTooltip(_getText(DJV_TEXT("Duration tooltip")));

            p.audioPopupWidget->setTooltip(_getText(DJV_TEXT("Audio popup tooltip")));
            p.volumeSlider->setTooltip(_getText(DJV_TEXT("Volume tooltip")));
            p.muteButton->setTooltip(_getText(DJV_TEXT("Mute tooltip")));

            _speedUpdate();
        }
        
    } // namespace ViewApp
} // namespace djv

