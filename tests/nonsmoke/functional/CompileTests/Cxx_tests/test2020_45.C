// RC-72:

    namespace conduit {
      struct Node {};
      void about(Node&);
      namespace relay {
        void about(conduit::Node&);
        void about() {
          Node n;
          relay::about(n);
        }
      }
    }

