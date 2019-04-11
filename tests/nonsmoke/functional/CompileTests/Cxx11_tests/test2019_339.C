

// ROSE-1905: (C++03) template specialization declaration cannot be a friend

        template <typename T>
        class hash;

        struct error_code {
          private:
            friend class hash<error_code>;
        };

        template<>
        struct hash<error_code> {};

