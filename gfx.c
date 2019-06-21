/*  $Id: gfx.c,v 1.2 2012/01/31 14:53:07 culot Exp $  */
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

#include <string.h>

#include "oldrunner.h"

#ifdef HAVE_CURSES_H
#  include <curses.h>
#elif defined HAVE_NCURSES_H
#  include <ncurses.h>
#elif defined HAVE_NCURSES_NCURSES_H
#  include <ncurses/ncurses.h>
#elif defined HAVE_NCURSESW_NCURSES_H
#  include <ncursesw/ncurses.h>
#else
#  error "Missing curses library header. Aborting..."
#endif

#define  INFOBAR_HEIGHT  2

struct window {
  WINDOW          *p;     /* pointer to window */
  struct size      siz;
  struct coord     pos;
};

struct scrollwin {
  struct window    pad;
  struct window    view;
};

static struct scrollwin  scr;
static struct size       term;

static chtype sprite[SPRITES];

static void
sprites_init (void)
{
  sprite[SP_NONE]           = ' ';
  sprite[SP_BRICK]          = ACS_CKBOARD;
  sprite[SP_BRICK_SCRACK]   = '~';
  sprite[SP_BRICK_LCRACK]   = 'U';
  sprite[SP_BRICK_BROKEN]   = sprite[SP_NONE];
  sprite[SP_CIMENT]         = ' ' | A_REVERSE;
  sprite[SP_FAKE_BRICK]     = sprite[SP_BRICK];
  sprite[SP_LADDER]         = 'H';
  sprite[SP_ROPE]           = ACS_HLINE;
  sprite[SP_ESCAPE_LADDER]  = ACS_VLINE;
  sprite[SP_MONEY]          = ACS_DIAMOND;
  sprite[SP_HERO]           = '@';
  sprite[SP_FOE]            = '@' | A_REVERSE | A_BLINK;
  sprite[SP_INVALID]        = '?';
}

static void
screen_init_size (void)
{
  scr.view.siz.w = VIEWPORT_WIDTH;
  scr.view.siz.h = VIEWPORT_HEIGHT;

  getmaxyx (stdscr, term.h, term.w); /* Size of the terminal. */
  coord_set_yx (&scr.view.pos,
                term.h > scr.view.siz.h ? (term.h - scr.view.siz.h) / 2 : 0,
                term.w > scr.view.siz.w ? (term.w - scr.view.siz.w) / 2 : 0);
  scr.pad.siz.w = LEVEL_MAX_WIDTH;
  scr.pad.siz.h = LEVEL_MAX_HEIGHT;
  coord_copy (&scr.view.pos, &scr.pad.pos);
}

static void
screen_init (void)
{
  screen_init_size ();
  scr.pad.p = newpad (scr.pad.siz.h, scr.pad.siz.w);
  scr.view.p = newwin (scr.view.siz.h, scr.view.siz.w,
                       scr.view.pos.y, scr.view.pos.x);
  keypad (scr.view.p, TRUE);  /* Enable function and arrow keys. */
  nodelay (scr.view.p, TRUE);
  box (scr.view.p, 0, 0);
  wnoutrefresh (scr.view.p);
}

static void
screen_update (void)
{
  pnoutrefresh (scr.pad.p, scr.pad.pos.y, scr.pad.pos.x,
                scr.view.pos.y + (scr.view.siz.h - lvl_height()) / 2,
                scr.view.pos.x + (scr.view.siz.w - lvl_width()) / 2,
                scr.view.pos.y + scr.view.siz.h - INFOBAR_HEIGHT,
                scr.view.pos.x + scr.view.siz.w - 2);
}

static void
newpopup (struct window *pop)
{
  const int BORDERWIDTH = 2;

  pop->siz.w = scr.view.siz.w - 2 * BORDERWIDTH;
  pop->siz.h = scr.view.siz.h - 2 * BORDERWIDTH;
  pop->pos.y = scr.view.pos.y + BORDERWIDTH;
  pop->pos.x = scr.view.pos.x + BORDERWIDTH;

  pop->p = newwin (pop->siz.h, pop->siz.w, pop->pos.y, pop->pos.x);
  keypad (pop->p, TRUE);
}

static void
delpopup (struct window *pop)
{
  wclear (pop->p);
  wrefresh (pop->p);
  delwin (pop->p);
}

static void
showpopup (struct window *pop)
{
  box (pop->p, 0, 0);
  wrefresh (pop->p);
}

void
gfx_init (void)
{
  /* Curses initialization. */
  initscr ();                   /* start the curses mode */
  cbreak ();                    /* control chars generate a signal */
  noecho ();                    /* controls echoing of typed chars */
  curs_set (0);                 /* make cursor invisible */
  sprites_init ();

  /* Game window initialization. */
  screen_init ();
}

void
gfx_end (void)
{
  wclear (scr.view.p);
  wrefresh (scr.view.p);
  delwin (scr.pad.p);
  delwin (scr.view.p);
  endwin ();
}

void
gfx_alert (void)
{
  flash ();
  napms (200);
  flash ();
}

void
gfx_center_at (struct coord *pos)
{
  coord_set_yx (&scr.pad.pos,
                pos->y - VIEWPORT_HEIGHT / 2, pos->x - VIEWPORT_WIDTH / 2);
}

void
gfx_update (void)
{
  screen_update ();
  doupdate ();
}

void
gfx_get_input (struct usr_input *input)
{
  int key;

  key = wgetch (scr.view.p);
  input->key = key == ERR ? -1 : key;
}

void
gfx_show_sprite (enum sprite sp, const struct coord *pos)
{
  gfx_move_sprite (sp, 0, pos);
}

/*
 * Show the given character at the current position.
 * The optional previous position is also given so that graphics routines can
 * properly clean it up if necessary.
 *
 * note:
 * If the viewport is larger than the level size, a shift must be applied in
 * order to center the level on screen.
 */
void
gfx_move_sprite (enum sprite sp, const struct coord *prev,
                 const struct coord *current)
{
  mvwaddch (scr.pad.p, current->y, current->x, sprite[sp]);
  if (prev)
    {
      sp = lvl_decor_at_pos (prev);
      mvwaddch (scr.pad.p, prev->y, prev->x, sprite[sp]);
    }

  screen_update ();
}

/*
 * Display information related to current game (score, number of lives and
 * current level).
 */
void
gfx_update_info (const struct game_info *info)
{
  const int HIGHLIGHT = WA_REVERSE | WA_BOLD;
  char score[BUFSIZ], lives[BUFSIZ], level[BUFSIZ];
  int score_len, lives_len, level_len, yoff, xoff;

  score_len = snprintf (score, sizeof score, "[SCORE: %010d]", info->score);
  lives_len = snprintf (lives, sizeof lives, "[LIVES: %03d]", info->lives);
  level_len = snprintf (level, sizeof level, "[LEVEL: %03d]", info->level);

  yoff = scr.view.siz.h - INFOBAR_HEIGHT + 1;
  xoff = scr.view.siz.w - (score_len + lives_len + level_len);
  xoff >>= 2;

  if (info->state & SCORE_CHANGE)
    wattron (scr.view.p, HIGHLIGHT);
  mvwprintw (scr.view.p, yoff, xoff, score);
  if (info->state & SCORE_CHANGE)
    wattroff (scr.view.p, HIGHLIGHT);

  if (info->state & LIVES_CHANGE)
    wattron (scr.view.p, HIGHLIGHT);
  mvwprintw (scr.view.p, yoff, score_len + 2 * xoff, lives);
  if (info->state & LIVES_CHANGE)
    wattroff (scr.view.p, HIGHLIGHT);

  if (info->state & LEVEL_CHANGE)
    wattron (scr.view.p, HIGHLIGHT);
  mvwprintw (scr.view.p, yoff, score_len + lives_len + 3 * xoff, level);
  if (info->state & LEVEL_CHANGE)
    wattroff (scr.view.p, HIGHLIGHT);

  wnoutrefresh (scr.view.p);
}

void
gfx_popup (const char *title, const char *msg)
{
  struct window pop;
  char *any_key = "Press any key...";

  newpopup (&pop);

  mvwhline (pop.p, 2, 1, ACS_HLINE, pop.siz.w - 2);
  mvwprintw (pop.p, 1, (pop.siz.w - strlen (title)) / 2, "%s", title);

  mvwprintw (pop.p, 5, 1, "%s", msg);
  mvwprintw (pop.p, pop.siz.h - 2, pop.siz.w - (strlen (any_key) + 1), "%s",
             any_key);

  showpopup (&pop);
  (void)wgetch (pop.p);
  delpopup (&pop);
}

void
gfx_game_over (void)
{
  const char *rip =
  "                   +-+                   \n"
  "                  +-+-+-+                 \n"
  "                  +-+-+-+                 \n"
  "                    | |                   \n"
  "             +------+-+------+            \n"
  "             |               |            \n"
  "             |   R   I   P   |            \n"
  "  ~~~~~~~~~~~|               |~~~~~~~~~~  \n"
  "             +---------------+            \n";

  gfx_popup ("G A M E  O V E R", rip);
}

/*
 * The ending screen was taken from the ncurses original sources
 * (firework.c file).
 * The original copyright is reproduced below.
 */
/****************************************************************************
 * Copyright (c) 1998-2005,2006 Free Software Foundation, Inc.              *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/
static void
fw_show (struct window *win)
{
  const char *title = "C O N G R A T U L A T I O N S";
  const int titlelen = 29;

  mvwhline (win->p, 2, 1, ACS_HLINE, win->siz.w - 2);
  mvwprintw (win->p, 1, (win->siz.w - titlelen) / 2, "%s", title);
  showpopup (win);
  napms (120);
}

static void
fw_explode (struct window *win, int row, int col)
{
  werase (win->p);
  mvwprintw (win->p, row, col, "-");
  fw_show (win);

  mvwprintw(win->p, row - 1, col - 1, " - ");
  mvwprintw(win->p, row + 0, col - 1, "-+-");
  mvwprintw(win->p, row + 1, col - 1, " - ");
  fw_show (win);

  mvwprintw(win->p, row - 2, col - 2, " --- ");
  mvwprintw(win->p, row - 1, col - 2, "-+++-");
  mvwprintw(win->p, row + 0, col - 2, "-+#+-");
  mvwprintw(win->p, row + 1, col - 2, "-+++-");
  mvwprintw(win->p, row + 2, col - 2, " --- ");
  fw_show (win);

  mvwprintw(win->p, row - 2, col - 2, " +++ ");
  mvwprintw(win->p, row - 1, col - 2, "++#++");
  mvwprintw(win->p, row + 0, col - 2, "+# #+");
  mvwprintw(win->p, row + 1, col - 2, "++#++");
  mvwprintw(win->p, row + 2, col - 2, " +++ ");
  fw_show (win);

  mvwprintw(win->p, row - 2, col - 2, "  #  ");
  mvwprintw(win->p, row - 1, col - 2, "## ##");
  mvwprintw(win->p, row + 0, col - 2, "#   #");
  mvwprintw(win->p, row + 1, col - 2, "## ##");
  mvwprintw(win->p, row + 2, col - 2, "  #  ");
  fw_show (win);

  mvwprintw(win->p, row - 2, col - 2, " # # ");
  mvwprintw(win->p, row - 1, col - 2, "#   #");
  mvwprintw(win->p, row + 0, col - 2, "     ");
  mvwprintw(win->p, row + 1, col - 2, "#   #");
  mvwprintw(win->p, row + 2, col - 2, " # # ");
  fw_show (win);
}

/* XXX quit end screen properly (free popup window) */
static void
fireworks (void)
{
  struct window pop;
  int start, end, row, diff, flag, direction;

  newpopup (&pop);
  showpopup (&pop);
  flag = 0;
  for (;;)
    {
      do
        {
          start = rand() % (pop.siz.w - 3);
          end = rand() % (pop.siz.w - 3);
          start = (start < 2) ? 2 : start;
          end = (end < 2) ? 2 : end;
          direction = (start > end) ? -1 : 1;
          diff = abs (start - end);
        }
      while (diff < 2 || diff >= pop.siz.h - 2);

      for (row = 0; row < diff; row++)
        {
          mvwprintw (pop.p, pop.siz.h - row, start + (row * direction),
                   (direction < 0) ? "\\" : "/");
          if (flag++)
            {
              fw_show (&pop);
              werase (pop.p);
              flag = 0;
            }
        }
      if (flag++)
        {
          fw_show (&pop);
          flag = 0;
        }
      fw_explode (&pop, pop.siz.h - row, start + (diff * direction));
      werase (pop.p);
      fw_show (&pop);
    }
}

void
gfx_game_won (void)
{
  fireworks ();
}
