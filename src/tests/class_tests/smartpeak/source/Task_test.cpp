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
#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE Task test suite
#include <boost/test/included/unit_test.hpp>

#include <SmartPeak/cli/ApplicationManager.h>
#include <SmartPeak/cli/Task.h>


struct TaskFixture 
{
    /* ctor/dtor */
    TaskFixture() 
    {
        // std::string seq = std::string{SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_sequence.csv")};
        std::string seq = std::string{SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files/sequence.csv")};
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

BOOST_FIXTURE_TEST_SUITE(Task, TaskFixture)

BOOST_AUTO_TEST_CASE(Task_InitializeApplicationSettings)
{
    namespace cli = SmartPeak::cli;
    {
        auto pa = cli::Parser{m_args};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto task = cli::InitializeApplicationSettings{};
        BOOST_CHECK(task(am));
    }
    {
        auto argv = m_args;
        argv[2] = "wrongvalue";
        auto pa = cli::Parser{argv};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto task = cli::InitializeApplicationSettings{};
        BOOST_CHECK_NO_THROW(task(am));
    }
    {
        auto argv = m_args;
        argv[2] = "wrongvalue";
        auto pa = cli::Parser{argv};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto task = cli::InitializeApplicationSettings{};
        BOOST_CHECK(!task(am));
    }
}

BOOST_AUTO_TEST_CASE(Task_InitializeLogger)
{
    namespace cli = SmartPeak::cli;
    {
        auto pa = cli::Parser{m_args};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto task = cli::InitializeLogger{};
        // Can't init twice:
        BOOST_CHECK(task(am));
        BOOST_CHECK(!task(am));
    }
}

BOOST_AUTO_TEST_CASE(Task_LoadSession)
{
    namespace cli = SmartPeak::cli;
    auto pa = cli::Parser{m_args};
    auto as = cli::ApplicationSettings{pa};
    auto am = cli::ApplicationManager{as};
    auto task = cli::LoadSession{};
    // Fails because InitializeApplicationSettings is not called
    BOOST_CHECK(!task(am));
}

BOOST_AUTO_TEST_CASE(Task_LoadSession_parameters_csv)
{
    namespace cli = SmartPeak::cli;
    auto param_file = std::string{SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv")};
    m_args.push_back("--param-file");
    m_args.push_back(param_file);
    auto pa = cli::Parser{m_args};
    auto as = cli::ApplicationSettings{pa};
    auto am = cli::ApplicationManager{as};
    auto init_app = cli::InitializeApplicationSettings{};
    BOOST_CHECK(init_app(am));

    auto task = cli::LoadSession{};
    BOOST_CHECK(task(am));
    BOOST_CHECK_EQUAL(am.get_application_handler().static_filenames_.parameters_csv_i, param_file);
}

BOOST_AUTO_TEST_CASE(Task_LoadSession_threads)
{
    namespace cli = SmartPeak::cli;
    auto n_threads = std::vector<int>{1,2,4,6,8};
    for (const auto& n_thread : n_threads)
    {
        auto args = m_args;
        auto n_thread_str = std::to_string(n_thread);
        args.push_back("--threads");
        args.push_back(n_thread_str);
        auto pa = cli::Parser{args};
        auto as = cli::ApplicationSettings{pa};
        auto am = cli::ApplicationManager{as};
        auto init_app = cli::InitializeApplicationSettings{};
        BOOST_CHECK(init_app(am));

        auto task = cli::LoadSession{};
        BOOST_CHECK(task(am));
        BOOST_CHECK_EQUAL(as.threads, n_thread);
        auto& params = am.get_application_handler().sequenceHandler_.getSequence().front().getRawData().getParameters();
        auto param = params.findParameter("SequenceProcessor", "n_thread");
        BOOST_CHECK(nullptr != param);
        BOOST_CHECK(param->isValid());
        BOOST_CHECK_EQUAL(param->getValueAsString(), n_thread_str);
    }
}

BOOST_AUTO_TEST_CASE(Task_LoadSession_param)
{
    namespace cli = SmartPeak::cli;
    
}

BOOST_AUTO_TEST_SUITE_END()
