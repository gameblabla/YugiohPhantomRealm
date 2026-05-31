
#include <Utility/TextureLoader.h>//contains GLCompat
#include <ThirdParty/lodepng.h>
#include <DefinesAndTypedefs.h>
#include <Utility/ErrorHandler.h>
#include <Utility/FileUtil.h>
#include <iostream>
#include <algorithm>
#include <string>

namespace {

	bool isPowerOfTwo(unsigned int value)
	{
		return value != 0 && (value & (value - 1)) == 0;
	}

	void setTexture2DParameters(unsigned int width, unsigned int height)
	{
		// The original renderer left wrap mode at OpenGL's default GL_REPEAT.
		// Many of the OBJ assets rely on repeated/negative UVs after objloader's
		// V-coordinate flip; forcing CLAMP_TO_EDGE stretches a single edge row and
		// produces the vertical barcode-looking garbage seen on the menu/screens.
#ifdef USE_GLES
		const bool canUseSamplerRepeat = isPowerOfTwo(width) && isPowerOfTwo(height);
		const GLint wrapMode = canUseSamplerRepeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
#else
		const GLint wrapMode = GL_REPEAT;
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

}

namespace Utility{

	//universal object
	TextureLoader TextureLoader::textureLoaderInstance;

	bool TextureLoader::initialize()
	{
		badTexture = 0;
		loadTexture("GameData/textures/models/badTexture.png", &badTexture);
		return (badTexture != 0);
	}


	bool TextureLoader::shutdown()
	{
		return false;
	}

	void  TextureLoader::loadTexture(const char* fileName, GLuint* textureID){
#ifndef NOVA
		glBindVertexArray(YUG_UNBIND);
#endif
		if((*textureID) != YUG_NULL_ID )
			deleteTexture(textureID);
		//get image as png
		std::vector<unsigned char> image;
		unsigned int iWidth, iHeight;

		const std::string requestedPath = fileName ? fileName : "";
		printf("Load texture PNG : %s\n", requestedPath.c_str());

		std::string resolvedPath = requestedPath;
		if(Utility::resolveExistingPath(requestedPath, resolvedPath)){
			if(resolvedPath != requestedPath)
				std::cout<<"Texture Loader: resolved path: "<<resolvedPath<<std::endl;
		}

		unsigned int error = lodepng::decode(image, iWidth, iHeight, resolvedPath);
		if(error != 0 || image.empty()){
			errorHandler.printError("textureLoader: failed to load png: ");
			errorHandler.printError(requestedPath.c_str());
			if(resolvedPath != requestedPath){
				errorHandler.printError("textureLoader: resolved png path also failed: ");
				errorHandler.printError(resolvedPath.c_str());
			}
			errorHandler.printError("textureLoader: using bad texture fallback\n");
			*textureID = badTexture;
			return;
		}

		//send to openGL
		glGenTextures(1, textureID);

		auto it = find(currentIds.begin(), currentIds.end(), *textureID );
		if(it != currentIds.end()){
			std::cout<<"Texture Loader: dup ids: "<<(*it)<<std::endl;
		}else{
			std::cout<<"Texture Loader: genned id: "<<*textureID<<std::endl;
		}
		currentIds.push_back(*textureID);

		glBindTexture(GL_TEXTURE_2D, (*textureID));
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

		image.clear();

		setTexture2DParameters(iWidth, iHeight);
		glBindTexture(GL_TEXTURE_2D,0);
	}

	void TextureLoader::deleteTexture(GLuint* textureID){
		if(textureID == NULL) return;
		if( textureID == &badTexture ) return;
		if( *textureID == YUG_UNBIND || *textureID == YUG_NULL_ID ) return;
		if( *textureID == badTexture ){
			*textureID = YUG_UNBIND;
			return;
		}
#ifndef NOVA
		glBindVertexArray(YUG_UNBIND);
#endif
		auto it = find(currentIds.begin(), currentIds.end(), *textureID );
		if(it != currentIds.end()){
			currentIds.erase(it);
		}
		//std::cout<<"texture Loader: delete ID: "<<*textureID<<std::endl;
		glDeleteTextures(1, textureID);
		glFlush(); glFinish();
		*textureID = YUG_UNBIND;

	}

}

