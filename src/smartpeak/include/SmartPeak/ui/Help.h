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
    {"Injections", " Use this table to view the list of injections\n and plot selected samples to the Chromatogram window"},
    {"Transitions", " Use this table to view the list of transitions\n and plot selected transitions on the Features (line) or Features (heatmap) window(s) or display in the Features (table) or Features (matrix) window(s)"},
    {"Features", " Use this table to view and select from list of features\n and plot selected samples to the Features (line) window"},
    
    // main windows (right)
    {"WorkflowStepWindow", " Use this window to add, edit and remove workflow steps before running the workflow"},
    {"Calibrators", "Use this plot to view calibrators as a line plot where the line represents the calibration fit and the points represent calibration points"},
    {"Features (heatmap)", "Use this window to plot features and subfeatures as a 2D heatmap"},
    {"Features (line)", "Use this window to plot features and subfeatures as a 2D lines"},
    {"Parameters", " Use this window to add, edit and remove parameters assocciated with the currently selected workflow"},
    
    {"Sequence", " Use this window to view the sequences"},
    {"Transitions", " Use this window to view the table of transitions"},
    {"Spectrum", " Use this window to view the spectrum"},
    {"Statistics", " Use this window to view statistics about feature samples and feature transitions"},
    {"Quantitation Method", " Use this window to view quantitation methods"},
    {"Standards Concentrations", " Use this window to view standard concentrations"},
    {"Component Filters", " Use this window to view component filters"},
    {"Component Group Filters", " Use this window to view component group filters"},
    {"Component QCs", " Use this window to view component QC"},
    {"Component Group QCs", " Use this window to view group component QC"},
    {"Features table", " Use this window to view list of features"},
    {"Component RSD Filters", " Use this window to view component RSD filters"},
    {"Component Group RSD Filters", " Use this window to view component group RSD filters"},
    {"Component RSD QCs", " Use this window to view component RSD QC"},
    {"Component Group RSD QCs", " Use this window to view component group RSD QC"},
    {"Component Background Filters", " Use this window to view component background filters"},
    {"Component Group Background Filters", " Use this window to view component group background filters"},
    {"Component Background QCs", " Use this window to view component background QCs"},
    {"Component Group Background QCs", " Use this window to view component group background QCs"},
    {"Component RSD Estimations", " Use this window to view component RSD Estimations"},
    {"Component Group RSD Estimations", " Use this window to view component group RSD Estimations"},
    {"Component Background Estimations", " Use this window to view component background estimations"},
    {"Component Group Background Estimations", " Use this window to view component group background estimations"},
    {"Features matrix", " Use this window to view feature matrix"},
    
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
