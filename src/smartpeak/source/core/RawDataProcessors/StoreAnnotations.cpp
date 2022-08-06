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
#include <SmartPeak/core/RawDataProcessors/StoreAnnotations.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FORMAT/MzTabFile.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{
  std::set<std::string> StoreAnnotations::getInputs() const
  {
    return { "Mz Tab" };
  }

  std::set<std::string> StoreAnnotations::getOutputs() const
  {
    return { };
  }

  std::vector<std::string> StoreAnnotations::getFilenameRequirements() const
  {
    return { "sequence", "traML" };
  }

  void StoreAnnotations::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("mzTab_o", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}.mzTab");
  };

  void StoreAnnotations::doProcess(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToStore(filenames_I, "mzTab_o"))
    {
      throw std::invalid_argument("Failed to store output file");
    }

    // Store outfile as mzTab
    OpenMS::MzTabFile mztabfile;
    mztabfile.store(filenames_I.getFullPath("mzTab_o").generic_string(), rawDataHandler_IO.getMzTab());
  }

}
