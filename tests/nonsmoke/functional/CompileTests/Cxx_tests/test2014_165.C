template <typename CoreMesh> class MeshBase;

template <typename CoreMesh> void getCoreMesh(MeshBase<CoreMesh>& mesh) {}

template <typename CoreMesh>
class MeshBase
   {
     protected:
       // Since this is not declared using template syntax it is not clearly a template, 
       // it is in fact a template instantiation.  However, we can't use EDG's representation
       // to find the associated template declaration because the template is not defined
       // in the EDG (e.g. p->defined == false).  But we can see that the function template
       // declaration is defined (above), however since this function is a prototype IT is 
       // not defined in the EDG sense. However, that p->assoc_template == NULL, be a bug 
       // in EDG.  We can find it directly by searching the outer scopes if we want to do 
       // so as an extra step.
          friend void getCoreMesh<CoreMesh>(MeshBase<CoreMesh>& mesh);
   };

