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
    auto filenames = LoadFilenames::loadFilenamesFromDB(filename);
    auto main_dir = std::filesystem::path(filename).remove_filename().generic_string();
    filenames->setTag(Filenames::Tag::MAIN_DIR, main_dir);
    // Popup Session Files management if some files are not existing
    if (filenames)
    {
      bool missing_file = false;
      for (const auto& file_id : (*filenames).getFileIds())
      {
        const auto full_path = (*filenames).getFullPath(file_id);
        if ((!(*filenames).isEmbedded(file_id)) && (!full_path.empty()) && (!std::filesystem::exists(full_path)))
        {
          missing_file = true;
          break;
        }
      }
      if (missing_file)
      {
        session_files_widget_manage_->open(*filenames);
      }
      else
      {
        application_handler->closeSession();
        application_handler->filenames_ = *filenames;
        application_handler->main_dir_ = filenames->getTag(Filenames::Tag::MAIN_DIR);
        LoadSession load_session(
          *application_handler,
          workflow_manager_,
          application_observer_,
          sequence_processor_observer_,
          sequence_segment_processor_observer_,
          sample_group_processor_observer_);
        load_session.to_serialize = to_serialize_;
        load_session.addApplicationProcessorObserver(application_observer_);
        load_session.filenames_ = filenames;
        load_session.notifyApplicationProcessorStart({ load_session.getName() }); // we need a proper loading in thread to profit from the progressbar
        if (!load_session.process())
        {
          // load failed
          application_handler->closeSession();
        }
        load_session.notifyApplicationProcessorEnd();
      }
    }
    return true;
  };
}
