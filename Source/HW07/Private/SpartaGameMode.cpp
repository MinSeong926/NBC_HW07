// SpartaGameMode.cpp

#include "SpartaGameMode.h"
#include "SpartaActor.h"

ASpartaGameMode::ASpartaGameMode() {
	DefaultPawnClass = ASpartaActor::StaticClass();
}