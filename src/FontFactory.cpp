#include "FontFactory.hpp"

#include <iostream>

static int *CodepointRemoveDuplicates(int *codepoints, int codepointCount,
                                      int *codepointsResultCount) {
    int codepointsNoDupsCount = codepointCount;
    int *codepointsNoDups = (int *)calloc(codepointCount, sizeof(int));
    memcpy(codepointsNoDups, codepoints, codepointCount * sizeof(int));

    // Remove duplicates
    for (int i = 0; i < codepointsNoDupsCount; i++) {
        for (int j = i + 1; j < codepointsNoDupsCount; j++) {
            if (codepointsNoDups[i] == codepointsNoDups[j]) {
                for (int k = j; k < codepointsNoDupsCount; k++)
                    codepointsNoDups[k] = codepointsNoDups[k + 1];

                codepointsNoDupsCount--;
                j--;
            }
        }
    }

    // NOTE: The size of codepointsNoDups is the same as original array but
    // only required positions are filled (codepointsNoDupsCount)

    *codepointsResultCount = codepointsNoDupsCount;
    return codepointsNoDups;
}

void FontFactory::Load(const std::string &id, const std::string &filename) {
    std::unique_ptr< Font > font(new Font());

    static char *text =
        " !\"#$%&\'()*+,-./"
        "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
        "abcdefghijklmnopqrstuvwxyz{|}~"
        "aAàÀảẢãÃáÁạẠăĂằẰẳẲẵẴắẮặẶâÂầẦẩẨẫẪấẤậẬbBcCdDđĐeEèÈẻẺẽẼéÉẹẸêÊềỀểỂễỄếẾệỆ"
        "fFgGhHiIìÌỉỈĩĨíÍịỊjJkKlLmMnNoOòÒỏỎõÕóÓọỌôÔồỒổỔỗỖốỐộỘơƠờỜởỞỡỠớỚợỢpPqQrR"
        "sStTuUùÙủỦũŨúÚụỤưƯừỪửỬữỮứỨựỰvVwWxXyYỳỲỷỶỹỸýÝỵỴzZ";

    int codepointCount = 0;
    int *codepoints = LoadCodepoints(text, &codepointCount);

    // Removed duplicate codepoints to generate smaller font atlas
    int codepointsNoDupsCount = 0;
    int *codepointsNoDups = CodepointRemoveDuplicates(
        codepoints, codepointCount, &codepointsNoDupsCount);
    UnloadCodepoints(codepoints);

    // Load font containing all the provided codepoint glyphs
    // A texture font atlas is automatically generated
    // *font = LoadFontEx(filename.c_str(), 36, codepointsNoDups, 256);
    *font = LoadFontEx(filename.c_str(), 72, nullptr, 0);
    SetTextureFilter(font->texture, TEXTURE_FILTER_ANISOTROPIC_16X);

    // std::cout << "Loading font: " << filename.c_str() << std::endl;

    // Free codepoints, atlas has already been generated
    free(codepointsNoDups);

    InsertResource(id, std::move(font));
}

Font &FontFactory::Get(const std::string &id) {
    auto found = mFontMap.find(id);
    assert(found != mFontMap.end());
    return *found->second.get();
}

const Font &FontFactory::Get(const std::string &id) const {
    auto found = mFontMap.find(id);
    assert(found != mFontMap.end());
    return *found->second.get();
}

void FontFactory::InsertResource(const std::string &id,
                                 std::unique_ptr< Font > font) {
    auto inserted = mFontMap.insert(std::make_pair(id, std::move(font)));
    assert(inserted.second);
}

FontFactory::FontFactory() {}

FontFactory::~FontFactory() {
    for (auto &pair : mFontMap) {
        UnloadFont(*pair.second);
    }
}