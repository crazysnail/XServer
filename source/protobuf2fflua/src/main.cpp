// Ryan Wang @KOOGame
// protobuf���, �Զ�����һЩfflua��ע�����
#include "fflua_generator.h"
#include<google/protobuf/compiler/plugin.h>

int main(int argc, char* argv[]) {
	FFluaGenerator generator;
	return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}