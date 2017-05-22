#include "stdafx.h"
#include "logfs.h"

std::ofstream logfs_s;

void logfs_init(char *file)
{
	logfs_s.open(file);
}

std::ofstream& logfs()
{
	return logfs_s;
}