/*  $Id: oldrunner.h,v 1.2 2012/01/06 10:11:30 culot Exp $  */

/*
 * Copyright (c) 2010, 2012 Frederic Culot <frederic@culot.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the
 *        following disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the
 *        following disclaimer in the documentation and/or other
 *        materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OLDRUNNER_H
#define OLDRUNNER_H

#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "compat.h"
#include "cfg.h"

/* Option to set auto-center mode on/off */
#define AUTO_CENTER_MODE  0

/* Game level screen properties. */
#define LEVEL_DEFAULT_WIDTH     26
#define LEVEL_DEFAULT_HEIGHT    16
#define LEVEL_MAX_WIDTH         128
#define LEVEL_MAX_HEIGHT        128
#define VIEWPORT_WIDTH          80
#define VIEWPORT_HEIGHT         24

/* Possible characters states. */
#define   STATE_ALIVE     0x01
#define   STATE_FALLING   0x02
#define   STATE_DIGGING   0x04
#define   STATE_TRAPPED   0x08

/* Game sprites. */
enum sprite {
  /* Sprites used to draw a level. */
  SP_NONE,
  SP_BRICK,
  SP_BRICK_SCRACK,
  SP_BRICK_LCRACK,
  SP_BRICK_BROKEN,
  SP_CIMENT,
  SP_FAKE_BRICK,
  SP_LADDER,
  SP_ROPE,
  SP_ESCAPE_LADDER,
  
  /* Sprites used for objects. */
  SP_MONEY,
  
  /* Sprites used to draw characters. */
  SP_HERO,
  SP_FOE,
  SP_INVALID,
  SPRITES
};

enum move {
  MOV_UP,
  MOV_DOWN,
  MOV_LEFT,
  MOV_RIGHT,
  MOV_FALL,
  MOV_NONE,
  MOVES
};

struct size {
  unsigned  h; /* height */  
  unsigned  w; /* width */
};

struct coord {
  int  y;  /* Position on the vertical axis. */
  int  x;  /* Position on the horizontal axis. */
};

struct timer {
  double  time;    /* time in usec. */
  double  elapsed;
};

/* Structure defining all possible user inputs. */
struct usr_input {
  int key;
};

struct game_info {
#define  NO_CHANGE     0x0  
#define  LEVEL_CHANGE  0x1
#define  SCORE_CHANGE  0x2
#define  LIVES_CHANGE  0x4  
  int       state;
  
  unsigned  level;  
  unsigned  score;
  unsigned  lives;
};

#define STRINGIFY(x)  #x
#define TOSTRING(x)   STRINGIFY(x)
#define __FILE_POS__   __FILE__ ":" TOSTRING(__LINE__)

#define ERROR_MSG(...) do {                                             \
  char msg[BUFSIZ];                                                     \
  int len;                                                              \
                                                                        \
  len = snprintf (msg, BUFSIZ, "%s: %d: ", __FILE__, __LINE__);         \
  (void)snprintf (msg + len, BUFSIZ - len, __VA_ARGS__);                \
  (void)fprintf (stderr, "%s\n", msg);                                  \
} while (0)

#define EXIT(...) do {                                                  \
          ERROR_MSG(__VA_ARGS__);                                       \
          exit (1);                                                     \
} while (0)

#define EXIT_IF(cond, ...) do {                                         \
  if ((cond))                                                           \
    EXIT(__VA_ARGS__);                                                  \
} while (0)

/* bricks.c */
#define   BRICK_COMEBACK_TIME  (OLDRUNNER_TIMEOUT * 80)
void      bricks_init (void);
void      bricks_free (void);
void      bricks_break (const struct coord *);
void      bricks_draw (void);
void      bricks_update (void);
unsigned  bricks_broken_at (const struct coord *);

/* coord.c */
void       coord_copy (const struct coord *, struct coord *);
void       coord_below (const struct coord *, struct coord *);
void       coord_above (const struct coord *, struct coord *);
enum move  coord_opposite_dir (enum move);
void       coord_compute (const struct coord *, enum move, struct coord *);
void       coord_set_yx (struct coord *, int, int);
unsigned   coord_equal (const struct coord *, const struct coord *);
void       coord_diff (const struct coord *, const struct coord *,
                       struct coord *);

/* foes.c */
void      foes_init (void);
void      foes_free (void);
void      foes_add (const struct coord *);
void      foes_draw (void);
unsigned  foes_at_pos (const struct coord *);
void      foes_update_pos (void);
unsigned  foes_wallup_at (const struct coord *);

/* game.c */
void      game_won (void);
void      game_init (int);
void      game_end (void);
void      game_update (void);
unsigned  game_load (const char *);
void      game_score_inc (void);
int       game_level_num (void);
void      game_level_inc (void);
void      game_level_dec (void);
void      game_lives_inc (void);
void      game_lives_dec (void);

/* gfx.c */
void  gfx_init (void);
void  gfx_end (void);
void  gfx_alert (void);
void  gfx_center_at (struct coord *);
void  gfx_update (void);
void  gfx_get_input (struct usr_input *);
void  gfx_show_sprite (enum sprite, const struct coord *);
void  gfx_move_sprite (enum sprite, const struct coord *, const struct coord *);
void  gfx_update_info (const struct game_info *);
void  gfx_popup (const char *, const char *);
void  gfx_game_over (void);
void  gfx_game_won (void);

/* hero.c */
void      hero_init (void);
void      hero_draw (void);
void      hero_set_pos (const struct coord *);
void      hero_set_initpos (const struct coord *);
void      hero_get_pos (struct coord *);
void      hero_get_initpos (struct coord *);
unsigned  hero_at_pos (const struct coord *);
void      hero_move (enum move);
void      hero_dig (enum move);
void      hero_dig_done (void);
void      hero_trapped (void);
void      hero_die (void);
unsigned  hero_wallup_at (const struct coord *);

/* io.c */
FILE  *io_fopen (const char *);
void   io_fclose (FILE *);
char  *io_getln (FILE *);

/* lvl.c */
void         lvl_started (void);
enum sprite  lvl_decor_at_pos (const struct coord *);
void         lvl_init (void);
unsigned     lvl_load (int);
unsigned     lvl_load_next (void);
unsigned     lvl_load_prev (void);
void         lvl_won (void);
void         lvl_lost (void);
void         lvl_draw_escape_ladder (void);
unsigned     lvl_set_name (const char *);
unsigned     lvl_set_author (const char *);
unsigned     lvl_set_size (char *);
unsigned     lvl_set_row (int, int, const char *);
unsigned     lvl_width (void);
unsigned     lvl_height (void);
unsigned     lvl_add_new (void);
unsigned     lvl_valid_move (const struct coord *, enum move, struct coord *,
                             enum sprite);
unsigned     lvl_valid_dig (const struct coord *);
unsigned     lvl_nothing_below (const struct coord *);
unsigned     lvl_obstacle_at (const struct coord *);
void         lvl_objects_update (void);
unsigned     lvl_got_hole_below (const struct coord *);
enum move    lvl_shortest_way (const struct coord *, enum move, enum move);
int          lvl_random_xpos (void);
enum move    lvl_random_hdir (void);

/* mem.c */
void  *xmalloc (size_t);
void  *xcalloc (size_t, size_t);
void  *xrealloc (void *, size_t, size_t);
char  *xstrdup (const char *);
void   xfree (void *);

/* money.c */
void      money_init (void);
void      money_free (void);
void      money_add (const struct coord *);
void      money_draw (void);
void      money_check_at (const struct coord *);
unsigned  money_all_collected (void);

/* timer.c */
void      timer_get_time (struct timer *);
void      timer_start (struct timer *);
void      timer_stop (struct timer *);
double    timer_diff (const struct timer *, const struct timer *);
double    timer_elapsed (const struct timer *);
void      timer_set (const struct timer *, struct timer *);
void      timer_add (struct timer *, double);
unsigned  timer_delay_elapsed (struct timer *, double);

/* usr.c */
void  usr_input (void);
void  usr_reset_input (void);

#endif /* OLDRUNNER_H */
