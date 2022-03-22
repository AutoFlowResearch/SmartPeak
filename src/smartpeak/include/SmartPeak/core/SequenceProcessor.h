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

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/core/SampleGroupProcessorObservable.h>
#include <SmartPeak/core/SequenceProcessorObservable.h>
#include <SmartPeak/core/SequenceSegmentProcessorObservable.h>
#include <SmartPeak/iface/IProcessorDescription.h>
#include <SmartPeak/iface/IFilenamesHandler.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <map>
#include <memory> // shared_ptr
#include <set>
#include <string>
#include <vector>
#include <atomic>
#include <thread>

namespace SmartPeak
{
  /**
    Multithreaded execution processor
  */
  class ProcessorMultithread {
  public:
    /**
      Determine the number of workers available based on the maximum available
      threads and the desired thread count. 1 thread will always be preserved for
      the GUI unless the maximum number of available threads couldn't be determined

      @param[in] n_threads desired number of threads to use
    */
    size_t getNumWorkers(unsigned int n_threads) const;
    
    virtual void run_processing() = 0;
  };

  /**
    Processes injections onto multiple threads of execution
  */
  class SequenceProcessorMultithread : public ProcessorMultithread {
  public:
    SequenceProcessorMultithread(
      std::vector<InjectionHandler>& injections,
      std::map<std::string, Filenames>& filenames,
      const std::vector<std::shared_ptr<RawDataProcessor>>& methods,
      SequenceProcessorObservable* observable = nullptr
    ) : injections_(injections), filenames_(filenames), methods_(methods), observable_(observable) {}

    /**
      Spawn a number of workers equal to the number of threads of execution
      offered by the CPU

      @note If the API is unable to fetch the required information, only a
      single thread will be used
    */
    void spawn_workers(unsigned int n_threads);

    /**
      Workers run this function. It implements a loop that runs the following steps:
      - fetch an injection
      - process all methods on it

      Workers decide on which injection to work according to an index fetched and
      incremented atomically (i_).

      The loop ends when the worker fetches an index that is out of range.
    */
    void run_processing();

  private:
    std::atomic_size_t i_ { 0 }; ///< a worker works on the i_-th injection
    std::vector<InjectionHandler>& injections_; ///< the injections to be processed
    std::map<std::string, Filenames>& filenames_; ///< mapping from injections names to the associated filenames
    const std::vector<std::shared_ptr<RawDataProcessor>>& methods_; ///< methods to run on each injection
    SequenceProcessorObservable* observable_;
  };

  /**
    Processes Sequence Segment onto multiple threads of execution
  */
  class SequenceSegmentProcessorMultithread : public ProcessorMultithread  {
  public:
    SequenceSegmentProcessorMultithread(
      std::vector<SequenceSegmentHandler>& sequenceSegmentHandler_IO,
      SequenceHandler& sequenceHandler_I,
      std::vector<std::shared_ptr<SequenceSegmentProcessor>>& sequence_segment_processing_methods_,
      std::map<std::string, Filenames>& filenames,
      SequenceSegmentProcessorObservable* observable = nullptr
    ) : sequence_segment_(sequenceSegmentHandler_IO),
        sequenceHandler_IO(sequenceHandler_I),
        sequence_segment_processing_methods_(sequence_segment_processing_methods_),
        filenames_(filenames),
        observable_(observable) {}
    
    /**
      Spawn a number of workers equal to the number of threads of execution
      offered by the CPU

      @note If the API is unable to fetch the required information, only a
      single thread will be used
    */
    void spawn_workers(unsigned int n_threads);

    /**
      Workers run this function. It implements a loop that runs the following steps:
      - fetch sequence segments
      - process all methods on it

      Workers decide on which sequence segment to work according to an index fetched and
      incremented atomically (i_).

      The loop ends when the worker fetches an index that is out of range.
    */
    void run_processing();

  private:
    std::atomic_size_t i_ { 0 }; ///< a worker works on the i_-th injection
    std::map<std::string, Filenames>& filenames_; ///< mapping from injections names to the associated filenames
    std::vector<SequenceSegmentHandler>& sequence_segment_;
    SequenceHandler& sequenceHandler_IO;
    std::vector<std::shared_ptr<SequenceSegmentProcessor>>& sequence_segment_processing_methods_;
    SequenceSegmentProcessorObservable* observable_;
  };

  /**
    Processes Sample Group onto multiple threads of execution
  */
  class SampleGroupProcessorMultithread : public ProcessorMultithread  {
  public:
    SampleGroupProcessorMultithread(
      std::vector<SampleGroupHandler>& sequenceSegmentHandler_IO,
      SequenceHandler& sequenceHandler_I,
      std::vector<std::shared_ptr<SampleGroupProcessor>>& sample_group_processing_methods,
      std::map<std::string, Filenames>& filenames,
      SampleGroupProcessorObservable* observable = nullptr
    ) : sample_group_(sequenceSegmentHandler_IO),
        sequenceHandler_IO(sequenceHandler_I),
        sample_group_processing_methods_(sample_group_processing_methods),
        filenames_(filenames),
        observable_(observable) {}
    
    /**
      Spawn a number of workers equal to the number of threads of execution
      offered by the CPU

      @note If the API is unable to fetch the required information, only a
      single thread will be used
    */
    void spawn_workers(unsigned int n_threads);

    /**
      Workers run this function. It implements a loop that runs the following steps:
      - fetch sequence segments
      - process all methods on it

      Workers decide on which sequence segment to work according to an index fetched and
      incremented atomically (i_).

      The loop ends when the worker fetches an index that is out of range.
    */
    void run_processing();

  private:
    std::atomic_size_t i_ { 0 }; ///< a worker works on the i_-th injection
    std::map<std::string, Filenames>& filenames_; ///< mapping from injections names to the associated filenames
    std::vector<SampleGroupHandler>& sample_group_;
    SequenceHandler& sequenceHandler_IO;
    std::vector<std::shared_ptr<SampleGroupProcessor>>& sample_group_processing_methods_;
    SampleGroupProcessorObservable* observable_;
  };

  /**
    Apply a processing workflow to a single injection

    @param[in,out] injection The injection to process
    @param[in] filenames Used by the methods
    @param[in] methods Methods to process on the injection
  */
  void processInjection(
    InjectionHandler& injection,
    Filenames& filenames_I,
    const std::vector<std::shared_ptr<RawDataProcessor>>& methods
  );

  /**
    Apply a processing workflow to a single sequence segment

    @param[in,out] sequence_segment The injection to process
    @param[in] SequenceHandler Sequence Segment IO
    @param[in] filenames Used by the methods
    @param[in] methods Methods to process on the sequence segment
  */
  void processSegment(SequenceSegmentHandler& sequence_segment,
                      SequenceHandler& sequenceHandler_IO,
                      Filenames& filenames,
                      const std::vector<std::shared_ptr<SequenceSegmentProcessor>>& methods);

  /**
    Apply a processing workflow to a single sample group

    @param[in,out] sample_group The sample group to process
    @param[in] SequenceHandler Sequence Segment IO
    @param[in] filenames Used by the methods
    @param[in] methods Methods to process on the sample group
  */
  void processSampleGroup(SampleGroupHandler& sample_group,
                          SequenceHandler& sequenceHandler_IO,
                          Filenames& filenames,
                          const std::vector<std::shared_ptr<SampleGroupProcessor>>& methods);

  struct SequenceProcessor : IProcessorDescription, IFilenamesHandler {
    explicit SequenceProcessor(SequenceHandler& sh) : sequenceHandler_IO(&sh) {}
    virtual ~SequenceProcessor() = default;

    virtual void process(Filenames& filenames_I);
    
    /* IProcessorDescription */
    virtual ParameterSet getParameterSchema() const override { return ParameterSet(); };
    virtual std::vector<std::string> getRequirements() const override { return {}; };
    virtual std::set<std::string> getInputs() const override { return {}; };
    virtual std::set<std::string> getOutputs() const override { return {}; };

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override {};

    SequenceHandler* sequenceHandler_IO = nullptr; /// Sequence handler, used by all SequenceProcessor derived classes

  protected:
    virtual void doProcess(Filenames& filenames_I) = 0;
  };

  /**
    Apply a processing workflow to all injections in a sequence
  */
  struct ProcessSequence : SequenceProcessor, SequenceProcessorObservable {
    std::map<std::string, Filenames>               filenames_;                     /// Mapping from injection names to pathnames
    std::set<std::string>                          injection_names_;               /// Injections to select from the sequence (all if empty)
    std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods_; /// Events to process

    ProcessSequence() = default;
    explicit ProcessSequence(SequenceHandler& sh, ISequenceProcessorObserver* sequence_processor_observer = nullptr) : SequenceProcessor(sh) 
    {
      addSequenceProcessorObserver(sequence_processor_observer);
    }
    static ParameterSet getParameterSchemaStatic();
    void doProcess(Filenames& filenames_I) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "PROCESS_SEQUENCE"; }
    virtual std::string getDescription() const override { return "Apply a processing workflow to all injections in a sequence"; }
    ParameterSet getParameterSchema() const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override {};
  };

  /**
    Apply a processing workflow to all injections in a sequence segment
  */
  struct ProcessSequenceSegments : SequenceProcessor, SequenceSegmentProcessorObservable {
    std::map<std::string, Filenames>                       filenames_;                             /// Mapping from sequence groups names to pathnames
    std::set<std::string>                                  sequence_segment_names_;                /// Sequence groups to select from the sequence (all if empty)
    std::vector<std::shared_ptr<SequenceSegmentProcessor>> sequence_segment_processing_methods_; /// Events to process

    ProcessSequenceSegments() = default;
    explicit ProcessSequenceSegments(SequenceHandler& sh, ISequenceSegmentProcessorObserver* sequence_segment_processor_observer = nullptr) : SequenceProcessor(sh)
    {
      addSequenceSegmentProcessorObserver(sequence_segment_processor_observer);
    }
    void doProcess(Filenames& filenames_I) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "PROCESS_SEQUENCE_SEGMENTS"; }
    virtual std::string getDescription() const override { return "Apply a processing workflow to all injections in a sequence segment"; }
  };

  /**
    Apply a processing workflow to all injections in a sample group
  */
  struct ProcessSampleGroups : SequenceProcessor, SampleGroupProcessorObservable {
    std::map<std::string, Filenames>                       filenames_;                     /// Mapping from sample groups names to pathnames
    std::set<std::string>                                  sample_group_names_;            /// sample groups to select from the sequence (all if empty)
    std::vector<std::shared_ptr<SampleGroupProcessor>> sample_group_processing_methods_; /// Events to process

    ProcessSampleGroups() = default;
    explicit ProcessSampleGroups(SequenceHandler& sh, ISampleGroupProcessorObserver* sample_group_processor_observer = nullptr) : SequenceProcessor(sh)
    {
      addSampleGroupProcessorObserver(sample_group_processor_observer);
    }
    void doProcess(Filenames& filenames_I) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "PROCESS_SAMPLE_GROUPS"; }
    virtual std::string getDescription() const override { return "Apply a processing workflow to all injections in a sample group"; }
  };

  struct LoadSequence : SequenceProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    LoadSequence() = default;
    explicit LoadSequence(SequenceHandler& sh) : SequenceProcessor(sh) {}
    void doProcess(Filenames& filenames_I) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_SEQUENCE"; }
    virtual std::string getDescription() const override { return "Load a sequence from file"; }

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreSequence : SequenceProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    StoreSequence() = default;
    explicit StoreSequence(SequenceHandler& sh) : SequenceProcessor(sh) {}
    void doProcess(Filenames& filenames_I) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_SEQUENCE"; }
    virtual std::string getDescription() const override { return "Store sequence to file"; }

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct LoadWorkflow : SequenceProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    LoadWorkflow() = default;
    explicit LoadWorkflow(SequenceHandler & sh) : SequenceProcessor(sh) {}
    void doProcess(Filenames& filenames_I) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_WORKFLOW"; }
    virtual std::string getDescription() const override { return "Load a workflow from file"; }

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreWorkflow : SequenceProcessor, IFilePickerHandler
  {
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    StoreWorkflow() = default;
    explicit StoreWorkflow(SequenceHandler& sh) : SequenceProcessor(sh) {}
    void doProcess(Filenames& filenames_I) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_WORKFLOW"; }
    virtual std::string getDescription() const override { return "Store a workflow to file"; }

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

}
