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
  std::string home_path;
  std::filesystem::path main_path;
  SmartPeak::SessionHandler::GraphVizData graph_viz_data_;
  
  SmartPeak::Utilities::getEnvVariable("HOME", &home_path);
  main_path = std::filesystem::path(home_path) / "SmartPeak/";
  
  if (!std::filesystem::exists(main_path)) {
    std::filesystem::create_directory(main_path);
  }
  
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
  SmartPeak::PlotExporter* exported_png = new SmartPeak::PlotExporter(main_path.string(), graph_viz_data_, 0);
  EXPECT_TRUE(exported_png->plot());
  delete exported_png;
  EXPECT_TRUE(std::filesystem::exists(main_path.string() + "plots/smartpeak-exported-plot.png"));
  EXPECT_TRUE(std::filesystem::file_size(main_path.string() + "plots/smartpeak-exported-plot.png") > 50000 );
  
  SmartPeak::PlotExporter* exported_pdf = new SmartPeak::PlotExporter(main_path.string(), graph_viz_data_, 1);
  EXPECT_TRUE(exported_pdf->plot());
  delete exported_pdf;
  EXPECT_TRUE(std::filesystem::exists(main_path.string() + "plots/smartpeak-exported-plot.pdf"));
  EXPECT_TRUE(std::filesystem::file_size(main_path.string() + "plots/smartpeak-exported-plot.pdf") > 100000 );
  
  SmartPeak::PlotExporter* exported_html = new SmartPeak::PlotExporter(main_path.string(), graph_viz_data_, 2);
  EXPECT_TRUE(exported_html->plot());
  delete exported_html;
  EXPECT_TRUE(std::filesystem::exists(main_path.string() + "plots/smartpeak-exported-plot.html"));
  EXPECT_TRUE(std::filesystem::file_size(main_path.string() + "plots/smartpeak-exported-plot.html") > 50000 );
  
  SmartPeak::PlotExporter* exported_svg = new SmartPeak::PlotExporter(main_path.string(), graph_viz_data_, 3);
  EXPECT_TRUE(exported_svg->plot());
  delete exported_svg;
  EXPECT_TRUE(std::filesystem::exists(main_path.string() + "plots/smartpeak-exported-plot.svg"));
  EXPECT_TRUE(std::filesystem::file_size(main_path.string() + "plots/smartpeak-exported-plot.svg") > 250000 );
  
  std::filesystem::remove_all(main_path / "plots");
  bool deb = true;
}
