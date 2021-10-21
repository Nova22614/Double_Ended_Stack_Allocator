/**
* Exercise: "Growing DoubleEndedStackAllocator with Canaries (VMEM)"
* Group members: Irena Ankerl (gs20m002), Maximilian Bauer (gs20m003), Nicolas Wolf (gs20m011)
**/
//#define NDEBUG

#include "stdio.h"
#include <Windows.h>
#include <iostream>
#include <assert.h>

namespace Tests
{
	void Test_Case_Success(const char* name, bool passed)
	{
		if (passed)
		{
			printf("[%s] passed the test!\n", name);
		}
		else
		{
			printf("[%s] failed the test!\n", name);
		}
	}

	void Test_Case_Failure(const char* name, bool passed)
	{
		if (!passed)
		{
			printf("[%s] passed the test!\n", name);
		}
		else
		{
			printf("[%s] failed the test!\n", name);
		}
	}

	/**
	* Example of how a test case can look like. The test cases in the end will check for
	* allocation success, proper alignment, overlaps and similar situations. This is an
	* example so you can already try to cover all cases you judge as being important by
	* yourselves.
	**/
	template<class A>
	bool VerifyAllocationSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem = allocator.Allocate(size, alignment);

		if (mem != nullptr)
		{
			printf("[AllocationSuccess]: Allocator returned no nullptr!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyAllocationBackSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem = allocator.AllocateBack(size, alignment);

		if (mem != nullptr)
		{
			printf("[AllocationBackSuccess]: Allocator returned no nullptr!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyFreeSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem = allocator.Allocate(size, alignment);
		allocator.Free(mem);
		void* mem2 = allocator.Allocate(size, alignment);

		if (mem == mem2)
		{
			printf("[FreeSuccess]: Pointer is the same before and after freeing!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyFreeBackSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem = allocator.AllocateBack(size, alignment);
		allocator.FreeBack(mem);
		void* mem2 = allocator.AllocateBack(size, alignment);

		if (mem == mem2)
		{
			printf("[FreeBackSuccess]: Pointer is the same before and after freeing!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyResetSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* memFront1 = allocator.Allocate(size, alignment);
		void* memBack1 = allocator.AllocateBack(size, alignment);
		allocator.Allocate(size, alignment);
		allocator.Allocate(size, alignment);
		allocator.Allocate(size, alignment);
		allocator.AllocateBack(size, alignment);
		allocator.AllocateBack(size, alignment);
		allocator.AllocateBack(size, alignment);
		allocator.Reset();
		void* memFront2 = allocator.Allocate(size, alignment);
		void* memBack2 = allocator.AllocateBack(size, alignment);


		if (memBack1 == memBack2 && memFront1 == memFront2)
		{
			printf("[ResetSuccess]: Pointers are the same as in the beginning after Reset()!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyMultipleAllocationSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.Allocate(size, alignment);
		void* mem2 = allocator.Allocate(size, alignment);

		if (mem1 != mem2)
		{
			printf("[MultipleAllocationSuccess]: Allocator returned two different pointers!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyMultipleAllocationBackSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.AllocateBack(size, alignment);
		void* mem2 = allocator.AllocateBack(size, alignment);

		if (mem1 != mem2)
		{
			printf("[MultipleAllocationBackSuccess]: Allocator returned two different pointers!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyMultipleFreeSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.Allocate(size, alignment);
		void* mem2 = allocator.Allocate(size, alignment);
		allocator.Free(mem2);
		allocator.Free(mem1);
		void* mem3 = allocator.Allocate(size, alignment);

		if (mem1 == mem3)
		{
			printf("[MultipleFreeSuccess]: Allocator returned the same pointers before and after freeing!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyMultipleFreeBackSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.AllocateBack(size, alignment);
		void* mem2 = allocator.AllocateBack(size, alignment);
		allocator.FreeBack(mem2);
		allocator.FreeBack(mem1);
		void* mem3 = allocator.AllocateBack(size, alignment);

		if (mem1 == mem3)
		{
			printf("[MultipleFreeBackSuccess]: Allocator returned the same pointers before and after freeing!\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyAlignmentBackSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.AllocateBack(size, alignment);
		void* mem2 = allocator.AllocateBack(size, alignment);

		if (reinterpret_cast<uintptr_t>(mem1) % alignment == 0 && reinterpret_cast<uintptr_t>(mem2) % alignment == 0)
		{
			printf("[AlignmentBackSuccess]: Pointers are aligned\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyAlignmentSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.Allocate(size, alignment);
		void* mem2 = allocator.Allocate(size, alignment);

		if (reinterpret_cast<uintptr_t>(mem1) % alignment == 0 && reinterpret_cast<uintptr_t>(mem2) % alignment == 0)
		{
			printf("[AlignmentSuccess]: Pointers are aligned\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyOverflowSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.Allocate(size, alignment);
		void* mem2 = allocator.Allocate(size, alignment);
		void* mem3 = allocator.Allocate(size, alignment);
		void* mem4 = allocator.Allocate(size, alignment);

		if (mem1 != nullptr && mem2 != nullptr && mem3 != nullptr && mem4 == nullptr)
		{
			printf("[OverflowSuccess]: Allocator returns nullptr when too much is allocated\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyOverflowBackSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.AllocateBack(size, alignment);
		void* mem2 = allocator.AllocateBack(size, alignment);
		void* mem3 = allocator.AllocateBack(size, alignment);
		void* mem4 = allocator.AllocateBack(size, alignment);

		if (mem1 != nullptr && mem2 != nullptr && mem3 != nullptr && mem4 == nullptr)
		{
			printf("[OverflowBackSuccess]: Allocator returns nullptr when two much is allocated\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyWriteReadSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.Allocate(size, alignment);
		*reinterpret_cast<uint32_t*>(mem1) = 123456;
		uint32_t data1 = *reinterpret_cast<uint32_t*>(mem1);
		void* mem2 = allocator.Allocate(size, alignment);
		uint32_t data2 = *reinterpret_cast<uint32_t*>(mem1);
		allocator.Free(mem2);
		uint32_t data3 = *reinterpret_cast<uint32_t*>(mem1);

		if (data1 == data2 && data2 == data3)
		{
			printf("[WriteReadSuccess]: Written data can be read\n");
			return true;
		}

		return false;
	}

	template<class A>
	bool VerifyWriteReadBackSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem1 = allocator.AllocateBack(size, alignment);
		*reinterpret_cast<uint32_t*>(mem1) = 123456;
		uint32_t data1 = *reinterpret_cast<uint32_t*>(mem1);
		void* mem2 = allocator.AllocateBack(size, alignment);
		uint32_t data2 = *reinterpret_cast<uint32_t*>(mem1);
		allocator.FreeBack(mem2);
		uint32_t data3 = *reinterpret_cast<uint32_t*>(mem1);

		if (data1 == data2 && data2 == data3)
		{
			printf("[WriteReadBackSuccess]: Written data can be read\n");
			return true;
		}

		return false;
	}
}

// Assignment functionality tests are going to be included here 

#define WITH_DEBUG_CANARIES 1
/**
* You work on your DoubleEndedStackAllocator. Stick to the provided interface, this is
* necessary for testing your assignment in the end. Don't remove or rename the public
* interface of the allocator. Also don't add any additional initialization code, the
* allocator needs to work after it was created and its constructor was called. You can
* add additional public functions but those should only be used for your own testing.
**/
class DoubleEndedStackAllocator
{
private:
	DWORD _pageSize;

	//Meta Data Struct + Meta Data Size
	struct MetaData
	{
		MetaData(size_t size, uintptr_t lastDataBlock) :size(size), lastDataBlock(lastDataBlock) {}
		size_t size;
		uintptr_t lastDataBlock;
	};
	uint32_t META_SIZE = sizeof(MetaData);


	//Virtual Memory Size
	size_t _virtualMemorySize = 1073741824u; //1 Gi

	//Virtual Memory Creation Pointer
	void* _firstMemoryElement = 0;

	//Front Stack Pointer
	uintptr_t _begin = 0;
	uintptr_t _currentFrontAddress = 0;
	uintptr_t _frontStackEnd = 0;

	//Back Stack Pointer
	uintptr_t _end = 0;
	uintptr_t _currentBackAddress = 0;
	uintptr_t _backStackBegin = 0;

	//Canary Variables
	uint32_t CANARY = 0xB00BEE5; //Ghost Bees
	uint32_t CANARY_SIZE = sizeof(CANARY) * WITH_DEBUG_CANARIES; //0 if canaries are deactivated

public:
	DoubleEndedStackAllocator(size_t max_size)
	{
		//The size of the memory that we reserve is 1GiByte except for the case, when max_size is bigger than that.
		//1GiByte should be a size that won't easily be reached. It could still be bigger, because the reserved memory
		//is not touched until it is committed, so if someone wants to reserve more than that, they can set it with max_size.
		//That also supports the requested 4GiByte.
		if (max_size > _virtualMemorySize)
			_virtualMemorySize = max_size;


		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		_pageSize = system_info.dwPageSize; //page size can be different on different systems

		void* first_Address = VirtualAlloc(NULL, _virtualMemorySize, MEM_RESERVE, PAGE_READWRITE);

		if (!first_Address)
		{
			std::cout << "\033[31mFailed while Reserving Memory\033[0m" << std::endl;
			return;
		}

		_firstMemoryElement = first_Address;

		//Committing the first page of the front stack
		first_Address = VirtualAlloc(first_Address, _pageSize, MEM_COMMIT, PAGE_READWRITE);

		//If there is any error with committing the first page, then the constructor fails.
		//For safety reasons we release the reserved memory here.
		if (!first_Address)
		{
			VirtualFree(_firstMemoryElement, 0, MEM_RELEASE);
			std::cout << "\033[31mFailed while Commiting Front Page\033[0m" << std::endl;
			return;
		}

		//Set field variables for later use:
		//_begin always stays at the begin of the front stack
		//_currentFrontAddress always points to the last item of the front stack, or to _begin if the stack is empty
		//_frontStackEnd always points to the end of the currently reserved pages for the front stack  (if this point is reached, new pages need to be committed)
		_begin = reinterpret_cast<uintptr_t>(first_Address);
		_currentFrontAddress = _begin;
		_frontStackEnd = _begin + _pageSize;

		//Committing the first page of the back stack
		first_Address = VirtualAlloc(reinterpret_cast<void*>(_begin + _virtualMemorySize - _pageSize), _pageSize, MEM_COMMIT, PAGE_READWRITE);

		//If there is any error with committing the first page, then the constructor fails.
		//For safety reasons we release the reserved memory here.
		if (!first_Address)
		{
			VirtualFree(_firstMemoryElement, 0, MEM_RELEASE);
			std::cout << "\033[31mFailed while Commiting Back Page\033[0m" << std::endl;
			return;
		}

		//Set field variables for later use:
		//_end always stays at the begin of the back stack
		//_currentBackAddress always points to the last item of the back stack, or to _end if the stack is empty
		//_backStackBegin always points to the end of the currently reserved pages for the front stack (if this point is reached, new pages need to be committed)
		_backStackBegin = reinterpret_cast<uintptr_t>(first_Address);
		_end = _backStackBegin + _pageSize;
		_currentBackAddress = _end;
	}

	//disable copy and move to prevent undefined behaviour
	DoubleEndedStackAllocator(const DoubleEndedStackAllocator&) = delete;
	DoubleEndedStackAllocator& operator= (const DoubleEndedStackAllocator&) = delete;
	DoubleEndedStackAllocator(DoubleEndedStackAllocator&&) = delete;
	DoubleEndedStackAllocator& operator= (DoubleEndedStackAllocator&&) = delete;

	void* Allocate(size_t size, size_t alignment)
	{
		uintptr_t allocatingAddress = _currentFrontAddress;

		//check if alignment is power of 2
		if ((alignment & (alignment - 1)) != 0)
		{
			assert(!"The Alignment for the Allocation is not Valid");

			std::cout << "\033[33mThe Alignment for the Allocation is not Valid\033[0m" << std::endl;
			return nullptr;
		}

		//if the _allocatingAddress is at _begin there is no data and canary that we need to jump over.
		//otherwise we position the pointer after the data and canary of the previous stack item.
		if (allocatingAddress != _begin)
		{
			MetaData* metaData = getMetaData(allocatingAddress);
			allocatingAddress = allocatingAddress + metaData->size + CANARY_SIZE;
		}

		//make space for canary and meta data
		allocatingAddress = allocatingAddress + sizeof(MetaData) + CANARY_SIZE;

		//align up
		allocatingAddress = allocatingAddress + alignment - allocatingAddress % alignment;

		//before writing, check if we would write into back stack
		if (allocatingAddress + size + CANARY_SIZE > _backStackBegin)
		{
			assert(!"Too Little Space for this Allocation");

			std::cout << "\033[33mToo Little Space for this Allocation\033[0m" << std::endl;
			return nullptr;
		}

		//before writing, check if we pass the currently committed page limit
		//if we pass it we need to commit as many new pages as needed, to fit data, canaries and meta data inside
		if (allocatingAddress + size + CANARY_SIZE > _frontStackEnd)
		{
			uintptr_t sizeDifference = (allocatingAddress + size + CANARY_SIZE) - _frontStackEnd;
			size_t pagesToCommit = ((size_t)(sizeDifference / _pageSize) + 1) * _pageSize;

			void* debugAlloc = VirtualAlloc(reinterpret_cast<void*>(_frontStackEnd), pagesToCommit, MEM_COMMIT, PAGE_READWRITE);

			if (!debugAlloc)
			{
				assert(!"Virtual Memmory failed to Commit a new Page.");

				std::cout << "\033[31mVirtual Memmory failed to Commit a new Page.\033[0m" << std::endl;
				return nullptr;
			}

			_frontStackEnd += pagesToCommit;
		}

		addMetaData(allocatingAddress, _currentFrontAddress, size);

#if WITH_DEBUG_CANARIES
		addCanary(allocatingAddress - META_SIZE - CANARY_SIZE);
		addCanary(allocatingAddress + size);
#endif

		//after writing canaries and meta data we finally set the _currentFrontAddress to the new last item of the stack
		_currentFrontAddress = allocatingAddress;

		return reinterpret_cast<void*>(_currentFrontAddress);
	}

	void* AllocateBack(size_t size, size_t alignment)
	{
		uintptr_t allocatingAddress = _currentBackAddress;

		//check if alignment is power of 2
		if ((alignment & (alignment - 1)) != 0)
		{
			assert(!"The Alignment for the Allocation is not Valid");

			std::cout << "\033[33mThe Alignment for the Allocation is not Valid\033[0m" << std::endl;
			return nullptr;
		}

		//if the _allocatingAddress is at _end there is no canary and meta data that we need to jump over.
		//otherwise we position the pointer after the meta data and canary of the previous stack item.
		if (allocatingAddress != _end)
		{
			MetaData* metaData = getMetaData(allocatingAddress);
			allocatingAddress = allocatingAddress - CANARY_SIZE - sizeof(MetaData);
		}

		//make space for canary and data
		allocatingAddress = allocatingAddress - CANARY_SIZE - size;

		//align down
		allocatingAddress = allocatingAddress - allocatingAddress % alignment;

		//before writing, check if we would write into front stack
		if (allocatingAddress - sizeof(MetaData) - CANARY_SIZE < _frontStackEnd)
		{
			assert(!"Too Little Space for this Allocation");

			std::cout << "\033[33mToo Little Space for this Allocation\033[0m" << std::endl;
			return nullptr;
		}

		//before writing, check if we pass the currently committed page limit
		//if we pass it we need to commit as many new pages as needed, to fit data, canaries and meta data inside
		if (allocatingAddress - sizeof(MetaData) - CANARY_SIZE < _backStackBegin)
		{
			uintptr_t sizeDifference = _backStackBegin - (allocatingAddress - sizeof(MetaData) - CANARY_SIZE);
			size_t pagesToCommit = ((size_t)(sizeDifference / _pageSize) + 1) * _pageSize;

			void* debugAlloc = VirtualAlloc(reinterpret_cast<void*>(_backStackBegin - pagesToCommit), pagesToCommit, MEM_COMMIT, PAGE_READWRITE);

			if (!debugAlloc)
			{
				assert(!"Vitual Memmory failed to Commit a new Page.");

				std::cout << "\033[31mVitual Memmory failed to Commit a new Page.\033[0m" << std::endl;
				return nullptr;
			}

			_backStackBegin -= pagesToCommit;
		}
		addMetaData(allocatingAddress, _currentBackAddress, size);

#if WITH_DEBUG_CANARIES
		addCanary(allocatingAddress - META_SIZE - CANARY_SIZE);
		addCanary(allocatingAddress + size);
#endif

		//after writing canaries and meta data we finally set the _currentBackAddress to the new last item of the stack
		_currentBackAddress = allocatingAddress;

		return reinterpret_cast<void*>(_currentBackAddress);
	}

	void Free(void* memory)
	{
		uintptr_t pointerToFree = reinterpret_cast<uintptr_t>(memory);

		//check if stack is empty
		if (_begin != _currentFrontAddress)
		{
			//LIFO check
			if (pointerToFree != _currentFrontAddress)
			{
				assert(!"Pointer to Free does not Point to last Entry");

				std::cout << "\033[31mGiven Memory was not Freed. The Given Pointer was not Pointing to the last Entry\033[0m" << std::endl;
				return;
			}

			MetaData* metaData = getMetaData(pointerToFree);
#if WITH_DEBUG_CANARIES
			//metaData and canary check
			if (metaData == nullptr)
			{
				assert(!"The MetaData of the Address " + pointerToFree + " is overwritten");
				std::cout << "The MetaData of the Address " << pointerToFree << " is overwritten" << std::endl;
				return;
			}
			checkForOverwrite(pointerToFree, metaData->size);
#endif
			//after freeing _currentFrontAddress needs to point to the new last item of the stack
			_currentFrontAddress = metaData->lastDataBlock;


			//if the stack became smaller, we check if we can decommit some of the pages.
			//if _currentFrontAddress is at _begin and there is currently only 1 page, we do nothing.
			//otherwise we reduce the pages to fit the whole stack inside.
			//or we only keep 1 page if the stack is now empty.
			if (_currentFrontAddress != _begin)
			{
				metaData = getMetaData(_currentFrontAddress);

				if (_currentFrontAddress + metaData->size + CANARY_SIZE < _frontStackEnd - _pageSize)
				{
					uint32_t pagesToDecommit = (_frontStackEnd - (_currentFrontAddress + metaData->size + CANARY_SIZE)) / _pageSize;

					VirtualFree(reinterpret_cast<void*>(_frontStackEnd - (pagesToDecommit * _pageSize)), pagesToDecommit * _pageSize, MEM_DECOMMIT);
					_frontStackEnd = _frontStackEnd - (pagesToDecommit * _pageSize);
				}
			}
			else if ((_frontStackEnd - _begin) / _pageSize > 1)
			{
				uint32_t pagesToDecommit = (_frontStackEnd - (_begin + _pageSize)) / _pageSize;
				VirtualFree(reinterpret_cast<void*>(_begin + _pageSize), pagesToDecommit * _pageSize, MEM_DECOMMIT);
				_frontStackEnd = _begin + _pageSize;
			}
		}
	}

	void FreeBack(void* memory)
	{
		uintptr_t pointerToFree = reinterpret_cast<uintptr_t>(memory);

		//check if stack is empty
		if (_end != _currentBackAddress)
		{
			//LIFO check
			if (pointerToFree != _currentBackAddress)
			{
				assert(!"Pointer to Free does not Point to last Entry");

				std::cout << "\033[31mGiven Memory was not Freed. The Given Pointer was not Pointing to the last Entry\033[0m" << std::endl;
				return;
			}

			MetaData* metaData = getMetaData(pointerToFree);

#if WITH_DEBUG_CANARIES
			//metaData and canary check
			if (metaData == nullptr)
			{
				assert(!"The MetaData of the Address " + pointerToFree + " is overwritten");
				std::cout << "The MetaData of the Address " << pointerToFree << " is overwritten" << std::endl;
				return;
			}
			checkForOverwrite(pointerToFree, metaData->size);
#endif
			//after freeing _currentBackAddress needs to point to the new last item of the stack
			_currentBackAddress = metaData->lastDataBlock;


			//if the stack became smaller, we check if we can decommit some of the pages.
			//if the stack is empty and there is currently only 1 page, we do nothing.
			//otherwise we reduce the pages to fit the whole stack inside.
			//or we only keep 1 page if the stack is now empty and there were more than 1 page.
			if (_currentBackAddress != _end)
			{
				if (_currentBackAddress - CANARY_SIZE - META_SIZE > _backStackBegin + _pageSize)
				{
					uint32_t pagesToDecommit = ((_currentBackAddress - CANARY_SIZE - META_SIZE) - _backStackBegin) / _pageSize;

					VirtualFree(reinterpret_cast<void*>(_backStackBegin), pagesToDecommit * _pageSize, MEM_DECOMMIT);
					_backStackBegin = _backStackBegin + pagesToDecommit * _pageSize;
				}
			}
			else if ((_end - _backStackBegin) / _pageSize > 1)
			{
				uint32_t pagesToDecommit = ((_end - _pageSize) - _backStackBegin) / _pageSize;

				VirtualFree(reinterpret_cast<void*>(_backStackBegin), pagesToDecommit * _pageSize, MEM_DECOMMIT);
				_backStackBegin = _end - _pageSize;
			}
		}
	}

	void Reset(void)
	{
		//free every item of the front stack. this way the canaries are all checked
		while (_begin != _currentFrontAddress)
		{
			Free(reinterpret_cast<void*>(_currentFrontAddress));
		}

		//free every item of the back stack. this way the canaries are all checked
		while (_end != _currentBackAddress)
		{
			FreeBack(reinterpret_cast<void*>(_currentBackAddress));
		}
	}

	~DoubleEndedStackAllocator(void)
	{
		Reset();

		//after resetting we release the whole reserved memory
		VirtualFree(_firstMemoryElement, 0, MEM_RELEASE);
	}

private:
	MetaData* getMetaData(uintptr_t pointerToData)
	{
		return reinterpret_cast<MetaData*>(pointerToData - sizeof(MetaData));
	}

	void addMetaData(uintptr_t pointerToMetaDataPositon, uintptr_t pointerToLastDataBlock, size_t size)
	{
		*reinterpret_cast<MetaData*>(pointerToMetaDataPositon - META_SIZE) = MetaData(size, pointerToLastDataBlock);
	}

	void addCanary(uintptr_t pointerToCanaryPosition)
	{
		*reinterpret_cast<uint32_t*>(pointerToCanaryPosition) = CANARY;
	}

	void checkForOverwrite(uintptr_t addressToCheck, size_t size)
	{
		uintptr_t canaryAddress = addressToCheck - sizeof(MetaData) - CANARY_SIZE;

		//check canary in front of data and meta data
		if (*reinterpret_cast<uint32_t*>(canaryAddress) != CANARY)
		{
			assert(!"The Front Canary of the Address " + addressToCheck + " is incomplete");
			std::cout << "The Front Canary of the Address " << addressToCheck << " is incomplete" << std::endl;
		}

		canaryAddress = addressToCheck + size;

		//check canary after data
		if (*reinterpret_cast<uint32_t*>(canaryAddress) != CANARY)
		{
			assert(!"The Back Canary of the Address " + addressToCheck + " is incomplete");
			std::cout << "The Back Canary of the Address " << addressToCheck << " is incomplete" << std::endl;
		}
	}
};


int main()
{
	// You can add your own tests here, I will call my tests at then end with a fresh instance of your allocator and a specific max_size
	{
		// You can remove this, just showcasing how the test functions can be used
		DoubleEndedStackAllocator allocator(1048576u);
		Tests::VerifyAllocationSuccess(allocator, 32, 2);
		Tests::VerifyAllocationBackSuccess(allocator, 32, 2);
		allocator.Reset();
		Tests::VerifyFreeSuccess(allocator, 32, 2);
		Tests::VerifyFreeBackSuccess(allocator, 32, 2);
		allocator.Reset();
		Tests::VerifyResetSuccess(allocator, 32, 2);
		allocator.Reset();
		Tests::VerifyMultipleAllocationSuccess(allocator, 32, 2);
		Tests::VerifyMultipleAllocationBackSuccess(allocator, 32, 2);
		allocator.Reset();
		Tests::VerifyMultipleFreeSuccess(allocator, 32, 2);
		Tests::VerifyMultipleFreeBackSuccess(allocator, 32, 2);
		allocator.Reset();
		Tests::VerifyAlignmentSuccess(allocator, 32, 128);
		Tests::VerifyAlignmentBackSuccess(allocator, 32, 128);
		allocator.Reset();
		Tests::VerifyOverflowSuccess(allocator, 1073741824u / 4, 2);
		allocator.Reset();
		Tests::VerifyOverflowBackSuccess(allocator, 1073741824u / 4, 2);
		allocator.Reset();
		Tests::VerifyWriteReadBackSuccess(allocator, 32, 2);
		allocator.Reset();
		Tests::VerifyWriteReadSuccess(allocator, 32, 2);
		allocator.Reset();


		//Canary test at front stack: should break the end Canary of mem1 and the front Canary of mem2
		/*void* mem1 = allocator.Allocate(sizeof(uint32_t), 2);
		void* mem2 = allocator.Allocate(sizeof(uint32_t), 2);
		uint32_t* testData = reinterpret_cast<uint32_t*>(mem1);
		testData[0] = 1;
		testData[1] = 2;
		testData[2] = 3;
		allocator.Free(mem2);
		allocator.Free(mem1);*/

		//Canary test at back stack: should break the front Canary of mem1 and the back Canary of mem2
		/*void* mem1 = allocator.AllocateBack(sizeof(uint32_t), 2);
		void* mem2 = allocator.AllocateBack(sizeof(uint32_t), 2);
		uint32_t* testData = reinterpret_cast<uint32_t*>(mem2);
		testData[0] = 1;
		testData[1] = 2;
		testData[2] = 3;
		allocator.FreeBack(mem2);
		allocator.FreeBack(mem1);*/
	}

	// You can do whatever you want here in the main function

	// Here the assignment tests will happen - it will test basic allocator functionality. 
	{

	}
}