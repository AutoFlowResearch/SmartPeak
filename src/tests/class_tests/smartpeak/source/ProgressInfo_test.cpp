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
#include <SmartPeak/core/ProgressInfo.h>

#include <chrono>
#include <thread>

namespace std
{
  std::ostream& operator<<(ostream& os, const tuple<size_t, string> t)
  {
    os << "(" << get<0>(t) << ", " << get<1>(t) << ")";
    return os;
  }

  std::ostream& operator<<(ostream& os, std::chrono::steady_clock::duration t)
  {
    os << t.count();
    return os;
  }
}

using namespace SmartPeak;

struct ProgressInfoFixture : public ::testing::Test
{
  struct ProgressInfoFixtureObservable:
    public ApplicationProcessorObservable,
    public SequenceProcessorObservable,
    public SequenceSegmentProcessorObservable,
    public SampleGroupProcessorObservable
  {

  };

  ProgressInfoFixture():
    progress_info_observable(),
    progress_info(progress_info_observable,
                  progress_info_observable,
                  progress_info_observable, 
                  progress_info_observable)
  {
  }

  ~ProgressInfoFixture() {}

public:
  ProgressInfoFixtureObservable progress_info_observable;
  ProgressInfo progress_info;
};

TEST_F(ProgressInfoFixture, isRunning)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4",
    "command5",
    "command6",
    "command7",
    "command8",
    "command9",
    "command10"
  };
  EXPECT_FALSE(progress_info.isRunning());
  progress_info_observable.notifyApplicationProcessorStart(commands);
  EXPECT_TRUE(progress_info.isRunning());
  progress_info_observable.notifyApplicationProcessorEnd();
  EXPECT_FALSE(progress_info.isRunning());
}

TEST_F(ProgressInfoFixture, runningTime)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4",
    "command5",
    "command6",
    "command7",
    "command8",
    "command9",
    "command10"
  };
  auto init_time = progress_info.runningTime();
  EXPECT_EQ(init_time.count(), std::chrono::steady_clock::duration::zero().count());
  progress_info_observable.notifyApplicationProcessorStart(commands);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto running_time = progress_info.runningTime();
  EXPECT_GE(running_time.count(), init_time.count());
}

TEST_F(ProgressInfoFixture, estimatedRemainingTime)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4"
  };
  auto estimated_time = progress_info.estimatedRemainingTime();
  EXPECT_TRUE(!estimated_time);
  progress_info_observable.notifyApplicationProcessorStart(commands);
  progress_info_observable.notifyApplicationProcessorCommandStart(1, "command1");
  progress_info_observable.notifyApplicationProcessorCommandStart(2, "command2");
  progress_info_observable.notifyApplicationProcessorCommandStart(3, "command3");
  progress_info_observable.notifyApplicationProcessorCommandStart(4, "command4");
  progress_info_observable.notifySequenceProcessorStart(10);
  progress_info_observable.notifySequenceProcessorSampleStart("injection1");
  estimated_time = progress_info.estimatedRemainingTime();
  EXPECT_TRUE(!estimated_time);
  progress_info_observable.notifySequenceProcessorSampleEnd("injection1");
  estimated_time = progress_info.estimatedRemainingTime();
  ASSERT_TRUE(estimated_time);
  EXPECT_GE(estimated_time->count(), 0);
}

TEST_F(ProgressInfoFixture, progressValue_1)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4"
  };
  auto progress_value = progress_info.progressValue();
  EXPECT_NEAR(progress_value, 0.0f, 1e-3f);
  progress_info_observable.notifyApplicationProcessorStart(commands);
  progress_info_observable.notifyApplicationProcessorCommandStart(1, "command1");
  progress_info_observable.notifyApplicationProcessorCommandStart(2, "command2");
  progress_info_observable.notifyApplicationProcessorCommandStart(3, "command3");
  progress_info_observable.notifyApplicationProcessorCommandStart(4, "command4");
  progress_info_observable.notifySequenceProcessorStart(10);
  progress_info_observable.notifySequenceProcessorSampleStart("injection1");
  progress_value = progress_info.progressValue();
  EXPECT_NEAR(progress_value, 0.0f, 1e-3f);
  progress_info_observable.notifySequenceProcessorSampleEnd("injection1");
  progress_value = progress_info.progressValue();
  EXPECT_NEAR(progress_value, 0.1f, 1e-3f);
}

TEST_F(ProgressInfoFixture, progressValue_2)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4"
  };
  // with 2 batches of commands, sequence and sample groups
  progress_info_observable.notifyApplicationProcessorStart(commands);
  progress_info_observable.notifyApplicationProcessorCommandStart(1, "command1");
  progress_info_observable.notifyApplicationProcessorCommandStart(2, "command2");

  progress_info_observable.notifySequenceProcessorStart(2);
  progress_info_observable.notifySequenceProcessorSampleStart("injection1");
  progress_info_observable.notifySequenceProcessorSampleEnd("injection1");
  progress_info_observable.notifySequenceProcessorSampleStart("injection2");
  progress_info_observable.notifySequenceProcessorSampleEnd("injection2");
  progress_info_observable.notifySequenceProcessorEnd();

  progress_info_observable.notifyApplicationProcessorCommandEnd(1, "command1");
  progress_info_observable.notifyApplicationProcessorCommandEnd(2, "command2");
  auto progress_value = progress_info.progressValue();
  EXPECT_NEAR(progress_value, 0.5f, 1e-3f);

  progress_info_observable.notifyApplicationProcessorCommandStart(3, "command3");
  progress_info_observable.notifyApplicationProcessorCommandStart(4, "command4");

  progress_info_observable.notifySampleGroupProcessorStart(2);
  progress_info_observable.notifySequenceProcessorSampleStart("sample1");
  progress_info_observable.notifySequenceProcessorSampleEnd("sample1");
  progress_value = progress_info.progressValue();
  EXPECT_NEAR(progress_value, 0.75f, 1e-3f);

  progress_info_observable.notifySequenceProcessorSampleStart("sample2");
  progress_info_observable.notifySequenceProcessorSampleEnd("sample2");
  progress_info_observable.notifySampleGroupProcessorEnd();
  progress_value = progress_info.progressValue();
  EXPECT_NEAR(progress_value, 1.0f, 1e-3f);

  progress_info_observable.notifyApplicationProcessorCommandEnd(3, "command3");
  progress_info_observable.notifyApplicationProcessorCommandEnd(4, "command4");
  progress_info_observable.notifyApplicationProcessorEnd();
  progress_value = progress_info.progressValue();
  EXPECT_NEAR(progress_value, 1.0f, 1e-3f);
}

TEST_F(ProgressInfoFixture, commands)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4"
  };
  auto progress_value = progress_info.progressValue();
  progress_info_observable.notifyApplicationProcessorStart(commands);
  const auto& wf_commands = progress_info.allCommands();
  std::vector<std::string>
    expected_commands {
      "command1",
      "command2",
      "command3",
      "command4",
  };
  EXPECT_THAT(wf_commands, ::testing::ContainerEq(expected_commands));
}

TEST_F(ProgressInfoFixture, runningCommands)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4"
  };
  auto progress_value = progress_info.progressValue();
  progress_info_observable.notifyApplicationProcessorStart(commands);
  progress_info_observable.notifyApplicationProcessorCommandStart(1, "command1");
  progress_info_observable.notifyApplicationProcessorCommandStart(2, "command2");
  progress_info_observable.notifyApplicationProcessorCommandStart(3, "command3");
  progress_info_observable.notifyApplicationProcessorCommandStart(4, "command4");
  const auto& running_commands = progress_info.runningCommands();
  std::vector<std::tuple<size_t, std::string>>
    expected_commands {
      {1,"command1"},
      {2,"command2"},
      {3,"command3"},
      {4,"command4"},
    };
  EXPECT_THAT(running_commands, ::testing::ContainerEq(expected_commands));
}

TEST_F(ProgressInfoFixture, runningBatch)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4"
  };
  auto progress_value = progress_info.progressValue();
  EXPECT_NEAR(progress_value, 0.0f, 1e-3f);
  progress_info_observable.notifyApplicationProcessorStart(commands);
  progress_info_observable.notifyApplicationProcessorCommandStart(1, "command1");
  progress_info_observable.notifyApplicationProcessorCommandStart(2, "command2");
  progress_info_observable.notifyApplicationProcessorCommandStart(3, "command3");
  progress_info_observable.notifyApplicationProcessorCommandStart(4, "command4");
  progress_info_observable.notifySequenceSegmentProcessorStart(10);
  progress_info_observable.notifySequenceSegmentProcessorSampleStart("segment1");
  progress_info_observable.notifySequenceSegmentProcessorSampleStart("segment2");
  progress_info_observable.notifySequenceSegmentProcessorSampleStart("segment3");
  progress_info_observable.notifySequenceSegmentProcessorSampleStart("segment4");
  progress_info_observable.notifySequenceSegmentProcessorSampleStart("segment5");
  const auto& segments = progress_info.runningBatch();
  std::vector<std::string> expected_segments{
    "segment1",
    "segment2",
    "segment3",
    "segment4",
    "segment5"
  };
  EXPECT_THAT(segments, ::testing::ContainerEq(expected_segments));
}

TEST_F(ProgressInfoFixture, lastRunTime)
{
  std::vector<std::string> commands =
  {
    "command1",
    "command2",
    "command3",
    "command4"
  };
  EXPECT_EQ(progress_info.lastRunTime(), std::chrono::steady_clock::duration::zero());
  progress_info_observable.notifyApplicationProcessorStart(commands);
  progress_info_observable.notifyApplicationProcessorEnd();
  EXPECT_NE(progress_info.lastRunTime(), std::chrono::steady_clock::duration::zero());
}
