/*  $Id: io.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */
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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "oldrunner.h"

FILE *
io_fopen (const char *path)
{
  FILE *f;

  if (!(f = fopen (path, "r")))
    {
      ERROR_MSG ("Could not open game file \"%s\": %s",
                 path, strerror (errno));
      return 0;
    }

  return f;
}

static char *line;

void
io_fclose (FILE *f)
{
  (void)fclose (f);
  if (line)
    xfree (line);
  line = 0;
}

char *
io_getln (FILE *f)
{
  size_t len;
  char *buf;

  line = 0;
  if (!(buf = fgetln (f, &len)))
    return 0;
  if (buf[len - 1] == '\n')
    buf[len - 1] = '\0';
  else
    {
      /* EOF without EOL, copy and add the NUL */
      line = xrealloc (line, 1, len + 1);
      memcpy (line, buf, len);
      line[len] = '\0';
      buf = line;
    }

  return buf;
}
