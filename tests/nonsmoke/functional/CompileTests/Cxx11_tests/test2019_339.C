

// ROSE-1905: (C++03) template specialization declaration cannot be a friend

        template <typename T>
        class hash;

        struct error_code {
          private:
         // Unparsed as: template<> friend struct hash< error_code  > ;
         // The use of template<> causes it to be a template specialization which is then in the wrong scope.
         // friend class hash<error_code>;
            friend class hash<error_code>;
        };

        template<>
        struct hash<error_code> {};

