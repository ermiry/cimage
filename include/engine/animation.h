#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdbool.h>

#include "blackrock.h"

#include "engine/timer.h"
#include "engine/sprites.h"

#define DEFAULT_ANIM_SPEED      100

typedef struct Animation {

    IndividualSprite **frames;
    u8 n_frames;
    u32 speed;

} Animation;

typedef struct Animator {

    u32 goID;
    bool start;
    bool playing;
    u8 currFrame;
    u8 n_animations;
    Animation **animations;
    Animation *currAnimation;
    Animation *defaultAnimation;
    Timer *timer;

} Animator;

extern Animation *animation_create (u8 n_frames, ...);
extern void animation_destroy (Animation *animation);
extern void animation_set_speed (Animation *animation, u32 speed);

extern Animator *animator_new (u32 objectID);
extern void animator_destroy (Animator *animator);

void animator_set_default_animation (Animator *animator, Animation *animation);
extern void animator_set_current_animation (Animator *animator, Animation *animation);
extern void animator_play_animation (Animator *animator, Animation *animation);

/*** ANIM THREAD ***/

extern int animations_init (void);
extern void animations_end (void);

#endif