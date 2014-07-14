#include "apilib.h"

void UcanMain(void)
{
	char a[100];
	a[10] = 'A';		/* 没问题	*/
	api_putchar(a[10]);
	a[102] = 'B';		/* 数组越界 */
	api_putchar(a[102]);
	a[123] = 'C';		/* 数组越界 */
	api_putchar(a[123]);
	api_end();
}
