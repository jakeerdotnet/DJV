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

#include <djvUI/FormLayout.h>

#include <djvUI/GridLayout.h>
#include <djvUI/Label.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Form::Private
            {
                std::vector<ColorRole> alternateRowsRoles = { ColorRole::None, ColorRole::None };
                std::shared_ptr<Grid> layout;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<Label>> widgetToLabel;
            };

            void Form::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Form");

                DJV_PRIVATE_PTR();
                p.layout = Grid::create(context);
                Widget::addChild(p.layout);
            }

            Form::Form() :
                _p(new Private)
            {}

            Form::~Form()
            {}

            std::shared_ptr<Form> Form::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Form>(new Form);
                out->_init(context);
                return out;
            }

            void Form::setText(const std::shared_ptr<Widget> & value, const std::string & text)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.widgetToLabel.find(value);
                if (i != p.widgetToLabel.end())
                {
                    i->second->setText(text);
                }
                else if (auto context = getContext().lock())
                {
                    auto label = Label::create(context);
                    label->setText(text);
                    label->setTextHAlign(TextHAlign::Left);
                    label->setMargin(Margin(MetricsRole::MarginSmall));
                    glm::ivec2 gridPos = p.layout->getGridPos(value);
                    p.layout->addChild(label);
                    p.layout->setGridPos(label, glm::ivec2(0, gridPos.y));
                    p.layout->setGridPos(value, glm::ivec2(1, gridPos.y), GridStretch::Horizontal);
                    p.widgetToLabel[value] = label;
                }
            }

            const Spacing & Form::getSpacing() const
            {
                return _p->layout->getSpacing();
            }

            void Form::setSpacing(const Spacing & value)
            {
                _p->layout->setSpacing(value);
            }

            void Form::setAlternateRowsRoles(ColorRole value0, ColorRole value1)
            {
                DJV_PRIVATE_PTR();
                if (value0 == p.alternateRowsRoles[0] && value1 == p.alternateRowsRoles[1])
                    return;
                p.alternateRowsRoles = { value0, value1 };
                _widgetUpdate();
            }

            float Form::getHeightForWidth(float value) const
            {
                const auto& style = _getStyle();
                float out = _p->layout->getHeightForWidth(value - getMargin().getWidth(style)) + getMargin().getWidth(style);
                return out;
            }

            void Form::addChild(const std::shared_ptr<IObject> & value)
            {
                DJV_PRIVATE_PTR();
                const glm::ivec2 gridSize = p.layout->getGridSize();
                p.layout->addChild(value);
                if (auto widget = std::dynamic_pointer_cast<Widget>(value))
                {
                    p.layout->setGridPos(widget, glm::ivec2(1, gridSize.y));
                    p.layout->setStretch(widget, GridStretch::Horizontal);
                }
                _widgetUpdate();
            }

            void Form::removeChild(const std::shared_ptr<IObject> & value)
            {
                _p->layout->removeChild(value);
                if (auto label = std::dynamic_pointer_cast<Label>(value))
                {
                    auto i = _p->widgetToLabel.find(label);
                    if (i != _p->widgetToLabel.end())
                    {
                        _p->widgetToLabel.erase(i);
                    }
                }
                _widgetUpdate();
            }

            void Form::clearChildren()
            {
                _p->layout->clearChildren();
                _p->widgetToLabel.clear();
                _widgetUpdate();
            }

            void Form::_preLayoutEvent(Event::PreLayout & event)
            {
                const auto& style = _getStyle();
                _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
            }

            void Form::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();
                const BBox2f & g = getGeometry();
                const auto& style = _getStyle();
                p.layout->setGeometry(getMargin().bbox(g, style));
            }

            void Form::_widgetUpdate()
            {
                DJV_PRIVATE_PTR();
                const glm::ivec2 gridSize = p.layout->getGridSize();
                for (int y = 0; y < gridSize.y; ++y)
                {
                    p.layout->setRowBackgroundRole(y, (y % 2 == 0) ? p.alternateRowsRoles[0] : p.alternateRowsRoles[1]);
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv
