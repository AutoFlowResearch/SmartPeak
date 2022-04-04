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

#include <SmartPeak/core/RawDataProcessor.h>

#include <map>
#include <vector>
#include <regex>
#include <sstream>

namespace SmartPeak
{

  struct LoadRawData : RawDataProcessor
  {
    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_RAW_DATA"; }
    virtual std::string getDescription() const override { return "Read in raw data mzML file from disk."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::set<std::string> getOutputs() const override;

    /** Read in raw data mzML file from disk.

      Depending upon user specifications, the mzML file will be mapped to the TraML file
      and/or various pre-processing methods will be run to extract out the data.
    */
    void doProcess(
      RawDataHandler& rawDataHandler_IO,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /** Extracts metadata from the chromatogram.
    
      NOTE:
      - Not used in the current version of SmartPeak
      - Would need to be refactored or carefully implemented so as not to conflict
        with current injection, sample, and file naming patterns
    */
    static void extractMetaData(RawDataHandler& rawDataHandler_IO);

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

}
