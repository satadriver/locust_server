



#include "utils.h"


int binarySearch(const char* data, int size, const char* tag, int tagsize) {
	for (int i = 0; i <= size - tagsize; i++)
	{
		if (memcmp(data + i, tag, tagsize) == 0)
		{
			return i;
		}
	}

	return -1;
}