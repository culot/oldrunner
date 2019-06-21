/*  $Id: hero.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */
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

struct hero {
  struct coord  pos, initpos;
  int           state;
};

static struct hero  hero;

void
hero_init (void)
{
  hero.state = STATE_ALIVE;
  hero_draw ();
  usr_reset_input ();
}

void
hero_draw (void)
{
  gfx_show_sprite (SP_HERO, &hero.pos);
}

void
hero_set_pos (const struct coord *pos)
{
  coord_set_yx (&hero.pos, pos->y, pos->x);
  if (hero.pos.y < 0)
    lvl_won ();
}

void
hero_set_initpos (const struct coord *pos)
{
  coord_set_yx (&hero.initpos, pos->y, pos->x);
}

void
hero_get_pos (struct coord *pos)
{
  coord_set_yx (pos, hero.pos.y, hero.pos.x);
}

void
hero_get_initpos (struct coord *pos)
{
  coord_set_yx (pos, hero.initpos.y, hero.initpos.x);
}

unsigned
hero_at_pos (const struct coord *pos)
{
  if (coord_equal (&hero.pos, pos))
    return 1;
  return 0;
}

void
check_collisions (const struct coord *hero_pos)
{
  if (lvl_got_hole_below (hero_pos))
    {
      struct coord below;

      hero_trapped ();
      coord_below (hero_pos, &below);
      gfx_move_sprite (SP_HERO, hero_pos, &below);
      hero_set_pos (&below);
    }
  else
    {
      if (foes_at_pos (hero_pos))
        hero_die ();
      money_check_at (hero_pos);
      if (money_all_collected ())
        lvl_draw_escape_ladder ();
    }
}

void
hero_move (enum move move)
{
  struct coord orig, dest;

  /* First check if the move is valid with regards to current player state. */
  if (hero.state & STATE_DIGGING)
    return;

  if ((hero.state & STATE_FALLING) && (move != MOV_FALL))
    move = MOV_FALL;

  /* Then check if the move is valid with regards to the level layout. */
  hero_get_pos (&orig);
  if (!lvl_valid_move (&orig, move, &dest, SP_HERO))
    {
      if (hero.state & STATE_FALLING)
        hero.state &= ~STATE_FALLING;
      return;
    }
  else
    hero_set_pos (&dest);

  if (move == MOV_FALL || lvl_nothing_below (&hero.pos))
    hero.state |= STATE_FALLING;

  gfx_move_sprite (SP_HERO, &orig, &hero.pos);
  check_collisions (&hero.pos);
}

void
hero_dig (enum move dir)
{
  struct coord heropos, digpos;

  hero_get_pos (&heropos);
  coord_set_yx (&digpos,
                heropos.y + 1, heropos.x + (dir == MOV_RIGHT ? 1 : -1));
  if (lvl_valid_dig (&digpos))
    {
      hero.state |= STATE_DIGGING;
      bricks_break (&digpos);
    }
}

void
hero_dig_done (void)
{
  hero.state &= ~STATE_DIGGING;
}

void
hero_trapped (void)
{
  hero.state |= STATE_TRAPPED | STATE_FALLING;
  usr_reset_input ();
}

void
hero_die (void)
{
  gfx_alert ();
  game_lives_dec ();
  lvl_lost ();
}

unsigned
hero_wallup_at (const struct coord *pos)
{
  if (coord_equal (pos, &hero.pos))
    {
      hero_die ();
      return 1;
    }

  return 0;
}
