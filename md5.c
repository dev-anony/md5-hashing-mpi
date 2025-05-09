#include "md5.h"
#include <stddef.h>
#include <mpi.h>
#include <math.h>

/*
 * Constants defined by the MD5 algorithm
 */
#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476

static uint32_t S[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                       5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                       4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                       6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

static uint32_t K[] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                       0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                       0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                       0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                       0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                       0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                       0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                       0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                       0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                       0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                       0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                       0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                       0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                       0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                       0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                       0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

/*
 * Bit-manipulation functions defined by the MD5 algorithm
 */
#define F(X, Y, Z) ((X & Y) | (~X & Z))
#define G(X, Y, Z) ((X & Z) | (Y & ~Z))
#define H(X, Y, Z) (X ^ Y ^ Z)
#define I(X, Y, Z) (Y ^ (X | ~Z))

/*
 * Padding used to make the size (in bits) of the input congruent to 448 mod 512
 */
static uint8_t PADDING[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*
 * Initialize a context
 */
void md5Init(MD5Context *ctx){
	ctx->size = (uint64_t)0;

	ctx->buffer[0] = (uint32_t)A;
	ctx->buffer[1] = (uint32_t)B;
	ctx->buffer[2] = (uint32_t)C;
	ctx->buffer[3] = (uint32_t)D;
}

/*
 * Add some amount of input to the context
 *
 * If the input fills out a block of 512 bits, apply the algorithm (md5Step)
 * and save the result in the buffer. Also updates the overall size.
 */
void md5Update(MD5Context *ctx, uint8_t *input_buffer, size_t input_len, int rank, int cluster){
	
	
	//FUNC
	uint32_t input[200];
	uint32_t input0[16];
	unsigned int offset ;
	if (rank == 0){
		offset = ctx->size % 64;
		
	}
	ctx->size += (uint64_t)input_len;
	//bcast vals
	MPI_Bcast(&offset, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&ctx->size, 1, MD5_dt, 0, MPI_COMM_WORLD);
	
	int max;
	float ub = ceil(16/cluster);
	max = ub +1;
	// Copy each byte in input_buffer into the next space in our context input
	for(unsigned int i = 0; i < input_len; ++i){
		ctx->input[offset++] = (uint8_t)*(input_buffer + i);

		if(offset % 64 == 0){
			for(unsigned int j = 0; j < max; j++){
				
					// The local variable `input` our 512-bit chunk separated into 32-bit words
					input0[j] = (uint32_t)(ctx->input[(((max * rank)+j)  * 4) + 3]) << 24 |
							   (uint32_t)(ctx->input[(((max * rank)+j) * 4) + 2]) << 16 |
							   (uint32_t)(ctx->input[(((max * rank)+j) * 4) + 1]) <<  8 |
							   (uint32_t)(ctx->input[(((max * rank)+j) * 4)]);
				}
			MPI_Gather(input0, max,  MPI_UINT32_T, input, max,  MPI_UINT32_T, 0, MPI_COMM_WORLD);
			md5Step(ctx->buffer, input, rank, 1);
			offset = 0;
		}
	}
	
}

/*
 * Pad the current input to get to 448 bytes, append the size in bits to the very end,
 * and save the result of the final iteration into digest.
 */
void md5Finalize(MD5Context *ctx, int rank, int cluster){
	//FUNC
	uint32_t input[200];
	uint32_t input0[16];
	unsigned int offset; 
	unsigned int padding_length;
	if(rank == 0){
		offset = ctx->size % 64;
		padding_length = offset < 56 ? 56 - offset : (56 + 64) - offset;
	}
	//bcasting vals
	MPI_Bcast(&offset, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	MPI_Bcast(&padding_length, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	
	// Fill in the padding andndo the changes to size that resulted from the update
	md5Update(ctx, PADDING, padding_length, rank, cluster);
	ctx->size -= (uint64_t)padding_length;
	
	// Do a final update (internal to this function)
	// Last two 32-bit words are the two halves of the size (converted from bytes to bits)
	
       int max;
       float ub = ceil(14/cluster);
       max = ub+1;
        for(unsigned int j = 0; j < max; j++){
			input0[j] = (uint32_t)(ctx->input[(((max * rank)+j) * 4) + 3]) << 24 |
				   (uint32_t)(ctx->input[(((max * rank)+j) * 4) + 2]) << 16 |
				   (uint32_t)(ctx->input[(((max * rank)+j) * 4) + 1]) <<  8 |
				   (uint32_t)(ctx->input[(((max * rank)+j) * 4)]);
		}
	MPI_Gather(input0, max,  MPI_UINT32_T, input, max,  MPI_UINT32_T, 0, MPI_COMM_WORLD);	
	
	input[14] = (uint32_t)(ctx->size * 8);
	input[15] = (uint32_t)((ctx->size * 8) >> 32);
	MPI_Barrier(MPI_COMM_WORLD);
	md5Step(ctx->buffer, input, rank, cluster);
	if(rank == 0){
		
	// Move the result into digest (convert from little-endian)
		for(unsigned int i = 0; i < 4; ++i){
			ctx->digest[(i * 4) + 0] = (uint8_t)((ctx->buffer[i] & 0x000000FF));
			ctx->digest[(i * 4) + 1] = (uint8_t)((ctx->buffer[i] & 0x0000FF00) >>  8);
			ctx->digest[(i * 4) + 2] = (uint8_t)((ctx->buffer[i] & 0x00FF0000) >> 16);
			ctx->digest[(i * 4) + 3] = (uint8_t)((ctx->buffer[i] & 0xFF000000) >> 24);
		}
	}
}
/*
 * Step on 512 bits of input with the main MD5 algorithm.
 */
void md5Step(uint32_t *buffer, uint32_t *input, int rank, int cluster){
	uint32_t AA = buffer[0];
	uint32_t BB = buffer[1];
	uint32_t CC = buffer[2];
	uint32_t DD = buffer[3];

	uint32_t E;

	unsigned int j;
 	/*int max;
        float ub = ceil(64/cluster);
        max = ub;
	for(unsigned int i = 0; i < max; ++i){
		printf("%d\n",((max * rank)+i));
		switch(((max * rank)+i) / 16){
			case 0:
				E = F(BB, CC, DD);
				j = ((max * rank)+i);
				break;
			case 1:
				E = G(BB, CC, DD);
				j = ((((max * rank)+i) * 5) + 1) % 16;
				break;
			case 2:
				E = H(BB, CC, DD);
				j = ((((max * rank)+i) * 3) + 5) % 16;
				break;
			default:
				E = I(BB, CC, DD);
				j = (((max * rank)+i) * 7) % 16;
				break;
		}
		
		uint32_t temp = DD;
		DD = CC;
		CC = BB;
		BB = BB + rotateLeft(AA + E + K[((max * rank)+i)] + input[j], S[((max * rank)+i)]);
		AA = temp;
	}*/
	for(unsigned int i = 0; i < 64; ++i){
		switch(i / 16){
			case 0:
				E = F(BB, CC, DD);
				j = i;
				break;
			case 1:
				E = G(BB, CC, DD);
				j = ((i * 5) + 1) % 16;
				break;
			case 2:
				E = H(BB, CC, DD);
				j = ((i * 3) + 5) % 16;
				break;
			default:
				E = I(BB, CC, DD);
				j = (i * 7) % 16;
				break;
		}

		uint32_t temp = DD;
		DD = CC;
		CC = BB;
		BB = BB + rotateLeft(AA + E + K[i] + input[j], S[i]);
		AA = temp;
	}

	buffer[0] += AA;
	buffer[1] += BB;
	buffer[2] += CC;
	buffer[3] += DD;
}

/*
 * Functions that will return a pointer to the hash of the provided input
 */
uint8_t* md5String(char *input, int rank, int cluster){

	// create a type for struct MD5CONTEXT	
	const int nitems=4;
	int blocklengths[4] = {1,1,1,1};
	MPI_Datatype types[4] = { MPI_UINT64_T, MPI_UINT32_T , MPI_UINT8_T,MPI_UINT8_T};
	MPI_Datatype MD5_dt;
	MPI_Aint     offsets[4];

	offsets[0] = offsetof(MD5Context, size);
	offsets[1] = offsetof(MD5Context, buffer);
	offsets[2] = offsetof(MD5Context, input);
	offsets[3] = offsetof(MD5Context, digest);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MD5_dt);
	MPI_Type_commit(&MD5_dt);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// create context
	MD5Context ctx;
	md5Init(&ctx);
	md5Update(&ctx, (uint8_t *)input, strlen(input), rank, cluster);
	md5Finalize(&ctx, rank, cluster);
	
		
	// copy result
	uint8_t *result = malloc(16);
	memcpy(result, ctx.digest, 16);
	MPI_Type_free(&MD5_dt);
	return result;

}

uint8_t* md5File(FILE *file, int rank, int cluster){
	char *input_buffer = malloc(1024);
	size_t input_size = 0;

	MD5Context ctx;
	md5Init(&ctx);

	while((input_size = fread(input_buffer, 1, 1024, file)) > 0){
		md5Update(&ctx, (uint8_t *)input_buffer, input_size, rank, cluster);
	}

	md5Finalize(&ctx, rank, cluster);

	free(input_buffer);

	uint8_t *result = malloc(16);
	memcpy(result, ctx.digest, 16);
	return result;
}

/*
 * Rotates a 32-bit word left by n bits
 */
uint32_t rotateLeft(uint32_t x, uint32_t n){
	return (x << n) | (x >> (32 - n));
}
