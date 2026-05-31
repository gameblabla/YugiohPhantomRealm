#ifndef TEXTURE_HPP
#define TEXTURE_HPP

// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const char * imagepath);

// Load a .TGA file using the custom loader
GLuint loadTGA_custom(const char * imagepath);

// Load a .DDS file using the custom loader
GLuint loadDDS(const char * imagepath);


#endif