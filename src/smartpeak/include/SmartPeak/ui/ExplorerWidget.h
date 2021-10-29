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
// $Maintainer: Douglas McCloskey, Ahmed Khalil, Bertrand Boudaud $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once


#include <imgui.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/ImEntry.h>
#include <SmartPeak/ui/Help.h>
#include <unsupported/Eigen/CXX11/Tensor>
#include <SmartPeak/iface/ISequenceSegmentObserver.h>
#include <SmartPeak/iface/IFeaturesObserver.h>
#include <SmartPeak/iface/IPropertiesHandler.h>
#include <SmartPeak/core/EventDispatcher.h>

#include <string>
#include <utility>
#include <vector>
#include <functional>

/**
Generic and base classes for Widgets
*/

namespace SmartPeak
{
  extern bool enable_quick_help;

  /**
    @brief Base class for all tables

    TODO: features
    - row highlighting on focus
    - searching
    - color coding of rows by status
  */
  class ExplorerWidget :
      public GenericTableWidget,
      public ISequenceObserver,
      public IFeaturesObserver
    {
    public:
      ExplorerWidget(const std::string& table_id, const std::string title ="", SequenceObservable* sequence_observable = nullptr)
        :GenericTableWidget(table_id, title)
      {
        if (sequence_observable)
        {
          sequence_observable->addSequenceObserver(this);
        }
      };
    /*
    @brief Show the explorer

    @param[in] headers Column header names
    @param[in,out] columns Table body or matrix
    @param[in,out] checked_rows What rows are checked/filtered
    */
    void draw() override;

    /**
    ISequenceObserver
    */
    virtual void onSequenceUpdated() override;
    
    /**
    IFeaturesObserver
    */
    virtual void onFeaturesUpdated() override;

    /**
    IPropertiesHandler
    */
    virtual std::map<std::string, CastValue::Type> getPropertiesSchema() const override;
    virtual std::optional<CastValue> getProperty(const std::string& field, const size_t row) const override;
    virtual void setProperty(const std::string& field, const CastValue& value, const size_t row) override;

    Eigen::Tensor<std::string, 1> checkbox_headers_;
    Eigen::Tensor<bool, 2> *checkbox_columns_ = nullptr;

    std::map<int, std::vector<bool>> serialized_checkboxes_;
  };

}
