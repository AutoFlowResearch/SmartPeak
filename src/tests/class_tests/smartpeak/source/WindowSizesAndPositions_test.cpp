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

TEST(WindowSizesAndPositions, defaults)
{
  WindowSizesAndPositions win_size_and_pos;
  EXPECT_NEAR(win_size_and_pos.main_menu_bar_y_size_, 18.0f, 1e-3);
  EXPECT_NEAR(win_size_and_pos.y_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.x_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_and_top_window_x_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_and_right_window_y_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_window_x_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_and_top_window_x_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_and_right_window_y_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_window_x_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_perc_, 0.25, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_window_x_perc_, 0.25, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_perc_, 0, 1e-3);
}

TEST(WindowSizesAndPositions, setXAndYSizes)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(10, 10);
  EXPECT_NEAR(win_size_and_pos.y_size_, -8, 1e-3);
  EXPECT_NEAR(win_size_and_pos.x_size_, 10, 1e-3);
}

TEST(WindowSizesAndPositions, setLeftWindowXSize)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(100, 100);
  win_size_and_pos.setLeftWindowXSize(10);
  win_size_and_pos.setWindowsVisible(true, true, true, false);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_size_, 20.5 , 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_size_, 61.5, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_and_top_window_x_size_, 90, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_and_right_window_y_size_, 82, 1e-3); //
  EXPECT_NEAR(win_size_and_pos.left_window_x_size_, 10, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_pos_, 79.5, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_pos_, 18, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_and_top_window_x_pos_, 10, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_and_right_window_y_pos_, 18, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_window_x_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_pos_, 100, 1e-3);
}

TEST(WindowSizesAndPositions, setTopWindowYSize)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(100, 100);
  win_size_and_pos.setTopWindowYSize(10);
  win_size_and_pos.setWindowsVisible(true, true, true, false);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_size_, 72, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_size_, 10, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_and_top_window_x_size_, 75, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_and_right_window_y_size_, 82, 1e-3); //
  EXPECT_NEAR(win_size_and_pos.left_window_x_size_, 25, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_pos_, 28, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_pos_, 18, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_and_top_window_x_pos_, 25, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_and_right_window_y_pos_, 18, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_window_x_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_pos_, 100, 1e-3);
}

TEST(WindowSizesAndPositions, showHide)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(100, 100);
  win_size_and_pos.setTopWindowYSize(10);
  win_size_and_pos.setWindowsVisible(true, true, true, false);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_size_, 72, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_size_, 10, 1e-3);
  win_size_and_pos.setWindowsVisible(true, false, true, false);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_size_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_size_, 82, 1e-3);
  win_size_and_pos.setWindowsVisible(true, true, true, false);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_size_, 20.5, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_size_, 61.5, 1e-3);
}

TEST(WindowSizesAndPositions, setWindowPercentages)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setWindowPercentages(0.1, 0.2, 0.3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_perc_, 0.1, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_window_x_perc_, 0.2, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_perc_, 0.3, 1e-3);
}

TEST(WindowSizesAndPositions, setWindowSizesAndPositions_)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(100, 100);
  win_size_and_pos.setWindowSizesAndPositions_(0.2, 0.2, 0.2);
  EXPECT_NEAR(win_size_and_pos.main_menu_bar_y_size_, 18.0f, 1e-3);
  EXPECT_NEAR(win_size_and_pos.y_size_, 82, 1e-3);
  EXPECT_NEAR(win_size_and_pos.x_size_, 100, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_size_, 16.4, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_and_top_window_x_size_, 60, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_size_, 65.6, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_and_right_window_y_size_, 82, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_window_x_size_, 20, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_size_, 20, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_pos_, 83.6, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_and_top_window_x_pos_, 20, 1e-3);
  EXPECT_NEAR(win_size_and_pos.top_window_y_pos_, 18, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_and_right_window_y_pos_, 18, 1e-3);
  EXPECT_NEAR(win_size_and_pos.left_window_x_pos_, 0, 1e-3);
  EXPECT_NEAR(win_size_and_pos.right_window_x_pos_, 80, 1e-3);
  EXPECT_NEAR(win_size_and_pos.bottom_window_y_perc_, 0.25, 1e-3); // Defaults
  EXPECT_NEAR(win_size_and_pos.left_window_x_perc_, 0.25, 1e-3); // Defaults
  EXPECT_NEAR(win_size_and_pos.right_window_x_perc_, 0, 1e-3); // Defaults
}

TEST(WindowSizesAndPositions, widget_WriteAndReadWindowSizesAndPositions)
{
  WindowSizesAndPositions win_size_and_pos_write;
  win_size_and_pos_write.setWindowPercentages(0.1, 0.3, 0.6);

  SessionDB session_db;
  auto path_db = std::tmpnam(nullptr);
  session_db.setDBFilePath(path_db);

  session_db.writePropertiesHandler(win_size_and_pos_write);

  WindowSizesAndPositions win_size_and_pos_read;
  session_db.readPropertiesHandler(win_size_and_pos_read);
  EXPECT_FLOAT_EQ(win_size_and_pos_read.bottom_window_y_perc_, 0.1);
  EXPECT_FLOAT_EQ(win_size_and_pos_read.left_window_x_perc_, 0.3);
  EXPECT_FLOAT_EQ(win_size_and_pos_read.right_window_x_perc_, 0.6);
}