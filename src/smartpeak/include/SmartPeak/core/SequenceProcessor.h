// TODO: Add copyright

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <map>
#include <memory> // shared_ptr
#include <set>
#include <string>
#include <vector>
#include <atomic>

namespace SmartPeak
{
  /**
    Processes injections onto multiple threads of execution
  */
  class SequenceProcessorMultithread {
  public:
    SequenceProcessorMultithread(
      std::vector<InjectionHandler>& injections,
      const std::map<std::string, Filenames>& filenames,
      const std::vector<std::shared_ptr<RawDataProcessor>>& methods
    ) : injections_(injections), filenames_(filenames), methods_(methods) {}

    /**
      Spawn a number of workers equal to the number of threads of execution
      offered by the CPU

      @note If the API is unable to fetch the required information, only a
      single thread will be used
    */
    void spawn_workers();

    /**
      Workers run this function. It implements a loop that runs the following steps:
      - fetch an injection
      - process all methods on it

      Workers decide on which injection to work according to an index fetched and
      incremented atomically (i_).

      The loop ends when the worker fetches an index that is out of range.
    */
    void run_injection_processing();

  private:
    std::atomic_size_t i_ { 0 }; ///< a worker works on the i_-th injection
    std::vector<InjectionHandler>& injections_; ///< the injections to be processed
    const std::map<std::string, Filenames>& filenames_; ///< mapping from injections names to the associated filenames
    const std::vector<std::shared_ptr<RawDataProcessor>>& methods_; ///< methods to run on each injection
  };

  /**
    Apply a processing workflow to a single injection

    @param[in,out] injection The injection to process
    @param[in] filenames Used by the methods
    @param[in] methods Methods to process on the injection
  */
  void processInjection(
    InjectionHandler& injection,
    const Filenames& filenames,
    const std::vector<std::shared_ptr<RawDataProcessor>>& methods
  );

  struct SequenceProcessor {
    SequenceProcessor(SequenceHandler& sh) : sequenceHandler_IO(&sh) {}
    virtual ~SequenceProcessor() = default;

    virtual void process() const = 0;

    SequenceHandler* sequenceHandler_IO = nullptr; /// Sequence handler, used by all SequenceProcessor derived classes
  };

  /**
    Create a new sequence from files or wizard
  */
  struct CreateSequence : SequenceProcessor {
    Filenames        filenames;                    /// Pathnames to load
    std::string      delimiter          = ",";     /// String delimiter of the imported file
    bool             checkConsistency   = true;    /// Check consistency of data contained in files

    CreateSequence() = default;
    CreateSequence(SequenceHandler& sh) : SequenceProcessor(sh) {}
    void process() const override;
  };

  /**
    Apply a processing workflow to all injections in a sequence
  */
  struct ProcessSequence : SequenceProcessor {
    std::map<std::string, Filenames>               filenames;                     /// Mapping from injection names to pathnames
    std::set<std::string>                          injection_names;               /// Injections to select from the sequence (all if empty)
    std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods_I; /// Events to process

    ProcessSequence() = default;
    ProcessSequence(SequenceHandler& sh) : SequenceProcessor(sh) {}
    void process() const override;
  };

  /**
    Apply a processing workflow to all injections in a sequence segment
  */
  struct ProcessSequenceSegments : SequenceProcessor {
    std::map<std::string, Filenames>                       filenames;                             /// Mapping from sequence groups names to pathnames
    std::set<std::string>                                  sequence_segment_names;                /// Sequence groups to select from the sequence (all if empty)
    std::vector<std::shared_ptr<SequenceSegmentProcessor>> sequence_segment_processing_methods_I; /// Events to process

    ProcessSequenceSegments() = default;
    ProcessSequenceSegments(SequenceHandler& sh) : SequenceProcessor(sh) {}
    void process() const override;
  };
}
