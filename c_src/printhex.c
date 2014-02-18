#include <stdio.h>

void printMemBlock(unsigned char const *pBuf, int len)
{
	int i, k;
	k = 0;
	for (i = 0; i < len; i++)
	{
		  if (i == 0) {
		  	printf("%04X  ", (k++)*16);
		  }
	    if (i > 0) {
	    	if (i%16 == 0) {	    		
	    		printf("\n");
	    		printf("%04X  ", (k++)*16);
	    	} else {
	    		if (i%8 == 0) printf(" ");
	    		printf(" ");
	    	}
	    }
	    printf("%02X", *(pBuf+i));
	}
	printf("\n");
}

void sprintf_mem(unsigned char const *bin, int binsz, char **result)
{
  if (!binsz)
    return;
    
  char hex_str[]= "0123456789abcdef";

  *result = (char *) malloc(binsz * 4 + 1);
  memset((void *)(*result), 0, binsz*4+1);
  //(*result)[binsz * 4] = 0;

	int i, k;
	k = 0;
	for (i = 0; i < binsz; i++)
	{
		  if (i == 0) {
		  	sprintf(*result, "%04X  ", (k++)*16);
		  }
	    if (i > 0) {
	    	if (i%16 == 0) {	    		
	    		sprintf(*result, "\n");
	    		sprintf(*result, "%04X  ", (k++)*16);
	    	} else {
	    		if (i%8 == 0) sprintf(*result, " ");
	    		sprintf(*result, " ");
	    	}
	    }
	    sprintf(*result, "%02X", *(bin+i));
	}
	sprintf(*result, "\n");
}

void bin_to_strhex(unsigned char *bin, unsigned int binsz, char **result)
{
  char          hex_str[]= "0123456789abcdef";
  unsigned int  i;

  *result = (char *) malloc(binsz * 2 + 1);
  (*result)[binsz * 2] = 0;

  if (!binsz)
    return;

  for (i = 0; i < binsz; i++)
    {
      (*result)[i * 2 + 0] = hex_str[bin[i] >> 4  ];
      (*result)[i * 2 + 1] = hex_str[bin[i] & 0x0F];
    }  
}

int test(){
    unsigned char buf[] = {0, 1, 10, 11};
    /* target buffer should be large enough */
    char str[12];

    unsigned char * pin = buf;
    const char * hex = "0123456789ABCDEF";
    char * pout = str;
    int i = 0;
    for(; i < sizeof(buf)-1; ++i){
        *pout++ = hex[(*pin>>4)&0xF];
        *pout++ = hex[(*pin++)&0xF];
        *pout++ = ':';
    }
    *pout++ = hex[(*pin>>4)&0xF];
    *pout++ = hex[(*pin)&0xF];
    *pout = 0;

    printf("%s\n", str);
    
		for (i = 0; i < sizeof(str)-1; i++)
		{
		    if (i > 0) printf(":");
		    printf("%02X", str[i]);
		}
		printf("\n");
		
		char xx[64] = "hello workd";
		
		printMemBlock(xx, sizeof(xx));
		
		char * result; 
		sprintf_mem(xx, sizeof(xx), &result);
		printf("result: \n%s\n", result);
		free(result);
}