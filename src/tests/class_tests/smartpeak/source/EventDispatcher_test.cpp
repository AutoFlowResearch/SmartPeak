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
// $Maintainer: Bertrand Boudaud, Ahmed Khalil $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/EventDispatcher.h>

namespace std
{
  std::ostream& operator<<(ostream& os, const tuple<string, size_t, string, std::vector<std::string> > t)
  {
    os << "(" << get<0>(t) << ", " << get<1>(t) << ", " << get<2>(t) << ", [";
    std::string separator = "";
    for (const auto& command : get<3>(t))
    {
      os << separator << command;
      separator = ", ";
    }
    os << "])";
    return os;
  }
}

using namespace SmartPeak;

struct EventDispatcherFixture : public ::testing::Test
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
    virtual void onApplicationProcessorStart(const std::vector<std::string>& commands) override
    {
      events_.push_back(std::make_tuple("onApplicationProcessorStart", 0, "", commands));
    }
    virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override
    {
      events_.push_back(std::make_tuple("onApplicationProcessorCommandStart", command_index, command_name, std::vector<std::string>()));
    }
    virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override
    {
      events_.push_back(std::make_tuple("onApplicationProcessorCommandEnd", command_index, command_name, std::vector<std::string>()));
    }
    virtual void onApplicationProcessorEnd() override
    {
      events_.push_back(std::make_tuple("onApplicationProcessorEnd", 0, "", std::vector<std::string>()));
    }
    virtual void onApplicationProcessorError(const std::string& error) override
    {
      // TODO
    }

    /**
      ISequenceSegmentProcessorObserver
    */
    virtual void onSequenceProcessorStart(const size_t nb_injections) override
    {
      events_.push_back(std::make_tuple("onSequenceProcessorStart", nb_injections, "", std::vector<std::string>()));
    }
    virtual void onSequenceProcessorSampleStart(const std::string& sample_name) override
    {
      events_.push_back(std::make_tuple("onSequenceProcessorSampleStart", 0, sample_name, std::vector<std::string>()));
    }
    virtual void onSequenceProcessorError(const std::string& sample_name, const std::string& processor_name, const std::string& error) override
    {
      // TODO
    }
    virtual void onSequenceProcessorSampleEnd(const std::string& sample_name) override
    {
      events_.push_back(std::make_tuple("onSequenceProcessorSampleEnd", 0, sample_name, std::vector<std::string>()));
    }
    virtual void onSequenceProcessorEnd() override
    {
      events_.push_back(std::make_tuple("onSequenceProcessorEnd", 0, "", std::vector<std::string>()));
    }
    virtual void onSequenceSegmentProcessorStart(const size_t nb_segments) override
    {
      events_.push_back(std::make_tuple("onSequenceSegmentProcessorStart", nb_segments, "", std::vector<std::string>()));
    }
    virtual void onSequenceSegmentProcessorSampleStart(const std::string& segment_name) override
    {
      events_.push_back(std::make_tuple("onSequenceSegmentProcessorSampleStart", 0, segment_name, std::vector<std::string>()));
    }
    virtual void onSequenceSegmentProcessorSampleEnd(const std::string& segment_name) override
    {
      events_.push_back(std::make_tuple("onSequenceSegmentProcessorSampleEnd", 0, segment_name, std::vector<std::string>()));
    }
    virtual void onSequenceSegmentProcessorEnd() override
    {
      events_.push_back(std::make_tuple("onSequenceSegmentProcessorEnd", 0, "", std::vector<std::string>()));
    }
    virtual void onSequenceSegmentProcessorError(const std::string& segment_name, const std::string& processor_name, const std::string& error) override
    {
      // TODO
    }

    /**
      ISampleGroupProcessorObserver
    */
    virtual void onSampleGroupProcessorStart(const size_t nb_groups) override
    {
      events_.push_back(std::make_tuple("onSampleGroupProcessorStart", nb_groups, "", std::vector<std::string>()));
    }
    virtual void onSampleGroupProcessorSampleStart(const std::string& group_name) override
    {
      events_.push_back(std::make_tuple("onSampleGroupProcessorSampleStart", 0, group_name, std::vector<std::string>()));
    }
    virtual void onSampleGroupProcessorSampleEnd(const std::string& group_name) override
    {
      events_.push_back(std::make_tuple("onSampleGroupProcessorSampleEnd", 0, group_name, std::vector<std::string>()));
    }
    virtual void onSampleGroupProcessorEnd() override
    {
      events_.push_back(std::make_tuple("onSampleGroupProcessorEnd", 0, "", std::vector<std::string>()));
    }
    virtual void onSampleGroupProcessorError(const std::string& group_name, const std::string& processor_name, const std::string& error)
    {
      // TODO
    }

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override
    {
      events_.push_back(std::make_tuple("onSequenceUpdated", 0, "", std::vector<std::string>()));
    }

    /**
      ITransitionsObserver
    */
    virtual void onTransitionsUpdated() override
    {
      events_.push_back(std::make_tuple("onTransitionsUpdated", 0, "", std::vector<std::string>()));
    }

    std::vector<std::tuple<std::string, size_t, std::string, std::vector<std::string>>> events_;
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

TEST_F(EventDispatcherFixture, dispatchEvents)
{
  std::vector<std::tuple<std::string, size_t, std::string, std::vector<std::string>>>
    expected_commands{ };
  
  EXPECT_THAT(event_displatcher_observer.events_, ::testing::ContainerEq(expected_commands));

  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4"
  };
  
  event_dispatcher_observable.notifyApplicationProcessorStart(commands);
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

  EXPECT_THAT(event_displatcher_observer.events_, ::testing::ContainerEq(expected_commands));

  event_dispatcher.dispatchEvents();

  expected_commands = 
  {
    {"onApplicationProcessorStart",0,"", commands},
    {"onApplicationProcessorCommandStart",1,"command1", std::vector<std::string>()},
    {"onApplicationProcessorCommandStart",2,"command2", std::vector<std::string>()},
    {"onSequenceProcessorStart",2,"", std::vector<std::string>()},
    {"onSequenceProcessorSampleStart",0,"injection1", std::vector<std::string>()},
    {"onSequenceProcessorSampleEnd",0,"injection1", std::vector<std::string>()},
    {"onSequenceProcessorSampleStart",0,"injection2", std::vector<std::string>()},
    {"onSequenceProcessorSampleEnd",0,"injection2", std::vector<std::string>()},
    {"onSequenceProcessorEnd",0,"", std::vector<std::string>()},
    {"onApplicationProcessorCommandEnd",1,"command1", std::vector<std::string>()},
    {"onApplicationProcessorCommandEnd",2,"command2", std::vector<std::string>()},
    {"onApplicationProcessorCommandStart",3,"command3", std::vector<std::string>()},
    {"onApplicationProcessorCommandStart",4,"command4", std::vector<std::string>()},
    {"onSampleGroupProcessorStart",2,"", std::vector<std::string>()},
    {"onSequenceProcessorSampleStart",0,"sample1", std::vector<std::string>()},
    {"onSequenceProcessorSampleEnd",0,"sample1", std::vector<std::string>()},
    {"onSequenceProcessorSampleStart",0,"sample2", std::vector<std::string>()},
    {"onSequenceProcessorSampleEnd",0,"sample2", std::vector<std::string>()},
    {"onSampleGroupProcessorEnd",0,"", std::vector<std::string>()},
    {"onApplicationProcessorCommandEnd",3,"command3", std::vector<std::string>()},
    {"onApplicationProcessorCommandEnd",4,"command4", std::vector<std::string>()},
    {"onApplicationProcessorEnd",0,"", std::vector<std::string>()},
  };

  EXPECT_THAT(event_displatcher_observer.events_, ::testing::ContainerEq(expected_commands));
}
