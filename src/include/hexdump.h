/* * Royal Rand - Crypto utility for Royal Bingo
 * Copyright (C) 2021-2022 Murilo Augusto <murilo@bad1337.com>
 *
 * This file is part of Royal Rand.
 *
 * Royal Rand is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Royal Rand is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Royal Rand.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HEXDUMP_H
#define _HEXDUMP_H

void hexdump(const char * desc, const void * addr, const int len, int perLine);

#endif /* _HEXDUMP_H */
