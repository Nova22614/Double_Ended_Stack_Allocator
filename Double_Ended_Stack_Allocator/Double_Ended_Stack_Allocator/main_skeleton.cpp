/**
* Exercise: "DoubleEndedStackAllocator with Canaries" OR "Growing DoubleEndedStackAllocator with Canaries (VMEM)"
* Group members: NAME1 (gsXXXX), NAME2 (gsXXXX), NAME3 (gsXXXX)
**/

#include "stdio.h"
#include <Windows.h>
#include <iostream>

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

		if (mem == nullptr)
		{
			printf("[Error]: Allocator returned nullptr!\n");
			return false;
		}

		return true;
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

	uintptr_t _begin = 0;
	uintptr_t _end = 0;

	uintptr_t _currentFrontAddress = 0;
	uintptr_t _currentBackAddress = 0;

	uintptr_t _frontStackEnd = 0;
	uintptr_t _backStackBegin = 0;

	uint32_t CANARY = 0xB00BEE5;
	uint32_t CANARY_SIZE = sizeof(CANARY) * WITH_DEBUG_CANARIES;

	 //Maybe Hash for MetaData Check
	struct MetaData
	{
		MetaData(size_t size):size(size) {}
		size_t size;
	};

public:
	DoubleEndedStackAllocator(size_t max_size)
	{
		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		_pageSize = system_info.dwPageSize;

		std::cout << _pageSize << std::endl;
		std::cout << CANARY_SIZE << std::endl;

		void* first_Address = VirtualAlloc(NULL, max_size, MEM_RESERVE, PAGE_READWRITE);

		if (!first_Address)
		{
			//TODO: Error Handling
		}

		first_Address = VirtualAlloc(first_Address, _pageSize, MEM_COMMIT, PAGE_READWRITE);

		if (!first_Address)
		{
			//TODO: Error Handling
		}

		_begin = reinterpret_cast<uintptr_t>(first_Address);
		_currentFrontAddress = _begin;
		_frontStackEnd = _begin + _pageSize;


		first_Address = VirtualAlloc(reinterpret_cast<void*>(_begin + max_size - _pageSize), _pageSize, MEM_COMMIT, PAGE_READWRITE);

		if (!first_Address)
		{
			//TODO: Error Handling
		}

		_backStackBegin = reinterpret_cast<uintptr_t>(first_Address);
		_end = _backStackBegin + _pageSize;
		_currentBackAddress = _end;
	}

	void* Allocate(size_t size, size_t alignment)
	{
		if (false) // TODO: Add Alignment Check (Is alignement Power of 2)
		{

		}

		if (_currentFrontAddress != _begin)
		{
			MetaData* metaData = getMetaData(_currentFrontAddress);
			_currentFrontAddress = _currentFrontAddress + CANARY_SIZE;
		}

		_currentFrontAddress = _currentFrontAddress + sizeof(MetaData) + CANARY_SIZE;

		_currentFrontAddress = _currentFrontAddress + alignment - _currentFrontAddress % alignment;

		if (_currentFrontAddress + size + CANARY_SIZE > _backStackBegin )
		{
			//TODO: Error Handling
			return nullptr;
		}

		if(_currentFrontAddress + size + CANARY_SIZE > _frontStackEnd)
		{
			uintptr_t sizeDifference = (_currentFrontAddress + size + CANARY_SIZE) - _frontStackEnd;
			size_t pagesToCommit = ((size_t)(sizeDifference / _pageSize) + 1) * _pageSize;

			void* debugAlloc = VirtualAlloc(reinterpret_cast<void*>(_frontStackEnd), pagesToCommit, MEM_COMMIT, PAGE_READWRITE);
			
			if (!debugAlloc)
			{
				//TODO: Error Handling
			}

			_frontStackEnd += pagesToCommit;
		}

		addMetaData(_currentFrontAddress - sizeof(MetaData), size);
		addCanary(_currentFrontAddress - sizeof(MetaData) - CANARY_SIZE);
		addCanary(_currentFrontAddress + size);

		return reinterpret_cast<void*>(_currentFrontAddress);
	}

	void* AllocateBack(size_t size, size_t alignment)
	{
		if (false) // TODO: Add Alignment Check (Is alignement Power of 2)
		{

		}

		if (_currentBackAddress != _end)
		{
			MetaData* metaData = getMetaData(_currentBackAddress);
			_currentBackAddress = _currentBackAddress - CANARY_SIZE - sizeof(MetaData);
		}

		_currentBackAddress = _currentBackAddress - CANARY_SIZE - size;

		_currentBackAddress = _currentBackAddress - _currentBackAddress % alignment;

		if (_currentBackAddress - sizeof(MetaData) - CANARY_SIZE < _frontStackEnd)
		{
			//TODO: Error Handling
			return nullptr;
		}

		if (_currentBackAddress - sizeof(MetaData) - CANARY_SIZE < _backStackBegin)
		{
			uintptr_t sizeDifference = _backStackBegin - (_currentBackAddress - sizeof(MetaData) - CANARY_SIZE);
			size_t pagesToCommit = ((size_t)(sizeDifference / _pageSize) + 1) * _pageSize;

			void* debugAlloc = VirtualAlloc(reinterpret_cast<void*>(_backStackBegin - pagesToCommit), pagesToCommit, MEM_COMMIT, PAGE_READWRITE);

			if (!debugAlloc)
			{
				//TODO: Error Handling
			}

			_backStackBegin -= pagesToCommit;
		}
		addMetaData(_currentBackAddress - sizeof(MetaData), size);
		addCanary(_currentBackAddress - sizeof(MetaData) - CANARY_SIZE);
		addCanary(_currentBackAddress + size);

		return reinterpret_cast<void*>(_currentBackAddress);
	}

	void Free(void* memory)
	{

	}

	void FreeBack(void* memory)
	{

	}

	void Reset(void) 
	{

	}

	~DoubleEndedStackAllocator(void) 
	{
	}

private:
	MetaData* getMetaData(uintptr_t pointerToData)
	{
		return reinterpret_cast<MetaData*>(pointerToData - sizeof(MetaData));
	}

	void addMetaData(uintptr_t pointerToMetaDataPositon, size_t size)
	{
		*reinterpret_cast<MetaData*>(pointerToMetaDataPositon) = MetaData(size);
	}

	void addCanary(uintptr_t pointerToCanaryPosition)
	{
		*reinterpret_cast<uint32_t*>(pointerToCanaryPosition) = CANARY;
	}
};


int main()
{
	// You can add your own tests here, I will call my tests at then end with a fresh instance of your allocator and a specific max_size
	{
		// You can remove this, just showcasing how the test functions can be used
		DoubleEndedStackAllocator allocator(1048576u);
		Tests::Test_Case_Success("Allocate() returns nullptr", [&allocator](){ return allocator.Allocate(32, 1) == nullptr; }());
		Tests::Test_Case_Success("Allocate() returns nullptr", [&allocator]() { return allocator.AllocateBack(32, 1) == nullptr; }());
	}

	// You can do whatever you want here in the main function

	// Here the assignment tests will happen - it will test basic allocator functionality. 
	{

	}
}