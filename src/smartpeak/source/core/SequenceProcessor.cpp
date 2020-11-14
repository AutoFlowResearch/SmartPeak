// TODO: Add copyright

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <plog/Log.h>
#include <atomic>
#include <future>
#include <list>

namespace SmartPeak
{
  void CreateSequence::process() const
  {
    LOGD << "START createSequence";

    SequenceParser::readSequenceFile(*sequenceHandler_IO, filenames_.sequence_csv_i, delimiter);
    if (sequenceHandler_IO->getSequence().empty()) {
      LOGE << "Empty sequence. Returning";
      LOGD << "END createSequence";
      return;
    }
    // TODO: Given that the raw data is shared between all injections, it could
    // be beneficial to move it somewhere else (i.e. in SequenceHandler) and adapt
    // the algorithms to the change
    RawDataHandler& rawDataHandler = sequenceHandler_IO->getSequence()[0].getRawData();

    // load rawDataHandler files (applies to the whole session)
    LoadParameters loadParameters;
    loadParameters.process(rawDataHandler, {}, filenames_);
    LoadTransitions loadTransitions;
    loadTransitions.process(rawDataHandler, {}, filenames_);
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)
    LoadValidationData loadValidationData;
    loadValidationData.process(rawDataHandler, {}, filenames_);
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)

    // load sequenceSegmentHandler files
    for (SequenceSegmentHandler& sequenceSegmentHandler: sequenceHandler_IO->getSequenceSegments()) {
      LoadQuantitationMethods loadQuantitationMethods;
      loadQuantitationMethods.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadStandardsConcentrations loadStandardsConcentrations;
      loadStandardsConcentrations.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureFilters loadFeatureFilters;
      loadFeatureFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureQCs loadFeatureQCs;
      loadFeatureQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureRSDFilters loadFeatureRSDFilters;
      loadFeatureRSDFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureRSDQCs loadFeatureRSDQCs;
      loadFeatureRSDQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureBackgroundFilters loadFeatureBackgroundFilters;
      loadFeatureBackgroundFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
      LoadFeatureBackgroundQCs loadFeatureBackgroundQCs;
      loadFeatureBackgroundQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames_);
    }

    if (checkConsistency) {
      InputDataValidation::sampleNamesAreConsistent(*sequenceHandler_IO);
      InputDataValidation::componentNamesAreConsistent(*sequenceHandler_IO);
      InputDataValidation::componentNameGroupsAreConsistent(*sequenceHandler_IO);
      InputDataValidation::heavyComponentsAreConsistent(*sequenceHandler_IO);
    }

    LOGD << "END createSequence";
  }

  void ProcessSequence::process() const
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

    // Determine the number of threads to launch
    const auto& params = injections.front().getRawData().getParameters();
    int n_threads = 6;
    if (params.count("SequenceProcessor") && !params.at("SequenceProcessor").empty()) {
      for (const auto& p : params.at("SequenceProcessor")) {
        if (p.at("name") == "n_thread") {
          try {
            n_threads = std::stoi(p.at("value"));
            LOGI << "n_threads set to " << n_threads;
          }
          catch (const std::exception& e) {
            LOGE << e.what();
          }
        }
      }
    }

    // Launch
    SequenceProcessorMultithread manager(
      injections,
      filenames_,
      raw_data_processing_methods_
    );
    manager.spawn_workers(n_threads);
  }

  void ProcessSequenceSegments::process() const
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

    // process by sequence segment
    for (SequenceSegmentHandler& sequence_segment : sequence_segments) {

      // handle user-desired sequence_segment_processing_methods
      if (!sequence_segment_processing_methods_.size()) {
        throw "no sequence segment processing methods given.\n";
      }

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
    }

    sequenceHandler_IO->setSequenceSegments(sequence_segments);
  }

  void ProcessSampleGroups::process() const
  {
    std::vector<SampleGroupHandler> sample_groups;

    if (sample_group_names_.empty()) { // select all
      sample_groups = sequenceHandler_IO->getSampleGroups();
    }
    else { // select those with specific sample group names
      for (SampleGroupHandler& s : sequenceHandler_IO->getSampleGroups()) {
        if (sample_group_names_.count(s.getSampleGroupName())) {
          sample_groups.push_back(s);
        }
      }
    }
    if (filenames_.size() < sample_groups.size()) {
      throw std::invalid_argument("The number of provided filenames_ locations is not correct.");
    }

    // process by sample group
    for (SampleGroupHandler& sample_group : sample_groups) {

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
    }

    sequenceHandler_IO->setSampleGroups(sample_groups);
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
      for (size_t i = 0; i < injections_.size(); ++i) {
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
          LOGE << "Injection [" << i << "]: " << e.what();
        }
      }
      catch (const std::exception& e) {
        LOGE << "Injection [" << i << "]: " << e.what();
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
      else if (n_threads <= max_threads && n_threads > 1) {
        n_workers = n_threads - 1;
      }
      else if (n_threads >= 0) {
        LOGD << "Max available threads: " << max_threads;
        LOGD << "but using just 1 thread.";
        n_workers = 1;
      }
    }
    else {
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
      p->process(
        injection.getRawData(),
        injection.getRawData().getParameters(),
        filenames
      );
    }
  }
}
