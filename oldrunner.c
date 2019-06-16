/*  $Id: oldrunner.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */
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

#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#include "oldrunner.h"

/*
 * Oldrunner is a remake of the Loadrunner game from Broderbund, and one of the
 * first computer games my father offered me. I wrote this in memory of him.
 *
 *      Jean-Francois Culot
 *    1952-Sep-30 - 2009-Oct-20
 */
int
main (int argc, char **argv)
{
  const char *errstr;
  int ch, startlvl;

  startlvl = 0;
  while ((ch = getopt (argc, argv, "l:v")) != -1)
    {
      switch (ch)
        {
        case 'l':
          startlvl = (int)strtonum (optarg, 0, INT_MAX, &errstr) - 1;
          if (errstr)
            {
              fprintf (stderr, "Bad level number: %s\n", errstr);
              return 1;
            }
          break;
        case 'v':
          fputs ("oldrunner-" VERSION "\n", stdout);
          return 0;
        default:
          goto err;
        }
    }
  argc -= optind;
  argv += optind;
  if (argc >= 1)
    {
    err:
      fputs ("usage: oldrunner [-v] [-l<num>]\n", stderr);
      return 0;
    }

  game_init (startlvl);
  usr_input ();
  game_end ();

  return 0;
}
