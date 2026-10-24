/*
 *  loginwebpage - Login Page on web
 *  Copyright (C) 2026  Mehmet Lotfi
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SHA256_H
#define SHA256_H

#include "stddef.h"
#include "stdint.h"

#define SHA256_DIGEST_SIZE 32

typedef struct
{
	uint32_t state [8];
	uint64_t bit_len;
	uint8_t block [64];
	size_t block_len;
} sha256_ctx;

void sha256_init (sha256_ctx * ctx);
void sha256_update (sha256_ctx * ctx , const void * data , size_t len);
void sha256_final (sha256_ctx * ctx , uint8_t out [SHA256_DIGEST_SIZE]);

void sha256 (const void * data , size_t len , uint8_t out [SHA256_DIGEST_SIZE]);

#endif
