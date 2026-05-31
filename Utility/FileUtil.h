#pragma once

#include <fstream>
#include <string>

namespace Utility{

	// Opens a file using the path as written, then retries by resolving each
	// path component case-insensitively. This keeps the old Windows-authored
	// asset paths working on case-sensitive filesystems without changing every
	// call site.
	bool openInputFile(std::ifstream& input, const std::string& path,
		std::string* resolvedPath = 0);

}
