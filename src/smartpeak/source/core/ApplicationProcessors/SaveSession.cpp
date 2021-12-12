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

#include <SmartPeak/core/ApplicationProcessors/SaveSession.h>
#include <SmartPeak/core/ApplicationProcessors/SavePropertiesHandlers.h>
#include <SmartPeak/core/ApplicationProcessors/StoreFilenames.h>
#include <SmartPeak/core/SequenceProcessor.h>

namespace SmartPeak
{

  bool SaveSession::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    application_handler->filenames_.getSessionDB().setDBFilePath(filename);
    process();
    return true;
  }

  bool SaveSession::process()
  {
    LOGD << "START SaveSession";

    notifyApplicationProcessorStart({ getName() }); // we need to use the workflow manager to profit from the progressbar
    StoreFilenames store_filenames(application_handler_);
    store_filenames.process();

    for (auto& storing_processor : application_handler_.storing_processors_)
    {
      bool to_be_saved = false;
      Filenames filenames;
      storing_processor->getFilenames(filenames);
      for (const auto& file_id : filenames.getFileIds())
      {
        to_be_saved |= (!application_handler_.isFileSaved(file_id));
      }
      if (to_be_saved)
      {
        auto sequence_processor = std::dynamic_pointer_cast<SequenceProcessor>(storing_processor);
        if (sequence_processor)
        {
          sequence_processor->process(application_handler_.filenames_);
        }
        auto raw_data_processor = std::dynamic_pointer_cast<RawDataProcessor>(storing_processor);
        if (raw_data_processor)
        {
          if (!application_handler_.sequenceHandler_.getSequence().empty())
          {
            RawDataHandler& rawDataHandler = application_handler_.sequenceHandler_.getSequence()[0].getRawData();
            raw_data_processor->process(rawDataHandler, {}, application_handler_.filenames_);
          }
          else
          {
            LOGE << "No Sequence available, Storing process aborted.";
          }
        }
        auto sequence_segment_processor = std::dynamic_pointer_cast<SequenceSegmentProcessor>(storing_processor);
        if (sequence_segment_processor)
        {
          if (!application_handler_.sequenceHandler_.getSequenceSegments().empty())
          {
            SequenceSegmentHandler& sequenceSegmentHandler = application_handler_.sequenceHandler_.getSequenceSegments().at(0);
            sequence_segment_processor->sequence_segment_observable_ = &application_handler_.sequenceHandler_;
            sequence_segment_processor->process(sequenceSegmentHandler, SequenceHandler(), {}, application_handler_.filenames_);
          }
          else
          {
            LOGE << "No Sequence Segment available, Storing process aborted.";
          }
        }
        // update saved state
        for (const auto& file_id : filenames.getFileIds())
        {
          application_handler_.setFileSavedState(file_id, true);
        }
      }
    }

    bool success = true;

    SavePropertiesHandlers loading_workflow(application_handler_);
    loading_workflow.properties_handlers = { &application_handler_.session_loader_generator };
    if (!loading_workflow.process())
    {
      LOGE << "Failed to save loading workflow.";
      success = false;
    }

    notifyApplicationProcessorEnd();
    LOGD << "END SaveSession";
    return success;
  }

}
