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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE WindowSizesAndPositions test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/ui/WindowSizesAndPositions.h>
#include <thread>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(guiappender)

BOOST_AUTO_TEST_CASE(constructor)
{
  WindowSizesAndPositions* p1 = nullptr;
  WindowSizesAndPositions* nullPointer = nullptr;
  p1 = new WindowSizesAndPositions();
  BOOST_CHECK_NE(p1, nullPointer);
  delete p1;
}

BOOST_AUTO_TEST_CASE(defaults)
{
  WindowSizesAndPositions win_size_and_pos;
  BOOST_CHECK_CLOSE(win_size_and_pos.main_menu_bar_y_size_, 18.0f, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.y_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.x_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_and_top_window_x_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.top_window_y_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_and_right_window_y_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_and_top_window_x_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.top_window_y_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_and_right_window_y_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_perc_, 0.25, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_perc_, 0.25, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_perc_, 0, 1e-3);
}

BOOST_AUTO_TEST_CASE(setXAndYSizes)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(10, 10);
  BOOST_CHECK_CLOSE(win_size_and_pos.y_size_, -8, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.x_size_, 10, 1e-3);
}

BOOST_AUTO_TEST_CASE(setLeftWindowXSize)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(100, 100);
  win_size_and_pos.setLeftWindowXSize(10);
  win_size_and_pos.setWindowSizesAndPositions(true, true, true, false);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_size_, 20.5 , 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.top_window_y_size_, 61.5, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_and_top_window_x_size_, 90, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_and_right_window_y_size_, 82, 1e-3); //
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_size_, 10, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_pos_, 79.5, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.top_window_y_pos_, 18, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_and_top_window_x_pos_, 10, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_and_right_window_y_pos_, 18, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_pos_, 100, 1e-3);
}

BOOST_AUTO_TEST_CASE(setTopWindowYSize)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(100, 100);
  win_size_and_pos.setTopWindowYSize(10);
  win_size_and_pos.setWindowSizesAndPositions(true, true, true, false);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_size_, 72, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.top_window_y_size_, 10, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_and_top_window_x_size_, 75, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_and_right_window_y_size_, 82, 1e-3); //
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_size_, 25, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_size_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_pos_, 28, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.top_window_y_pos_, 18, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_and_top_window_x_pos_, 25, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_and_right_window_y_pos_, 18, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_pos_, 100, 1e-3);
}

BOOST_AUTO_TEST_CASE(setWindowPercentages)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setWindowPercentages(0.1, 0.2, 0.3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_perc_, 0.1, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_perc_, 0.2, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_perc_, 0.3, 1e-3);
}

BOOST_AUTO_TEST_CASE(setWindowSizesAndPositions_)
{
  WindowSizesAndPositions win_size_and_pos;
  win_size_and_pos.setXAndYSizes(100, 100);
  win_size_and_pos.setWindowSizesAndPositions_(0.2, 0.2, 0.2);
  BOOST_CHECK_CLOSE(win_size_and_pos.main_menu_bar_y_size_, 18.0f, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.y_size_, 82, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.x_size_, 100, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_size_, 16.4, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_and_top_window_x_size_, 60, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.top_window_y_size_, 65.6, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_and_right_window_y_size_, 82, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_size_, 20, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_size_, 20, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_pos_, 83.6, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_and_top_window_x_pos_, 20, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.top_window_y_pos_, 18, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_and_right_window_y_pos_, 18, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_pos_, 0, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_pos_, 80, 1e-3);
  BOOST_CHECK_CLOSE(win_size_and_pos.bottom_window_y_perc_, 0.25, 1e-3); // Defaults
  BOOST_CHECK_CLOSE(win_size_and_pos.left_window_x_perc_, 0.25, 1e-3); // Defaults
  BOOST_CHECK_CLOSE(win_size_and_pos.right_window_x_perc_, 0, 1e-3); // Defaults
}

BOOST_AUTO_TEST_SUITE_END()
