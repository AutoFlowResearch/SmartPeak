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

#include <SmartPeak/cli/Parser.h>
#include <SmartPeak/cli/ApplicationManager.h>
#include <SmartPeak/cli/ApplicationSettings.h>
#include <SmartPeak/cli/Task.h>


namespace cli {

class ParserMock : public SmartPeak::cli::Parser
{
    using base_type = SmartPeak::cli::Parser;
public:
    explicit ParserMock(int argc, const char** argv) : base_type {argc, argv} {}
};

class ApplicationSettingsMock : public SmartPeak::cli::ApplicationSettings
{
    using base_type = SmartPeak::cli::ApplicationSettings;
public:
    explicit ApplicationSettingsMock(SmartPeak::cli::Parser& parser) : base_type {parser} {}

    void define_options() override {}
    void load_options() override {}
    void process_options() override {}
    void validate() const override {}
};

class SuccessTaskMock : public SmartPeak::cli::Task
{
public:
    bool operator() (SmartPeak::cli::ApplicationManager& application_manager) override
    {
        return true;
    }
};

class FailureTaskMock : public SmartPeak::cli::Task
{
public:
    bool operator() (SmartPeak::cli::ApplicationManager& application_manager) override
    {
        return false;
    }
};

class ThrowTaskMock : public SmartPeak::cli::Task
{
public:
    bool operator() (SmartPeak::cli::ApplicationManager& application_manager) override
    {
        throw std::runtime_error{"Error"};
    }
};

}; /* namespace cli */

/* ---------------------------------------------- */

struct ApplicationManagerFixture : public ::testing::Test
{
    /* ctor/dtor */
    ApplicationManagerFixture() 
    {
        const auto argc = 1;
        const char* argv[] = {"ApplicationManager_test"};
        m_parser_ptr = std::make_shared<cli::ParserMock>(argc, argv);
        m_application_settings_ptr = std::make_shared<cli::ApplicationSettingsMock>(*m_parser_ptr);
        m_application_manager_ptr = std::make_shared<SmartPeak::cli::ApplicationManager>(*m_application_settings_ptr);
    }

    SmartPeak::cli::ApplicationManager& get_application_manager() { return *m_application_manager_ptr; }
    const SmartPeak::cli::ApplicationManager& get_application_manager() const { return *m_application_manager_ptr; }

public:
    std::shared_ptr<cli::ParserMock> m_parser_ptr;
    std::shared_ptr<cli::ApplicationSettingsMock> m_application_settings_ptr;
    std::shared_ptr<SmartPeak::cli::ApplicationManager> m_application_manager_ptr;
};

/* ---------------------------------------------- */

TEST_F(ApplicationManagerFixture, ApplicationManager_add)
{
    auto& application_manager = get_application_manager()
        .add(std::make_shared<cli::SuccessTaskMock>())
        .add(std::make_shared<cli::SuccessTaskMock>())
        .add(std::make_shared<cli::SuccessTaskMock>());
    
    EXPECT_EQ(application_manager.size(), 3);
    EXPECT_TRUE(application_manager.run());
}

TEST_F(ApplicationManagerFixture, ApplicationManager_run)
{
    auto& application_manager = get_application_manager()
        .add(std::make_shared<cli::ThrowTaskMock>());

    EXPECT_EQ(application_manager.size(), 1);
    EXPECT_THROW(application_manager.run(), std::runtime_error);
}

TEST_F(ApplicationManagerFixture, ApplicationManager_run_failure)
{
    auto& application_manager = get_application_manager()
        .add(std::make_shared<cli::FailureTaskMock>())
        .add(std::make_shared<cli::SuccessTaskMock>());

    EXPECT_EQ(application_manager.size(), 2);
    EXPECT_TRUE(!application_manager.run());
}
