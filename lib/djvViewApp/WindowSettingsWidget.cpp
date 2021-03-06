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

#include <djvViewApp/WindowSettingsWidget.h>

#include <djvViewApp/WindowSettings.h>

#include <djvDesktopApp/GLFWSystem.h>

#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/LineEdit.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolButton.h>

#include <djvAV/IO.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AutoHideSettingsWidget::Private
        {
            std::shared_ptr<UI::CheckBox> autoHideCheckBox;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<bool> > autoHideObserver;
        };

        void AutoHideSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::AutoHideSettingsWidget");

            p.autoHideCheckBox = UI::CheckBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->addChild(p.autoHideCheckBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<AutoHideSettingsWidget>(std::dynamic_pointer_cast<AutoHideSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.autoHideCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
                            windowSettings->setAutoHide(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto windowSettings = settingsSystem->getSettingsT<WindowSettings>();
            p.autoHideObserver = ValueObserver<bool>::create(
                windowSettings->observeAutoHide(),
                [weak](bool value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->autoHideCheckBox->setChecked(value);
                }
            });
        }

        AutoHideSettingsWidget::AutoHideSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<AutoHideSettingsWidget> AutoHideSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<AutoHideSettingsWidget>(new AutoHideSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string AutoHideSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Auto-Hide");
        }

        std::string AutoHideSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Window");
        }

        std::string AutoHideSettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void AutoHideSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.autoHideCheckBox->setText(_getText(DJV_TEXT("Automatically hide the user interface")));
        }
        
        struct FullscreenMonitorSettingsWidget::Private
        {
            std::vector<std::string> monitorNames;
            int monitor = 0;
            std::shared_ptr<UI::ComboBox> monitorComboBox;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<ListObserver<Desktop::MonitorInfo> > monitorInfoObserver;
            std::shared_ptr<ValueObserver<int> > monitorObserver;
        };

        void FullscreenMonitorSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::FullscreenMonitorSettingsWidget");

            p.monitorComboBox = UI::ComboBox::create(context);

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.monitorComboBox);
            addChild(p.formLayout);

            auto weak = std::weak_ptr<FullscreenMonitorSettingsWidget>(std::dynamic_pointer_cast<FullscreenMonitorSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.monitorComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                            {
                                windowSettings->setFullscreenMonitor(value);
                            }
                        }
                    }
                });

            auto glfwSystem = context->getSystemT<Desktop::GLFWSystem>();
            p.monitorInfoObserver = ListObserver<Desktop::MonitorInfo>::create(
                glfwSystem->observeMonitorInfo(),
                [weak](const std::vector<Desktop::MonitorInfo>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->monitorNames.clear();
                        for (const auto& i : value)
                        {
                            widget->_p->monitorNames.push_back(i.name);
                        }
                        widget->_widgetUpdate();
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
            {
                p.monitorObserver = ValueObserver<int>::create(
                    windowSettings->observeFullscreenMonitor(),
                    [weak](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->monitor = value;
                            widget->_widgetUpdate();
                        }
                    });
            }
        }

        FullscreenMonitorSettingsWidget::FullscreenMonitorSettingsWidget() :
            _p(new Private)
        {}

        FullscreenMonitorSettingsWidget::~FullscreenMonitorSettingsWidget()
        {}

        std::shared_ptr<FullscreenMonitorSettingsWidget> FullscreenMonitorSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FullscreenMonitorSettingsWidget>(new FullscreenMonitorSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string FullscreenMonitorSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Fullscreen");
        }

        std::string FullscreenMonitorSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Window");
        }

        std::string FullscreenMonitorSettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void FullscreenMonitorSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.monitorComboBox, _getText(DJV_TEXT("Monitor")) + ":");
        }

        void FullscreenMonitorSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.monitorComboBox->setItems(p.monitorNames);
            p.monitorComboBox->setCurrentItem(p.monitor);
        }

        struct BackgroundImageSettingsWidget::Private
        {
            std::string fileName;
            AV::ThumbnailSystem::ImageFuture imageFuture;
            std::shared_ptr<AV::Image::Image> image;
            bool scale = false;
            bool colorize = false;
            Core::FileSystem::Path fileBrowserPath = Core::FileSystem::Path(".");

            std::shared_ptr<UI::ImageWidget> imageWidget;
            std::shared_ptr<UI::LineEdit> lineEdit;
            std::shared_ptr<UI::ToolButton> openButton;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::CheckBox> scaleCheckBox;
            std::shared_ptr<UI::CheckBox> colorizeCheckBox;
            std::shared_ptr<UI::HorizontalLayout> layout;
            std::shared_ptr<UI::FileBrowser::Dialog> fileBrowserDialog;

            std::shared_ptr<ValueObserver<std::string> > backgroundImageObserver;
            std::shared_ptr<ValueObserver<bool> > backgroundImageScaleObserver;
            std::shared_ptr<ValueObserver<bool> > backgroundImageColorizeObserver;
        };

        void BackgroundImageSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::BackgroundImageSettingsWidget");

            p.imageWidget = UI::ImageWidget::create(context);
            p.imageWidget->setImageAlphaBlend(AV::AlphaBlend::Straight);
            p.imageWidget->setSizeRole(UI::MetricsRole::TextColumn);
            p.imageWidget->setHAlign(UI::HAlign::Center);
            p.imageWidget->setVAlign(UI::VAlign::Center);

            p.lineEdit = UI::LineEdit::create(context);

            p.openButton = UI::ToolButton::create(context);
            p.openButton->setIcon("djvIconFile");
            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");

            p.scaleCheckBox = UI::CheckBox::create(context);
            p.colorizeCheckBox = UI::CheckBox::create(context);

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->addChild(p.imageWidget);
            auto vLayout = UI::VerticalLayout::create(context);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            hLayout ->addChild(p.lineEdit);
            hLayout->setStretch(p.lineEdit, UI::RowStretch::Expand);
            hLayout->addChild(p.openButton);
            hLayout->addChild(p.closeButton);
            vLayout->addChild(hLayout);
            vLayout->addChild(p.scaleCheckBox);
            vLayout->addChild(p.colorizeCheckBox);
            p.layout->addChild(vLayout);
            p.layout->setStretch(vLayout, UI::RowStretch::Expand);
            addChild(p.layout);

            _widgetUpdate();
            _imageUpdate();

            auto weak = std::weak_ptr<BackgroundImageSettingsWidget>(std::dynamic_pointer_cast<BackgroundImageSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.lineEdit->setTextEditCallback(
                [weak, contextWeak](const std::string& value, UI::TextEditReason)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                            {
                                windowSettings->setBackgroundImage(value);
                            }
                        }
                    }
                });

            p.openButton->setClickedCallback(
                [weak, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->fileBrowserDialog)
                            {
                                widget->_p->fileBrowserDialog->close();
                            }
                            widget->_p->fileBrowserDialog = UI::FileBrowser::Dialog::create(context);
                            auto io = context->getSystemT<AV::IO::System>();
                            widget->_p->fileBrowserDialog->setFileExtensions(io->getFileExtensions());
                            widget->_p->fileBrowserDialog->setPath(widget->_p->fileBrowserPath);
                            widget->_p->fileBrowserDialog->setCallback(
                                [weak, contextWeak](const Core::FileSystem::FileInfo& value)
                                {
                                    if (auto context = contextWeak.lock())
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                            widget->_p->fileBrowserDialog->close();
                                            widget->_p->fileBrowserDialog.reset();
                                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                                            {
                                                windowSettings->setBackgroundImage(std::string(value.getPath()));
                                            }
                                        }
                                    }
                                });
                            widget->_p->fileBrowserDialog->setCloseCallback(
                                [weak]
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->fileBrowserPath = widget->_p->fileBrowserDialog->getPath();
                                        widget->_p->fileBrowserDialog->close();
                                        widget->_p->fileBrowserDialog.reset();
                                    }
                                });
                            widget->_p->fileBrowserDialog->show();
                        }
                    }
                });

            p.closeButton->setClickedCallback(
                [contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setBackgroundImage(std::string());
                        }
                    }
                });

            p.scaleCheckBox->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setBackgroundImageScale(value);
                        }
                    }
                });

            p.colorizeCheckBox->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
                        {
                            windowSettings->setBackgroundImageColorize(value);
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
            {
                p.backgroundImageObserver = ValueObserver<std::string>::create(
                    windowSettings->observeBackgroundImage(),
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->fileName = value;
                            widget->_widgetUpdate();
                            widget->_imageUpdate();
                        }
                    });

                p.backgroundImageScaleObserver = ValueObserver<bool>::create(
                    windowSettings->observeBackgroundImageScale(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->scale = value;
                            widget->_widgetUpdate();
                        }
                    });

                p.backgroundImageColorizeObserver = ValueObserver<bool>::create(
                    windowSettings->observeBackgroundImageColorize(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->colorize = value;
                            widget->_widgetUpdate();
                        }
                    });
            }
        }

        BackgroundImageSettingsWidget::BackgroundImageSettingsWidget() :
            _p(new Private)
        {}

        BackgroundImageSettingsWidget::~BackgroundImageSettingsWidget()
        {
            DJV_PRIVATE_PTR();
            if (p.fileBrowserDialog)
            {
                p.fileBrowserDialog->close();
            }
        }

        std::shared_ptr<BackgroundImageSettingsWidget> BackgroundImageSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<BackgroundImageSettingsWidget>(new BackgroundImageSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string BackgroundImageSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Background Image");
        }

        std::string BackgroundImageSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Window");
        }

        std::string BackgroundImageSettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void BackgroundImageSettingsWidget::_initEvent(Event::Init& event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            p.openButton->setTooltip(_getText(DJV_TEXT("Open the file browser")));
            p.closeButton->setTooltip(_getText(DJV_TEXT("Clear the background image")));
            p.scaleCheckBox->setText(_getText(DJV_TEXT("Scale to fit")));
            p.colorizeCheckBox->setText(_getText(DJV_TEXT("Colorize with the UI style")));
            _imageUpdate();
        }

        void BackgroundImageSettingsWidget::_updateEvent(Event::Update&)
        {
            DJV_PRIVATE_PTR();
            if (p.imageFuture.future.valid() &&
                p.imageFuture.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.image = p.imageFuture.future.get();
                }
                catch (const std::exception& e)
                {
                    p.image.reset();
                    _log(e.what(), LogLevel::Error);
                }
                p.imageWidget->setImage(p.image);
            }
        }

        void BackgroundImageSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.imageWidget->setImageColorRole(p.colorize ? UI::ColorRole::Button : UI::ColorRole::None);
            p.lineEdit->setText(p.fileName);
            p.scaleCheckBox->setChecked(p.scale);
            p.colorizeCheckBox->setChecked(p.colorize);
        }

        void BackgroundImageSettingsWidget::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (!p.fileName.empty())
                {
                    const auto& style = _getStyle();
                    const float s = style->getMetric(UI::MetricsRole::TextColumn);
                    auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                    p.imageFuture = thumbnailSystem->getImage(p.fileName, AV::Image::Size(s, s));
                }
            }
        }

    } // namespace ViewApp
} // namespace djv

