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

#define BOOST_TEST_MODULE ApplicationSettings test suite
#include <boost/test/included/unit_test.hpp>
#include <algorithm>
#include <random>

#include <SmartPeak/cli/Parser.h>
#include <SmartPeak/cli/ApplicationSettings.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/core/Utilities.h>


struct ApplicationSettingsFixture 
{
    /* ctor/dtor */
    ApplicationSettingsFixture() 
    {
        const auto argc = 27;
        const char* argv[] = {
            "ApplicationSettings_test", 
                "--report", "featureDB", "pivottable",
                "--report-metadata", "PeAk_aRea", "Mz", "rt",
                "--report-sample-types", "stanDard", "Unknown", "double blank",
                "--load-session", "C:/SmartPeak/src/examples/data/GCMS_SIM_Unknowns/sequence.csv",
                "--verbose",
                "--allow-inconsistent",
                "--workflow", "load_raw_data", "EXTRACT_SPECTRA_WINDOWS", "pick_ms1_features" 
                              "SEARCH_ACCURATE_MASS", "LOAD_FeATURE_BACKGROUND_FILTERS",
                "--log-dir", ".",
                "--integrity", "SAMPLE", "is",
                "--disable-colors"
        };
        m_parser_ptr = std::make_shared<SmartPeak::cli::Parser>(argc, argv);
        m_application_settings_ptr = std::make_shared<SmartPeak::cli::ApplicationSettings>(*m_parser_ptr);
    }

    SmartPeak::cli::ApplicationSettings& get_application_settings() { return *m_application_settings_ptr; }
    const SmartPeak::cli::ApplicationSettings& get_application_settings() const { return *m_application_settings_ptr; }

    template<typename T>
    bool option_exists(const std::string& option)
    {
        try
        {
            m_parser_ptr->get<T>(option);
        }
        catch(const std::runtime_error& re)
        {
            return false;
        }
        return true;
    }

public:
    std::shared_ptr<SmartPeak::cli::Parser> m_parser_ptr;
    std::shared_ptr<SmartPeak::cli::ApplicationSettings> m_application_settings_ptr;
};

/* ---------------------------------------------- */

BOOST_FIXTURE_TEST_SUITE(ApplicationSettings, ApplicationSettingsFixture)

BOOST_AUTO_TEST_CASE(ApplicationSettings_define_options)
{
    auto& application_settings = get_application_settings();
    {
        application_settings.define_options();
    }
    BOOST_CHECK(option_exists<std::string>("l"));
    BOOST_CHECK(option_exists<std::vector<std::string>>("r"));
    BOOST_CHECK(option_exists<std::vector<std::string>>("rt"));
    BOOST_CHECK(option_exists<std::vector<std::string>>("rm"));
    BOOST_CHECK(option_exists<std::vector<std::string>>("w"));
    BOOST_CHECK(option_exists<std::vector<std::string>>("i"));
    BOOST_CHECK(option_exists<bool>("a"));
    BOOST_CHECK(option_exists<bool>("v"));
    BOOST_CHECK(option_exists<bool>("d"));
    BOOST_CHECK(option_exists<std::string>("ld"));
}

BOOST_AUTO_TEST_CASE(ApplicationSettings_load_options)
{
    auto& application_settings = get_application_settings();
    {
        application_settings.define_options();
        application_settings.load_options();
    }
    BOOST_CHECK_EQUAL(application_settings.load_session, 
        "C:/SmartPeak/src/examples/data/GCMS_SIM_Unknowns/sequence.csv");
    auto report = std::vector<std::string>{"featureDB", "pivottable"};
    BOOST_CHECK(application_settings.report == report);
    auto metadata = std::vector<std::string>{"PeAk_aRea", "Mz", "rt"};
    BOOST_CHECK(application_settings.report_metadata == metadata);
    auto samples = std::vector<std::string>{"stanDard", "Unknown", "double blank"};
    BOOST_CHECK(application_settings.report_sample_types == samples);
    BOOST_CHECK(application_settings.verbose);
    BOOST_CHECK(application_settings.disable_colors);
    BOOST_CHECK(application_settings.allow_inconsistent);
    auto workflow = std::vector<std::string>{
        "load_raw_data", "EXTRACT_SPECTRA_WINDOWS", "pick_ms1_features" 
        "SEARCH_ACCURATE_MASS", "LOAD_FeATURE_BACKGROUND_FILTERS" };
    BOOST_CHECK(application_settings.workflow == workflow);
    BOOST_CHECK_EQUAL(application_settings.log_dir, ".");
    auto integrity = std::vector<std::string>{"SAMPLE", "is"};
    BOOST_CHECK(application_settings.integrity == integrity);
}

BOOST_AUTO_TEST_CASE(ApplicationSettings_process_options)
{
    auto& application_settings = get_application_settings();
    auto& parser = application_settings.get_parser();
    {
        application_settings.define_options();
        application_settings.load_options();
        application_settings.process_options();
    }
    auto report = std::vector<std::string>{"FEATUREDB", "PIVOTTABLE"};
    BOOST_CHECK(application_settings.report == report);
    auto metadata = std::vector<std::string>{"PEAK_AREA", "MZ", "RT"};
    BOOST_CHECK(application_settings.report_metadata == metadata);
    auto samples = std::vector<std::string>{"STANDARD", "UNKNOWN", "DOUBLE BLANK"};
    BOOST_CHECK(application_settings.report_sample_types == samples);
    auto workflow = std::vector<std::string>{
        "LOAD_RAW_DATA", "EXTRACT_SPECTRA_WINDOWS", "PICK_MS1_FEATURES"
        "SEARCH_ACCURATE_MASS", "LOAD_FEATURE_BACKGROUND_FILTERS" };
    BOOST_CHECK(application_settings.workflow == workflow);
    auto integrity = std::vector<std::string>{"SAMPLE", "IS"};
    BOOST_CHECK(application_settings.integrity == integrity);
}

BOOST_AUTO_TEST_CASE(ApplicationSettings_contains_option)
{
    using SmartPeak::cli::ApplicationSettings;
    auto list = std::vector<std::string>{"string1", "string2", "string3"};
    BOOST_CHECK(ApplicationSettings::contains_option(list, "string1", "loginfo"));
    BOOST_CHECK(!ApplicationSettings::contains_option(list, "string4", "loginfo"));
}

BOOST_AUTO_TEST_CASE(ApplicationSettings_validate_report)
{
    auto& application_settings = get_application_settings();
    application_settings.report = std::vector<std::string>{"FEATUREDB"};
    BOOST_CHECK_NO_THROW(application_settings.validate_report());
    application_settings.report = std::vector<std::string>{"PIVOTTABLE"};
    BOOST_CHECK_NO_THROW(application_settings.validate_report());
    application_settings.report = std::vector<std::string>{"FEATUREDB", "PIVOTTABLE"};
    BOOST_CHECK_NO_THROW(application_settings.validate_report());
    application_settings.report = std::vector<std::string>{"ALL"};
    BOOST_CHECK_NO_THROW(application_settings.validate_report());
    application_settings.report = std::vector<std::string>{"aksdng"};
    BOOST_CHECK_THROW(application_settings.validate_report(), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ApplicationSettings_validate_report_sample_types)
{
    auto& application_settings = get_application_settings();
    auto& available_types = SmartPeak::sampleTypeToString;
    auto values = std::vector<std::string>{ "ALL" };
    std::for_each(available_types.cbegin(), available_types.cend(), 
        [&values, &application_settings](const auto& t) { 
            auto str = SmartPeak::Utilities::str2upper(t.second);
            values.push_back(str);
            application_settings.report_sample_types = std::vector<std::string>{ str };
            BOOST_CHECK_NO_THROW(application_settings.validate_report_sample_types());
            application_settings.report_sample_types = values;
            auto& rst = application_settings.report_sample_types;
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(rst.begin(), rst.end(), g);
            BOOST_CHECK_NO_THROW(application_settings.validate_report_sample_types());
        });
    application_settings.report_sample_types = std::vector<std::string>{"aksdng", "Standard"};
    BOOST_CHECK_THROW(application_settings.validate_report_sample_types(), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ApplicationSettings_validate_report_metadata)
{
    auto& application_settings = get_application_settings();
    auto& available = SmartPeak::metadataToString;
    auto values = std::vector<std::string>{ "ALL" };
    std::for_each(available.cbegin(), available.cend(), 
        [&values, &application_settings](const auto& t) { 
            auto str = SmartPeak::Utilities::str2upper(t.second);
            values.push_back(str);
            application_settings.report_metadata = std::vector<std::string>{ str };
            BOOST_CHECK_NO_THROW(application_settings.validate_report_metadata());
            application_settings.report_metadata = values;
            auto& rm = application_settings.report_metadata;
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(rm.begin(), rm.end(), g);
            BOOST_CHECK_NO_THROW(application_settings.validate_report_metadata());
        });
    application_settings.report_metadata = std::vector<std::string>{"aksdng", "Standard"};
    BOOST_CHECK_THROW(application_settings.validate_report_metadata(), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ApplicationSettings_validate_workflow)
{
    auto& application_settings = get_application_settings();
    auto cmds_ = std::vector<std::string>{};
    {
        auto& cmds1 = SmartPeak::n_to_raw_data_method_;
        auto& cmds2 = SmartPeak::n_to_seq_seg_method_;
        auto& cmds3 = SmartPeak::n_to_sample_group_method_;
        std::transform(cmds1.cbegin(), cmds1.cend(), std::back_inserter(cmds_), [](const auto& cmd) { return cmd.first; });
        std::transform(cmds2.cbegin(), cmds2.cend(), std::back_inserter(cmds_), [](const auto& cmd) { return cmd.first; });
        std::transform(cmds3.cbegin(), cmds3.cend(), std::back_inserter(cmds_), [](const auto& cmd) { return cmd.first; });
    }
    auto values = std::vector<std::string>{};
    std::for_each(cmds_.cbegin(), cmds_.cend(), 
        [&values, &application_settings](const auto& t){
            application_settings.workflow = std::vector<std::string>{ t };
            BOOST_CHECK_NO_THROW(application_settings.validate_workflow());
            values.push_back(t);
            application_settings.workflow = values;
            auto& w = application_settings.workflow;
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(w.begin(), w.end(), g);
            BOOST_CHECK_NO_THROW(application_settings.validate_workflow());
        });
    application_settings.workflow = std::vector<std::string>{"ASDFG", "LOAD_RAW_DATA"};
    BOOST_CHECK_THROW(application_settings.validate_workflow(), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ApplicationSettings_validate_integrity)
{
    auto& application_settings = get_application_settings();
    auto options = std::vector<std::string>{"SAMPLE", "COMP", "COMP_GROUP", "IS", "ALL", "NONE"};
    auto values = std::vector<std::string>{};
    std::for_each(options.cbegin(), options.cend(), 
        [&values, &application_settings](const auto& t) {
            application_settings.integrity = std::vector<std::string>{ t };
            BOOST_CHECK_NO_THROW(application_settings.validate_integrity());
            values.push_back(t);
            application_settings.integrity = values;
            auto& i = application_settings.integrity;
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(i.begin(), i.end(), g);
            BOOST_CHECK_NO_THROW(application_settings.validate_integrity());
        });
    application_settings.integrity = std::vector<std::string>{"ASDFG", "COMP_GROUP"};
    BOOST_CHECK_THROW(application_settings.validate_integrity(), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
