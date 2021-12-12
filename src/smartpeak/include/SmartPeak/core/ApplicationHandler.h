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
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/core/SessionLoaderGenerator.h>
#include <map>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct ApplicationHandler final : 
    public IParametersObserver,
    public IWorkflowObserver
  {

  public:

    ApplicationHandler();

    /**
     IParametersObserver
    */
    virtual void onParametersUpdated() override;
    /**
     IWorkflowObserver
    */
    virtual void onWorkflowUpdated() override;

    class Command {
    public:
      enum CommandType {
        RawDataMethod,
        SequenceSegmentMethod,
        SampleGroupMethod,
      } type;

      void setMethod(const std::shared_ptr<RawDataProcessor> method);

      void setMethod(const std::shared_ptr<SequenceSegmentProcessor> method);

      void setMethod(const std::shared_ptr<SampleGroupProcessor> method);

      std::string getName() const;

      ParameterSet getParameterSchema() const;

      std::string getDescription() const;

    public:
      std::shared_ptr<RawDataProcessor> raw_data_method;
      std::shared_ptr<SequenceSegmentProcessor> seq_seg_method;
      std::shared_ptr<SampleGroupProcessor> sample_group_method;
      std::map<std::string, Filenames> dynamic_filenames;

    private:
      const IProcessorDescription* getIProcessorDescription() const;
    };

    /**
    * @brief clear all data in the aapplication handler.
    */
    void closeSession();

    /**
    * @brief check if one session is currently opened
    */
    bool sessionIsOpened() const;

    /**
    * @brief return the saved status of the session
    */
    bool sessionIsSaved() const;

    /**
    * @brief Set one file saved status
    */
    void setFileSavedState(const std::string& file_id, bool saved_state);

    /**
    * @brief returns weither a file is saved
    */
    bool isFileSaved(const std::string& file_id) const;

    /**
    * @brief returns Parameter Schema of the current Workflow
    */
    ParameterSet getWorkflowParameterSchema();

    std::filesystem::path sequence_pathname_;
    std::filesystem::path main_dir_                = ".";
    SequenceHandler       sequenceHandler_;
    Filenames             filenames_;
    std::vector<std::shared_ptr<IFilenamesHandler>> loading_processors_;
    std::vector<std::shared_ptr<IFilenamesHandler>> storing_processors_;
    SessionLoaderGenerator session_loader_generator;

  protected:
    std::map<std::string, bool> saved_files_;
  };


  struct SetRawDataPathname : IFilePickerHandler
  {
    SetRawDataPathname(Filenames* filenames)
    {
      filenames_ = filenames;
    }
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override
    {
      filenames_->setTagValue(Filenames::Tag::MZML_INPUT_PATH, filename.generic_string());
      return true;
    };

    Filenames *filenames_;
  };

  struct SetInputFeaturesPathname : IFilePickerHandler
  {
    SetInputFeaturesPathname(Filenames* filenames)
    {
      filenames_ = filenames;
    }
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override
    {
      filenames_->setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, filename.generic_string());
      return true;
    };
    Filenames *filenames_;
  };

  struct SetOutputFeaturesPathname : IFilePickerHandler
  {
    SetOutputFeaturesPathname(Filenames* filenames)
    {
      filenames_ = filenames;
    }
    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override
    {
      filenames_->setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, filename.generic_string());
      return true;
    }
    Filenames *filenames_;
  };
}
