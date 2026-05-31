#include <Utility/FileUtil.h>
#include <algorithm>
#include <cctype>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>

namespace Utility{
namespace{
	std::string lowerString(const std::string& s){
		std::string out = s;
		std::transform(out.begin(), out.end(), out.begin(),
			[](unsigned char c){ return static_cast<char>(std::tolower(c)); });
		return out;
	}

	bool pathExists(const std::string& path){
		struct stat st;
		return stat(path.c_str(), &st) == 0;
	}

	std::vector<std::string> splitPath(const std::string& path){
		std::vector<std::string> parts;
		std::string current;
		for(char ch : path){
			if(ch == '/' || ch == '\\'){
				if(!current.empty()){
					parts.push_back(current);
					current.clear();
				}
			}else{
				current.push_back(ch);
			}
		}
		if(!current.empty())
			parts.push_back(current);
		return parts;
	}


	std::string joinPath(const std::string& base, const std::string& part){
		if(base.empty())
			return part;
		if(base == ".")
			return std::string("./") + part;
		if(base == "/")
			return std::string("/") + part;
		if(base[base.size()-1] == '/')
			return base + part;
		return base + "/" + part;
	}

	bool findCaseInsensitiveEntry(const std::string& directory,
		const std::string& wanted, std::string& matched)
	{
		DIR* dir = opendir(directory.empty() ? "." : directory.c_str());
		if(!dir)
			return false;

		const std::string wantedLower = lowerString(wanted);
		bool found = false;
		while(dirent* entry = readdir(dir)){
			std::string name(entry->d_name);
			if(name == "." || name == "..")
				continue;
			if(lowerString(name) == wantedLower){
				matched = name;
				found = true;
				break;
			}
		}
		closedir(dir);
		return found;
	}

	bool resolveCaseInsensitivePath(const std::string& path, std::string& resolved){
		if(path.empty())
			return false;

		const bool absolute = path[0] == '/';
		std::string current = absolute ? "/" : ".";
		std::vector<std::string> parts = splitPath(path);
		for(const std::string& part : parts){
			std::string direct = joinPath(current, part);

			if(pathExists(direct)){
				current = direct;
				continue;
			}

			std::string matched;
			if(!findCaseInsensitiveEntry(current, part, matched))
				return false;

			current = joinPath(current, matched);
		}

		resolved = current;
		if(!absolute && resolved.compare(0, 2, "./") == 0)
			resolved.erase(0, 2);
		return true;
	}
}

	bool resolveExistingPath(const std::string& path, std::string& resolvedPath)
	{
		if(path.empty())
			return false;

		if(pathExists(path)){
			resolvedPath = path;
			return true;
		}

		return resolveCaseInsensitivePath(path, resolvedPath);
	}

	bool openInputFile(std::ifstream& input, const std::string& path,
		std::string* resolvedPath)
	{
		input.close();
		input.clear();

		std::string resolved;
		if(!resolveExistingPath(path, resolved))
			return false;

		input.open(resolved.c_str());
		if(!input)
			return false;

		if(resolvedPath)
			*resolvedPath = resolved;
		return true;
	}

}
