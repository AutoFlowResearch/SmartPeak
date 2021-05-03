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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

#pragma once
#include <string>
#include <unordered_map>

namespace SmartPeak
{
  /**
    @brief Documentation/Help for SmartPeakGUI
    //TODO:auto-gen this file
  */
  static std::unordered_map<std::string, std::string> tooltip_info
  {
    // explorer windows (left)
    {"InjectionsExplorerWindow", " Use this table to view the list of injections\n and plot selected samples to the Chromatogram window"},
    {"TransitionsExplorerWindow", " Use this table to view the list of samples\n and plot selected samples to the Features (line) window"},
    {"FeaturesExplorerWindow", " Use this table to view and select from list of features\n and plot selected samples to the Features (line) window"},
    
    // main windows (right)
    {"WorkflowStepWindow", " Use this window to add, edit and remove workflow steps before running the workflow"},
    {"CalibratorsPlotWidget", "Use this plot view calibrators in 2D"},
    {"Heatmap2DWidget", "Use this window to plot features and subfeatures as a 2D heatmap "},
    {"ParametersTableWidget", " Use this window to add, edit and remove parameters assocciated with the currently selected workflow"},
    
    {"SequenceMainWindow", " Use this window to view the sequences"},
    {"TransitionsMainWindow", " Use this window to view transitions"},
    {"SpectrumMainWindow", " Use this window to view the spectrum"},
    {"QuantMethodMainWindow", " Use this window to view quantitation methods"},
    {"StdsConcsMainWindow", " Use this window to view standard concentrations"},
    {"CompFiltersMainWindow", " Use this window to view component filters"},
    {"CompGroupFiltersMainWindow", " Use this window to view component group filters"},
    {"CompQCsMainWindow", " Use this window to view component QC"},
    {"CompGroupQCsMainWindow", " Use this window to view group component QC"},
    {"featuresTableMainWindow", " Use this window to view list of features"},
    {"CompRSDFiltersMainWindow", " Use this window to view component RSD filters"},
    {"CompGroupRSDFiltersMainWindow", " Use this window to view component group RSD filters"},
    {"CompRSDQCsMainWindow", " Use this window to view component RSD QC"},
    {"CompGroupRSDQCsMainWindow", " Use this window to view component group RSD QC"},
    {"CompBackgroundFiltersMainWindow", " Use this window to view component component background filters"},
    {"CompGroupBackgroundFiltersMainWindow", " Use this window to view component group background filters"},
    {"CompBackgroundQCsMainWindow", " Use this window to view component background QCs"},
    {"CompGroupBackgroundQCsMainWindow", " Use this window to view component group background QCs"},
    {"CompRSDEstimationsMainWindow", " Use this window to view component RSD Estimations"},
    {"CompGroupRSDEstimationsMainWindow", " Use this window to view component group RSD Estimations"},
    {"CompBackgroundEstimationsMainWindow", " Use this window to view component background estimations"},
    {"CompGroupBackgroundEstimationsMainWindow", " Use this window to view component group background estimations"},
    {"featureMatrixMainWindow", " Use this window to view feature matrix"},
    
    // info/log (bottom)
    {"Info", " Use this window to get an overview of the currently loaded sequence"},
    {"Log", " Use this window to view a detailed log and spot major warnings & errors"},
    
    // menu elements
    {"file", " Load, import and export files"},
    {"load_session_from_sequence", " Load a session by opening a sequence file (sequence.csv)"},
    {"import_file", " Add a file to the current session by choosing the file type"},
    {"export_file", " Save a workflow or a sequence from the current session"},
    
    {"edit", "-"}, //TODO:update once implemented
    
    {"view", " View and hide tables and plot"},
    
    {"actions", " Perform actions on the loaded sequence"},
    {"run_workflow", " Run the workflow on the loaded sequence file"},
    {"integrity_checks", " Perform integrity checks prior to running the workflow"},
    {"report", " Generate and export detailed reports"},
    
    {"help", " Get help on how to use SmartPeak"}
    
  };
}
