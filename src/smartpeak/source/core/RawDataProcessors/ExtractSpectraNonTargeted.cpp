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
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------
#include <SmartPeak/core/RawDataProcessors/ExtractSpectraNonTargeted.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>

#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>

#include <plog/Log.h>

#include <algorithm>
#include <exception>

namespace SmartPeak
{

  std::vector<std::string> ExtractSpectraNonTargeted::getFilenameRequirements() const
  {
    return { "sequence", "traML" };
  }

  std::set<std::string> ExtractSpectraNonTargeted::getInputs() const
  {
    return { "Experiment", "Features" };
  }

  std::set<std::string> ExtractSpectraNonTargeted::getOutputs() const
  {
    return { "Features", "Extracted Spectra"};
  }

  ParameterSet ExtractSpectraNonTargeted::getParameterSchema() const
  {
    OpenMS::TargetedSpectraExtractor oms_params;
    ParameterSet parameters({ oms_params });
    return parameters;
  }

  void ExtractSpectraNonTargeted::doProcess(
    RawDataHandler& rawDataHandler_IO,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);
    // Complete user parameters with schema
    ParameterSet params(params_I);
    params.merge(getParameterSchema());

    OpenMS::TargetedSpectraExtractor targeted_spectra_extractor;
    Utilities::setUserParameters(targeted_spectra_extractor, params);

    std::vector<OpenMS::MSSpectrum> annotated_spectra;
    OpenMS::FeatureMap selected_features;
    targeted_spectra_extractor.extractSpectra(rawDataHandler_IO.getExperiment(), rawDataHandler_IO.getFeatureMap(), annotated_spectra, selected_features);

    // Update the features
    rawDataHandler_IO.setFeatureMap("extracted_spectra", selected_features);
    // NOTE: useful only for DDA using MRM trigger scans and not for DDA untargeted applications
    
    // Create the featureMap from the annotated/picked/scored spectra
    OpenMS::FeatureMap featureMap;
    for (const OpenMS::MSSpectrum& spec : annotated_spectra) {
      for (auto it = spec.begin(); it != spec.end(); ++it)
      {
        // set the metadata
        OpenMS::Feature f;
        f.setUniqueId();
        f.setMZ(it->getMZ());
        f.setRT(spec.getRT());
        std::ostringstream mass_of_the_peak;
        mass_of_the_peak << f.getMZ();
        std::string native_id = std::string(spec.getNativeID()) + std::string("_") + mass_of_the_peak.str();
        f.setMetaValue("native_id", native_id);
        f.setMetaValue("PeptideRef",spec.getName());
        f.setMetaValue("scan_polarity", rawDataHandler_IO.getMetaData().scan_polarity);
        f.setMetaValue("ms_level", 2);

        //// Ignoring LeftWidth and RightWidth for now...
        //f.setMetaValue("leftWidth", );
        //f.setMetaValue("rightWidth", );

        // Calculate the peak area (See Pick2D Features for using OpenMS::PeakIntegrator::PeakArea)
        f.setIntensity(it->getIntensity());
        f.setMetaValue("peak_apex_int", it->getIntensity());
        f.setMetaValue("peak_apex_position", it->getMZ());

        // Calculate peak shape metrics that will be used for later QC (See Pick2D Features for using OpenMS::PeakIntegrator::PeakShapeMetrics)
        // Ignore for now...

        // Extract out the convex hull (See Pick2DFeatures for using OpenMS::PeakIntegrator::PeakArea)
        // Ignoring the extraction of the convex hull for now...

        featureMap.push_back(f);
      }
    }

    rawDataHandler_IO.setFeatureMap(featureMap);
    rawDataHandler_IO.updateFeatureMapHistory();
    
    // Update the spectrum with the picked/scored/annotated spectrum
    rawDataHandler_IO.getChromatogramMap().setSpectra(annotated_spectra);
  }
}
