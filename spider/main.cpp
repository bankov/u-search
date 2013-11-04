/*
 * Copyright (c) 2013 Morgen Matvey, Yulugin Evgeny and others.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * The names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <sys/time.h>

#include <string>

#include "common.h"
#include "spider.h"

int main() {
  int hours;
  int minutes;

  // Read config from database
  std::string name, server, user, password;
  if (UNLIKELY(read_database_config(&name, &server, &user, &password,
                                    "../" DATABASE_CONFIG))) {
    MSS_DEBUG_MESSAGE("failed");
  }

  Spider spider("../etc/u-search/servers.dat", name, server, user, password);
  if (spider.get_error()) {
    MSS_DEBUG_ERROR("Spider", spider.get_error());
    exit(EXIT_FAILURE);
  }

  struct timeval start, end;

  // Looped indexation and print time of each iteration
  while (true) {
    gettimeofday(&start, NULL);

    spider.Run();

    gettimeofday(&end, NULL);

    hours = (end.tv_sec - start.tv_sec) / 3600;
    minutes = ((end.tv_sec - start.tv_sec) % 3600) / 60;

    printf("Iteration time: %d hourse, %d minutes.\n", hours, minutes);
  }

  return 0;
}
