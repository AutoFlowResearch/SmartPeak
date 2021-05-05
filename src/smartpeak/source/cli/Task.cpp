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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------
#include <SmartPeak/cli/Task.h>

#include <SmartPeak/core/ApplicationProcessor.h>
#include <boost/filesystem.hpp>


namespace SmartPeak {
namespace cli {

bool InitializeApplicationSettings::operator() (ApplicationManager& application_manager) 
{
    auto& application_settings = application_manager.get_application_settings();
    try
    {
        application_settings.define_options();
        application_settings.load_options();
        application_settings.process_options();
        application_settings.validate();
    }
    catch(const std::invalid_argument& ia)
    {
        std::cerr << ia.what() << std::endl;
        std::cerr << application_settings.usage() << std::endl;
        return false;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

bool InitializeLogger::operator() (ApplicationManager& application_manager) 
{
    auto& application_settings = application_manager.get_application_settings();
    try
    {
        auto& console_handler = ConsoleHandler::get_instance();
        console_handler.set_log_directory(application_settings.log_dir);
        console_handler.use_colors(!application_settings.disable_colors);
        console_handler.set_severity(application_settings.verbose ? plog::debug : plog::info);
        console_handler.initialize("Start SmartPeak version " + SmartPeak::Utilities::getSmartPeakVersion());
        if (application_settings.verbose) LOG_DEBUG << "Verbose mode ON";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

bool LoadSession::operator() (ApplicationManager& application_manager) 
{
    auto& application_settings = application_manager.get_application_settings();
    auto& application_handler = application_manager.get_application_handler();
    LoadSessionFromSequence load_session_processor{application_handler};
    {
        load_session_processor.pathname_ = application_settings.load_session;
    }
    return load_session_processor.process();
}

bool RunIntegrityChecks::operator() (ApplicationManager& application_manager) 
{
    auto& application_settings = application_manager.get_application_settings();
    auto& application_handler = application_manager.get_application_handler();
    try
    {
        auto& integrity_args = application_settings.integrity;
        auto int_msg = std::string{"Detected integrity option: "};
        auto no_integrity_check = ApplicationSettings::contains_option(integrity_args, "NONE", int_msg);
        auto all_integrity_check = ApplicationSettings::contains_option(integrity_args, "ALL", int_msg);

        if (no_integrity_check)
        {
            LOG_WARNING << "No integrity check are run, continue with the workflow";
        }
        else
        {
            if (all_integrity_check || ApplicationSettings::contains_option(integrity_args, "SAMPLE", int_msg))
            {
                handle_integrity_check(
                    run_integrity_check(
                        application_handler.sequenceHandler_, 
                        SmartPeak::InputDataValidation::sampleNamesAreConsistent,
                        "Run sample integrity check"),
                    application_settings.allow_inconsistent, 
                    "Sample integrity check failed!");
            }
            if (all_integrity_check || ApplicationSettings::contains_option(integrity_args, "COMP", int_msg))
            {
                handle_integrity_check(
                    run_integrity_check(
                        application_handler.sequenceHandler_, 
                        SmartPeak::InputDataValidation::componentNamesAreConsistent,
                        "Run compound integrity check"),
                    application_settings.allow_inconsistent,
                    "Compound integrity check failed!");
            }
            if (all_integrity_check || ApplicationSettings::contains_option(integrity_args, "COMP_GROUP", int_msg))
            {
                handle_integrity_check(
                    run_integrity_check(
                        application_handler.sequenceHandler_, 
                        SmartPeak::InputDataValidation::componentNameGroupsAreConsistent,
                        "Run compound group integrity check"),
                    application_settings.allow_inconsistent,
                    "Compound group integrity check failed!");
            }
            if (all_integrity_check || ApplicationSettings::contains_option(integrity_args, "IS", int_msg))
            {
                handle_integrity_check(
                    run_integrity_check(
                        application_handler.sequenceHandler_, 
                        SmartPeak::InputDataValidation::heavyComponentsAreConsistent,
                        "Run IS integrity check"),
                    application_settings.allow_inconsistent,
                    "IS integrity check failed!");
            }
        }
    }
    catch (std::runtime_error& re)
    {
        LOG_ERROR << re.what();
        return false;
    }
    return true;
}

void RunIntegrityChecks::handle_integrity_check(
    bool check, bool allow_inconsistent, std::string msg) 
{
    // Fail the pipeline if integrity check fails and allow-inconsistent is false:   
    if (!check && !allow_inconsistent)
    {
        throw std::runtime_error(msg);
    }
    // Allow to continue the pipeline if integrity check fails and allow-inconsistent is true: 
    else if (!check && allow_inconsistent)
    {
        LOG_WARNING << msg << " Continue the workflow with --allow-inconsistent set.";
    }
    if (check)
    {
        LOG_INFO << "Integrity check succeeded";
    }
}

bool RunIntegrityChecks::run_integrity_check(
    const SmartPeak::SequenceHandler& sequence_handler, 
    std::function<bool(const SmartPeak::SequenceHandler&)> f,
    const std::string& info_msg)
{
    LOG_INFO << info_msg;
    return f(sequence_handler);
}

bool InitializeWorkflowResources::operator() (ApplicationManager& application_manager) 
{
    namespace fs = boost::filesystem;
    auto& application_settings = application_manager.get_application_settings();
    auto& application_handler = application_manager.get_application_handler();

    // Initialize directories:
    auto main_dir = application_handler.main_dir_;
    if (!application_settings.out_dir.empty() && application_settings.out_dir != ".")
    {
        main_dir = application_settings.out_dir;
        LOG_DEBUG << "Output feature directory: " << main_dir;
    }
    application_handler.mzML_dir_           = application_handler.main_dir_ + "/mzML";
    application_handler.features_in_dir_    = application_handler.main_dir_ + "/features";
    application_handler.features_out_dir_   = main_dir + "/features";

    auto paths = {
        application_handler.mzML_dir_, 
        application_handler.features_in_dir_, 
        application_handler.features_out_dir_ 
    };
    auto current_path = std::string{};
    try
    {
        for (const std::string& pathname : paths) 
        {
            current_path = pathname;
            fs::create_directories(fs::path(pathname));
        }
    }
    catch (fs::filesystem_error& fe)
    {
        if (fe.code() == boost::system::errc::permission_denied)
            LOG_ERROR << static_cast<std::ostringstream&&>(
                std::ostringstream() 
                    << "Unable to create output directory, permission denied: '" << current_path << "'").str();
        else
            LOG_ERROR << static_cast<std::ostringstream&&>(
                std::ostringstream()
                    << "Unable to create output directory: '" << current_path << "'").str();
        return false;
    }
    return true;
}

bool InitializeWorkflowSettings::operator() (ApplicationManager& application_manager) 
{
    auto& application_settings = application_manager.get_application_settings();
    auto& application_handler = application_manager.get_application_handler();
    BuildCommandsFromNames buildCommandsFromNames{application_handler};
    {
        buildCommandsFromNames.names_ = application_handler.sequenceHandler_.getWorkflow();
        if (!application_settings.workflow.empty())
        {
            // Override workflow given by cli option:
            buildCommandsFromNames.names_ = application_settings.workflow;
            auto workflow_string = std::string{};
            for (const auto& cmd : buildCommandsFromNames.names_) workflow_string += "\t" + cmd + " \n";
            LOG_WARNING << "Override workflow with: \n" << workflow_string;
        }
    }
    if (!buildCommandsFromNames.process())
    {
        LOG_ERROR << "Failed to create workflow commands, abort.";
        return false;
    }
    application_manager.set_workflow_commands(buildCommandsFromNames.commands_);
    _update_filenames(buildCommandsFromNames, application_handler);
    return true;
}

void InitializeWorkflowSettings::_update_filenames(
    SmartPeak::BuildCommandsFromNames& buildCommandsFromNames,
    SmartPeak::ApplicationHandler& application_handler)
{
    for (auto& cmd : buildCommandsFromNames.commands_)
    {
        for (auto& p : cmd.dynamic_filenames)
        {
            SmartPeak::Filenames::updateDefaultDynamicFilenames(
                application_handler.mzML_dir_,
                application_handler.features_in_dir_,
                application_handler.features_out_dir_,
                p.second
            );
        }
    }
}

bool RunWorkflow::operator() (ApplicationManager& application_manager) 
{
    auto& application_handler = application_manager.get_application_handler();
    auto& session_handler = application_manager.get_session_handler();
    auto& workflow_manager = application_manager.get_workflow_manager();
    try
    {
        const auto injection_names = session_handler.getSelectInjectionNamesWorkflow(
            application_handler.sequenceHandler_);
        const auto sequence_segment_names = session_handler.getSelectSequenceSegmentNamesWorkflow(
            application_handler.sequenceHandler_);
        const auto sample_group_names = session_handler.getSelectSampleGroupNamesWorkflow(
            application_handler.sequenceHandler_);

        workflow_manager.addWorkflow(
            application_handler, injection_names, sequence_segment_names, 
            sample_group_names, application_manager.get_workflow_commands(), true);
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << e.what();
        return false;
    }
    return true;
}

bool ExportReport::operator() (ApplicationManager& application_manager) 
{
    auto& application_settings = application_manager.get_application_settings();
    auto& application_handler = application_manager.get_application_handler();
    try
    {
        auto all_reports = ApplicationSettings::contains_option(
            application_settings.report, "ALL", "Detected report option: ");
        auto feature_db = all_reports || ApplicationSettings::contains_option(
            application_settings.report, "FEATUREDB", "Detected report option: ");
        auto pivot_table = all_reports || ApplicationSettings::contains_option(
            application_settings.report, "PIVOTTABLE", "Detected report option: ");

        auto report_sample_types = std::set<SampleType>{};
        auto report_metadata = std::vector<FeatureMetadata>{};
        _extract_report_sampletypes(application_settings, report_sample_types);
        _extract_report_metadata(application_settings, report_metadata);

        auto main_dir = application_handler.main_dir_;
        if (!application_settings.out_dir.empty() && application_settings.out_dir != ".")
        {
            main_dir = application_settings.out_dir;
        }

        if (feature_db)
        {
            auto& sequance_handler = application_handler.sequenceHandler_;
            const auto filepath = main_dir + "/FeatureDB.csv";
            SequenceParser::writeDataTableFromMetaValue(
                sequance_handler, filepath, 
                report_metadata, report_sample_types);
        }
        if (pivot_table)
        {
            auto& sequance_handler = application_handler.sequenceHandler_;
            const auto filepath = main_dir + "/PivotTable.csv";
            SequenceParser::writeDataMatrixFromMetaValue(
                sequance_handler, filepath, 
                report_metadata, report_sample_types);
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << e.what();
        return false;
    }
    return true;
}

void ExportReport::_extract_report_sampletypes(
    const ApplicationSettings& application_settings, 
    std::set<SampleType>& report_sample_types)
{
    auto& available_types = SmartPeak::sampleTypeToString;
    auto& types = application_settings.report_sample_types;
    if (ApplicationSettings::contains_option(
            types, "ALL", "Detected report-sample-types option: "))
    {
        // If no sample-types options are selected, use all features:
        std::transform(available_types.cbegin(), available_types.cend(), 
            std::inserter(report_sample_types, report_sample_types.begin()), 
                [](const auto& m){ return m.first; });
    }
    else
    {
        // If selected sample types are specified, validate and construct a vector:
        std::for_each(types.cbegin(), types.cend(), 
            [&available_types, &report_sample_types, this](const auto& type_prop)
            {
                auto it = std::find_if(available_types.cbegin(), available_types.cend(), 
                    [&type_prop](const auto& t) { return type_prop == Utilities::str2upper(t.second); });
                if (it != std::cend(available_types))
                {
                    LOG_DEBUG << "Detected report-sample-types option: " << it->second;
                    report_sample_types.insert(it->first);
                }
            });
    }
}

void ExportReport::_extract_report_metadata(
    const ApplicationSettings& application_settings, 
    std::vector<FeatureMetadata>& report_metadata)
{
    auto& available_metadata = SmartPeak::metadataToString;
    auto& meta = application_settings.report_metadata;
    if (ApplicationSettings::contains_option(
            meta, "ALL", "Detected report-metadata option: "))
    {
        // If no sample-types options are selected, use all features:
        std::transform(available_metadata.cbegin(), available_metadata.cend(), 
            std::back_inserter(report_metadata), [](const auto& m){ return m.first; });
    }
    else
    {
        // If selected sample types are specified, validate and construct a vector:
        std::for_each(meta.cbegin(), meta.cend(), 
            [&available_metadata, &report_metadata, this](const auto& meta_prop)
            {
                auto it = std::find_if(available_metadata.cbegin(), available_metadata.cend(), 
                    [&meta_prop](const auto& t) { return meta_prop == Utilities::str2upper(t.second); });
                if (it != std::cend(available_metadata))
                {
                    LOG_DEBUG << "Detected report-metadata option: " << it->second;
                    report_metadata.push_back(it->first);
                }
            });
    }
}

} /* namespace cli */
} /* namespace SmartPeak */