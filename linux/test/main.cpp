#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

int main()
{
	int rc;
	int fileHandle = -1;
	void *bufferSrc = NULL;
	void *bufferDst = NULL;
	void *bufferDev = NULL;
	unsigned long sizeTest = 256 * 1024 * 1024;
	unsigned long ii, jj;
	unsigned long *p;
	unsigned char *c;

	printf( "* open file\n" );
	fileHandle = open( "/dev/minipci0", O_RDWR );
	if ( fileHandle < 0 )
	{
		printf( "Device cannot be opened\n" );
		exit( -1 );
	}

	printf( "* allocate memory\n" );
	bufferSrc = malloc( sizeTest );
	printf( "  - bufferSrc = 0x%p\n", bufferSrc );
	bufferDst = malloc( sizeTest );
	printf( "  - bufferDst = 0x%p\n", bufferDst );
	if (( NULL == bufferSrc ) || ( NULL == bufferDst ))
	{
		printf( "Out of memory\n" );
		exit( -1 );
	}

	printf( "* fill memory1\n" );
	memset( bufferSrc, 0x00, sizeTest );
	memset( bufferDst, 0x00, sizeTest );

	printf( "* fill memory2\n" );
	c = ( unsigned char * )bufferSrc;
	for ( ii = 0; ii < sizeTest; ++ii )
	{
		c[ ii ] = ( unsigned char )ii;
	}

#if 1
	printf( "* map memory\n" );
	bufferDev = mmap( 0, sizeTest, PROT_WRITE, MAP_SHARED, fileHandle, 0 );
	if ( MAP_FAILED == bufferDev )
	{
		printf( "Mmap failed\n" );
		exit( -1 );
	}
	printf( "* write hardware\n" );
	memcpy( bufferDev, bufferSrc, sizeTest );

	printf( "* read hardware\n" );
	memcpy( bufferDst, bufferDev, sizeTest );
#else
	printf( "* write hardware\n" );
        rc = write( fileHandle, bufferSrc, sizeTest );
	printf( "  - rc = %ld (should be zero)\n", rc );	// hmmm: ~16MB/s

	printf( "* read hardware\n" );
        rc = read( fileHandle, bufferDst, sizeTest );		// hmmm: ~1MB/s
	printf( "  - rc = %ld (should be zero)\n", rc );
#endif
	printf( "* compare data\n" );
	rc = memcmp( bufferSrc, bufferDst, sizeTest );
	printf( "  - rc = %ld (should be zero)\n", rc );

#if 0
	c = ( unsigned char * )bufferDst;
	for ( ii = 1 * 1024 * 1024; ii < ( 1 * 1024 * 1024 + 64 * 1024 ); ii += 16 )
	{
		printf( "%8.8x: ", ii );
		for ( jj = 0; jj < 16; ++jj )
		{
			printf( "%2.2x ", c[ ii + jj ]);
		}
		printf( "\n" );
	}
#endif

	printf( "* free memory\n" );
	free( bufferSrc );
	free( bufferDst );

	printf( "* close file\n" );
	close( fileHandle );
}
