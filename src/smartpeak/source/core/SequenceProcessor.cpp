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
//#include <map>
//#include <memory> // shared_ptr
//#include <set>
//#include <string>

//#include <vector>

namespace SmartPeak
{
  void CreateSequence::process() const
  {
    LOGD << "START createSequence";

    SequenceParser::readSequenceFile(*sequenceHandler_IO, filenames.sequence_csv_i, delimiter);
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
    loadParameters.process(rawDataHandler, {}, filenames);
    LoadTransitions loadTransitions;
    loadTransitions.process(rawDataHandler, {}, filenames);
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)
    LoadValidationData loadValidationData;
    loadValidationData.process(rawDataHandler, {}, filenames);
    // raw data files (i.e., mzML, trafo, etc., will be loaded dynamically)

    // load sequenceSegmentHandler files
    for (SequenceSegmentHandler& sequenceSegmentHandler: sequenceHandler_IO->getSequenceSegments()) {
      LoadQuantitationMethods loadQuantitationMethods;
      loadQuantitationMethods.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
      LoadStandardsConcentrations loadStandardsConcentrations;
      loadStandardsConcentrations.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
      LoadFeatureFilters loadFeatureFilters;
      loadFeatureFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
      LoadFeatureQCs loadFeatureQCs;
      loadFeatureQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
      LoadFeatureRSDFilters loadFeatureRSDFilters;
      loadFeatureRSDFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
      LoadFeatureRSDQCs loadFeatureRSDQCs;
      loadFeatureRSDQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
      LoadFeatureBackgroundFilters loadFeatureBackgroundFilters;
      loadFeatureBackgroundFilters.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
      LoadFeatureBackgroundQCs loadFeatureBackgroundQCs;
      loadFeatureBackgroundQCs.process(sequenceSegmentHandler, SequenceHandler(), {}, filenames);
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
    if (raw_data_processing_methods_I.empty()) {
      throw "no raw data processing methods given.\n";
    }

    std::vector<InjectionHandler> injections = injection_names.empty()
      ? sequenceHandler_IO->getSequence()
      : sequenceHandler_IO->getSamplesInSequence(injection_names);

    if (filenames.size() < injections.size()) {
      throw std::invalid_argument("The number of provided filenames locations is not correct.");
    }

    SequenceProcessorMultithread manager(
      injections,
      filenames,
      raw_data_processing_methods_I
    );

    manager.spawn_workers(6);
  }

  void ProcessSequenceSegments::process() const
  {
    std::vector<SequenceSegmentHandler> sequence_segments;

    if (sequence_segment_names.empty()) { // select all
      sequence_segments = sequenceHandler_IO->getSequenceSegments();
    } else { // select those with specific sequence segment names
      for (SequenceSegmentHandler& s : sequenceHandler_IO->getSequenceSegments()) {
        if (sequence_segment_names.count(s.getSequenceSegmentName())) {
          sequence_segments.push_back(s);
        }
      }
    }
    if (filenames.size() < sequence_segments.size()) {
      throw std::invalid_argument("The number of provided filenames locations is not correct.");
    }

    // process by sequence segment
    for (SequenceSegmentHandler& sequence_segment : sequence_segments) {

      // handle user-desired sequence_segment_processing_methods
      if (!sequence_segment_processing_methods_I.size()) {
        throw "no sequence segment processing methods given.\n";
      }

      const size_t n = sequence_segment_processing_methods_I.size();

      // process the sequence segment
      for (size_t i = 0; i < n; ++i) {
        LOGI << "[" << (i + 1) << "/" << n << "] steps in processing sequence segments";
        sequence_segment_processing_methods_I[i]->process(
          sequence_segment,
          *sequenceHandler_IO,
          (*sequenceHandler_IO)
            .getSequence()
            .at(sequence_segment.getSampleIndices().front())
            .getRawData()
            .getParameters(), // assumption: all parameters are the same for each sample in the sequence segment!
          filenames.at(sequence_segment.getSequenceSegmentName())
        );
      }
    }

    sequenceHandler_IO->setSequenceSegments(sequence_segments);
  }

  void SequenceProcessorMultithread::spawn_workers(unsigned int n_threads)
  {
    size_t n_workers = getNumWorkers(n_threads);

    LOGD << "Number of workers: " << n_workers;
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

  void SequenceProcessorMultithread::run_injection_processing()
  {
    while (true) {
      const size_t i = i_.fetch_add(1);
      if (i >= injections_.size()) {
        break;
      }
      InjectionHandler& injection { injections_[i] };
      std::future<void> f = std::async(
        std::launch::async,
        processInjection,
        std::ref(injection),
        std::cref(filenames_.at(injection.getMetaData().getInjectionName())),
        std::cref(methods_)
      );
      LOGD << "Injenction [" << i << "]: waiting...";
      f.wait();
      LOGD << "Injenction [" << i << "]: done";
      try {
        f.get(); // check for exceptions
      } catch (const std::exception& e) {
        LOGE << "Injenction [" << i << "]: " << e.what();
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
