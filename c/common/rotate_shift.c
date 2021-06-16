#include <stdio.h>

/**
* \require <stdio.h>
* \brief rotate_left 循环左移，可以把unsigned char替换成需要的数据类型
* \param value
* \param count 位移位数，正数为左移，负数为右移
* \return 结果
*/
unsigned char rotate_left(unsigned char value, int count)
{
	const int nbits = sizeof(unsigned char) * 8;

	if ( count > 0 )
	{
		count %= nbits;
		unsigned char high = value >> (nbits - count);
		if ( (unsigned char)(-1) < 0 ) // signed value
			high &= ~(((unsigned char)(-1) << count));
		value <<= count;
		value |= high;
	}
	else
	{
		count = -count % nbits;
		int low = value << (nbits - count);
		value >>= count;
		value |= low;
	}
	return value;

}
