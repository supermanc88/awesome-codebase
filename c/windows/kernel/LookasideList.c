#include <wdm.h>
#include <ntddk.h>


typedef struct _MY_TEST_STRUCT {
	PVOID a;
	ULONG b;
} MY_TEST_STRUCT, *PMY_TEST_STRUCT;

PAGED_LOOKASIDE_LIST pagedLookasideList;
NPAGED_LOOKASIDE_LIST npagedLookasideList;

/**
* \brief PagedLookasideList 分页内存使用
* \return
*/
VOID PagedLookasideList()
{
	ExInitializePagedLookasideList(&pagedLookasideList,
									NULL,
									NULL,
									0,
									sizeof(MY_TEST_STRUCT),
									'tset',
									0);

	PMY_TEST_STRUCT test = ExAllocateFromPagedLookasideList( &pagedLookasideList );


	ExFreeToPagedLookasideList( &pagedLookasideList, test);

}



/**
* \brief NPagedLookasideList 非分页内存使用
* \return
*/
VOID NPagedLookasideList()
{
	ExInitializeNPagedLookasideList( &npagedLookasideList,
									 NULL,
									 NULL,
									 0,
									 sizeof(MY_TEST_STRUCT),
									 'setn',
									 0);
	PMY_TEST_STRUCT test = ExAllocateFromNPagedLookasideList( &npagedLookasideList );


	ExFreeToNPagedLookasideList( &npagedLookasideList, test );

}
