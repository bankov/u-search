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

#include <pthread.h>
#include <signal.h>

#include "threadobject.h"

ThreadObject::ThreadObject() {
  this->lasterrno = 0;
  this->thread_retval = NULL;
  this->thread_id = 0;
  this->run = false;
}

ThreadObject::~ThreadObject() {
  this->Cancel();
  this->Detach();
}

void *ThreadObject::ThreadStarter(void *threadArgs) {
  ThreadObject *self = (ThreadObject*)threadArgs;
  return self->ThreadRoutine();
}

int ThreadObject::Start() {
  if (this->run == true) {
    return 0;
  }

  this->run = true;

  // Initalize default thread attribute object with default values
  int attributeSetStatus = pthread_attr_init(&this->thread_attributes);
  if (attributeSetStatus != 0) {
    this->set_lasterrno(attributeSetStatus);
    this->run = false;
    return attributeSetStatus;
  }

  // Create thread
  int createStatus = pthread_create(&this->thread_id, &this->thread_attributes,
                   ThreadObject::ThreadStarter, this);
  if (createStatus != 0) {
    this->set_lasterrno(createStatus);
    this->run = false;
    return createStatus;
  }

  return 0;
}

int ThreadObject::Join() {
  if (!this->is_run()) {
    this->set_lasterrno(-1);
    return -1;
  }

  int joinStatus = pthread_join(this->thread_id, &this->thread_retval);
  if (joinStatus != 0) {
    this->set_lasterrno(joinStatus);
    return joinStatus;
  }

  this->run = false;
  return 0;
}

int ThreadObject::Cancel() {
  if (this->thread_id == 0 || !this->is_run())
    return 0;

  int cancelStatus = pthread_cancel(this->thread_id);
  if (cancelStatus != 0) {
    this->set_lasterrno(cancelStatus);
    return cancelStatus;
  }

  return 0;
}

int ThreadObject::Detach() {
  if (this->thread_id == 0 || !this->is_run())
    return 0;

  int detachStatus = pthread_detach(this->thread_id);
  if (detachStatus != 0) {
    this->set_lasterrno(detachStatus);
    return detachStatus;
  }

  return 0;
}

int ThreadObject::Kill(int signal) {
  int sendSignalStatus = pthread_kill(this->thread_id, signal);
  if (sendSignalStatus != 0) {
    this->set_lasterrno(sendSignalStatus);
    return sendSignalStatus;
  }

  return 0;
}

void ThreadObject::set_lasterrno(int errno_) {
  this->lasterrno = errno_;
}

