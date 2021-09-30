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
#include <SmartPeak/cli/ApplicationSettings.h>

#include <memory>
#include <plog/Log.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SharedProcessors.h>


namespace SmartPeak {
namespace cli {

void ApplicationSettings::define_options()
{
    m_parser.set_required<std::string>("l", "load-session", "The path to session file, or a directory with structured file organization (prefer session file)");
    m_parser.set_optional<std::vector<std::string>>("r", "report", std::vector<std::string>{ "FeatureDB", "PivotTable" }, 
        "Specify a report type to export.");
    m_parser.set_optional<std::vector<std::string>>("rt", "report-sample-types", std::vector<std::string>{ "ALL" }, 
        "A list of sample types to include in the report. "
        "The full list can be found in documentation https://smartpeak.readthedocs.io");
    m_parser.set_optional<std::vector<std::string>>("rm", "report-metadata", std::vector<std::string>{ "ALL" }, 
        "A list of metadata to include in the report. "
        "The full list can be found in documentation https://smartpeak.readthedocs.io");
    m_parser.set_optional<std::vector<std::string>>("w", "workflow", std::vector<std::string>{}, 
        "The workflow sequence as a list of commands, e.g. LOAD_DATA MAP_CHROMATOGRAMS ... "
        "Overrides the workflow settings loaded from the sequence file (with option --load-session)");
    m_parser.set_optional<std::vector<std::string>>("t", "integrity", std::vector<std::string>{ "NONE" }, 
        "Specify which integrity checks to run, available are: SAMPLE, COMP, COMP_GROUP, IS and ALL (runs all listed).");
    m_parser.set_optional<bool>("a", "allow-inconsistent", false, 
        "Given that any integrity checks were specified with '--integrity', "
        "this option allow to run workflow even if one or more check has failed.");
    m_parser.set_optional<bool>("v", "verbose", false, 
        "Run SmartPeak in verbose mode, display more detailed information");
    m_parser.set_optional<bool>("d", "disable-colors", false, 
        "By default the console output is colored, this flag disables colors.");
    m_parser.set_optional<bool>("pg", "disable-progressbar", false, 
        "Progress bar allows to track the progress of the entire workflow. This option disables the progress bar.");
    m_parser.set_optional<std::string>("ld", "log-dir", "", 
        "The path to the log directory. Given directory has to exist. Overrides the default location for the log file: "
        "https://smartpeak.readthedocs.io/en/latest/guide/guistart.html#logs");
    m_parser.set_optional<std::string>("o", "output-features", "./features", 
        "An absolute or relative path to an output directory. Overrides the default location which is the current working directory. " 
        "SmartPeak will create given directory if one does not exist.");
    m_parser.set_optional<std::string>("i", "input-features", "./features",
        "An absolute or relative path to the input features directory. Overrides the default location which is the current working directory. ");
    m_parser.set_optional<std::string>("z", "mzml", "./mzML",
        "An absolute or relative path to the mzML directory. Overrides the default location which is the mzML folder under the current working directory. "
        "SmartPeak will create given directory if one does not exist.");
    m_parser.set_optional<std::string>("f", "input-files", "",
        "Override input files. Ex: -f featureQCComponents=\"./featureQCComponents_new.csv;traML=./traML2.csv\".");
    m_parser.set_optional<std::string>("p", "parameters", "",
        "Override parameters. Ex: '-p SequenceProcessor:n_thread=8;MRMFeatureFinderScoring:TransitionGroupPicker:peak_integration=smoothed'.");
    m_parser.run_and_exit_if_error();
}

void ApplicationSettings::load_options()
{
    load_session            = m_parser.get<std::string>("l");
    report                  = m_parser.get<std::vector<std::string>>("r");
    report_sample_types     = m_parser.get<std::vector<std::string>>("rt");
    report_metadata         = m_parser.get<std::vector<std::string>>("rm");
    workflow                = m_parser.get<std::vector<std::string>>("w");
    integrity               = m_parser.get<std::vector<std::string>>("t");
    allow_inconsistent      = m_parser.get<bool>("a");
    verbose                 = m_parser.get<bool>("v");
    disable_colors          = m_parser.get<bool>("d");
    disable_progressbar     = m_parser.get<bool>("pg");
    log_dir                 = m_parser.get<std::string>("ld");
    features_out_dir        = m_parser.get<std::string>("o");
    features_in_dir         = m_parser.get<std::string>("i");
    input_files             = m_parser.get<std::string>("f");
    parameters              = m_parser.get<std::string>("p");
    mzml_dir                = m_parser.get<std::string>("z");
}

void ApplicationSettings::process_options()
{
    std::transform(report.begin(), report.end(), report.begin(), Utilities::str2upper);
    std::transform(report_sample_types.begin(), report_sample_types.end(), report_sample_types.begin(), Utilities::str2upper);
    std::transform(report_metadata.begin(), report_metadata.end(), report_metadata.begin(), Utilities::str2upper);
    std::transform(workflow.begin(), workflow.end(), workflow.begin(), Utilities::str2upper);
    std::transform(integrity.begin(), integrity.end(), integrity.begin(), Utilities::str2upper);
}

void ApplicationSettings::validate() const
{
    validate_report();
    validate_report_sample_types();
    validate_report_metadata();
    validate_workflow();
    validate_integrity();
}

bool ApplicationSettings::contains_option(
    const std::vector<std::string>& list, 
    const std::string& option, 
    std::string log_msg)
{
    auto flag = false;
    if (1 <= list.size())
    {
        auto it = std::find(list.cbegin(), list.cend(), option);
        if (it != std::end(list))
        {
            flag = true;
            LOG_DEBUG << log_msg << option;
        }
    }
    return flag;
}

std::map<std::string, std::string> ApplicationSettings::get_split_option(
  const std::string& option)
{
  std::string escaped_option = option;
  if ((option.size() > 1) && (option.front() == '\"') && (option.back() == '\"'))
  {
    escaped_option = option.substr(1, option.size() - 2);
  }
  std::map<std::string, std::string> map_options;
  std::istringstream iss(escaped_option);
  std::string key_value;
  while (std::getline(iss, key_value, ';'))
  {
    auto separator_pos = key_value.find("=");
    if (separator_pos != key_value.npos)
    {
      std::string key = key_value.substr(0, separator_pos);
      std::string value = key_value.substr((separator_pos + 1), key_value.size() - (separator_pos + 1));
      if (!key.empty())
      {
        map_options.emplace(key, value);
      }
    }
  };
  return map_options;
}

void ApplicationSettings::validate_report() const
{
    auto options = std::vector<std::string>{"FEATUREDB", "PIVOTTABLE", "ALL"};
    std::for_each(report.cbegin(), report.cend(), [&options](const auto& option) {
        if (std::find(options.cbegin(), options.cend(), option) == std::cend(options))
        {
            auto options_str = Utilities::join(options.cbegin(), options.cend(), ", ");
            throw std::invalid_argument(static_cast<std::ostringstream&&>(
                std::ostringstream{}
                    << "Invalid value for 'report', available options are: { " 
                    << options_str << " }").str());
        }
    });
}

void ApplicationSettings::validate_report_sample_types() const
{
    auto& types = report_sample_types;
    auto& available_types = SmartPeak::sampleTypeToString;
    auto values = std::vector<std::string>{ "ALL" };
    std::for_each(available_types.begin(), available_types.end(), 
        [&values](auto& t) { 
            values.push_back(Utilities::str2upper(t.second));
        });

    std::for_each(types.cbegin(), types.cend(), 
        [&values](const auto& t) {
            auto it = std::find(values.cbegin(), values.cend(), t);
            if (it == std::cend(values))
            {
                throw std::invalid_argument("Given report-sample-types option is incorrect");
            }
        });
}

void ApplicationSettings::validate_report_metadata() const
{
    auto& meta = report_metadata;
    auto& available_metadata = SmartPeak::metadataToString;

    auto values = std::vector<std::string>{ "ALL" };
    std::for_each(available_metadata.begin(), available_metadata.end(), 
        [&values](auto& t) { 
            values.push_back(Utilities::str2upper(t.second));
        });

    std::for_each(meta.cbegin(), meta.cend(), 
        [&values](const auto& t) {
            auto it = std::find(values.cbegin(), values.cend(), t);
            if (it == std::cend(values))
            {
                throw std::invalid_argument("Given report-metadata option is incorrect");
            }
        });
}

void ApplicationSettings::validate_workflow() const 
{
    auto cmds_ = std::vector<std::string>{};
    auto& cmds1 = SmartPeak::n_to_raw_data_method_;
    auto& cmds2 = SmartPeak::n_to_seq_seg_method_;
    auto& cmds3 = SmartPeak::n_to_sample_group_method_;
    std::transform(cmds1.cbegin(), cmds1.cend(), std::back_inserter(cmds_), [](const auto& cmd) { return cmd.first; });
    std::transform(cmds2.cbegin(), cmds2.cend(), std::back_inserter(cmds_), [](const auto& cmd) { return cmd.first; });
    std::transform(cmds3.cbegin(), cmds3.cend(), std::back_inserter(cmds_), [](const auto& cmd) { return cmd.first; });

    std::for_each(workflow.cbegin(), workflow.cend(), [&cmds_](const auto& option) {
        auto it = std::find(cmds_.cbegin(), cmds_.cend(), option);
        if (it == std::cend(cmds_))
        {
            throw std::invalid_argument("Invalid value for 'workflow', make sure that you specify correct command name");
        }
    });
}

void ApplicationSettings::validate_integrity() const
{
    auto options = std::vector<std::string>{"SAMPLE", "COMP", "COMP_GROUP", "IS", "ALL", "NONE"};
    std::for_each(integrity.cbegin(), integrity.cend(), [&options, this](const auto& option) {
        auto it = std::find(options.cbegin(), options.cend(), option);
        if (it == std::cend(options))
        {
            auto options_str = Utilities::join(options.cbegin(), options.cend(), ", ");
            throw std::invalid_argument(static_cast<std::ostringstream&&>(
                std::ostringstream{}
                    << "Invalid value for 'integrity', available options are: { " 
                    << options_str << " }").str());
        }
    });
}

} /* namespace cli */
} /* namespace SmartPeak */
