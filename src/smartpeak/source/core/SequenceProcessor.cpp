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
  
  class WorkflowException : public std::exception
  {
  public:

    explicit WorkflowException(const std::string& item, const std::string& processor, const std::string& message)
      : item_(item), processor_(processor), msg_(message)  {}

    virtual ~WorkflowException() noexcept {}

    virtual const char* what() const noexcept override {
      return msg_.c_str();
    }

    virtual const std::string item() const noexcept {
      return item_;
    }

    virtual const std::string processor() const noexcept {
      return processor_;
    }

  protected:
    std::string item_;
    std::string processor_;
    std::string msg_;
  };

  int nbThreads(const std::vector<InjectionHandler> injections)
  {
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
    return n_threads;
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

  void ProcessSequence::process(Filenames& filenames_I)
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

    // Launch
    // notifySequenceProcessorStart(injections.size());
    SequenceProcessorMultithread manager(
      injections,
      filenames_,
      raw_data_processing_methods_,
      this);
    int n_threads = nbThreads(injections);
    manager.spawn_workers(n_threads);
    //notifySequenceProcessorEnd();
  }

  void ProcessSequenceSegments::process(Filenames& filenames_I)
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

    // handle user-desired sequence_segment_processing_methods
    if (!sequence_segment_processing_methods_.size()) {
      throw "no sequence segment processing methods given.\n";
    }

    // Launch
    //notifySequenceSegmentProcessorStart(sequence_segments.size());
    SequenceSegmentProcessorMultithread manager(
      sequence_segments,
      (*sequenceHandler_IO),
      sequence_segment_processing_methods_,
      filenames_,
      this);
    int n_threads = nbThreads(sequenceHandler_IO->getSequence());
    manager.spawn_workers(n_threads);
    sequenceHandler_IO->setSequenceSegments(sequence_segments);
    //notifySequenceSegmentProcessorEnd();
  }

  void processSegment(
    SequenceSegmentHandler& sequence_segment,
    SequenceHandler& sequenceHandler_IO,
    Filenames& filenames,
    const std::vector<std::shared_ptr<SequenceSegmentProcessor>>& methods)
  {
    const size_t nr_methods = methods.size();
    LOGI << ">>Processing SequenceSegment [" << sequence_segment.getSequenceSegmentName() << "]\n";
    for (size_t i = 0; i < nr_methods; ++i) {
      LOGI << "[" << (i + 1) << "/" << nr_methods << "] steps in processing sequence segments";
      try
      {
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
      catch (const std::exception& e)
      {
        LOGE << e.what();
        WorkflowException we(sequence_segment.getSequenceSegmentName(), methods[i]->getName(), e.what());
        throw we;
      }
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
      //if (observable_) observable_->notifySequenceSegmentProcessorSampleStart(sequence_seg.getSequenceSegmentName());
      
      try {
        std::future<void> f = std::async(
        std::launch::async,
        processSegment,
        std::ref(sequence_seg),
        std::ref(sequenceHandler_IO),
        std::ref(filenames_.at( sequence_seg.getSequenceSegmentName())),
        std::cref(sequence_segment_processing_methods_));
          
        LOGI << ">>SequenceSegment [" << sequence_seg.getSequenceSegmentName() << "]: waiting...";
        f.wait();
        LOGI << ">>SequenceSegment [" << sequence_seg.getSequenceSegmentName() << "]: done";
          
        try {
          f.get();
        }
        catch (const WorkflowException& e)
        {
          /*
          if (observable_)
          {
            observable_->notifySequenceSegmentProcessorError(
              e.item(),
              e.processor(),
              e.what());
          }
          else
          */
          {
            LOGE << "SequenceSegment [" << "i" << "]: " << typeid(e).name() << " : " << e.what();
          }
        }
          
        //if (observable_) observable_->notifySequenceSegmentProcessorSampleEnd(sequence_seg.getSequenceSegmentName());
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
      //if (observable_) observable_->notifySampleGroupProcessorSampleStart(sequence_seg.getSampleGroupName());
      
      try {
        std::future<void> f = std::async(
        std::launch::async,
        processSampleGroup,
        std::ref(sequence_seg),
        std::ref(sequenceHandler_IO),
        std::ref(filenames_.at( sequence_seg.getSampleGroupName())),
        std::cref(sample_group_processing_methods_));
          
        LOGI << ">>SampleGroup [" << sequence_seg.getSampleGroupName() << "]: waiting...";
        f.wait();
        LOGI << ">>SampleGroup [" << sequence_seg.getSampleGroupName() << "]: done";
          
        try {
          f.get();
        }
        catch (const WorkflowException& e)
        {
          /*
          if (observable_)
          {
            observable_->notifySampleGroupProcessorError(
              e.item(),
              e.processor(),
              e.what());
          }
          else
          */
          {
            LOGE << "SampleGroup [" << "i" << "]: " << typeid(e).name() << " : " << e.what();
          }
        }
          
        if (observable_) observable_->notifySampleGroupProcessorSampleEnd(sequence_seg.getSampleGroupName());
      }
      catch (const std::exception& e) {
        LOGE << "SampleGroup [" << "i" << "]: " << typeid(e).name() << " : " << e.what();
      }
    }
  }

  void ProcessSampleGroups::process(Filenames& filenames_I)
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

    // Launch
    // notifySampleGroupProcessorStart(sample_groups.size());
    SampleGroupProcessorMultithread manager(
      sample_groups,
      (*sequenceHandler_IO),
      sample_group_processing_methods_,
      filenames_,
      this);
    int n_threads = nbThreads(sequenceHandler_IO->getSequence());
    manager.spawn_workers(n_threads);
    sequenceHandler_IO->setSampleGroups(sample_groups);
    // notifySampleGroupProcessorEnd();
  }

  void processSampleGroup(
    SampleGroupHandler& sample_group,
    SequenceHandler& sequenceHandler_IO,
    Filenames& filenames,
    const std::vector<std::shared_ptr<SampleGroupProcessor>>& methods)
  {
    const size_t n = methods.size();
    for (size_t i = 0; i < n; ++i) {
      LOGI << "[" << (i + 1) << "/" << n << "] steps in processing sample groups";
      try
      {
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
      catch (const std::exception& e)
      {
        LOGE << e.what();
        WorkflowException we(sample_group.getSampleGroupName(), methods[i]->getName(), e.what());
        throw we;
      }
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
      //if (observable_) observable_->notifySequenceProcessorSampleStart(injection.getMetaData().getSampleName());
      try {
        std::future<void> f = std::async(
          std::launch::async,
          processInjection,
          std::ref(injection),
          std::ref(filenames_.at(injection.getMetaData().getInjectionName())),
          std::cref(methods_));
        LOGD << "Injection [" << i << "]: waiting...";
        f.wait();
        LOGD << "Injection [" << i << "]: done";
        try {
          f.get(); // check for exceptions
        }
        catch (const WorkflowException& e)
        {
          /*
          if (observable_)
          {
            observable_->notifySequenceProcessorError(
              e.item(),
              e.processor(),
              e.what());
          }
          else
          */
          {
            LOGE << "Injection [" << i << "]: " << typeid(e).name() << " : " << e.what();
          }
        }
        //if (observable_) observable_->notifySequenceProcessorSampleEnd(injection.getMetaData().getSampleName());
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
    Filenames& filenames_I,
    const std::vector<std::shared_ptr<RawDataProcessor>>& methods
  )
  {
    size_t i_step { 1 };
    const size_t n_steps { methods.size() };
    const std::string inj_name { injection.getMetaData().getInjectionName() };
    for (const std::shared_ptr<RawDataProcessor>& p : methods) {
      try
      {
        LOGI << "[" << (i_step++) << "/" << n_steps << "] method on injection: " << inj_name;
        p->process( //TODO: (SIGABRT)
          injection.getRawData(),
          injection.getRawData().getParameters(),
          filenames_I
        );
      }
      catch (const std::exception& e)
      {
        LOGE << e.what();
        WorkflowException we(inj_name, p->getName(), e.what());
        throw we;
      }
    }
  }

  bool LoadSequence::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    filenames.setFullPath("sequence", filename);
    process(filenames);
    return true;
  }

  void LoadSequence::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("sequence", "${MAIN_DIR}/sequence.csv", "Injections", true, true);
  };

  void LoadSequence::process(Filenames& filenames_I)
  {
    LOGD << "START LoadSequence";
    getFilenames(filenames_I);
    if (!InputDataValidation::prepareToLoad(filenames_I, "sequence"))
    {
      LOGD << "END " << getName();
      return;
    }

    try
    {
      if (filenames_I.isEmbedded("sequence"))
      {
        int rc;
        sqlite3_stmt* stmt;
        auto db_context = filenames_I.getSessionDB().beginRead(
          "sequence",
          "sample_name",
          "sample_group_name",
          "sequence_segment_name",
          "replicate_group_name",
          "sample_type",
          "original_filename",
          "proc_method_name",
          "rack_number",
          "plate_number",
          "pos_number",
          "inj_number",
          "dilution_factor",
          "acq_method_name",
          "acquisition_date_and_time",
          "inj_volume",
          "inj_volume_units",
          "batch_name",
          "scan_polarity",
          "scan_mass_low",
          "scan_mass_high"
        );
        if (!db_context)
        {
          return;
        }
        MetaDataHandler t; // as in temporary
        std::string sample_name;
        std::string sample_group_name;
        std::string sequence_segment_name;
        std::string replicate_group_name;
        std::string sample_type;
        std::string filename;
        std::string acquisition_date_and_time;
        while (filenames_I.getSessionDB().read(
          *db_context,
          sample_name,
          sample_group_name,
          sequence_segment_name,
          replicate_group_name,
          sample_type,
          filename,
          t.proc_method_name,
          t.rack_number,
          t.plate_number,
          t.pos_number,
          t.inj_number,
          t.dilution_factor,
          t.acq_method_name,
          acquisition_date_and_time,
          t.inj_volume,
          t.inj_volume_units,
          t.batch_name,
          t.scan_polarity,
          t.scan_mass_low,
          t.scan_mass_high
        ))
        {
          t.setSampleName(sample_name);
          t.setSampleGroupName(sample_group_name);
          t.setSequenceSegmentName(sequence_segment_name);
          t.setReplicateGroupName(replicate_group_name);
          t.setSampleType(stringToSampleType.at(sample_type));
          t.setFilename(filename);
          t.setAcquisitionDateAndTimeFromString(acquisition_date_and_time);
          sequenceHandler_IO->addSampleToSequence(t, OpenMS::FeatureMap());
        }
        filenames_I.getSessionDB().endRead(*db_context);
      }
      else
      {
        SequenceParser::readSequenceFile(*sequenceHandler_IO, filenames_I.getFullPath("sequence"), ",");
        if (sequenceHandler_IO->getSequence().empty()) {
          LOGE << "Empty sequence. Returning";
          LOGD << "END createSequence";
          return;
        }
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }
    LOGD << "END LoadSequence";
  }

  void StoreSequence::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("sequence", "${MAIN_DIR}/sequence.csv", "Injections", true, true);
  };

  bool StoreSequence::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    filenames.setFullPath("sequence", filename);
    process(filenames);
    return true;
  }

  void StoreSequence::process(Filenames& filenames_I)
  {
    LOGD << "START StoreSequence";

    if (!InputDataValidation::prepareToStore(filenames_I, "sequence"))
    {
      LOGD << "END " << getName();
      return;
    }
    if (filenames_I.isEmbedded("sequence"))
    {
      auto db_context = filenames_I.getSessionDB().beginWrite(
        "sequence",
        "sample_name", "TEXT",
        "sample_group_name", "TEXT",
        "sequence_segment_name", "TEXT",
        "replicate_group_name", "TEXT",
        "sample_type", "TEXT",
        "original_filename", "TEXT",
        "proc_method_name", "TEXT",
        "rack_number", "INT",
        "plate_number", "INT",
        "pos_number", "INT",
        "inj_number", "REAL",
        "dilution_factor", "INT",
        "acq_method_name", "TEXT",
        "acquisition_date_and_time", "TEXT",
        "inj_volume", "INT",
        "inj_volume_units", "TEXT",
        "batch_name", "TEXT",
        "scan_polarity", "TEXT",
        "scan_mass_low", "REAL",
        "scan_mass_high", "REAL"
      );
      if (!db_context)
      {
        return;
      }
      for (const auto& injection : sequenceHandler_IO->getSequence())
      {
        const auto& meta_data = injection.getMetaData();
        filenames_I.getSessionDB().write(
          *db_context,
        meta_data.getSampleName(),
        meta_data.getSampleGroupName(),
        meta_data.getSequenceSegmentName(),
        meta_data.getReplicateGroupName(),
        sampleTypeToString.at(meta_data.getSampleType()),
        meta_data.getFilename(),
        meta_data.proc_method_name,
        meta_data.rack_number,
        meta_data.plate_number,
        meta_data.pos_number,
        meta_data.inj_number,
        meta_data.dilution_factor,
        meta_data.acq_method_name,
        meta_data.getAcquisitionDateAndTimeAsString(),
        meta_data.inj_volume,
        meta_data.inj_volume_units,
        meta_data.batch_name,
        meta_data.scan_polarity,
        meta_data.scan_mass_low,
        meta_data.scan_mass_high
        );
      }
      filenames_I.getSessionDB().endWrite(*db_context);
    }
    else
    {
      LOGI << "Storing " << filenames_I.getFullPath("sequence").generic_string();
      if (sequenceHandler_IO->getSequence().size() == 0)
      {
        LOGE << "File cannot be stored without first loading the sequence.";
      }
      SequenceParser::writeSequenceFileSmartPeak(*sequenceHandler_IO, filenames_I.getFullPath("sequence").generic_string().c_str());
    }
    LOGD << "END StoreSequence";
  }

  bool LoadWorkflow::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    filenames.setFullPath("workflow", filename);
    process(filenames);
    return true;
  }

  void LoadWorkflow::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("workflow", "${MAIN_DIR}/workflow.csv", "Workflow", true, true);
  };

  void LoadWorkflow::process(Filenames& filenames_I)
  {
    LOGD << "START LoadWorkflow";
    getFilenames(filenames_I);
    if (!InputDataValidation::prepareToLoad(filenames_I, "workflow"))
    {
      LOGD << "END " << getName();
      return;
    }

    std::vector<std::string> res;
    try 
    {
      if (filenames_I.isEmbedded("workflow"))
      {
        auto db_context = filenames_I.getSessionDB().beginRead(
          "workflow",
          "command_name"
        );
        if (!db_context)
        {
          return;
        }
        std::string step;
        while (filenames_I.getSessionDB().read(
          *db_context,
          step
        ))
        {
          res.push_back(step);
        }
        filenames_I.getSessionDB().endRead(*db_context);
      }
      else
      {
        io::CSVReader<1, io::trim_chars<>, io::no_quote_escape<','>> in(filenames_I.getFullPath("workflow").generic_string());
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

  void StoreWorkflow::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("workflow", "${MAIN_DIR}/workflow.csv", "Workflow", true, true);
  };

  bool StoreWorkflow::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    Filenames filenames;
    filenames.setFullPath("workflow", filename);
    process(filenames);
    return true;
  }

  void StoreWorkflow::process(Filenames& filenames_I)
  {
    LOGD << "START StoreWorkflow";

    if (!InputDataValidation::prepareToStore(filenames_I, "workflow"))
    {
      LOGD << "END " << getName();
      return;
    }
    if (filenames_I.isEmbedded("workflow"))
    {
      auto db_context = filenames_I.getSessionDB().beginWrite(
        "workflow",
        "command_name", "TEXT"
      );
      if (!db_context)
      {
        return;
      }
      const auto& steps = sequenceHandler_IO->getWorkflow();
      for (const auto& step : steps)
      {
        filenames_I.getSessionDB().write(
          *db_context,
          step
        );
      }
      filenames_I.getSessionDB().endWrite(*db_context);
    }
    else
    {
      LOGI << "Storing " << filenames_I.getFullPath("workflow").generic_string();

      std::vector<std::string> headers = { "command_name" };
      CSVWriter writer(filenames_I.getFullPath("workflow").generic_string(), ",");
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
    }
    LOGD << "END StoreWorkflow";
  }
}
