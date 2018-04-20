#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <math.h>

struct __attribute__((__packed__)) superblock_t {
    uint8_t   fs_id [8];
    uint16_t block_size;
    uint32_t file_system_block_count;
    uint32_t fat_start_block;
    uint32_t fat_block_count;
    uint32_t root_dir_start_block;
    uint32_t root_dir_block_count;
};

void main(int argc, char* argv[]) {

    int fd = open(argv[1], O_RDWR);
    struct stat buffer;
    int status = fstat(fd, &buffer);
    char getFile[1000];

	strcpy(getFile, argv[2]);
	//printf("\n%s", getFile);

	
    //tamplate:   pa=mmap(addr, len, prot, flags, fildes, off);
    //c will implicitly cast void* to char*, while c++ does NOT
    void* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct superblock_t* sb;
    sb=(struct superblock_t*)address;
 	/*printf("Super block information:\n");
    printf("Block size: %d\n", htons(sb->block_size));
    printf("Block count: %d\n", htonl(sb->file_system_block_count));
    printf("FAT starts: %d\n", htonl(sb->fat_start_block));
    printf("FAT blocks: %d\n", htonl(sb->fat_block_count));
    printf("Root directory start: %d\n", htonl(sb->root_dir_start_block));
    printf("Root directory blocks: %d\n", htonl(sb->root_dir_block_count));*/
    
    int blocksize = htons(sb->block_size);
    int blockcount = htonl(sb->file_system_block_count);

	int rootblockcount = htonl(sb->root_dir_block_count);
	int rootstartblock = htonl(sb->root_dir_start_block);
    
    int fatblockcount = htonl(sb->fat_block_count);
    int fatstartblock = htonl(sb->fat_start_block);
    int fatstart = fatstartblock*blocksize;
    int startingbyte = (rootstartblock)*blocksize;
    int fatstartingbyte = (fatstartblock)*blocksize;
    
    void* entirefile = mmap(NULL, blockcount*blocksize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);

	FILE *fp = fopen(argv[2], "rb");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	int numblocks = ceil(size/blocksize);

	numblocks = size / blocksize;
	if (numblocks * blocksize < size) ++numblocks;
//	printf("\norig   sizeleft: %d %d\n: ", size, blocksize);

//	printf("the sissze: %d\n", numblocks);

    	int fssize;
	char stat;
	int counter = 0;
	int reserved = 0;
	int available = 0;
	int allocated = 0;
	int jump = 0;   
	int spacing1 = 0; 
	char temp[1000];
	int indice = 0;
	int flag = 0;
	memcpy(&fssize, entirefile, 4);
    	fssize=ntohs(fssize);
//	printf("%x", fssize);


    for(int i = 0; i < rootblockcount*(blocksize/64); i++){
    	memcpy(&stat, entirefile+startingbyte+jump, 1);
		
    	if(stat & (1 << 0)){
 			if(stat & (1 << 1)){
		//		printf("F ");
			}
			else if(stat & (1 << 2)){
		//		printf("D ");
			}
			memcpy(&fssize, entirefile+startingbyte+jump+9, 4);
			fssize = ntohl(fssize);
		//	printf("%10d ", fssize);
			for(int name_count = 27; name_count < 54; name_count++){
				memcpy(&stat, entirefile+startingbyte+jump+name_count, 1);
				temp[indice] = stat;
				indice = indice+1;
			}
			indice = 0;
			
			
			if(strcmp(temp, getFile) == 0){
				flag = 1;
				//printf("File found");
				//FILE *fp = null;
    			
    			int filewrite = open(argv[2], O_TRUNC | O_CREAT | O_WRONLY | O_RDONLY | O_APPEND, 777);
    			
				memcpy(&fssize, entirefile+startingbyte+jump+5, 4);
    			fssize=ntohl(fssize);
    			int numblock = fssize;
    			
				memcpy(&fssize, entirefile+startingbyte+jump+9, 4);
    			fssize=ntohl(fssize);
    			int filesize = fssize;
    			int jump2 = 0;
    			
    			//write(filewrite, &fssize, sizeof(fssize));
    			memcpy(&fssize, entirefile+startingbyte+jump+1, 4);
    			fssize=ntohl(fssize);
    			int startblock = fssize;
    			
    			char stat2[0];
    			
    			while(fssize != 0xFFFFFFFF){
    			//	printf("\n\n%d\n\n",fssize);
    				
    				for(int i = 0; i < blocksize; i++){
    					memcpy(&stat, entirefile+fssize*blocksize+i, 1);
						if(spacing1 == 0){
						//	printf("%30c", stat);
							spacing1 = 1;
						}
						else if(spacing1 == 1){
					//		printf("%c", stat);
						}
    					//printf("%c", stat);
    					write(filewrite,&stat, 1);
    				}
    				memcpy(&fssize, entirefile+fatstart+4*startblock, 4);
    				fssize=ntohl(fssize);
    				startblock = fssize;
    				
    				//break;
    			}
    			/*for(int i = startblock*blocksize; i < startblock*blocksize + filesize; i++){
    				memcpy(&stat, entirefile+i, 1);
    				printf("%c", stat);
    			}
    			
    			
    			printf("startblock %d\n", startblock);
    			printf("# Blocks %d\n", numblock);
				printf("File Size %d\n", filesize);
				*/
				//printf("startblock %d\n", startblock);
			}
			
			/*printf("test %s\n", temp);
			memcpy(&fssize, entirefile+startingbyte+jump+20, 2);
			fssize = ntohs(fssize);
			printf("\t%10d/", fssize);
			memcpy(&stat, entirefile+startingbyte+jump+22, 1);
			printf("%.2d/", stat);
			memcpy(&stat, entirefile+startingbyte+jump+23, 1);
			printf("%.2d ", stat);
			memcpy(&stat, entirefile+startingbyte+jump+24, 1);
			printf("%.2d:", stat);
			memcpy(&stat, entirefile+startingbyte+jump+25, 1);
			printf("%.2d:", stat);
			memcpy(&stat, entirefile+startingbyte+jump+26, 1);
			printf("%.2d", stat);
			printf("\n");*/

		}
		
		spacing1 = 0;
		jump = jump + 64;
    	//counter = counter + 4;
    }
   	int jump2 = 0;
   	int count = 0; 
   	int blockz = 0;
    int count2 = 0;
    int fatflag = 0;
    int sizeleft = size;
	int firstblock = 0;
	int stupidflag = 0;
	//printf("\norig   sizeleft: %d\n: ", sizeleft);
    //access rootdir
	if(flag == 0){
		for(int i = 0; i < rootblockcount*(blocksize/64); i++){
	    	memcpy(&stat, entirefile+startingbyte+jump2, 1);
			//printf("test");
			if(stat & (1 << 1)){
				sleep(0);
			}else{
				//access fat
				while(blockz < numblocks){
				
					memcpy(&fssize, entirefile+fatstartingbyte+count,4);
					fssize = ntohl(fssize);
					//if empty spot
					if(fssize == 0){
						if(blockz == 0){
							firstblock = (count/4)-1;
						}
						count2 = count + 4;//start next counter as next element
						fatflag = 0;
						//loop through to find the next empty spot
						while(fatflag == 0){
							memcpy(&fssize, entirefile+fatstartingbyte+count2,4);
							fssize = ntohl(fssize);
							//found the next empty spot
							if(fssize == 0){
								//if last block
								if(sizeleft <= blocksize){
										if(blockz > 1){
											//printf("\nss%d\n", count2/4);
											//printf("\n2nd%d\n: ", fatstartingbyte+count);
											//memset(entirefile+fatstartingbyte+count, (count2/4)-1, 4);
											memset(entirefile+fatstartingbyte+count, 0xFFFFFFFF, 4);
											blockz = blockz + 1;
										}
										else if(blockz < 1){
										//	printf("\n2nd%d\n: ", fatstartingbyte+count);
											memset(entirefile+fatstartingbyte+count, 0xFFFFFFFF, 4);
											blockz = blockz + 1;
											//printf("\nss%d\n", blockz);
										}
								}
								else if (sizeleft > blocksize){
									sizeleft = sizeleft - blocksize;
									int tempc = (count2/4);
									tempc = ntohl(tempc);
								//	printf("\ncount 2 = %d count 1 = %d", count2, count);
								//	printf("\n1st%d  nextval: %x %d sizeleft: %d\n: ", fatstartingbyte+count,tempc, tempc, sizeleft);
									memcpy(entirefile+fatstartingbyte+count, &tempc, 4);
									if(blockz == 0){
										firstblock = (count/4);
									}
									blockz = blockz + 1;
								}
								//printf("\nss%d\n", blockz);
								count = count2;
								stupidflag = 1;
								fatflag = 1;
							}else{
								count2 = count2 + 4;
							}
						}
					}
					if(stupidflag != 1){
						count = count + 4;
					}
					stupidflag = 0;
				}
				//printf("\nfirst block: %x\n", firstblock);
				
				stupidflag = 0;
				fssize = 0;

				char buffer2;
				int offset = 0;
				rewind(fp);
				sizeleft = size;

				while(fssize != 0xFFFFFFFF){
					if(stupidflag == 0){
						fssize = firstblock;
						stupidflag = 1;
					}

					if(sizeleft > blocksize){
						sizeleft = sizeleft - blocksize;
						for(int i = 0; i < blocksize; i++){
							fseek(fp, offset, SEEK_CUR);
							fread(&buffer2, 1, 1, fp);
							memcpy(entirefile+blocksize*fssize+i, &buffer2, 1);
					//		printf("%c", buffer2);
						}	
					}else{
						for(int i = 0; i < sizeleft; i++){
							fseek(fp, offset, SEEK_CUR);
							fread(&buffer2, 1, 1, fp);
							memcpy(entirefile+blocksize*fssize+i, &buffer2, 1);
					//		printf("%c", buffer2);
						}	
					}
									
					
				//	printf("\n\nasd%d\n\n", fatstartingbyte);
					memcpy(&fssize, entirefile+fatstartingbyte+fssize*4,4);
					fssize = ntohl(fssize);
				//	printf("\n\nLOOP %x\n\n", fssize);
				}

				/*for(int i = 0; i < numblocks; i++){
					if(i == 0){
						fssize = firstblock;
					}
					memcpy(&fssize, entirefile+fatstartingbyte+fssize*4,4);
					fssize = ntohl(fssize);
					printf("\n\n%x\n\n", fssize);
				}*/
				
				/*while(blockz != numblocks){
					memcpy(&fssize, entirefile+fatstartingbyte+count,4);
					fssize = ntohl(fssize);
					if(fssize == 0){
						fatflag = 0;
						tempcount = count;
						blockz++;
						while(fatflag == 0){
							
							count = count + 4;
							memcpy(&fssize, entirefile+fatstartingbyte+count,4);
							fssize = ntohl(fssize);
							if(fssize == 0){
								printf("\ntest: %d\n", blockz);
								if(blockz != numblocks){
									memset(entirefile+fatstartingbyte+tempcount, count/4, 4);
									printf("\ntest1: %d\n", blockz);
									//write into block here
									char buff;
									
								}
								else{
									printf("\ntest2: %d\n", blockz);
									memset(entirefile+fatstartingbyte+tempcount, 0xFFFFFF, 4);
								}
								fatflag = 1;
								
								
							}
							
						}
					}
					else{
						count = count + 4;
					}
				}*/
				
				//printf("\n\nBlockz is : %d \n", blockz);
				
	 			//printf("\np:%d\n",startingbyte+jump2+27);
				char temp[1];
			//	printf("test\n");
				int argsize = strlen(argv[2]);
				int x = 0x3;
				firstblock = ntohl(firstblock);
				numblocks = ntohl(numblocks);
				//printf("\nnumblocks: %d\n", numblocks);
				size = ntohl(size);
				memcpy(entirefile+startingbyte+jump2, &x, 1);
				memcpy(entirefile+startingbyte+jump2+1, &firstblock, 4);
				memcpy(entirefile+startingbyte+jump2+5, &numblocks, 4);
				memcpy(entirefile+startingbyte+jump2+9, &size, 4);
				memcpy(entirefile+startingbyte+jump2+27, argv[2], argsize);
				memcpy(&fssize,entirefile+startingbyte+jump2+9, 4);
				fssize=ntohl(fssize);
			//	printf("test%x", fssize);
				break;
	 		}
	 		jump2 = jump2 + 64;
	    }
	}
    munmap(address,buffer.st_size);
    close(fd);
}
