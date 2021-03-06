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

#include <djvCoreTest/ISystemTest.h>

#include <djvCore/Context.h>
#include <djvCore/ISystem.h>
#include <djvCore/ResourceSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        class TestSystem : public ISystem
        {
            DJV_NON_COPYABLE(TestSystem);
            void _init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("TestSystem", context);
                _log(_getText("TestSystem"));
                {
                    std::stringstream ss;
                    ss << "Application path: ";
                    ss << _getResourceSystem()->getPath(FileSystem::ResourcePath::Application);
                    _log(ss.str());
                }
            }
            
            TestSystem()
            {}

        public:
            static std::shared_ptr<TestSystem> create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<TestSystem>(new TestSystem);
                out->_init(context);
                return out;
            }
        };

        ISystemTest::ISystemTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ISystemTest", context)
        {}
                
        void ISystemTest::run(const std::vector<std::string>&)
        {
            if (auto context = getContext().lock())
            {
                auto system = TestSystem::create(context);
            }
        }
                
    } // namespace CoreTest
} // namespace djv

