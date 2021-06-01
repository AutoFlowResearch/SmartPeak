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
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <SmartPeak/core/ApplicationProcessorObservable.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationProcessor : IProcessorDescription, ApplicationProcessorObservable {
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
    void processCommands(ApplicationHandler& application_handler, 
      std::vector<ApplicationHandler::Command> commands, 
      const std::set<std::string>& injection_names, 
      const std::set<std::string>& sequence_segment_names, 
      const std::set<std::string>& sample_group_names,
      IApplicationProcessorObserver* application_processor_observer = nullptr,
      ISequenceProcessorObserver* sequence_processor_observer = nullptr,
      ISequenceSegmentProcessorObserver* sequence_segment_processor_observer = nullptr,
      ISampleGroupProcessorObserver* sample_group_processor_observer = nullptr);
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

}
