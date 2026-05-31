#pragma once

#include <fstream>
#include <string>

namespace Utility{

	// Resolves an existing path using the path as written first, then retries each
	// path component case-insensitively. This keeps the old Windows-authored
	// asset paths working on case-sensitive filesystems without changing every
	// call site.
	bool resolveExistingPath(const std::string& path, std::string& resolvedPath);

	// Opens a file using the same case-insensitive resolver.
	bool openInputFile(std::ifstream& input, const std::string& path,
		std::string* resolvedPath = 0);

}
