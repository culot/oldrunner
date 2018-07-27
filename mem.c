/*  $Id: mem.c,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */

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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "oldrunner.h"

void *
xmalloc (size_t size)
{
  void *p;
  
  EXIT_IF (size == 0, "xmalloc: zero size");
  p = malloc (size);
  EXIT_IF (p == 0, "xmalloc: out of memory");

  return p;
}

void *
xcalloc (size_t nmemb, size_t size)
{
  void *p;

  EXIT_IF (nmemb == 0 || size == 0, "xcalloc: zero size");
  EXIT_IF (SIZE_MAX / nmemb < size, "xcalloc: overflow");
  p = calloc (nmemb, size);
  EXIT_IF (p == 0, "xcalloc: out of memory");

  return p;
}

void *
xrealloc (void *ptr, size_t nmemb, size_t size)
{
  void *new_ptr;
  size_t new_size;

  new_size = nmemb * size;
  EXIT_IF (new_size == 0, "xrealloc: zero size");
  EXIT_IF (SIZE_MAX / nmemb < size, "xrealloc: overflow");
  new_ptr = realloc (ptr, new_size);
  EXIT_IF (new_ptr == 0, "xrealloc: out of memory");

  return new_ptr;
}

char *
xstrdup (const char *str)
{
  size_t len;
  char *cp;

  len = strlen (str) + 1;
  cp = xmalloc (len);

  return strncpy (cp, str, len);
}

void
xfree (void *p)
{
  EXIT_IF (p == 0, "xfree: null pointer");
  free (p);
}
