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

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/iface/IProcessorDescription.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationProcessor : IProcessorDescription {
    ApplicationProcessor(const ApplicationProcessor& other) = delete;
    ApplicationProcessor& operator=(const ApplicationProcessor& other) = delete;
    virtual ~ApplicationProcessor() = default;

    // Each of the derived classes implement the following virtual methods
    virtual bool process() = 0;

    /* IProcessorDescription */
    int getID() const override { return -1; }
    std::string getDescription() const override { return ""; }
    ParameterSet getParameterSchema() const override { return ParameterSet(); };

    ApplicationHandler& application_handler_;

  protected:
    ApplicationProcessor() = default;
    ApplicationProcessor(ApplicationHandler& application_handler) : application_handler_(application_handler) {}
  };

  namespace ApplicationProcessors {
    ParameterSet getParameterSchema();
    void processCommands(ApplicationHandler& application_handler, std::vector<ApplicationHandler::Command> commands, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names, const std::set<std::string>& sample_group_names);
    void processCommands(
        ApplicationHandler& application_handler, 
        std::vector<ApplicationHandler::Command> commands, 
        const std::set<std::string>& injection_names, 
        const std::set<std::string>& sequence_segment_names, 
        const std::set<std::string>& sample_group_names);
  }

  struct CreateCommand : ApplicationProcessor {
    CreateCommand(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string name_;
    ApplicationHandler::Command cmd_; 
    std::string getName() const override { return "CreateCommand"; };
  };

  struct BuildCommandsFromNames : ApplicationProcessor {
    BuildCommandsFromNames(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::vector<std::string> names_;
    std::vector<ApplicationHandler::Command> commands_;
    std::string getName() const override { return "BuildCommandsFromNames"; };
  };

  struct FilePickerProcessor : ApplicationProcessor {
    FilePickerProcessor(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    std::string pathname_;
  };

  struct LoadSessionFromSequence : FilePickerProcessor {
    LoadSessionFromSequence(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSessionFromSequence"; };
  private:
    bool buildStaticFilenames();
    void updateFilenames(Filenames& f, const std::string& pathname);
    bool requiredPathnamesAreValid(const std::vector<InputDataValidation::FilenameInfo>& validation);
    void clearNonExistantDefaultGeneratedFilenames(Filenames& f);
    void clearNonExistantFilename(std::string& filename);
    std::string getValidPathnameOrPlaceholder(const std::string& pathname, const bool is_valid);
  };

  struct LoadSequenceParameters : FilePickerProcessor {
    LoadSequenceParameters(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceParameters"; };
  };

  struct LoadSequenceTransitions : FilePickerProcessor {
    LoadSequenceTransitions(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceTransitions"; };
  };

  struct LoadSequenceValidationData : FilePickerProcessor {
    LoadSequenceValidationData(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceValidationData"; };
  };

  struct LoadSequenceSegmentQuantitationMethods : FilePickerProcessor {
    LoadSequenceSegmentQuantitationMethods(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentQuantitationMethods"; };
  };

  struct LoadSequenceSegmentStandardsConcentrations : FilePickerProcessor {
    LoadSequenceSegmentStandardsConcentrations(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentStandardsConcentrations"; };
  };

  struct LoadSequenceSegmentFeatureFilterComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureFilterComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureFilterComponents"; };
  };

  struct LoadSequenceSegmentFeatureFilterComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureFilterComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureFilterComponentGroups"; };
  };

  struct LoadSequenceSegmentFeatureQCComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureQCComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureQCComponents"; };
  };

  struct LoadSequenceSegmentFeatureQCComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureQCComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureQCComponentGroups"; };
  };

  struct LoadSequenceSegmentFeatureRSDFilterComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureRSDFilterComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureRSDFilterComponents"; };
  };

  struct LoadSequenceSegmentFeatureRSDFilterComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureRSDFilterComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureRSDFilterComponentGroups"; };
  };

  struct LoadSequenceSegmentFeatureRSDQCComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureRSDQCComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureRSDQCComponents"; };
  };

  struct LoadSequenceSegmentFeatureRSDQCComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureRSDQCComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureRSDQCComponentGroups"; };
  };

  struct LoadSequenceSegmentFeatureBackgroundFilterComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureBackgroundFilterComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureBackgroundFilterComponents"; };
  };

  struct LoadSequenceSegmentFeatureBackgroundFilterComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureBackgroundFilterComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureBackgroundFilterComponentGroups"; };
  };

  struct LoadSequenceSegmentFeatureBackgroundQCComponents : FilePickerProcessor {
    LoadSequenceSegmentFeatureBackgroundQCComponents(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureBackgroundQCComponents"; };
  };

  struct LoadSequenceSegmentFeatureBackgroundQCComponentGroups : FilePickerProcessor {
    LoadSequenceSegmentFeatureBackgroundQCComponentGroups(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceSegmentFeatureBackgroundQCComponentGroups"; };
  };

  struct StoreSequenceFileAnalyst : ApplicationProcessor {
    StoreSequenceFileAnalyst(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "StoreSequenceFileAnalyst"; };
  };

  struct StoreSequenceFileMasshunter : ApplicationProcessor {
    StoreSequenceFileMasshunter(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "StoreSequenceFileMasshunter"; };
  };

  struct StoreSequenceFileXcalibur : ApplicationProcessor {
    StoreSequenceFileXcalibur(ApplicationHandler& application_handler) : ApplicationProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "StoreSequenceFileXcalibur"; };
  };

  struct SetRawDataPathname : FilePickerProcessor {
    SetRawDataPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "SetRawDataPathname"; };
  };

  struct SetInputFeaturesPathname : FilePickerProcessor {
    SetInputFeaturesPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "SetInputFeaturesPathname"; };
  };

  struct SetOutputFeaturesPathname : FilePickerProcessor {
    SetOutputFeaturesPathname(ApplicationHandler& application_handler) : FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "SetOutputFeaturesPathname"; };
  };

  struct LoadSequenceWorkflow : FilePickerProcessor {
    LoadSequenceWorkflow(ApplicationHandler& application_handler) : 
      FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "LoadSequenceWorkflow"; };
  };

  struct StoreSequenceWorkflow : FilePickerProcessor {
    StoreSequenceWorkflow(ApplicationHandler& application_handler) :
      FilePickerProcessor(application_handler) {}
    bool process() override;
    std::string getName() const override { return "StoreSequenceWorkflow"; };
  };
}
