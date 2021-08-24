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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/ApplicationHandler.h>
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
#include <unordered_set>
#include <filesystem>

namespace SmartPeak
{

  bool CreateSequence::buildStaticFilenames(ApplicationHandler* application_handler, Filenames& filenames)
  {
    application_handler->main_dir_ = std::filesystem::path(application_handler->sequence_pathname_).remove_filename().generic_string();
    filenames.setTag(Filenames::Tag::MAIN_DIR, application_handler->main_dir_.generic_string());
    filenames.setFullPath("sequence_csv_i", application_handler->sequence_pathname_);

    LOGN << "\n\n"
      "The following list of file was searched for:\n";
    bool is_valid  = InputDataValidation::precheckProcessorInputs(*this, "sequence", filenames, true);
    is_valid &= InputDataValidation::precheckProcessorInputs(LoadParameters(), "parameters", filenames, true);
    is_valid &= InputDataValidation::precheckProcessorInputs(LoadTransitions(), "traml", filenames, true);
    is_valid &= InputDataValidation::precheckProcessorInputs(LoadFeatureFilters(), "featureFilter", filenames, false);
    is_valid &= InputDataValidation::precheckProcessorInputs(LoadFeatureQCs(), "featureQC", filenames, false);
    is_valid &= InputDataValidation::precheckProcessorInputs(LoadQuantitationMethods(), "quantitationMethods", filenames, false);
    is_valid &= InputDataValidation::precheckProcessorInputs(LoadStandardsConcentrations(), "standardsConcentrations", filenames, false);
    is_valid &= InputDataValidation::precheckProcessorInputs(LoadValidationData(), "referenceData", filenames, false);
    std::cout << "\n\n";

    if (!is_valid) {
      LOGE << "One or more pathnames are not valid.\n";
      LOGF << "Make sure that the following required pathnames are provided:\n"
              " - sequence\n"
              " - parameters\n"
              " - traml\n";
      LOGN << "Apply the fixes and reload the sequence file.\n";
      return false;
    }
    return true;
  }

  bool CreateSequence::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    application_handler->sequence_pathname_ = filename;
    application_handler->mzML_dir_.clear();
    application_handler->features_in_dir_.clear();
    application_handler->features_out_dir_.clear();
    LOGI << "Pathnames for 'mzML', 'INPUT features' and 'OUTPUT features' reset.";
    const bool pathnamesAreCorrect = buildStaticFilenames(application_handler, filenames_);
    if (pathnamesAreCorrect) {
      application_handler->sequenceHandler_.clear();
      delimiter = ",";
      checkConsistency = false; // NOTE: Requires a lot of time on large sequences with a large number of components
      process();
      return true;
    }
    else
    {
      LOGE << "Provided and/or inferred pathnames are not correct."
        "The sequence has not been modified.";
      return false;
    }
  }

  void CreateSequence::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("sequence_csv_i", "${MAIN_DIR}/sequence.csv");
  };

  void CreateSequence::process()
  {
    LOGD << "START createSequence";
    filenames_ = prepareFilenames(filenames_);

    SequenceParser::readSequenceFile(*sequenceHandler_IO, filenames_.getFullPath("sequence_csv_i"), delimiter);
    if (sequenceHandler_IO->getSequence().empty()) {
      LOGE << "Empty sequence. Returning";
      LOGD << "END createSequence";
      return;
    }

    // load workflow
    LoadWorkflow loadWorkflow(*sequenceHandler_IO);
    loadWorkflow.filenames_ = filenames_;
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
              LOGI << "SequenceProcessor::n_threads set to " << n_threads;
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
      this);
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

    // handle user-desired sequence_segment_processing_methods
    if (!sequence_segment_processing_methods_.size()) {
      throw "no sequence segment processing methods given.\n";
    }

    SequenceSegmentProcessorMultithread manager(
      sequence_segments,
      (*sequenceHandler_IO),
      sequence_segment_processing_methods_,
      filenames_,
      this);
    manager.run_processing();
    sequenceHandler_IO->setSequenceSegments(sequence_segments);
    notifySequenceSegmentProcessorEnd();
  }

  void processSegment(
    SequenceSegmentHandler& sequence_segment,
    SequenceHandler& sequenceHandler_IO,
    const Filenames& filenames,
    const std::vector<std::shared_ptr<SequenceSegmentProcessor>>& methods)
  {
    const size_t nr_methods = methods.size();
    LOGI << ">>Processing SequenceSegment [" << sequence_segment.getSequenceSegmentName() << "]\n";
    for (size_t i = 0; i < nr_methods; ++i) {
      LOGI << "[" << (i + 1) << "/" << nr_methods << "] steps in processing sequence segments";
      methods[i]->process(
        sequence_segment,
        sequenceHandler_IO,
        (sequenceHandler_IO)
          .getSequence()
          .at(sequence_segment.getSampleIndices().front())
          .getRawData()
          .getParameters(),
        filenames
      );
    }
  }

  void SequenceSegmentProcessorMultithread::run_processing()
  {
    while (true) {
      const size_t i = i_.fetch_add(1);
      if (i >= sequence_segment_.size()) {
        break;
      }
  
      SequenceSegmentHandler& sequence_seg {sequence_segment_[i]};
      if (observable_) observable_->notifySequenceSegmentProcessorSampleStart(sequence_seg.getSequenceSegmentName());
      
      try {
        std::future<void> f = std::async(
        std::launch::async,
        processSegment,
        std::ref(sequence_seg),
        std::ref(sequenceHandler_IO),
        std::cref(filenames_.at( sequence_seg.getSequenceSegmentName())),
        std::cref(sequence_segment_processing_methods_));
          
        LOGI << ">>SequenceSegment [" << sequence_seg.getSequenceSegmentName() << "]: waiting...";
        f.wait();
        LOGI << ">>SequenceSegment [" << sequence_seg.getSequenceSegmentName() << "]: done";
          
        try {
          f.get();
        }
        catch (const std::exception& e) {
          LOGE << "SequenceSegment [" << "i" << "]: " << typeid(e).name() << " : " << e.what();
        }
          
        if (observable_) observable_->notifySequenceSegmentProcessorSampleEnd(sequence_seg.getSequenceSegmentName());
      }
      catch (const std::exception& e) {
        LOGE << "SequenceSegment [" << "i" << "]: " << typeid(e).name() << " : " << e.what();
      }
    }
  }

  void SampleGroupProcessorMultithread::run_processing()
  {
    while (true) {
      const size_t i = i_.fetch_add(1);
      if (i >= sample_group_.size()) {
        break;
      }
  
      SampleGroupHandler& sequence_seg {sample_group_[i]};
      if (observable_) observable_->notifySampleGroupProcessorSampleStart(sequence_seg.getSampleGroupName());
      
      try {
        std::future<void> f = std::async(
        std::launch::async,
        processSampleGroup,
        std::ref(sequence_seg),
        std::ref(sequenceHandler_IO),
        std::cref(filenames_.at( sequence_seg.getSampleGroupName())),
        std::cref(sample_group_processing_methods_));
          
        LOGI << ">>SequenceSegment [" << sequence_seg.getSampleGroupName() << "]: waiting...";
        f.wait();
        LOGI << ">>SequenceSegment [" << sequence_seg.getSampleGroupName() << "]: done";
          
        try {
          f.get();
        }
        catch (const std::exception& e) {
          LOGE << "SequenceSegment [" << "i" << "]: " << typeid(e).name() << " : " << e.what();
        }
          
        if (observable_) observable_->notifySampleGroupProcessorSampleEnd(sequence_seg.getSampleGroupName());
      }
      catch (const std::exception& e) {
        LOGE << "SequenceSegment [" << "i" << "]: " << typeid(e).name() << " : " << e.what();
      }
    }
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
      
      SampleGroupProcessorMultithread manager(
        sample_groups,
        (*sequenceHandler_IO),
        sample_group_processing_methods_,
        filenames_,
        this);
      manager.run_processing();
      //manager.spawn_workers(8);
      notifySampleGroupProcessorSampleEnd(sample_group.getSampleGroupName());
    }

    sequenceHandler_IO->setSampleGroups(sample_groups);
    notifySampleGroupProcessorEnd();
  }

  void processSampleGroup(
    SampleGroupHandler& sample_group,
    SequenceHandler& sequenceHandler_IO,
    const Filenames& filenames,
    const std::vector<std::shared_ptr<SampleGroupProcessor>>& methods)
  {
    const size_t n = methods.size();
    for (size_t i = 0; i < n; ++i) {
      LOGI << "[" << (i + 1) << "/" << n << "] steps in processing sample groups";
      methods[i]->process(
        sample_group,
        sequenceHandler_IO,
        (sequenceHandler_IO)
        .getSequence()
        .at(sample_group.getSampleIndices().front())
        .getRawData()
        .getParameters(),
        filenames
      );
    }
  }

  void SequenceSegmentProcessorMultithread::spawn_workers(unsigned int n_threads)
  {
    // Refine the # of threads based on the hardware
    size_t n_workers = getNumWorkers(n_threads);
    LOGD << "Number of workers: " << n_workers;

    // Spawn the workers
    try {
      std::list<std::future<void>> futures;
      LOGD << "Spawning workers...";
      for (size_t i = 0; i < n_workers; ++i) {
        futures.emplace_back(std::async(std::launch::async, &SequenceSegmentProcessorMultithread::run_processing, this));
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

  void SampleGroupProcessorMultithread::spawn_workers(unsigned int n_threads)
  {
    // Refine the # of threads based on the hardware
    size_t n_workers = getNumWorkers(n_threads);
    LOGD << "Number of workers: " << n_workers;

    // Spawn the workers
    try {
      std::list<std::future<void>> futures;
      LOGD << "Spawning workers...";
      for (size_t i = 0; i < n_workers; ++i) {
        futures.emplace_back(std::async(std::launch::async, &SampleGroupProcessorMultithread::run_processing, this));
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
        futures.emplace_back(std::async(std::launch::async, &SequenceProcessorMultithread::run_processing, this));
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

  void SequenceProcessorMultithread::run_processing()
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

  size_t ProcessorMultithread::getNumWorkers(unsigned int n_threads) const {
    const unsigned int max_threads = std::thread::hardware_concurrency(); // might return 0
    size_t n_workers = 0;

    if (max_threads != 0) {
      if (n_threads > max_threads) {
        n_workers = max_threads - 1;
      } else if (n_threads <= max_threads && n_threads > 1) {
        n_workers = n_threads - 1;
      } else if (n_threads >= 0) {
        LOGD << "Max available threads: " << max_threads;
        LOGD << "but using just 1 thread.";
        n_workers = 1;
      }
    } else {
      LOGD << "Couldn't determine # of threads, using just 1 thread!";
      n_workers = 1;
    }
    return n_workers;
  }

  void processInjection(
    InjectionHandler& injection,
    const Filenames& filenames_I,
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
        filenames_I
      );
    }
  }

  bool LoadWorkflow::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    filenames_.setFullPath("workflow_csv_i", filename);
    process();
    return true;
  }

  void LoadWorkflow::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("workflow_csv_i", "${MAIN_DIR}/workflow.csv");
  };

  void LoadWorkflow::process()
  {
    // TODO: move to parameters at some point
    LOGD << "START LoadWorkflow";
    filenames_ = prepareFilenames(filenames_);
    if (!InputDataValidation::prepareToLoad(filenames_, "workflow_csv_i"))
    {
      LOGD << "END " << getName();
      return;
    }

    std::vector<std::string> res;
    try {
      io::CSVReader<1, io::trim_chars<>, io::no_quote_escape<','>> in(filenames_.getFullPath("workflow_csv_i").generic_string());
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

  bool StoreWorkflow::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    filename_ = filename;
    process();
    return true;
  }

  void StoreWorkflow::process()
  {
    LOGD << "START StoreWorkflow";
    LOGI << "Storing " << filename_.generic_string();

    std::vector<std::string> headers = { "command_name" };
    CSVWriter writer(filename_.generic_string(), ",");
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
