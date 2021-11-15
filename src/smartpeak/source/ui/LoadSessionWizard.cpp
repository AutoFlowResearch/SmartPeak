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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/LoadSessionWizard.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <plog/Log.h>

namespace SmartPeak
{
  bool LoadSessionWizard::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    loaded_filenames_ = LoadFilenames::loadFilenamesFromDB(filename);
    if (!loaded_filenames_)
    {
      notifyApplicationProcessorError("Failed to load session file.");
      return false;
    }
    // Popup Session Files management if some files are not existing
    auto main_dir = std::filesystem::path(filename).remove_filename().generic_string();
    loaded_filenames_->setTagValue(Filenames::Tag::MAIN_DIR, main_dir);
    missing_input_output_directories_ = missingInputOutputDirectories();
    if (missing_input_output_directories_)
    {
      set_input_output_widget->open(*loaded_filenames_, this);
    }
    else
    {
      onInputOutputSet();
    }
    return true;
  };

  bool LoadSessionWizard::missingInputFileNames()
  {
    for (const auto& file_id : (*loaded_filenames_).getFileIds())
    {
      const auto full_path = (*loaded_filenames_).getFullPath(file_id);
      if ((!(*loaded_filenames_).isEmbedded(file_id)) && (!full_path.empty()) && (!std::filesystem::exists(full_path)))
      {
        return true;
        break;
      }
    }
    return false;
  }

  void LoadSessionWizard::onInputOutputSet()
  {
    missing_input_file_ = missingInputFileNames();
    if (missing_input_file_)
    {
      session_files_widget_manage_->open(*loaded_filenames_);
    }
    else
    {
      application_handler_.closeSession();
      application_handler_.filenames_ = *loaded_filenames_;
      application_handler_.main_dir_ = loaded_filenames_->getTagValue(Filenames::Tag::MAIN_DIR);
      LoadSession load_session(
        application_handler_,
        workflow_manager_,
        application_processor_observer_,
        sequence_processor_observer_,
        sequence_segment_processor_observer_,
        sample_group_processor_observer_);
      load_session.addApplicationProcessorObserver(application_processor_observer_);
      load_session.filenames_ = loaded_filenames_;
      load_session.notifyApplicationProcessorStart({ load_session.getName() }); // we need to use the workflow manager to profit from the progressbar
      if (!load_session.process())
      {
        // load failed
        application_handler_.closeSession();
        load_session.notifyApplicationProcessorError("Failed to load session.");
      }
      load_session.notifyApplicationProcessorEnd();
    }
  }

  void LoadSessionWizard::onInputOutputCancel()
  {
  }

  bool LoadSessionWizard::missingInputOutputDirectories()
  {
    std::vector<Filenames::Tag> tag_to_check = { 
      Filenames::Tag::FEATURES_INPUT_PATH, 
      Filenames::Tag::FEATURES_OUTPUT_PATH, 
      Filenames::Tag::MZML_INPUT_PATH
    };
    for (const auto& tag : tag_to_check)
    {
      const auto tag_path = std::filesystem::path((*loaded_filenames_).getTagValue(tag));
      if ((!tag_path.empty()) && (!std::filesystem::exists(tag_path)))
      {
        return true;
        break;
      }
    }
    return false;
  }

}
