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

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/iface/IFilenamesHandler.h>
#include <string>

namespace SmartPeak
{
  class InputDataValidation
  {
public:
    InputDataValidation()                                      = delete;
    ~InputDataValidation()                                     = delete;
    InputDataValidation(const InputDataValidation&)            = delete;
    InputDataValidation& operator=(const InputDataValidation&) = delete;
    InputDataValidation(InputDataValidation&&)                 = delete;
    InputDataValidation& operator=(InputDataValidation&&)      = delete;

    static bool fileExists(const std::filesystem::path& filepath);

    static bool precheckProcessorInputs(
      const IFilenamesHandler& input_files,
      const std::string& processor_name,
      const Filenames& filenames_I,
      bool required);

    static bool prepareToLoad(
      const Filenames& filenames,
      const std::string& id,
      bool check_bom
    );

    static bool prepareToLoadOneOfTwo(
      const Filenames& filenames,
      const std::string& id1,
      const std::string& id2,
      bool check_bom
    );

    static bool prepareToStore(
      const Filenames& filenames,
      const std::string& id
    );

    static bool prepareToStoreOneOfTwo(
      const Filenames& filenames,
      const std::string& id1,
      const std::string& id2
    );

    static std::string getSequenceInfo(
      const SequenceHandler& sequenceHandler
    );

    static std::string getParametersInfo(
      const std::map<std::string,std::vector<std::map<std::string,std::string>>>& parameters
    );

    static std::string getTraMLInfo(
      const RawDataHandler& rawDataHandler
    );

    static std::string getFeatureFiltersInfo(
      const RawDataHandler& rawDataHandler,
      const bool is_feature_filter // else is feature qc
    );

    static std::string getFeatureRSDFiltersInfo(
      const RawDataHandler& rawDataHandler,
      const bool is_feature_filter // else is feature qc
    );

    static std::string getFeatureBackgroundFiltersInfo(
      const RawDataHandler& rawDataHandler,
      const bool is_feature_filter // else is feature qc
    );

    static std::string getQuantitationMethodsInfo(
      const SequenceSegmentHandler& sequenceSegmentHandler
    );

    static std::string getStandardsConcentrationsInfo(
      const SequenceSegmentHandler& sequenceSegmentHandler
    );

    static bool sampleNamesAreConsistent(
      const SequenceHandler& sequenceHandler
    );

    static bool componentNamesAreConsistent(
      const SequenceHandler& sequenceHandler
    );

    static bool componentNameGroupsAreConsistent(
      const SequenceHandler& sequenceHandler
    );

    static bool heavyComponentsAreConsistent(
      const SequenceHandler& sequenceHandler
    );

    static bool validateNamesInStructures(
      const std::set<std::string>& names1,
      const std::set<std::string>& names2,
      const std::string& structure_ref1,
      const std::string& structure_ref2,
      const bool check_both_directions = true
    );

    static std::set<std::string> findMissingNames(
      const std::set<std::string>& names,
      const std::set<std::string>& bucket
    );

    static std::string logMissingNames(
      const std::set<std::string>& missing_names,
      const std::string& structure_ref1,
      const std::string& structure_ref2
    );
  };
}
