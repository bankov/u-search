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

#ifndef SERVER_THREADOBJECT_H_
#define SERVER_THREADOBJECT_H_

#include <pthread.h>
#include <signal.h>

class ThreadObject {
    public:
        ThreadObject();

        virtual ~ThreadObject();

        // Inline functions
        int get_lasterrno() { return lasterrno; }
        bool is_run() { return run; }

        // Start thread and return 0 if start successfull; on error it return
        // error code and set this->lasterrno in error number
        int Start();

        // Join to thread, and return join status (not 0 on error) and set
        // this->lasterrno in error number
        int Join();

        // Send cancel signal to thread using pthread_cancel() function, and
        // return 0 if success; on error it return error number and set
        // this->lasterrno in error number
        int Cancel();

        // Mark thread identifier as detached (for more information see
        // pthread_detach). Function return 0 on success; on error it return
        // error number and set this->lasterrno in error number
        int Detach();

        // Send signal to thread. On success 0 will returned; on error, number
        // of error will returned and set this->lasterrno in error number
        int Kill(int signal);

    protected:
        // Inline functions
        void set_lasterrno(int errno_);

    private:
        // Variable for storage last error number
        int lasterrno;

        // Variable for storage thread id
        pthread_t thread_id;

        // Variable for storage pointer on thread returned value
        void *thread_retval;

        // Variable for storage attriutes for starting thread
        pthread_attr_t thread_attributes;

        bool run;

        // Function which called by pthread_create function. threadArg it's
        // pointer on called class. Then it call pthread_create with
        // start_routine as this->ThreadRoutine and arg as this
        static void *ThreadStarter(void *self);

        // Thread function. This function must designed for safely frees
        // resources which is allocated in thread and avialible only from them,
        // use pthread_cleanup_push(), pthread_cleanup_pop() and
        // pthread_setcancel_state for this, when it recive cancle signal
        // via pthread_cancel
        virtual void *ThreadRoutine() = 0;
};

#endif  // SERVER_THREADOBJECT_H_
