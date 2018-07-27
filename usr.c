/*  $Id: usr.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */

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

#define ORKEY_NONE  -1
#define USER_DELAY  OLDRUNNER_TIMEOUT

static struct usr_input  rec_input;

void
usr_reset_input (void)
{
  rec_input.key = ORKEY_NONE;
}

void
usr_input (void)
{
  struct timer user_timer;

  timer_get_time (&user_timer);
  usr_reset_input ();
  for (;;)
    {
      struct usr_input input;

      game_update ();
      gfx_get_input (&input);
      if (!timer_delay_elapsed (&user_timer, USER_DELAY))
        {
          if (input.key != ORKEY_NONE)
            rec_input.key = input.key;
          continue;
        }

      switch (rec_input.key)
        {
        case ORKEY_EXIT:
          return;

        case ORKEY_MOVE_RIGHT:
          hero_move (MOV_RIGHT);
          break;
        case ORKEY_MOVE_LEFT:
          hero_move (MOV_LEFT);
          break;
        case ORKEY_MOVE_UP:
          hero_move (MOV_UP);
          break;
        case ORKEY_MOVE_DOWN:
          hero_move (MOV_DOWN);
          break;

        case ORKEY_DIG_LEFT:
          hero_dig (MOV_LEFT);
          break;
        case ORKEY_DIG_RIGHT:
          hero_dig (MOV_RIGHT);
          break;
          
        case ORKEY_LEVEL_NEXT:
          lvl_load_next ();
          break;
        case ORKEY_LEVEL_PREV:
          lvl_load_prev ();
          break;

        case ORKEY_SUICIDE:
          hero_die ();
          break;
          
        default:
          /*
           * No action were performed by the user in the time interval.
           * Simulate gravity by requiring a fall by default.
           */
          hero_move (MOV_FALL);
          break;
        }

      timer_get_time (&user_timer);
    }
}
