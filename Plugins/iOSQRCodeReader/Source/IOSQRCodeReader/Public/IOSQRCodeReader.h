//
//  IOSQRCodeReader.h
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 1/15/23.
//  Copyright © 2023 Matthew Zane. All rights reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FIOSQRCodeReaderModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
