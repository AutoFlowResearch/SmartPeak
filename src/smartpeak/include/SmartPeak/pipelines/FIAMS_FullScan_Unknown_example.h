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

#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;

void example_FIAMS_FullScan_Unknowns(
  const std::string& dir_I,
  const Filenames& static_filenames,
  const std::string& delimiter_I = ","
)
{
  SequenceHandler sequenceHandler;

  CreateSequence cs(sequenceHandler);
  cs.filenames_        = static_filenames;
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
  methods_filenames.setRootPaths(dir_I,
    dir_I + "/mzML/",
    dir_I + "/features/",
    dir_I + "/features/");
  for (const auto& m : raw_data_processing_methods)
  {
    m->getInputsOutputs(methods_filenames);
  }
  std::map<std::string, Filenames> dynamic_filenames;
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    Filenames injection_filenames = methods_filenames;
    injection_filenames.setFileVariants(
      injection.getMetaData().getFilename(),
      key,
      key,
      injection.getMetaData().getSampleGroupName(),
      injection.getMetaData().getSampleGroupName()
    );
     dynamic_filenames[key] = injection_filenames;
  }

  ProcessSequence ps(sequenceHandler);
  ps.filenames_                     = dynamic_filenames;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.process();
}
