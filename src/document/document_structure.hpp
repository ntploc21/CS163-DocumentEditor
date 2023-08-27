#ifndef DOCUMENT_DOCUMENT_STRUCTURE_CPP
#define DOCUMENT_DOCUMENT_STRUCTURE_CPP

#include "rope/rope.hpp"

namespace document {
    inline namespace HeadingLevel {
        enum Level { None, H1, H2, H3, H4, H5, H6 };
    }  // namespace HeadingLevel

    inline namespace Alignment {
        enum Align { Left, Center, Right };
    }  // namespace Alignment

    inline namespace List {
        enum Type { None, Bullet, Number };
    }  // namespace List

    class DocumentStructure {
    public:
        DocumentStructure();

        void insert_line(std::size_t index);
        void delete_line(std::size_t index);

        void delete_lines(std::size_t start, std::size_t end);

        void set_headings(std::size_t index, std::size_t level);
        void set_alignment(std::size_t index, std::size_t alignment);
        void set_list(std::size_t index, std::size_t list);

        std::size_t get_headings(std::size_t index) const;
        std::size_t get_alignment(std::size_t index) const;
        std::size_t get_list(std::size_t index) const;

        Rope& rope();
        const Rope& rope() const;

    private:
        Rope mRope;
    };
}  // namespace document

#endif  // DOCUMENT_DOCUMENT_STRUCTURE_CPP