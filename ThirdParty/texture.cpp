#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_GLES
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#include <GL/glfw.h>
#endif

GLuint loadBMP_custom(const char * imagepath){

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file)							    {printf("Image could not be opened\n"); return 0;}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 byes are read, problem
	if ( fread(header, 1, 54, file)!=54 ){ 
		printf("Not a correct BMP file\n");
		return false;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    return 0;}
	if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    return 0;}

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);

	// Everything is in memory now, the file wan be closed
	fclose (file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	#ifdef USE_GLES
	{
		unsigned char* p = (unsigned char*)data;
		for (i=0; i<width*height; i++) {
			unsigned char tmp = *p;
			*p = *(p+2);
			*(p+2) = tmp;
			p+=3;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	#else
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	#endif

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap(GL_TEXTURE_2D);

	delete[] data;

	// Return the ID of the texture we just created
	return textureID;
}

#ifdef USE_GLES
// taken from NeHe TGA lesson 33 http://nehe.gamedev.net/tutorial/loading_compressed_and_uncompressed_tga's/22001/

typedef struct
{
	GLubyte	* imageData;									/* Image Data (Up To 32 Bits) */
	GLuint	bpp;											/* Image Color Depth In Bits Per Pixel */
	GLuint	width;											/* Image Width */
	GLuint	height;											/* Image Height */
	GLuint	texID;											/* Texture ID Used To Select A Texture */
	GLuint	type;											/* Image Type (GL_RGB, GL_RGBA) */
} Texture;	

typedef struct
{
	GLubyte Header[12];									/* TGA File Header */
} TGAHeader;

typedef struct
{
	GLubyte		header[6];								/* First 6 Useful Bytes From The Header */
	GLuint		bytesPerPixel;							/* Holds Number Of Bytes Per Pixel Used In The TGA File */
	GLuint		imageSize;								/* Used To Store The Image Size When Setting Aside Ram */
	GLuint		temp;									/* Temporary Variable */
	GLuint		type;
	GLuint		Height;									/* Height of Image */
	GLuint		Width;									/* Width ofImage */
	GLuint		Bpp;									/* Bits Per Pixel */
} TGA;

TGAHeader tgaheader;									/* TGA header */
TGA tga;												/* TGA image data */

GLubyte uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	/* Uncompressed TGA Header */
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	/* Compressed TGA Header */
BOOL LoadUncompressedTGA(Texture *,const char *,FILE *);	/* Load an Uncompressed file */
BOOL LoadCompressedTGA(Texture *,const char *,FILE *);		/* Load a Compressed file */

BOOL LoadTGA(Texture * texture,const char * filename)				/* Load a TGA file */
{
	FILE * fTGA;													/* File pointer to texture file */
	fTGA = fopen(filename, "rb");									/* Open file for reading */

	if(fTGA == NULL)												/* If it didn't open.... */
	{
		printf("Error could not open texture file");				/* Display an error message */
		return False;												/* Exit function */
	}

	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)			/* Attempt to read 12 byte header from file */
	{
		printf("Error could not read file header");					/* If it fails, display an error message */
		if(fTGA != NULL)												/* Check to seeiffile is still open */
		{
			fclose(fTGA);												/* If it is, close it */
		}
		return False;													/* Exit function */
	}

	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)	/* See if header matches the predefined header of */
	{																/* an Uncompressed TGA image */
		LoadUncompressedTGA(texture, filename, fTGA);					/* If so, jump to Uncompressed TGA loading code */
	}
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)	/* See if header matches the predefined header of */
	{																	/* an RLE compressed TGA image */
		LoadCompressedTGA(texture, filename, fTGA);						/* If so, jump to Compressed TGA loading code */
	}
	else																/* If header matches neither type */
	{
		printf("Error TGA file be type 2 or type 10 ");				/* Display an error */
		fclose(fTGA);
		return False;												/* Exit function */
	}
	return True;														/* All went well, continue on */
}

BOOL LoadUncompressedTGA(Texture * texture, const char * filename, FILE * fTGA)	/* Load an uncompressed TGA (note, much of this code is based on NeHe's */
{
	GLuint cswap;															/* TGA Loading code nehe.gamedev.net) */
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)				/* Read TGA header */
	{
		printf("Error could not read info header");							/* Display error */
		if(fTGA != NULL)													/* if file is still open */
		{
			fclose(fTGA);													/* Close it */
		}
		return False;														/* Return failular */
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];					/* Determine The TGA Width	(highbyte*256+lowbyte) */
	texture->height = tga.header[3] * 256 + tga.header[2];					/* Determine The TGA Height	(highbyte*256+lowbyte) */
	texture->bpp	= tga.header[4];										/* Determine the bits per pixel */
	tga.Width		= texture->width;										/* Copy width into local structure */
	tga.Height		= texture->height;										/* Copy height into local structure */
	tga.Bpp			= texture->bpp;											/* Copy BPP into local structure */

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	/* Make sure all information is valid */
	{
		printf("Error invalid texture information");						/* Display Error */
		if(fTGA != NULL)													/* Check if file is still open */
		{
			fclose(fTGA);													/* If so, close it */
		}
		return False;														/* Return failed */
	}

	if(texture->bpp == 24)													/* If the BPP of the image is 24... */
	{
		texture->type	= GL_RGB;											/* Set Image type to GL_RGB */
	}
	else																	/* Else if its 32 BPP */
	{
		texture->type	= GL_RGBA;											/* Set image type to GL_RGBA */
	}

	tga.bytesPerPixel	= (tga.Bpp / 8);									/* Compute the number of BYTES per pixel */
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		/* Compute the total amout ofmemory needed to store data */
	texture->imageData	= malloc(tga.imageSize);							/* Allocate that much memory */

	if(texture->imageData == NULL)											/* If no space was allocated */
	{
		printf("Error could not allocate memory for image");				/* Display Error */
		fclose(fTGA);														/* Close the file */
		return False;														/* Return failed */
	}

	if(fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)	/* Attempt to read image data */
	{
		printf("Error could not read image data");							/* Display Error */
		if(texture->imageData != NULL)										/* If imagedata has data in it */
		{
			free(texture->imageData);										/* Delete data from memory */
		}
		fclose(fTGA);														/* Close file */
		return False;														/* Return failed */
	}

	/* Byte Swapping Optimized By Steve Thomas */
	for(cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
		texture->imageData[cswap] ^= texture->imageData[cswap+2];
	}

	fclose(fTGA);															/* Close file */
	return True;															/* Return success */
}

BOOL LoadCompressedTGA(Texture * texture,const char * filename, FILE * fTGA)		/* Load COMPRESSED TGAs */
{
	GLuint pixelcount	= tga.Height * tga.Width;							/* Nuber of pixels in the image */
	GLuint currentpixel	= 0;												/* Current pixel being read */
	GLuint currentbyte	= 0;												/* Current byte */
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);			/* Storage for 1 pixel */

	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)				/* Attempt to read header */
	{
		printf("Error could not read info header");							/* Display Error */
		if(fTGA != NULL)													/* If file is open */
		{
			fclose(fTGA);													/* Close it */
		}
		return False;														/* Return failed */
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];					/* Determine The TGA Width	(highbyte*256+lowbyte) */
	texture->height = tga.header[3] * 256 + tga.header[2];					/* Determine The TGA Height	(highbyte*256+lowbyte) */
	texture->bpp	= tga.header[4];										/* Determine Bits Per Pixel */
	tga.Width		= texture->width;										/* Copy width to local structure */
	tga.Height		= texture->height;										/* Copy width to local structure */
	tga.Bpp			= texture->bpp;											/* Copy width to local structure */

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	/*Make sure all texture info is ok */
	{
		printf("Error Invalid texture information");						/* If it isnt...Display error */
		if(fTGA != NULL)													/* Check if file is open */
		{
			fclose(fTGA);													/* Ifit is, close it */
		}
		return False;														/* Return failed */
	}

	tga.bytesPerPixel	= (tga.Bpp / 8);									/* Compute BYTES per pixel */
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		/* Compute amout of memory needed to store image */
	texture->imageData	= (GLubyte *)malloc(tga.imageSize);					/* Allocate that much memory */

	if(texture->imageData == NULL)											/* If it wasnt allocated correctly.. */
	{
		printf("Error could not allocate memory for image");				/* Display Error */
		fclose(fTGA);														/* Close file */
		return False;														/* Return failed */
	}

	do
	{
		GLubyte chunkheader = 0;											/* Storage for "chunk" header */

		if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)				/* Read in the 1 byte header */
		{
			printf("Error could not read RLE header");						/*Display Error */
			if(fTGA != NULL)												/* If file is open */
			{
				fclose(fTGA);												/* Close file */
			}
			if(texture->imageData != NULL)									/* If there is stored image data */
			{
				free(texture->imageData);									/* Delete image data */
			}
			return False;													/* Return failed */
		}

		if(chunkheader < 128)												/* If the ehader is < 128, it means the that is the number of RAW color packets minus 1 */
		{
			short counter;													/* that follow the header */
			chunkheader++;													/* add 1 to get number of following color values */
			for(counter = 0; counter < chunkheader; counter++)				/* Read RAW color values */
			{
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) /* Try to read 1 pixel */
				{
					printf("Error could not read image data");				/* IF we cant, display an error */

					if(fTGA != NULL)										/* See if file is open */
					{
						fclose(fTGA);										/* If so, close file */
					}

					if(colorbuffer != NULL)									/* See if colorbuffer has data in it */
					{
						free(colorbuffer);									/* If so, delete it */
					}

					if(texture->imageData != NULL)										/* See if there is stored Image data */
					{
						free(texture->imageData);										/* If so, delete it too */
					}

					return False;														/* Return failed */
				}
																						/* write to memory */
				texture->imageData[currentbyte		] = colorbuffer[2];				    /* Flip R and B vcolor values around in the process */
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												/* if its a 32 bpp image */
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3];				/* copy the 4th byte */
				}

				currentbyte += tga.bytesPerPixel;										/* Increase thecurrent byte by the number of bytes per pixel */
				currentpixel++;															/* Increase current pixel by 1 */

				if(currentpixel > pixelcount)											/* Make sure we havent read too many pixels */
				{
					printf("Error too many pixels read");								/* if there is too many... Display an error! */

					if(fTGA != NULL)													/* If there is a file open */
					{
						fclose(fTGA);													/* Close file */
					}

					if(colorbuffer != NULL)												/* If there is data in colorbuffer */
					{
						free(colorbuffer);												/* Delete it */
					}

					if(texture->imageData != NULL)										/* If there is Image data */
					{
						free(texture->imageData);										/* delete it */
					}

					return False;														/* Return failed */
				}
			}
		}
		else																			/* chunkheader > 128 RLE data, next color  reapeated chunkheader - 127 times */
		{
			short counter;
			chunkheader -= 127;															/* Subteact 127 to get rid of the ID bit */
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		/* Attempt to read following color values */
			{
				printf("Error could not read from file");			/* If attempt fails.. Display error (again) */

				if(fTGA != NULL)														/* If thereis a file open */
				{
					fclose(fTGA);														/* Close it */
				}

				if(colorbuffer != NULL)													/* If there is data in the colorbuffer */
				{
					free(colorbuffer);													/* delete it */
				}

				if(texture->imageData != NULL)											/* If thereis image data */
				{
					free(texture->imageData);											/* delete it */
				}

				return False;															/* return failed */
			}

			for(counter = 0; counter < chunkheader; counter++)					/* copy the color into the image data as many times as dictated */
			{																			/* by the header */
				texture->imageData[currentbyte		] = colorbuffer[2];					/* switch R and B bytes areound while copying */
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												/* If TGA images is 32 bpp */
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3];				/* Copy 4th byte */
				}

				currentbyte += tga.bytesPerPixel;										/* Increase current byte by the number of bytes per pixel */
				currentpixel++;															/* Increase pixel count by 1 */

				if(currentpixel > pixelcount)											/* Make sure we havent written too many pixels */
				{
					printf("Error too many pixels read");								/* if there is too many... Display an error! */

					if(fTGA != NULL)													/* If there is a file open */
					{
						fclose(fTGA);													/* Close file */
					}

					if(colorbuffer != NULL)												/* If there is data in colorbuffer */
					{
						free(colorbuffer);												/* Delete it */
					}

					if(texture->imageData != NULL)										/* If there is Image data */
					{
						free(texture->imageData);										/* delete it */
					}

					return False;														/* Return failed */
				}
			}
		}
	}

	while(currentpixel < pixelcount);													/* Loop while there are still pixels left */
	fclose(fTGA);																		/* Close the file */
	return True;																		/* return success */
}

void customLoadTexture2DTGA(const char * imagepath) {
	Texture mytex;
	LoadTGA(&mytex, imagepath);
	glTexImage2D(GL_TEXTURE_2D, 0, mytex.type, mytex.width, mytex.height, 0, mytex.type, GL_UNSIGNED_BYTE, mytex.imageData);
	free(mytex.imageData);
}
#endif

GLuint loadTGA_glfw(const char * imagepath){

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Read the file, call glTexImage2D with the right parameters
#ifdef USE_GLES
	customLoadTexture2DTGA(imagepath);
#else
	glfwLoadTexture2D(imagepath, 0);
#endif
	// Nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}



#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath){

	unsigned char header[124];

	FILE *fp; 
 
	/* try to open the file */ 
	fp = fopen(imagepath, "rb"); 
	if (fp == NULL) 
		return 0; 
   
	/* verify the type of file */ 
	char filecode[4]; 
	fread(filecode, 1, 4, fp); 
	if (strncmp(filecode, "DDS ", 4) != 0) { 
		fclose(fp); 
		return 0; 
	}
	
	/* get the surface desc */ 
	fread(&header, 124, 1, fp); 

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */ 
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
	fread(buffer, 1, bufsize, fp); 
	/* close the file pointer */ 
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	unsigned int format;
	switch(fourCC) 
	{ 
	case FOURCC_DXT1: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
		break; 
	case FOURCC_DXT3: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
		break; 
	case FOURCC_DXT5: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
		break; 
	default: 
		free(buffer); 
		return 0; 
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	unsigned int offset = 0;

	/* load the mipmaps */ 
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
			0, size, buffer + offset); 
	 
		offset += size; 
		width  /= 2; 
		height /= 2; 
	} 

	free(buffer); 

	return textureID;


}
