#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "types/myTypes.h"

/*** COMMON HEX COLORS ***/

#define HEX_NO_COLOR        0x00000000
#define HEX_WHITE           0xFFFFFFFF
#define HEX_BLACK           0x000000FF

#define HEX_FULL_GREEN      0x00FF00FF
#define HEX_FULL_RED        0xFF0000FF

#define HEX_YELLOW          0xFFD32AFF
#define HEX_SAPPHIRE        0x1E3799FF

#define HEX_SILVER          0xBDC3C7FF

/*** COMMON RGBA COLORS ***/

typedef SDL_Color RGBA_Color;

extern RGBA_Color RGBA_NO_COLOR;
extern RGBA_Color RGBA_WHITE;
extern RGBA_Color RGBA_BLACK;

/*** UI ELEMENTS ***/

typedef enum UIElementType {

    UI_TEXTBOX,
    UI_BUTTON,

} UIElementType;

#define DEFAULT_MAX_UI_ELEMENTS     10

typedef struct UIElement {

    i32 id;
    UIElementType type;
    void *element;

} UIElement;

typedef SDL_Rect UIRect;

extern RGBA_Color ui_rgba_color_create (u8 r, u8 g, u8 b, u8 a);

/*** FONTS/TEXT ***/

#pragma region FONT-TEXT

#include <SDL2/SDL_ttf.h>

typedef SDL_Texture FontImage;

#define DEFAULT_FONT_SIZE           50

#define TTF_STYLE_OUTLINE	        16
#define FONT_LOAD_MAX_SURFACES      10

typedef enum FilterEnum {

    FILTER_NEAREST,
    FILTER_LINEAR,

} FilterEnum;

typedef struct GlyphData {

    UIRect rect;
    int cacheLevel;

} GlyphData;

typedef struct FontMapNode {

    u32 key;
    GlyphData value;
    struct FontMapNode *next;

} FontMapNode;

#define DEFAULT_FONT_MAP_N_BUCKETS      300

typedef struct FontMap {

    i32 n_buckets;
    FontMapNode **buckets;

} FontMap;

typedef struct Font {

    TTF_Font *ttf_source;
    u8 owns_ttf_source;

    FilterEnum filter;

    RGBA_Color default_color;
    u16 height;
    u16 maxWidth;
    u16 baseline;
    i32 ascent;
    i32 descent;

    i32 lineSpacing;
    i32 letterSpacing;

    FontMap *glyphs;
    GlyphData last_glyph;
    i32 glyph_cache_size;
    i32 glyph_cache_count;
    FontImage **glyph_cache;
    char *loading_string;

} Font;

extern Font *ui_font_create (void);
extern void ui_font_destroy (Font *font);

/*** TEXTBOX ***/

// TODO: handle input logic
typedef struct TextBox {

    Font *font;
    SDL_Texture *texture;
    UIRect bgrect;
    RGBA_Color bgcolor;

    RGBA_Color textColor;
    char *text;
    bool ispassword;
    char *pswd;

    bool isVolatile;

} TextBox;

extern TextBox *ui_textBox_create_static (u32 x, u32 y, RGBA_Color bgColor,
    const char *text, RGBA_Color textColor, Font *font, bool isPassword);
extern TextBox *ui_textBox_create_volatile (u32 x, u32 y, RGBA_Color bgColor,
    const char *text, RGBA_Color textColor, Font *font, bool isPassword);

extern void ui_textBox_set_text (TextBox *textBox, const char *newText);
extern void ui_textBox_set_text_color (TextBox *textBox, RGBA_Color newColor);
extern void ui_textBox_set_bg_color (TextBox *textBox, RGBA_Color newColor);

extern void ui_textBox_destroy (TextBox *textbox);

#pragma endregion

/*** PUBLIC UI FUNCS ***/

extern void ui_render (void);

extern u8 ui_init (void);
extern u8 ui_destroy (void);

#endif