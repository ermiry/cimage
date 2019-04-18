#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "blackrock.h"

#include "myos.h"

#include "game/game.h"

#include "engine/mythread.h"
#include "engine/timer.h"
#include "engine/sprites.h"
#include "engine/animation.h"

#include "collections/llist.h"

#ifdef DEV
    #include "utils/log.h"
#endif

static bool anim_init = false;

/*** ANIMATION ***/

Animation *animation_create (u8 n_frames, ...) {

    va_list valist;
    va_start (valist, n_frames);

    Animation *animation = (Animation *) malloc (sizeof (Animation));
    if (animation) {
        animation->speed = DEFAULT_ANIM_SPEED;
        animation->n_frames = n_frames;
        animation->frames = (IndividualSprite **) calloc (n_frames, sizeof (IndividualSprite *));

        for (u8 i = 0; i < n_frames; i++)
            animation->frames[i] = va_arg (valist, IndividualSprite *);

        va_end (valist);
    }

    return animation;

}

void animation_destroy (Animation *animation) {

    if (animation) {
        if (animation->frames) free (animation->frames);

        free (animation);
    }

}

void animation_set_speed (Animation *animation, u32 speed) {

    if (animation) animation->speed = speed;

}

/*** ANIMATOR ***/

LList *animators = NULL;

Animator *animator_new (u32 objectID) {

    Animator *new_animator = (Animator *) malloc (sizeof (Animator));
    if (new_animator) {
        new_animator->goID = objectID;
        new_animator->start = true;
        new_animator->playing = false;
        new_animator->defaultAnimation = NULL;
        new_animator->currAnimation = NULL;
        new_animator->currFrame = 0;
        new_animator->n_animations = 0;
        new_animator->animations = NULL;
        new_animator->timer = timer_new ();

        llist_insert_next (animators, llist_end (animators), new_animator);
    }
    
    return new_animator;

}

void animator_destroy (Animator *animator) {

    if (animator) {
        if (animator->animations) {
            for (u8 i = 0; i < animator->n_animations; i++)
                if (animator->animations[i])
                    animation_destroy (animator->animations[i]);

            free (animator->animations);
        }

        timer_destroy (animator->timer);

        void *data = llist_remove (animators, llist_get_list_node (animators, animator));
        if (data) free (data);
        else free (animator);
    }

}

static void animator_destroy_ref (void *data) { 
    
    if (data) animator_destroy ((Animator *) data);
    
}

void animator_set_default_animation (Animator *animator, Animation *animation) {

    if (animator && animation) 
        animator->defaultAnimation = animation;

}

void animator_set_current_animation (Animator *animator, Animation *animation) {

    if (animator && animation) 
        if (!animator->playing) animator->currAnimation = animation; 

}

void animator_play_animation (Animator *animator, Animation *animation) {

    if (animator && animation) {
        animator_set_current_animation (animator, animation);
        animator->start = true;
        animator->playing = true;
        animator->currFrame = 0;
        timer_start (animator->timer);
    } 

}

/*** ANIM THREAD ***/

static pthread_t anim_thread;

void *animations_update (void *data) {

    thread_set_name ("animation");

    u32 timePerFrame = 1000 / FPS_LIMIT;
    u32 frameStart = 0;
    i32 sleepTime = 0;

    float deltaTime = 0;
    u32 deltaTicks = 0;
    u32 fps = 0;

    while (running) {
        frameStart = SDL_GetTicks ();

        if (llist_size (animators) > 0) {
             // update all animations
            Animator *animator = NULL;
            Graphics *graphics = NULL;
            for (ListNode *node = llist_start (animators); node != NULL; node = node->next) {
                animator = (Animator *) node->data;
                graphics = (Graphics *) game_object_get_component (game_object_get_by_id (animator->goID), GRAPHICS_COMP);

                animator->currFrame = (int) (((animator->timer->ticks / animator->currAnimation->speed) %
                        animator->currAnimation->n_frames));

                graphics->x_sprite_offset = animator->currAnimation->frames[animator->currFrame]->col;
                graphics->y_sprite_offset = animator->currAnimation->frames[animator->currFrame]->row;

                if (animator->playing) {
                    if (animator->currFrame >= (animator->currAnimation->n_frames - 1)) {
                        animator->playing = false;
                        animator->currAnimation = animator->defaultAnimation;
                        animator->currFrame = 0;
                        timer_start (animator->timer);
                    }
                }
            }
        }

        // limit the FPS
        sleepTime = timePerFrame - (SDL_GetTicks () - frameStart);
        if (sleepTime > 0) SDL_Delay (sleepTime);

        // update animators timers
        if (llist_size (animators) > 0) {
            Animator *animator = NULL;
            for (ListNode *node = llist_start (animators); node != NULL; node = node->next) {
                animator = (Animator *) node->data;
                animator->timer->ticks = SDL_GetTicks () - animator->timer->startTicks;
            }
        }

        // count fps
        deltaTime = SDL_GetTicks () - frameStart;
        deltaTicks += deltaTime;
        fps++;
        if (deltaTicks >= 1000) {
            // printf ("anim fps: %i\n", fps);
            deltaTicks = 0;
            fps = 0;
        }
    }

}

int animations_init (void) {

    int errors = 0;

    animators = llist_init (animator_destroy_ref);
    if (animators) {
        if (!pthread_create (&anim_thread, NULL, animations_update, NULL)) anim_init = true;
        else {
            #ifdef DEV
            logMsg (stderr, ERROR, NO_TYPE, "Failed to create animations thread.");
            #endif
            errors = 1;
        }
    }

    else {
        #ifdef DEV
        logMsg (stderr, ERROR, NO_TYPE, "Failed to create animators list!");
        #endif
        errors = 1;
    }

    return errors;

}

void animations_end (void) {

    llist_destroy (animators);

    if (anim_init) pthread_join (anim_thread, NULL);

}