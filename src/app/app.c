#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "types/myTypes.h"
#include "myos.h"

#ifdef LINUX
#include <pthread.h>
#endif

#include "cengine/renderer.h"
#include "cengine/textures.h"
#include "cengine/animation.h"

#include "collections/dllist.h"

// #include "utils/objectPool.h"
#include "utils/myUtils.h"
#include "utils/log.h"

#pragma region Components 

static Transform *transform_new (u32 objectID) {

    Transform *new_transform = (Transform *) malloc (sizeof (Transform));
    if (new_transform) {
        new_transform->goID = objectID;
        new_transform->position.x = 0;
        new_transform->position.y = 0;
    }

    return new_transform;

}

static void transform_destroy (Transform *transform) { if (transform) free (transform); }

static Graphics *graphics_new (u32 objectID) {

    Graphics *new_graphics = (Graphics *) malloc (sizeof (Graphics));
    if (new_graphics) {
        new_graphics->goID = objectID;
        new_graphics->sprite = NULL;
        new_graphics->spriteSheet = NULL;
        new_graphics->refSprite = false;
        new_graphics->multipleSprites = false;
        new_graphics->x_sprite_offset = 0;
        new_graphics->y_sprite_offset = 0;
        new_graphics->layer = UNSET_LAYER;
        new_graphics->flip = NO_FLIP;
    }

    return new_graphics;

}

static void graphics_destroy (Graphics *graphics) {

    if (graphics) {
        if (graphics->refSprite) {
            graphics->sprite = NULL;
            graphics->spriteSheet = NULL;
        }

        else {
            if (graphics->sprite) sprite_destroy (graphics->sprite);
            if (graphics->spriteSheet) sprite_sheet_destroy (graphics->spriteSheet);
        }

        free (graphics);
    }

}

void graphics_set_sprite (Graphics *graphics, const char *filename) {

    if (graphics && filename) {
        graphics->sprite = sprite_load (filename, main_renderer);
        graphics->spriteSheet = NULL;
        graphics->multipleSprites = false;
    }

}

void graphics_set_sprite_sheet (Graphics *graphics, const char *filename) {

    if (graphics && filename) {
        graphics->sprite = NULL;
        graphics->spriteSheet = sprite_sheet_load (filename, main_renderer);
        graphics->multipleSprites = true;
    }

}

void graphics_ref_sprite (Graphics *graphics, Sprite *sprite) {

    if (graphics && sprite) {
        graphics->sprite = sprite;
        graphics->spriteSheet = NULL;
        graphics->multipleSprites = false;
        graphics->refSprite = true;
    }

}

void graphics_ref_sprite_sheet (Graphics *graphics, SpriteSheet *spriteSheet) {

    if (graphics && spriteSheet) {
        graphics->sprite = NULL;
        graphics->spriteSheet = spriteSheet;
        graphics->multipleSprites = true;
        graphics->refSprite = true;
    }

}

#pragma endregion

#pragma region App Objects

static GameObject **gameObjects;
static u32 max_gos;
static u32 curr_max_objs;
static u32 new_go_id;

static bool game_objects_realloc (void) {

    u32 new_max_gos = curr_max_objs * 2;

    gameObjects = realloc (gameObjects, new_max_gos * sizeof (GameObject *));

    if (gameObjects) {
        max_gos = new_max_gos;
        return true;
    }

    return false;

}

// init our game objects array
static u8 game_objects_init_all (void) {

    gameObjects = (GameObject **) calloc (DEFAULT_MAX_GOS, sizeof (GameObject *));
    if (gameObjects) {
        for (u32 i = 0; i < DEFAULT_MAX_GOS; i++) gameObjects[i] = NULL;

        max_gos = DEFAULT_MAX_GOS;
        curr_max_objs = 0;
        new_go_id = 0;

        return 0;
    }

    return 1;

}

static i32 game_object_get_free_spot (void) {

    for (u32 i = 0; i < curr_max_objs; i++)
        if (gameObjects[i]->id == -1)
            return i;

    return -1;

}

static void game_object_init (GameObject *go, u32 id, const char *name, const char *tag) {

    if (go) {
        go->id = id;

        if (name) {
            go->name = (char *) calloc (strlen (name) + 1, sizeof (char));
            strcpy (go->name, name);
        }

        else go->name = NULL;

        if (tag) {
            go->tag = (char *) calloc (strlen (tag) + 1, sizeof (char));
            strcpy (go->tag, tag);
        }

        else go->tag = NULL;

        for (u8 i = 0; i < COMP_COUNT; i++) go->components[i] = NULL;

        go->children = NULL;

        go->update = NULL;
    }

}

// game object constructor
GameObject *game_object_new (const char *name, const char *tag) {

    GameObject *new_go = NULL;

    // first check if we have a reusable go in the array
    i32 spot = game_object_get_free_spot ();

    if (spot >= 0) {
        new_go = gameObjects[spot];
        game_object_init (new_go, spot, name, tag);
    } 

    else {
        if (new_go_id >= max_gos) game_objects_realloc ();

        new_go = (GameObject *) malloc (sizeof (GameObject));
        if (new_go) {
            game_object_init (new_go, new_go_id, name, tag);
            gameObjects[new_go->id] = new_go;
            new_go_id++;
            curr_max_objs++;
        }
    } 

    return new_go;

}

void game_object_add_child (GameObject *parent, GameObject *child) {

    if (parent && child) {
        if (!parent->children) parent->children = llist_init (NULL);
        llist_insert_next (parent->children, llist_end (parent->children), child);
    }

}

GameObject *game_object_remove_child (GameObject *parent, GameObject *child) {

    if (parent && child) {
        if (parent->children) { 
            GameObject *go = NULL;
            ListNode *n = llist_start (parent->children);
            while (n != NULL) { 
                go = (GameObject *) n->data;
                if (go->id == child->id) break;
                n = n->next;
            }

            if (n) return (GameObject *) llist_remove (parent->children, n);
        }
    }

    return NULL;

}

GameObject *game_object_get_by_id (u32 id) {

    if (id <= curr_max_objs) return gameObjects[id];

}

// mark as inactive or reusable the game object
void game_object_destroy (GameObject *go) {

    if (go) {
        go->id = -1;
        go->update = NULL;

        if (go->name) free (go->name);
        if (go->tag) free (go->tag);

        if (go->children) free (go->children);

        // individually destroy each component
        transform_destroy ((Transform *) go->components[TRANSFORM_COMP]);
        graphics_destroy ((Graphics *) go->components[GRAPHICS_COMP]);
        animator_destroy ((Animator *) go->components[ANIMATOR_COMP]);
    }

}

void game_object_destroy_ref (void *data) { game_object_destroy ((GameObject *) data); } 

static void game_object_delete (GameObject *go) {

    if (go) {
        go->update = NULL;

        // individually destroy each component
        transform_destroy ((Transform *) go->components[TRANSFORM_COMP]);
        graphics_destroy ((Graphics *) go->components[GRAPHICS_COMP]);
        animator_destroy ((Animator *) go->components[ANIMATOR_COMP]);

        if (go->name) free (go->name);
        if (go->tag) free (go->tag);

        free (go);
    }

}

void *game_object_add_component (GameObject *go, GameComponent component) {

    void *retval = NULL;

    if (go) {
        switch (component) {
            case TRANSFORM_COMP: 
                retval = go->components[component] = transform_new (go->id); 
                break;
            case GRAPHICS_COMP: 
                retval = go->components[component] = graphics_new (go->id); 
                break;
            case ANIMATOR_COMP: 
                retval = go->components[component] = animator_new (go->id); 
                break;

            default: break;
        }
    }

    return retval;

}

void *game_object_get_component (GameObject *go, GameComponent component) {

    if (go) return go->components[component];

}

void game_object_remove_component (GameObject *go, GameComponent component) {

    if (go) {
        switch (component) {
            case TRANSFORM_COMP: 
                transform_destroy (go->components[component]); 
                break;
            case GRAPHICS_COMP: 
                graphics_destroy (go->components[component]);
                break;
            case ANIMATOR_COMP: 
                animator_destroy (go->components[component]);
                break;

            default: break;
        }
    }

}

#pragma endregion

/*** GAME ***/

#pragma region GAME 

// FIXME: how do we want to manage the score?
// Score
Score *playerScore = NULL;

// TODO:
static u8 load_game_data (void) {

    // connect to items db
    // items_init ();
    // connect to enemies db and load enemy data
    if (!enemies_connect_db ()) 
        return 0;

    return 0;

}

// FIXME:
static void game_update (void);

// TODO: this inits the game to the tavern/village
// TODO: this can be a good place to check if we have a save file of a map and load that from disk

// FIXME: move this from here
static u8 game_init (void) {

    game_objects_init_all ();

    if (!load_game_data ()) {
        // init world
        world = world_create ();

        // init map
        // FIXME: from where do we get this values?
        // single player -> random values
        // multiplayer -> we get it from the server
        world->game_map = map_create (100, 40);
        world->game_map->dungeon = dungeon_generate (world->game_map, 
            world->game_map->width, world->game_map->heigth, 100, .45);

        // spawn enemies
        // enemies_spawn_all (world, random_int_in_range (5, 10));

        // spawn items

        // init player(s)
        llist_insert_next (world->players, llist_start (world->players), player_init ());

        // spawn players
        GameObject *go = NULL;
        Transform *transform = NULL;
        for (ListNode *n = llist_start (world->players); n != NULL; n = n->next) {
            go = (GameObject *) n->data;
            transform = (Transform *) game_object_get_component (go, TRANSFORM_COMP);
            Coord spawnPoint = map_get_free_spot (world->game_map);
            // FIXME: fix wolrd scale!!!
            transform->position.x = spawnPoint.x * 64;
            transform->position.y = spawnPoint.y * 64;
        }

        // update camera
        GameObject *main_player = (GameObject *) (llist_start (world->players)->data );
        transform = (Transform *) game_object_get_component (main_player, TRANSFORM_COMP);
        world->game_camera->center = transform->position;

        // init score

        // FIXME: we are ready to start updating the game
        game_state->update = game_update;
        // app_manager->currState->update = game_update;

        return 0;
    } 

    return 1;

}

#pragma endregion

/*** App State ***/

#pragma region App State

AppState *app_state = NULL;

static void app_onEnter (void) {}

static void app_onExit (void) {}

static void app_update (void) {

    // update every game object
    for (u32 i = 0; i < curr_max_objs; i++) {
        if (gameObjects[i]->id != -1) {
            if (gameObjects[i]->update)
                gameObjects[i]->update (NULL);
        }
    }

    // FIXME:
    // update the camera
    // camera_update (world->game_camera);
    
}

// FIXME: we need to implement occlusion culling!
static void app_render (void) {

    Transform *transform = NULL;
    Graphics *graphics = NULL;
    for (u32 i = 0; i < curr_max_objs; i++) {
        transform = (Transform *) game_object_get_component (gameObjects[i], TRANSFORM_COMP);
        graphics = (Graphics *) game_object_get_component (gameObjects[i], GRAPHICS_COMP);
        if (transform && graphics) {
            if (graphics->multipleSprites)
                texture_draw_frame (world->game_camera, graphics->spriteSheet, 
                transform->position.x, transform->position.y, 
                graphics->x_sprite_offset, graphics->y_sprite_offset,
                graphics->flip);
            
            else
                texture_draw (world->game_camera, graphics->sprite, 
                transform->position.x, transform->position.y, 
                graphics->flip);
        }
    }

}

void app_clean_up (void) {

    // clean up game objects
    for (u32 i = 0; i < curr_max_objs; i++) 
        if (gameObjects[i])
            game_object_delete (gameObjects[i]);

    free (gameObjects);

    #ifdef DEV
    logMsg (stdout, SUCCESS, GAME, "Done cleaning up app data!");
    #endif
    
}

AppState *app_state_new (void) {

    AppState *new_game_state = (AppState *) malloc (sizeof (AppState));
    if (new_game_state) {
        new_game_state->state = IN_GAME;

        // new_game_state->update = game_update;
        new_game_state->update = NULL;
        new_game_state->render = game_render;

        new_game_state->onEnter = game_onEnter;
        new_game_state->onExit = game_onExit;
    }

}

#pragma endregion

/*** GAME MANAGER ***/

#pragma region GAME MANAGER

GameManager *app_manager = NULL;

AppState *menu_state = NULL;
AppState *game_over_state =  NULL;

GameManager *app_manager_new (AppState *initState) {

    GameManager *new_app_manager = (GameManager *) malloc (sizeof (GameManager));
    if (new_app_manager) {
        new_app_manager->currState = initState;
        if (new_app_manager->currState->onEnter)
            new_app_manager->currState->onEnter ();
    } 

    return new_app_manager;

}

State game_state_get_current (void) { return app_manager->currState->state; }

void game_state_change_state (AppState *newState) { 
    
    if (app_manager->currState->onExit)
        app_manager->currState->onExit ();

    app_manager->currState = newState; 
    if (app_manager->currState->onEnter)
        app_manager->currState->onEnter ();
    
}

#pragma endregion