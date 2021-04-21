// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Bertrand Boudaud $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#define BOOST_TEST_MODULE EventDispatcher test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/EventDispatcher.h>

namespace std
{
  std::ostream& operator<<(ostream& os, const tuple<string, size_t, string> t)
  {
    os << "(" << get<0>(t) << ", " << get<1>(t) << ", " << get<2>(t) << ")";
    return os;
  }
}

using namespace SmartPeak;

struct EventDispatcherFixture
{
  struct EventDispatcherFixtureObservable:
    ApplicationProcessorObservable,
    SequenceProcessorObservable,
    SequenceSegmentProcessorObservable,
    SampleGroupProcessorObservable,
    SequenceObservable,
    TransitionsObservable
  {

  };

  struct EventDispatcherObserver :
    IApplicationProcessorObserver,
    ISequenceProcessorObserver,
    ISequenceSegmentProcessorObserver,
    ISampleGroupProcessorObserver,
    ISequenceObserver,
    ITransitionsObserver
  {
    /**
      IApplicationProcessorObserver
    */
    virtual void onApplicationProcessorStart(const size_t nb_commands) override
    {
      events_.push_back(std::make_tuple("onApplicationProcessorStart", nb_commands, ""));
    }
    virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override
    {
      events_.push_back(std::make_tuple("onApplicationProcessorCommandStart", command_index, command_name));
    }
    virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override
    {
      events_.push_back(std::make_tuple("onApplicationProcessorCommandEnd", command_index, command_name));
    }
    virtual void onApplicationProcessorEnd() override
    {
      events_.push_back(std::make_tuple("onApplicationProcessorEnd", 0, ""));
    }
    /**
      ISequenceSegmentProcessorObserver
    */
    virtual void onSequenceProcessorStart(const size_t nb_injections) override
    {
      events_.push_back(std::make_tuple("onSequenceProcessorStart", nb_injections, ""));
    }
    virtual void onSequenceProcessorSampleStart(const std::string& sample_name) override
    {
      events_.push_back(std::make_tuple("onSequenceProcessorSampleStart", 0, sample_name));
    }
    virtual void onSequenceProcessorSampleEnd(const std::string& sample_name) override
    {
      events_.push_back(std::make_tuple("onSequenceProcessorSampleEnd", 0, sample_name));
    }
    virtual void onSequenceProcessorEnd() override
    {
      events_.push_back(std::make_tuple("onSequenceProcessorEnd", 0, ""));
    }
    virtual void onSequenceSegmentProcessorStart(const size_t nb_segments) override
    {
      events_.push_back(std::make_tuple("onSequenceSegmentProcessorStart", nb_segments, ""));
    }
    virtual void onSequenceSegmentProcessorSampleStart(const std::string& segment_name) override
    {
      events_.push_back(std::make_tuple("onSequenceSegmentProcessorSampleStart", 0, segment_name));
    }
    virtual void onSequenceSegmentProcessorSampleEnd(const std::string& segment_name) override
    {
      events_.push_back(std::make_tuple("onSequenceSegmentProcessorSampleEnd", 0, segment_name));
    }
    virtual void onSequenceSegmentProcessorEnd() override
    {
      events_.push_back(std::make_tuple("onSequenceSegmentProcessorEnd", 0, ""));
    }

    /**
      ISampleGroupProcessorObserver
    */
    virtual void onSampleGroupProcessorStart(const size_t nb_groups) override
    {
      events_.push_back(std::make_tuple("onSampleGroupProcessorStart", nb_groups, ""));
    }
    virtual void onSampleGroupProcessorSampleStart(const std::string& group_name) override
    {
      events_.push_back(std::make_tuple("onSampleGroupProcessorSampleStart", 0, group_name));
    }
    virtual void onSampleGroupProcessorSampleEnd(const std::string& group_name) override
    {
      events_.push_back(std::make_tuple("onSampleGroupProcessorSampleEnd", 0, group_name));
    }
    virtual void onSampleGroupProcessorEnd() override
    {
      events_.push_back(std::make_tuple("onSampleGroupProcessorEnd", 0, ""));
    }

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override
    {
      events_.push_back(std::make_tuple("onSequenceUpdated", 0, ""));
    }

    /**
      ITransitionsObserver
    */
    virtual void onTransitionsUpdated() override
    {
      events_.push_back(std::make_tuple("onTransitionsUpdated", 0, ""));
    }

    std::vector<std::tuple<std::string, size_t, std::string>> events_;
  };

  EventDispatcherFixture()
  {
    event_dispatcher_observable.addApplicationProcessorObserver(&event_dispatcher);
    event_dispatcher_observable.addSequenceProcessorObserver(&event_dispatcher);
    event_dispatcher_observable.addSequenceSegmentProcessorObserver(&event_dispatcher);
    event_dispatcher_observable.addSampleGroupProcessorObserver(&event_dispatcher);
    event_dispatcher_observable.addSequenceObserver(&event_dispatcher);
    event_dispatcher_observable.addTransitionsObserver(&event_dispatcher);

    event_dispatcher.addApplicationProcessorObserver(&event_displatcher_observer);
    event_dispatcher.addSequenceProcessorObserver(&event_displatcher_observer);
    event_dispatcher.addSequenceSegmentProcessorObserver(&event_displatcher_observer);
    event_dispatcher.addSampleGroupProcessorObserver(&event_displatcher_observer);
    event_dispatcher.addSequenceObserver(&event_displatcher_observer);
    event_dispatcher.addTransitionsObserver(&event_displatcher_observer);
  }

public:
  EventDispatcherFixtureObservable event_dispatcher_observable;
  EventDispatcher event_dispatcher;
  EventDispatcherObserver event_displatcher_observer;
};

BOOST_FIXTURE_TEST_SUITE(EventDispatcher, EventDispatcherFixture)


BOOST_AUTO_TEST_CASE(dispatchEvents)
{
  std::vector<std::tuple<std::string, size_t, std::string>>
    expected_commands{ };
  BOOST_CHECK_EQUAL_COLLECTIONS(event_displatcher_observer.events_.begin(), event_displatcher_observer.events_.end(),
    expected_commands.begin(), expected_commands.end());

  event_dispatcher_observable.notifyApplicationProcessorStart(4);
  event_dispatcher_observable.notifyApplicationProcessorCommandStart(1, "command1");
  event_dispatcher_observable.notifyApplicationProcessorCommandStart(2, "command2");
  event_dispatcher_observable.notifySequenceProcessorStart(2);
  event_dispatcher_observable.notifySequenceProcessorSampleStart("injection1");
  event_dispatcher_observable.notifySequenceProcessorSampleEnd("injection1");
  event_dispatcher_observable.notifySequenceProcessorSampleStart("injection2");
  event_dispatcher_observable.notifySequenceProcessorSampleEnd("injection2");
  event_dispatcher_observable.notifySequenceProcessorEnd();
  event_dispatcher_observable.notifyApplicationProcessorCommandEnd(1, "command1");
  event_dispatcher_observable.notifyApplicationProcessorCommandEnd(2, "command2");
  event_dispatcher_observable.notifyApplicationProcessorCommandStart(3, "command3");
  event_dispatcher_observable.notifyApplicationProcessorCommandStart(4, "command4");
  event_dispatcher_observable.notifySampleGroupProcessorStart(2);
  event_dispatcher_observable.notifySequenceProcessorSampleStart("sample1");
  event_dispatcher_observable.notifySequenceProcessorSampleEnd("sample1");
  event_dispatcher_observable.notifySequenceProcessorSampleStart("sample2");
  event_dispatcher_observable.notifySequenceProcessorSampleEnd("sample2");
  event_dispatcher_observable.notifySampleGroupProcessorEnd();
  event_dispatcher_observable.notifyApplicationProcessorCommandEnd(3, "command3");
  event_dispatcher_observable.notifyApplicationProcessorCommandEnd(4, "command4");
  event_dispatcher_observable.notifyApplicationProcessorEnd();

  BOOST_CHECK_EQUAL_COLLECTIONS(event_displatcher_observer.events_.begin(), event_displatcher_observer.events_.end(),
    expected_commands.begin(), expected_commands.end());

  event_dispatcher.dispatchEvents();

  expected_commands = 
  {
    {"onApplicationProcessorStart",4,""},
    {"onApplicationProcessorCommandStart",1,"command1"},
    {"onApplicationProcessorCommandStart",2,"command2"},
    {"onSequenceProcessorStart",2,""},
    {"onSequenceProcessorSampleStart",0,"injection1"},
    {"onSequenceProcessorSampleEnd",0,"injection1"},
    {"onSequenceProcessorSampleStart",0,"injection2"},
    {"onSequenceProcessorSampleEnd",0,"injection2"},
    {"onSequenceProcessorEnd",0,""},
    {"onApplicationProcessorCommandEnd",1,"command1"},
    {"onApplicationProcessorCommandEnd",2,"command2"},
    {"onApplicationProcessorCommandStart",3,"command3"},
    {"onApplicationProcessorCommandStart",4,"command4"},
    {"onSampleGroupProcessorStart",2,""},
    {"onSequenceProcessorSampleStart",0,"sample1"},
    {"onSequenceProcessorSampleEnd",0,"sample1"},
    {"onSequenceProcessorSampleStart",0,"sample2"},
    {"onSequenceProcessorSampleEnd",0,"sample2"},
    {"onSampleGroupProcessorEnd",0,""},
    {"onApplicationProcessorCommandEnd",3,"command3"},
    {"onApplicationProcessorCommandEnd",4,"command4"},
    {"onApplicationProcessorEnd",0,""},
  };

  BOOST_CHECK_EQUAL_COLLECTIONS(event_displatcher_observer.events_.begin(), event_displatcher_observer.events_.end(),
    expected_commands.begin(), expected_commands.end());
}

BOOST_AUTO_TEST_SUITE_END()
