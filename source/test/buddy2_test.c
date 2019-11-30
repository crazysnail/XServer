#include "buddy2.h"
#include <stdio.h>
//
//
//int main() {
//
//  unsigned int test = 0;
//  test =floor_align_power_2(17);
//  test = fixsize(17);
//
//  test = floor_align_power_2(1);
//  test = fixsize(1);
//
//  char cmd[80];
//  int arg;
//  struct buddy2* buddy = buddy2_new(32);
//  buddy2_dump(buddy);
//  for (;;) {
//    scanf("%s %d", cmd, &arg);
//    if (strcmp(cmd, "alloc") == 0) {
//      printf("allocated@%d\n", buddy2_alloc(buddy, arg));
//      buddy2_dump(buddy);
//    } else if (strcmp(cmd, "free") == 0) {
//      buddy2_free(buddy, arg);
//      buddy2_dump(buddy);
//    } else if (strcmp(cmd, "size") == 0) {
//      printf("size: %d\n", buddy2_size(buddy, arg));
//      buddy2_dump(buddy);
//    } else
//      buddy2_dump(buddy);
//  }
//}

#include "lua/fflua.h"
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>  
#include <zlib.h>  

int main(int argc, char* argv[])
{
	//auto name = fflua.get_name(script_id);
	char* lbuffer = 0;
	FILE * fp = fopen("D:\\mt\\Source\\Server\\bin\\lua_script\\mission\\test.lua", "rb");
	if (fp == NULL) {
		return ;
	}

	fseek(fp, 0, SEEK_END);
	auto size = ftell(fp);
	lbuffer = malloc(size + 1);//补个收尾符
	memset(lbuffer, 0, size + 1);
	fseek(fp, 0, SEEK_SET);
	auto read_size = fread(lbuffer, 1, size, fp);
	if (size != (long)read_size) {
		fclose(fp);
		free(lbuffer);
		return ;
	}
	fclose(fp);



	//char text[] = "zlib compress and uncompress test\nturingo@163.com\n2012-11-05\n";
	//uLong tlen = strlen(text) + 1;  /* 需要把字符串的结束符'\0'也一并处理 */
	//char* buf = NULL;
	//uLong blen;

	///* 计算缓冲区大小，并为其分配内存 */
	//blen = compressBound(tlen); /* 压缩后的长度是不会超过blen的 */
	//if ((buf = (char*)malloc(sizeof(char) * blen)) == NULL)
	//{
	//	printf("no enough memory!\n");
	//	return -1;
	//}

	///* 压缩 */
	//if (compress(buf, &blen, text, tlen) != Z_OK)
	//{
	//	printf("compress failed!\n");
	//	return -1;
	//}

	///* 解压缩 */
	//char new_text[512] = { 0 };
	//if (uncompress(new_text, &tlen, buf, blen) != Z_OK)
	//{
	//	printf("uncompress failed!\n");
	//	return -1;
	//}

	///* 打印结果，并释放内存 */
	//printf("%s", text);
	//printf("%s", new_text);
	//if (buf != NULL)
	//{
	//	free(buf);
	//	buf = NULL;
	//}

	return 0;
}