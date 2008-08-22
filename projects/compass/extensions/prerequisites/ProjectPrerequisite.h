#ifndef COMPASS_PROJECT_PREREQUISITE_H
#define COMPASS_PROJECT_PREREQUISITE_H

// The prerequisite for getting the SgProject
class ProjectPrerequisite: public Prerequisite 
{
  private:
    SgProject* proj;
  public:
    ProjectPrerequisite(): Prerequisite("SgProject"), proj(NULL) {}
    void run(SgProject* p) 
    {
      if (done) return;
      proj = p;
      done = true;
    } //run(SgProject *p)

    PrerequisiteList getPrerequisites() const {
      return PrerequisiteList();
    } //getPrerequisites()

    SgProject* getProject() const {
      ROSE_ASSERT (done);
      return proj;
    } //getProject()
}; //class ProjectPrerequisite

extern ProjectPrerequisite projectPrerequisite;

#endif
