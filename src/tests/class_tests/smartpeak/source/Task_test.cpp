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
// $Maintainer: Krzysztof Abram $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SmartPeak/test_config.h>

#include <SmartPeak/cli/ApplicationManager.h>
#include <SmartPeak/cli/Task.h>


struct TaskFixture : public ::testing::Test
{
    /* ctor/dtor */
    TaskFixture() 
    {
        std::string seq = std::string{SMARTPEAK_GET_TEST_DATA_PATH("/SequenceProcessor_sequence.csv")};
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

TEST_F(TaskFixture, Task_InitializeApplicationSettings)
{
    namespace cli = SmartPeak::cli;
    {
        auto pa = cli::Parser{m_args};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto task = cli::InitializeApplicationSettings{};
        EXPECT_TRUE(task(am));
    }
    {
        auto argv = m_args;
        argv[2] = "wrongvalue";
        auto pa = cli::Parser{argv};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto task = cli::InitializeApplicationSettings{};
        EXPECT_NO_THROW(task(am));
    }
    {
        auto argv = m_args;
        argv[2] = "wrongvalue";
        auto pa = cli::Parser{argv};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto task = cli::InitializeApplicationSettings{};
        EXPECT_TRUE(!task(am));
    }
}

TEST_F(TaskFixture, Task_InitializeLogger)
{
    namespace cli = SmartPeak::cli;
    {
        auto pa = cli::Parser{m_args};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto task = cli::InitializeLogger{};
        // Can't init twice:
        EXPECT_TRUE(task(am));
        EXPECT_TRUE(!task(am));
    }
}

TEST_F(TaskFixture, Task_LoadSession)
{
    namespace cli = SmartPeak::cli;
    auto pa = cli::Parser{m_args};
    auto as = cli::ApplicationSettings{pa};
    auto am = cli::ApplicationManager{as};
    auto task = cli::LoadSession{};
    EXPECT_TRUE(!task(am));
}
