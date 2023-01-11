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

#include <SmartPeak/ui/Widget.h>

#include <string>
#include <vector>

namespace SmartPeak
{
  /**
  * @brief list all windows ptr to be used in the Presets layout
  */
  struct AllWindows final
  {
    std::shared_ptr<Widget> quickInfoText_;
    std::shared_ptr<Widget> workflow_;
    std::shared_ptr<Widget> statistics_;
    std::shared_ptr<Widget> log_widget_;
    std::shared_ptr<Widget> parameters_table_widget_;
    std::shared_ptr<Widget> chromatogram_plot_widget_;
    std::shared_ptr<Widget> chromatogram_ms2_xic_plot_widget_;
    std::shared_ptr<Widget> spectra_ms2_plot_widget_;
    std::shared_ptr<Widget> chromatogram_ms1_xic_plot_widget_;
    std::shared_ptr<Widget> spectra_msms_plot_widget_;
    std::shared_ptr<Widget> chromatogram_tic_plot_widget_;
    std::shared_ptr<Widget> heatmap_plot_widget_;
    std::shared_ptr<Widget> spectra_plot_widget_;
    std::shared_ptr<Widget> feature_line_plot_;
    std::shared_ptr<Widget> injections_explorer_window_;
    std::shared_ptr<Widget> calibrators_line_plot_;
    std::shared_ptr<Widget> transitions_explorer_window_;
    std::shared_ptr<Widget> features_explorer_window_;
    std::shared_ptr<Widget> sequence_main_window_;
    std::shared_ptr<Widget> transitions_main_window_;
    std::shared_ptr<Widget> spectrum_main_window_;
    std::shared_ptr<Widget> quant_method_main_window_;
    std::shared_ptr<Widget> stds_consc_main_window_;
    std::shared_ptr<Widget> comp_filters_main_window_;
    std::shared_ptr<Widget> comp_group_filters_main_window_;
    std::shared_ptr<Widget> comp_qc_main_window_;
    std::shared_ptr<Widget> comp_group_qc_main_window_;
    std::shared_ptr<Widget> comp_rsd_filters_main_window_;
    std::shared_ptr<Widget> comp_group_rds_filters_main_window_;
    std::shared_ptr<Widget> comp_rsdcqcs_main_window_;
    std::shared_ptr<Widget> comp_group_rsdqcs_main_window_;
    std::shared_ptr<Widget> comp_background_filters_main_window_;
    std::shared_ptr<Widget> comp_group_background_filters_main_window_;
    std::shared_ptr<Widget> comp_background_qcs_main_window_;
    std::shared_ptr<Widget> comp_group_background_qcs_main_window_;
    std::shared_ptr<Widget> comp_rsd_estimations_main_window_;
    std::shared_ptr<Widget> comp_group_rsd_estimation_main_window_;
    std::shared_ptr<Widget> comp_background_estimations_main_window_;
    std::shared_ptr<Widget> comp_group_background_estimations_main_window_;
    std::shared_ptr<Widget> features_table_main_window_;
    std::shared_ptr<Widget> feature_matrix_main_window_;
  };
}
