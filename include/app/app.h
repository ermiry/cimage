#ifndef GAME_H_
#define GAME_H_

#include "myos.h"
#include "types/myTypes.h"

#include "types/vector2D.h"

#include "cengine/sprites.h"

#include "collections/dllist.h"

/*** App Manager ***/

typedef enum State {

    LOGO = 0,
    PRELOADING = 1, 
    MAIN_MENU = 2,
    IN_GAME = 3,
    GAME_OVER = 4,

    LOADING = 10,

} State;

typedef struct AppState {

    State state;

    void (*update)(void);
    void (*render)(void);

    void (*onEnter)(void);
    void (*onExit)(void);

} AppState;

typedef struct GameManager {

    AppState *currState;

} GameManager;

extern GameManager *app_manager;

extern GameManager *app_manager_new (AppState *initState);
extern State app_state_get_current (void);
extern void app_state_change_state (AppState *newState);

extern void app_clean_up (void);

// TODO: maybe add a function to register when we change to a state,
// so that we can load many things with like an array of events?

/*** App State ***/

extern AppState *app_state;

extern AppState *app_state_new (void);

/*** GAME OBJECTS ***/

#define DEFAULT_MAX_GOS     200

#define COMP_COUNT      7

typedef struct GameObject {
    
    i32 id;

    char *name;
    char *tag;
    void *components[COMP_COUNT];

    DoubleList *children;
    void (*update)(void *data);

} GameObject;

extern GameObject *game_object_new (const char *name, const char *tag);
extern void game_object_destroy (GameObject *go);
extern void game_object_destroy_ref (void *data);

extern void game_object_add_child (GameObject *parent, GameObject *child);
extern GameObject *game_object_remove_child (GameObject *parent, GameObject *child);

extern GameObject *game_object_get_by_id (u32 id);

/*** COMPONENTS ***/

typedef enum GameComponent {

    TRANSFORM_COMP = 0,
    GRAPHICS_COMP,
    ANIMATOR_COMP,

} GameComponent;

extern void *game_object_add_component (GameObject *go, GameComponent component);
extern void *game_object_get_component (GameObject *go, GameComponent component);
extern void game_object_remove_component (GameObject *go, GameComponent component);

typedef struct Transform {

    u32 goID;
    Vector2D position;

} Transform;

typedef struct Position {

    u32 objectId;
    u8 x, y;
    u8 layer;   

} Position;

typedef enum Layer {

    UNSET_LAYER = 0,
    GROUND_LAYER = 1,
    LOWER_LAYER = 2,
    MID_LAYER = 3,
    TOP_LAYER = 4,

} Layer;

typedef enum Flip {

    NO_FLIP = 0x00000000,
    FLIP_HORIZONTAL = 0x00000001,
    FLIP_VERTICAL = 0x00000002

} Flip;

typedef struct Graphics {

    u32 goID;

    Sprite *sprite;
    SpriteSheet *spriteSheet;
    bool refSprite;

    u32 x_sprite_offset, y_sprite_offset;
    bool multipleSprites;
    Layer layer; 
    Flip flip;
    bool hasBeenSeen;
    bool visibleOutsideFov;

} Graphics;

extern void graphics_set_sprite (Graphics *graphics, const char *filename);
extern void graphics_set_sprite_sheet (Graphics *graphics, const char *filename);

extern void graphics_ref_sprite (Graphics *graphics, Sprite *sprite);
extern void graphics_ref_sprite_sheet (Graphics *graphics, SpriteSheet *spriteSheet);

/*** EVENTS ***/

typedef void (*EventListener)(void *);

typedef struct Event {

    u32 objectId;
    EventListener callback;

} Event;

#endif