#ifndef SOLVER_H
#define SOLVER_H

namespace CodeThorn {

  class Analyzer;

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Solver interface
 */
  class Solver {

  friend class Analyzer;

  public:
    Solver();
    Solver(Analyzer* analyzer);
    virtual void run() = 0;
    virtual int getId() = 0;

  protected:
    void setAnalyzer(Analyzer* analyzer);
    void initDiagnostics(Sawyer::Message::Facility& logger, int solverId);

    Analyzer* _analyzer;
  };

} // end of namespace CodeThorn

#endif
