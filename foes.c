/*  $Id: foes.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */

/*
 * Copyright (c) 2010 Frederic Culot <frederic@culot.org>
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

#include "oldrunner.h"

#define FOES_DELAY         (OLDRUNNER_TIMEOUT * 3)
#define FOES_ESCAPE_DELAY  (BRICK_COMEBACK_TIME * .75)

struct foe {
  struct coord       pos;
  struct timer       timer;
  enum move          current_move;
  int                state;
  SLIST_ENTRY(foe)   foesp;
};

SLIST_HEAD(, foe) foes;

void
foes_init (void)
{
  SLIST_INIT (&foes);
}

void
foes_free (void)
{
  while (!SLIST_EMPTY (&foes))
    {
      struct foe *f;
      
      f = SLIST_FIRST (&foes);
      SLIST_REMOVE_HEAD (&foes, foesp);
      xfree (f);
    }
}

void
foes_add (const struct coord *pos)
{
  struct foe *f;

  f = xmalloc (sizeof *f);
  coord_set_yx (&f->pos, pos->y, pos->x);
  f->current_move = MOV_NONE;
  f->state = STATE_ALIVE;
  SLIST_INSERT_HEAD (&foes, f, foesp);
}

void
foes_draw (void)
{
  struct foe *f;

  SLIST_FOREACH (f, &foes, foesp)
    gfx_show_sprite (SP_FOE, &f->pos);
}

/*
 * Once killed, a foe reappears on top of current level with
 * a random horizontal position.
 */
static void
get_random_inipos (struct coord *pos)
{
  coord_set_yx (pos, 0, -1);
  while (lvl_obstacle_at (pos))
    pos->x = lvl_random_xpos ();
}

static void
foe_killed (struct foe *foe)
{
  struct coord inipos;

  get_random_inipos (&inipos);
  coord_copy  (&inipos, &foe->pos);
  foe->current_move = MOV_FALL;
  foe->state = STATE_ALIVE;
}

static void
foe_trapped (struct foe *foe)
{
  foe->state |= STATE_TRAPPED;
  timer_start (&foe->timer);
}

static void
check_trap (struct foe *foe)
{
  if (lvl_got_hole_below (&foe->pos))
    {
      struct coord below;

      foe_trapped (foe);
      coord_below (&foe->pos, &below);
      gfx_move_sprite (SP_FOE, &foe->pos, &below);
      coord_copy (&below, &foe->pos);
    }
}

static void
check_hole (struct foe *foe)
{
  if (lvl_nothing_below (&foe->pos))
    foe->current_move = MOV_FALL;
}

static unsigned
another_foe_at_pos (const struct foe *foe, const struct coord *pos)
{
  struct foe *f;

  SLIST_FOREACH (f, &foes, foesp)
    if (f != foe && coord_equal (pos, &f->pos))
      return 1;

  return 0;
}

unsigned
foes_at_pos (const struct coord *pos)
{
  struct foe *f;

  SLIST_FOREACH (f, &foes, foesp)
    if (coord_equal (pos, &f->pos))
      return 1;

  return 0;
}

static void
compute_move (const struct coord *hero, struct foe *foe)
{
  struct coord dpos;

  if (foe->current_move == MOV_FALL)
    coord_compute (&foe->pos, foe->current_move, &foe->pos);    
  else
    {
      coord_diff (hero, &foe->pos, &dpos);
      if (dpos.y != 0)
        {
          enum move wanted_dir, prefered_move;
          
          if (foe->current_move != MOV_LEFT && foe->current_move != MOV_RIGHT)
            prefered_move = dpos.x >= 0 ? MOV_RIGHT : MOV_LEFT;
          else
            prefered_move = foe->current_move;
          wanted_dir = dpos.y > 0 ? MOV_DOWN : MOV_UP;
          foe->current_move = lvl_shortest_way (&foe->pos,
                                                wanted_dir,
                                                prefered_move);
          coord_compute (&foe->pos, foe->current_move, &foe->pos);
        }
      else if (dpos.x > 0)
        {
          foe->current_move = MOV_RIGHT;
          foe->pos.x++;
        }
      else if (dpos.x < 0)
        {
          foe->current_move = MOV_LEFT;
          foe->pos.x--;
        }
      else
        foe->current_move = MOV_NONE;
    }
}

static void
try_escape (struct foe *f)
{
  struct coord posorig;
  
  if (!timer_delay_elapsed (&f->timer, FOES_ESCAPE_DELAY))
    return;

  coord_set_yx (&posorig, f->pos.y, f->pos.x);
  coord_set_yx (&f->pos, f->pos.y - 1, f->pos.x);
  gfx_move_sprite (SP_FOE, &posorig, &f->pos);
  f->state &= ~STATE_TRAPPED;
}

static void
update_foe_pos (const struct coord *hero_pos, struct foe *f)
{
  struct coord posorig;
  
  coord_set_yx (&posorig, f->pos.y, f->pos.x);
  compute_move (hero_pos, f);  
  if (!coord_equal (&posorig, &f->pos)
      && lvl_valid_move (&posorig, f->current_move, &f->pos, SP_FOE)
      && !another_foe_at_pos (f, &f->pos))
    {
      gfx_move_sprite (SP_FOE, &posorig, &f->pos);
      check_trap (f);
      check_hole (f);
      if (coord_equal (&f->pos, hero_pos))
        hero_die ();
    }
  else
    {
      /*
       * The move is not possible, revert to current position
       * and force next move to be in opposite direction using
       * foe->current_move.
       */
      coord_copy (&posorig, &f->pos);
      f->current_move =
        f->current_move == MOV_FALL ?
        MOV_NONE : coord_opposite_dir (f->current_move);
    }
}

void
foes_update_pos (void)
{
  static struct timer  foes_timer;
  struct timer         now;
  struct foe          *foe;
  struct coord         hero_pos;

  timer_get_time (&now);
  if (timer_diff (&now, &foes_timer) < FOES_DELAY)
    return;
  
  hero_get_pos (&hero_pos);

  SLIST_FOREACH (foe, &foes, foesp)
    {
      struct coord foe_prevpos;

      coord_set_yx (&foe_prevpos, foe->pos.y, foe->pos.x);
      
      if (foe->state & STATE_TRAPPED)
        try_escape (foe);
      else
        update_foe_pos (&hero_pos, foe);
    }
  timer_get_time (&foes_timer);
}

unsigned
foes_wallup_at (const struct coord *pos)
{
  struct foe *f;

  SLIST_FOREACH (f, &foes, foesp)
    {
      if (coord_equal (pos, &f->pos))
        {
          foe_killed (f);
          return 1;
        }
    }
  
  return 0;
}
