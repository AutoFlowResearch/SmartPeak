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

#include <SmartPeak/ui/Plotter.h>

namespace SmartPeak {

  PlotLineProperties::PlotLineProperties(uint index)
  {
    if (index==0) {
      line_index_ = ++(PlotLineProperties::line_count_);
    } else {
      line_index_ = index;
    }
    colour_ = PlotLineProperties::basic_colours_hex_[ (line_index_-1) % PlotLineProperties::basic_colours_hex_.size() ];
  }

  void PlotLineProperties::set(const PlotLineSetting property, const std::string &value)
  {
    switch (property) {
    case LineStyle:
      if (value.empty()) {
        throw std::invalid_argument("Line style must be provided.");
      }
      line_type_ = value;
      break;
    case LineWidth:
      try {
        line_width_ = stod(value);
      } catch (const std::invalid_argument &) {
        throw std::invalid_argument("Line width must be a number.");
      }
      line_width_ = (line_width_<0) ? 0 : line_width_;
      break;
    case Marker:
      point_type_ = value;
      break;
    case MarkerSize:
      try {
        point_size_ = stod(value);
      } catch (const std::invalid_argument &) {
        throw std::invalid_argument("Marker size must be a number.");
      }
      point_size_ = (point_size_<0) ? 0 : point_size_;
      break;
    case Color:
      if (value.empty()) {
        throw std::invalid_argument("Colour cannot be empty");
      }
      this->colour_ = value;
      break;
    default:
      throw std::invalid_argument("Unknown line property");
    }
  }

  void PlotLineProperties::set(const std::pair<PlotLineSetting, std::string> &input)
  {
    this->set(input.first, input.second);
  }

  std::string PlotLineProperties::plotStyler() const
  {
    std::string style;
    style = "set style line " + std::to_string(line_index_);

    try {
      std::stringstream line_type;
      line_type << std::setprecision(3) << linetype_cairo_term_.at(line_type_);
      style += " lt " + line_type.str();
    }
    catch (const std::out_of_range &) {
      throw std::invalid_argument("Line style must be : \"-\", \"--\", \":\", \"-.\", or \"none\"");
    }
    
    std::stringstream line_width;
    line_width << std::setprecision(3) << line_width_;
    style += " lw " + line_width.str();

    try {
      style += " pt " + std::to_string( (point_type_.empty()) ? line_index_ : pointtype_generic_term_.at(point_type_) );
    }
    catch (const std::out_of_range &) {
      throw std::invalid_argument("Marker must be one of \"o+*.xsd^v><ph\" or \"none\"");
    }

    std::stringstream point_size;
    point_size << std::setprecision(3) << point_size_;
    style += " ps " + point_size.str();
    style += " lc rgb '" + colour_ + "'";
    return style;
  }

  uint PlotLineProperties::line_count_ = 0;

  void PlotLineProperties::resetLineCount()
  {
    PlotLineProperties::line_count_ = 0;
  }

  bool PlotLineProperties::pointtypeNone() const
  {
    return (line_type_.compare("none") == 0) ? true : false;
  }

  uint PlotLineProperties::getGridLineType()
  {
    return PlotLineProperties::linetype_cairo_term_.at(":");
  }

  const std::vector<std::string> PlotLineProperties::basic_colours_hex_
  {
    // red // green // blue // magenta // cyan // brown
    // yellow // olive // gray // purple // light-purple
    "#EB0450", "#1E6900", "#304eec", "#e919f1", "#00b2b2", "#96572a",
    "#d8ac00", "#8c8c19", "#616161", "#7919d6", "#bc8cea"
  };

  const std::map<std::string, int> PlotLineProperties::linetype_cairo_term_ {
    {"-",    1},
    {"--",   2},
    {":",    3},
    {"-.",   4},
    {"none", 0}
  };

  const std::map<std::string, int> PlotLineProperties::pointtype_generic_term_ {
    {"none", 0},
    {"+", 1},
    {"x", 2},
    {"*", 3},
    {"s", 4},
    {"p", 5},
    {"o", 6},
    {".", 7},
    {"^", 8},
    {">", 9},
    {"v", 10},
    {"<", 11},
    {"d", 12},
    {"h", 13}
  };

  PlotExporter::PlotExporter(std::string output_path, SessionHandler::GraphVizData& graphvis_data, int format)
    : x_title_(graphvis_data.x_axis_title_),
      y_title_(graphvis_data.y_axis_title_),
      nr_plots_(0),
      with_grid_(true),
      graphvis_data_(graphvis_data),
      output_path_(output_path),
      gnuplot_path_("gnuplot")
  {
    //PNG=0, PDF=1, HTML=2, SVG=3
    if (format == 0) plot_PNG_ = true;
    if (format == 1) plot_PDF_ = true;
    if (format == 2) plot_HTML_ = true;
    if (format == 3) plot_SVG_ = true;
  }

  bool PlotExporter::plot()
  {
   if (isGNUPLOTPresent_()) {
     std::ofstream data_vals_file((output_path_ + this->filename + std::string(".dat")).c_str());
     nr_plots_ = 0;
     if (graphvis_data_.x_data_area_.size() != graphvis_data_.y_data_area_.size()) {
       throw std::length_error("Graph visualisation data are not equal in size!");
     }
     if (data_vals_file.is_open()) {
       for (size_t i = 0; i < graphvis_data_.x_data_area_.size(); ++i) {
         const std::vector<float> &x = graphvis_data_.x_data_area_.at(i);
         const std::vector<float> &y = graphvis_data_.y_data_area_.at(i);
         data_vals_file << "# PLOT " << (nr_plots_)++ << std::endl;
         for (uint i = 0; i < x.size(); ++i) {
           data_vals_file << x[i] << "," << y[i] << std::endl;
         }
         data_vals_file << std::endl << std::endl;
       }
       data_vals_file.close();
     } else {
       LOGE << "PlotExporter::plot : Cannot save plot data to disk. ";
     }
    
     if (nr_plots_==0) {
       LOGE << "PlotExporter::plot : nothing to plot ";
       return true;
     }

     PlotLineProperties::resetLineCount();
     plotlines_properties_.resize(nr_plots_);

     if (plot_PNG_) {
       generatePNG_();
     }
     if (plot_PDF_) {
       generatePDF_();
     }
     if (plot_HTML_) {
       generateHTML_();
     }
     if (plot_SVG_) {
       generateSVG_();
     }
     return true;
   } else {
     LOGE << "gnuplot not found! Not generating any plots.";
     return false;
   }
  }

  bool PlotExporter::isGNUPLOTPresent_()
  {
    bool is_gnuplot_present = false;
    std::string is_present = "";
    
#if defined(__APPLE__) || defined(__linux__)
    std::string check_gnuplot = "command -v " + gnuplot_path_ + " 2>&1 | tee " +  output_path_ + "is_gnuplot_present.tmp";
    system(check_gnuplot.c_str());

    std::ifstream tmp_file((output_path_ + "is_gnuplot_present.tmp").c_str());
    if (tmp_file.is_open()) {
      tmp_file >> is_present;
      if (!is_present.empty() && is_present.size() >= 7) is_gnuplot_present = true;
      tmp_file.close();
    } else {
      LOGE << "Cannot save temporary files to disk.";
    }

#elif _WIN32
    std::string check_gnuplot =
      "if ((Get-Command \"" + gnuplot_path_ + "\" -ErrorAction SilentlyContinue) -eq $null) { Write \"0\" | Out-File "
      +  output_path_ +  "is_gnuplot_present.tmp -Append } else { Write \"1\" | Out-File "
      +  output_path_ +  "is_gnuplot_present.tmp -Append }";
    system(check_gnuplot.c_str());

    std::ifstream tmp_file((output_path_ + "is_gnuplot_present.tmp").c_str());
    if (tmp_file.is_open()) {
      tmp_file >> is_present;
      if (is_present == "1") is_gnuplot_present = true;
      tmp_file.close();
    } else {
      LOGE << "Cannot save temporary files to disk.";
    }
#endif
    
    return is_gnuplot_present;
  }

  bool PlotExporter::isTermAvailable_(std::string term_name)
  {
    bool term_exists = false;
    std::string term_available = "";
    std::string check_term = gnuplot_path_ + " -e \"set print '" +  output_path_ +
                              this->filename + "-exists-" + term_name + ".tmp" +
                              "'; if (strstrt(GPVAL_TERMINALS, '" + term_name + "')) print 1; else print 0\"";
    system(check_term.c_str());

    std::ifstream tmp_file((output_path_ + this->filename + "-exists-" + term_name + ".tmp").c_str());
    if (tmp_file.is_open()) {
      tmp_file >> term_available;
      if (term_available == "1") term_exists = true;
      tmp_file.close();
    } else {
      LOGE << "Cannot save temporary files to disk.";
    }
    return term_exists;
  }

  void PlotExporter::setGrid_(std::ofstream &file_stream) {
    if (file_stream.is_open()) {
      file_stream << "set grid lc rgb '" << "#cccccc" << "' lw 1 lt "
                  << PlotLineProperties::getGridLineType() << std::endl;
    }
  }

  void PlotExporter::generatePNG_()
  {
    std::string filename = output_path_+this->filename+"-png.gpi";
    std::string exported_plot = output_path_+this->filename+".png";
    std::ofstream fout(filename.c_str());

    if (fout.is_open()) {
      appendFileHeader_(fout);
      if (isTermAvailable_("cairo")) {
        fout << "set terminal pngcairo dashed enhanced" << std::endl;
        fout << "set output '" << exported_plot << "'" << std::endl;
        if (with_grid_) {
          setGrid_(fout);
        }
        for (uint i = 0; i < plotlines_properties_.size(); ++i) {
          fout << plotlines_properties_[i].plotStyler() << std::endl;
        }
      } else {
        fout << "# No cairo terminal found. Defaulting png terminal." << std::endl;
        fout << "set terminal png dashed enhanced" << std::endl;
        fout << "set output '" << exported_plot << "'" << std::endl;
        if (with_grid_) {
          setGrid_(fout);
        }
        for (uint i = 0; i < plotlines_properties_.size(); ++i) {
          fout << plotlines_properties_[i].plotStyler() << std::endl;
        }
      }
      generatePlot_(fout, filename);
      fout.close();
      plot_PNG_ = false;
    } else {
      LOGE << "Failed to save plot to disk.";
    }
  }

  void PlotExporter::generatePDF_()
  {
    std::string filename =  output_path_+this->filename+"-pdf.gpi";
    std::string exported_plot =  output_path_+this->filename+".pdf";
    std::ofstream fout(filename.c_str());
    
    if (fout.is_open()) {
      appendFileHeader_(fout);
      if (isTermAvailable_("cairo")) {
        fout << "set terminal pdfcairo transparent color dashed enhanced" << std::endl;
        fout << "set output '" << exported_plot << "'" << std::endl;
        if (with_grid_) {
          setGrid_(fout);
        }
        for (uint i = 0; i < plotlines_properties_.size(); ++i) {
          fout << plotlines_properties_[i].plotStyler() << std::endl;
        }
      }
      generatePlot_(fout, filename);
      fout.close();
      plot_PDF_ = false;
    } else {
      LOGE << "Failed to save plot to disk.";
    }
  }

  void PlotExporter::generateHTML_()
  {
    std::string filename = output_path_+this->filename+"-html.gpi";
    std::string exported_plot = output_path_+this->filename+".html";
    std::ofstream fout(filename.c_str());
    
    if (fout.is_open()) {
      appendFileHeader_(fout);
      if (isTermAvailable_("cairo")) {
        fout << "set terminal canvas dashed enhanced" << std::endl;
        fout << "set output '" << exported_plot << "'" << std::endl;
        if (with_grid_) {
          setGrid_(fout);
        }
        for (uint i = 0; i < plotlines_properties_.size(); ++i) {
          fout << plotlines_properties_[i].plotStyler() << std::endl;
        }
      }
      generatePlot_(fout, filename);
      fout.close();
      plot_HTML_ = false;
    } else {
      LOGE << "Failed to save plot to disk.";
    }
  }

  void PlotExporter::generateSVG_()
  {
    std::string filename = output_path_+this->filename+"-svg.gpi";
    std::string exported_plot = output_path_+this->filename+".svg";
    std::ofstream fout(filename.c_str());
    
    if (fout.is_open()) {
      appendFileHeader_(fout);
      if (isTermAvailable_("cairo")) {
        fout << "set terminal svg dashed enhanced" << std::endl;
        fout << "set output '" << exported_plot << "'" << std::endl;
        if (with_grid_) {
          setGrid_(fout);
        }
        for (int i = 0; i < plotlines_properties_.size(); ++i) {
          fout << plotlines_properties_[i].plotStyler() << std::endl;
        }
      }
      generatePlot_(fout, filename);
      fout.close();
      plot_SVG_ = false;
    } else {
      LOGE << "Failed to save plot to disk.";
    }
  }

  void PlotExporter::appendFileHeader_(std::ofstream &fout)
  {
    fout << "# gnuplot plot source file" << std::endl;
    fout << "# Auto-generated by SmartPeak Ver. " << Utilities::getSmartPeakVersion() << std::endl;
    fout << "set datafile separator ','" << std::endl;
  }

  std::vector<std::string> PlotExporter::getLegend_()
  {
    std::vector<std::string> legends;
    for (auto legend : graphvis_data_.series_names_area_) {
      std::replace_if (legend.begin(), legend.end(), [](auto x){return x == '_';}, '-');
      legends.push_back(legend);
    }
    return legends;
  }

  void PlotExporter::generatePlot_(std::ofstream &fout, const std::string &filename)
  {
    auto legends = getLegend_();
    fout << "set key font \"Helvetica, 8\"" << std::endl;
    fout << "set style increment userstyle" << std::endl;
    fout << "set autoscale" << std::endl;
    fout << "unset log" << std::endl;
    fout << "unset label" << std::endl;
    fout << "set xtic auto" << std::endl;
    fout << "set ytic auto" << std::endl;
    fout << "set title \"" << plot_title_ << "\"" << std::endl;
    fout << "set xlabel \"" << x_title_ << "\"" << std::endl;
    fout << "set ylabel \"" << y_title_ << "\"" << std::endl;
    fout << "plot ";

    for (uint i = 0; i < nr_plots_; ++i) {
      std::string filename = output_path_ + this->filename+".dat";
      fout  << "'" << filename << "' index " << i
            << " title \"" << legends[i]
            << "\" with ";

      if (plotlines_properties_[i].pointtypeNone()) {
        fout << "points";
      } else {
        fout << "linespoints";
      }
        fout << ", ";
      }
      fout << std::endl;

#if defined(__APPLE__) || defined(__linux__)
    std::system((gnuplot_path_ + " " + filename).c_str());
    std::system(("rm " + output_path_ + "*.gpi").c_str());
    std::system(("rm " + output_path_ + "*.dat").c_str());
    std::system(("rm " + output_path_ + "*.tmp").c_str());
#elif _WIN32
    std::system(("start powershell.exe -windowstyle hidden " + gnuplot_path_ + filename).c_str());
    std::system(("start powershell.exe -windowstyle hidden rm " + output_path_ + "*.gpi").c_str());
    std::system(("start powershell.exe -windowstyle hidden rm " + output_path_ + "*.dat").c_str());
    std::system(("start powershell.exe -windowstyle hidden rm " + output_path_ + "*.tmp").c_str());
#endif
  }
}
