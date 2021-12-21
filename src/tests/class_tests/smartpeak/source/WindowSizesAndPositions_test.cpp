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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/ui/WindowSizesAndPositions.h>
#include <SmartPeak/io/SessionDB.h>

#include <imgui.h>

#include <thread>

using namespace SmartPeak;
using namespace std;

TEST(WindowSizesAndPositions, constructor)
{
  WindowSizesAndPositions* p1 = nullptr;
  WindowSizesAndPositions* nullPointer = nullptr;
  p1 = new WindowSizesAndPositions();
  EXPECT_NE(p1, nullPointer);
  delete p1;
}

class WindowSizesAndPositions_Test : public WindowSizesAndPositions
{
public:
  // wrappers to protected methods
  std::string wrapper_get_imgui_ini_() const
  {
    return imgui_ini_;
  }

  bool wrapper_get_imgui_ini_updated_() const
  {
    return imgui_ini_updated_;
  }
};

TEST(WindowSizesAndPositions, widget_WriteAndReadWindowSizesAndPositions)
{
  ImGui::CreateContext();
  WindowSizesAndPositions_Test win_size_and_pos_write;
  EXPECT_EQ(win_size_and_pos_write.wrapper_get_imgui_ini_(), std::string(""));
  EXPECT_EQ(win_size_and_pos_write.wrapper_get_imgui_ini_updated_(), false);

  SessionDB session_db;
  auto path_db = std::tmpnam(nullptr);
  session_db.setDBFilePath(path_db);

  session_db.writePropertiesHandler(win_size_and_pos_write);
  EXPECT_EQ(win_size_and_pos_write.wrapper_get_imgui_ini_(), std::string(""));
  EXPECT_EQ(win_size_and_pos_write.wrapper_get_imgui_ini_updated_(), false);

  WindowSizesAndPositions_Test win_size_and_pos_read;
  session_db.readPropertiesHandler(win_size_and_pos_read);

  // Unfortunately, we would need to create a full graphic context to create windows 
  // in order to get a proper, populated ini file from imgui.
  // so we just check that it's empty and it didn"t crashed.
  EXPECT_EQ(win_size_and_pos_read.wrapper_get_imgui_ini_(), std::string(""));
  EXPECT_EQ(win_size_and_pos_read.wrapper_get_imgui_ini_updated_(), true);
}