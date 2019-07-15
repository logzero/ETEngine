#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <vector>

#include <EtCore/Helper/AtomicTypes.h>
#include <EtCore/FileSystem/FileUtil.h>


int main(int argc, char* argv[]) 
{
	// working dir
	if (argc > 0)
	{
		FileUtil::SetExecutablePath(argv[0]);
	}
	else
	{
		std::cerr << "main > Couldn't get extract working directory from arguments, exiting!" << std::endl;
		return 1;
	}

	int result = Catch::Session().run(argc, argv);

	return result;
}