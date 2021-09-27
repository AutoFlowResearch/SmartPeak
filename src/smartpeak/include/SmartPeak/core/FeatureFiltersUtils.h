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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/Filenames.h>
#include <OpenMS/FORMAT/MRMFeatureQCFile.h>

#include <string>

namespace SmartPeak
{

  /**
  * Class used by all the processor loading and storing Features Filters.
  */
  class FeatureFiltersUtils
  {
  public:
    FeatureFiltersUtils()                                      = delete;
    ~FeatureFiltersUtils()                                     = delete;
    FeatureFiltersUtils(const FeatureFiltersUtils&)            = delete;
    FeatureFiltersUtils& operator=(const FeatureFiltersUtils&) = delete;
    FeatureFiltersUtils(FeatureFiltersUtils&&)                 = delete;
    FeatureFiltersUtils& operator=(FeatureFiltersUtils&&)      = delete;

    static bool onFilePicked(
      const std::filesystem::path& filename,
      ApplicationHandler* application_handler,
      Filenames& filenames,
      const std::string& file_id,
      const std::string& file_group_id,
      int feature_filter_mode);
      
    static void storeFeatureFilters(
      const std::string& file_id,
      const std::string& file_group_id,
      Filenames& filenames,
      const OpenMS::MRMFeatureQC& features_qc,
      int feature_filter_mode);

    static void loadFeatureFilters(
      const std::string& file_id, 
      const std::string& file_group_id,
      Filenames& filenames,
      OpenMS::MRMFeatureQC& features_qc,
      std::function<void()> notification,
      std::function<void()> notification_group,
      int feature_filter_mode);

  private:
    static void storeMetadataInDB(
      Filenames& filenames,
      const std::string& file_id,
      const std::vector<int>& inserted_rows,
      const std::vector<const std::map<OpenMS::String, std::pair<double, double>>*> metadata_values);

    static void loadMetadataFromDB(
      Filenames& filenames,
      const std::string& file_id,
      const std::vector<int>& features_id,
      std::vector<std::map<OpenMS::String, std::pair<double, double>>*> metadata_values);
  };
}
