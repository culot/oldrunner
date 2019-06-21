/*  $Id: lvl.c,v 1.3 2012/01/31 14:36:29 culot Exp $  */
/* vim: et ai sts=2 ts=2 sw=2:
 * */

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

#include <string.h>

#include "oldrunner.h"

struct lvlattr {
  struct size    siz;
  char          *name;
  char          *author;
};

/* Doubly-linked to store information about game levels. */
TAILQ_HEAD(levels_head, level) levels;

struct level {
  struct lvlattr       attr;
  char               **lay;
  TAILQ_ENTRY(level)   levelsp;
};

static struct level    *curlvl;

static enum sprite char2sprite[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

/*   !  "     #          $     %    &     '  (  )  *  +  ,     -     .  / */
  0, 0, 0, SP_BRICK, SP_MONEY, 0, SP_FOE, 0, 0, 0, 0, 0, 0, SP_ROPE, 0, 0,

/*0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ? */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

/*   @     A  B  C  D  E  F  G      H      I  J  K  L  M  N  O */
  SP_HERO, 0, 0, 0, 0, 0, 0, 0, SP_LADDER, 0, 0, 0, 0, 0, 0, 0,

/*P  Q  R  S  T  U        V        W      X      Y  Z  [  \  ]  ^  _ */
  0, 0, 0, 0, 0, 0, SP_FAKE_BRICK, 0, SP_CIMENT, 0, 0, 0, 0, 0, 0, 0,

/*`  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

/*p  q  r  s  t  u  v  w  x  y  z  {          |         }  ~    */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SP_ESCAPE_LADDER, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned obj_is_static[SPRITES] = {
  1,  /* SP_NONE */
  1,  /* SP_BRICK */
  1,  /* SP_BRICK_SCRACK */
  1,  /* SP_BRICK_LCRACK */
  1,  /* SP_BRICK_BROKEN */
  1,  /* SP_CIMENT */
  1,  /* SP_FAKE_BRICK */
  1,  /* SP_LADDER */
  1,  /* SP_ROPE */
  0,  /* SP_ESCAPE_LADDER */
  0,  /* SP_MONEY */
  0,  /* SP_HERO */
  0,  /* SP_FOE */
  0,  /* SP_INVALID */
};

static unsigned obj_is_obstacle[SPRITES] = {
  0,  /* SP_NONE */
  1,  /* SP_BRICK */
  1,  /* SP_BRICK_SCRACK */
  1,  /* SP_BRICK_LCRACK */
  0,  /* SP_BRICK_BROKEN */
  1,  /* SP_CIMENT */
  0,  /* SP_FAKE_BRICK */
  0,  /* SP_LADDER */
  0,  /* SP_ROPE */
  0,  /* SP_ESCAPE_LADDER */
  0,  /* SP_MONEY */
  0,  /* SP_HERO */
  0,  /* SP_FOE */
  0,  /* SP_INVALID */
};

static void
init_layout (struct level *l)
{
  int i;

  l->lay = xcalloc (l->attr.siz.h, sizeof (char *));
  for (i = 0; i < l->attr.siz.h; i++)
    l->lay[i] = xmalloc (l->attr.siz.w);
}

static enum sprite
sprite_at_pos (const struct coord *pos)
{
  if (pos->y >= curlvl->attr.siz.h || pos->x >= curlvl->attr.siz.w)
    return SP_INVALID;

  return char2sprite[(unsigned char)curlvl->lay[pos->y][pos->x]];
}

enum sprite
lvl_decor_at_pos (const struct coord *pos)
{
  enum sprite sp;

  sp = sprite_at_pos (pos);
  return obj_is_static[sp] || (sp == SP_ESCAPE_LADDER
                               && money_all_collected ())
    ? sp : SP_NONE;
}

unsigned
lvl_set_name (const char *name)
{
  if (!(curlvl->attr.name = xstrdup (name)))
    return 0;
  return 1;
}

unsigned
lvl_set_author (const char *author)
{
  if (!(curlvl->attr.author = xstrdup (author)))
    return 0;
  return 1;
}

/* The size must be of the form AxB. */
unsigned
lvl_set_size (char *sizstr)
{
  const char *errstr;
  char *x, *y;
  int sizex, sizey;

  x = sizstr;
  if (!(y = strchr (sizstr, 'x')))
    return 0;
  *y++ = '\0';

  sizex = strtonum (x, 0, LEVEL_MAX_WIDTH, &errstr);
  if (errstr)
    return 0;
  sizey = strtonum (y, 0, LEVEL_MAX_HEIGHT, &errstr);
  if (errstr)
    return 0;

  curlvl->attr.siz.h = sizey;
  curlvl->attr.siz.w = sizex;

  return 1;
}

unsigned
lvl_set_row (int rownum, int rowlen, const char *row)
{
  if (rowlen != curlvl->attr.siz.w)
    {
      fprintf (stderr, "Incorrect row length (was: %d, expected: %d)!\n",
               rowlen, curlvl->attr.siz.w);
      return 0;
    }
  if (rownum >= curlvl->attr.siz.h)
    {
      fprintf (stderr, "Level row number too large (was: %d, max: %d)!\n",
               rownum, curlvl->attr.siz.h);
      return 0;
    }
  if (!curlvl->lay)
    init_layout (curlvl);

  memcpy (curlvl->lay[rownum], row, rowlen);

  return 1;
}

unsigned
lvl_width (void)
{
  return curlvl->attr.siz.w;
}

unsigned
lvl_height (void)
{
  return curlvl->attr.siz.h;
}

unsigned
lvl_add_new (void)
{
  struct level *lvl;

  lvl = xmalloc (sizeof *lvl);
  bzero (lvl, sizeof *lvl);
  lvl->attr.siz.w = LEVEL_DEFAULT_WIDTH;
  lvl->attr.siz.h = LEVEL_DEFAULT_HEIGHT;
  TAILQ_INSERT_TAIL (&levels, lvl, levelsp);
  curlvl = lvl;

  return 1;
}

void
lvl_init (void)
{
  TAILQ_INIT (&levels);
  money_init ();
  foes_init ();
  bricks_init ();
}

static void
dynaobj_free (void)
{
  money_free ();
  foes_free ();
  bricks_free ();
}

static void
lvl_load_dynaobjs (void)
{
  struct coord lvlpos;
  int r, c;

  dynaobj_free ();

  for (r = 0; r < curlvl->attr.siz.h; r++)
    {
      lvlpos.y = r;
      for (c = 0; c < curlvl->attr.siz.w; c++)
        {
          lvlpos.x = c;
          switch (sprite_at_pos (&lvlpos))
            {
            case SP_HERO:
              hero_set_initpos (&lvlpos);
              break;
            case SP_MONEY:
              money_add (&lvlpos);
              break;
            case SP_FOE:
              foes_add (&lvlpos);
              break;
            default:
              /* DO NOTHING */
              break;
            }
        }
    }
}

static void
draw_hero_init_pos (void)
{
  struct coord pos;

  hero_get_initpos (&pos);
  hero_set_pos (&pos);
}

static void
draw_static_objects (void)
{
  int r;

  for (r = 0; r < curlvl->attr.siz.h; r++)
    {
      struct coord pos;
      int c;

      pos.y = r;
      for (c = 0; c < curlvl->attr.siz.w; c++)
        {
          enum sprite sp;

          pos.x = c;
          sp = sprite_at_pos (&pos);
          gfx_show_sprite (obj_is_static[sp] ? sp : SP_NONE, &pos);
        }
    }
}

static void
draw_level (void)
{
  draw_static_objects ();
  money_draw ();
  bricks_draw ();
  foes_draw ();
}

static void
lvl_update_new (void)
{
  draw_hero_init_pos ();
  draw_level ();
}

static void
show_level_info (void)
{
  char title[BUFSIZ], info[BUFSIZ];

  (void)snprintf (title, sizeof title, "LEVEL #%d", game_level_num ());
  (void)snprintf (info, sizeof info,
                  "\t  Name:  %s\n\n"
                  "\tAuthor:  %s\n",
                  curlvl->attr.name, curlvl->attr.author);
  gfx_popup (title, info);
}

static void
load_level (void)
{
  lvl_load_dynaobjs ();
  lvl_update_new ();
  hero_init ();
  show_level_info ();
}

static void
lvl_select_current (int lvlnum)
{
  curlvl = TAILQ_FIRST (&levels);
  while (lvlnum)
    {
      curlvl = TAILQ_NEXT (curlvl, levelsp);
      EXIT_IF (curlvl == 0, "lvl_select_current: invalid level number");
      lvlnum--;
    }
}

/* Load the level, assign initial player position and draw it. */
unsigned
lvl_load (int levelnum)
{
  lvl_select_current (levelnum);
  load_level ();

  return 1;
}

unsigned
lvl_load_next (void)
{
  if (!(curlvl = TAILQ_NEXT (curlvl, levelsp)))
    game_won ();
  game_level_inc ();
  load_level ();

  return 1;
}

unsigned
lvl_load_prev (void)
{
  struct level *previous_lvl;

  if (!(previous_lvl = TAILQ_PREV (curlvl, levels_head, levelsp)))
    return 0;

  curlvl = previous_lvl;
  game_level_dec ();
  load_level ();

  return 1;
}

void
lvl_won (void)
{
  lvl_load_next ();
}

void
lvl_lost (void)
{
  load_level ();
}

void
lvl_draw_escape_ladder (void)
{
  int r;

  for (r = 0; r < curlvl->attr.siz.h; r++)
    {
      struct coord pos;
      int c;

      pos.y = r;
      for (c = 0; c < curlvl->attr.siz.w; c++)
        {
          pos.x = c;
          if (sprite_at_pos (&pos) == SP_ESCAPE_LADDER)
            gfx_show_sprite (SP_ESCAPE_LADDER, &pos);
        }
    }
}

static unsigned
valid_decor_move (const struct coord *pos_orig, const struct coord *pos_dest,
                  enum move wanted_move, enum sprite sp)
{
  enum sprite sp_orig, sp_dest;

  sp_orig = lvl_decor_at_pos (pos_orig);
  if (wanted_move == MOV_FALL && sp_orig == SP_ROPE)
    return 0;

  if (pos_dest->y < 0 && sp_orig == SP_ESCAPE_LADDER)
    return 1;

  sp_dest = lvl_decor_at_pos (pos_dest);
  switch (sp_dest)
    {
    case SP_NONE:
    case SP_ROPE:
      if (wanted_move == MOV_UP)
        return sp_orig == SP_LADDER || sp_orig == SP_ESCAPE_LADDER ? 1 : 0;
      else
        return 1;

    case SP_BRICK:
      return wanted_move != MOV_UP && bricks_broken_at (pos_dest) ? 1 : 0;

    case SP_CIMENT:
      return 0;

    case SP_LADDER:
      return wanted_move == MOV_FALL ? 0 : 1;

    case SP_ESCAPE_LADDER:
      if (sp == SP_HERO)
        return wanted_move == MOV_FALL ? 0 : 1;
      else
        return wanted_move == MOV_UP ? 0 : 1;

    default:
      return 0;
      /* NOTREACHED */
    }
}

unsigned
lvl_valid_move (const struct coord *orig, enum move wanted_move,
                struct coord *dest, enum sprite sp)
{
  if (wanted_move == MOV_NONE)
    return 0;

  coord_compute (orig, wanted_move, dest);
  if (dest->y >= (int)curlvl->attr.siz.h
      || dest->x < 0 || dest->x >= curlvl->attr.siz.w)
    return 0;

  return valid_decor_move (orig, dest, wanted_move, sp);
}

unsigned
lvl_valid_dig (const struct coord *pos)
{
  switch (lvl_decor_at_pos (pos))
    {
    case SP_BRICK:
      return 1;
    default:
      return 0;
    }
}

unsigned
lvl_nothing_below (const struct coord *pos)
{
  struct coord below;

  coord_below (pos, &below);
  return lvl_decor_at_pos (&below) == SP_NONE ? 1 : 0;
}

unsigned
lvl_obstacle_at (const struct coord *pos)
{
  enum sprite sp;

  if (pos->y < 0 || pos->x < 0
      || pos->y > curlvl->attr.siz.h || pos->x > curlvl->attr.siz.w)
    return 1;

  sp = char2sprite[(unsigned char)curlvl->lay[pos->y][pos->x]];
  return obj_is_obstacle[sp];
}

void
lvl_objects_update (void)
{
  bricks_update ();
  bricks_draw ();
  money_draw ();
  foes_update_pos ();
}

unsigned
lvl_got_hole_below (const struct coord *pos)
{
  struct coord below;

  coord_below (pos, &below);
  if (!bricks_broken_at (&below))
    return 0;
  else
    {
      if (foes_at_pos (&below) || hero_at_pos (&below))
        return 0;
      else
        return 1;
    }
}

static unsigned
got_way (const struct coord *orig,
          enum move wanted_dir, enum move prefered_move)
{
  struct coord pos;

  coord_copy (orig, &pos);
  while (!lvl_obstacle_at (&pos))
    {
      struct coord wanted_pos;

      coord_set_yx (&wanted_pos,
                    pos.y + (wanted_dir == MOV_UP ? -1 : 1), pos.x);
      if (valid_decor_move (&pos, &wanted_pos, wanted_dir, SP_NONE))
        return 1;

      pos.x += prefered_move == MOV_RIGHT ? 1 : -1;
    }

  return 0;
}

enum move
lvl_shortest_way (const struct coord *orig,
                  enum move wanted_dir, enum move prefered_move)
{
  struct coord dest;

  if (lvl_valid_move (orig, wanted_dir, &dest, SP_NONE))
    return wanted_dir;
  else
    {
      if (got_way (orig, wanted_dir, prefered_move))
        return prefered_move;
      else
        return coord_opposite_dir (prefered_move);
    }
}

int
lvl_random_xpos (void)
{
  return rand () % curlvl->attr.siz.w;
}
