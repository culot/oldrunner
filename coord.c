/*  $Id: coord.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */

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

void
coord_copy (const struct coord *orig, struct coord *dest)
{
  dest->y = orig->y;
  dest->x = orig->x;
}

void
coord_below (const struct coord *orig, struct coord *dest)
{
  dest->y = orig->y + 1;
  dest->x = orig->x;
}

void
coord_above (const struct coord *orig, struct coord *dest)
{
  dest->y = orig->y - 1;
  dest->x = orig->x;
}

enum move
coord_opposite_dir (enum move dir)
{
  switch (dir)
    {
    case MOV_RIGHT:
      return MOV_LEFT;
    case MOV_LEFT:
      return MOV_RIGHT;
    case MOV_UP:
      return MOV_DOWN;
    case MOV_DOWN:
      return MOV_UP;
    default:
      return MOV_NONE;
    }
}

void
coord_compute (const struct coord *orig, enum move move, struct coord *dest)
{
  coord_copy (orig, dest);
  switch (move)
    {
    case MOV_UP:
      dest->y--;
      break;
    case MOV_DOWN:
    case MOV_FALL:      
      dest->y++;
      break;      
    case MOV_LEFT:
      dest->x--;
      break;
    case MOV_RIGHT:
      dest->x++;
      break;
    default:
      dest->y = dest->x = 0;
      /* NOTREACHED */
    }
}

void
coord_set_yx (struct coord *coord, int y, int x)
{
  coord->y = y;
  coord->x = x;
}

unsigned
coord_equal (const struct coord *p1, const struct coord *p2)
{
  return p1->x == p2->x && p1->y == p2->y ? 1 : 0;
}

void
coord_diff (const struct coord *p1, const struct coord *p2, struct coord *diff)
{
  diff->y = p1->y - p2->y;
  diff->x = p1->x - p2->x;
}
