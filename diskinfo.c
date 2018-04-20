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
    printf("Super block information:\n");
    printf("Block size: %d\n", htons(sb->block_size));
    printf("Block count: %d\n", htonl(sb->file_system_block_count));
    printf("FAT starts: %d\n", htonl(sb->fat_start_block));
    printf("FAT blocks: %d\n", htonl(sb->fat_block_count));
    printf("Root directory start: %d\n", htonl(sb->root_dir_start_block));
    printf("Root directory blocks: %d\n", htonl(sb->root_dir_block_count));
    
    int fatblockcount =  htonl(sb->fat_block_count);
    int fatstartblock = htonl(sb->fat_start_block);
    int blocksize = htons(sb->block_size);
    int blockcount = htonl(sb->file_system_block_count);
    
    int startingbyte = (fatstartblock)*blocksize;
    
    
    void* entirefile = mmap(NULL, blockcount*blocksize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);

    
    int fssize;
    int counter = 0;
    int reserved = 0;
    int available = 0;
    int allocated = 0;
    
    for(int i = 0; i < fatblockcount*(blocksize/4); i++){
    	memcpy(&fssize, entirefile+startingbyte+counter, 4);
    	fssize=ntohl(fssize);
    	if(fssize == 1){
    		reserved++;
    	}else if(fssize == 0){
    		available++;
    	}else{
    		allocated++;
    	}
    	counter = counter + 4;
    }
    
    printf("\nFAT Information:\n");
    printf("Free Blocks: %d\n",available);
    printf("Reserved Blocks: %d\n",reserved);
    printf("Allocated Blocks: %d\n",allocated);

    munmap(address,buffer.st_size);
    close(fd);
}
