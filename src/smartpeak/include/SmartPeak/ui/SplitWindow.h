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

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/LayoutLoader.h>
#include <SmartPeak/ui/WindowSizesAndPositions.h>

#include <string>
#include <vector>

namespace SmartPeak
{
  class SplitWindow final : public Widget
  {
  public:
    SplitWindow() :
      Widget("SplitWindow")
    {
    };

    void draw() override;

    std::vector<std::shared_ptr<Widget>> top_windows;
    std::vector<std::shared_ptr<Widget>> bottom_windows;
    std::vector<std::shared_ptr<Widget>> left_windows;

    void setupLayoutLoader(LayoutLoader& layout_loader);

    WindowSizesAndPositions win_size_and_pos;
  };
}
