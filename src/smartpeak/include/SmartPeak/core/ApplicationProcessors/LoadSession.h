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

#pragma once

#include <SmartPeak/core/ApplicationProcessor.h>

namespace SmartPeak
{

  struct LoadSession : ApplicationProcessor, IFilePickerHandler
  {
    /* IFilePickerHandler */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    std::optional<Filenames>    filenames_;           /// Pathnames to load - if not set, read it from session
    std::optional<Filenames>    filenames_override_;  /// Pathnames override
    std::optional<ParameterSet> parameters_override_; /// Parameters override
    std::optional<std::vector<std::string>> workflow_override_; /// Workflow override
    std::string      delimiter = ",";                 /// String delimiter of the imported file
    bool             checkConsistency = true;         /// Check consistency of data contained in files

    LoadSession() = default;
    explicit LoadSession(
      ApplicationHandler& application_handler, 
      WorkflowManager& workflow_manager,
      IApplicationProcessorObserver* application_processor_observer = nullptr,
      ISequenceProcessorObserver* sequence_processor_observer = nullptr,
      ISequenceSegmentProcessorObserver* sequence_segment_processor_observer = nullptr,
      ISampleGroupProcessorObserver* sample_group_processor_observer = nullptr)
      : ApplicationProcessor(application_handler),
        workflow_manager_(workflow_manager),
        application_processor_observer_(application_processor_observer),
        sequence_processor_observer_(sequence_processor_observer),
        sequence_segment_processor_observer_(sequence_segment_processor_observer),
        sample_group_processor_observer_(sample_group_processor_observer)
    {}

    /* ApplicationProcessor */
    virtual bool doProcess() override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_SESSION"; }
    virtual std::string getDescription() const override { return "Load an existing session"; }

  protected:
    bool overrideFilenames();
    bool overrideParameters();
    bool overrideWorkflow();
    bool readFilenames();
    bool readInputFiles();
    bool readLoadingWorkflow();
    bool runLoadingWorkflow();
    WorkflowManager& workflow_manager_;
    IApplicationProcessorObserver* application_processor_observer_;
    ISequenceProcessorObserver* sequence_processor_observer_;
    ISequenceSegmentProcessorObserver* sequence_segment_processor_observer_;
    ISampleGroupProcessorObserver* sample_group_processor_observer_;
  };

}
