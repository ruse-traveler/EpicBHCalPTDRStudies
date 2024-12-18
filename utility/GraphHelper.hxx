/// ===========================================================================
/*! \file   GraphHelper.hxx
 *  \author Derek Anderson
 *  \date   09.29.2024
 *
 *  A lightweight namespace to help work with graphs.
 */
/// ===========================================================================

#ifndef GraphHelper_hxx
#define GraphHelper_hxx

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
#include <TGraph.h>
#include <TGraph2D.h>
#include <TGraphErrors.h>
#include <TGraph2DErrors.h>
#include <TGraphAsymmErrors.h>
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,28,0)
#include <TGraph2DAsymmErrors.h>
#endif



// ============================================================================
//! Graph Helper
// ============================================================================
/*! A small namespace to help work with
 *  ROOT graphs.
 */
namespace GraphHelper {

  // ==========================================================================
  //! Point
  // ==========================================================================
  /*! A small struct to consolidate data
   *  for a point on a graph
   */
  struct Point {

    // members
    double x;
    double y;
    double z;
    double ex;
    double ex_lo;
    double ex_hi;
    double ey;
    double ey_lo;
    double ey_hi;
    double ez;
    double ez_lo;
    double ez_hi;

    // ------------------------------------------------------------------------
    //! default ctor/dtor
    // ------------------------------------------------------------------------
    Point()  {};
    ~Point() {};

    // ------------------------------------------------------------------------
    //! ctor without errors (x, y)
    // ------------------------------------------------------------------------
    Point(const double x_arg, const double y_arg) {

      x     = x_arg;
      y     = y_arg;
      z     = 0.;
      ex    = 0.;
      ex_lo = 0.;
      ex_hi = 0.;
      ey    = 0.;
      ey_lo = 0.;
      ey_hi = 0.;
      ez    = 0.;
      ez_lo = 0.;
      ez_hi = 0.;

    }  // end ctor(double, double)

    // ------------------------------------------------------------------------
    //! ctor without errors (x, y, z)
    // ------------------------------------------------------------------------
    Point(const double x_arg, const double y_arg, const double z_arg) {

      x     = x_arg;
      y     = y_arg;
      z     = z_arg;
      ex    = 0.;
      ex_lo = 0.;
      ex_hi = 0.;
      ey    = 0.;
      ey_lo = 0.;
      ey_hi = 0.;
      ez    = 0.;
      ez_lo = 0.;
      ez_hi = 0.;

    }  // end ctor(double, double, double)

    // ------------------------------------------------------------------------
    //! ctor accepting symmetric errors (x, y)
    // ------------------------------------------------------------------------
    Point(
      const double x_arg,
      const double y_arg,
      const double ex_arg,
      const double ey_arg
    ) {

      x     = x_arg;
      y     = y_arg;
      z     = 0.;
      ex    = ex_arg;
      ex_lo = ex_arg / 2.;
      ex_hi = ex_arg / 2.;
      ey    = ey_arg;
      ey_lo = ey_arg / 2.;
      ey_hi = ey_arg / 2.;
      ez    = 0.;
      ez_lo = 0.;
      ez_hi = 0.;

    }  // end ctor(double, double, double, double)'

    // ------------------------------------------------------------------------
    //! ctor accepting symmetric errors (x, y, z)
    // ------------------------------------------------------------------------
    Point(
      const double x_arg,
      const double y_arg,
      const double z_arg,
      const double ex_arg,
      const double ey_arg,
      const double ez_arg
    ) {

      x     = x_arg;
      y     = y_arg;
      z     = z_arg;
      ex    = ex_arg;
      ex_lo = ex_arg / 2.;
      ex_hi = ex_arg / 2.;
      ey    = ey_arg;
      ey_lo = ey_arg / 2.;
      ey_hi = ey_arg / 2.;
      ez    = ez_arg;
      ez_lo = ez_arg / 2.;
      ez_hi = ez_arg / 2.;

    }  // end ctor(double, double, double, double, double, double)'

    // ------------------------------------------------------------------------
    //! ctor accepting asymmetric errors (x, y)
    // ------------------------------------------------------------------------
    Point(
      const double x_arg,
      const double y_arg,
      const std::pair<double, double> ex_arg,
      const std::pair<double, double> ey_arg
    ) {

      x     = x_arg;
      y     = y_arg;
      z     = 0.;
      ex_lo = ex_arg.first;
      ex_hi = ex_arg.second;
      ex    = ex_arg.first + ex_arg.second;
      ey_lo = ey_arg.first;
      ey_hi = ey_arg.second;
      ey    = ey_arg.first + ey_arg.second;
      ez    = 0.;
      ez_lo = 0.;
      ez_hi = 0.;

    }  // end ctor(double x 2, std::pair<double, double> x 2)

    // ------------------------------------------------------------------------
    //! ctor accepting asymmetric errors (x, y, z)
    // ------------------------------------------------------------------------
    Point(
      const double x_arg,
      const double y_arg,
      const double z_arg,
      const std::pair<double, double> ex_arg,
      const std::pair<double, double> ey_arg,
      const std::pair<double, double> ez_arg
    ) {

      x     = x_arg;
      y     = y_arg;
      z     = z_arg;
      ex_lo = ex_arg.first;
      ex_hi = ex_arg.second;
      ex    = ex_arg.first + ex_arg.second;
      ey_lo = ey_arg.first;
      ey_hi = ey_arg.second;
      ey    = ey_arg.first + ey_arg.second;
      ez_lo = ez_arg.first;
      ez_hi = ez_arg.second;
      ez    = ez_arg.first + ez_arg.second;

    }  // end ctor(double x 3, std::pair<double, double> x 3)

  };   // end Point 



  // ==========================================================================
  //! Graph definition
  // ==========================================================================
  /*! A small class to consolidate necessary data
   *  to define a TGraph, TGraphErrors, or
   *  TGraphAsymmErrors
   */ 
  class Definition {

    private:

      // options for accessing specific members of a point
      enum Member {X, Y, Z, EX, EXlo, EXhi, EY, EYlo, EYhi, EZ, EZlo, EZhi};

      // data members
      std::string        m_name;
      std::vector<Point> m_points;

      // ----------------------------------------------------------------------
      //! Helper method to pull a vector of x, y, etc. from a vector of points
      // ----------------------------------------------------------------------
      std::vector<double> GetMembers(const Member member) const {

        std::vector<double> members;
        for (const auto& point : m_points) {

          // select relevant member
          switch (member) {

            case Member::X:
              members.push_back(point.x);
              break;

            case Member::Y:
              members.push_back(point.y);
              break;

            case Member::Z:
              members.push_back(point.z);
              break;

            case Member::EX:
              members.push_back(point.ex);
              break;

            case Member::EXlo:
              members.push_back(point.ex_lo);
              break;

            case Member::EXhi:
              members.push_back(point.ex_hi);
              break;

            case Member::EY:
              members.push_back(point.ey);
              break;

            case Member::EYlo:
              members.push_back(point.ey_lo);
              break;

            case Member::EYhi:
              members.push_back(point.ey_hi);
              break;

            case Member::EZ:
              members.push_back(point.ez);
              break;

            case Member::EZlo:
              members.push_back(point.ez_lo);
              break;

            case Member::EZhi:
              members.push_back(point.ez_hi);
              break;

            // by default return nothing
            default:
              break;

          }  // end switch(member)

        }
        return members;

      }  // end 'GetMembers(Member)'

    public:

      // ----------------------------------------------------------------------
      //! Getters
      // ----------------------------------------------------------------------
      std::string        GetName()   const {return m_name;}
      std::vector<Point> GetPoints() const {return m_points;}

      // ----------------------------------------------------------------------
      //! Setters
      // ----------------------------------------------------------------------
      void SetName(const std::string& name)            {m_name = name;}
      void SetPoints(const std::vector<Point>& points) {m_points = points;}

      // ----------------------------------------------------------------------
      //! Reset points
      // ----------------------------------------------------------------------
      void ResetPoints() {

        m_points.clear();
        return;

      }  // end 'ResetPoints()'

      // ----------------------------------------------------------------------
      //! Add a point
      // ----------------------------------------------------------------------
      void AddPoint(const Point& point) {

        m_points.push_back(point);
        return;

      }  // end 'AddPoint(Point&)'

      // ----------------------------------------------------------------------
      //! Make a TGraph
      // ----------------------------------------------------------------------
      TGraph* MakeTGraph() const {

        // decompose points
        std::vector<double> x_vals = GetMembers(Member::X);
        std::vector<double> y_vals = GetMembers(Member::Y);

        // create TGraph
        TGraph* graph = new TGraph(
          m_points.size(),
          x_vals.data(),
          y_vals.data()
        );
        graph -> SetName(m_name.data());
        return graph;

      }  // end 'MakeTGraph()'

      // ----------------------------------------------------------------------
      //! Make a TGraph2D
      // ----------------------------------------------------------------------
      TGraph2D* MakeTGraph2D() const {

        // decompose points
        std::vector<double> x_vals = GetMembers(Member::X);
        std::vector<double> y_vals = GetMembers(Member::Y);
        std::vector<double> z_vals = GetMembers(Member::Z);

        // create graph
        TGraph2D* graph = new TGraph2D(
          m_points.size(),
          x_vals.data(),
          y_vals.data(),
          z_vals.data()
        );
        graph -> SetName(m_name.data());
        return graph;

      }  // end 'MakeTGraph2D()'

      // ----------------------------------------------------------------------
      //! Make a TGraphErrors
      // ----------------------------------------------------------------------
      TGraphErrors* MakeTGraphErrors() const {

        // decompose points
        std::vector<double> x_vals = GetMembers(Member::X);
        std::vector<double> y_vals = GetMembers(Member::Y);
        std::vector<double> x_errs = GetMembers(Member::EX);
        std::vector<double> y_errs = GetMembers(Member::EY);

        // create TGraph
        TGraphErrors* graph = new TGraphErrors(
          m_points.size(),
          x_vals.data(),
          y_vals.data(),
          x_errs.data(),
          y_errs.data()
        );
        graph -> SetName(m_name.data());
        return graph;

      }  // end 'MakeTGraphErrors()'

      // ----------------------------------------------------------------------
      //! Make a TGraph2DErrors
      // ----------------------------------------------------------------------
      TGraph2DErrors* MakeTGraph2DErrors() const {

        // decompose points
        std::vector<double> x_vals = GetMembers(Member::X);
        std::vector<double> y_vals = GetMembers(Member::Y);
        std::vector<double> z_vals = GetMembers(Member::Z);
        std::vector<double> x_errs = GetMembers(Member::EX);
        std::vector<double> y_errs = GetMembers(Member::EY);
        std::vector<double> z_errs = GetMembers(Member::EZ);

        // create graph
        TGraph2DErrors* graph = new TGraph2DErrors(
          m_points.size(),
          x_vals.data(),
          y_vals.data(),
          z_vals.data(),
          x_errs.data(),
          y_errs.data(),
          z_errs.data()
        );
        graph -> SetName(m_name.data());
        return graph;

      }  // end 'MakeTGraph2DErrors()'

      // ----------------------------------------------------------------------
      //! Make a TGraphAsymmErrors
      // ----------------------------------------------------------------------
      TGraphAsymmErrors* MakeTGraphAsymmErrors() const {

        // decompose points
        std::vector<double> x_vals    = GetMembers(Member::X);
        std::vector<double> y_vals    = GetMembers(Member::Y);
        std::vector<double> x_lo_errs = GetMembers(Member::EXlo);
        std::vector<double> x_hi_errs = GetMembers(Member::EXhi);
        std::vector<double> y_lo_errs = GetMembers(Member::EYlo);
        std::vector<double> y_hi_errs = GetMembers(Member::EYhi);

        // create TGraph
        TGraphAsymmErrors* graph = new TGraphAsymmErrors(
          m_points.size(),
          x_vals.data(),
          y_vals.data(),
          x_lo_errs.data(),
          x_hi_errs.data(),
          y_lo_errs.data(),
          y_hi_errs.data()
        );
        graph -> SetName(m_name.data());
        return graph;

      }  // end 'MakeTGraphAsymmErrors()'

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,28,0)
      // ----------------------------------------------------------------------
      //! Make a TGraph2DAsymmErrors
      // ----------------------------------------------------------------------
      TGraph2DAsymmErrors* MakeTGraph2DAsymmErrors() const {

        // decompose points
        std::vector<double> x_vals    = GetMembers(Member::X);
        std::vector<double> y_vals    = GetMembers(Member::Y);
        std::vector<double> z_vals    = GetMembers(Member::Z);
        std::vector<double> x_lo_errs = GetMembers(Member::EXlo);
        std::vector<double> x_hi_errs = GetMembers(Member::EXhi);
        std::vector<double> y_lo_errs = GetMembers(Member::EYlo);
        std::vector<double> y_hi_errs = GetMembers(Member::EYhi);
        std::vector<double> z_lo_errs = GetMembers(Member::EZlo);
        std::vector<double> z_hi_errs = GetMembers(Member::EZhi);

        // create graph
        TGraph2DAsymmErrors* graph = new TGraph2DAsymmErrors(
          m_points.size(),
          x_vals.data(),
          y_vals.data(),
          z_vals.data(),
          x_lo_errs.data(),
          x_hi_errs.data(),
          y_lo_errs.data(),
          y_hi_errs.data(),
          z_lo_errs.data(),
          z_hi_errs.data()
        );
        graph -> SetName(m_name.data());
        return graph;

      }  // end 'MakeTGraphAsymmErrors()'
#endif

      // ----------------------------------------------------------------------
      //! default ctor/dtor
      // ----------------------------------------------------------------------
      Definition()  {};
      ~Definition() {};

      // ----------------------------------------------------------------------
      //! ctor accepting a name
      // ----------------------------------------------------------------------
      Definition(const std::string& name) {

        SetName(name);

      }  // end ctor(std::string)

  };  // end Definition

}  // end GraphHelper namespace

#endif

// end ========================================================================
