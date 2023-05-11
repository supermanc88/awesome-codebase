
#ifndef AWESOME_CODEBASE_NGX_SHMTX_H
#define AWESOME_CODEBASE_NGX_SHMTX_H

#include <sys/types.h>
#include <stdint.h>

typedef intptr_t        ngx_int_t;
typedef uintptr_t       ngx_uint_t;
typedef pid_t       ngx_pid_t;
typedef long                        ngx_atomic_int_t;
typedef unsigned long               ngx_atomic_uint_t;
typedef volatile ngx_atomic_uint_t  ngx_atomic_t;

typedef struct {
    ngx_atomic_t  *lock;
    ngx_uint_t     spin;
} ngx_shmtx_t;

typedef struct {
    ngx_atomic_t   lock;
} ngx_shmtx_sh_t;

typedef struct {
    u_char      *addr;
    size_t       size;
} ngx_shm_t;


ngx_int_t
ngx_shm_alloc(ngx_shm_t *shm);

void
ngx_shm_free(ngx_shm_t *shm);

int ngx_shmtx_create(ngx_shmtx_t *mtx, ngx_shmtx_sh_t *addr, u_char *name);

void
ngx_shmtx_destroy(ngx_shmtx_t *mtx);

ngx_uint_t
ngx_shmtx_trylock(ngx_shmtx_t *mtx);

void
ngx_shmtx_lock(ngx_shmtx_t *mtx);

void
ngx_shmtx_unlock(ngx_shmtx_t *mtx);

ngx_uint_t
ngx_shmtx_force_unlock(ngx_shmtx_t *mtx, ngx_pid_t pid);


#endif //AWESOME_CODEBASE_NGX_SHMTX_H
