#include "document_structure.hpp"

namespace document {
    DocumentStructure::DocumentStructure() : mRope{"000\n"} {}

    void DocumentStructure::insert_line(std::size_t index) {
        mRope = mRope.insert(index * 4, "000\n");
    }
    void DocumentStructure::delete_line(std::size_t index) {
        std::size_t idx = mRope.find_line_start(index);
        mRope = mRope.erase(idx, 4);

        if (mRope.length() == 0) {
            mRope = mRope.append("000\n");
        }
    }

    void DocumentStructure::delete_lines(std::size_t start, std::size_t end) {
        if (start == end) return;

        std::size_t idx = mRope.find_line_start(start);
        std::size_t len = mRope.find_line_start(end) - idx;
        mRope = mRope.erase(idx, len);

        if (mRope.length() == 0) {
            mRope = mRope.append("000\n");
        }
    }

    void DocumentStructure::set_headings(std::size_t index, std::size_t level) {
        std::size_t idx = mRope.find_line_start(index);
        mRope = mRope.replace(idx, 1, std::to_string(level));
    }
    void DocumentStructure::set_alignment(std::size_t index,
                                          std::size_t alignment) {
        std::size_t idx = mRope.find_line_start(index) + 1;
        mRope = mRope.replace(idx, 1, std::to_string(alignment));
    }
    void DocumentStructure::set_list(std::size_t index, std::size_t list) {
        std::size_t idx = mRope.find_line_start(index) + 2;
        mRope = mRope.replace(idx, 1, std::to_string(list));
    }

    std::size_t DocumentStructure::get_headings(std::size_t index) const {
        std::size_t idx = mRope.find_line_start(index);
        return mRope[idx].getChar()[0] - '0';
    }

    std::size_t DocumentStructure::get_alignment(std::size_t index) const {
        std::size_t idx = mRope.find_line_start(index) + 1;
        return mRope[idx].getChar()[0] - '0';
    }

    std::size_t DocumentStructure::get_list(std::size_t index) const {
        std::size_t idx = mRope.find_line_start(index) + 2;
        return mRope[idx].getChar()[0] - '0';
    }

    Rope& DocumentStructure::rope() { return mRope; }

    const Rope& DocumentStructure::rope() const { return mRope; }
}  // namespace document