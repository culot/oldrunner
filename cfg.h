/*  $Id: cfg.h,v 1.1.1.1 2010/07/17 17:30:32 culot Exp $  */

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

#define CTRLVAL                 0x1F
#define CTRL(x)                 ((x) & CTRLVAL)

/*
 * User-definable variables are defined below.
 * Update configuration to suit your needs and then recompile.
 */

/* User keys. */
#define ORKEY_MOVE_RIGHT     'l'
#define ORKEY_MOVE_LEFT      'h'
#define ORKEY_MOVE_DOWN      'j'
#define ORKEY_MOVE_UP        'k'
#define ORKEY_DIG_LEFT       's'
#define ORKEY_DIG_RIGHT      'd'
#define ORKEY_LEVEL_NEXT     CTRL('n')
#define ORKEY_LEVEL_PREV     CTRL('p')
#define ORKEY_SUICIDE        CTRL('g')
#define ORKEY_EXIT           CTRL('d')

/* To set game speed (timeout in microseconds). */
#define OLDRUNNER_TIMEOUT    80000

/*
 * If game levels files are installed in a system-wide location, set the
 * LEVELS_PATH symbol to the appropriate path, such as:
 *
 *   #define LEVELS_PATH     "/usr/local/share/games/oldrunner/levels"
 */
