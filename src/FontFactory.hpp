#ifndef FONTFACTORY_HPP
#define FONTFACTORY_HPP

#include <cassert>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "raygui.h"

class FontFactory {
public:
    FontFactory();

    /**
     * @brief The destructor.
     * @details The destructor that is used to destroy the font holder.
     */
    ~FontFactory();

    /**
     * @brief Load the font from the file.
     * @details This function is used to load the font from the file.
     * @param id The font ID.
     * @param filename The font filename.
     * @see Font::ID
     */
    void Load(const std::string& id, const std::string& filename);

    /**
     * @brief Retrieve the font.
     * @details This function is used to retrieve the font.
     * @param id The font ID.
     * @return The font.
     * @see Font::ID
     */
    Font& Get(const std::string& id);

    /**
     * @brief Retrieve the font.
     * @details This function is used to retrieve the font.
     * @param id The font ID.
     * @return The font.
     * @see Font::ID
     */
    const Font& Get(const std::string& id) const;

private:
    /**
     * @brief Insert the font.
     * @details This function is used to insert the font.
     * @param id The font ID.
     * @param font The font.
     * @see Font::ID
     */
    void InsertResource(const std::string& id, std::unique_ptr< Font > font);

private:
    /**
     * @brief The font map.
     * @details The font map that is used to store the fonts. As the flyweight
     * pattern is used, the fonts are stored as the pointers and should be
     * stored in the map.
     */
    std::map< std::string, std::unique_ptr< Font > > mFontMap;
};
#endif  // FONTHOLDER_HPP