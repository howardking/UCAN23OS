#include "apilib.h"

void UcanMain(void)
{
	char a[100];
	a[10] = 'A';		/* û����	*/
	api_putchar(a[10]);
	a[102] = 'B';		/* ����Խ�� */
	api_putchar(a[102]);
	a[123] = 'C';		/* ����Խ�� */
	api_putchar(a[123]);
	api_end();
}
