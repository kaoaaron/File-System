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

    //tamplate:   pa=mmap(addr, len, prot, flags, fildes, off);
    //c will implicitly cast void* to char*, while c++ does NOT
    void* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct superblock_t* sb;
    sb=(struct superblock_t*)address;
    
    int blocksize = htons(sb->block_size);
    int blockcount = htonl(sb->file_system_block_count);

	int rootblockcount = htonl(sb->root_dir_block_count);
	int rootstartblock = htonl(sb->root_dir_start_block);
    
    int startingbyte = (rootstartblock)*blocksize;
    
    void* entirefile = mmap(NULL, blockcount*blocksize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);

    int fssize;
	char stat;
    int counter = 0;
    int reserved = 0;
    int available = 0;
    int allocated = 0;
	int jump = 0;   
	int spacing1 = 0; 

	//memcpy(&fssize, entirefile, 4);
    //fssize=ntohs(fssize) >> 1;
	//printf("%x", fssize);


    for(int i = 0; i < rootblockcount*(blocksize/64); i++){
    	memcpy(&stat, entirefile+startingbyte+jump, 1);
		
    	if(stat & (1 << 0)){
 			if(stat & (1 << 1)){
				printf("F ");
			}
			else if(stat & (1 << 2)){
				printf("D ");
			}
			memcpy(&fssize, entirefile+startingbyte+jump+9, 4);
			fssize = ntohl(fssize);
			printf("%10d ", fssize);
			for(int name_count = 27; name_count < 54; name_count++){
				memcpy(&stat, entirefile+startingbyte+jump+name_count, 1);
				if(spacing1 == 0){
					printf("%30c", stat);
					spacing1 = 1;
				}
				else if(spacing1 == 1){
					printf("%c", stat);
				}
			}
			memcpy(&fssize, entirefile+startingbyte+jump+20, 2);
			fssize = ntohs(fssize);
			printf("\t%d/", fssize);
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
			printf("\n");

		}
		spacing1 = 0;
		jump = jump + 64;
    	//counter = counter + 4;
    }

    munmap(address,buffer.st_size);
    close(fd);
}
