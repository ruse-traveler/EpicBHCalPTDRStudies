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
#include <limits>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <optional>
#include <algorithm>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>



// ============================================================================
//! Hist Helper
// ============================================================================
/*! A small namespace to help work with
 *  ROOT histograms (and graphs).
 */
namespace HistHelper {

  // ==========================================================================
  //! Binning definition
  // ==========================================================================
  /*! A small class to consolidate data
   *  for defining histogram bins.
   */ 
  class Binning {

    private:

      // members for uniform binning
      uint32_t m_num;
      double   m_start;
      double   m_stop;

      // member for variable binning
      std::optional<std::vector<double>> m_bins = std::nullopt;

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
      std::optional<std::vector<double>> GetBins() const {return m_bins;}

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
        { "number", {202, -1.5, 200.5} },
        { "energy", {202, -1., 100.}   }
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

    public:

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

        TH1D* hist;
        if (m_bins_x.GetBins().has_value()) {
          hist = new TH1D(
            m_name.data(),
            m_title.data(),
            m_bins_x.GetNum(),
            m_bins_x.GetBins().value().data()
          );
        } else {
          hist = new TH1D(
            m_name.data(),
            m_title.data(),
            m_bins_x.GetNum(),
            m_bins_x.GetStart(),
            m_bins_x.GetStop()
          );
        }
        return hist;

      }  // end 'MakeTH1()'

      // ----------------------------------------------------------------------
      //! Generate TH1DModel
      // ----------------------------------------------------------------------
      ROOT::RDF::TH1DModel MakeTH1Model() const {

        ROOT::RDF::TH1DModel hist;
        if (m_bins_x.GetBins().has_value()) {
          hist = ROOT::RDF::TH1DModel(
            m_name.data(),
            m_title.data(),
            m_bins_x.GetNum(),
            m_bins_x.GetBins().value().data()
          );
        } else {
          hist = ROOT::RDF::TH1DModel(
            m_name.data(),
            m_title.data(),
            m_bins_x.GetNum(),
            m_bins_x.GetStart(),
            m_bins_x.GetStop()
          );
        }
        return hist;

      }  // end 'MakeTH1Model()'

      /* TODO add similar for TH2, TH3 */

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

      }  // end 'ctor(std::string&, std::string&, std::vector<std::string>&, std::vector<Binning>&)'

  };  // end Definition

}  // end HistHelper namespace

#endif

// end ========================================================================
