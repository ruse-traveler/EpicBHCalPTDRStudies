/// ===========================================================================
/*! \file   HistHelper.hxx
 *  \author Derek Anderson
 *  \date   09.21.2024
 *
 *  A lightweight namespace to help work with histograms.
 */
/// ===========================================================================

#ifndef HistHelper_hxx
#define HistHelper_hxx

// c++ utilities
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDF/HistoModels.hxx>



// ============================================================================
//! Hist Helper
// ============================================================================
/*! A small namespace to help work with
 *  ROOT histograms.
 */
namespace HistHelper {

  // ------------------------------------------------------------------------
  //! Helper method to divide a range into a certain number of bins
  // ------------------------------------------------------------------------
  std::vector<double> GetBinEdges(
    const std::size_t num,
    const double start,
    const double stop
  ) {

    // throw error if start/stop are out of order
    // or if num is zero
    if (num <= 0)     assert(num > 0);
    if (start > stop) assert(start <= stop);

    // set start/stop, calculate bin steps
    const double step = (stop - start) / num;
 
    // instantiate vector to hold bins
    std::vector<double> bins;

    // and fill vector
    double edge = start;
    for (std::size_t inum = 0; inum < num; ++inum) {
      bins.push_back( edge );
      edge += step;
    }
    bins.push_back( edge );
    return bins;

  }  // end 'GetBinEdges(uint32_t, double, double)'



  // ==========================================================================
  //! Binning definition
  // ==========================================================================
  /*! A small class to consolidate data
   *  for defining histogram bins.
   */ 
  class Binning {

    private:

      // data members
      double              m_start;
      double              m_stop;
      uint32_t            m_num;
      std::vector<double> m_bins;

    public:

      // ----------------------------------------------------------------------
      //! Uniform bin getters
      //-----------------------------------------------------------------------
      uint32_t GetNum()   const {return m_num;}
      double   GetStart() const {return m_start;}
      double   GetStop()  const {return m_stop;}

      // ----------------------------------------------------------------------
      //! Variable bin getter
      // ----------------------------------------------------------------------
      std::vector<double> GetBins() const {return m_bins;}

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Binning()  {};
      ~Binning() {};

      // ----------------------------------------------------------------------
      //! ctor accepting uniform parameters
      // ----------------------------------------------------------------------
      Binning(const uint32_t num, const double start, const double stop) {

        m_num   = num;
        m_start = start;
        m_stop  = stop;
        m_bins  = GetBinEdges(m_num, m_start, m_stop);

      }  // end ctor(uint32_t, double, double)

      // ----------------------------------------------------------------------
      //! ctor accepting non-uniform parameters
      // ----------------------------------------------------------------------
      Binning(const std::vector<double> edges) {

        m_bins  = edges;
        m_num   = edges.size() - 1;
        m_start = edges.front();
        m_stop  = edges.back();

      }  // end ctor(std::vector<double>)

  };  // end Binning



  // ==========================================================================
  //! Bin Database
  // ==========================================================================
  /*! A class to centralize binning for
   *  various quantities like energy, etc.
   *  Methods are provided to update
   *  exisiting/add new bin definitions
   *  on the fly.
   */
  class Bins {

    private:

      std::map<std::string, Binning> m_bins = {
        { "number",   {202, -1.5, 200.5} },
        { "fraction", {104, -0.1, 5.1}   },
        { "chi2",     {220, -0.05, 1.05} },
        { "energy",   {202, -1., 100.}   },
        { "eta",      {30 , -1.5, 1.5}   },
        { "phi",      {320, -3.15, 3.15} }
      };

    public:

      // ----------------------------------------------------------------------
      //! Add a binning
      // ----------------------------------------------------------------------
      void Add(const std::string& name, const Binning& binning) {

        // throw error if binning already exists
        if (m_bins.count(name) >= 1) {
          assert(m_bins.count(name) == 0);
        }

        // otherwise insert new binning
        m_bins.insert( {name, binning} );
        return;

      }  // end 'Add(std::string&, Binning&)'

      // ----------------------------------------------------------------------
      //! Change a binning
      // ----------------------------------------------------------------------
      void Set(const std::string& variable, const Binning& binning) {

        // throw error if binning doesn't exist
        if (m_bins.count(variable) == 0) {
          assert(m_bins.count(variable) >= 1);
        }

        // otherwise update binning
        m_bins[variable] = binning;
        return;

      }  // end 'Set(std::string&, Binning&)'

      // ----------------------------------------------------------------------
      //! Get a binning
      // ----------------------------------------------------------------------
      Binning Get(const std::string& variable) {

        // throw error if binning doesn't exist
        if (m_bins.count(variable) == 0) {
          assert(m_bins.count(variable) == 0);
        }

        // otherwise return binning
        return m_bins[variable];

      }  // end 'Get(std::string&)'

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Bins()  {};
      ~Bins() {};

  };  // end Bins



  // ==========================================================================
  //! Histogram definition
  // ==========================================================================
  /*! A small class to consolidate necessary
   *  data to define a TH1, TH2, or TH3. Used
   *  to produce either a THND or a THNModel
   */ 
  class Definition {

    private:

      // data members
      std::string m_name;
      std::string m_title;
      std::string m_title_x;
      std::string m_title_y;
      std::string m_title_z;
      Binning     m_bins_x;
      Binning     m_bins_y;
      Binning     m_bins_z;

      // ----------------------------------------------------------------------
      //! Make histogram title
      // ----------------------------------------------------------------------
      std::string MakeTitle() const {

        std::string title = m_title;
        title.append( ";" + m_title_x );
        title.append( ";" + m_title_y );
        title.append( ";" + m_title_z );
        return title;

      }  // end 'MakeTitle()'

    public:

      // ----------------------------------------------------------------------
      //! Getters
      // ----------------------------------------------------------------------
      std::string GetName()   const {return m_name;}
      std::string GetTitle()  const {return m_title;}
      std::string GetTitleX() const {return m_title_x;}
      std::string GetTitleY() const {return m_title_y;}
      std::string GetTitleZ() const {return m_title_z;}

      // ----------------------------------------------------------------------
      //! Set and modify histogram title/name
      // ----------------------------------------------------------------------
      void SetHistTitle(const std::string& title)   {m_title = title;}
      void SetHistName(const std::string& name)     {m_name = name;}
      void PrependToName(const std::string& prefix) {m_name = prefix + m_name;}
      void AppendToName(const std::string& suffix)  {m_name = m_name + suffix;}

      // ----------------------------------------------------------------------
      //! Set axis titles via list
      // ----------------------------------------------------------------------
      void SetAxisTitles(const std::vector<std::string>& titles) {

        if (titles.size() >= 1) m_title_x = titles[0];
        if (titles.size() >= 2) m_title_y = titles[1];
        if (titles.size() >= 3) m_title_z = titles[2];
        return;

      }  // end 'SetAxisTitles(std::vector<std::string>&)'

      // ----------------------------------------------------------------------
      //! Set binning via list
      // ----------------------------------------------------------------------
      void SetAxisBins(const std::vector<Binning>& bins) {

        if (bins.size() >= 1) m_bins_x = bins[0];
        if (bins.size() >= 2) m_bins_y = bins[1];
        if (bins.size() >= 3) m_bins_z = bins[2];
        return;

      }  // end 'SetAxisBins(std::vector<Binning>&)'

      // ----------------------------------------------------------------------
      //! Generate TH1D
      // ----------------------------------------------------------------------
      TH1D* MakeTH1() const {

        // make hist + axis titles
        const std::string title = MakeTitle();

        TH1D* hist = new TH1D(
          m_name.data(),
          title.data(),
          m_bins_x.GetNum(),
          m_bins_x.GetBins().data()
        );
        return hist;

      }  // end 'MakeTH1()'

      // ----------------------------------------------------------------------
      //! Generate TH2D
      // ----------------------------------------------------------------------
      TH2D* MakeTH2() const {

        // make hist + axis titles
        const std::string title = MakeTitle();

        TH2D* hist = new TH2D(
          m_name.data(),
          title.data(),
          m_bins_x.GetNum(),
          m_bins_x.GetBins().data(),
          m_bins_y.GetNum(),
          m_bins_y.GetBins().data()
        );
        return hist;

      }  // end 'MakeTH2()'

      // ----------------------------------------------------------------------
      //! Generate TH3D
      // ----------------------------------------------------------------------
      TH3D* MakeTH3() const {

        // make hist + axis titles
        const std::string title = MakeTitle();

        TH3D* hist = new TH3D(
          m_name.data(),
          title.data(),
          m_bins_x.GetNum(),
          m_bins_x.GetBins().data(),
          m_bins_y.GetNum(),
          m_bins_y.GetBins().data(),
          m_bins_z.GetNum(),
          m_bins_z.GetBins().data()
        );
        return hist;

      }  // end 'MakeTH3()'

      // ----------------------------------------------------------------------
      //! Generate TH1DModel
      // ----------------------------------------------------------------------
      ROOT::RDF::TH1DModel MakeTH1Model() const {

        // make hist + axis titles
        const std::string title = MakeTitle();

        ROOT::RDF::TH1DModel hist = ROOT::RDF::TH1DModel(
          m_name.data(),
          title.data(),
          m_bins_x.GetNum(),
          m_bins_x.GetBins().data()
        );
        return hist;

      }  // end 'MakeTH1Model()'

      // ----------------------------------------------------------------------
      //! Generate TH2DModel
      // ----------------------------------------------------------------------
      ROOT::RDF::TH2DModel MakeTH2Model() const {

        // make hist + axis titles
        const std::string title = MakeTitle();

        ROOT::RDF::TH2DModel hist = ROOT::RDF::TH2DModel(
          m_name.data(),
          title.data(),
          m_bins_x.GetNum(),
          m_bins_x.GetBins().data(),
          m_bins_y.GetNum(),
          m_bins_y.GetBins().data()
        );
        return hist;

      }  // end 'MakeTH2Model()'

      // ----------------------------------------------------------------------
      //! Generate TH3DModel
      // ----------------------------------------------------------------------
      ROOT::RDF::TH3DModel MakeTH3Model() const {

        // make hist + axis titles
        const std::string title = MakeTitle();

        ROOT::RDF::TH3DModel hist = ROOT::RDF::TH3DModel(
          m_name.data(),
          title.data(),
          m_bins_x.GetNum(),
          m_bins_x.GetBins().data(),
          m_bins_y.GetNum(),
          m_bins_y.GetBins().data(),
          m_bins_z.GetNum(),
          m_bins_z.GetBins().data()
        );
        return hist;

      }  // end 'MakeTH3Model()'

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Definition()   {};
       ~Definition() {};

      // ----------------------------------------------------------------------
      //! ctor accepting arguments
      // ----------------------------------------------------------------------
      Definition(
        const std::string& hist_name,
        const std::string& hist_title,
        const std::vector<std::string>& axis_titles,
        const std::vector<Binning>& axis_bins
      ) {

        SetHistName(hist_name);
        SetHistTitle(hist_title);
        SetAxisTitles(axis_titles);
        SetAxisBins(axis_bins);

      }  // end ctor(std::string&, std::string&, std::vector<std::string>&, std::vector<Binning>&)

  };  // end Definition

}  // end HistHelper namespace

#endif

// end ========================================================================
