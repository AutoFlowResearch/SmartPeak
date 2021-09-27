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

#include <gtest/gtest.h>
#include <SmartPeak/ui/Plotter.h>
#include <SmartPeak/core/SessionHandler.h>

#include <filesystem>
#include <cmath>

TEST(PlotExporter, plot)
{
  auto main_path = std::filesystem::temp_directory_path();
  std::filesystem::current_path(main_path);
  std::filesystem::create_directory("plots");
  std::string gnuplot_path = "gnuplot";
  std::string seperator = "/";
#ifdef _WIN32
  gnuplot_path = "C:\\ProgramData\\chocolatey\\bin\\gnuplot.exe";
  seperator = "\\";
#endif
  main_path.append("plots" + seperator);
  SmartPeak::SessionHandler::GraphVizData graph_viz_data_;
  
  std::vector<float> x;
  std::vector<float> y;
  float rads = 3.14159265/180;
  for(int i = 0; i < 3000; i++) {
    x.push_back((3076 * sin(1.4 * i * rads) + 34767));
    y.push_back(i);
  }
  
  graph_viz_data_.max_nb_points_ = 3010;
  graph_viz_data_.addData(x, y, "phase_shifted_sin");
  
  bool is_successful = false;

  //PNG=0, PDF=1, HTML=2, SVG=3
  auto exported_png = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), graph_viz_data_, 0);
  exported_png->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(exported_png->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.png"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.png") > 5000 );
  
  auto exported_pdf = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), graph_viz_data_, 1);
  exported_pdf->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(exported_pdf->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.pdf"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.pdf") > 10000 );
  
  auto exported_html = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), graph_viz_data_, 2);
  exported_html->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(exported_html->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.html"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.html") > 5000 );
  
  auto exported_svg = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), graph_viz_data_, 3);
  exported_svg->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(exported_svg->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.svg"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.svg") > 25000 );
  
  std::filesystem::remove_all(main_path);
}
