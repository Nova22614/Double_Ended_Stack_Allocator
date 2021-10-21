/**
* Exercise: "DoubleEndedStackAllocator with Canaries" OR "Growing DoubleEndedStackAllocator with Canaries (VMEM)"
* Group members: NAME1 (gsXXXX), NAME2 (gsXXXX), NAME3 (gsXXXX)
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
	size_t _virtualMemorySize = 1073741824u;

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
	uint32_t CANARY = 0xB00BEE5;
	uint32_t CANARY_SIZE = sizeof(CANARY) * WITH_DEBUG_CANARIES;

public:
	DoubleEndedStackAllocator(size_t max_size)
	{
		if (max_size > _virtualMemorySize)
			_virtualMemorySize = max_size;


		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		_pageSize = system_info.dwPageSize;

		//std::cout << _pageSize << std::endl;
		//std::cout << CANARY_SIZE << std::endl;

		void* first_Address = VirtualAlloc(NULL, _virtualMemorySize, MEM_RESERVE, PAGE_READWRITE);

		if (!first_Address)
		{
			std::cout << "\033[31mFailed while Reserving Memory\033[0m" << std::endl;
			return;
		}

		_firstMemoryElement = first_Address;

		first_Address = VirtualAlloc(first_Address, _pageSize, MEM_COMMIT, PAGE_READWRITE);

		if (!first_Address)
		{
			VirtualFree(_firstMemoryElement, 0, MEM_RELEASE);
			std::cout << "\033[31mFailed while Commiting Front Page\033[0m" << std::endl;
			return;
		}

		_begin = reinterpret_cast<uintptr_t>(first_Address);
		_currentFrontAddress = _begin;
		_frontStackEnd = _begin + _pageSize;


		first_Address = VirtualAlloc(reinterpret_cast<void*>(_begin + _virtualMemorySize - _pageSize), _pageSize, MEM_COMMIT, PAGE_READWRITE);

		if (!first_Address)
		{
			VirtualFree(_firstMemoryElement, 0, MEM_RELEASE);
			std::cout << "\033[31mFailed while Commiting Back Page\033[0m" << std::endl;
			return;
		}

		_backStackBegin = reinterpret_cast<uintptr_t>(first_Address);
		_end = _backStackBegin + _pageSize;
		_currentBackAddress = _end;
	}

	void* Allocate(size_t size, size_t alignment)
	{
		uintptr_t allocatingAddress = _currentFrontAddress;

		if ((alignment & (alignment - 1)) != 0)
		{
			assert(!"The Alignment fir the Allocation is not Valid");

			std::cout << "\033[33mThe Alignment fir the Allocation is not Valid\033[0m" << std::endl;
			return nullptr;
		}

		if (allocatingAddress != _begin)
		{
			MetaData* metaData = getMetaData(allocatingAddress);
			allocatingAddress = allocatingAddress + metaData->size + CANARY_SIZE;
		}

		allocatingAddress = allocatingAddress + sizeof(MetaData) + CANARY_SIZE;

		allocatingAddress = allocatingAddress + alignment - allocatingAddress % alignment;

		if (allocatingAddress + size + CANARY_SIZE > _backStackBegin)
		{
			assert(!"Too Little Space for this Allocation");

			std::cout << "\033[33mToo Little Space for this Allocation\033[0m" << std::endl;
			return nullptr;
		}

		if (allocatingAddress + size + CANARY_SIZE > _frontStackEnd)
		{
			uintptr_t sizeDifference = (allocatingAddress + size + CANARY_SIZE) - _frontStackEnd;
			size_t pagesToCommit = ((size_t)(sizeDifference / _pageSize) + 1) * _pageSize;

			void* debugAlloc = VirtualAlloc(reinterpret_cast<void*>(_frontStackEnd), pagesToCommit, MEM_COMMIT, PAGE_READWRITE);

			if (!debugAlloc)
			{
				assert(!"Vitual Memmory failed to Commit a new Page.");

				std::cout << "\033[31mVitual Memmory failed to Commit a new Page.\033[0m" << std::endl;
				return nullptr;
			}

			_frontStackEnd += pagesToCommit;
		}

		addMetaData(allocatingAddress, _currentFrontAddress, size);

#if WITH_DEBUG_CANARIES
		addCanary(allocatingAddress - META_SIZE - CANARY_SIZE);
		addCanary(allocatingAddress + size);
#endif

		_currentFrontAddress = allocatingAddress;

		return reinterpret_cast<void*>(_currentFrontAddress);
	}

	void* AllocateBack(size_t size, size_t alignment)
	{
		uintptr_t allocatingAddress = _currentBackAddress;

		if ((alignment & (alignment - 1)) != 0)
		{
			assert(!"The Alignment fir the Allocation is not Valid");

			std::cout << "\033[33mThe Alignment fir the Allocation is not Valid\033[0m" << std::endl;
			return nullptr;
		}

		if (allocatingAddress != _end)
		{
			MetaData* metaData = getMetaData(allocatingAddress);
			allocatingAddress = allocatingAddress - CANARY_SIZE - sizeof(MetaData);
		}

		allocatingAddress = allocatingAddress - CANARY_SIZE - size;

		allocatingAddress = allocatingAddress - allocatingAddress % alignment;

		if (allocatingAddress - sizeof(MetaData) - CANARY_SIZE < _frontStackEnd)
		{
			assert(!"Too Little Space for this Allocation");

			std::cout << "\033[33mToo Little Space for this Allocation\033[0m" << std::endl;
			return nullptr;
		}

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

		_currentBackAddress = allocatingAddress;

		return reinterpret_cast<void*>(_currentBackAddress);
	}

	void Free(void* memory)
	{
		uintptr_t pointerToFree = reinterpret_cast<uintptr_t>(memory);

		if (_begin != _currentFrontAddress)
		{
			if (pointerToFree != _currentFrontAddress)
			{
				assert(!"Pointer to Free does not Point to last Entry");

				std::cout << "\033[31mGiven Memory was not Freed. The Given Pointer was not Pointing to the last Entry\033[0m" << std::endl;
				return;
			}

			MetaData* metaData = getMetaData(pointerToFree);
#if WITH_DEBUG_CANARIES
			if (metaData == nullptr)
			{
				assert(!"The MetaData of the Address " + pointerToFree + " is overwritten");
				std::cout << "The MetaData of the Address " << pointerToFree << " is overwritten" << std::endl;
				return;
			}
			checkForOverwrite(pointerToFree, metaData->size);
#endif
			_currentFrontAddress = metaData->lastDataBlock;
		}
	}

	void FreeBack(void* memory)
	{
		uintptr_t pointerToFree = reinterpret_cast<uintptr_t>(memory);

		if (_end != _currentBackAddress)
		{
			if (pointerToFree != _currentBackAddress)
			{
				assert(!"Pointer to Free does not Point to last Entry");

				std::cout << "\033[31mGiven Memory was not Freed. The Given Pointer was not Pointing to the last Entry\033[0m" << std::endl;
				return;
			}

			MetaData* metaData = getMetaData(pointerToFree);

#if WITH_DEBUG_CANARIES
			if (metaData == nullptr)
			{
				assert(!"The MetaData of the Address " + pointerToFree + " is overwritten");
				std::cout << "The MetaData of the Address " << pointerToFree << " is overwritten" << std::endl;
				return;
			}
			checkForOverwrite(pointerToFree, metaData->size);
#endif
			_currentBackAddress = metaData->lastDataBlock;
		}
	}

	void Reset(void)
	{
		while (_begin != _currentFrontAddress)
		{
			Free(reinterpret_cast<void*>(_currentFrontAddress));
		}

		while (_end != _currentBackAddress)
		{
			FreeBack(reinterpret_cast<void*>(_currentBackAddress));
		}
	}

	~DoubleEndedStackAllocator(void)
	{
		Reset();

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

		if (*reinterpret_cast<uint32_t*>(canaryAddress) != CANARY)
		{
			assert(!"The Front Canary of the Address " + addressToCheck + " is incomplete");
			std::cout << "The Front Canary of the Address " << addressToCheck << " is incomplete" << std::endl;
		}

		canaryAddress = addressToCheck + size;

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
		DoubleEndedStackAllocator allocator2(1048576u);
		Tests::VerifyOverflowBackSuccess(allocator2, 1073741824u / 4, 2);
		allocator2.Reset();


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