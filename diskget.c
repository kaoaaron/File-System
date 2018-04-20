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
    
    int fatstartblock = htonl(sb->fat_start_block);
    int fatstart = fatstartblock*blocksize;
    int startingbyte = (rootstartblock)*blocksize;
    int fatstartingbyte = (fatstartblock)*blocksize;
    void* entirefile = mmap(NULL, blockcount*blocksize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);

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
	//memcpy(&fssize, entirefile, 4);
    //fssize=ntohs(fssize) >> 1;
	//printf("%x", fssize);


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
				/*if(spacing1 == 0){
					printf("%30c", stat);
					spacing1 = 1;
				}
				else if(spacing1 == 1){
					printf("%c", stat);
				}*/
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
    			
    			/*while(fssize != 0xFFFFFFFF){
    				printf("\n\n%d\n\n",fssize);
    				
    				for(int i = 0; i < blocksize; i++){
    					memcpy(&stat, entirefile+(fssize+1)*blocksize+i, 1);
    					//printf("%c", stat);
    					write(filewrite,&stat, 1);
    				}
    				memcpy(&fssize, entirefile+fatstart+4*startblock, 4);
    				fssize=ntohl(fssize);
    				startblock = fssize;
    				
    				//break;
    			}*/
			//fssize = 0;
			int sizeleft = filesize;
			char buffer2;
			//printf("\n\nNUM BLOCK %d\n\n", numblock);
			while(fssize != 0xFFFFFFFF){
			//		printf("\n\nLOOP %d\n\n", fssize);

					if(sizeleft > blocksize){
						sizeleft = sizeleft - blocksize;
						for(int i = 0; i < blocksize; i++){
							memcpy(&buffer2,entirefile+blocksize*fssize+i, 1);
				//			printf("%c", buffer2);
							write(filewrite,&buffer2, 1);
						}	
					}else{
						for(int i = 0; i < sizeleft; i++){
							memcpy(&buffer2,entirefile+blocksize*fssize+i, 1);
					//		printf("%c", buffer2);
							write(filewrite,&buffer2, 1);
						}	
					}
									
					
				//	printf("\n\nasd%d\n\n", fatstartingbyte);
					memcpy(&fssize, entirefile+fatstartingbyte+fssize*4,4);
					fssize = ntohl(fssize);
				//	printf("\n\nsafsafas %x\n\n", fssize);
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
	if(flag == 0){
		printf("File not found\n");
	}else{
		printf("Get is Successful\n");
	}
    munmap(address,buffer.st_size);
    close(fd);
}
