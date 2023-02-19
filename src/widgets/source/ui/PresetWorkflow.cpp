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

#include <SmartPeak/PresetWorkflows/PresetWorkflow.h>

#include <SmartPeak/PresetWorkflows/FIAMS_Unknowns.h>
#include <SmartPeak/PresetWorkflows/GCMS_Full_Scan_Unknowns.h>
#include <SmartPeak/PresetWorkflows/GCMS_SIM_Unknowns.h>
#include <SmartPeak/PresetWorkflows/HPLC_UV_Standards.h>
#include <SmartPeak/PresetWorkflows/HPLC_UV_Unknowns.h>
#include <SmartPeak/PresetWorkflows/LCMS_DDA_Spectra_Library_Construction.h>
#include <SmartPeak/PresetWorkflows/LCMS_DDA_Spectra_Library_Matching.h>
#include <SmartPeak/PresetWorkflows/LCMS_DDA_Transitions_Library_Construction.h>
#include <SmartPeak/PresetWorkflows/LCMS_MRM_Standards.h>
#include <SmartPeak/PresetWorkflows/LCMS_MRM_Unknowns.h>

namespace SmartPeak
{
  const std::vector<std::shared_ptr<PresetWorkflow>> PresetWorkflow::all_presets_ =
  {
    std::make_shared<FIAMS_Unknowns>(),
    std::make_shared<GCMS_Full_Scan_Unknowns>(),
    std::make_shared<GCMS_SIM_Unknowns>(),
    std::make_shared<HPLC_UV_Standards>(),
    std::make_shared<HPLC_UV_Unknowns>(),
    std::make_shared<LCMS_DDA_Spectra_Library_Construction>(),
    std::make_shared<LCMS_DDA_Spectra_Library_Matching>(),
    std::make_shared<LCMS_DDA_Transitions_Library_Construction>(),
    std::make_shared<LCMS_MRM_Standards>(),
    std::make_shared<LCMS_MRM_Unknowns>()
  };
}