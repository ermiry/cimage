#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "blackrock.h"
#include "ui/ui.h"

#include "engine/renderer.h"
#include "engine/sprites.h"
#include "engine/textures.h"
#include "engine/input.h"

#include "utils/log.h"
#include "utils/myUtils.h"

/*** COMMON RGBA COLORS ***/

RGBA_Color RGBA_NO_COLOR = { 0, 0, 0, 0 };
RGBA_Color RGBA_WHITE = { 255, 255, 255, 255 };
RGBA_Color RGBA_BLACK = { 0, 0, 0, 255 };

/*** BASIC UI ELEMENTS ***/

#pragma region BASIC UI ELEMENTS

static inline UIRect ui_rect_create (u32 x, u32 y, u32 w, u32 h) {

    UIRect ret = { x, y, w, h };
    return ret;

}

static inline UIRect ui_rect_union (UIRect a, UIRect b) {

    u32 x1 = MIN (a.x, b.x);
    u32 y1 = MIN (a.y, b.y);
    u32 x2 = MAX (a.x + a.w, b.x + b.w);
    u32 y2 = MAX (a.y + a.h, b.y + b.h);

    UIRect retval = { x1, y1, MAX (0, x2 - x1), MAX (0, y2 - y1) };
    return retval;

}

RGBA_Color ui_rgba_color_create (u8 r, u8 g, u8 b, u8 a) { 

    RGBA_Color retval = { r, g, b, a };
    return retval;

}

#pragma endregion

/*** UI ELEMENTS ***/

#pragma region UI ELEMENTS

UIElement **ui_elements = NULL;
static u32 max_ui_elements;
u32 curr_max_ui_elements;
static u32 new_ui_element_id;

static u8 ui_elements_realloc (void) {

    u32 new_max_ui_elements = curr_max_ui_elements * 2;

    ui_elements = realloc (ui_elements, new_max_ui_elements * sizeof (UIElement *));
    if (ui_elements) {
        max_ui_elements = new_max_ui_elements;
        return 0;
    }

    return 1;

}

// init our ui elements structures
static u8 ui_elements_init (void) {

    ui_elements = (UIElement **) calloc (DEFAULT_MAX_UI_ELEMENTS, sizeof (UIElement *));
    if (ui_elements) {
        for (u32 i = 0; i < DEFAULT_MAX_UI_ELEMENTS; i++) ui_elements[i] = NULL;

        max_ui_elements = DEFAULT_MAX_UI_ELEMENTS;
        curr_max_ui_elements = 0;
        new_ui_element_id = 0;

        return 0;
    }

    return 1;

}

static i32 ui_element_get_free_spot (void) {

    for (u32 i = 0; i < curr_max_ui_elements; i++) 
        if (ui_elements[i]->id == -1)
            return i;

    return -1;

}

// ui element constructor
static UIElement *ui_element_new (UIElementType type) {

    UIElement *new_element = NULL;

    // first check if we have a reusable ui element
    i32 spot = ui_element_get_free_spot ();

    if (spot >= 0) {
        new_element = ui_elements[spot];
        new_element->id = spot;
        new_element->type = type;
        new_element->element = NULL;
    }

    else {
        if (new_ui_element_id >= max_ui_elements) ui_elements_realloc ();

        new_element = (UIElement *) malloc (sizeof (UIElement));
        if (new_element) {
            new_element->id = new_ui_element_id;
            new_element->type = type;
            new_element->element = NULL;
            ui_elements[new_element->id] = new_element;
            new_ui_element_id++;
            curr_max_ui_elements++;
        }
    }

    return new_element;

}

void ui_textBox_destroy (TextBox *textbox);

// mark as inactive or reusable the ui element
static void ui_element_destroy (UIElement *ui_element) {

    if (ui_element) {
        ui_element->id = -1;
        
        switch (ui_element->type) {
            case UI_TEXTBOX: ui_textBox_destroy ((TextBox *) ui_element->element); break;
            case UI_BUTTON: break;

            default: break;
        }
    }

}

static void ui_element_delete (UIElement *ui_element) {

    if (ui_element) {
        ui_element_destroy (ui_element);
        free (ui_element);
    }

}

#pragma endregion

#pragma region FONT 

static const char *mainFontPath = "fonts/Roboto-Regular.ttf";
Font *mainFont = NULL;

static u8 has_render_target_support = 0;

/*** MISC ***/

static int u8_charsize (const char *character) {

    if (!character) return 0;

    if((unsigned char) *character <= 0x7F) return 1;
    else if((unsigned char) *character < 0xE0) return 2;
    else if((unsigned char) *character < 0xF0) return 3;
    else return 4;

    return 1;
}

static inline const char *u8_next (const char *string) {

    return string + u8_charsize (string);

}

static int u8_charcpy (char *buffer, const char *source, int buffer_size) {

    if(buffer == NULL || source == NULL || buffer_size < 1)
        return 0;

    int charsize = u8_charsize (source);
    if (charsize > buffer_size)
        return 0;

    memcpy (buffer, source, charsize);

    return charsize;

}

static u32 get_code_point_from_UTF8 (const char **c, u8 advancePtr) {

    if (c && *c) {
        u32 retval = 0;
        const char *str = *c;

        if((unsigned char)*str <= 0x7F) retval = *str;
        else if((unsigned char)*str < 0xE0) {
            retval |= (unsigned char)(*str) << 8;
            retval |= (unsigned char)(*(str + 1));
            if (advancePtr) *c += 1;
        }

        else if((unsigned char)*str < 0xF0) {
            retval |= (unsigned char)(*str) << 16;
            retval |= (unsigned char)(*(str + 1)) << 8;
            retval |= (unsigned char)(*(str + 2));
            if (advancePtr) *c += 2;
        }
        else {
            retval |= (unsigned char)(*str) << 24;
            retval |= (unsigned char)(*(str + 1)) << 16;
            retval |= (unsigned char)(*(str + 2)) << 8;
            retval |= (unsigned char)(*(str + 3));
            if (advancePtr) *c += 3;
        }

        return retval;
    }

    return 0;

}

static char *font_get_string_ASCII (void) {

    static char *buffer = NULL;
    if (!buffer) {
        int i;
        char c;
        buffer = (char *) calloc (512, sizeof (char));
        i = 0;
        c = 32;
        while (1) {
            buffer[i] = c;
            if (c == 126)
                break;
            ++i;
            ++c;
        }
    }

    char *retval = (char*) calloc (strlen (buffer) + 1, sizeof (char));
    if (retval) strcpy (retval, buffer);

    return retval;

}

static inline SDL_Surface *font_create_surface (u32 width, u32 height) {

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return SDL_CreateRGBSurface (SDL_SWSURFACE, width, height, 32, 
            0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    #else
        return SDL_CreateRGBSurface (SDL_SWSURFACE, width, height, 32, 
            0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    #endif

}

/*** FONT MAP ***/

static FontMap *font_map_create (u32 n_buckets) {

    FontMap *fontMap = (FontMap *) malloc (sizeof (FontMap));
    if (fontMap) {
        fontMap->n_buckets = n_buckets;
        fontMap->buckets = (FontMapNode **) calloc (n_buckets, sizeof (FontMapNode *));
        for (u32 i = 0; i < n_buckets; i++) fontMap->buckets[i] = NULL;
    }

    return fontMap;

}

static void font_map_destroy (FontMap *fontMap) {

    if (fontMap) {
        FontMapNode *node = NULL, *last = NULL;;
        for (u32 i = 0; i < fontMap->n_buckets; i++) {
            node = fontMap->buckets[i];
            if (node) {
                last = node;
                node = node->next;
                free (last);
            }
        }

        free (fontMap->buckets);
        free (fontMap);
    }

}

static GlyphData *font_map_find (FontMap *fontMap, u32 codepoint) {

    if (fontMap) {
        FontMapNode *node = NULL;
        u32 index = codepoint % fontMap->n_buckets;

        for (node = fontMap->buckets[index]; node != NULL; node = node->next)
            if (node->key == codepoint)
                return &node->value;
    }

    return NULL;    

}

static GlyphData *font_map_insert (FontMap *map, u32 codepoint, GlyphData glyph) {

    if (map) {
        FontMapNode *node = NULL;
        u32 index = codepoint % map->n_buckets;

        if (!map->buckets[index]) {
            node = map->buckets[index] = (FontMapNode *) malloc (sizeof (FontMapNode));
            node->key = codepoint;
            node->value = glyph;
            node->next = NULL;
            return &node->value;
        }

        for(node = map->buckets[index]; node != NULL; node = node->next) {
            if(!node->next) {
                node->next = (FontMapNode*) malloc (sizeof (FontMapNode));
                node = node->next;

                node->key = codepoint;
                node->value = glyph;
                node->next = NULL;
                return &node->value;
            }
        }
    }

    return NULL;

}

/*** GLYPH ***/
static GlyphData glyph_data_make (int cacheLevel, i16 x, i16 y, u16 w, u16 h) {

    GlyphData gd;

    gd.rect.x = x;
    gd.rect.y = y;
    gd.rect.w = w;
    gd.rect.h = h;
    gd.cacheLevel = cacheLevel;

    return gd;

}

static GlyphData *glyph_data_pack (Font *font, u32 codepoint, u16 width, 
    u16 maxWidth, u16 maxHeight) {

    FontMap *glyphs = font->glyphs;
    GlyphData *last_glyph = &font->last_glyph;
    u16 height = font->height + 1;

    if (last_glyph->rect.x + last_glyph->rect.w + width >= maxWidth - 1) {
        if(last_glyph->rect.y + height + height >= maxHeight - 1) {
            last_glyph->cacheLevel = font->glyph_cache_count;
            last_glyph->rect.x = 1;
            last_glyph->rect.y = 1;
            last_glyph->rect.w = 0;
            return NULL;
        }

        else {
            last_glyph->rect.x = 1;
            last_glyph->rect.y += height;
            last_glyph->rect.w = 0;
        }
    }

    // move to next space
    last_glyph->rect.x += last_glyph->rect.w + 1 + 1;
    last_glyph->rect.w = width;

    return font_map_insert (glyphs, codepoint, 
        glyph_data_make (last_glyph->cacheLevel, last_glyph->rect.x, last_glyph->rect.y,
                        last_glyph->rect.w, last_glyph->rect.h));
}

static u8 glyph_get_data (Font *font, GlyphData *result, u32 codepoint) {

    GlyphData *g = font_map_find (font->glyphs, codepoint);
    if (g) {
        *result = *g;
        return 0;   // success
    } 

    // TODO: maybe add the new glyph data to the font cache? check draft code

    return 1;   // error

}


static u8 glyph_set_cache_level (Font* font, int cache_level, SDL_Texture *cache_texture) {

    if (font && cache_level >= 0) {
        if (cache_level <= font->glyph_cache_count + 1) {
            if (cache_level == font->glyph_cache_count) {
                font->glyph_cache_count++;

                if (font->glyph_cache_count > font->glyph_cache_size) {
                    font->glyph_cache = 
                        (SDL_Texture **) realloc (font->glyph_cache, sizeof (font->glyph_cache_count));
                    font->glyph_cache_size = font->glyph_cache_count;
                }
            }

            font->glyph_cache[cache_level] = cache_texture;

            return 0;
        }
    }

    return 1;   // error

}

static inline SDL_Texture *glyph_get_cache_level (Font *font, int cacheLevel) {

    if (font && cacheLevel >= 0 && cacheLevel <= font->glyph_cache_count)
        return font->glyph_cache[cacheLevel];

}

static u8 glyph_upload_cache (Font *font, int cacheLevel, SDL_Surface *dataSurface) {

    if (font && dataSurface) {
        SDL_Texture *new_level = NULL;

        if (has_render_target_support) {
            char old_filter_mode[16];  
            snprintf(old_filter_mode, 16, "%s", SDL_GetHint (SDL_HINT_RENDER_SCALE_QUALITY));

            if (font->filter == FILTER_LINEAR) SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "1");
            else SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "0");
            
            new_level = SDL_CreateTexture (main_renderer, dataSurface->format->format, 
                SDL_TEXTUREACCESS_TARGET, dataSurface->w, dataSurface->h);;

            SDL_SetTextureBlendMode (new_level, SDL_BLENDMODE_BLEND);

            // reset filter mode for the temp texture
            SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "0");

            u8 r, g, b, a;
            SDL_Texture *temp = SDL_CreateTextureFromSurface (main_renderer, dataSurface);
            SDL_SetTextureBlendMode (temp, SDL_BLENDMODE_NONE);
            SDL_SetRenderTarget (main_renderer, new_level);

            SDL_GetRenderDrawColor (main_renderer, &r, &g, &b, &a);
            SDL_SetRenderDrawColor (main_renderer, 0, 0, 0, 0);
            SDL_RenderClear (main_renderer);
            SDL_SetRenderDrawColor (main_renderer, r, g, b, a);

            SDL_RenderCopy (main_renderer, temp, NULL, NULL);
            SDL_SetRenderTarget (main_renderer, NULL);

            SDL_DestroyTexture (temp);

            SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, old_filter_mode);
        }

        if (!glyph_set_cache_level (font, cacheLevel, new_level)) return 0;   // success
        
        #ifdef DEV
        logMsg (stderr, ERROR, NO_TYPE, "Font cache ran out of packing space "
        "and could not add another cache level!");
        #else
        SDL_DestroyTexture (new_level);
        #endif
    }

    return 1;   // error

}

/*** FONT ***/

static void ui_font_init (Font *font) {

    font->ttf_source = NULL;
    font->owns_ttf_source = 0;

    font->filter = FILTER_NEAREST;

    font->default_color.r = font->default_color.g = font->default_color.b = 0;
    font->default_color.a = 255;

    font->height = 0;
    font->maxWidth = 0;
    font->baseline = 0;
    font->ascent = 0;
    font->descent = 0;

    font->lineSpacing = 0;
    font->letterSpacing = 0;

    font->last_glyph.rect.x = font->last_glyph.rect.y = 1;
    font->last_glyph.rect.w = font->last_glyph.rect.h = 0;
    font->last_glyph.cacheLevel = 0;

    if (font->glyphs) font_map_destroy (font->glyphs);
    font->glyphs = font_map_create (DEFAULT_FONT_MAP_N_BUCKETS);

    font->glyph_cache_size = 3;
    font->glyph_cache_count = 0;
    font->glyph_cache = (FontImage **) calloc (font->glyph_cache_size, sizeof (FontImage *));

    font->loading_string = font_get_string_ASCII ();

}

Font *ui_font_create (void) {

    Font *font = (Font *) malloc (sizeof (Font));
    if (font) {
        memset (font, 0, sizeof (Font));
        ui_font_init (font);
    } 

    return font;

}

void ui_font_destroy (Font *font) {

    if (font) {
        if (font->owns_ttf_source) TTF_CloseFont (font->ttf_source);

        font_map_destroy (font->glyphs);

        // delete glyph cache
        if (font->glyph_cache) {
            for (u32 i = 0; i < font->glyph_cache_count; i++) 
                if (font->glyph_cache[i])
                    SDL_DestroyTexture (font->glyph_cache[i]);

            free (font->glyph_cache);
        }

        if (font->loading_string) free (font->loading_string);

        free (font);
    }

}

static u8 ui_font_load_from_ttf (Font *font, TTF_Font *ttf, RGBA_Color color) {

    // TODO: do we need to clear up the font?
    // FC_ClearFont(font);

    SDL_RendererInfo info;
    SDL_GetRendererInfo (main_renderer, &info);
    has_render_target_support = (info.flags & SDL_RENDERER_TARGETTEXTURE);

    font->ttf_source = ttf;

    font->height = TTF_FontHeight(ttf);
    font->ascent = TTF_FontAscent(ttf);
    font->descent = -TTF_FontDescent(ttf);
    
    // Some bug for certain fonts can result in an incorrect height.
    if (font->height < font->ascent - font->descent)
        font->height = font->ascent - font->descent;

    font->baseline = font->height - font->descent;

    font->default_color = color;

    SDL_Surface *glyph_surf = NULL;
    char buff[5];
    memset (buff, 0, 5);
    const char *buff_ptr = buff;
    const char *source_string = font->loading_string;
    u8 packed = 0;

    u32 w = font->height * 12;
    u32 h = font->height * 12;

    SDL_Surface *surfaces[FONT_LOAD_MAX_SURFACES];
    int num_surfaces = 1;
    surfaces[0] = font_create_surface (w, h);

    font->last_glyph.rect.x = font->last_glyph.rect.y = 1;
    font->last_glyph.rect.w = 0;
    font->last_glyph.rect.h = font->height;

    for (; *source_string != '\0'; source_string = u8_next (source_string)) {
        if (!u8_charcpy (buff, source_string, 5)) continue;

        glyph_surf = TTF_RenderUTF8_Blended (ttf, buff, RGBA_WHITE);
        if (!glyph_surf) continue;

        packed = (glyph_data_pack (font, get_code_point_from_UTF8 (&buff_ptr, 0),
            glyph_surf->w, surfaces[num_surfaces - 1]->w, surfaces[num_surfaces-1]->h) != NULL);
        if (!packed) {
            int i = num_surfaces - 1;
            if (num_surfaces >= FONT_LOAD_MAX_SURFACES) {
                // FIXME: better handle this error - also set a retval
                #ifdef DEV
                logMsg (stderr, ERROR, NO_TYPE, "Font cache error - Could not create"
                "enough cache surfaces to fit all of the loading string!");
                #else
                logMsg (stderr, ERROR, NO_TYPE, "Failed to create font cache!");
                #endif
                SDL_FreeSurface (glyph_surf);
                break;
            }

            // upload the current surface to the glyph cache
            glyph_upload_cache (font, i, surfaces[i]);
            SDL_FreeSurface (surfaces[i]);
            font->last_glyph.cacheLevel = num_surfaces;

            surfaces[num_surfaces] = font_create_surface (w, h);
            num_surfaces++;
        }

        if (packed || glyph_data_pack (font, get_code_point_from_UTF8 (&buff_ptr, 0),
            glyph_surf->w, surfaces[num_surfaces - 1]->w, surfaces[num_surfaces - 1]->h) != NULL) {
            SDL_SetSurfaceBlendMode (glyph_surf, SDL_BLENDMODE_NONE);
            SDL_Rect srcRect = { 0, 0, glyph_surf->w, glyph_surf->h };
            SDL_Rect destRect = font->last_glyph.rect;
            SDL_BlitSurface (glyph_surf, &srcRect, surfaces[num_surfaces - 1], &destRect);
        }

        SDL_FreeSurface (glyph_surf);
    }

    int n = num_surfaces - 1;
    glyph_upload_cache (font, n, surfaces[n]);
    SDL_FreeSurface (surfaces[n]);
    SDL_SetTextureBlendMode (font->glyph_cache[n], SDL_BLENDMODE_BLEND);

    return 0;

}

static u8 ui_font_load_rw (Font *font, SDL_RWops *file_rwops_ttf,
    u8 own_rwops, u32 pointSize, RGBA_Color color, int style) {

    u8 retval, outline;

    TTF_Font *ttf = TTF_OpenFontRW (file_rwops_ttf, own_rwops, pointSize);
    if (!ttf) {
        logMsg (stderr, ERROR, NO_TYPE, "Unable to load ttf!");
        return 1;
    }

    outline = (style & TTF_STYLE_OUTLINE);
    if (outline) {
        style &= ~TTF_STYLE_OUTLINE;
        TTF_SetFontOutline (ttf, 1);
    }

    TTF_SetFontStyle (ttf, style);

    retval = ui_font_load_from_ttf (font, ttf, color);

    font->owns_ttf_source = own_rwops;
    if (!own_rwops) {
        TTF_CloseFont (font->ttf_source);
        font->ttf_source = NULL;
    }

    return retval;

}

u8 ui_font_load (Font *font, const char *filename, u32 pointSize, 
    RGBA_Color color, int style) {

    if (font) {
        SDL_RWops *rwops = SDL_RWFromFile (filename, "rb");
        if (rwops) return ui_font_load_rw (font, rwops, 1, pointSize, color, style);

        else logMsg (stderr, ERROR, NO_TYPE, 
            createString ("Unable to open file for reading: %s", filename));
    }

    return 1;

}

#pragma endregion

#pragma region TEXTBOX

// TODO: handle input fields like in blackrock

void ui_textBox_set_text (TextBox *textBox, const char *newText) {

    if (textBox) {
        if (textBox->text) free (textBox->text);
        if (textBox->pswd) free (textBox->pswd);

        if (newText) {
            if (textBox->ispassword) {
                textBox->pswd = (char *) calloc (strlen (newText) + 1, sizeof (char));
                strcpy (textBox->pswd, newText);
                u32 len = strlen (newText);
                for (u8 i = 0; i < len; i++) textBox->text[i] = '*';
            }

            else {
                textBox->text = (char *) calloc (strlen (newText) + 1, sizeof (char));
                strcpy (textBox->text, newText);
                textBox->pswd = NULL;
            }
        }

        else textBox->text = textBox->pswd = NULL;

        if (!textBox->isVolatile) {
            if (textBox->texture) SDL_DestroyTexture (textBox->texture);
            SDL_Surface *surface = TTF_RenderText_Solid (textBox->font->ttf_source, newText, textBox->textColor);
            textBox->texture = SDL_CreateTextureFromSurface (main_renderer, surface);
       
            textBox->bgrect.w = surface->w;
            textBox->bgrect.h = surface->h;

            SDL_FreeSurface(surface);
        }

    }

}

void ui_textBox_set_text_color (TextBox *textBox, RGBA_Color newColor) {

    if (textBox) {
        textBox->textColor = newColor;

        if (!textBox->isVolatile) {
            SDL_Surface *surface = TTF_RenderText_Solid (textBox->font->ttf_source, 
                textBox->text, textBox->textColor);
            textBox->texture = SDL_CreateTextureFromSurface (main_renderer, surface);
    
            textBox->bgrect.w = surface->w;
            textBox->bgrect.h = surface->h;

            SDL_FreeSurface(surface);
        }
    }

}

void ui_textBox_set_bg_color (TextBox *textBox, RGBA_Color newColor) {

    if (textBox) textBox->bgcolor = newColor;

}

static TextBox *ui_textBox_new (u32 x, u32 y, RGBA_Color bgColor,
    const char *text, RGBA_Color textColor, Font *font, bool isPassword) {

    UIElement *ui_element = ui_element_new (UI_TEXTBOX);
    if (ui_element) {
        TextBox *textBox = (TextBox *) malloc (sizeof (TextBox));
        if (textBox) {
            textBox->texture = NULL;
            textBox->font = font ? font : mainFont;

            textBox->bgrect.x = x;
            textBox->bgrect.y = y;
            textBox->bgrect.w = textBox->bgrect.h = 0;
            textBox->bgcolor = bgColor;

            textBox->textColor = textColor;

            textBox->ispassword = isPassword;

            if (text) {
                if (isPassword) {
                    textBox->pswd = (char *) calloc (strlen (text) + 1, sizeof (char));
                    strcpy (textBox->pswd, text);
                    u32 len = strlen (text);
                    for (u8 i = 0; i < len; i++) textBox->text[i] = '*';
                }

                else {
                    textBox->text = (char *) calloc (strlen (text) + 1, sizeof (char));
                    strcpy (textBox->text, text);
                    textBox->pswd = NULL;
                }
            }

            else textBox->text = textBox->pswd = NULL;

            ui_element->element = textBox;

            return textBox;
        }
    }

    return NULL;

}

// TODO: generate a smoother text like with the volatile ones
// TODO: handle password logic
TextBox *ui_textBox_create_static (u32 x, u32 y, RGBA_Color bgColor,
    const char *text, RGBA_Color textColor, Font *font, bool isPassword) {

    TextBox *textBox = ui_textBox_new (x, y, bgColor, text, textColor, font, isPassword);
    if (textBox) {
        SDL_Surface *surface = TTF_RenderText_Solid (textBox->font->ttf_source, text, textColor);
        textBox->texture = SDL_CreateTextureFromSurface (main_renderer, surface);

        textBox->bgrect.x = x;
        textBox->bgrect.y = y;        
        textBox->bgrect.w = surface->w;
        textBox->bgrect.h = surface->h;

        SDL_FreeSurface(surface);

        textBox->isVolatile = false;
    }

    return textBox;

}

TextBox *ui_textBox_create_volatile (u32 x, u32 y, RGBA_Color bgColor,
    const char *text, RGBA_Color textColor, Font *font, bool isPassword) {

    TextBox *textBox = ui_textBox_new (x, y, bgColor, text, textColor, font, isPassword);
    if (textBox) {
        textBox->texture = NULL;
        textBox->isVolatile = true;
    }

    return textBox;

}

void ui_textBox_destroy (TextBox *textbox) {

    if (textbox) {
        textbox->font = NULL;
        if (textbox->texture) SDL_DestroyTexture (textbox->texture);
        if (textbox->text) free (textbox->text);
        if (textbox->pswd) free (textbox->pswd);

        free (textbox);
    }

}

// FIXME: move this form here!
// FC_Default_RenderCallback
UIRect ui_rect_render (SDL_Texture *srcTexture, UIRect *srcRect, u32 x, u32 y) {

    UIRect retval;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    UIRect r = *srcRect;
    UIRect dr = { x, y, r.w, r.h };
    SDL_RenderCopyEx (main_renderer, srcTexture, &r, &dr, 0, NULL, flip);

    retval.x = x;
    retval.y = y;
    retval.w = srcRect->w;
    retval.h = srcRect->h;

    return retval;

}

// FIXME: handle text color change
// FIXME: handle password logic
// TODO: maybe add scale
// TODO: we need a better way for selecting font sizes!!
// this was FC_RenderLeft...
void ui_textbox_draw (TextBox *textbox) {

    if (textbox) {
        const char *c = textbox->text;

        // TODO: do we want this inside the textbox?
        UIRect srcRect, destRect, 
            dirtyRect = ui_rect_create (textbox->bgrect.x, textbox->bgrect.y, 0, 0);

        GlyphData glyph;
        u32 codepoint;

        u32 destX = textbox->bgrect.x;
        u32 destY = textbox->bgrect.y;
        float destH, destLineSpacing, destLetterSpacing;

        // TODO: add scale here!
        destH = textbox->font->height;
        destLineSpacing = textbox->font->lineSpacing;
        destLetterSpacing = textbox->font->letterSpacing;

        int newLineX = textbox->bgrect.x;

        for (; *c != '\0'; c++) {
            if (*c == '\n') {
                destX = newLineX;
                destY += destH + destLineSpacing;
                continue;
            }

            codepoint = get_code_point_from_UTF8 (&c, 1);
            if (glyph_get_data (textbox->font, &glyph, codepoint)) {
                // FIXME: handle bas caharcters
            }

            if (codepoint == ' ') {
                destX += glyph.rect.w + destLetterSpacing;
                continue;
            }

            srcRect = glyph.rect;

            destRect = ui_rect_render (glyph_get_cache_level (textbox->font, glyph.cacheLevel),
                &srcRect, destX, destY);

            if (dirtyRect.w == 0 || dirtyRect.h == 0) dirtyRect = destRect;
            else dirtyRect = ui_rect_union (dirtyRect, destRect);

            destX += glyph.rect.w + destLetterSpacing;
        }

        // FIXME:
        // return dirtyRect;
    }

}

#pragma endregion

#pragma region CURSOR

SDL_Cursor *sysCursor; 
Sprite *cursorImg = NULL;

// FIXME: set the correct sprite
static u8 ui_cursor_init (void) {

    u8 retval = 1;

    // hide the system cursor inside the window
    i32 cursorData[2] = { 0, 0 };
    sysCursor = SDL_CreateCursor ((Uint8 *) cursorData, (Uint8 *) cursorData, 8, 8, 4, 4);
    SDL_SetCursor (sysCursor);

    cursorImg = sprite_load ("./assets/artwork/mapTile_087.png", main_renderer);
    if (cursorImg) retval = 0;
    else {
        #ifdef BLACK_DEBUG
        logMsg (stderr, ERROR, NO_TYPE, "Failed to load cursor imgs!");
        #endif
    }

    return retval;

}

void ui_cursor_draw (void) {

    cursorImg->dest_rect.x = mousePos.x;
    cursorImg->dest_rect.y = mousePos.y;

    SDL_RenderCopyEx (main_renderer, cursorImg->texture, &cursorImg->src_rect, &cursorImg->dest_rect, 
        0, 0, NO_FLIP);   

}

#pragma endregion

/*** PUBLIC UI FUNCS ***/

void ui_render (void) {

    TextBox *textBox = NULL;
    for (u32 i = 0; i < curr_max_ui_elements; i++) {
        switch (ui_elements[i]->type) {
            case UI_TEXTBOX: 
                textBox = (TextBox *) ui_elements[i]->element;
                if (textBox->isVolatile) ui_textbox_draw (textBox);
                else SDL_RenderCopy (main_renderer, textBox->texture, NULL, &textBox->bgrect);

                break;
            case UI_BUTTON: break;

            default: break;
        }
    }

    // render the cursor on top of everything
    ui_cursor_draw ();

}

// init main ui elements
u8 ui_init (void) {

    int errors = 0;

    // init and load fonts
    TTF_Init ();
    mainFont = ui_font_create ();
    if (!mainFont) {
        #ifdef DEV
        logMsg (stderr, ERROR, NO_TYPE, "Failed to allocate space for new font!");
        #endif
        return 1;
    }

    if (ui_font_load (mainFont, createString ("%s%s", ASSETS_PATH, mainFontPath), 
        DEFAULT_FONT_SIZE, RGBA_WHITE, TTF_STYLE_NORMAL)) {
        #ifdef DEV
        logMsg (stderr, ERROR, NO_TYPE, "Failed to load font and create font cache!");
        #endif
        return 1;
    }

    // init ui elements
    errors = ui_elements_init ();

    // init cursor
    errors = ui_cursor_init ();

    return errors;

}

// destroy main ui elements
u8 ui_destroy (void) {

    // ui elements
    if (ui_elements) {
        for (u32 i = 0; i < curr_max_ui_elements; i++)
            ui_element_delete (ui_elements[i]);

        free (ui_elements);
    }
    
    // fonts
    ui_font_destroy (mainFont);
    TTF_Quit ();

    SDL_FreeCursor (sysCursor);

    #ifdef DEV
    logMsg (stdout, SUCCESS, GAME, "Done cleaning up the UI!");
    #endif

    return 0;

}