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
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/iface/IPropertiesHandler.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationProcessor : IProcessorDescription, ApplicationProcessorObservable {
    ApplicationProcessor(const ApplicationProcessor& other) = delete;
    ApplicationProcessor& operator=(const ApplicationProcessor& other) = delete;
    virtual ~ApplicationProcessor() = default;

    virtual bool process() = 0;

    /* IProcessorDescription */
    virtual std::string getDescription() const override { return ""; }
    virtual ParameterSet getParameterSchema() const override { return ParameterSet(); };
    virtual std::vector<std::string> getRequirements() const override { return {}; };
    virtual std::set<std::string> getInputs() const override { return {}; };
    virtual std::set<std::string> getOutputs() const override { return {}; };

  protected:
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

}
