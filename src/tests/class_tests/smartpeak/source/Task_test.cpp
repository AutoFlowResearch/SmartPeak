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
// $Maintainer: Krzysztof Abram, Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SmartPeak/test_config.h>

#include <SmartPeak/cli/ApplicationManager.h>
#include <SmartPeak/cli/Task.h>

static bool plog_init = false;
void init_plog()
{
  if (!plog_init)
  {
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
    plog_init = true;
  }
}

struct TaskFixture : public ::testing::Test
{
    /* ctor/dtor */
    TaskFixture() 
    {
        std::string seq = std::string{SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_sequence.csv")};
        m_args = std::vector<std::string>{
            "Task_test", 
                "--report", "featureDB", "pivottable",
                "--report-metadata", "PeAk_aRea", "Mz", "rt",
                "--report-sample-types", "stanDard", "Unknown", "double blank",
                "--load-session", seq,
                "--verbose",
                "--allow-inconsistent",
                "--log-dir", ".",
                "--integrity", "SAMPLE", "is",
                "--disable-colors"
        };
    }

public:
    std::vector<std::string> m_args;
};

/* ---------------------------------------------- */
/*
TEST(Task, Task_run_1_thread)
{
  init_plog();
  namespace cli = SmartPeak::cli;
  std::filesystem::path seq_path = SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs");
  std::string seq = std::string{ seq_path.lexically_normal().generic_string() };
  std::vector<std::string> args = std::vector<std::string>{
      "Task_test",
          "--load-session", seq,
          "--verbose",
          "--allow-inconsistent",
          "--log-dir", ".",
          "--disable-colors",
          "--parameter", "SequenceProcessor:n_thread=1"
  };
  try
  {
    auto pa = cli::Parser{ args };
    auto as = cli::ApplicationSettings{ pa };
    auto am = cli::ApplicationManager{ as };
    am
      .add(std::make_shared<cli::InitializeApplicationSettings>())
      .add(std::make_shared<cli::InitializeLogger>())
      .add(std::make_shared<cli::LoadSession>())
      .add(std::make_shared<cli::InitializeWorkflowResources>())
      .add(std::make_shared<cli::InitializeWorkflowSettings>())
      .add(std::make_shared<cli::RunIntegrityChecks>())
      .add(std::make_shared<cli::RunWorkflow>())
      .add(std::make_shared<cli::ExportReport>());

    am.run();
  }
  catch (const std::exception& e)
  {
    LOG_ERROR << e.what();
  }
}

TEST(Task, Task_run_2_threads)
{
  init_plog();
  namespace cli = SmartPeak::cli;
  std::filesystem::path seq_path = SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs");
  std::string seq = std::string{ seq_path.lexically_normal().generic_string() };
  std::vector<std::string> args = std::vector<std::string>{
      "Task_test",
          "--load-session", seq,
          "--verbose",
          "--allow-inconsistent",
          "--log-dir", ".",
          "--disable-colors",
          "--parameter", "SequenceProcessor:n_thread=2"
  };
  try
  {
    auto pa = cli::Parser{ args };
    auto as = cli::ApplicationSettings{ pa };
    auto am = cli::ApplicationManager{ as };
    am
      .add(std::make_shared<cli::InitializeApplicationSettings>())
      .add(std::make_shared<cli::InitializeLogger>())
      .add(std::make_shared<cli::LoadSession>())
      .add(std::make_shared<cli::InitializeWorkflowResources>())
      .add(std::make_shared<cli::InitializeWorkflowSettings>())
      .add(std::make_shared<cli::RunIntegrityChecks>())
      .add(std::make_shared<cli::RunWorkflow>())
      .add(std::make_shared<cli::ExportReport>());

    am.run();
  }
  catch (const std::exception& e)
  {
    LOG_ERROR << e.what();
  }
}
*/
TEST(Task, Task_run_10_threads)
{
  init_plog();
  namespace cli = SmartPeak::cli;
  std::filesystem::path seq_path = SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs");
  std::string seq = std::string{ seq_path.lexically_normal().generic_string() };
  std::vector<std::string> args = std::vector<std::string>{
      "Task_test",
          "--load-session", seq,
          "--verbose",
          "--allow-inconsistent",
          "--log-dir", ".",
          "--disable-colors",
          "--parameter", "SequenceProcessor:n_thread=10"
  };
  try
  {
    auto pa = cli::Parser{ args };
    auto as = cli::ApplicationSettings{ pa };
    auto am = cli::ApplicationManager{ as };
    am
      .add(std::make_shared<cli::InitializeApplicationSettings>())
      .add(std::make_shared<cli::InitializeLogger>())
      .add(std::make_shared<cli::LoadSession>())
      .add(std::make_shared<cli::InitializeWorkflowResources>())
      .add(std::make_shared<cli::InitializeWorkflowSettings>())
      .add(std::make_shared<cli::RunIntegrityChecks>())
      .add(std::make_shared<cli::RunWorkflow>())
      .add(std::make_shared<cli::ExportReport>());

    am.run();
  }
  catch (const std::exception& e)
  {
    LOG_ERROR << e.what();
  }
}
