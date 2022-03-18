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

#include <SmartPeak/core/ApplicationProcessors/CreateCommand.h>
#include <SmartPeak/core/SharedProcessors.h>

namespace SmartPeak
{

  bool CreateCommand::process() {
    // Enumerate the valid command keys
    std::vector<std::string> valid_commands_raw_data_processor;
    for (const auto& it: n_to_raw_data_method_) { valid_commands_raw_data_processor.push_back(it.first); }
    std::vector<std::string> valid_commands_sequence_segment_processor;
    for (const auto& it: n_to_seq_seg_method_) { valid_commands_sequence_segment_processor.push_back(it.first); }
    std::vector<std::string> valid_commands_sample_group_processor;
    for (const auto& it : n_to_sample_group_method_) { valid_commands_sample_group_processor.push_back(it.first); }

    // Run the command depending on whether it is a raw data processor method or sequence segment processor method
    if (std::count(valid_commands_raw_data_processor.begin(), valid_commands_raw_data_processor.end(), name_)) {
      const auto& method = n_to_raw_data_method_.at(name_);
      cmd_.setMethod(method);
      Filenames method_filenames = application_handler_.filenames_;
      for (const InjectionHandler& injection : application_handler_.sequenceHandler_.getSequence()) {
        const std::string& key = injection.getMetaData().getInjectionName();
        cmd_.dynamic_filenames[key] = method_filenames;
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, injection.getMetaData().getFilename());
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
      }
    } else if (std::count(valid_commands_sequence_segment_processor.begin(), valid_commands_sequence_segment_processor.end(), name_)) {
      const auto& method = n_to_seq_seg_method_.at(name_);
      cmd_.setMethod(method);
      Filenames method_filenames = application_handler_.filenames_;
      for (const SequenceSegmentHandler& sequence_segment : application_handler_.sequenceHandler_.getSequenceSegments()) {
        const std::string& key = sequence_segment.getSequenceSegmentName();
        cmd_.dynamic_filenames[key] = method_filenames;
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, "");
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_SEQUENCE_SEGMENT_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::OUTPUT_SEQUENCE_SEGMENT_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_GROUP_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, key);
      }
    } else if (std::count(valid_commands_sample_group_processor.begin(), valid_commands_sample_group_processor.end(), name_)) {
      const auto& method = n_to_sample_group_method_.at(name_);
      cmd_.setMethod(method);
      Filenames method_filenames = application_handler_.filenames_;
      for (const SampleGroupHandler& sample_group : application_handler_.sequenceHandler_.getSampleGroups()) {
        const std::string& key = sample_group.getSampleGroupName();
        cmd_.dynamic_filenames[key] = method_filenames;
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, "");
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::INPUT_GROUP_NAME, key);
        cmd_.dynamic_filenames[key].setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, key);
      }
    }
    else 
    {
      LOGE << "No command for selection name " << name_;
      return false;
    }
    return true;
  }

}
