/*  $Id: bricks.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */
/* vim: et ai sts=2 ts=2 sw=2:
 * */

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

#define BRICK_LATENCY        (OLDRUNNER_TIMEOUT * 4)

enum brick_state {
  BRICK_SMALL_CRACK,
  BRICK_LARGE_CRACK,
  BRICK_BROKEN,
  BRICK_COMEBACK_LARGE_CRACK,
  BRICK_COMEBACK_SMALL_CRACK,
  BRICK_REBUILT,
  BRICK_STATES
};

struct brick {
  enum brick_state    state;
  struct timer        timer;
  struct coord        pos;
  SLIST_ENTRY(brick)  bricksp;
};

SLIST_HEAD(, brick)  bricks;

void
bricks_init (void)
{
  SLIST_INIT (&bricks);
}

void
bricks_free (void)
{
  while (!SLIST_EMPTY (&bricks))
    {
      struct brick *b;

      b = SLIST_FIRST (&bricks);
      SLIST_REMOVE_HEAD (&bricks, bricksp);
      xfree (b);
    }
}

void
bricks_break (const struct coord *pos)
{
  struct brick *b;

  b = xmalloc (sizeof *b);
  coord_set_yx (&b->pos, pos->y, pos->x);
  b->state = BRICK_SMALL_CRACK;
  timer_start (&b->timer);
  SLIST_INSERT_HEAD (&bricks, b, bricksp);
}

static void
draw_brick (const struct coord *pos, enum brick_state state)
{
  switch (state)
    {
    case BRICK_SMALL_CRACK:
    case BRICK_COMEBACK_SMALL_CRACK:
      gfx_show_sprite (SP_BRICK_SCRACK, pos);
      break;
    case BRICK_LARGE_CRACK:
    case BRICK_COMEBACK_LARGE_CRACK:
      gfx_show_sprite (SP_BRICK_LCRACK, pos);
      break;
    case BRICK_BROKEN:
      if (hero_at_pos (pos))
        gfx_show_sprite (SP_HERO, pos);
      else if (foes_at_pos (pos))
        gfx_show_sprite (SP_FOE, pos);
      else
        gfx_show_sprite (SP_BRICK_BROKEN, pos);
      break;
    case BRICK_REBUILT:
      gfx_show_sprite (SP_BRICK, pos);
      break;
    default:
      return;
      /* NOTREACHED */
    }
}

void
bricks_draw (void)
{
  struct brick *b;

  SLIST_FOREACH (b, &bricks, bricksp)
    draw_brick (&b->pos, b->state);
}

unsigned
bricks_broken_at (const struct coord *pos)
{
  struct brick *b;

  SLIST_FOREACH (b, &bricks, bricksp)
    {
      if (b->state == BRICK_BROKEN && coord_equal (&b->pos, pos))
        return 1;
    }

  return 0;
}

void
bricks_update (void)
{
  struct brick *b;
  struct timer now;

  timer_get_time (&now);
  SLIST_FOREACH (b, &bricks, bricksp)
    {
      double difftot;

      difftot = timer_diff (&now, &b->timer);
      if (difftot > BRICK_LATENCY)
        {
          timer_set (&now, &b->timer);
          b->state++;
          if (b->state == BRICK_BROKEN)
            {
              hero_dig_done ();
              timer_add (&b->timer, BRICK_COMEBACK_TIME);
            }
          else if (b->state > BRICK_REBUILT)
            {
              foes_wallup_at (&b->pos);
              if (!hero_wallup_at (&b->pos))
                SLIST_REMOVE (&bricks, b, brick, bricksp);
            }
        }
    }
}
