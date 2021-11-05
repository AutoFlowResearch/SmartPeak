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
#pragma once

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/FilePicker.h>

#include <string>
#include <vector>

namespace SmartPeak
{
  struct ISetInputOutputWidgetObserver
  {
    virtual void onInputOutputSet() = 0;
    virtual void onInputOutputCancel() = 0;
  };

  class SetInputOutputWidget : public Widget
  {
  public:
    SetInputOutputWidget(ApplicationHandler& application_handler);

    void draw() override;

    void open(Filenames& filenames, ISetInputOutputWidgetObserver* observer);

    void setEditField(int id, std::string label, std::string& edit_box, std::shared_ptr<IFilePickerHandler>& file_picker_handler);

  protected:
    Filenames* filenames_ = nullptr;
    ApplicationHandler& application_handler_;
    std::optional<std::tuple<std::string, std::shared_ptr<IFilePickerHandler>>> popup_file_picker_;
    FilePicker file_picker_;
    std::string mzML_dir_edit_;
    std::string features_in_dir_edit_;
    std::string features_out_dir_edit_;
    std::string mzML_dir_old_;
    std::string features_in_dir_old_;
    std::string features_out_dir_old_;
    std::shared_ptr<IFilePickerHandler> set_raw_data_path_name;
    std::shared_ptr<IFilePickerHandler> set_input_features_path_name;
    std::shared_ptr<IFilePickerHandler> set_output_features_path_name;
    bool one_missing_directory_ = false;
    ISetInputOutputWidgetObserver* observer_ = nullptr;
  };
}
