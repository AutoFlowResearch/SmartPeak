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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;

void example_FIAMS_FullScan_Unknowns(
  const std::string& dir_I,
  Filenames& filenames_I,
  const std::string& delimiter_I = ","
)
{
  ApplicationHandler application_handler;
  LoadSession cs(application_handler);
  cs.filenames_        = filenames_I;
  cs.delimiter        = delimiter_I;
  cs.checkConsistency = true;
  cs.process();

  const std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = {
    std::make_shared<LoadRawData>(),
    std::make_shared<ExtractSpectraWindows>(),
    std::make_shared<MergeSpectra>(),
    std::make_shared<PickMS1Features>(),
    std::make_shared<SearchAccurateMass>(),
    std::make_shared<StoreAnnotations>(),
    std::make_shared<StoreFeatures>()
  };

  Filenames methods_filenames;
  methods_filenames.setTag(Filenames::Tag::MAIN_DIR, dir_I);
  methods_filenames.setTag(Filenames::Tag::MZML_INPUT_PATH, dir_I + "/mzML/");
  methods_filenames.setTag(Filenames::Tag::FEATURES_INPUT_PATH, dir_I + "/features/");
  methods_filenames.setTag(Filenames::Tag::FEATURES_OUTPUT_PATH, dir_I + "/features/");

  std::map<std::string, Filenames> dynamic_filenames;
  for (const InjectionHandler& injection : application_handler.sequenceHandler_.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    dynamic_filenames[key] = methods_filenames;
    dynamic_filenames[key].setTag(Filenames::Tag::INPUT_MZML_FILENAME, injection.getMetaData().getFilename());
    dynamic_filenames[key].setTag(Filenames::Tag::INPUT_INJECTION_NAME, key);
    dynamic_filenames[key].setTag(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
    dynamic_filenames[key].setTag(Filenames::Tag::INPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
    dynamic_filenames[key].setTag(Filenames::Tag::OUTPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
  }

  ProcessSequence ps(application_handler.sequenceHandler_);
  ps.filenames_                     = dynamic_filenames;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.process(methods_filenames);
}
