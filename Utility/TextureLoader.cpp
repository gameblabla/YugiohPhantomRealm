
#include <Utility/TextureLoader.h>//contains GLCompat
#include <ThirdParty/lodepng.h>
#include <DefinesAndTypedefs.h>
#include <Utility/ErrorHandler.h>
#include <iostream>
#include <algorithm>

namespace {

	void setTexture2DParameters(unsigned int width, unsigned int height)
	{
#ifdef USE_GLES
		// GLES2 requires NPOT textures to use CLAMP_TO_EDGE and non-mipmapped
		// filtering unless full NPOT support is available. The safe path is used
		// here for all PNG assets because this loader never relied on repeating.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
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
		
		printf("Load texture PNG : %s\n", fileName);
		
		unsigned int error = lodepng::decode(image, iWidth, iHeight, fileName);
		if(error != 0 || image.empty()){
			errorHandler.printError("textureLoader: failed to load png: ");
			errorHandler.printError(fileName);
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
		/*auto it = find(currentIds.begin(), currentIds.end(), *textureID );
		if(it == currentIds.end()){
			std::cout<<"Texture Loader: couldn't find id in vector: "<<(*textureID)<<std::endl;
			std::cout<<"Texture loader size: "<<currentIds.size()<<std::endl;
		}else{
			std::cout<<"Texture Loader: deleted id: "<<*textureID<<std::endl;
			currentIds.erase(it);
		}*/
		//std::cout<<"texture Loader: delete ID: "<<*textureID<<std::endl;
		glDeleteTextures(1, textureID);
		glFlush(); glFinish();
		*textureID = YUG_UNBIND;

	}

}

