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
// $Maintainer: Ahmed Khalil, Douglas McCloskey $
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
  SmartPeak::SessionHandler::HeatMapData heatmap_data_;
  
  std::vector<float> x;
  std::vector<float> y;
  float rads = 3.14159265/180;
  for(int i = 0; i < 3000; i++) {
    x.push_back((3076 * sin(1.4 * i * rads) + 34767));
    y.push_back(i);
  }
  
  graph_viz_data_.max_nb_points_ = 3010;
  graph_viz_data_.addData(x, y, "phase_shifted_sin");
  
  heatmap_data_.feat_heatmap_col_labels.resize(2);
  heatmap_data_.feat_heatmap_row_labels.resize(3);
  heatmap_data_.feat_heatmap_data.resize(2,3);
  heatmap_data_.feat_heatmap_col_labels(0) = "inj1";
  heatmap_data_.feat_heatmap_col_labels(1) = "inj2";
  heatmap_data_.feat_heatmap_row_labels(0) = "trans1";
  heatmap_data_.feat_heatmap_row_labels(1) = "trans2";
  heatmap_data_.feat_heatmap_row_labels(2) = "trans3";
  heatmap_data_.feat_heatmap_data(0,0) = 93856.1;
  heatmap_data_.feat_heatmap_data(0,1) = 44340.3;
  heatmap_data_.feat_heatmap_data(0,2) = 45617.3;
  heatmap_data_.feat_heatmap_data(1,0) = 40322.7;
  heatmap_data_.feat_heatmap_data(1,1) = 15318.3;
  heatmap_data_.feat_heatmap_data(1,2) = 5287.02;
  
  bool is_successful = false;

  //PNG=0, PDF=1, HTML=2, SVG=3
  auto curve_png = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), graph_viz_data_, 0,
                                                                SmartPeak::PlotExporter::PlotType::CURVE);
  curve_png->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(curve_png->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.png"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.png") > 5000 );

  auto heatmap_png = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), heatmap_data_, 0,
                                                                SmartPeak::PlotExporter::PlotType::HEATMAP);
  heatmap_png->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(heatmap_png->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.png"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.png") > 5000 );
  
  
  auto curve_pdf = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), graph_viz_data_, 1,
                                                                SmartPeak::PlotExporter::PlotType::CURVE);
  curve_pdf->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(curve_pdf->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.pdf"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.pdf") > 10000 );
  
  auto heatmap_pdf = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), heatmap_data_, 1,
                                                                SmartPeak::PlotExporter::PlotType::HEATMAP);
  heatmap_pdf->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(heatmap_pdf->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.pdf"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.pdf") > 10000 );
  
  
  auto curve_html = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), graph_viz_data_, 2,
                                                                 SmartPeak::PlotExporter::PlotType::CURVE);
  curve_html->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(curve_html->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.html"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.html") > 5000 );
  
  auto heatmap_html = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), heatmap_data_, 2,
                                                                 SmartPeak::PlotExporter::PlotType::HEATMAP);
  heatmap_html->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(heatmap_html->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.html"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.html") > 5000 );
  
  
  auto curve_svg = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), graph_viz_data_, 3,
                                                                SmartPeak::PlotExporter::PlotType::CURVE);
  curve_svg->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(curve_svg->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.svg"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.svg") > 25000 );
  
  auto heatmap_svg = std::make_unique<SmartPeak::PlotExporter>(main_path.string(), heatmap_data_, 3,
                                                                SmartPeak::PlotExporter::PlotType::HEATMAP);
  heatmap_svg->setGNUPLOTPath(gnuplot_path);
  EXPECT_TRUE(heatmap_svg->plot());
  EXPECT_TRUE(std::filesystem::exists(main_path / "smartpeak-exported-plot.svg"));
  EXPECT_TRUE(std::filesystem::file_size(main_path / "smartpeak-exported-plot.svg") > 2500 );
  
  std::filesystem::remove_all(main_path);
}
