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

#include <SmartPeak/core/SampleGroupHandler.h>

namespace SmartPeak
{
  void SampleGroupHandler::clear() {
    feature_map_.clear();
  }

  void SampleGroupHandler::setSampleGroupName(const std::string& sample_group_name)
  {
    sample_group_name_ = sample_group_name;
  }

  std::string& SampleGroupHandler::getSampleGroupName()
  {
    return sample_group_name_;
  }

  const std::string& SampleGroupHandler::getSampleGroupName() const
  {
    return sample_group_name_;
  }

  void SampleGroupHandler::setSampleIndices(const std::vector<size_t>& sample_indices)
  {
    sample_indices_ = sample_indices;
  }

  std::vector<size_t>& SampleGroupHandler::getSampleIndices()
  {
    return sample_indices_;
  }

  const std::vector<size_t>& SampleGroupHandler::getSampleIndices() const
  {
    return sample_indices_;
  }

  void SampleGroupHandler::setFeatureMap(const OpenMS::FeatureMap& feature_map)
  {
    feature_map_ = feature_map;
  }

  OpenMS::FeatureMap& SampleGroupHandler::getFeatureMap()
  {
    return feature_map_;
  }

  const OpenMS::FeatureMap& SampleGroupHandler::getFeatureMap() const
  {
    return feature_map_;
  }
}
