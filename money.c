/*  $Id: money.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */
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

struct money {
  struct coord        pos;
  SLIST_ENTRY(money)  moneyp;
};

SLIST_HEAD(, money) money_list;

void
money_init (void)
{
  SLIST_INIT (&money_list);
}

void
money_free (void)
{
  while (!SLIST_EMPTY (&money_list))
    {
      struct money *m;

      m = SLIST_FIRST (&money_list);
      SLIST_REMOVE_HEAD (&money_list, moneyp);
      xfree (m);
    }
}

void
money_add (const struct coord *pos)
{
  struct money *m;

  m = xmalloc (sizeof *m);
  coord_set_yx (&m->pos, pos->y, pos->x);
  SLIST_INSERT_HEAD (&money_list, m, moneyp);
}

void
money_draw (void)
{
  struct money  *m;

  SLIST_FOREACH (m, &money_list, moneyp)
    gfx_show_sprite (SP_MONEY, &m->pos);
}

void
money_check_at (const struct coord *hero_pos)
{
  struct money *m;

  SLIST_FOREACH (m, &money_list, moneyp)
    {
      if (coord_equal (&m->pos, hero_pos))
        {
          SLIST_REMOVE (&money_list, m, money, moneyp);
          game_score_inc ();
          return;
        }
    }
}

unsigned
money_all_collected (void)
{
  return SLIST_EMPTY (&money_list);
}
