#include "Font.h"
#include "core/Log.h"
#include "ft2build.h"
#include FT_FREETYPE_H

nimo::Font::Font(const std::string& file, unsigned int pixelSize)
{
    NIMO_DEBUG("nimo::Font::Font({})", file );
    FT_Library  library;
    FT_Face     face;
    auto error = FT_Init_FreeType( &library );
    if ( error ) {  }

    error = FT_New_Face(library, file.c_str(), 0, &face);
    if ( error )
    {
        NIMO_ERROR("Error loading font file: {}", file );
    }

    NIMO_INFO("Font face {} with {} charmaps", file, face->num_charmaps);
    FT_Set_Pixel_Sizes(face, 0, pixelSize);
    TextureSpecification spec;
    spec.filtering = TextureFiltering::Nearest;
    spec.wrapping = TextureWrapping::RepeatMirrored;
    for(unsigned char c = 0; c < 255; c++)
    {
        error = FT_Load_Char( face, c, FT_LOAD_RENDER);
        if ( error )
        {
            NIMO_WARN("Char {} not loaded: {}", c, file);
            continue;
        }

        m_glyphs[c] = {
            std::make_unique<Texture>(face->glyph->bitmap.width,face->glyph->bitmap.rows, face->glyph->bitmap.buffer, 1, spec),
            glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
            glm::ivec2(face->glyph->advance.x, face->glyph->advance.y)
        };
    }
    lineSpacing = face->size->metrics.height;
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}
nimo::Font::~Font()
{
    NIMO_DEBUG("nimo::Font::~Font()");
    m_glyphs.clear();
}