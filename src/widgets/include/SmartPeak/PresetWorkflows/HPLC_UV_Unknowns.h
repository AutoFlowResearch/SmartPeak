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

#include <SmartPeak/PresetWorkflows/PresetWorkflow.h>

namespace SmartPeak
{
  struct HPLC_UV_Unknowns : public PresetWorkflow {

    virtual std::string getName() const override 
    { 
      return "HPLC UV Unknowns";
    };

    virtual std::vector<std::string> getWorkflowSteps() const override
    {
      return
      {
        "LOAD_RAW_DATA",
        "MAP_CHROMATOGRAMS",
        "EXTRACT_CHROMATOGRAM_WINDOWS",
        "ZERO_CHROMATOGRAM_BASELINE",
        "PICK_MRM_FEATURES",
        "QUANTIFY_FEATURES",
        "CHECK_FEATURES",
        "SELECT_FEATURES",
        "STORE_FEATURES"
      };
    };

    virtual std::string getDescription() const override
    {
      return
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Nunc pellentesque sagittis lorem non interdum. Donec pulvinar laoreet velit, in cursus sapien lacinia quis. "
        "Interdum et malesuada fames ac ante ipsum primis in faucibus. "
        "Cras tincidunt suscipit sem, id placerat elit lobortis a.";
    };

    virtual std::map<std::string, std::vector<std::shared_ptr<Widget>>> getLayout(const AllWindows& all_windows) const override
    {
      return
      {
        {
          "top",
          {
            all_windows.injections_explorer_window_,
            all_windows.statistics_,
            all_windows.sequence_main_window_,
            all_windows.transitions_main_window_,
            all_windows.spectrum_main_window_,
            all_windows.workflow_,
            all_windows.parameters_table_widget_,
            all_windows.quant_method_main_window_,
            all_windows.stds_consc_main_window_,
            all_windows.comp_filters_main_window_,
            all_windows.comp_group_filters_main_window_,
            all_windows.comp_qc_main_window_,
            all_windows.comp_group_qc_main_window_,
            all_windows.comp_rsd_filters_main_window_,
            all_windows.comp_group_rds_filters_main_window_,
            all_windows.comp_rsdcqcs_main_window_,
            all_windows.comp_group_rsdqcs_main_window_,
            all_windows.comp_background_filters_main_window_,
            all_windows.comp_group_background_filters_main_window_,
            all_windows.comp_background_qcs_main_window_,
            all_windows.comp_group_background_qcs_main_window_,
            all_windows.comp_rsd_estimations_main_window_,
            all_windows.comp_group_rsd_estimation_main_window_,
            all_windows.comp_background_estimations_main_window_,
            all_windows.comp_group_background_estimations_main_window_,
            all_windows.features_table_main_window_,
            all_windows.feature_matrix_main_window_,
            all_windows.chromatogram_plot_widget_,
            all_windows.chromatogram_tic_plot_widget_,
            all_windows.chromatogram_ms1_xic_plot_widget_,
            all_windows.chromatogram_ms2_xic_plot_widget_,
            all_windows.spectra_plot_widget_,
            all_windows.feature_line_plot_,
            all_windows.heatmap_plot_widget_,
            all_windows.calibrators_line_plot_
          },
        },
        {
          "bottom",
          {
          all_windows.quickInfoText_,
          all_windows.log_widget_,
          all_windows.spectra_msms_plot_widget_,
          all_windows.spectra_ms2_plot_widget_,
          },
        },
        {
          "left",
          {
            all_windows.transitions_explorer_window_,
            all_windows.features_explorer_window_
          },
        }
      };
    };
  };
}
