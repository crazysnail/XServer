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
	lbuffer = malloc(size + 1);//������β��
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
	//uLong tlen = strlen(text) + 1;  /* ��Ҫ���ַ����Ľ�����'\0'Ҳһ������ */
	//char* buf = NULL;
	//uLong blen;

	///* ���㻺������С����Ϊ������ڴ� */
	//blen = compressBound(tlen); /* ѹ����ĳ����ǲ��ᳬ��blen�� */
	//if ((buf = (char*)malloc(sizeof(char) * blen)) == NULL)
	//{
	//	printf("no enough memory!\n");
	//	return -1;
	//}

	///* ѹ�� */
	//if (compress(buf, &blen, text, tlen) != Z_OK)
	//{
	//	printf("compress failed!\n");
	//	return -1;
	//}

	///* ��ѹ�� */
	//char new_text[512] = { 0 };
	//if (uncompress(new_text, &tlen, buf, blen) != Z_OK)
	//{
	//	printf("uncompress failed!\n");
	//	return -1;
	//}

	///* ��ӡ��������ͷ��ڴ� */
	//printf("%s", text);
	//printf("%s", new_text);
	//if (buf != NULL)
	//{
	//	free(buf);
	//	buf = NULL;
	//}

	return 0;
}