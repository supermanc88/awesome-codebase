
#include "ngx_shmtx.h"

// 实现一个nginx中的accept_mutex锁
// 该锁用于多个worker进程之间的竞争，防止多个worker进程同时accept新的连接
// 该锁的实现是基于共享内存

#include <unistd.h>
#include <sched.h>
#include <sys/mman.h>



#define NGX_OK          0
#define NGX_ERROR      -1
#define ngx_sched_yield()  sched_yield()
#define NGX_ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)

#define ngx_atomic_cmp_set(lock, old, set)                                    \
    __sync_bool_compare_and_swap(lock, old, set)

#define ngx_atomic_fetch_add(value, add)                                      \
    __sync_fetch_and_add(value, add)

#define ngx_memory_barrier()        __sync_synchronize()

#if ( __i386__ || __i386 || __amd64__ || __amd64 )
#define ngx_cpu_pause()             __asm__ ("pause")
#else
#define ngx_cpu_pause()
#endif

#if 0
// 当gcc不支持__sync_bool_compare_and_swap时，使用下面的代码

#if (NGX_SMP)
#define NGX_SMP_LOCK  "lock;"
#else
#define NGX_SMP_LOCK
#endif

/*
 * "cmpxchgq  r, [m]":
 *
 *     if (rax == [m]) {
 *         zf = 1;
 *         [m] = r;
 *     } else {
 *         zf = 0;
 *         rax = [m];
 *     }
 *
 *
 * The "r" is any register, %rax (%r0) - %r16.
 * The "=a" and "a" are the %rax register.
 * Although we can return result in any register, we use "a" because it is
 * used in cmpxchgq anyway.  The result is actually in %al but not in $rax,
 * however as the code is inlined gcc can test %al as well as %rax.
 *
 * The "cc" means that flags were changed.
 */
static inline ngx_atomic_uint_t
ngx_atomic_cmp_set(ngx_atomic_t *lock, ngx_atomic_uint_t old,
    ngx_atomic_uint_t set)
{
    u_char  res;

    __asm__ volatile (

         NGX_SMP_LOCK
    "    cmpxchgq  %3, %1;   "
    "    sete      %0;       "

    : "=a" (res) : "m" (*lock), "a" (old), "r" (set) : "cc", "memory");

    return res;
}

#endif


static void ngx_shmtx_wakeup(ngx_shmtx_t *mtx);

ngx_int_t
ngx_shm_alloc(ngx_shm_t *shm)
{
    shm->addr = (u_char *) mmap(NULL, shm->size,
                                PROT_READ|PROT_WRITE,
                                MAP_ANON|MAP_SHARED, -1, 0);

    if (shm->addr == MAP_FAILED) {
        return NGX_ERROR;
    }

    return NGX_OK;
}


void
ngx_shm_free(ngx_shm_t *shm)
{
    if (munmap((void *) shm->addr, shm->size) == -1) {
    }
}



int ngx_shmtx_create(ngx_shmtx_t *mtx, ngx_shmtx_sh_t *addr, u_char *name)
{
    mtx->lock = &addr->lock;

    if (mtx->spin == (ngx_uint_t) -1) {
        return NGX_OK;
    }

    mtx->spin = 2048;
    return NGX_OK;
}

void
ngx_shmtx_destroy(ngx_shmtx_t *mtx)
{
}

ngx_uint_t
ngx_shmtx_trylock(ngx_shmtx_t *mtx)
{
    return (*mtx->lock == 0 && ngx_atomic_cmp_set(mtx->lock, 0, getpid()));
}

void
ngx_shmtx_lock(ngx_shmtx_t *mtx)
{
    ngx_uint_t         i, n;

    ngx_int_t   ngx_ncpu;

    if (ngx_ncpu == 0) {
        ngx_ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    }

    for ( ;; ) {

        if (*mtx->lock == 0 && ngx_atomic_cmp_set(mtx->lock, 0, getpid())) {
            return;
        }

        if (ngx_ncpu > 1) {

            for (n = 1; n < mtx->spin; n <<= 1) {

                for (i = 0; i < n; i++) {
                    ngx_cpu_pause();
                }

                if (*mtx->lock == 0
                    && ngx_atomic_cmp_set(mtx->lock, 0, getpid()))
                {
                    return;
                }
            }
        }

        ngx_sched_yield();
    }
}

void
ngx_shmtx_unlock(ngx_shmtx_t *mtx)
{
    if (ngx_atomic_cmp_set(mtx->lock, getpid(), 0)) {
        ngx_shmtx_wakeup(mtx);
    }
}

ngx_uint_t
ngx_shmtx_force_unlock(ngx_shmtx_t *mtx, ngx_pid_t pid)
{

    if (ngx_atomic_cmp_set(mtx->lock, pid, 0)) {
        ngx_shmtx_wakeup(mtx);
        return 1;
    }

    return 0;
}

static void
ngx_shmtx_wakeup(ngx_shmtx_t *mtx)
{
}

#define SELF_TEST 0

#if (SELF_TEST)

int *shared = 0;

int main()
{
    ngx_shm_t shm = {0};
    ngx_shmtx_t mtx = {0};
    ngx_shmtx_sh_t *addr = NULL;

    shm.size = sizeof(ngx_shmtx_sh_t);
    ngx_shm_alloc(&shm);
    addr = (ngx_shmtx_sh_t *) shm.addr;


    ngx_shm_t shm2 = {0};
    shm2.size = sizeof(int);
    ngx_shm_alloc(&shm2);
    shared = (int *) shm2.addr;
    *shared = 0;


    ngx_shmtx_create(&mtx, addr, NULL);

    // ngx_shmtx_lock(&mtx);
    // ngx_shmtx_unlock(&mtx);

    int i = 0;
    for (i = 0; i < 10; i++) {
        // fork sub process
        if (fork() == 0) {
            while (1) {
                ngx_shmtx_lock(&mtx);
                (*shared)++;
                printf("pid: %d,shared: %d\n", getpid(), *shared);
                ngx_shmtx_unlock(&mtx);
                sleep(1);
            }
        }
    }

    wait(NULL);

    ngx_shmtx_destroy(&mtx);
    ngx_shm_free(&shm);

    return 0;
}

#endif