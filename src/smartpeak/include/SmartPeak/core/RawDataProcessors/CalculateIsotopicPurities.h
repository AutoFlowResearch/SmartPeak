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
  
  struct CalculateIsotopicPurities : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "CALCULATE_MDV_ISOTOPIC_PURITIES"; }
    virtual std::string getDescription() const override { return "Calculate MDV Isotopic Purities."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /** Calculate MDV Isotopic Purities
     */
    void process(
                 RawDataHandler& rawDataHandler_IO,
                 const ParameterSet& params_I,
                 Filenames& filenames_I
                 ) const override;
  };

}
