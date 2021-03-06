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

#include <djvCoreTest/ListObserverTest.h>

#include <djvCore/ListObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ListObserverTest::ListObserverTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ListObserverTest", context)
        {}
        
        void ListObserverTest::run(const std::vector<std::string>& args)
        {
            std::vector<int> value;
            auto subject = ListSubject<int>::create(value);
            {
                std::vector<int> value2;
                auto observer = ListObserver<int>::create(
                    subject,
                    [&value2](const std::vector<int> value)
                    {
                        value2 = value;
                    });
                DJV_ASSERT(1 == subject->getObserversCount());
                    
                DJV_ASSERT(!subject->setIfChanged(value));
                value.push_back(1);
                subject->setAlways(value);
                DJV_ASSERT(subject->get() == value2);
                DJV_ASSERT(!subject->isEmpty());
                DJV_ASSERT(1 == subject->getItem(0));
                value.push_back(2);
                DJV_ASSERT(subject->setIfChanged(value));
                DJV_ASSERT(!subject->setIfChanged(value));
                DJV_ASSERT(subject->get() == value2);
                
                DJV_ASSERT(subject->contains(2));
                DJV_ASSERT(1 == subject->indexOf(2));
            }
            DJV_ASSERT(0 == subject->getObserversCount());
        }
        
    } // namespace CoreTest
} // namespace djv

