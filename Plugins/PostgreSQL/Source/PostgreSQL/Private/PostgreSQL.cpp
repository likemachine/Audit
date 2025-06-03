// Copyright 2018-2023, Athian Games. All Rights Reserved. 

#include "PostgreSQL.h"

#define LOCTEXT_NAMESPACE "FPostgreSQLModule"

void FPostgreSQLModule::StartupModule()
{
	
}

void FPostgreSQLModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPostgreSQLModule, PostgreSQL)
