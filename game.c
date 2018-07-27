/*  $Id: game.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */

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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "oldrunner.h"

#ifdef LEVELS_PATH
#  define  LEVELS  LEVELS_PATH "/original.lvl"
#else
#  define  LEVELS  "./levels/original.lvl"
#endif

#define  POINTS_FOR_MONEY  50
#define  LIVES_AT_START    5

static struct game_info ginfo;

static void
game_lost (void)
{
  struct usr_input input;
  
  gfx_game_over ();
  gfx_get_input (&input);
  game_end ();
}

void
game_won (void)
{
  struct usr_input input;
  
  gfx_game_won ();
  gfx_get_input (&input);
  game_end ();
}

void
game_init (int startlvl)
{
  lvl_init ();
  if (!game_load (LEVELS))
    EXIT ("Could not load game levels. Aborting...");
  ginfo.lives = LIVES_AT_START;
  ginfo.level = startlvl + 1;
  gfx_init ();  
  lvl_load (startlvl);
  hero_init ();
  game_update ();
}

/*
 * The window will be centered on player either if the user requests it or if
 * the level board is too large to fit it the screen.
 */
static void
set_view (void)
{
  struct size lvl;

  lvl.w = lvl_width ();
  lvl.h = lvl_height ();
  
  if (AUTO_CENTER_MODE || lvl.h > VIEWPORT_HEIGHT || lvl.w > VIEWPORT_WIDTH)
    {
      struct coord player_pos;

      hero_get_pos (&player_pos);      
      gfx_center_at (&player_pos);
    }
  else
    {
      struct coord fixed_pos;

      coord_set_yx (&fixed_pos, VIEWPORT_HEIGHT / 2, lvl.w / 2);
      gfx_center_at (&fixed_pos);
    }
}

void
game_update (void)
{
  set_view ();
  lvl_objects_update ();
  hero_draw ();
  if (!(ginfo.state & NO_CHANGE))
    {
      gfx_update_info (&ginfo);
      ginfo.state = NO_CHANGE;
    }
  gfx_update ();
}

void
game_end (void)
{
  gfx_end ();
  exit (0);
}

static unsigned
read_level_row (char *row, unsigned rownum)
{
  char *rowend;
  int len;
  
  if (*row != '[')
    return 0;
  row++;

  if (!(rowend = strchr (row, ']')))
    return 0;
  *rowend = '\0';
  len = rowend - row;
  
  return lvl_set_row (rownum, len, row);
}

static unsigned
store_level_attr (const char *attr, char *val)
{
  if (!strcmp (attr, "level"))
    return lvl_set_name (val);
  else if (!strcmp (attr, "author"))
    return lvl_set_author (val);
  else if (!strcmp (attr, "size"))
    return lvl_set_size (val);
  else
    return 0;
}

/* XXX use strsep instead. */
static unsigned
read_level_attr (char *line)
{
  char *p, *attr, *val;

  attr = p = line;

  while (!isblank ((int)(*p)))
    p++;
  *p++ = '\0';

  while (isblank ((int)(*p)))
    p++;
  val = p;

  return store_level_attr (attr, val);
}

/*
 * XXX improve comment.
 *
 * A game file contains the description of the levels.
 * The following rules apply:
 *   - empty lines are ignored, as well as those beginning with #
 *   - the following keywords are used to describe a level:
 *     -  level: name of the level (default: "")
 *     - author: author of the level (default: "")
 *     -   size: in the form widthxlength (default: 26x16)
 *   - if keywords are present, they must be placed before the level layout
 *   - the reading of a level row stops as soon as the line does
 *     not start with '[', meaning a layout description must not have empty
 *     lines
 */
unsigned
game_load (const char *path)
{
  FILE *f;
  char *buf;
  unsigned rownum, newlevel;
  
  if (!(f = io_fopen (LEVELS)))
    return 0;

  newlevel = 1;
  rownum = 0;
  while ((buf = io_getln (f)))
    {
      char *p;

#define CHK_NEW_LVL  do {                       \
        if (newlevel)                           \
          {                                     \
            lvl_add_new ();                     \
            newlevel = 0;                       \
            rownum = 0;                         \
          }                                     \
    } while (0)
      
      /* Remove trailing spaces. */
      p = buf;
      while (isblank ((int)(*p)))
        p++;
      
      switch (*p)
        {
        case '[':
          CHK_NEW_LVL;
          read_level_row (p, rownum);
          rownum++;
          break;
        case '#':
        case '\0':
          if (rownum)
            newlevel = 1;
          continue;
        default:
          if (rownum)
            newlevel = 1;
          CHK_NEW_LVL;
          read_level_attr (p);
          break;
        }
      
#undef CHK_NEW_LVL      
    }

  io_fclose (f);

  return 1;
}

void
game_score_inc (void)
{
  ginfo.state |= SCORE_CHANGE;
  ginfo.score += POINTS_FOR_MONEY;
}

int
game_level_num (void)
{
  return ginfo.level;
}

void
game_level_inc (void)
{
  ginfo.state |= LEVEL_CHANGE;
  ginfo.level++;
}

void
game_level_dec (void)
{
  ginfo.state |= LEVEL_CHANGE;  
  ginfo.level--;
}

void
game_lives_inc (void)
{
  ginfo.state |= LIVES_CHANGE;  
  ginfo.lives++;
}

void
game_lives_dec (void)
{
  ginfo.state |= LIVES_CHANGE;
  if (ginfo.lives == 0)
    game_lost ();
  ginfo.lives--;
}
