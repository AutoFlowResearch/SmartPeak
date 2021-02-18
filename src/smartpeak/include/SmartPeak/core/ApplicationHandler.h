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

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/core/WorkflowObservable.h>
#include <SmartPeak/core/ParametersObservable.h>
#include <map>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationHandler final : public ParametersObservable, public WorkflowObservable {
    class Command {
    public:
      enum CommandType {
        RawDataMethod,
        SequenceSegmentMethod,
        SampleGroupMethod,
      } type;

      void setMethod(const std::shared_ptr<RawDataProcessor> method)
      {
        type = RawDataMethod;
        raw_data_method = method;
      }

      void setMethod(const std::shared_ptr<SequenceSegmentProcessor> method)
      {
        type = SequenceSegmentMethod;
        seq_seg_method = method;
      }

      void setMethod(const std::shared_ptr<SampleGroupProcessor> method)
      {
        type = SampleGroupMethod;
        sample_group_method = method;
      }

      int getID() const
      {
        const auto description = getIProcessorDescription();
        return (description ? description->getID() : 0);
      }

      std::string getName() const
      {
        const auto description = getIProcessorDescription();
        return (description ? description->getName() : "");
      }

      ParameterSet getParameterSchema() const
      {
        const auto description = getIProcessorDescription();
        return (description ? description->getParameterSchema() : ParameterSet());
      }

      std::string getDescription() const
      {
        const auto description = getIProcessorDescription();
        return (description ? description->getDescription() : "");
      }

      std::shared_ptr<RawDataProcessor> raw_data_method;
      std::shared_ptr<SequenceSegmentProcessor> seq_seg_method;
      std::shared_ptr<SampleGroupProcessor> sample_group_method;

      std::map<std::string, Filenames> dynamic_filenames;

    private:
      const IProcessorDescription* getIProcessorDescription() const
      {
        switch (type)
        {
        case RawDataMethod:
          return raw_data_method.get();
        case SequenceSegmentMethod:
          return seq_seg_method.get();
        case SampleGroupMethod:
          return sample_group_method.get();
        default:
          throw "Unsupported CommandType in ApplicationHandler::Command::getIProcessorDescription()";
        }
      }
    };

    std::string                           sequence_pathname_;
    std::string                           main_dir_                = ".";
    std::string                           mzML_dir_;
    std::string                           features_in_dir_;
    std::string                           features_out_dir_;
    Filenames                             static_filenames_;
    SequenceHandler                       sequenceHandler_;
  };
}
