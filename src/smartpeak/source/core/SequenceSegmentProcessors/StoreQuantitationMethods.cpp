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

#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/FORMAT/AbsoluteQuantitationMethodFile.h>

#include <plog/Log.h>

namespace SmartPeak
{

  std::vector<std::string> StoreQuantitationMethods::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet StoreQuantitationMethods::getParameterSchema() const
  {
    return ParameterSet();
  }

  void StoreQuantitationMethods::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("quantitationMethods", constructFilename("quantitationMethods.csv", static_filenames_), "Quantitation Methods");
  };

  void StoreQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START storeQuantitationMethods";
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToStore(filenames_I, "quantitationMethods"))
    {
      throw std::invalid_argument("Failed to store output file");
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile aqmf;
      aqmf.store(
        filenames_I.getFullPath("quantitationMethods").generic_string(),
        sequenceSegmentHandler_IO.getQuantitationMethods()
      );
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeQuantitationMethods";
  }

}
