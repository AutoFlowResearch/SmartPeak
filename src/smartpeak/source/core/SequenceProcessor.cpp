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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/io/CSVWriter.h>
#include <plog/Log.h>
#include <atomic>
#include <future>
#include <list>

namespace SmartPeak
{
  void CreateSequence::process()
  {
    LOGD << "START createSequence";

    SequenceParser::readSequenceFile(*sequenceHandler_IO, filenames_.sequence_csv_i, delimiter);
    if (sequenceHandler_IO->getSequence().empty()) {
      LOGE << "Empty sequence. Returning";
      LOGD << "END createSequence";
      return;
    }

    // load workflow
    LoadWorkflow loadWorkflow(*sequenceHandler_IO);
    loadWorkflow.filename_ = filenames_.workflow_csv_i;
    loadWorkflow.process();

    // TODO: Given that the raw data is shared between all injections, it could
    // be beneficial to move it somewhere else (i.e. in SequenceHandler) and adapt
    // the algorithms to the change
    RawDataHandler& rawDataHandler = sequenceHandler_IO->getSequence()[0].getRawData();

    // load rawDataHandler files (applies to the whole session)
    LoadParameters loadParameters;
    loadParameters.parameters_observable_ = sequenceHandler_IO;
    loadParameters.process(rawDataHandler, {}, filenames_);
    LoadTransitions loadTransitions;
    loadTransitions.transitions_observable_ = sequenceHandler_IO;
    loadTransitions.process(rawDataHandler, {}, filenames_);
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)
    LoadValidationData loadValidationData;
    loadValidationData.process(rawDataHandler, {}, filenames_);
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)

    // load sequenceSegmentHandler files
    for (SequenceSegmentHandler& sequenceSegmentHandler: sequenceHandler_IO->getSequenceSegments()) {
      LoadQuantitationMethods loadQuantitationMethods;
      loadQuantitationMethods.sequence_segment_observable_ = sequenceHandler_IO;
      loadQuantitationMethods.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadStandardsConcentrations loadStandardsConcentrations;
      loadStandardsConcentrations.sequence_segment_observable_ = sequenceHandler_IO;
      loadStandardsConcentrations.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureFilters loadFeatureFilters;
      loadFeatureFilters.sequence_segment_observable_ = sequenceHandler_IO;
      loadFeatureFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureQCs loadFeatureQCs;
      loadFeatureQCs.sequence_segment_observable_ = sequenceHandler_IO;
      loadFeatureQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureRSDFilters loadFeatureRSDFilters;
      loadFeatureRSDFilters.sequence_segment_observable_ = sequenceHandler_IO;
      loadFeatureRSDFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureRSDQCs loadFeatureRSDQCs;
      loadFeatureRSDQCs.sequence_segment_observable_ = sequenceHandler_IO;
      loadFeatureRSDQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureBackgroundFilters loadFeatureBackgroundFilters;
      loadFeatureBackgroundFilters.sequence_segment_observable_ = sequenceHandler_IO;
      loadFeatureBackgroundFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureBackgroundQCs loadFeatureBackgroundQCs;
      loadFeatureBackgroundQCs.sequence_segment_observable_ = sequenceHandler_IO;
      loadFeatureBackgroundQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
    }

    if (checkConsistency) {
      InputDataValidation::sampleNamesAreConsistent(*sequenceHandler_IO);
      InputDataValidation::componentNamesAreConsistent(*sequenceHandler_IO);
      InputDataValidation::componentNameGroupsAreConsistent(*sequenceHandler_IO);
      InputDataValidation::heavyComponentsAreConsistent(*sequenceHandler_IO);
    }

    sequenceHandler_IO->notifySequenceUpdated();
    LOGD << "END createSequence";
  }

  ParameterSet ProcessSequence::getParameterSchemaStatic()
  {
    std::map<std::string, std::vector<std::map<std::string, std::string>>> param_struct({
    {"SequenceProcessor", {
      {
        {"name", "n_thread"},
        {"type", "int"},
        {"value", "6"},
        {"description", "number of working threads"},
        {"min", "1"}
      }
    }} });
    return ParameterSet(param_struct);
  }

  ParameterSet ProcessSequence::getParameterSchema() const
  {
    return ProcessSequence::getParameterSchemaStatic();
  }

  void ProcessSequence::process()
  {
    // Check that there are raw data processing methods
    if (raw_data_processing_methods_.empty()) {
      throw "no raw data processing methods given.\n";
    }

    // Get the specified injections to process
    std::vector<InjectionHandler> injections = injection_names_.empty()
      ? sequenceHandler_IO->getSequence()
      : sequenceHandler_IO->getSamplesInSequence(injection_names_);

    // Check that the filenames is consistent with the number of injections
    if (filenames_.size() < injections.size()) {
      throw std::invalid_argument("The number of provided filenames locations is not correct.");
    }

    notifySequenceProcessorStart(injections.size());

    // Determine the number of threads to launch
    int n_threads = 6;
    if (injections.size())
    {
      const auto& params = injections.front().getRawData().getParameters();
      if (params.count("SequenceProcessor") && !params.at("SequenceProcessor").empty()) {
        for (const auto& p : params.at("SequenceProcessor")) {
          if (p.getName() == "n_thread") {
            try {
              n_threads = std::stoi(p.getValueAsString());
              LOGI << "n_threads set to " << n_threads;
            }
            catch (const std::exception& e) {
              LOGE << e.what();
            }
          }
        }
      }
    }

    // Launch
    SequenceProcessorMultithread manager(
      injections,
      filenames_,
      raw_data_processing_methods_,
      this
    );
    manager.spawn_workers(n_threads);
    notifySequenceProcessorEnd();
  }

  void ProcessSequenceSegments::process()
  {
    std::vector<SequenceSegmentHandler> sequence_segments;

    if (sequence_segment_names_.empty()) { // select all
      sequence_segments = sequenceHandler_IO->getSequenceSegments();
    } else { // select those with specific sequence segment names
      for (SequenceSegmentHandler& s : sequenceHandler_IO->getSequenceSegments()) {
        if (sequence_segment_names_.count(s.getSequenceSegmentName())) {
          sequence_segments.push_back(s);
        }
      }
    }
    if (filenames_.size() < sequence_segments.size()) {
      throw std::invalid_argument("The number of provided filenames_ locations is not correct.");
    }

    notifySequenceSegmentProcessorStart(sequence_segments.size());

    // process by sequence segment
    for (SequenceSegmentHandler& sequence_segment : sequence_segments) {

      // handle user-desired sequence_segment_processing_methods
      if (!sequence_segment_processing_methods_.size()) {
        throw "no sequence segment processing methods given.\n";
      }

      notifySequenceSegmentProcessorSampleStart(sequence_segment.getSequenceSegmentName());

      const size_t n = sequence_segment_processing_methods_.size();

      // process the sequence segment
      for (size_t i = 0; i < n; ++i) {
        LOGI << "[" << (i + 1) << "/" << n << "] steps in processing sequence segments";
        sequence_segment_processing_methods_[i]->process(
          sequence_segment,
          *sequenceHandler_IO,
          (*sequenceHandler_IO)
            .getSequence()
            .at(sequence_segment.getSampleIndices().front())
            .getRawData()
            .getParameters(), // assumption: all parameters are the same for each sample in the sequence segment!
          filenames_.at(sequence_segment.getSequenceSegmentName())
        );
      }
      notifySequenceSegmentProcessorSampleEnd(sequence_segment.getSequenceSegmentName());
    }

    sequenceHandler_IO->setSequenceSegments(sequence_segments);
    notifySequenceSegmentProcessorEnd();
  }

  void ProcessSampleGroups::process()
  {
    std::vector<SampleGroupHandler> sample_groups;

    if (sample_group_names_.empty()) { // select all
      sample_groups = sequenceHandler_IO->getSampleGroups();
    }
    else 
    { // select those with specific sample group names
      for (SampleGroupHandler& s : sequenceHandler_IO->getSampleGroups()) {
        if (sample_group_names_.count(s.getSampleGroupName())) {
          sample_groups.push_back(s);
        }
      }
    }
    if (filenames_.size() < sample_groups.size()) {
      throw std::invalid_argument("The number of provided filenames_ locations is not correct.");
    }

    notifySampleGroupProcessorStart(sample_groups.size());

    // process by sample group
    for (SampleGroupHandler& sample_group : sample_groups) {

      notifySampleGroupProcessorSampleStart(sample_group.getSampleGroupName());
      // handle user-desired sample_group_processing_methods
      if (!sample_group_processing_methods_.size()) {
        throw "no sample group processing methods given.\n";
      }

      const size_t n = sample_group_processing_methods_.size();

      // process the sample group
      for (size_t i = 0; i < n; ++i) {
        LOGI << "[" << (i + 1) << "/" << n << "] steps in processing sample groups";
        sample_group_processing_methods_[i]->process(
          sample_group,
          *sequenceHandler_IO,
          (*sequenceHandler_IO)
          .getSequence()
          .at(sample_group.getSampleIndices().front())
          .getRawData()
          .getParameters(), // assumption: all parameters are the same for each sample in the sample group!
          filenames_.at(sample_group.getSampleGroupName())
        );
      }
      notifySampleGroupProcessorSampleEnd(sample_group.getSampleGroupName());
    }

    sequenceHandler_IO->setSampleGroups(sample_groups);
    notifySampleGroupProcessorEnd();
  }

  void SequenceProcessorMultithread::spawn_workers(unsigned int n_threads)
  {
    // Refine the # of threads based on the hardware
    size_t n_workers = getNumWorkers(n_threads);
    LOGD << "Number of workers: " << n_workers;

    // Spawn the workers
    try {
      std::list<std::future<void>> futures;
      LOGD << "Spawning workers...";
      for (size_t i = 0; i < n_workers; ++i) {
        futures.emplace_back(std::async(std::launch::async, &SequenceProcessorMultithread::run_injection_processing, this));
      }
      LOGD << "Waiting for workers...";
      for (std::future<void>& f : futures) {
        f.wait();
      }
      LOGD << "Workers are done";
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }
  }

  void SequenceProcessorMultithread::run_injection_processing()
  {
    while (true) {
      // fetch the atomic injection counter
      const size_t i = i_.fetch_add(1);
      if (i >= injections_.size()) {
        break;
      }

      // Launch the processing method
      InjectionHandler& injection { injections_[i] };
      if (observable_) observable_->notifySequenceProcessorSampleStart(injection.getMetaData().getSampleName());
      try {
        std::future<void> f = std::async(
          std::launch::async,
          processInjection,
          std::ref(injection),
          std::cref(filenames_.at(injection.getMetaData().getInjectionName())),
          std::cref(methods_));
        LOGD << "Injection [" << i << "]: waiting...";
        f.wait();
        LOGD << "Injection [" << i << "]: done";
        try {
          f.get(); // check for exceptions
        }
        catch (const std::exception& e) {
          LOGE << "Injection [" << i << "]: " << typeid(e).name() << " : " << e.what();
        }
        if (observable_) observable_->notifySequenceProcessorSampleEnd(injection.getMetaData().getSampleName());
      }
      catch (const std::exception& e) {
        LOGE << "Injection [" << i << "]: " << typeid(e).name() << " : " << e.what();
      }
    }
    LOGD << "Worker is done";
  }

  size_t SequenceProcessorMultithread::getNumWorkers(unsigned int n_threads) const {
    const unsigned int max_threads = std::thread::hardware_concurrency(); // might return 0
    size_t n_workers = 0;

    if (max_threads != 0) {
      if (n_threads > max_threads) {
        n_workers = max_threads - 1;
      }
      else if (n_threads <= max_threads && n_threads > 1) 
      {
        n_workers = n_threads - 1;
      }
      else if (n_threads >= 0) 
      {
        LOGD << "Max available threads: " << max_threads;
        LOGD << "but using just 1 thread.";
        n_workers = 1;
      }
    }
    else 
    {
      LOGD << "Couldn't determine # of threads, using just 1 thread!";
      n_workers = 1;
    }
    return n_workers;
  }

  void processInjection(
    InjectionHandler& injection,
    const Filenames& filenames,
    const std::vector<std::shared_ptr<RawDataProcessor>>& methods
  )
  {
    size_t i_step { 1 };
    const size_t n_steps { methods.size() };
    const std::string inj_name { injection.getMetaData().getInjectionName() };
    for (const std::shared_ptr<RawDataProcessor>& p : methods) {
      LOGI << "[" << (i_step++) << "/" << n_steps << "] method on injection: " << inj_name;
      p->process( //TODO: (SIGABRT)
        injection.getRawData(),
        injection.getRawData().getParameters(),
        filenames
      );
    }
  }

  void LoadWorkflow::process()
  {
    // TODO: move to parameters at some point
    LOGD << "START LoadWorkflow";
    LOGI << "Loading " << filename_;

    if (filename_.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END LoadWorkflow";
      return;
    }

    if (!InputDataValidation::fileExists(filename_)) {
      LOGE << "File not found";
      LOGD << "END LoadWorkflow";
      return;
    }

    std::vector<std::string> res;
    try {
      io::CSVReader<1, io::trim_chars<>, io::no_quote_escape<','>> in(filename_);
      const std::string s_command_name{ "command_name" };
      in.read_header(
        io::ignore_extra_column,
        s_command_name
      );
      if (!in.has_column(s_command_name))
      {
        LOGE << "Unexpected header";
        return;
      }
      std::string command_name;
      while (in.read_row(
        command_name
      )) {
        res.push_back(command_name);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "workflow clear";
      res.clear();
    }
    sequenceHandler_IO->setWorkflow(res);
    sequenceHandler_IO->notifyWorkflowUpdated();
    LOGD << "END LoadWorkflow";
  }

  void StoreWorkflow::process()
  {
    LOGD << "START StoreWorkflow";
    LOGI << "Storing " << filename_;

    std::vector<std::string> headers = { "command_name" };
    CSVWriter writer(filename_, ",");
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      LOGD << "END writeDataTableFromMetaValue";
      return;
    }

    std::vector<std::vector<std::string>> rows;
    for (const auto& command : sequenceHandler_IO->getWorkflow())
    {
      std::vector<std::string> row;
      row.push_back(command);
      rows.push_back(row);
    }

    for (const std::vector<std::string>& line : rows) {
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END StoreWorkflow";
  }
}
