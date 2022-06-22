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
#include <SmartPeak/core/RawDataProcessors/LoadMSP.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FORMAT/MSPGenericFile.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> LoadMSP::getRequirements() const
  {
    return { "sequence" };
  }

  std::set<std::string> LoadMSP::getInputs() const
  {
    return {  };
  }

  std::set<std::string> LoadMSP::getOutputs() const
  {
    return { "Library" };
  }

  bool LoadMSP::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    RawDataHandler& rawDataHandler = application_handler->sequenceHandler_.getSequence().at(0).getRawData();
    library_observable_ = &(application_handler->sequenceHandler_);
    Filenames filenames;
    filenames.setFullPath("msp", filename);
    process(rawDataHandler, {}, filenames);
    return true;
  }

  void LoadMSP::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("msp", "${MAIN_DIR}/library.msp", "Library", false, false);
  }

  void LoadMSP::doProcess(RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);

    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    if (!InputDataValidation::prepareToLoad(filenames_I, "msp", true))
    {
      throw std::invalid_argument("Failed to load input file");
    }

    try {
      // Load spectral library for downstream spectral matching
      OpenMS::MSPGenericFile msp_file;
      OpenMS::MSExperiment library;
      msp_file.load(filenames_I.getFullPath("msp").generic_string(), library);
      library.sortSpectra();
      rawDataHandler_IO.setLibrary(library);
      if (library_observable_) library_observable_->notifyLibraryUpdated();
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      throw;
    }
  }
}
