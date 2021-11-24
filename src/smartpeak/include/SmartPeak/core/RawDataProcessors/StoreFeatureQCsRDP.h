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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni, Svetlana Kutuzova, Ahmed Khalil $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/iface/IProcessorDescription.h>
#include <SmartPeak/core/WorkflowObservable.h>
#include <SmartPeak/core/ParametersObservable.h>
#include <SmartPeak/core/TransitionsObservable.h>
#include <SmartPeak/iface/IFilePickerHandler.h>
#include <SmartPeak/iface/IFilenamesHandler.h>
#include <SmartPeak/core/FeatureFiltersUtilsMode.h>
#include <SmartPeak/core/RawDataProcessor.h>

#include <map>
#include <vector>
#include <regex>
#include <sstream>

namespace SmartPeak
{

  struct StoreFeatureQCsRDP : RawDataProcessor, IFilePickerHandler
  {
    StoreFeatureQCsRDP(int feature_filter_mode = FeatureFiltersUtilsMode::EFeatureFilterComponentAndGroup, bool static_filenames = false)
      : feature_filter_mode_(feature_filter_mode), static_filenames_(static_filenames) {}
    int feature_filter_mode_;
    bool static_filenames_;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "STORE_FEATURE_QCS"; }
    virtual std::string getDescription() const override { return "Store the component and component group transition QC specifications from file."; }
    virtual std::vector<std::string> getRequirements() const override;

    /** Store the component and component group transition QCs from file.
    */
    void process(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /**
    IFilePickerHandler
    */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

}
