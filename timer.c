/*  $Id: timer.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */

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

#include <sys/time.h>

#include "oldrunner.h"

#define  SEC2USEC  1000000

void
timer_get_time (struct timer *now)
{
  struct timeval t;

  (void)gettimeofday (&t, 0);
  now->time = (double)t.tv_sec * SEC2USEC + t.tv_usec;
}

void
timer_start (struct timer *t)
{
  timer_get_time (t);
}

void
timer_stop (struct timer *t)
{
  struct timer now;

  timer_get_time (&now);
  t->elapsed = timer_diff (t, &now);
  t->time = now.time;
}

double
timer_diff (const struct timer *t1, const struct timer *t2)
{
  return t1->time - t2->time;
}

double
timer_elapsed (const struct timer *t)
{
  return t->elapsed;
}

void
timer_set (const struct timer *orig, struct timer *dest)
{
  dest->time = orig->time;
}

void
timer_add (struct timer *t, double val)
{
  t->time += val;
}

unsigned
timer_delay_elapsed (struct timer *t, double delay)
{
  struct timer now;

  timer_get_time (&now);
  return timer_diff (&now, t) > delay ? 1 : 0;
}
