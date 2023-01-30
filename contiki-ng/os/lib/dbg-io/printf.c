/*
 * Copyright (c) 2009, Simon Berg
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "lib/dbg-io/dbg.h"

#include <stdio.h>
#include <string.h>
#include <strformat.h>
/*---------------------------------------------------------------------------*/
static strformat_result
write_str(void *user_data, const char *data, unsigned int len)
{
  if(len > 0) {
    dbg_send_bytes((unsigned char *)data, len);
  }
  return STRFORMAT_OK;
}
/*---------------------------------------------------------------------------*/
static strformat_context_t ctxt =
{
  write_str,
  NULL
};
/*---------------------------------------------------------------------------*/
int
printf(const char *fmt, ...)
{
  int res;
  va_list ap;
  va_start(ap, fmt);
  res = format_str_v(&ctxt, fmt, ap);
  va_end(ap);
  return res;
}

#if defined(CONTIKI_TARGET_COOJA) && !defined(__APPLE__)
extern int __wrap_printf(const char *fmt, ...) __attribute__((nonnull, alias("printf")));
#endif
/*---------------------------------------------------------------------------*/
